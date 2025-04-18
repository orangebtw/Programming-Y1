/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */
#include "tomcrypt_private.h"

static void hash_gen(void)
{
   unsigned char md[MAXBLOCKSIZE], *buf;
   unsigned long outlen, x, y, z;
   FILE *out;
   int   err;

   out = fopen("hash_tv.txt", "w");
   if (out == NULL) {
      perror("can't open hash_tv.txt");
      return;
   }

   fprintf(out, "Hash Test Vectors:\n\nThese are the hashes of nn bytes '00 01 02 03 .. (nn-1)'\n\n");
   for (x = 0; hash_descriptor[x].name != NULL; x++) {
      buf = XMALLOC(2 * hash_descriptor[x].blocksize + 1);
      if (buf == NULL) {
         perror("can't alloc mem");
         exit(EXIT_FAILURE);
      }
      fprintf(out, "Hash: %s\n", hash_descriptor[x].name);
      for (y = 0; y <= (hash_descriptor[x].blocksize * 2); y++) {
         for (z = 0; z < y; z++) {
            buf[z] = (unsigned char)(z & 255);
         }
         outlen = sizeof(md);
         if ((err = hash_memory(x, buf, y, md, &outlen)) != CRYPT_OK) {
            printf("hash_memory error: %s\n", error_to_string(err));
            exit(EXIT_FAILURE);
         }
         fprintf(out, "%3lu: ", y);
         for (z = 0; z < outlen; z++) {
            fprintf(out, "%02X", md[z]);
         }
         fprintf(out, "\n");
      }
      fprintf(out, "\n");
      XFREE(buf);
   }
   fclose(out);
}

static void cipher_gen(void)
{
   unsigned char *key, pt[MAXBLOCKSIZE];
   unsigned long x, y, z, w;
   int err, kl, lastkl;
   FILE *out;
   symmetric_key skey;

   out = fopen("cipher_tv.txt", "w");

   fprintf(out,
"Cipher Test Vectors\n\nThese are test encryptions with key of nn bytes '00 01 02 03 .. (nn-1)' and original PT of the same style.\n"
"The output of step N is used as the key and plaintext for step N+1 (key bytes repeated as required to fill the key)\n\n");

   for (x = 0; cipher_descriptor[x].name != NULL; x++) {
      fprintf(out, "Cipher: %s\n", cipher_descriptor[x].name);

      /* three modes, smallest, medium, large keys */
      lastkl = 10000;
      for (y = 0; y < 3; y++) {
         switch (y) {
            case 0: kl = cipher_descriptor[x].min_key_length; break;
            case 1: kl = (cipher_descriptor[x].min_key_length + cipher_descriptor[x].max_key_length)/2; break;
            case 2: kl = cipher_descriptor[x].max_key_length; break;
         }
         if ((err = cipher_descriptor[x].keysize(&kl)) != CRYPT_OK) {
            printf("keysize error: %s\n", error_to_string(err));
            exit(EXIT_FAILURE);
         }
         if (kl == lastkl) continue;
         lastkl = kl;
         fprintf(out, "Key Size: %d bytes\n", kl);

         key = XMALLOC(kl);
         if (key == NULL) {
            perror("can't malloc memory");
            exit(EXIT_FAILURE);
         }

         for (z = 0; (int)z < kl; z++) {
             key[z] = (unsigned char)z;
         }
         if ((err = cipher_descriptor[x].setup(key, kl, 0, &skey)) != CRYPT_OK) {
            printf("setup error: %s\n", error_to_string(err));
            exit(EXIT_FAILURE);
         }

         for (z = 0; (int)z < cipher_descriptor[x].block_length; z++) {
            pt[z] = (unsigned char)z;
         }
         for (w = 0; w < 50; w++) {
             cipher_descriptor[x].ecb_encrypt(pt, pt, &skey);
             fprintf(out, "%2lu: ", w);
             for (z = 0; (int)z < cipher_descriptor[x].block_length; z++) {
                fprintf(out, "%02X", pt[z]);
             }
             fprintf(out, "\n");

             /* reschedule a new key */
             for (z = 0; z < (unsigned long)kl; z++) {
                 key[z] = pt[z % cipher_descriptor[x].block_length];
             }
             if ((err = cipher_descriptor[x].setup(key, kl, 0, &skey)) != CRYPT_OK) {
                printf("cipher setup2 error: %s\n", error_to_string(err));
                exit(EXIT_FAILURE);
             }
         }
         fprintf(out, "\n");
         XFREE(key);
     }
     fprintf(out, "\n");
  }
  fclose(out);
}

