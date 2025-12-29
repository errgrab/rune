/*
 * GLYPH - A character-based register VM
 * 
 * A minimalist virtual machine with character opcodes inspired by UXN.
 * Features 128 registers, 256 I/O ports, and memory-mapped operations.
 * 
 * Architecture:
 *   - 128 32-bit registers (r[0-127])
 *   - Special registers: r['.'] = PC, r[','] = Stack Pointer
 *   - 256 I/O ports for device communication
 *   - Memory: byte-addressable, size must be power-of-2
 *   - All operations use register operands (register-based VM)
 * 
 * Instruction Format:
 *   [opcode] [arg1] [arg2] [arg3]
 *   Arguments are register indices (ASCII characters)
 * 
 * Opcode Categories:
 *   Alchemy (Math):        + - * / %
 *   Shaping (Bitwise):     & | ^ ~ < >
 *   Manifestation (Imm):   : (with modifiers ', d, x, w)
 *   Substrate (Memory):    @ !
 *   Portals (I/O):         ( )
 *   Ritual (Control Flow): . ? ; ,
 * 
 * Usage:
 *   #define G_IMPL before including to get the implementation
 *   g_init() to initialize, g_load() to load program, g_r() to run
 */
#ifndef GLYPH_H
#define GLYPH_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

typedef uint8_t  u8;
typedef uint32_t u32;

/* Error codes */
enum g_error {
	G_OK = 0,
	G_ERR_BOUNDS = 1,      /* PC out of bounds */
	G_ERR_DIV_ZERO = 2,    /* division by zero */
	G_ERR_INVALID_OP = 3,  /* invalid opcode */
};

typedef struct g g;
struct g {
	u8 *m;      /* memory buffer (byte-addressable) */
	u32 l;      /* memory length (must be power-of-2) */
	u32 r[128]; /* registers: r[0-31] general, r[32-127] char-indexed */
	u32 p[256]; /* I/O ports for device communication */
	bool halt;  /* halt flag - stops execution */
	u32 err;    /* error code from g_error enum */
	bool trace; /* trace execution flag for debugging */
};

/* Public API */
void g_s(g *g);          /* Execute single step */
void g_r(g *g);          /* Run until halt */
void g_init(g *vm, u8 *mem, u32 size);      /* Initialize VM */
void g_load(g *vm, const u8 *prog, u32 len); /* Load program */
void g_reset(g *vm);     /* Reset VM state */
const char* g_error_str(u32 err);            /* Get error string */

/* Debug API */
void g_dump_regs(g *vm);                    /* Dump registers */
void g_dump_mem(g *vm, u32 start, u32 len); /* Dump memory range */
void g_trace_inst(g *vm, u8 op, u32 pc);    /* Trace instruction */

#ifdef  G_IMPL

#define PC r['.']
#define STK r[',']
#define NEXT() (PC >= g->l ? (g->halt = true, g->err = G_ERR_BOUNDS, 0) : m[PC++])
void g_s(g *g) {
	if (g->halt) return;
	u32 *r = g->r;
	u8 *m = g->m;
	u32 l = g->l;
	u32 pc_before = PC;
	u8 op = NEXT();
	if (g->halt) return; /* check bounds */
	
	if (g->trace) g_trace_inst(g, op, pc_before);
	
	u8 a=0,b=0,c=0;
	switch (op) {
		/* Alchemy: Math operations - r[a] = r[b] OP r[c] */
		case '+': a=NEXT(),b=NEXT(),c=NEXT(); r[a] = r[b] + r[c]; break;
		case '-': a=NEXT(),b=NEXT(),c=NEXT(); r[a] = r[b] - r[c]; break;
		case '*': a=NEXT(),b=NEXT(),c=NEXT(); r[a] = r[b] * r[c]; break;
		case '/': a=NEXT(),b=NEXT(),c=NEXT(); 
			if (r[c]) r[a] = r[b] / r[c]; 
			else { g->halt = true; g->err = G_ERR_DIV_ZERO; }
			break;
		case '%': a=NEXT(),b=NEXT(),c=NEXT(); 
			if (r[c]) r[a] = r[b] % r[c]; 
			else { g->halt = true; g->err = G_ERR_DIV_ZERO; }
			break;
		
		/* Shaping: Bitwise operations */
		case '&': a=NEXT(),b=NEXT(),c=NEXT(); r[a] = r[b] & r[c]; break;
		case '|': a=NEXT(),b=NEXT(),c=NEXT(); r[a] = r[b] | r[c]; break;
		case '^': a=NEXT(),b=NEXT(),c=NEXT(); r[a] = r[b] ^ r[c]; break;
		case '~': a=NEXT(),b=NEXT(); r[a] = ~r[b]; break;
		case '<': a=NEXT(),b=NEXT(),c=NEXT(); r[a] = r[b] << r[c]; break;
		case '>': a=NEXT(),b=NEXT(),c=NEXT(); r[a] = r[b] >> r[c]; break;
		
		/* Manifestation: Immediate value loading */
		case ':': {
			a=NEXT(),b=NEXT();
			switch (b) {
				case '\'': r[a] = NEXT(); break;                     /* raw byte */
				case 'd': r[a] = NEXT() - '0'; break;                /* decimal digit */
				case 'x': c = NEXT();                                /* hex digit */
					r[a] = (c>='0'&&c<='9')?c-'0':(c&0xDF)-'A'+10; break;
				case 'w': {                                          /* word (4 bytes, little-endian) */
					u8 b0=NEXT(), b1=NEXT(), b2=NEXT(), b3=NEXT();
					r[a] = (u32)b0|((u32)b1<<8)|((u32)b2<<16)|((u32)b3<<24);
				} break;
				default: break;
			}
		} break;
		
		/* Substrate: Memory operations */
		case '@': a=NEXT(),b=NEXT(); r[a] = m[r[b] & (l-1)]; break;  /* load byte */
		case '!': a=NEXT(),b=NEXT(); m[r[a] & (l-1)] = r[b]; break;  /* store byte */
		
		/* Portals: I/O operations */
		case '(': a=NEXT(),b=NEXT(); r[a] = g->p[r[b]]; break;  /* read from port */
		case ')': a=NEXT(),b=NEXT(); g->p[r[a]] = r[b]; break;  /* write to port */
		
		/* Ritual: Control flow */
		case '.': a=NEXT(); PC = r[a]; break;  /* jump to address in r[a] */
		case '?': {  /* conditional skip next instruction */
			a=NEXT(),b=NEXT(),c=NEXT();
			switch (a) {
				case '!': if (!(r[b] != r[c])) PC++; break;  /* skip if not equal */
				case '=': if (!(r[b] == r[c])) PC++; break;  /* skip if equal */
				case '>': if (!(r[b] > r[c])) PC++; break;   /* skip if greater */
				case '<': if (!(r[b] < r[c])) PC++; break;   /* skip if less */
				default: break;
			}
		} break;
		case ';': /* call: push PC and jump to r[a] */
			a=NEXT();
			STK = (STK - 4) & (l-1);
			/* Store 4 bytes of return address (little-endian) */
			m[STK] = PC & 0xFF;
			m[(STK+1)&(l-1)] = (PC >> 8) & 0xFF;
			m[(STK+2)&(l-1)] = (PC >> 16) & 0xFF;
			m[(STK+3)&(l-1)] = (PC >> 24) & 0xFF;
			PC = r[a];
			break;
		case ',': /* return: pop PC from stack */
			/* Load 4 bytes of return address (little-endian) */
			PC = (u32)m[STK] |
			     ((u32)m[(STK+1)&(l-1)] << 8) |
			     ((u32)m[(STK+2)&(l-1)] << 16) |
			     ((u32)m[(STK+3)&(l-1)] << 24);
			STK = (STK + 4) & (l-1);
			break;
		
		/* Termination and errors */
		case '\0': g->halt = true; break;  /* normal halt */
		default: g->halt = true; g->err = G_ERR_INVALID_OP; break;  /* invalid opcode */
	}
}

