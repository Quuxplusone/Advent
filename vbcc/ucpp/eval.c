/*
 * (c) Thomas Pornin 1999, 2000
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 4. The name of the authors may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <limits.h>
#include "ucppi.h"
#include "tune.h"
#include "mem.h"

JMP_BUF eval_exception;
long eval_line;
static int emit_eval_warnings;

/*
 * If you want to hardcode a conversion table, define a static array
 * of 256 int, and make transient_characters point to it.
 */
int *transient_characters = 0;

#define OCTAL(x)	((x) >= '0' && (x) <= '7')
#define DECIM(x)	((x) >= '0' && (x) <= '9')
#define HEXAD(x)	(((x) >= '0' && (x) <= '9') \
			|| (x) == 'a' || (x) == 'b' || (x) == 'c' \
			|| (x) == 'd' || (x) == 'e' || (x) == 'f' \
			|| (x) == 'A' || (x) == 'B' || (x) == 'C' \
			|| (x) == 'D' || (x) == 'E' || (x) == 'F')
#define OVAL(x)		((int)((x) - '0'))
#define DVAL(x)		((int)((x) - '0'))
#define HVAL(x)		(DECIM(x) ? DVAL(x) \
			: (x) == 'a' || (x) == 'A' ? 10 \
			: (x) == 'b' || (x) == 'B' ? 11 \
			: (x) == 'c' || (x) == 'C' ? 12 \
			: (x) == 'd' || (x) == 'D' ? 13 \
			: (x) == 'e' || (x) == 'E' ? 14 : 15)

#if !defined(NATIVE_UINTMAX) && !defined(SIMUL_UINTMAX)
#  if __STDC__ && __STDC_VERSION__ >= 199901L
#    include <stdint.h>
#    define NATIVE_UINTMAX uintmax_t
#    define NATIVE_INTMAX intmax_t
#  else
#    define NATIVE_UINTMAX unsigned long
#    define NATIVE_INTMAX long
#  endif
#endif

#ifdef NATIVE_UINTMAX

typedef NATIVE_UINTMAX ucppulong;
typedef NATIVE_INTMAX ucppslong;

#define to_ulong(x)		((ucppulong)(x))
#define to_slong(x)		((ucppslong)(x))

#define op_star_u(x, y)		((x) * (y))

static inline ucppulong op_slash_u(ucppulong x, ucppulong y)
{
	if (y == 0) {
		error(eval_line, "division by 0");
		throw(eval_exception);
	}
	return x / y;
}

static inline ucppulong op_pct_u(ucppulong x, ucppulong y)
{
	if (y == 0) {
		error(eval_line, "division by 0");
		throw(eval_exception);
	}
	return x % y;
}

#define op_plus_u(x, y)		((x) + (y))
#define op_minus_u(x, y)	((x) - (y))
#define op_neg_u(x)		(-(x))
#define op_lsh_u(x, y)		((x) << (y))
#define op_rsh_u(x, y)		((x) >> (y))
#define op_lt_u(x, y)		((x) < (y))
#define op_leq_u(x, y)		((x) <= (y))
#define op_gt_u(x, y)		((x) > (y))
#define op_geq_u(x, y)		((x) >= (y))
#define op_same_u(x, y)		((x) == (y))
#define op_neq_u(x, y)		((x) != (y))
#define op_and_u(x, y)		((x) & (y))
#define op_circ_u(x, y)		((x) ^ (y))
#define op_or_u(x, y)		((x) | (y))
#define op_lval_u(x)		((x) != 0)
#define op_lnot_u(x)		(!(x))
#define op_not_u(x)		(~(x))
#define op_uplus_u(x)		(x)
#define op_uminus_u(x)		op_neg_u(x)

#define op_star_s(x, y)		((x) * (y))

static inline ucppslong op_slash_s(ucppslong x, ucppslong y)
{
	if (y == 0) {
		error(eval_line, "division by 0");
		throw(eval_exception);
	}
	return x / y;
}

static inline ucppslong op_pct_s(ucppslong x, ucppslong y)
{
	if (y == 0) {
		error(eval_line, "division by 0");
		throw(eval_exception);
	}
	return x % y;
}