static void hmac_gen(void)
{
   unsigned char key[MAXBLOCKSIZE], output[MAXBLOCKSIZE], *input;
   int x, y, z, err;
   FILE *out;
   unsigned long len;

   out = fopen("hmac_tv.txt", "w");

   fprintf(out,
"HMAC Tests.  In these tests messages of N bytes long (00,01,02,...,NN-1) are HMACed.  The initial key is\n"
"of the same format (the same length as the HASH output size).  The HMAC key in step N+1 is the HMAC output of\n"
"step N.\n\n");

   for (x = 0; hash_descriptor[x].name != NULL; x++) {
      fprintf(out, "HMAC-%s\n", hash_descriptor[x].name);

      /* initial key */
      for (y = 0; y < (int)hash_descriptor[x].hashsize; y++) {
          key[y] = (y&255);
      }

      input = XMALLOC(hash_descriptor[x].blocksize * 2 + 1);
      if (input == NULL) {
         perror("Can't malloc memory");
         exit(EXIT_FAILURE);
      }

      for (y = 0; y <= (int)(hash_descriptor[x].blocksize * 2); y++) {
         for (z = 0; z < y; z++) {
            input[z] = (unsigned char)(z & 255);
         }
         len = sizeof(output);
         if ((err = hmac_memory(x, key, hash_descriptor[x].hashsize, input, y, output, &len)) != CRYPT_OK) {
            printf("Error hmacing: %s\n", error_to_string(err));
            exit(EXIT_FAILURE);
         }
         fprintf(out, "%3d: ", y);
         for (z = 0; z <(int) len; z++) {
            fprintf(out, "%02X", output[z]);
         }
         fprintf(out, "\n");

         /* forward the key */
         memcpy(key, output, hash_descriptor[x].hashsize);
      }
      XFREE(input);
      fprintf(out, "\n");
   }
   fclose(out);
}

#ifdef LTC_OMAC
static void omac_gen(void)
{
   unsigned char key[MAXBLOCKSIZE], output[MAXBLOCKSIZE], input[MAXBLOCKSIZE*2+2];
   int err, x, y, z, kl;
   FILE *out;
   unsigned long len;

   out = fopen("omac_tv.txt", "w");

   fprintf(out,
"OMAC Tests.  In these tests messages of N bytes long (00,01,02,...,NN-1) are OMAC'ed.  The initial key is\n"
"of the same format (length specified per cipher).  The OMAC key in step N+1 is the OMAC output of\n"
"step N (repeated as required to fill the array).\n\n");

   for (x = 0; cipher_descriptor[x].name != NULL; x++) {
      kl = cipher_descriptor[x].block_length;

      /* skip ciphers which do not have 64 or 128 bit block sizes */
      if (kl != 8 && kl != 16) continue;

      if (cipher_descriptor[x].keysize(&kl) != CRYPT_OK) {
         kl = cipher_descriptor[x].max_key_length;
      }
      fprintf(out, "OMAC-%s (%d byte key)\n", cipher_descriptor[x].name, kl);

      /* initial key/block */
      for (y = 0; y < kl; y++) {
          key[y] = (y & 255);
      }

      for (y = 0; y <= (int)(cipher_descriptor[x].block_length*2); y++) {
         for (z = 0; z < y; z++) {
            input[z] = (unsigned char)(z & 255);
         }
         len = sizeof(output);
         if ((err = omac_memory(x, key, kl, input, y, output, &len)) != CRYPT_OK) {
            printf("Error OMAC'ing: %s\n", error_to_string(err));
            exit(EXIT_FAILURE);
         }
         if (len == 0) {
            printf("Error OMAC'ing: zero length\n");
            exit(EXIT_FAILURE);
         }
         fprintf(out, "%3d: ", y);
         for (z = 0; z <(int)len; z++) {
            fprintf(out, "%02X", output[z]);
         }
         fprintf(out, "\n");

         /* forward the key */
         for (z = 0; z < kl; z++) {
             key[z] = output[z % len];
         }
      }
      fprintf(out, "\n");
   }
   fclose(out);
}
#endif

