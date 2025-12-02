#ifndef BASE_H
#define BASE_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

typedef void     u0;
typedef uint8_t  u8;
typedef int8_t   i8;
typedef uint16_t u16;
typedef int16_t  i16;
typedef uint32_t u32;
typedef int32_t  i32;
typedef uint64_t u64;
typedef int64_t  i64;
typedef float    f32;
typedef double   f64;
typedef size_t   usz;
typedef ssize_t  isz;

typedef u0  *ptr;
typedef u0  *u0p;
typedef u8  *u8p;
typedef i8  *i8p;
typedef u16 *u16p;
typedef i16 *i16p;
typedef u32 *u32p;
typedef i32 *i32p;
typedef u64 *u64p;
typedef i64 *i64p;
typedef f32 *f32p;
typedef f64 *f64p;
typedef usz *uszp;
typedef isz *iszp;

typedef char *cstr;

#define _RESET "\x1b[0m"
#define _GRAY "\x1b[0;36m"
#define _RED "\x1b[0;31m"
#define _GREEN "\x1b[0;32m"
#define _ORANGE "\x1b[0;33m"

typedef enum exit_e exit_t;
enum exit_e {
	ERR = -1,
	OK,
};

u0 b_logerr_v(const cstr fmt, va_list args);
u0 base__assert(const cstr expr, const cstr file, const u32 line, const cstr fmt, ...);
#define b_assert(expr, ...) (void)((!!(expr)) || \
		(base__assert(#expr, __FILE__, __LINE__, __VA_ARGS__), 0))

#ifdef BASE_IMPL
#undef BASE_IMPL

u0 b_logerr_v(const cstr fmt, va_list args) {
	fprintf(stderr, "%s[ERROR]: ", _RED);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "%s\n", _RESET);
}

u0 base__assert(const cstr expr, const cstr file, const u32 line, const cstr fmt, ...) {
	fprintf(stderr, "%sAssertion failed: %s, file %s, line %u %s\n",
			_RED, expr, file, line, _RESET);
	if (fmt) {
		va_list args;
		va_start(args, fmt);
		b_logerr_v(fmt, args);
		va_end(args);
	}
	exit(ERR);
}

#endif // BASE_IMPL

#endif // BASE_H
