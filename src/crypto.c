/**
  crypto.c

  Copyright (C) 2015 clowwindy

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <sodium.h>
#include <string.h>

// will not copy key any more
static unsigned char key[32];

int crypto_init() {
  // set implementation here if you want to use custom random generator
  // while it is not recommended
  // https://download.libsodium.org/doc/advanced/custom_rng.html
  // drawbacks:
  //   1. not thread-safe
  //   2. just a boilerplate for writing implementations for embedded operating systems
  //   3. should rekey after fork() call
  if (-1 == sodium_init())
    return 1;
  return 0;
}

int crypto_set_password(const char *password,
                        unsigned long long password_len) {
  return crypto_generichash(key, sizeof key, (unsigned char *)password,
                            password_len, NULL, 0);
}

int crypto_encrypt(unsigned char *c, unsigned char *m,
                   unsigned long long mlen) {
  unsigned char nonce[8];
  randombytes_buf(nonce, 8);
  int r = crypto_secretbox_xsalsa20poly1305(c, m, mlen + 32, nonce, key);
  if (r != 0) return r;
  // copy nonce to the head
  memcpy(c + 8, nonce, 8);
  return 0;
}

int crypto_decrypt(unsigned char *m, unsigned char *c,
                   unsigned long long clen) {
  unsigned char nonce[8];
  memcpy(nonce, c + 8, 8);
  int r = crypto_secretbox_xsalsa20poly1305_open(m, c, clen + 32, nonce, key);
  if (r != 0) return r;
  return 0;
}

