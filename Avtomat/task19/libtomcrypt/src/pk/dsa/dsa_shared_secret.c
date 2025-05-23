/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
#include "tomcrypt_private.h"

/**
  @file dsa_shared_secret.c
  DSA Crypto, Tom St Denis
*/

#ifdef LTC_MDSA

/**
  Create a DSA shared secret between two keys
  @param private_key      The private DSA key (the exponent)
  @param base             The base of the exponentiation (allows this to be used for both encrypt and decrypt)
  @param public_key       The public key
  @param out              [out] Destination of the shared secret
  @param outlen           [in/out] The max size and resulting size of the shared secret
  @return CRYPT_OK if successful
*/
int dsa_shared_secret(void          *private_key, void *base,
                      const dsa_key *public_key,
                      unsigned char *out,         unsigned long *outlen)
{
   unsigned long  x;
   void          *res;
   int            err;

   LTC_ARGCHK(private_key != NULL);
   LTC_ARGCHK(public_key  != NULL);
   LTC_ARGCHK(out         != NULL);
   LTC_ARGCHK(outlen      != NULL);

   /* make new point */
   if ((err = ltc_mp_init(&res)) != CRYPT_OK) {
      return err;
   }

   if ((err = ltc_mp_exptmod(base, private_key, public_key->p, res)) != CRYPT_OK) {
      ltc_mp_clear(res);
      return err;
   }

   x = (unsigned long)ltc_mp_unsigned_bin_size(res);
   if (*outlen < x) {
      *outlen = x;
      err = CRYPT_BUFFER_OVERFLOW;
      goto done;
   }
   zeromem(out, x);
   if ((err = ltc_mp_to_unsigned_bin(res, out + (x - ltc_mp_unsigned_bin_size(res))))   != CRYPT_OK)          { goto done; }

   err     = CRYPT_OK;
   *outlen = x;
done:
   ltc_mp_clear(res);
   return err;
}

#endif