#ifdef LTC_PMAC
static void pmac_gen(void)
{
   unsigned char key[MAXBLOCKSIZE], output[MAXBLOCKSIZE], input[MAXBLOCKSIZE*2+2];
   int err, x, y, z, kl;
   FILE *out;
   unsigned long len;

   out = fopen("pmac_tv.txt", "w");

   fprintf(out,
"PMAC Tests.  In these tests messages of N bytes long (00,01,02,...,NN-1) are PMAC'ed.  The initial key is\n"
"of the same format (length specified per cipher).  The PMAC key in step N+1 is the PMAC output of\n"
"step N (repeated as required to fill the array).\n\n");

   for (x = 0; cipher_descriptor[x].name != NULL; x++) {
      kl = cipher_descriptor[x].block_length;

      /* skip ciphers which do not have 64 or 128 bit block sizes */
      if (kl != 8 && kl != 16) continue;

      if (cipher_descriptor[x].keysize(&kl) != CRYPT_OK) {
         kl = cipher_descriptor[x].max_key_length;
      }
      fprintf(out, "PMAC-%s (%d byte key)\n", cipher_descriptor[x].name, kl);

      /* initial key/block */
      for (y = 0; y < kl; y++) {
          key[y] = (y & 255);
      }

      for (y = 0; y <= (int)(cipher_descriptor[x].block_length*2); y++) {
         for (z = 0; z < y; z++) {
            input[z] = (unsigned char)(z & 255);
         }
         len = sizeof(output);
         if ((err = pmac_memory(x, key, kl, input, y, output, &len)) != CRYPT_OK) {
            printf("Error PMACing: %s\n", error_to_string(err));
            exit(EXIT_FAILURE);
         }
         if (len == 0) {
            printf("Error PMAC'ing: zero length\n");
            exit(EXIT_FAILURE);
         }
         fprintf(out, "%3d: ", y);
         for (z = 0; z <(int)len; z++) {
            fprintf(out, "%02X", output[z]);
         }
         fprintf(out, "\n");

         /* forward the key */
         for (z = 0; z < kl; z++) {
             key[z] = output[z % len];
         }
      }
      fprintf(out, "\n");
   }
   fclose(out);
}
#endif

