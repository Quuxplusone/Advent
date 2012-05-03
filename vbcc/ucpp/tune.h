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

#ifndef UCPP__TUNE__
#define UCPP__TUNE__

/* ====================================================================== */
/*
 * Define LOW_MEM for low memory machines. It seems to improve performance
 * on larger systems, so this is on by default.
 */
#define LOW_MEM

/* ====================================================================== */
/*
 * Define AMIGA for systems using "drive letters" at the beginning of
 * some paths; define MSDOS on systems with drive letters and using
 * backslashes to seperate directory components. ATARI is same as MSDOS
 * but does not use %VAR% notation and Turbo C specific code.
 */
/* #define AMIGA */
/* #define MSDOS */
/* #define ATARI */

/* ====================================================================== */
/*
 * Buffering: there are two levels of buffering on input and output streams:
 * the standard libc buffering (manageable with setbuf() and setvbuf())
 * and some buffering provided by ucpp itself. The ucpp buffering uses
 * two buffers, of size respectively INPUT_BUF_MEMG and OUTPUT_BUF_MEMG
 * (as defined below).
 * You can disable one or both of these bufferings by defining the macros
 * NO_LIBC_BUF and NO_UCPP_BUF.
 *
 * Performance may vary, depending on the target architecture. On a
 * Linux/Alpha workstation, use both bufferings (that means, disable none
 * of them) for maximum performance. On a Minix-86 machine, disabling
 * ucpp buffering saves some memory and does not seem to impact performance.
 */
/* #define NO_LIBC_BUF */
/* #define NO_UCPP_BUF */

/*
 * On Unix stations, the system call mmap() might be used on input files.
 * This option is a subclause of ucpp internal buffering. On one station,
 * a 10% speed improvement was observe. Do not define this unless the
 * host architecture has the following characteristics:
 *  -- Posix compliance
 *  -- Text files correspond one to one with memory representation
 * If a file is not seekable or not mmapable, ucpp will revert to the
 * standard fread() solution.
 *
 * This feature is still considered beta quality. On some systems where
 * files can be bigger than memory address space (mainly, 32-bit systems
 * with files bigger than 4 GB), this option makes ucpp fail to operate
 * on those extremely large files.
 */
/* #define UCPP_MMAP */

/* To protect the innocent. */
#if defined(NO_UCPP_BUF) && defined(UCPP_MMAP)
#undef UCPP_MMAP
#endif

/* ====================================================================== */
/*
 * Define this if you want ucpp to generate tokenized PRAGMA tokens;
 * otherwise, it will generate raw string contents. This setting is
 * irrelevant to the stand-alone version of ucpp.
 */
/*#define PRAGMA_TOKENIZE*/

/*
 * Define this to the special character that marks the end of tokens with
 * a string value inside a tokenized PRAGMA token. The #pragma and _Pragma()
 * directives which use this character will be a bit more difficult to
 * decode (but ucpp will not mind). 0 cannot be used. '\n' is fine because
 * it cannot appear inside a #pragma or _Pragma(), since newlines cannot be
 * embedded inside tokens, neither directly nor by macro substitution and
 * stringization. Besides, '\n' is portable.
 */
#define PRAGMA_TOKEN_END	((unsigned char)'\n')

/*
 * Define this if you want ucpp to include encountered #pragma directives
 * in its output in non-lexer mode; _Pragma() are translated to equivalent
 * #pragma directives.
 */
#define PRAGMA_DUMP

/*
 * According to may interpretation of the C99 standard, _Pragma() are
 * evaluated wherever macro expansion could take place. However, Neil Booth,
 * whose mother language is English (contrary to me) and who is well aware
 * of the C99 standard (and especially the C preprocessor) told me that
 * it was unclear whether _Pragma() are evaluated inside directives such
 * as #if, #include and #line. If you want to disable the evaluation of
 * _Pragma() inside such directives, define the following macro.
 */
/* #define NO_PRAGMA_IN_DIRECTIVE */

/* ====================================================================== */
/*
 * Define INMACRO_FLAG to include two flags to the structure lexer_state,
 * that tell whether tokens come from a macro-replacement, and count those
 * macro-replacements.
 */
/* #define INMACRO_FLAG */

/* ====================================================================== */
/*
 * Paths where files are looked for by default, when #include is used.
 * Typical path is /usr/local/include and /usr/include, in that order.
 * If you want to set up no path, define the macro to 0.
 *
 * For Linux, get gcc includes too, or you will miss things like stddef.h.
 * The exact path varies much, depending on the distribution.
 */
#define STD_INCLUDE_PATH	"/usr/local/include", "/usr/include", \
	"/usr/lib/gcc-lib/alphaev56-unknown-linux-gnu/2.95.2/include"

/* ====================================================================== */
/*
 * For the evaluation of #if expression. If NATIVE_UINTMAX is defined,
 * the evaluation is done with NATIVE_UINTMAX as unsigned type, and
 * NATIVE_INTMAX as signed type. If NATIVE_UINTMAX is not defined but
 * SIMUL_UINTMAX is defined, evaluation is performed with a big integer
 * type, whom size is twice the size of unsigned long. If neither is
 * defined, ucpp will use uintmax_t and intmax_t if available, unsigned
 * long and long otherwise.
 * See the README file for details.
 */
/*
 * uncomment these two lines if you want evaluation with a "long long" type
#define NATIVE_UINTMAX	unsigned long long
#define NATIVE_INTMAX	long long
 */