#define op_plus_s(x, y)		((x) + (y))
#define op_minus_s(x, y)	((x) - (y))
#define op_neg_s(x)		(-(x))
#define op_lsh_s(x, y)		((x) << (y))
#define op_rsh_s(x, y)		((x) >> (y))
#define op_lt_s(x, y)		((x) < (y))
#define op_leq_s(x, y)		((x) <= (y))
#define op_gt_s(x, y)		((x) > (y))
#define op_geq_s(x, y)		((x) >= (y))
#define op_same_s(x, y)		((x) == (y))
#define op_neq_s(x, y)		((x) != (y))
#define op_and_s(x, y)		((x) & (y))
#define op_circ_s(x, y)		((x) ^ (y))
#define op_or_s(x, y)		((x) | (y))
#define op_lval_s(x)		((x) != 0)
#define op_lnot_s(x)		(!(x))
#define op_not_s(x)		(~(x))
#define op_uplus_s(x)		(x)
#define op_uminus_s(x)		op_neg_s(x)

#define op_promo(x)		((ucppulong)x)
#define back_ulong(x)		((unsigned long)x)

#else

/*
 * We suppose that the unsigned long is not padded in its memory
 * representation, and that it has an even binary length.
 *
 * We could explore the size of ULONG_MAX with some dichotomic
 * trick using a recursive #include, but this would be really ugly.
 */
#define LONGSIZE	(CHAR_BIT * sizeof(unsigned long))
#define HALFLONGSIZE	(LONGSIZE / 2)
#define HUL_MASK	((1UL << HALFLONGSIZE) - 1)
#define UL_MS(x)	(((x) >> HALFLONGSIZE) & HUL_MASK)
#define UL_LS(x)	((x) & HUL_MASK)
#define SIGNBIT(x)	((x) & (1UL << (LONGSIZE - 1)))

typedef struct {
	unsigned long msw, lsw;
} ucppulong;

#define ucppslong	ucppulong

static ucppulong to_ulong(unsigned long x)
{
	ucppulong y;

	y.msw = 0;
	y.lsw = x;
	return y;
}

static ucppslong to_slong(long x)
{
	ucppslong y;

	y.lsw = x;
	y.msw = x >= 0 ? 0 : (unsigned long)(-1);
	return y;
}

static ucppulong op_plus_u(ucppulong x, ucppulong y)
{
	ucppulong z;

	z.lsw = x.lsw + y.lsw;
	z.msw = x.msw + y.msw;
	if (z.lsw < x.lsw) z.msw ++;
	return z;
}

static ucppulong op_neg_u(ucppulong x)
{
	ucppulong z;

	z.lsw = ~x.lsw;
	z.msw = ~x.msw;
	if (!(++ z.lsw)) z.msw ++;
	return z;
}

#define op_minus_u(x, y)	op_plus_u(x, op_neg_u(y))

static ucppulong op_lsh_u(ucppulong x, int s)
{
	ucppulong z;

	s %= (2 * LONGSIZE);
	if (s == 0) return x;
	if (s >= LONGSIZE) {
		z.lsw = 0;
		z.msw = x.lsw;
		return op_lsh_u(z, s - LONGSIZE);
	}
	z.lsw = (x.lsw << s);
	z.msw = (x.msw << s) | (x.lsw >> (LONGSIZE - s));
	return z;
}

static ucppulong op_rsh_u(ucppulong x, int s)
{
	ucppulong z;

	s %= (2 * LONGSIZE);
	if (s == 0) return x;
	if (s >= LONGSIZE) {
		z.msw = 0;
		z.lsw = x.msw;
		return op_rsh_u(z, s - LONGSIZE);
	}
	z.lsw = (x.lsw >> s) | (x.msw << (LONGSIZE - s));
	z.msw = (x.msw >> s);
	return z;
}

static int op_lt_u(ucppulong x, ucppulong y)
{
	if (x.msw < y.msw) return 1;
	if (x.msw > y.msw) return 0;
	if (x.lsw < y.lsw) return 1;
	return 0;
}

static int op_leq_u(ucppulong x, ucppulong y)
{
	if (x.msw < y.msw) return 1;
	if (x.msw > y.msw) return 0;
	if (x.lsw <= y.lsw) return 1;
	return 0;
}

#define op_gt_u(x, y)	op_lt_u(y, x)
#define op_geq_u(x, y)	op_leq_u(y, x)

static int op_same_u(ucppulong x, ucppulong y)
{
	return x.msw == y.msw && x.lsw == y.lsw;
}

#define op_neq_u(x, y)	(!op_same_u(x, y))

