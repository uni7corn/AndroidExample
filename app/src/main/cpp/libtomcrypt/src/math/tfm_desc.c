/* LibTomCrypt, modular cryptographic library -- Tom St Denis */
/* SPDX-License-Identifier: Unlicense */

#define DESC_DEF_ONLY
#include "tomcrypt_private.h"

#ifdef TFM_DESC

#include <tfm.h>

#if !defined(TFM_VERSION_3)
# if 0                          /* Enable if desirable */
#  warning "pre-constification TFM used (TFM_VERSION_3 undefined)"
# endif
# define TFM_UNCONST(type) (type)
#elif TFM_VERSION <= TFM_VERSION_3(0, 13, 89)
# if 0                          /* Enable if desirable */
#  warning "pre-constification TFM used (older version detected)"
# endif
# define TFM_UNCONST(type) (type)
#else
# define TFM_UNCONST(type)
#endif

static const struct {
    int tfm_code, ltc_code;
} tfm_to_ltc_codes[] = {
   { FP_OKAY ,  CRYPT_OK},
   { FP_MEM  ,  CRYPT_MEM},
   { FP_VAL  ,  CRYPT_INVALID_ARG},
};

/**
   Convert a tfm error to a LTC error (Possibly the most powerful function ever!  Oh wait... no)
   @param err    The error to convert
   @return The equivalent LTC error code or CRYPT_ERROR if none found
*/
static int tfm_to_ltc_error(int err)
{
   int x;

   for (x = 0; x < (int)(sizeof(tfm_to_ltc_codes)/sizeof(tfm_to_ltc_codes[0])); x++) {
       if (err == tfm_to_ltc_codes[x].tfm_code) {
          return tfm_to_ltc_codes[x].ltc_code;
       }
   }
   return CRYPT_ERROR;
}

static int init(void **a)
{
   LTC_ARGCHK(a != NULL);

   *a = XCALLOC(1, sizeof(fp_int));
   if (*a == NULL) {
      return CRYPT_MEM;
   }
   fp_init(*a);
   return CRYPT_OK;
}

static void deinit(void *a)
{
   LTC_ARGCHKVD(a != NULL);
   XFREE(a);
}

static int neg(const void *a, void *b)
{
   /* fp_neg() is a macro that accesses the internals of the b */
   fp_int *tmpb = b;

   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   fp_neg(a, tmpb);
   return CRYPT_OK;
}

static int copy(const void *a, void *b)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   fp_copy(a, b);
   return CRYPT_OK;
}

static int init_copy(void **a, const void *b)
{
   if (init(a) != CRYPT_OK) {
      return CRYPT_MEM;
   }
   return copy(b, *a);
}

/* ---- trivial ---- */
static int set_int(void *a, ltc_mp_digit b)
{
   LTC_ARGCHK(a != NULL);
   fp_set(a, b);
   return CRYPT_OK;
}

static unsigned long get_int(const void *a)
{
   const fp_int *A;
   LTC_ARGCHK(a != NULL);
   A = a;
   return A->used > 0 ? A->dp[0] : 0;
}

static ltc_mp_digit get_digit(const void *a, int n)
{
   const fp_int *A;
   LTC_ARGCHK(a != NULL);
   A = a;
   return (n >= A->used || n < 0) ? 0 : A->dp[n];
}

static int get_digit_count(const void *a)
{
   const fp_int *A;
   LTC_ARGCHK(a != NULL);
   A = a;
   return A->used;
}

static int compare(const void *a, const void *b)
{
   int ret;
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   ret = fp_cmp(TFM_UNCONST(void *)a, TFM_UNCONST(void *)b);
   switch (ret) {
      case FP_LT: return LTC_MP_LT;
      case FP_EQ: return LTC_MP_EQ;
      case FP_GT: return LTC_MP_GT;
   }
   return 0;
}

static int compare_d(const void *a, ltc_mp_digit b)
{
   int ret;
   LTC_ARGCHK(a != NULL);
   ret = fp_cmp_d(TFM_UNCONST(void *)a, b);
   switch (ret) {
      case FP_LT: return LTC_MP_LT;
      case FP_EQ: return LTC_MP_EQ;
      case FP_GT: return LTC_MP_GT;
   }
   return 0;
}

