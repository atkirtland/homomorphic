#ifndef PALLIER_H_
#define PALLIER_H_

typedef struct pallier_public_key {
  mpz_t n;
  mpz_t g;
} pallier_public_key;

typedef struct pallier_private_key {
  mpz_t lambda;
  mpz_t mu;
} pallier_private_key;

#endif
