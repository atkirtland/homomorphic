#include <gmp.h>
#include <linux/random.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "paillier.h"
#include "util.h"

#define DEBUG

namespace paillier {

void encrypt(mpz_t c, mpz_t m, paillier::public_key *pub) {
  gmp_randstate_t state;
  gmp_randinit_default(state);
  unsigned long int seed;
  util::randomul(&seed);
  gmp_randseed_ui(state, seed);

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

  mpz_t x;
  mpz_init(x);
  mpz_powm(x, r, pub->n, pub->n2);
  mpz_powm(c, pub->g, m, pub->n2);
  mpz_mul(c, c, x);
  mpz_mod(c, c, pub->n2);
}

void keygen(paillier::keypair *kp, unsigned long int bits) {
  mpz_t gcd;
  mpz_init(gcd);
  mpz_t p;
  mpz_init(p);
  mpz_t q;
  mpz_init(q);
  util::randomprime(p, bits / 2);
  util::randomprime(q, bits / 2);
#ifdef DEBUG
  gmp_printf("p: %Zd\n\n", p);
  gmp_printf("q: %Zd\n\n", q);
#endif

  mpz_mul(kp->pub->n, p, q);
  mpz_pow_ui(kp->pub->n2, kp->pub->n, static_cast<unsigned long int>(2));
  mpz_add_ui(kp->pub->g, kp->pub->n, static_cast<unsigned long int>(1));

  // https://github.com/mnassar/paillier-gmpy2/blob/master/paillier_gmpy2.py
  mpz_sub_ui(p, p, 1);
  mpz_sub_ui(q, q, 1);
  mpz_mul(kp->priv->lambda, p, q);
  mpz_invert(kp->priv->mu, kp->priv->lambda, kp->pub->n);
  // TODO change to log 2
  kp->pub->bits = bits;
}

void add(mpz_t z, mpz_t a, mpz_t b, paillier::public_key *pub) {
  mpz_mul(z, a, b);
  mpz_mod(z, z, pub->n2);
}

void add_const(mpz_t z, mpz_t a, unsigned long int c,
               paillier::public_key *pub) {
  mpz_powm_ui(z, pub->g, c, pub->n2);
  mpz_mul(z, a, z);
  mpz_mod(z, z, pub->n2);
}

void mul_const(mpz_t z, mpz_t a, unsigned long int c,
               paillier::public_key *pub) {
  mpz_powm_ui(z, a, c, pub->n2);
}

void decrypt(mpz_t m, mpz_t c, paillier::private_key *priv,
             paillier::public_key *pub) {
  mpz_powm(m, c, priv->lambda, pub->n2);
  mpz_sub_ui(m, m, static_cast<unsigned long int>(1));
  mpz_fdiv_q(m, m, pub->n);  // round to -inf (floor)
  mpz_mul(m, m, priv->mu);
  mpz_mod(m, m, pub->n);
}
}