static int count_bits(const void *a)
{
   LTC_ARGCHK(a != NULL);
   return fp_count_bits(TFM_UNCONST(void *)a);
}

static int count_lsb_bits(const void *a)
{
   LTC_ARGCHK(a != NULL);
   return fp_cnt_lsb(TFM_UNCONST(void *)a);
}

static int twoexpt(void *a, int n)
{
   LTC_ARGCHK(a != NULL);
   fp_2expt(a, n);
   return CRYPT_OK;
}

/* ---- conversions ---- */

/* read ascii string */
static int read_radix(void *a, const char *b, int radix)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   return tfm_to_ltc_error(fp_read_radix(a, b, radix));
}

/* write one */
static int write_radix(const void *a, char *b, int radix)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   return tfm_to_ltc_error(fp_toradix(TFM_UNCONST(void *)a, b, radix));
}

/* get size as unsigned char string */
static unsigned long unsigned_size(const void *a)
{
   LTC_ARGCHK(a != NULL);
   return fp_unsigned_bin_size(TFM_UNCONST(void *)a);
}

/* store */
static int unsigned_write(const void *a, unsigned char *b)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   fp_to_unsigned_bin(TFM_UNCONST(void *)a, b);
   return CRYPT_OK;
}

/* read */
static int unsigned_read(void *a, const unsigned char *b, unsigned long len)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   fp_read_unsigned_bin(a, b, len);
   return CRYPT_OK;
}

/* add */
static int add(const void *a, const void *b, void *c)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   LTC_ARGCHK(c != NULL);
   fp_add(TFM_UNCONST(void *)a, TFM_UNCONST(void *)b, c);
   return CRYPT_OK;
}

static int addi(const void *a, ltc_mp_digit b, void *c)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(c != NULL);
   fp_add_d(TFM_UNCONST(void *)a, b, c);
   return CRYPT_OK;
}

/* sub */
static int sub(const void *a, const void *b, void *c)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   LTC_ARGCHK(c != NULL);
   fp_sub(TFM_UNCONST(void *)a, TFM_UNCONST(void *)b, c);
   return CRYPT_OK;
}

static int subi(const void *a, ltc_mp_digit b, void *c)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(c != NULL);
   fp_sub_d(TFM_UNCONST(void *)a, b, c);
   return CRYPT_OK;
}

/* mul */
static int mul(const void *a, const void *b, void *c)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   LTC_ARGCHK(c != NULL);
   fp_mul(TFM_UNCONST(void *)a, TFM_UNCONST(void *)b, c);
   return CRYPT_OK;
}

static int muli(const void *a, ltc_mp_digit b, void *c)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(c != NULL);
   fp_mul_d(TFM_UNCONST(void *)a, b, c);
   return CRYPT_OK;
}

/* sqr */
static int sqr(const void *a, void *b)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   fp_sqr(TFM_UNCONST(void *)a, b);
   return CRYPT_OK;
}

/* sqrtmod_prime - NOT SUPPORTED */

/* div */
static int divide(const void *a, const void *b, void *c, void *d)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   return tfm_to_ltc_error(fp_div(TFM_UNCONST(void *)a, TFM_UNCONST(void *)b, c, d));
}

static int div_2(const void *a, void *b)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   fp_div_2(TFM_UNCONST(void *)a, b);
   return CRYPT_OK;
}

/* modi */
static int modi(const void *a, ltc_mp_digit b, ltc_mp_digit *c)
{
   fp_digit tmp;
   int      err;

   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(c != NULL);

   if ((err = tfm_to_ltc_error(fp_mod_d(TFM_UNCONST(void *)a, b, &tmp))) != CRYPT_OK) {
      return err;
   }
   *c = tmp;
   return CRYPT_OK;
}

/* gcd */
static int gcd(const void *a, const void *b, void *c)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   LTC_ARGCHK(c != NULL);
   fp_gcd(TFM_UNCONST(void *)a, TFM_UNCONST(void *)b, c);
   return CRYPT_OK;
}

