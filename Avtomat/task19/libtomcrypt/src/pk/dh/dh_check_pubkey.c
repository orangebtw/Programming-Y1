/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#include "tomcrypt_private.h"

#ifdef LTC_MDH

/**
  Check DH public key (INTERNAL ONLY, not part of public API)
  @param key    The key you wish to test
  @return CRYPT_OK if successful
*/
int dh_check_pubkey(const dh_key *key)
{
   void *p_minus1;
   ltc_mp_digit digit;
   int i, digit_count, bits_set = 0, err;

   LTC_ARGCHK(key != NULL);

   if ((err = ltc_mp_init(&p_minus1)) != CRYPT_OK) {
      return err;
   }

   /* avoid: y <= 1 OR y >= p-1 */
   if ((err = ltc_mp_sub_d(key->prime, 1, p_minus1)) != CRYPT_OK) {
      goto error;
   }
   if (ltc_mp_cmp(key->y, p_minus1) != LTC_MP_LT || ltc_mp_cmp_d(key->y, 1) != LTC_MP_GT) {
      err = CRYPT_INVALID_ARG;
      goto error;
   }

   /* public key must have more than one bit set */
   digit_count = ltc_mp_get_digit_count(key->y);
   for (i = 0; i < digit_count && bits_set < 2; i++) {
      digit = ltc_mp_get_digit(key->y, i);
      while (digit > 0) {
         if (digit & 1) bits_set++;
         digit >>= 1;
      }
   }
   if (bits_set > 1) {
      err = CRYPT_OK;
   }
   else {
      err = CRYPT_INVALID_ARG;
   }

error:
   ltc_mp_clear(p_minus1);
   return err;
}

#endif /* LTC_MDH */
