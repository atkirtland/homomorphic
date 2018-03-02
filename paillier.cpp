#include <gmp.h>
#include <stdio.h>

#include <linux/random.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <iostream>

#include "paillier.h"

void randomul(unsigned long int *ul) {
  syscall(SYS_getrandom, ul, sizeof(unsigned long int), 0);
}

int randomprime(mpz_t rop, unsigned long int bits) {
  gmp_randstate_t state;
  gmp_randinit_default(state);  // should this use gmp_randinit_mt?
  unsigned long int seed;
  randomul(&seed);
  gmp_randseed_ui(state, seed);
  mpz_urandomb(rop, state, bits);
  mpz_setbit(rop, 0);  // make odd
#ifdef DEBUG
  gmp_printf("%Zd\n", rop);
#endif
  int isProbPrime = 0;
  int cnt = 0;
  while (isProbPrime == 0) {
    mpz_add_ui(rop, rop, (unsigned long int)2);
    isProbPrime = mpz_probab_prime_p(rop, 64);
#ifdef DEBUG
    gmp_printf("%d\t%Zd\n", isProbPrime, rop);
#endif
    ++cnt;
  }
#ifdef DEBUG
  gmp_printf("%d\t%d\t%Zd\n", isProbPrime, cnt, rop);
#endif
  return 0;
}

// int encrypt(mpz_t c, mpz_t m, paillier_public_key *pub) {
//   if (m < pub->n || m >= pub->n) {
//     return 1;
//   }
//   std::cout << "helasldkfj" << std::endl;
//   gmp_printf("c==%Zd\n", c);
//
//   // generate random num 0 <= r <= n
//   mpz_t r;
//   mpz_init(r);
//   gmp_printf("c==%Zd\n", c);
//
//   mpz_t first;
//   mpz_init(first);
//   mpz_powm_sec(first, pub->g, m, pub->n2);
//   mpz_t second;
//   mpz_init(second);
//   mpz_powm_sec(first, r, pub->n, pub->n2);
//   mpz_mul(c, first, second);
//   mpz_mod(c, c, pub->n2);
//   gmp_printf("c==%Zd\n", c);
//   mpz_set_ui(c, 2);
//   return 0;
// }

int encrypt(mpz_t c, mpz_t m, paillier_public_key *pub) {
  gmp_randstate_t state;
  gmp_randinit_default(state);
  unsigned long int seed;
  randomul(&seed);
  gmp_randseed_ui(state, seed);
  // mpz_urandomm(r, state, pub->n + 1);  // urandomm is [0, n)
  mpz_t r;
  mpz_init(r);
  mpz_t gcd;
  mpz_init(gcd);

  // Use this for smallish pub->bits
  // while (true) {
  //   unsigned long r1 = gmp_urandomb_ui(state, pub->bits);
  //   mpz_set_ui(r, r1);
  //   mpz_gcd(gcd, r, pub->n);
  //   if (r > 0 && r < pub->n && mpz_cmp_ui(gcd, 1) == 0) {
  //     break;
  //   }
  // }

  // Use this form for sufficiently large pub->bits
  unsigned long r1 = gmp_urandomb_ui(state, pub->bits);
  mpz_set_ui(r, r1);

  gmp_printf("c==%Zd\n", c);
  mpz_t x;
  mpz_init(x);
  mpz_powm(x, r, pub->n, pub->n2);
  mpz_powm(c, pub->g, m, pub->n2);
  mpz_mul(c, c, x);
  mpz_mod(c, c, pub->n2);
  return 0;
}

int keygen(keypair *kp, unsigned long int bits) {
  mpz_t gcd;
  mpz_init(gcd);
  mpz_t p;
  mpz_init(p);
  mpz_t q;
  mpz_init(q);
  while (mpz_cmp_ui(gcd, static_cast<unsigned long int>(1))) {
    randomprime(p, bits / 2);
    randomprime(q, bits / 2);
    mpz_gcd(gcd, p, q);
  }

  mpz_mul(kp->pub->n, p, q);
  mpz_lcm(kp->priv->lambda, p - 1, q - 1);
  mpz_pow_ui(kp->pub->n2, kp->pub->n, static_cast<unsigned long int>(2));

  // https://github.com/mnassar/paillier-gmpy2/blob/master/paillier_gmpy2.py
  mpz_add_ui(kp->pub->g, kp->pub->n, static_cast<unsigned long int>(1));
  mpz_invert(kp->priv->mu, kp->priv->lambda, kp->pub->n);
  kp->pub->bits = bits;
  return 0;
}

void add(mpz_t z, mpz_t a, mpz_t b, paillier_public_key *pub) {
  mpz_mul(z, a, b);
  mpz_mod(z, z, pub->n2);
}

void add_const(mpz_t z, mpz_t a, mpz_t c, paillier_public_key *pub) {
  mpz_powm(z, pub->g, c, pub->n2);
  mpz_mul(z, a, z);
  mpz_mod(z, z, pub->n2);
}

void mul_const(mpz_t z, mpz_t a, mpz_t c, paillier_public_key *pub) {
  mpz_powm(z, a, c, pub->n2);
}

void decrypt(mpz_t m, mpz_t c, paillier_private_key *priv,
             paillier_public_key *pub) {
  mpz_powm(m, c, priv->lambda, pub->n2);
  mpz_sub_ui(m, m, static_cast<unsigned long int>(1));
  mpz_fdiv_q(m, m, pub->n);  // round to -inf (floor)
  mpz_mul(m, m, priv->mu);
  mpz_mod(m, m, pub->n);
}