/* lcm */
static int lcm(const void *a, const void *b, void *c)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   LTC_ARGCHK(c != NULL);
   fp_lcm(TFM_UNCONST(void *)a, TFM_UNCONST(void *)b, c);
   return CRYPT_OK;
}

static int addmod(const void *a, const void *b, const void *c, void *d)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   LTC_ARGCHK(c != NULL);
   LTC_ARGCHK(d != NULL);
   return tfm_to_ltc_error(fp_addmod(TFM_UNCONST(void *)a,TFM_UNCONST(void *)b,TFM_UNCONST(void *)c,d));
}

static int submod(const void *a, const void *b, const void *c, void *d)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   LTC_ARGCHK(c != NULL);
   LTC_ARGCHK(d != NULL);
   return tfm_to_ltc_error(fp_submod(TFM_UNCONST(void *)a,TFM_UNCONST(void *)b,TFM_UNCONST(void *)c,d));
}

static int mulmod(const void *a, const void *b, const void *c, void *d)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   LTC_ARGCHK(c != NULL);
   LTC_ARGCHK(d != NULL);
   return tfm_to_ltc_error(fp_mulmod(TFM_UNCONST(void *)a,TFM_UNCONST(void *)b,TFM_UNCONST(void *)c,d));
}

static int sqrmod(const void *a, const void *b, void *c)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   LTC_ARGCHK(c != NULL);
   return tfm_to_ltc_error(fp_sqrmod(TFM_UNCONST(void *)a,TFM_UNCONST(void *)b,c));
}

/* invmod */
static int invmod(const void *a, const void *b, void *c)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   LTC_ARGCHK(c != NULL);
   return tfm_to_ltc_error(fp_invmod(TFM_UNCONST(void *)a, TFM_UNCONST(void *)b, c));
}

/* setup */
static int montgomery_setup(const void *a, void **b)
{
   int err;
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   *b = XCALLOC(1, sizeof(fp_digit));
   if (*b == NULL) {
      return CRYPT_MEM;
   }
   if ((err = tfm_to_ltc_error(fp_montgomery_setup(TFM_UNCONST(void *)a, *b))) != CRYPT_OK) {
      XFREE(*b);
   }
   return err;
}

/* get normalization value */
static int montgomery_normalization(void *a, const void *b)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   fp_montgomery_calc_normalization(a, TFM_UNCONST(void *)b);
   return CRYPT_OK;
}

/* reduce */
static int montgomery_reduce(void *a, const void *b, void *c)
{
   fp_digit *tmpc = c;
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   LTC_ARGCHK(c != NULL);
   fp_montgomery_reduce(a, TFM_UNCONST(void *)b, *tmpc);
   return CRYPT_OK;
}

/* clean up */
static void montgomery_deinit(void *a)
{
   XFREE(a);
}

static int exptmod(const void *a, const void *b, const void *c, void *d)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(b != NULL);
   LTC_ARGCHK(c != NULL);
   LTC_ARGCHK(d != NULL);
   return tfm_to_ltc_error(fp_exptmod(TFM_UNCONST(void *)a,TFM_UNCONST(void *)b,TFM_UNCONST(void *)c,d));
}

static int isprime(const void *a, int b, int *c)
{
   LTC_ARGCHK(a != NULL);
   LTC_ARGCHK(c != NULL);
   if (b == 0) {
       b = LTC_MILLER_RABIN_REPS;
   } /* if */
   *c = (fp_isprime_ex(TFM_UNCONST(void *)a, b) == FP_YES) ? LTC_MP_YES : LTC_MP_NO;
   return CRYPT_OK;
}

#if defined(LTC_MECC) && defined(LTC_MECC_ACCEL)

