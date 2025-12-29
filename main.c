#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#define RUNE_IMPL
#include "rune.h"

enum {
	OK,
	ERR,
};

enum {
	FD_IN,
	FD_OUT,
	FD_ERR,
};

void dump_mem(u8 *mem, size_t size) {
	bool first_line = true;
	while (size) {
		size_t left = (size >= 8) ? 8 : size;
		printf("%x ", mem); // address
		u8 *tmp = mem;
		for (size_t i = 0; i < 8; i++) {
			if (i < left) printf("%02x ", *tmp++); // bytes
			else printf("   ");
		}
		printf(" |");
		for (size_t i = 0; i < left; i++) {
			printf("%c", isprint(*mem) ? *mem : '.'); // chars
			mem++;
		}
		for (size_t i = left; i < 8; i++) printf(" ");
		printf("|\n");
		size -= left;
		if (!first_line && left == 8) {
			bool skipped = false;
			while (size >= 8 && 0 == memcmp(mem, mem - left, left)) {
				mem += 8;
				size -= 8;
				skipped = true;
			}
			if (skipped) printf("*\n"); // repeating lines
		}
		first_line = false;
	}
}

int main(void) {
	u8 memory[0x10000] = {0};
	g g = {
		.m = memory;
		.l = sizeof(memory);
		.halt = false;
	};
	dump_mem(memory, sizeof(memory));
	rune_run(&vm);
	return (OK);
}