static ucppulong op_and_u(ucppulong x, ucppulong y)
{
	ucppulong z;

	z.lsw = x.lsw & y.lsw;
	z.msw = x.msw & y.msw;
	return z;
}

static ucppulong op_circ_u(ucppulong x, ucppulong y)
{
	ucppulong z;

	z.lsw = x.lsw ^ y.lsw;
	z.msw = x.msw ^ y.msw;
	return z;
}

static ucppulong op_or_u(ucppulong x, ucppulong y)
{
	ucppulong z;

	z.lsw = x.lsw | y.lsw;
	z.msw = x.msw | y.msw;
	return z;
}

#define op_lval_u(x)	op_neq_u(x, to_ulong(0))
#define op_lnot_u(x)	(!op_lval_u(x))

static ucppulong op_not_u(ucppulong x)
{
	ucppulong z;

	z.lsw = ~x.lsw;
	z.msw = ~x.msw;
	return z;
}

#define op_uplus_u(x)	(x)
#define op_uminus_u(x)	op_neg_u(x)

static ucppulong umul(unsigned long x, unsigned long y)
{
	ucppulong z;
	unsigned long t00, t01, t10, t11, c = 0, t;

	t00 = UL_LS(x) * UL_LS(y);
	t01 = UL_LS(x) * UL_MS(y);
	t10 = UL_MS(x) * UL_LS(y);
	t11 = UL_MS(x) * UL_MS(y);
	t = z.lsw = t00;
	if (t > (z.lsw += (UL_LS(t01) << HALFLONGSIZE))) c ++;
	t = z.lsw;
	if (t > (z.lsw += (UL_LS(t10) << HALFLONGSIZE))) c ++;
	z.msw = t11 + UL_MS(t10) + UL_MS(t01) + c;
	return z;
}

static ucppulong op_star_u(ucppulong x, ucppulong y)
{
	ucppulong z1, z2;

	z1.lsw = z2.lsw = 0;
	z1.msw = x.lsw * y.msw;
	z2.msw = x.msw * y.lsw;
	return op_plus_u(umul(x.lsw, y.lsw), op_plus_u(z1, z2));
}

static void udiv(ucppulong x, ucppulong y, ucppulong *q, ucppulong *r)
{
	int i, j;

	*q = to_ulong(0);
	for (i = 2 * LONGSIZE - 1; i >= 0; i --) {
		if (i >= LONGSIZE && (y.msw & (1UL << (i - LONGSIZE)))) break;
		if (i < LONGSIZE && (y.lsw & (1UL << i))) break;
	}
	if (i < 0) {
		error(eval_line, "division by 0");
		throw(eval_exception);
	}
	for (j = 2 * LONGSIZE - 1 - i; j >= 0; j --) {
		ucppulong a = op_lsh_u(y, j);

		if (op_leq_u(a, x)) {
			x = op_minus_u(x, a);
			if (j >= LONGSIZE)
				q->msw |= (1UL << (j - LONGSIZE));
			else
				q->lsw |= (1UL << j);
		}
	}
	*r = x;
}

static ucppulong op_slash_u(ucppulong x, ucppulong y)
{
	ucppulong q, r;

	udiv(x, y, &q, &r);
	return q;
}

static ucppulong op_pct_u(ucppulong x, ucppulong y)
{
	ucppulong q, r;

	udiv(x, y, &q, &r);
	return r;
}


static ucppslong op_star_s(ucppslong x, ucppslong y)
{
	ucppulong a = x, b = y, c;
	int xn = 0, yn = 0;

	if (SIGNBIT(x.msw)) { a = op_neg_u(x); xn = 1; }
	if (SIGNBIT(y.msw)) { b = op_neg_u(y); yn = 1; }
	c = op_star_u(a, b);
	/* Turbo C seems to miscompile the ?: operators when operands
	   are ulong/slong structures */
	if (xn ^ yn) return op_neg_u(c);
	return c;
}

static void sdiv(ucppslong x, ucppslong y, ucppslong *q, ucppslong *r)
{
	ucppulong a = x, b = y, c, d;
	int xn = 0, yn = 0;

	if (SIGNBIT(x.msw)) { a = op_neg_u(x); xn = 1; }
	if (SIGNBIT(y.msw)) { b = op_neg_u(y); yn = 1; }
	udiv(a, b, &c, &d);
	if (xn ^ yn) *q = op_neg_u(c); else *q = c;
	if (xn ^ yn) *r = op_neg_u(d); else *r = d;
}