static int tfm_ecc_projective_dbl_point(const ecc_point *P, ecc_point *R, const void *ma, const void *modulus, void *Mp)
{
   fp_int t1, t2;
   fp_digit mp;
   int err, inf;

   LTC_ARGCHK(P       != NULL);
   LTC_ARGCHK(R       != NULL);
   LTC_ARGCHK(modulus != NULL);
   LTC_ARGCHK(Mp      != NULL);

   mp = *((fp_digit*)Mp);

   fp_init(&t1);
   fp_init(&t2);

   if (P != R) {
      fp_copy(P->x, R->x);
      fp_copy(P->y, R->y);
      fp_copy(P->z, R->z);
   }

   if ((err = ltc_ecc_is_point_at_infinity(P, modulus, &inf)) != CRYPT_OK) return err;
   if (inf) {
      /* if P is point at infinity >> Result = point at infinity */
      ltc_mp.set_int(R->x, 1);
      ltc_mp.set_int(R->y, 1);
      ltc_mp.set_int(R->z, 0);
      return CRYPT_OK;
   }

   /* t1 = Z * Z */
   fp_sqr(R->z, &t1);
   fp_montgomery_reduce(&t1, TFM_UNCONST(void *)modulus, mp);
   /* Z = Y * Z */
   fp_mul(R->z, R->y, R->z);
   fp_montgomery_reduce(R->z, TFM_UNCONST(void *)modulus, mp);
   /* Z = 2Z */
   fp_add(R->z, R->z, R->z);
   if (fp_cmp(R->z, TFM_UNCONST(void *)modulus) != FP_LT) {
      fp_sub(R->z, TFM_UNCONST(void *)modulus, R->z);
   }

   if (ma == NULL) { /* special case for curves with a == -3 (10% faster than general case) */
      /* T2 = X - T1 */
      fp_sub(R->x, &t1, &t2);
      if (fp_cmp_d(&t2, 0) == LTC_MP_LT) {
         fp_add(&t2, TFM_UNCONST(void *)modulus, &t2);
      }
      /* T1 = X + T1 */
      fp_add(&t1, R->x, &t1);
      if (fp_cmp(&t1, TFM_UNCONST(void *)modulus) != FP_LT) {
         fp_sub(&t1, TFM_UNCONST(void *)modulus, &t1);
      }
      /* T2 = T1 * T2 */
      fp_mul(&t1, &t2, &t2);
      fp_montgomery_reduce(&t2, TFM_UNCONST(void *)modulus, mp);
      /* T1 = 2T2 */
      fp_add(&t2, &t2, &t1);
      if (fp_cmp(&t1, TFM_UNCONST(void *)modulus) != FP_LT) {
         fp_sub(&t1, TFM_UNCONST(void *)modulus, &t1);
      }
      /* T1 = T1 + T2 */
      fp_add(&t1, &t2, &t1);
      if (fp_cmp(&t1, TFM_UNCONST(void *)modulus) != FP_LT) {
         fp_sub(&t1, TFM_UNCONST(void *)modulus, &t1);
      }
   }
   else {
      /* T2 = T1 * T1 */
      fp_sqr(&t1, &t2);
      fp_montgomery_reduce(&t2, TFM_UNCONST(void *)modulus, mp);
      /* T1 = T2 * a */
      fp_mul(&t2, TFM_UNCONST(void *)ma, &t1);
      fp_montgomery_reduce(&t1, TFM_UNCONST(void *)modulus, mp);
      /* T2 = X * X */
      fp_sqr(R->x, &t2);
      fp_montgomery_reduce(&t2, TFM_UNCONST(void *)modulus, mp);
      /* T1 = T1 + T2 */
      fp_add(&t1, &t2, &t1);
      if (fp_cmp(&t1, TFM_UNCONST(void *)modulus) != FP_LT) {
         fp_sub(&t1, TFM_UNCONST(void *)modulus, &t1);
      }
      /* T1 = T1 + T2 */
      fp_add(&t1, &t2, &t1);
      if (fp_cmp(&t1, TFM_UNCONST(void *)modulus) != FP_LT) {
         fp_sub(&t1, TFM_UNCONST(void *)modulus, &t1);
      }
      /* T1 = T1 + T2 */
      fp_add(&t1, &t2, &t1);
      if (fp_cmp(&t1, TFM_UNCONST(void *)modulus) != FP_LT) {
         fp_sub(&t1, TFM_UNCONST(void *)modulus, &t1);
      }
   }

   /* Y = 2Y */
   fp_add(R->y, R->y, R->y);
   if (fp_cmp(R->y, TFM_UNCONST(void *)modulus) != FP_LT) {
      fp_sub(R->y, TFM_UNCONST(void *)modulus, R->y);
   }
   /* Y = Y * Y */
   fp_sqr(R->y, R->y);
   fp_montgomery_reduce(R->y, TFM_UNCONST(void *)modulus, mp);
   /* T2 = Y * Y */
   fp_sqr(R->y, &t2);
   fp_montgomery_reduce(&t2, TFM_UNCONST(void *)modulus, mp);
   /* T2 = T2/2 */
   if (fp_isodd(&t2)) {
      fp_add(&t2, TFM_UNCONST(void *)modulus, &t2);
   }
   fp_div_2(&t2, &t2);
   /* Y = Y * X */
   fp_mul(R->y, R->x, R->y);
   fp_montgomery_reduce(R->y, TFM_UNCONST(void *)modulus, mp);

   /* X  = T1 * T1 */
   fp_sqr(&t1, R->x);
   fp_montgomery_reduce(R->x, TFM_UNCONST(void *)modulus, mp);
   /* X = X - Y */
   fp_sub(R->x, R->y, R->x);
   if (fp_cmp_d(R->x, 0) == FP_LT) {
      fp_add(R->x, TFM_UNCONST(void *)modulus, R->x);
   }
   /* X = X - Y */
   fp_sub(R->x, R->y, R->x);
   if (fp_cmp_d(R->x, 0) == FP_LT) {
      fp_add(R->x, TFM_UNCONST(void *)modulus, R->x);
   }

   /* Y = Y - X */
   fp_sub(R->y, R->x, R->y);
   if (fp_cmp_d(R->y, 0) == FP_LT) {
      fp_add(R->y, TFM_UNCONST(void *)modulus, R->y);
   }
   /* Y = Y * T1 */
   fp_mul(R->y, &t1, R->y);
   fp_montgomery_reduce(R->y, TFM_UNCONST(void *)modulus, mp);
   /* Y = Y - T2 */
   fp_sub(R->y, &t2, R->y);
   if (fp_cmp_d(R->y, 0) == FP_LT) {
      fp_add(R->y, TFM_UNCONST(void *)modulus, R->y);
   }

   return CRYPT_OK;
}