#ifdef LTC_EAX_MODE
static void eax_gen(void)
{
   int err, kl, x, y1, z;
   FILE *out;
   unsigned char key[MAXBLOCKSIZE], nonce[MAXBLOCKSIZE*2], header[MAXBLOCKSIZE*2],
                 plaintext[MAXBLOCKSIZE*2], tag[MAXBLOCKSIZE];
   unsigned long len;

   out = fopen("eax_tv.txt", "w");
   fprintf(out, "EAX Test Vectors.  Uses the 00010203...NN-1 pattern for header/nonce/plaintext/key.  The outputs\n"
                "are of the form ciphertext,tag for a given NN.  The key for step N>1 is the tag of the previous\n"
                "step repeated sufficiently.\n\n");

   for (x = 0; cipher_descriptor[x].name != NULL; x++) {
      kl = cipher_descriptor[x].block_length;

      /* skip ciphers which do not have 64 or 128 bit block sizes */
      if (kl != 8 && kl != 16) continue;

      if (cipher_descriptor[x].keysize(&kl) != CRYPT_OK) {
         kl = cipher_descriptor[x].max_key_length;
      }
      fprintf(out, "EAX-%s (%d byte key)\n", cipher_descriptor[x].name, kl);

      /* the key */
      for (z = 0; z < kl; z++) {
          key[z] = (z & 255);
      }

      for (y1 = 0; y1 <= (int)(cipher_descriptor[x].block_length*2); y1++){
         for (z = 0; z < y1; z++) {
            plaintext[z] = (unsigned char)(z & 255);
            nonce[z]     = (unsigned char)(z & 255);
            header[z]    = (unsigned char)(z & 255);
         }
         len = sizeof(tag);
         if ((err = eax_encrypt_authenticate_memory(x, key, kl, nonce, y1, header, y1, plaintext, y1, plaintext, tag, &len)) != CRYPT_OK) {
            printf("Error EAX'ing: %s\n", error_to_string(err));
            exit(EXIT_FAILURE);
         }
         if (len == 0) {
            printf("Error EAX'ing: zero length\n");
            exit(EXIT_FAILURE);
         }
         fprintf(out, "%3d: ", y1);
         for (z = 0; z < y1; z++) {
            fprintf(out, "%02X", plaintext[z]);
         }
         fprintf(out, ", ");
         for (z = 0; z <(int)len; z++) {
            fprintf(out, "%02X", tag[z]);
         }
         fprintf(out, "\n");

         /* forward the key */
         for (z = 0; z < kl; z++) {
             key[z] = tag[z % len];
         }
      }
      fprintf(out, "\n");
   }
   fclose(out);
}
#endif

#ifdef LTC_OCB_MODE
static void ocb_gen(void)
{
   int err, kl, x, y1, z;
   FILE *out;
   unsigned char key[MAXBLOCKSIZE], nonce[MAXBLOCKSIZE*2],
                 plaintext[MAXBLOCKSIZE*2], tag[MAXBLOCKSIZE];
   unsigned long len;

   out = fopen("ocb_tv.txt", "w");
   fprintf(out, "OCB Test Vectors.  Uses the 00010203...NN-1 pattern for nonce/plaintext/key.  The outputs\n"
                "are of the form ciphertext,tag for a given NN.  The key for step N>1 is the tag of the previous\n"
                "step repeated sufficiently.  The nonce is fixed throughout.\n\n");

   for (x = 0; cipher_descriptor[x].name != NULL; x++) {
      kl = cipher_descriptor[x].block_length;

      /* skip ciphers which do not have 64 or 128 bit block sizes */
      if (kl != 8 && kl != 16) continue;

      if (cipher_descriptor[x].keysize(&kl) != CRYPT_OK) {
         kl = cipher_descriptor[x].max_key_length;
      }
      fprintf(out, "OCB-%s (%d byte key)\n", cipher_descriptor[x].name, kl);

      /* the key */
      for (z = 0; z < kl; z++) {
          key[z] = (z & 255);
      }

      /* fixed nonce */
      for (z = 0; z < cipher_descriptor[x].block_length; z++) {
          nonce[z] = z;
      }

      for (y1 = 0; y1 <= (int)(cipher_descriptor[x].block_length*2); y1++){
         for (z = 0; z < y1; z++) {
            plaintext[z] = (unsigned char)(z & 255);
         }
         len = sizeof(tag);
         if ((err = ocb_encrypt_authenticate_memory(x, key, kl, nonce, plaintext, y1, plaintext, tag, &len)) != CRYPT_OK) {
            printf("Error OCB'ing: %s\n", error_to_string(err));
            exit(EXIT_FAILURE);
         }
         if (len == 0) {
            printf("Error OCB'ing: zero length\n");
            exit(EXIT_FAILURE);
         }
         fprintf(out, "%3d: ", y1);
         for (z = 0; z < y1; z++) {
            fprintf(out, "%02X", plaintext[z]);
         }
         fprintf(out, ", ");
         for (z = 0; z <(int)len; z++) {
            fprintf(out, "%02X", tag[z]);
         }
         fprintf(out, "\n");

         /* forward the key */
         for (z = 0; z < kl; z++) {
             key[z] = tag[z % len];
         }
      }
      fprintf(out, "\n");
   }
   fclose(out);
}
#endif