static ucppslong op_slash_s(ucppslong x, ucppslong y)
{
	ucppslong q, r;

	sdiv(x, y, &q, &r);
	return q;
}

static ucppslong op_pct_s(ucppslong x, ucppslong y)
{
	ucppslong q, r;

	sdiv(x, y, &q, &r);
	return r;
}

#define op_plus_s(x, y)		op_plus_u(x, y)
#define op_minus_s(x, y)	op_minus_u(x, y)
#define op_neg_s(x)		op_neg_u(x)
#define op_lsh_s(x, y)		op_lsh_u(x, y)

/*
 * What happens if x represents a negative value, is implementation
 * specified. We emit a warning, and extend the sign (which is what
 * most implementations do).
 */
static ucppslong op_rsh_s(ucppslong x, int y)
{
	int xn = (SIGNBIT(x.msw) != 0);
	ucppulong q = op_rsh_u(x, y);

	if (xn && y > 0) {
		if (emit_eval_warnings)
			warning(eval_line, "right shift of a signed negative "
				"value in #if");
		q = op_or_u(q, op_lsh_u(op_not_u(to_ulong(0)),
			2 * LONGSIZE - y));
	}
	return q;
}

static int op_lt_s(ucppslong x, ucppslong y)
{
	int xn = (SIGNBIT(x.msw) != 0);
	int yn = (SIGNBIT(y.msw) != 0);

	if (xn && !yn) return 1;
	if (!xn && yn) return 0;
	if (xn) return op_lt_u(op_neg_u(y), op_neg_u(x));
	return op_lt_u(x, y);
}

static int op_leq_s(ucppslong x, ucppslong y)
{
	int xn = (SIGNBIT(x.msw) != 0);
	int yn = (SIGNBIT(y.msw) != 0);

	if (xn && !yn) return 1;
	if (!xn && yn) return 0;
	if (xn) return op_leq_u(op_neg_u(y), op_neg_u(x));
	return op_leq_u(x, y);
}

#define op_gt_s(x, y)		op_lt_s(y, x)
#define op_geq_s(x, y)		op_leq_s(y, x)
#define op_same_s(x, y)		op_same_u(x, y)
#define op_neq_s(x, y)		(!op_same_s(x, y))

#define op_and_s(x, y)		op_and_u(x, y)
#define op_circ_s(x, y)		op_circ_u(x, y)
#define op_or_s(x, y)		op_or_u(x, y)

#define op_lval_s(x)		op_neq_s(x, to_slong(0))
#define op_lnot_s(x)		(!op_lval_s(x))
#define op_not_s(x)		op_not_u(x)
#define op_uplus_s(x)		(x)
#define op_uminus_s(x)		op_neg_s(x)

#define op_promo(x)		(x)
#define back_ulong(x)		((x).lsw)

#endif

typedef struct {
	int sign;
	union {
		ucppulong uv;
		ucppslong sv;
	} u;
} ppval;

static int boolval(ppval x)
{
	return x.sign ? op_lval_s(x.u.sv) : op_lval_u(x.u.uv);
}

#if !defined(WCHAR_SIGNEDNESS)
#  if CHAR_MIN == 0
#    define WCHAR_SIGNEDNESS	0
#  else
#    define WCHAR_SIGNEDNESS	1
#  endif
#endif

/*
 * Check the suffix, return 1 if it is signed, 0 otherwise.
 * u, l, ll, ul, lu, ull, llu, and variations with uppercase letters
 * are legal, no suffix either; other suffixes are not legal.
 */
static int pp_suffix(char *d, char *refc)
{
	if (!*d) return 1;
	if (*d == 'u' || *d == 'U') {
		d ++;
		if (!*d) return 0;
		if (*d == 'l' || *d == 'L') {
			d ++;
			if (*d == 'l' || *d == 'L') d ++;
			if (!*d) return 0;
			goto suffix_error;
		}
		goto suffix_error;
	}
	if (*d == 'l' || *d == 'L') {
		d ++;
		if (*d == 'l' || *d == 'L') d ++;
		if (!*d) return 1;
		if (*d == 'u' || *d == 'U') {
			d ++;
			if (!*d) return 0;
		}
		goto suffix_error;
	}
suffix_error:
	error(eval_line, "invalid integer constant '%s'", refc);
	throw(eval_exception);
	return 666;
}