/*
 * uncomment the following if you want evaluation with an emulated type
 * built with two "unsigned long".
#define SIMUL_UINTMAX
 */
/*
 * to force signedness of wide character constants, define WCHAR_SIGNEDNESS
 * to 0 for unsigned, 1 for signed.
#define WCHAR_SIGNEDNESS	0
 */

/* ====================================================================== */
/*
 * Standard assertions. They should include one cpu() assertion, one machine()
 * assertion (identical to cpu()), and one or more system() assertions.
 *
 * for Linux/PC:      cpu(i386),  machine(i386),  system(unix), system(linux)
 * for Linux/Alpha:   cpu(alpha), machine(alpha), system(unix), system(linux)
 * for Sparc/Solaris: cpu(sparc), machine(sparc), system(unix), system(solaris)
 *
 * These are only suggestions. On Solaris, machine() should be defined
 * for i386 or sparc. For cross-compilation, define assertions related
 * to the target architecture.
 *
 * If you want no standard assertion, define STD_ASSERT to 0.
 */
#define STD_ASSERT	"cpu(alpha)", "machine(alpha)", "system(unix)", \
			"system(linux)"

/* ====================================================================== */
/*
 * System predefined macros. Nothing really mandatory, but some programs
 * might rely on those.
 * Each string must be either "name" or "name=token-list". If you want
 * no predefined macro, define STD_MACROS to 0.
 */
#define STD_MACROS	"__linux", "__unix", "__alpha", \
			"__linux__", "__unix__", "__alpha__"

/* ====================================================================== */
/*
 * Default flags; HANDLE_ASSERTIONS is required for Solaris system headers.
 * See cpp.h for the definition of these flags.
 */
#define DEFAULT_CPP_FLAGS	(DISCARD_COMMENTS | WARN_STANDARD \
				| WARN_PRAGMA | FAIL_SHARP | MACRO_VAARG \
				| CPLUSPLUS_COMMENTS | LINE_NUM | TEXT_OUTPUT \
				| KEEP_OUTPUT | HANDLE_TRIGRAPHS \
				| HANDLE_ASSERTIONS)
#define DEFAULT_LEXER_FLAGS	(DISCARD_COMMENTS | WARN_STANDARD | FAIL_SHARP \
				| MACRO_VAARG | CPLUSPLUS_COMMENTS | LEXER \
				| HANDLE_TRIGRAPHS | HANDLE_ASSERTIONS)

/* ====================================================================== */
/*
 * Define this to use sigsetjmp()/siglongjmp() instead of setjmp()/longjmp().
 * This is non-ANSI, but recommended on AIX systems.
 */
/* #define POSIX_JMP */

/* ====================================================================== */
/*
 * Maximum value (plus one) of a character handled by the lexer; 128 is
 * alright for ASCII native source code, but 256 is needed for EBCDIC.
 * 256 is safe in both cases; you will have big problems if you set
 * this value to INT_MAX or above. On Minix-86 or Msdos (small memory
 * model), define MAX_CHAR_VAL to 128.
 *
 * Set MAX_CHAR_VAL to a power of two to increase lexing speed. Beware
 * that lexer.c defines a static array of size MSTATE * MAX_CHAR_VAL
 * values of type int (MSTATE is defined in lexer.c and is about 40).
 */
#define MAX_CHAR_VAL	256

/*
 * If you want some extra character to be considered as whitespace,
 * define this macro to that space. On ISO-8859-1 machines, 160 is
 * the code for the unbreakable space.
 */
/* #define UNBREAKABLE_SPACE	160 */

/*
 * If you want whitespace tokens contents to be recorded (making them
 * tokens with a string content), define this. The macro STRING_TOKEN
 * will be adjusted accordingly.
 * Without this option, whitespace tokens are not even returned by the
 * lex() function. This is irrelevant for the non-lexer mode (almost --
 * it might slow down a bit ucpp, and with this option, comments will be
 * kept inside #pragma directives).
 */
/* #define SEMPER_FIDELIS */

/* ====================================================================== */
/*
 * Some constants used for memory increment granularity. Increasing these
 * values reduces the number of calls to malloc() but increases memory
 * consumption.
 *
 * Values should be powers of 2.
 */

/* for cpp.c */
#define COPY_LINE_LENGTH	80
#define INPUT_BUF_MEMG		8192
#define OUTPUT_BUF_MEMG		8192
#define TOKEN_NAME_MEMG		64	/* must be at least 4 */
#define TOKEN_LIST_MEMG		32
#define INCPATH_MEMG		16
#define GARBAGE_LIST_MEMG	32
#define LS_STACK_MEMG		4
#define FNAME_MEMG		32

/* ====================================================================== */
/*
 * C90 does not know about the "inline" keyword, but C99 does know,
 * and some C90 compilers know it as an extension. This part detects
 * these occurrences.
 */

#ifndef INLINE

#if __STDC__ && __STDC_VERSION >= 199901L
/* this is a C99 compiler, keep inline unchanged */
#elif defined(__GNUC__)
/* this is GNU gcc; modify inline. The semantics is not identical to C99
   but the differences are irrelevant as long as inline functions are static */
#undef inline
#define inline __inline__
#elif defined(__DECC) && defined(__linux__)
/* this is Compaq C under Linux, use __inline__ */
#undef inline
#define inline __inline__
#else
/* unknown compiler -> deactivate inline */
#undef inline
#define inline
#endif

#else
/* INLINE has been set, use its value */
#undef inline
#define inline INLINE
#endif

#endif
