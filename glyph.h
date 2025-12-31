/*
 * GLYPH - Single-header character-based VM (~100 lines)
 * Usage: #define GLYPH_IMPL before including in ONE .c file
 */
#ifndef GLYPH_H
#define GLYPH_H

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint8_t  u8;
typedef uint32_t u32;

/* Resonance */
typedef void (*GlyphRes)(u8 port);

typedef struct {
	u8 *mem;
	u8  sp;
	u32 size;
	u32 reg[128];
	u32 stk[256];
	u32 port[256];
	GlyphRes emit, sense;
	bool halt;
} Glyph;

void glyph_init(Glyph *vm, u8 *mem, u32 size);
void glyph_run(Glyph *vm);

/* ────────────────────────────────────────────────────────────────────────── */
#ifdef GLYPH_IMPL

#define R(x) vm->reg[(x) & 127]
#define M(x) vm->mem[(x) & (vm->size - 1)]
#define PC   R('.')

static inline u8 N(Glyph *vm) {
	return (PC < vm->size) ? vm->mem[PC++] : (vm->halt = 1, 0);
}

void glyph_run(Glyph *vm) {
	u8 op, a, b, c;
	while (!vm->halt) {
		op = N(vm);
		if (vm->halt) break;
		#ifdef DEBUG
		printf("OP: %c(%d) PC: %u\n", op, op, PC - 1);
		#endif
		switch (op) {
		/* Arithmetic: +abc -abc *abc /abc %abc */
		case '+': a=N(vm); b=N(vm); c=N(vm); R(a) = R(b) + R(c); break;
		case '-': a=N(vm); b=N(vm); c=N(vm); R(a) = R(b) - R(c); break;
		case '*': a=N(vm); b=N(vm); c=N(vm); R(a) = R(b) * R(c); break;
		case '/': a=N(vm); b=N(vm); c=N(vm); R(a) = R(c) ? R(b)/R(c) : 0; break;
		case '%': a=N(vm); b=N(vm); c=N(vm); R(a) = R(c) ? R(b)%R(c) : 0; break;

		/* Bitwise: &abc |abc ^abc ~ab <abc >abc */
		case '&': a=N(vm); b=N(vm); c=N(vm); R(a) = R(b) & R(c); break;
		case '|': a=N(vm); b=N(vm); c=N(vm); R(a) = R(b) | R(c); break;
		case '^': a=N(vm); b=N(vm); c=N(vm); R(a) = R(b) ^ R(c); break;
		case '~': a=N(vm); b=N(vm); R(a) = ~R(b); break;
		case '<': a=N(vm); b=N(vm); c=N(vm); R(a) = R(b) << R(c); break;
		case '>': a=N(vm); b=N(vm); c=N(vm); R(a) = R(b) >> R(c); break;

		/* Load: :.ab :'ab :0ab */
		case ':':
			a = N(vm); b = N(vm); c = N(vm);
			if      (a == '.') R(b) = R(c);
			else if (a == '\'') R(b) = c;
			else if (a == '0') R(b) = (c <= '9') ? c - '0' : (c | 32) - 'a' + 10;
			break;

		/* Memory: @<ab @>ab */
		case '@':
			a = N(vm); b = N(vm); c = N(vm);
			if      (a == '<') R(b) = M(R(c));
			else if (a == '>') M(R(b)) = R(c);
			break;

		/* Ports: #<ab #>ab (resonance) */
		case '#':
			a=N(vm); b=N(vm); c=N(vm);
			if (a == '<') {
				u8 p = R(c) & 255;
				if (vm->sense) vm->sense(p);
				R(b) = vm->port[p];
			} else if (a == '>') {
				u8 p = R(b) & 255;
				vm->port[p] = R(c);
				if (vm->emit) vm->emit(p);
			}
			break;

		/* Compare: ?ab sets r['?'] = flags(a,b) [bit0=eq, bit1=gt, bit2=lt] */
		case '?': {
			a = N(vm); b = N(vm);
			u32 va = R(a), vb = R(b);
			R('?') = (va == vb ? 1 : 0) | (va > vb ? 2 : 0) | (va < vb ? 4 : 0);
			break;
		}

		/* Label: 'a sets r[a] = PC (for backward jumps) */
		case '\'': a = N(vm); R(a) = PC; break;

		/* Block end markers: }a and ]a are 2-byte NOPs */
		case '}': case ']': N(vm); break;

		/* Jump backward: ..a .=a .!a .>a .<a (to r[a]) */
		case '.': {
			a = N(vm); b = N(vm);
			int cond = (a == '.') ||
			           (a == '=' && (R('?') & 1)) ||
			           (a == '!' && !(R('?') & 1)) ||
			           (a == '>' && (R('?') & 2)) ||
			           (a == '<' && (R('?') & 4));
			if (cond) PC = R(b);
			break;
		}

		/* Skip forward: {a (sets r[a]=PC, skips to }a), [=a [!a [>a [<a (conditional to ]a) */
		case '{': {
			b = N(vm);
			R(b) = PC;  /* record function entry point */
			u32 scan = PC;
			while (scan < vm->size - 1) {
				if (M(scan) == '}' && M(scan + 1) == b) { PC = scan + 2; break; }
				scan++;
			}
			break;
		}
		case '[': {
			a = N(vm); b = N(vm);
			int cond = (a == '=' && (R('?') & 1)) ||
			           (a == '!' && !(R('?') & 1)) ||
			           (a == '>' && (R('?') & 2)) ||
			           (a == '<' && (R('?') & 4));
			if (cond) {
				u32 scan = PC;
				while (scan < vm->size - 1) {
					if (M(scan) == ']' && M(scan + 1) == b) { PC = scan + 2; break; }
					scan++;
				}
			}
			break;
		}

		/* Call/Return: ;a , */
		case ';': a = N(vm); vm->stk[vm->sp++] = PC; PC = R(a); break;
		case ',': PC = vm->stk[--vm->sp]; break;

		case 0: vm->halt = 1; break;
		case ' ':
		case '\f':
		case '\n':
		case '\v':
		case '\r':
		case '\t': break;
		default: vm->halt = 1; break;
		}
	}
}

void glyph_init(Glyph *vm, u8 *mem, u32 size) {
	memset(vm, 0, sizeof(Glyph));
	vm->mem = mem;
	vm->size = size;
}

#undef R
#undef M
#undef PC

#endif /* GLYPH_IMPL */

#endif /* GLYPH_H */