static ppval pp_decconst(char *c, int sign, char *refc)
{
	ppval q;

	q.sign = sign;
	if (q.sign) {
		q.u.sv = to_slong(0);
		for (; DECIM(*c); c ++) {
			q.u.sv = op_star_s(q.u.sv, to_slong(10));
			q.u.sv = op_plus_s(q.u.sv, to_slong(DVAL(*c)));
		}
		if (HEXAD(*c) || *c == 'x' || *c == 'X') goto const_error;
	} else {
		q.u.uv = to_ulong(0);
		for (; DECIM(*c); c ++) {
			q.u.uv = op_star_u(q.u.uv, to_ulong(10));
			q.u.uv = op_plus_u(q.u.uv, to_ulong(DVAL(*c)));
		}
		if (HEXAD(*c) || *c == 'x' || *c == 'X') goto const_error;
	}
	return q;
const_error:
	error(eval_line, "invalid integer constant '%s'", refc);
	throw(eval_exception);
	return q;
}

static ppval pp_octconst(char *c, int sign, char *refc)
{
	ppval q;

	q.sign = sign;
	if (q.sign) {
		q.u.sv = to_slong(0);
		for (; OCTAL(*c); c ++) {
			q.u.sv = op_star_s(q.u.sv, to_slong(8));
			q.u.sv = op_plus_s(q.u.sv, to_slong(OVAL(*c)));
		}
		if (HEXAD(*c) || *c == 'x' || *c == 'X') goto const_error;
	} else {
		q.u.uv = to_ulong(0);
		for (; OCTAL(*c); c ++) {
			q.u.uv = op_star_u(q.u.uv, to_ulong(8));
			q.u.uv = op_plus_u(q.u.uv, to_ulong(OVAL(*c)));
		}
		if (HEXAD(*c) || *c == 'x' || *c == 'X') goto const_error;
	}
	return q;
const_error:
	error(eval_line, "invalid integer constant '%s'", refc);
	throw(eval_exception);
	return q;
}

static ppval pp_hexconst(char *c, int sign, char *refc)
{
	ppval q;

	q.sign = sign;
	if (q.sign) {
		q.u.sv = to_slong(0);
		for (; HEXAD(*c); c ++) {
			q.u.sv = op_star_s(q.u.sv, to_slong(16));
			q.u.sv = op_plus_s(q.u.sv, to_slong(HVAL(*c)));
		}
		if (HEXAD(*c) || *c == 'x' || *c == 'X') goto const_error;
	} else {
		q.u.uv = to_ulong(0);
		for (; HEXAD(*c); c ++) {
			q.u.uv = op_star_u(q.u.uv, to_ulong(16));
			q.u.uv = op_plus_u(q.u.uv, to_ulong(HVAL(*c)));
		}
		if (HEXAD(*c) || *c == 'x' || *c == 'X') goto const_error;
	}
	return q;
const_error:
	error(eval_line, "invalid integer constant '%s'", refc);
	throw(eval_exception);
	return q;
}

static unsigned long pp_char(char *c, char *refc)
{
	unsigned long r = 0;

	c ++;
	if (*c == '\\') {
		int i;

		c ++;
		switch (*c) {
		case 'n': r = '\n'; c ++; break;
		case 't': r = '\t'; c ++; break;
		case 'v': r = '\v'; c ++; break;
		case 'b': r = '\b'; c ++; break;
		case 'r': r = '\r'; c ++; break;
		case 'f': r = '\f'; c ++; break;
		case 'a': r = '\a'; c ++; break;
		case '\\': r = '\\'; c ++; break;
		case '\?': r = '\?'; c ++; break;
		case '\'': r = '\''; c ++; break;
		case '\"': r = '\"'; c ++; break;
		case 'u':
			for (i = 0, c ++; i < 4 && HEXAD(*c); i ++, c ++) {
				r = (r * 16) + HVAL(*c);
			}
			if (i != 4) {
				error(eval_line, "malformed UCN in %s", refc);
				throw(eval_exception);
			}
			break;
		case 'U':
			for (i = 0, c ++; i < 8 && HEXAD(*c); i ++, c ++) {
				r = (r * 16) + HVAL(*c);
			}
			if (i != 8) {
				error(eval_line, "malformed UCN in %s", refc);
				throw(eval_exception);
			}
			break;
		case 'x':
			for (c ++; HEXAD(*c); c ++) r = (r * 16) + HVAL(*c);
			break;
		default:
			if (OCTAL(*c)) {
				r = OVAL(*(c ++));
				if (OCTAL(*c)) r = (r * 8) + OVAL(*(c ++));
				if (OCTAL(*c)) r = (r * 8) + OVAL(*(c ++));
			} else {
				error(eval_line, "invalid escape sequence "
					"'\\%c'", *c);
				throw(eval_exception);
			}
		}
	} else if (*c == '\'') {
		error(eval_line, "empty character constant");
		throw(eval_exception);
	} else {
		r = *((unsigned char *)(c ++));
	}

	if (transient_characters && r < 256) {
		r = transient_characters[(size_t)r];
	}

	if (*c != '\'' && emit_eval_warnings) {
		warning(eval_line, "multicharacter constant");
	}
	return r;
}