#ifdef LTC_OCB3_MODE
static void ocb3_gen(void)
{
   int err, kl, x, y1, z, noncelen;
   FILE *out;
   unsigned char key[MAXBLOCKSIZE], nonce[MAXBLOCKSIZE*2],
                 plaintext[MAXBLOCKSIZE*2], tag[MAXBLOCKSIZE];
   unsigned long len;

   out = fopen("ocb3_tv.txt", "w");
   fprintf(out, "OCB3 Test Vectors.  Uses the 00010203...NN-1 pattern for nonce/plaintext/key.  The outputs\n"
                "are of the form ciphertext,tag for a given NN.  The key for step N>1 is the tag of the previous\n"
                "step repeated sufficiently.  The nonce is fixed throughout. AAD is fixed to 3 bytes (ASCII) 'AAD'.\n\n");

   for (x = 0; cipher_descriptor[x].name != NULL; x++) {
      kl = cipher_descriptor[x].block_length;

      /* skip ciphers which do not have 64 or 128 bit block sizes */
      if (kl != 16) continue;

      if (cipher_descriptor[x].keysize(&kl) != CRYPT_OK) {
         kl = cipher_descriptor[x].max_key_length;
      }
      fprintf(out, "OCB3-%s (%d byte key)\n", cipher_descriptor[x].name, kl);

      /* the key */
      for (z = 0; z < kl; z++) {
          key[z] = (z & 255);
      }

      /* fixed nonce */
      noncelen = MIN(15, cipher_descriptor[x].block_length);
      for (z = 0; z < noncelen; z++) {
          nonce[z] = z;
      }

      for (y1 = 0; y1 <= (int)(cipher_descriptor[x].block_length*2); y1++){
         for (z = 0; z < y1; z++) {
            plaintext[z] = (unsigned char)(z & 255);
         }
         len = 16;
         if ((err = ocb3_encrypt_authenticate_memory(x, key, kl, nonce, noncelen, (unsigned char*)"AAD", 3, plaintext, y1, plaintext, tag, &len)) != CRYPT_OK) {
            printf("Error OCB3'ing: %s\n", error_to_string(err));
            exit(EXIT_FAILURE);
         }
         if (len == 0) {
            printf("Error OCB3'ing: zero length\n");
            exit(EXIT_FAILURE);
         }
         fprintf(out, "%3d: ", y1);
         for (z = 0; z < y1; z++) {
            fprintf(out, "%02X", plaintext[z]);
         }
         fprintf(out, ", ");
         for (z = 0; z <(int)len; z++) {
            fprintf(out, "%02X", tag[z]);
         }
         fprintf(out, "\n");

         /* forward the key */
         for (z = 0; z < kl; z++) {
             key[z] = tag[z % len];
         }
      }
      fprintf(out, "\n");
   }
   fclose(out);
}
#endif

