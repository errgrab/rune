#ifndef GLYPH_H
#define GLYPH_H

#include <stdint.h>

typedef uint8_t  u8;
typedef uint32_t u32;

typedef struct g g;
struct g {
	u8 *m;
	u32 l;
	u32 r[128];
	u32 p[256];
	bool halt;
};

void g_s(g *g);
void g_r(g *g);

#ifdef  G_IMPL

#define PC r['.']
#define STK r[',']
#define N m[PC++]
void g_s(g *g) {
	if (g->halt) return;
	u8 *r = g->r;
	u8 *m = g->m;
	u8 l = g->l;
	u8 op = N;
	switch (op) {
		/*alchemy:math*/
		case '+': u8 a=N,b=N,c=N; r[a] = r[b] + r[c]; break;
		case '-': u8 a=N,b=N,c=N; r[a] = r[b] - r[c]; break;
		case '*': u8 a=N,b=N,c=N; r[a] = r[b] * r[c]; break;
		case '/': u8 a=N,b=N,c=N; if (r[c]) r[a] = r[b] / r[c]; break;
		case '%': u8 a=N,b=N,c=N; if (r[c]) r[a] = r[b] % r[c]; break;
		/*shaping:bitwise*/
		case '&': u8 a=N,b=N,c=N; r[a] = r[b] & r[c]; break;
		case '|': u8 a=N,b=N,c=N; r[a] = r[b] | r[c]; break;
		case '^': u8 a=N,b=N,c=N; r[a] = r[b] ^ r[c]; break;
		case '~': u8 a=N,b=N; r[a] = ~r[b]; break;
		case '<': u8 a=N,b=N,c=N; r[a] = r[b] << r[c]; break;
		case '>': u8 a=N,b=N,c=N; r[a] = r[b] >> r[c]; break;
		/*imm:manifestation*/
		case ':': {
			u8 a=N,b=N;
			switch (b) {
				case '\'': r[a] = N; break;
				case 'd': r[a] = N - '0'; break;
				case 'x': u8 c = N;
					r[a] = (c>='0'&&c<='9')?c-'0':(c&0xDF)-'A'+10; break;
				case 'w':
					r[a] = (u32)N|((u32)N<<8)|((u32)N<<16)|((u32)N<<24); break;
				default: break;
			}
		} break;
		/*memory:substrate*/
		case '@': u8 a=N,b=N; r[a] = m[r[b]%l]; break;
		case '!': u8 a=N,b=N; m[r[a]%l] = r[b]; break;
		/*flow:ritual*/
		case '.': u8 a=N; PC = r[a]; break;
		case '?': {
			u8 a=N,b=N,c=N;
			switch (a) {
				case '!': if (!(r[b] != r[c])) PC++; break;
				case '=': if (!(r[b] == r[c])) PC++; break;
				case '>': if (!(r[b] > r[c])) PC++; break;
				case '<': if (!(r[b] < r[c])) PC++; break;
				default: break;
			}
		} break;
		case ';': u8 a=N; STK = (STK - 4) % l; m[STK] = PC; PC = r[a]; break;
		case ',': PC = m[STK]; STK = (STK + 4) % l; break;
		case '\0': g->halt = true; break;
		default: /*g->halt = true;*/ break;
	}
}

void g_r(g *g) {
	while (!g->halt) g_s(g);
}

#endif//G_IMPL

#endif//GLYPH_H