static ppval pp_strtoconst(char *refc)
{
	ppval q;
	char *c = refc, *d;
	int sign = 1;

	if (*c == '\'' || *c == 'L') {
		q.sign = (*c == 'L') ? WCHAR_SIGNEDNESS : 1;
		if (*c == 'L') c ++;
		if (q.sign) {
			q.u.sv = to_slong(pp_char(c, refc));
		} else {
			q.u.uv = to_ulong(pp_char(c, refc));
		}
		return q;
	}
	for (d = c; *d; d ++) {
		if (!HEXAD(*d) && *d != 'x' && *d != 'X') {
			sign = pp_suffix(d, refc);
			break;
		}
	}
	if (*c == '0') {
		c ++;
		if (*c == 'x' || *c == 'X') {
			/* hexadecimal constant */
			c ++;
			return pp_hexconst(c, sign, refc);
		}
		return pp_octconst(c, sign, refc);
	}
	return pp_decconst(c, sign, refc);
}

/*
 * Used by #line directives -- anything beyond what can be put in an
 * unsigned long, is considered absurd.
 */
unsigned long strtoconst(char *c)
{
	ppval q = pp_strtoconst(c);

	if (q.sign) q.u.uv = op_promo(q.u.sv);
	return back_ulong(q.u.uv);
}

#define OP_UN(x)	((x) == LNOT || (x) == NOT || (x) == UPLUS \
			|| (x) == UMINUS)

static ppval eval_opun(int op, ppval v)
{
	if (op == LNOT) {
		v.sign = 1;
		v.u.sv = to_slong(op_lnot_s(v.u.sv));
		return v;
	}
	if (v.sign) {
		switch (op) {
		case NOT: v.u.sv = op_not_s(v.u.sv); break;
		case UPLUS: v.u.sv = op_uplus_s(v.u.sv); break;
		case UMINUS: v.u.sv = op_uminus_s(v.u.sv); break;
		}
	} else {
		switch (op) {
		case NOT: v.u.uv = op_not_u(v.u.uv); break;
		case UPLUS: v.u.uv = op_uplus_u(v.u.uv); break;
		case UMINUS: v.u.uv = op_uminus_u(v.u.uv); break;
		}
	}
	return v;
}

#define OP_BIN(x)	((x) == STAR || (x) == SLASH || (x) == PCT \
			|| (x) == PLUS || (x) == MINUS || (x) == LSH \
			|| (x) == RSH || (x) == LT || (x) == LEQ \
			|| (x) == GT || (x) == GEQ || (x) == SAME \
			|| (x) == NEQ || (x) == AND || (x) == CIRC \
			|| (x) == OR || (x) == LAND || (x) == LOR \
			|| (x) == COMMA)