#ifdef LTC_CCM_MODE
static void ccm_gen(void)
{
   int err, kl, x, y1, z;
   unsigned int t;
   FILE *out;
   unsigned char key[MAXBLOCKSIZE], nonce[MAXBLOCKSIZE*2],
                 plaintext[MAXBLOCKSIZE*2], tag[16];
   unsigned long len;
   const unsigned int taglen[] = {4, 6, 8, 10, 12, 14, 16};

   out = fopen("ccm_tv.txt", "w");
   fprintf(out, "CCM Test Vectors.  Uses the 00010203...NN-1 pattern for nonce/header/plaintext/key.  The outputs\n"
                "are of the form ciphertext,tag for a given NN.  The key for step N>1 is the tag of the previous\n"
                "step repeated sufficiently.  The nonce is fixed throughout at 13 bytes 000102...\n\n");

   for (x = 0; cipher_descriptor[x].name != NULL; x++) {
      kl = cipher_descriptor[x].block_length;

      /* skip ciphers which do not have 128 bit block sizes */
      if (kl != 16) continue;

      if (cipher_descriptor[x].keysize(&kl) != CRYPT_OK) {
         kl = cipher_descriptor[x].max_key_length;
      }
      fprintf(out, "CCM-%s (%d byte key)\n", cipher_descriptor[x].name, kl);

      /* the key */
      for (z = 0; z < kl; z++) {
          key[z] = (z & 255);
      }

      /* fixed nonce */
      for (z = 0; z < cipher_descriptor[x].block_length; z++) {
          nonce[z] = z;
      }

      for (t = 0; t < sizeof(taglen)/sizeof(taglen[0]); ++t) {
         for (y1 = 0; y1 <= (int)(cipher_descriptor[x].block_length*2); y1++){
            for (z = 0; z < y1; z++) {
               plaintext[z] = (unsigned char)(z & 255);
            }
            len = taglen[t];
            if ((err = ccm_memory(x, key, kl, NULL, nonce, 13, plaintext, y1, plaintext, y1, plaintext, tag, &len, CCM_ENCRYPT)) != CRYPT_OK) {
               printf("Error CCM'ing: %s\n", error_to_string(err));
               exit(EXIT_FAILURE);
            }
            if (len == 0) {
               printf("Error CCM'ing: zero length\n");
               exit(EXIT_FAILURE);
            }
            fprintf(out, "%3d: ", y1);
            for (z = 0; z < y1; z++) {
               fprintf(out, "%02X", plaintext[z]);
            }
            fprintf(out, ", ");
            for (z = 0; z <(int)len; z++) {
               fprintf(out, "%02X", tag[z]);
            }
            fprintf(out, "\n");

            /* forward the key */
            for (z = 0; z < kl; z++) {
                key[z] = tag[z % len];
            }
         }
      }
      fprintf(out, "\n");
   }
   fclose(out);
}
#endif

#ifdef LTC_GCM_MODE
static void gcm_gen(void)
{
   int err, kl, x, y1, z;
   FILE *out;
   unsigned char key[MAXBLOCKSIZE], plaintext[MAXBLOCKSIZE*2], tag[MAXBLOCKSIZE];
   unsigned long len;

   out = fopen("gcm_tv.txt", "w");
   fprintf(out, "GCM Test Vectors.  Uses the 00010203...NN-1 pattern for nonce/header/plaintext/key.  The outputs\n"
                "are of the form ciphertext,tag for a given NN.  The key for step N>1 is the tag of the previous\n"
                "step repeated sufficiently.  The nonce is fixed throughout at 13 bytes 000102...\n\n");

   for (x = 0; cipher_descriptor[x].name != NULL; x++) {
      kl = cipher_descriptor[x].block_length;

      /* skip ciphers which do not have 128 bit block sizes */
      if (kl != 16) continue;

      if (cipher_descriptor[x].keysize(&kl) != CRYPT_OK) {
         kl = cipher_descriptor[x].max_key_length;
      }
      fprintf(out, "GCM-%s (%d byte key)\n", cipher_descriptor[x].name, kl);

      /* the key */
      for (z = 0; z < kl; z++) {
          key[z] = (z & 255);
      }

      for (y1 = 1; y1 <= (int)(cipher_descriptor[x].block_length*2); y1++){
         for (z = 0; z < y1; z++) {
            plaintext[z] = (unsigned char)(z & 255);
         }
         len = sizeof(tag);
         if ((err = gcm_memory(x, key, kl, plaintext, y1, plaintext, y1, plaintext, y1, plaintext, tag, &len, GCM_ENCRYPT)) != CRYPT_OK) {
            printf("Error GCM'ing: %s\n", error_to_string(err));
            exit(EXIT_FAILURE);
         }
         if (len == 0) {
            printf("Error GCM'ing: zero length\n");
            exit(EXIT_FAILURE);
         }
         fprintf(out, "%3d: ", y1);
         for (z = 0; z < y1; z++) {
            fprintf(out, "%02X", plaintext[z]);
         }
         fprintf(out, ", ");
         for (z = 0; z <(int)len; z++) {
            fprintf(out, "%02X", tag[z]);
         }
         fprintf(out, "\n");

         /* forward the key */
         for (z = 0; z < kl; z++) {
             key[z] = tag[z % len];
         }
      }
      fprintf(out, "\n");
   }
   fclose(out);
}
#endif

