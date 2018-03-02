#ifndef PALLIER_H_
#define PALLIER_H_

namespace paillier {
class public_key {
 public:
  public_key(void) {
    mpz_init(n);
    mpz_init(n2);
    mpz_init(g);
  }
  mpz_t n;
  mpz_t n2;
  mpz_t g;
  unsigned long int bits;
};

class private_key {
 public:
  private_key(void) {
    mpz_init(lambda);
    mpz_init(mu);
  }
  mpz_t lambda;
  mpz_t mu;
};

class keypair {
 public:
  keypair(void) {
    pub = new public_key();
    priv = new private_key();
  }
  public_key *pub;
  private_key *priv;
};

void encrypt(mpz_t c, mpz_t m, paillier::public_key *pub);
void keygen(paillier::keypair *kp, unsigned long int bits);
void decrypt(mpz_t m, mpz_t c, paillier::private_key *priv,
             paillier::public_key *pub);
void mul_const(mpz_t z, mpz_t a, unsigned long int c,
               paillier::public_key *pub);
void add_const(mpz_t z, mpz_t a, unsigned long int c,
               paillier::public_key *pub);
void add(mpz_t z, mpz_t a, mpz_t b, paillier::public_key *pub);
}

#endif