static ppval eval_opbin(int op, ppval v1, ppval v2)
{
	ppval r;

	switch (op) {
	case STAR:	case SLASH:	case PCT:
	case PLUS:	case MINUS:	case AND:
	case CIRC:	case OR:
		/* promote operands, adjust signedness of result */
		if (!v1.sign || !v2.sign) {
			if (v1.sign) {
				v1.u.uv = op_promo(v1.u.sv);
				v1.sign = 0;
			} else if (v2.sign) {
				v2.u.uv = op_promo(v2.u.sv);
				v2.sign = 0;
			}
			r.sign = 0;
		} else {
			r.sign = 1;
		}
		break;
	case LT:	case LEQ:	case GT:
	case GEQ:	case SAME:	case NEQ:
		/* promote operands */
		if (!v1.sign || !v2.sign) {
			if (v1.sign) {
				v1.u.uv = op_promo(v1.u.sv);
				v1.sign = 0;
			} else if (v2.sign) {
				v2.u.uv = op_promo(v2.u.sv);
				v2.sign = 0;
			}
		}
		/* fall through */
	case LAND:
	case LOR:
		/* result is signed anyway */
		r.sign = 1;
		break;
	case LSH:
	case RSH:
		/* result is as signed as left operand; promote right
		   operand to unsigned (this is not required by the
		   standard) */
		r.sign = v1.sign;
		if (v2.sign) {
			v2.u.uv = op_promo(v2.u.sv);
			v2.sign = 0;
		}
		break;
	case COMMA:
		if (emit_eval_warnings) {
			warning(eval_line, "ISO C forbids evaluated comma "
				"operators in #if expressions");
		}
		r.sign = v2.sign;
		break;
#ifdef AUDIT
	default: ouch("a good operator is a dead operator");
#endif
	}

#define SBINOP(x)	if (r.sign) r.u.sv = op_ ## x ## _s(v1.u.sv, v2.u.sv); \
			else r.u.uv = op_ ## x ## _u(v1.u.uv, v2.u.uv);

#define NSSBINOP(x)	if (v1.sign) r.u.sv = to_slong(op_ ## x ## _s(v1.u.sv, \
			v2.u.sv)); else r.u.sv = to_slong(op_ ## x ## _u \
			(v1.u.uv, v2.u.uv));

#define LBINOP(x)	if (v1.sign) r.u.sv = to_slong(op_lval_s(v1.u.sv) x \
			op_lval_s(v2.u.sv)); else r.u.sv = \
			to_slong(op_lval_u(v1.u.uv) x op_lval_u(v2.u.uv));

#define ABINOP(x)	if (r.sign) r.u.sv = op_ ## x ## _s(v1.u.sv, \
			back_ulong(v2.u.uv)); else r.u.uv = \
			op_ ## x ## _u(v1.u.uv, back_ulong(v2.u.uv));

	switch (op) {
	case STAR: SBINOP(star); break;
	case SLASH: SBINOP(slash); break;
	case PCT: SBINOP(pct); break;
	case PLUS: SBINOP(plus); break;
	case MINUS: SBINOP(minus); break;
	case LSH: ABINOP(lsh); break;
	case RSH: ABINOP(rsh); break;
	case LT: NSSBINOP(lt); break;
	case LEQ: NSSBINOP(leq); break;
	case GT: NSSBINOP(gt); break;
	case GEQ: NSSBINOP(geq); break;
	case SAME: NSSBINOP(same); break;
	case NEQ: NSSBINOP(neq); break;
	case AND: SBINOP(and); break;
	case CIRC: SBINOP(circ); break;
	case OR: SBINOP(or); break;
	case LAND: LBINOP(&&); break;
	case LOR: LBINOP(||); break;
	case COMMA: r = v2; break;
	}
	return r;
}

#define ttOP(x)		(OP_UN(x) || OP_BIN(x) || (x) == QUEST || (x) == COLON)

static int op_prec(int op)
{
	switch (op) {
	case LNOT:
	case NOT:
	case UPLUS:
	case UMINUS:
		return 13;
	case STAR:
	case SLASH:
	case PCT:
		return 12;
	case PLUS:
	case MINUS:
		return 11;
	case LSH:
	case RSH:
		return 10;
	case LT:
	case LEQ:
	case GT:
	case GEQ:
		return 9;
	case SAME:
	case NEQ:
		return 8;
	case AND:
		return 7;
	case CIRC:
		return 6;
	case OR:
		return 5;
	case LAND:
		return 4;
	case LOR:
		return 3;
	case QUEST:
		return 2;
	case COMMA:
		return 1;
	}
#ifdef AUDIT
	ouch("an unknown species should have a higher precedence");
#endif
	return 666;
}

/*
 * Perform the hard work of evaluation.
 *
 * This function works because:
 * -- all unary operators are right to left associative, and with
 *    identical precedence
 * -- all binary operators are left to right associative
 * -- there is only one non-unary and non-binary operator: the quest-colon
 *
 * If do_eval is 0, the evaluation of operators is not done. This is
 * for sequence point operators (&&, || and ?:).
 */