static void base64_gen(void)
{
   FILE *out;
   unsigned char src[32], ch;
   char dst[256];
   unsigned long x, len;

   out = fopen("base64_tv.txt", "w");
   fprintf(out, "Base64 vectors.  These are the base64 encodings of the strings 00,01,02...NN-1\n\n");
   for (x = 0; x <= 32; x++) {
       for (ch = 0; ch < x; ch++) {
           src[ch] = ch;
       }
       len = sizeof(dst);
       base64_encode(src, x, dst, &len);
       fprintf(out, "%2lu: %s\n", x, dst);
   }
   fclose(out);
}

static void math_gen(void)
{
}

static void ecc_gen(void)
{
   FILE         *out;
   unsigned char str[512];
   void          *k, *order, *modulus, *a;
   ecc_point    *G, *R;
   int           x;

   out = fopen("ecc_tv.txt", "w");
   fprintf(out, "ecc vectors.  These are for kG for k=1,3,9,27,...,3**n until k > order of the curve outputs are <k,x,y> triplets\n\n");
   G = ltc_ecc_new_point();
   R = ltc_ecc_new_point();
   ltc_mp_init(&k);
   ltc_mp_init(&order);
   ltc_mp_init(&modulus);
   ltc_mp_init(&a);

   for (x = 0; ltc_ecc_curves[x].prime != NULL; x++) {
        fprintf(out, "%s\n", ltc_ecc_curves[x].OID);
        ltc_mp_set(k, 1);

        ltc_mp_read_radix(order,   ltc_ecc_curves[x].order, 16);
        ltc_mp_read_radix(modulus, ltc_ecc_curves[x].prime, 16);
        ltc_mp_read_radix(a,       ltc_ecc_curves[x].A,     16);
        ltc_mp_read_radix(G->x,    ltc_ecc_curves[x].Gx,    16);
        ltc_mp_read_radix(G->y,    ltc_ecc_curves[x].Gy,    16);
        ltc_mp_set(G->z, 1);

        while (ltc_mp_cmp(k, order) == LTC_MP_LT) {
            ltc_mp.ecc_ptmul(k, G, R, a, modulus, 1);
            ltc_mp_tohex(k,    (char*)str); fprintf(out, "%s, ", (char*)str);
            ltc_mp_tohex(R->x, (char*)str); fprintf(out, "%s, ", (char*)str);
            ltc_mp_tohex(R->y, (char*)str); fprintf(out, "%s\n", (char*)str);
            ltc_mp_mul_d(k, 3, k);
        }
   }
   ltc_mp_deinit_multi(k, order, modulus, a, LTC_NULL);
   ltc_ecc_del_point(G);
   ltc_ecc_del_point(R);
   fclose(out);
}

#ifdef LTC_LRW_MODE
static void lrw_gen(void)
{
   FILE *out;
   unsigned char tweak[16], key[16], iv[16], buf[1024];
   int x, y, err;
   symmetric_LRW lrw;

   /* initialize default key and tweak */
   for (x = 0; x < 16; x++) {
      tweak[x] = key[x] = iv[x] = x;
   }

   out = fopen("lrw_tv.txt", "w");
   for (x = 16; x < (int)(sizeof(buf)); x += 16) {
       if ((err = lrw_start(find_cipher("aes"), iv, key, 16, tweak, 0, &lrw)) != CRYPT_OK) {
          fprintf(stderr, "Error starting LRW-AES: %s\n", error_to_string(err));
          exit(EXIT_FAILURE);
       }

       /* encrypt incremental */
       for (y = 0; y < x; y++) {
           buf[y] = y & 255;
       }

       if ((err = lrw_encrypt(buf, buf, x, &lrw)) != CRYPT_OK) {
          fprintf(stderr, "Error encrypting with LRW-AES: %s\n", error_to_string(err));
          exit(EXIT_FAILURE);
       }

       /* display it */
       fprintf(out, "%d:", x);
       for (y = 0; y < x; y++) {
          fprintf(out, "%02x", buf[y]);
       }
       fprintf(out, "\n");

       /* reset IV */
       if ((err = lrw_setiv(iv, 16, &lrw)) != CRYPT_OK) {
          fprintf(stderr, "Error setting IV: %s\n", error_to_string(err));
          exit(EXIT_FAILURE);
       }

       /* copy new tweak, iv and key */
       for (y = 0; y < 16; y++) {
          key[y]   = buf[y];
          iv[y]    = buf[(y+16)%x];
          tweak[y] = buf[(y+32)%x];
       }

       if ((err = lrw_decrypt(buf, buf, x, &lrw)) != CRYPT_OK) {
          fprintf(stderr, "Error decrypting with LRW-AES: %s\n", error_to_string(err));
          exit(EXIT_FAILURE);
       }

       /* display it */
       fprintf(out, "%d:", x);
       for (y = 0; y < x; y++) {
          fprintf(out, "%02x", buf[y]);
       }
       fprintf(out, "\n");
       lrw_done(&lrw);
   }
   fclose(out);
}
#endif

