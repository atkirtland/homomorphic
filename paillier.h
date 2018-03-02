#ifndef PALLIER_H_
#define PALLIER_H_

class paillier_public_key {
 public:
  paillier_public_key(void) {
    mpz_init(n);
    mpz_init(n2);
    mpz_init(g);
  }
  mpz_t n;
  mpz_t n2;
  mpz_t g;
  unsigned long int bits;
};

class paillier_private_key {
 public:
  paillier_private_key(void) {
    mpz_init(lambda);
    mpz_init(mu);
  }
  mpz_t lambda;
  mpz_t mu;
};

class keypair {
 public:
  keypair(void) {
    pub = new paillier_public_key();
    priv = new paillier_private_key();
  }
  paillier_public_key *pub;
  paillier_private_key *priv;
};

int encrypt(mpz_t c, mpz_t m, paillier_public_key *pub);
int keygen(keypair *kp, unsigned long int bits);
void decrypt(mpz_t m, mpz_t c, paillier_private_key *priv,
             paillier_public_key *pub);
void mul_const(mpz_t z, mpz_t a, mpz_t c, paillier_public_key *pub);
void add_const(mpz_t z, mpz_t a, mpz_t c, paillier_public_key *pub);
void add(mpz_t z, mpz_t a, mpz_t b, paillier_public_key *pub);

#endif