static ppval eval_shrd(struct token_fifo *tf, int minprec, int do_eval)
{
	ppval top;
	struct token *ct;

	top.sign = 1;
	if (tf->art == tf->nt) goto trunc_err;
	ct = tf->t + (tf->art ++);
	if (ct->type == LPAR) {
		top = eval_shrd(tf, 0, do_eval);
		if (tf->art == tf->nt) goto trunc_err;
		ct = tf->t + (tf->art ++);
		if (ct->type != RPAR) {
			error(eval_line, "a right parenthesis was expected");
			throw(eval_exception);
		}
	} else if (ct->type == NUMBER || ct->type == CHAR) {
		top = pp_strtoconst(ct->name);
	} else if (OP_UN(ct->type)) {
		top = eval_opun(ct->type, eval_shrd(tf,
			op_prec(ct->type), do_eval));
		goto eval_loop;
	} else if (ttOP(ct->type)) goto rogue_op_err;
	else {
		goto invalid_token_err;
	}

eval_loop:
	if (tf->art == tf->nt) {
		return top;
	}
	ct = tf->t + (tf->art ++);
	if (OP_BIN(ct->type)) {
		int bp = op_prec(ct->type);

		if (bp > minprec) {
			ppval tr;

			if ((ct->type == LOR && boolval(top))
				|| (ct->type == LAND && !boolval(top))) {
				tr = eval_shrd(tf, bp, 0);
				if (do_eval) {
					top.sign = 1;
					if (ct->type == LOR)
						top.u.sv = to_slong(1);
					if (ct->type == LAND)
						top.u.sv = to_slong(0);
				}
			} else {
				tr = eval_shrd(tf, bp, do_eval);
				if (do_eval)
					top = eval_opbin(ct->type, top, tr);
			}
			goto eval_loop;
		}
	} else if (ct->type == QUEST) {
		int bp = op_prec(QUEST);
		ppval r1, r2;

		if (bp >= minprec) {
			int qv = boolval(top);

			r1 = eval_shrd(tf, bp, qv ? do_eval : 0);
			if (tf->art == tf->nt) goto trunc_err;
			ct = tf->t + (tf->art ++);
			if (ct->type != COLON) {
				error(eval_line, "a colon was expected");
				throw(eval_exception);
			}
			r2 = eval_shrd(tf, bp, qv ? 0 : do_eval);
			if (do_eval) {
				if (qv) top = r1; else top = r2;
			}
			goto eval_loop;
		}
	}
	tf->art --;
	return top;

trunc_err:
	error(eval_line, "truncated constant integral expression");
	throw(eval_exception);
rogue_op_err:
	error(eval_line, "rogue operator '%s' in constant integral "
		"expression", operators_name[ct->type]);
	throw(eval_exception);
invalid_token_err:
	error(eval_line, "invalid token in constant integral expression");
	throw(eval_exception);
}

#define UNARY(x)	((x) != NUMBER && (x) != NAME && (x) != CHAR \
			&& (x) != RPAR)

/*
 * Evaluate the integer expression contained in the given token_fifo.
 * Evaluation is made by precedence of operators, as described in the
 * Dragon Book. The unary + and - are distinguished from their binary
 * counterparts using the Fortran way: a + or a - is considered unary
 * if it does not follow a constant, an identifier or a right parenthesis.
 */
unsigned long eval_expr(struct token_fifo *tf, int *ret, int ew)
{
	size_t sart;
	ppval r;

	emit_eval_warnings = ew;
	if (catch(eval_exception)) goto eval_err;
	/* first, distinguish unary + and - from binary + and - */
	for (sart = tf->art; tf->art < tf->nt; tf->art ++) {
		if (tf->t[tf->art].type == PLUS) {
			if (sart == tf->art || UNARY(tf->t[tf->art - 1].type))
				tf->t[tf->art].type = UPLUS;
		} else if (tf->t[tf->art].type == MINUS) {
			if (sart == tf->art || UNARY(tf->t[tf->art - 1].type))
				tf->t[tf->art].type = UMINUS;
		}
	}
	tf->art = sart;
	r = eval_shrd(tf, 0, 1);
	if (tf->art < tf->nt) {
		error(eval_line, "trailing garbage in constant integral "
			"expression");
		goto eval_err;
	}
	*ret = 0;
	return boolval(r);
eval_err:
	*ret = 1;
	return 0;
}
