#include <gmp.h>
#include <linux/random.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "util.h"

namespace util {

void randomul(unsigned long int *ul) {
  syscall(SYS_getrandom, ul, sizeof(unsigned long int), 0);
}

void randomprime(mpz_t rop, unsigned long int bits) {
  gmp_randstate_t state;
  // should this use gmp_randinit_mt?
  gmp_randinit_default(state);
  unsigned long int seed;
  randomul(&seed);
  gmp_randseed_ui(state, seed);
  mpz_t temp;
  mpz_init(temp);

  while (true) {
    mpz_ui_pow_ui(rop, 2, bits - 1);
    mpz_urandomb(temp, state, bits - 1);
    mpz_add(rop, rop, temp);
    // 32 reps
    if (mpz_probab_prime_p(rop, 32) >= 1) {
      break;
    }
  }
}
}
