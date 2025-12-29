#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#define G_IMPL
#include "glyph.h"

/* Status codes */
enum {
	OK = 0,
	ERR = 1,
};

/* Example programs */
void demo_arithmetic(void) {
	printf("=== Arithmetic Demo ===\n");
	u8 memory[256];
	g vm;
	g_init(&vm, memory, sizeof(memory));
	
	/* Program: :a'10 :b'5 +cab -dab *eab /fab */
	u8 prog[] = {
		':', 'a', '\'', 10,
		':', 'b', '\'', 5,
		'+', 'c', 'a', 'b',  /* c = 10 + 5 = 15 */
		'-', 'd', 'a', 'b',  /* d = 10 - 5 = 5 */
		'*', 'e', 'a', 'b',  /* e = 10 * 5 = 50 */
		'/', 'f', 'a', 'b',  /* f = 10 / 5 = 2 */
		'\0'
	};
	
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	printf("a = %u, b = %u\n", vm.r['a'], vm.r['b']);
	printf("c (a+b) = %u\n", vm.r['c']);
	printf("d (a-b) = %u\n", vm.r['d']);
	printf("e (a*b) = %u\n", vm.r['e']);
	printf("f (a/b) = %u\n\n", vm.r['f']);
}

void demo_bitwise(void) {
	printf("=== Bitwise Demo ===\n");
	u8 memory[256];
	g vm;
	g_init(&vm, memory, sizeof(memory));
	
	/* Program: :a'12 :b'10 &cab |dab ^eab ~fa */
	u8 prog[] = {
		':', 'a', '\'', 12,   /* 1100 */
		':', 'b', '\'', 10,   /* 1010 */
		'&', 'c', 'a', 'b',   /* c = 12 & 10 = 8 (1000) */
		'|', 'd', 'a', 'b',   /* d = 12 | 10 = 14 (1110) */
		'^', 'e', 'a', 'b',   /* e = 12 ^ 10 = 6 (0110) */
		'~', 'f', 'a',        /* f = ~12 */
		'\0'
	};
	
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	printf("a = %u (0x%x), b = %u (0x%x)\n", vm.r['a'], vm.r['a'], vm.r['b'], vm.r['b']);
	printf("c (a&b) = %u\n", vm.r['c']);
	printf("d (a|b) = %u\n", vm.r['d']);
	printf("e (a^b) = %u\n", vm.r['e']);
	printf("f (~a) = 0x%08x\n\n", vm.r['f']);
}

void demo_io(void) {
	printf("=== I/O Demo ===\n");
	u8 memory[256];
	g vm;
	g_init(&vm, memory, sizeof(memory));
	
	/* Set some input values on ports */
	vm.p[0] = 42;
	vm.p[1] = 100;
	
	/* Program: :a'0 (ba :c'1 (dc +eba :f'2 )fe */
	u8 prog[] = {
		':', 'a', '\'', 0,
		'(', 'b', 'a',        /* read port 0 into b */
		':', 'c', '\'', 1,
		'(', 'd', 'c',        /* read port 1 into d */
		'+', 'e', 'b', 'd',   /* e = b + d */
		':', 'f', '\'', 2,
		')', 'f', 'e',        /* write e to port 2 */
		'\0'
	};
	
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	printf("Port 0 = %u\n", vm.p[0]);
	printf("Port 1 = %u\n", vm.p[1]);
	printf("Port 2 (sum) = %u\n\n", vm.p[2]);
}

int main(int argc, char **argv) {
	bool run_demos = (argc == 1);
	
	if (run_demos) {
		printf("GLYPH VM - Character-based Register VM\n");
		printf("======================================\n\n");
		
		demo_arithmetic();
		demo_bitwise();
		demo_io();
		
		printf("Run './vm --help' for usage information\n");
	} else {
		/* TODO: Add command-line interface for loading/running programs */
		printf("Usage: %s [options]\n", argv[0]);
		printf("Options:\n");
		printf("  --help    Show this help message\n");
		printf("  --version Show version information\n");
		printf("\nRun without arguments to see demos.\n");
	}
	
	return OK;
}


