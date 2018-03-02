#include <gmp.h>
#include <iostream>
#include "paillier.h"

#define DEBUG

int main() {
  std::cout << "Generating keypair... 512 bits\n\n";
  paillier::keypair *kp = new paillier::keypair();
  paillier::keygen(kp, 512);
#ifdef DEBUG
  gmp_printf(
      "kp.pub.n==%Zd\n\nkp.pub.n2==%Zd\n\nkp.pub.g==%Zd\n\nkp.priv.lambda==%"
      "Zd\n\nkp."
      "priv.mu==%Zd\n\n",
      kp->pub->n, kp->pub->n2, kp->pub->g, kp->priv->lambda, kp->priv->mu);
#endif
  mpz_t x;
  mpz_init_set_ui(x, 3);
  gmp_printf("x==%Zd\n\n", x);
  mpz_t cx;
  mpz_init(cx);
  paillier::encrypt(cx, x, kp->pub);
  gmp_printf("cx==%Zd\n\n", cx);

  mpz_t y;
  mpz_init_set_ui(y, 5);
  gmp_printf("y==%Zd\n\n", y);
  mpz_t cy;
  mpz_init(cy);
  paillier::encrypt(cy, y, kp->pub);
  gmp_printf("cy==%Zd\n\n", cy);

  mpz_t cz;
  mpz_init(cz);
  paillier::add(cz, cx, cy, kp->pub);
  gmp_printf("cz==%Zd\n\n", cz);
  mpz_t z;
  mpz_init(z);
  paillier::decrypt(z, cz, kp->priv, kp->pub);
  gmp_printf("z==%Zd\n\n", z);

  mpz_t cw;
  mpz_init(cw);
  add_const(cw, cz, 2, kp->pub);
  mul_const(cw, cw, 3, kp->pub);
  mpz_t w;
  mpz_init(w);
  paillier::decrypt(w, cw, kp->priv, kp->pub);
  gmp_printf("w==%Zd\n\n", w);
}
