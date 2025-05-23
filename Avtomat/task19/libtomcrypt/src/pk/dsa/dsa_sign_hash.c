/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
#include "tomcrypt_private.h"

/**
   @file dsa_sign_hash.c
   DSA implementation, sign a hash, Tom St Denis
*/

#ifdef LTC_MDSA

/**
  Sign a hash with DSA
  @param in       The hash to sign
  @param inlen    The length of the hash to sign
  @param r        The "r" integer of the signature (caller must initialize with ltc_mp_init() first)
  @param s        The "s" integer of the signature (caller must initialize with ltc_mp_init() first)
  @param prng     An active PRNG state
  @param wprng    The index of the PRNG desired
  @param key      A private DSA key
  @return CRYPT_OK if successful
*/
int dsa_sign_hash_raw(const unsigned char *in,  unsigned long inlen,
                                   void   *r,   void *s,
                               prng_state *prng, int wprng, const dsa_key *key)
{
   void         *k, *kinv, *tmp;
   unsigned char *buf;
   int            err, qbits;

   LTC_ARGCHK(in  != NULL);
   LTC_ARGCHK(r   != NULL);
   LTC_ARGCHK(s   != NULL);
   LTC_ARGCHK(key != NULL);

   if ((err = prng_is_valid(wprng)) != CRYPT_OK) {
      return err;
   }
   if (key->type != PK_PRIVATE) {
      return CRYPT_PK_NOT_PRIVATE;
   }

   /* check group order size  */
   if (key->qord >= LTC_MDSA_MAX_GROUP) {
      return CRYPT_INVALID_ARG;
   }

   buf = XMALLOC(LTC_MDSA_MAX_GROUP);
   if (buf == NULL) {
      return CRYPT_MEM;
   }

   /* Init our temps */
   if ((err = ltc_mp_init_multi(&k, &kinv, &tmp, LTC_NULL)) != CRYPT_OK)                   { goto ERRBUF; }

   qbits = ltc_mp_count_bits(key->q);
retry:

   do {
      /* gen random k */
      if ((err = rand_bn_bits(k, qbits, prng, wprng)) != CRYPT_OK)                         { goto error; }

      /* k should be from range: 1 <= k <= q-1 (see FIPS 186-4 B.2.2) */
      if (ltc_mp_cmp_d(k, 0) != LTC_MP_GT || ltc_mp_cmp(k, key->q) != LTC_MP_LT)           { goto retry; }

      /* test gcd */
      if ((err = ltc_mp_gcd(k, key->q, tmp)) != CRYPT_OK)                                  { goto error; }
   } while (ltc_mp_cmp_d(tmp, 1) != LTC_MP_EQ);

   /* now find 1/k mod q */
   if ((err = ltc_mp_invmod(k, key->q, kinv)) != CRYPT_OK)                                 { goto error; }

   /* now find r = g^k mod p mod q */
   if ((err = ltc_mp_exptmod(key->g, k, key->p, r)) != CRYPT_OK)                           { goto error; }
   if ((err = ltc_mp_mod(r, key->q, r)) != CRYPT_OK)                                       { goto error; }

   if (ltc_mp_iszero(r) == LTC_MP_YES)                                                     { goto retry; }

   /* FIPS 186-4 4.6: use leftmost min(bitlen(q), bitlen(hash)) bits of 'hash'*/
   inlen = MIN(inlen, (unsigned long)(key->qord));

   /* now find s = (in + xr)/k mod q */
   if ((err = ltc_mp_read_unsigned_bin(tmp, in, inlen)) != CRYPT_OK)                       { goto error; }
   if ((err = ltc_mp_mul(key->x, r, s)) != CRYPT_OK)                                       { goto error; }
   if ((err = ltc_mp_add(s, tmp, s)) != CRYPT_OK)                                          { goto error; }
   if ((err = ltc_mp_mulmod(s, kinv, key->q, s)) != CRYPT_OK)                              { goto error; }

   if (ltc_mp_iszero(s) == LTC_MP_YES)                                                     { goto retry; }

   err = CRYPT_OK;
error:
   ltc_mp_deinit_multi(k, kinv, tmp, LTC_NULL);
ERRBUF:
#ifdef LTC_CLEAN_STACK
   zeromem(buf, LTC_MDSA_MAX_GROUP);
#endif
   XFREE(buf);
   return err;
}

/**
  Sign a hash with DSA
  @param in       The hash to sign
  @param inlen    The length of the hash to sign
  @param out      [out] Where to store the signature
  @param outlen   [in/out] The max size and resulting size of the signature
  @param prng     An active PRNG state
  @param wprng    The index of the PRNG desired
  @param key      A private DSA key
  @return CRYPT_OK if successful
*/
int dsa_sign_hash(const unsigned char *in,  unsigned long inlen,
                        unsigned char *out, unsigned long *outlen,
                        prng_state *prng, int wprng, const dsa_key *key)
{
   void         *r, *s;
   int           err;

   LTC_ARGCHK(in      != NULL);
   LTC_ARGCHK(out     != NULL);
   LTC_ARGCHK(outlen  != NULL);
   LTC_ARGCHK(key     != NULL);

   if (ltc_mp_init_multi(&r, &s, LTC_NULL) != CRYPT_OK) {
      return CRYPT_MEM;
   }

   if ((err = dsa_sign_hash_raw(in, inlen, r, s, prng, wprng, key)) != CRYPT_OK) {
      goto error;
   }

   err = der_encode_sequence_multi(out, outlen,
                             LTC_ASN1_INTEGER, 1UL, r,
                             LTC_ASN1_INTEGER, 1UL, s,
                             LTC_ASN1_EOL,     0UL, NULL);

error:
   ltc_mp_deinit_multi(r, s, LTC_NULL);
   return err;
}

#endif
