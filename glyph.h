/*
 * GLYPH - Single-header character-based VM (~100 lines)
 * Usage: #define GLYPH_IMPL before including in ONE .c file
 */
#ifndef GLYPH_H
#define GLYPH_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

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
    GlyphRes emit;
    GlyphRes sense;
    bool halt;
} Glyph;

void glyph_init(Glyph *vm, u8 *mem, u32 size);
void glyph_run(Glyph *vm);

/* ────────────────────────────────────────────────────────────────────────── */
#ifdef GLYPH_IMPL

#define R(x)  vm->reg[(x) & 127]
#define M(x)  vm->mem[(x) & (vm->size - 1)]
#define PC    R('.')

static inline u8 N(Glyph *vm) {
    return (PC < vm->size) ? vm->mem[PC++] : (vm->halt = 1, 0);
}

#include <stdio.h>
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

        /* Load: :agX :axF :a.b */
        case ':':
            a = N(vm); b = N(vm);
            if      (b == 'g') R(a) = N(vm);
            else if (b == 'x') { c = N(vm); R(a) = (c <= '9') ? c - '0' : (c | 32) - 'a' + 10; }
            else if (b == '.') R(a) = R(N(vm));
            break;

        /* Memory: @ab !ab */
        case '@': a=N(vm); b=N(vm); R(a) = M(R(b)); break;
        case '!': a=N(vm); b=N(vm); M(R(a)) = R(b); break;

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

        /* Control: .a ?=bct ;a , */
        case '.': a=N(vm); PC = R(a); break;
        case '?':
            a=N(vm); b=N(vm); c=N(vm);
            if ((a=='=' && R(b)==R(c)) || (a=='!' && R(b)!=R(c)) ||
                (a=='>' && R(b)>R(c))  || (a=='<' && R(b)<R(c))) PC = R(N(vm));
            else N(vm);
            break;
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
