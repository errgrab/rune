#ifndef RUNE_H
#define RUNE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef uint8_t  u8;
typedef uint32_t u32;
typedef bool b;
typedef u8   r;
typedef u32  w;

// main rune structure
typedef struct vsl vsl;
struct vsl {
	r *a; // aether
	w m, s[128], r[256]; // magnitude, spirits, resonance
	b f; // flow
};

// main interface
void rune_cast(vsl *v);
void rune_learn(vsl *v, r *book);
void rune_incant(vsl *v);

#endif//RUNE_H

#ifdef RUNE_IMPL

w v_r(vsl *v, w p) {
	if (p + 4 > r->m) return 0;
	return (w)r->a[p]|
		((w)r->a[p+1]<<8)|
		((w)r->a[p+2]<<16)|
		((w)r->a[p+3]<<24);
}

void v_w(vsl *v, u32 pos, u32 val) {
	if (pos + 4 > r->m) return;
	r->a[pos]     = (u8)(val & 0xff);
	r->a[pos + 1] = (u8)((val >> 8) & 0xff);
	r->a[pos + 2] = (u8)((val >> 16) & 0xff);
	r->a[pos + 3] = (u8)((val >> 24) & 0xff);
}

/*
 * ISA:
 * Types:
 * - Monad  .    (.)
 * - Diad   .a   (a = .a)
 * - Triad  .ab  (a = a . b)
 * - Tetrad .abc (a = b . c)
 * Spells:
 * - Alchemy + - * / %
 * - Shaping & | ^ ~ { }
 * - Divination = > <
 * - Teleportation . ? ; ,
 * - Manifestation :
 * - Void ! @
 * - Portals ( )
 */
void rune_cast(rune *r) {
	if (!r->f) return;
	switch (op) {
		/*Alchemy*/
		case '+': WA(RB + RC); break;
		case '-': WA(RB - RC); break;
		case '*': WA(RB * RC); break;
		case '/': if (RC) WA(RB / RC); break;
		case '%': if (RC) WA(RB % RC); break;
		/*Shaping*/
		case '&': WA(RB & RC); break;
		case '|': WA(RB | RC); break;
		case '^': WA(RB ^ RC); break;
		case '!': WA(~RB); break;
		/*Manifestation*/
		case '#': WA(PC); WPC(PC + 4); break;
		/*Void*/
		case '@': WA(FETCH(RB)); break;
		case '$': STORE(RA, RB); break;
		/*Teleportation*/
		case ';': PUSH(PC); WPC(RA); break;
		case ',': WPC(POP); break;
		/*Divination*/
		case '=': if (!(RA == RB)) WPC(PC + 4); break;
		case '>': if (!(RA > RB)) WPC(PC + 4); break;
		case '<': if (!(RA < RB)) WPC(PC + 4); break;
		/**/
		case '~': if (r->syscall) r->syscall(r, RA, RB, RC); break;
		case 0: r->halt = true; break;
		default: /*r->halt = true;*/ break;
	}
}

void rune_run(rune *r) {
	while (r->f) rune_step(r);
}

#endif // RUNE_IMPL