/**
   Add two ECC points
   @param P        The point to add
   @param Q        The point to add
   @param R        [out] The destination of the double
   @param modulus  The modulus of the field the ECC curve is in
   @param Mp       The "b" value from montgomery_setup()
   @return CRYPT_OK on success
*/
static int tfm_ecc_projective_add_point(const ecc_point *P, const ecc_point *Q, ecc_point *R, const void *ma, const void *modulus, void *Mp)
{
   fp_int  t1, t2, x, y, z;
   fp_digit mp;
   int err, inf;

   LTC_ARGCHK(P       != NULL);
   LTC_ARGCHK(Q       != NULL);
   LTC_ARGCHK(R       != NULL);
   LTC_ARGCHK(modulus != NULL);
   LTC_ARGCHK(Mp      != NULL);

   mp = *((fp_digit*)Mp);

   fp_init(&t1);
   fp_init(&t2);
   fp_init(&x);
   fp_init(&y);
   fp_init(&z);

   if ((err = ltc_ecc_is_point_at_infinity(P, modulus, &inf)) != CRYPT_OK) return err;
   if (inf) {
      /* P is point at infinity >> Result = Q */
      ltc_mp.copy(Q->x, R->x);
      ltc_mp.copy(Q->y, R->y);
      ltc_mp.copy(Q->z, R->z);
      return CRYPT_OK;
   }

   if ((err = ltc_ecc_is_point_at_infinity(Q, modulus, &inf)) != CRYPT_OK) return err;
   if (inf) {
      /* Q is point at infinity >> Result = P */
      ltc_mp.copy(P->x, R->x);
      ltc_mp.copy(P->y, R->y);
      ltc_mp.copy(P->z, R->z);
      return CRYPT_OK;
   }

   /* should we dbl instead? */
   fp_sub(TFM_UNCONST(void *)modulus, Q->y, &t1);
   if ( (fp_cmp(P->x, Q->x) == FP_EQ) &&
        (Q->z != NULL && fp_cmp(P->z, Q->z) == FP_EQ) &&
        (fp_cmp(P->y, Q->y) == FP_EQ || fp_cmp(P->y, &t1) == FP_EQ)) {
        return tfm_ecc_projective_dbl_point(P, R, ma, modulus, Mp);
   }

   fp_copy(P->x, &x);
   fp_copy(P->y, &y);
   fp_copy(P->z, &z);

   /* if Z is one then these are no-operations */
   if (Q->z != NULL) {
      /* T1 = Z' * Z' */
      fp_sqr(Q->z, &t1);
      fp_montgomery_reduce(&t1, TFM_UNCONST(void *)modulus, mp);
      /* X = X * T1 */
      fp_mul(&t1, &x, &x);
      fp_montgomery_reduce(&x, TFM_UNCONST(void *)modulus, mp);
      /* T1 = Z' * T1 */
      fp_mul(Q->z, &t1, &t1);
      fp_montgomery_reduce(&t1, TFM_UNCONST(void *)modulus, mp);
      /* Y = Y * T1 */
      fp_mul(&t1, &y, &y);
      fp_montgomery_reduce(&y, TFM_UNCONST(void *)modulus, mp);
   }

   /* T1 = Z*Z */
   fp_sqr(&z, &t1);
   fp_montgomery_reduce(&t1, TFM_UNCONST(void *)modulus, mp);
   /* T2 = X' * T1 */
   fp_mul(Q->x, &t1, &t2);
   fp_montgomery_reduce(&t2, TFM_UNCONST(void *)modulus, mp);
   /* T1 = Z * T1 */
   fp_mul(&z, &t1, &t1);
   fp_montgomery_reduce(&t1, TFM_UNCONST(void *)modulus, mp);
   /* T1 = Y' * T1 */
   fp_mul(Q->y, &t1, &t1);
   fp_montgomery_reduce(&t1, TFM_UNCONST(void *)modulus, mp);

   /* Y = Y - T1 */
   fp_sub(&y, &t1, &y);
   if (fp_cmp_d(&y, 0) == FP_LT) {
      fp_add(&y, TFM_UNCONST(void *)modulus, &y);
   }
   /* T1 = 2T1 */
   fp_add(&t1, &t1, &t1);
   if (fp_cmp(&t1, TFM_UNCONST(void *)modulus) != FP_LT) {
      fp_sub(&t1, TFM_UNCONST(void *)modulus, &t1);
   }
   /* T1 = Y + T1 */
   fp_add(&t1, &y, &t1);
   if (fp_cmp(&t1, TFM_UNCONST(void *)modulus) != FP_LT) {
      fp_sub(&t1, TFM_UNCONST(void *)modulus, &t1);
   }
   /* X = X - T2 */
   fp_sub(&x, &t2, &x);
   if (fp_cmp_d(&x, 0) == FP_LT) {
      fp_add(&x, TFM_UNCONST(void *)modulus, &x);
   }
   /* T2 = 2T2 */
   fp_add(&t2, &t2, &t2);
   if (fp_cmp(&t2, TFM_UNCONST(void *)modulus) != FP_LT) {
      fp_sub(&t2, TFM_UNCONST(void *)modulus, &t2);
   }
   /* T2 = X + T2 */
   fp_add(&t2, &x, &t2);
   if (fp_cmp(&t2, TFM_UNCONST(void *)modulus) != FP_LT) {
      fp_sub(&t2, TFM_UNCONST(void *)modulus, &t2);
   }

   /* if Z' != 1 */
   if (Q->z != NULL) {
      /* Z = Z * Z' */
      fp_mul(&z, Q->z, &z);
      fp_montgomery_reduce(&z, TFM_UNCONST(void *)modulus, mp);
   }

   /* Z = Z * X */
   fp_mul(&z, &x, &z);
   fp_montgomery_reduce(&z, TFM_UNCONST(void *)modulus, mp);

   /* T1 = T1 * X  */
   fp_mul(&t1, &x, &t1);
   fp_montgomery_reduce(&t1, TFM_UNCONST(void *)modulus, mp);
   /* X = X * X */
   fp_sqr(&x, &x);
   fp_montgomery_reduce(&x, TFM_UNCONST(void *)modulus, mp);
   /* T2 = T2 * x */
   fp_mul(&t2, &x, &t2);
   fp_montgomery_reduce(&t2, TFM_UNCONST(void *)modulus, mp);
   /* T1 = T1 * X  */
   fp_mul(&t1, &x, &t1);
   fp_montgomery_reduce(&t1, TFM_UNCONST(void *)modulus, mp);

   /* X = Y*Y */
   fp_sqr(&y, &x);
   fp_montgomery_reduce(&x, TFM_UNCONST(void *)modulus, mp);
   /* X = X - T2 */
   fp_sub(&x, &t2, &x);
   if (fp_cmp_d(&x, 0) == FP_LT) {
      fp_add(&x, TFM_UNCONST(void *)modulus, &x);
   }

   /* T2 = T2 - X */
   fp_sub(&t2, &x, &t2);
   if (fp_cmp_d(&t2, 0) == FP_LT) {
      fp_add(&t2, TFM_UNCONST(void *)modulus, &t2);
   }
   /* T2 = T2 - X */
   fp_sub(&t2, &x, &t2);
   if (fp_cmp_d(&t2, 0) == FP_LT) {
      fp_add(&t2, TFM_UNCONST(void *)modulus, &t2);
   }
   /* T2 = T2 * Y */
   fp_mul(&t2, &y, &t2);
   fp_montgomery_reduce(&t2, TFM_UNCONST(void *)modulus, mp);
   /* Y = T2 - T1 */
   fp_sub(&t2, &t1, &y);
   if (fp_cmp_d(&y, 0) == FP_LT) {
      fp_add(&y, TFM_UNCONST(void *)modulus, &y);
   }
   /* Y = Y/2 */
   if (fp_isodd(&y)) {
      fp_add(&y, TFM_UNCONST(void *)modulus, &y);
   }
   fp_div_2(&y, &y);

   fp_copy(&x, R->x);
   fp_copy(&y, R->y);
   fp_copy(&z, R->z);

   return CRYPT_OK;
}