void g_r(g *g) {
	while (!g->halt) g_s(g);
}

/* Initialize VM with memory buffer */
/* Note: size should be a power of 2 for optimal performance */
void g_init(g *vm, u8 *mem, u32 size) {
	memset(vm, 0, sizeof(g));
	memset(mem, 0, size);
	vm->m = mem;
	vm->l = size;
	vm->halt = false;
	vm->err = 0;
	vm->trace = false;
}

/* Load program into memory and reset PC */
void g_load(g *vm, const u8 *prog, u32 len) {
	if (len > vm->l) len = vm->l;
	memcpy(vm->m, prog, len);
	vm->r['.'] = 0; /* reset PC */
	vm->halt = false;
	vm->err = 0;
}

/* Reset VM state without clearing memory */
void g_reset(g *vm) {
	for (int i = 0; i < 128; i++) {
		vm->r[i] = 0;
	}
	for (int i = 0; i < 256; i++) {
		vm->p[i] = 0;
	}
	vm->halt = false;
	vm->err = 0;
}

/* Get error string */
const char* g_error_str(u32 err) {
	switch (err) {
		case G_OK: return "No error";
		case G_ERR_BOUNDS: return "PC out of bounds";
		case G_ERR_DIV_ZERO: return "Division by zero";
		case G_ERR_INVALID_OP: return "Invalid opcode";
		default: return "Unknown error";
	}
}

/* Dump registers */
void g_dump_regs(g *vm) {
	printf("Registers:\n");
	for (int i = 0; i < 128; i++) {
		if (vm->r[i] != 0) {
			if (i >= 32 && i <= 126) {
				printf("  r['%c'] = 0x%08x (%u)\n", i, vm->r[i], vm->r[i]);
			} else {
				printf("  r[%3d] = 0x%08x (%u)\n", i, vm->r[i], vm->r[i]);
			}
		}
	}
	printf("  PC = %u, STK = 0x%x, halt = %d, err = %u (%s)\n",
		vm->r['.'], vm->r[','], vm->halt, vm->err, g_error_str(vm->err));
}

/* Dump memory */
void g_dump_mem(g *vm, u32 start, u32 len) {
	if (start >= vm->l) return;
	if (start + len > vm->l) len = vm->l - start;
	
	printf("Memory [0x%04x - 0x%04x]:\n", start, start + len - 1);
	for (u32 i = 0; i < len; i += 16) {
		printf("  %04x: ", start + i);
		for (u32 j = 0; j < 16 && i + j < len; j++) {
			printf("%02x ", vm->m[start + i + j]);
		}
		printf("\n");
	}
}

/* Trace instruction */
void g_trace_inst(g *vm, u8 op, u32 pc) {
	(void)vm; /* unused */
	printf("[%04x] %02x '%c'\n", pc, op, 
		(op >= 32 && op <= 126) ? op : '?');
}

#endif//G_IMPL

#endif//GLYPH_H