static void LTC_NORETURN die(int status)
{
   FILE* o = status == EXIT_SUCCESS ? stdout : stderr;
   fprintf(o,
         "Usage: tv_gen [<-h>]\n\n"
         "Generate the internal test-vectors.\n\n"
         "\t-h\tThe help you're looking at.\n"
   );
   exit(status);
}

int main(int argc, char **argv)
{
   if (argc > 1) {
      if (strstr(argv[1], "-h")) {
         die(EXIT_SUCCESS);
      } else {
         die(EXIT_FAILURE);
      }
   }
   register_all_ciphers();
   register_all_hashes();
   register_all_prngs();
#ifdef USE_LTM
   ltc_mp = ltm_desc;
#elif defined(USE_TFM)
   ltc_mp = tfm_desc;
#elif defined(USE_GMP)
   ltc_mp = gmp_desc;
#elif defined(EXT_MATH_LIB)
   extern ltc_math_descriptor EXT_MATH_LIB;
   ltc_mp = EXT_MATH_LIB;
#endif

   printf("Generating hash   vectors..."); fflush(stdout); hash_gen();   printf("done\n");
   printf("Generating cipher vectors..."); fflush(stdout); cipher_gen(); printf("done\n");
   printf("Generating HMAC   vectors..."); fflush(stdout); hmac_gen();   printf("done\n");
#ifdef LTC_OMAC
   printf("Generating OMAC   vectors..."); fflush(stdout); omac_gen();   printf("done\n");
#endif
#ifdef LTC_PMAC
   printf("Generating PMAC   vectors..."); fflush(stdout); pmac_gen();   printf("done\n");
#endif
#ifdef LTC_EAX_MODE
   printf("Generating EAX    vectors..."); fflush(stdout); eax_gen();    printf("done\n");
#endif
#ifdef LTC_OCB_MODE
   printf("Generating OCB    vectors..."); fflush(stdout); ocb_gen();    printf("done\n");
#endif
#ifdef LTC_OCB3_MODE
   printf("Generating OCB3   vectors..."); fflush(stdout); ocb3_gen();   printf("done\n");
#endif
#ifdef LTC_CCM_MODE
   printf("Generating CCM    vectors..."); fflush(stdout); ccm_gen();    printf("done\n");
#endif
#ifdef LTC_GCM_MODE
   printf("Generating GCM    vectors..."); fflush(stdout); gcm_gen();    printf("done\n");
#endif
   printf("Generating BASE64 vectors..."); fflush(stdout); base64_gen(); printf("done\n");
   if (ltc_mp.name != NULL) {
      printf("Generating MATH   vectors..."); fflush(stdout); math_gen();   printf("done\n");
      printf("Generating ECC    vectors..."); fflush(stdout); ecc_gen();    printf("done\n");
   }
#ifdef LTC_LRW_MODE
   printf("Generating LRW    vectors..."); fflush(stdout); lrw_gen();    printf("done\n");
#endif
   return 0;
}
