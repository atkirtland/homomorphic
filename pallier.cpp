#include <gmp.h>
#include <stdio.h>

#include <linux/random.h>
#include <sys/syscall.h>
#include <unistd.h>

unsigned long int randomul(unsigned long int *ul) {
  syscall(SYS_getrandom, ul, sizeof(unsigned long int), 0);
}

mpz_t randomprime() {
  mpz_t rop;  // return operand
  mpz_init(rop);
  gmp_randstate_t state;
  gmp_randinit_default(state);  // should this use gmp_randinit_mt?
  unsigned long int seed;
  randomul(&seed);
  gmp_randseed_ui(state, seed);
  mpz_urandomb(rop, state, 2048);
  mpz_setbit(rop, 0);  // make odd
  gmp_printf("%Zd\n", rop);
  int isProbPrime = 0;
  int cnt = 0;
  while (isProbPrime == 0) {
    mpz_add_ui(rop, rop, (unsigned long int)2);
    isProbPrime = mpz_probab_prime_p(rop, 64);
    gmp_printf("%d\t%Zd\n", isProbPrime, rop);
    ++cnt;
  }
  gmp_printf("%d\t%d\t%Zd\n", isProbPrime, cnt, rop);
}

int encrypt(mpz_t c, mpz_t m, pallier_public_key key) {
  if (m < n || m >= n) {
    return 1;
  }

  // generate random num 0 <= r <= n
  mpz_t r;
  mpz_init(r);
  gmp_randstate_t state;
  gmp_randinit_default(state);
  unsigned long int seed;
  randomul(&seed);
  gmp_randseed_ui(state, seed);
  gmp_urandomm(r, state, n + 1);  // urandomm is [0, n)

  mpz_t first;
  mpz_init(first);
  mpz_powm_sec(first, key.g, m, key.n * key.n);
  mpz_t second;
  mpz_init(second);
  mpz_powm_sec(first, r, key.n, key.n * key.n);
  mpz_mul(c, first, second);
  mpz_mod(c, c, key.n * key.n);
  return 0;
}

int keygen(void) {
  mpz_t gcd;
  mpz_init(gcd);
  while (gcd != 1) {
    mpz_t p = randomprime();
    mpz_t q = randomprime();
    mpz_gcd(gcd, p, q);
  }
  mpz_t n;
  mpz_init(n);
  mpz_mul(n, p, q);
  mpz_t lambda;
  mpz_init(lambda);
  mpz_lcm(lambda, p - 1, q - 1);
}
