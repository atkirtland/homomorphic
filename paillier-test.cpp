#include <gmp.h>
#include <iostream>
#include "paillier.h"

#define DEBUG

int main() {
  std::cout << "Generating keypair... 512 bits\n";
  keypair *kp = new keypair();
  keygen(kp, 512);
#ifdef DEBUG
  gmp_printf(
      "kp.pub.n==%Zd\nkp.pub.n2==%Zd\nkp.pub.g==%Zd\nkp.priv.lambda==%Zd\nkp."
      "priv.mu==%Zd\n",
      kp->pub->n, kp->pub->n2, kp->pub->g, kp->priv->lambda, kp->priv->mu);
#endif
  mpz_t x;
  mpz_init_set_ui(x, 3);
  gmp_printf("x==%Zd\n", x);
  mpz_t cx;
  mpz_init(cx);
  encrypt(cx, x, kp->pub);
  gmp_printf("cx==%Zd\n", cx);

  mpz_t y;
  mpz_init_set_ui(y, 5);
  gmp_printf("y==%Zd\n", y);
  mpz_t cy;
  mpz_init(cy);
  encrypt(cy, y, kp->pub);
  gmp_printf("cy==%Zd\n", cy);

  mpz_t cz;
  mpz_init(cz);
  add(cz, cx, cy, kp->pub);
  gmp_printf("cz==%Zd\n", cz);
  mpz_t z;
  mpz_init(z);
  decrypt(z, cz, kp->priv, kp->pub);
  gmp_printf("z==%Zd\n", z);
  // std::cout << "cz==" + z + "\n";
  // int w = decrypt(kp.priv, kp.pub, mult_const(pub, add_const(pub, cz, 2),
  // 3));
  // std::cout << "(z+2)*3==" + w + "\n";
}