#endif

static int set_rand(void *a, int size)
{
   LTC_ARGCHK(a != NULL);
   fp_rand(a, size);
   return CRYPT_OK;
}

const ltc_math_descriptor tfm_desc = {

   "TomsFastMath",
   (int)DIGIT_BIT,

   &init,
   &init_copy,
   &deinit,

   &neg,
   &copy,

   &set_int,
   &get_int,
   &get_digit,
   &get_digit_count,
   &compare,
   &compare_d,
   &count_bits,
   &count_lsb_bits,
   &twoexpt,

   &read_radix,
   &write_radix,
   &unsigned_size,
   &unsigned_write,
   &unsigned_read,

   &add,
   &addi,
   &sub,
   &subi,
   &mul,
   &muli,
   &sqr,
   NULL, /* TODO: &sqrtmod_prime */
   &divide,
   &div_2,
   &modi,
   &gcd,
   &lcm,

   &mulmod,
   &sqrmod,
   &invmod,

   &montgomery_setup,
   &montgomery_normalization,
   &montgomery_reduce,
   &montgomery_deinit,

   &exptmod,
   &isprime,

#ifdef LTC_MECC
#ifdef LTC_MECC_FP
   &ltc_ecc_fp_mulmod,
#else
   &ltc_ecc_mulmod,
#endif /* LTC_MECC_FP */
#ifdef LTC_MECC_ACCEL
   &tfm_ecc_projective_add_point,
   &tfm_ecc_projective_dbl_point,
#else
   &ltc_ecc_projective_add_point,
   &ltc_ecc_projective_dbl_point,
#endif /* LTC_MECC_ACCEL */
   &ltc_ecc_map,
#ifdef LTC_ECC_SHAMIR
#ifdef LTC_MECC_FP
   &ltc_ecc_fp_mul2add,
#else
   &ltc_ecc_mul2add,
#endif /* LTC_MECC_FP */
#else
   NULL,
#endif /* LTC_ECC_SHAMIR */
#else
   NULL, NULL, NULL, NULL, NULL,
#endif /* LTC_MECC */

#ifdef LTC_MRSA
   &rsa_make_key,
   &rsa_exptmod,
#else
   NULL, NULL,
#endif
   &addmod,
   &submod,

   set_rand,

};


#endif
