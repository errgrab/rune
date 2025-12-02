#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#define MEM_SIZE 0x10000

typedef size_t usz;
typedef ssize_t isz;
typedef uint8_t byte;
typedef uint64_t word;
typedef struct vm vm;
typedef struct instr instr;

enum {
	ERR = -1,
	OK,
};

struct instr {
	byte opcode;
	void (*exec)(vm*);
	instr *next;
};

struct vm {
	byte *mem;
	word reg[0x100];
	word stk[0x200];
	instr *isa;
};

vm *vm_new(usz mem_size) {
	vm *ret = calloc(sizeof *ret, 1);
	if (!ret) return (NULL);
	ret->mem = calloc(sizeof *ret->mem, mem_size);
	if (!ret->mem) return (free(ret), NULL);
	return (ret);
}

vm *vm_del(vm *vm, instr *(*isa_del)(instr*)) {
	free(vm->mem);
	if (vm->isa) isa_del(vm->isa);
	free(vm);
	return (NULL);
}

instr *instr_new(char opcode, void (*exec)(vm*)) {
	instr *ret = calloc(sizeof *ret, 1);
	if (!ret) return (NULL);
	ret->opcode = opcode;
	ret->exec = exec;
	return (ret);
}

instr *instr_add(instr *dest, instr *src) {

}

instr *instr_del(instr *ins) {
	if (ins->next) instr_del(ins->next);
	free(ins);
	return (NULL);
}

int main(void) {
	vm *makhina = vm_new(MEM_SIZE);
	if (!makhina) return (fprintf(stderr, "ERR: makhina allocation\n"), ERR);
	makhina->reg[0] = 0x100;
	makhina = vm_del(makhina, instr_del);
	printf("%d\n", 0x10000);
	return (OK);
}

void dump_mem(byte *mem, usz size) {
	bool first_line = true;
	while (size) {
		usz left = (size >= 8) ? 8 : size;
		printf("%x ", mem); // address
		byte *tmp = mem;
		for (usz i = 0; i < 8; i++) {
			if (i < left) printf("%02x ", *tmp++); // bytes
			else printf("   ");
		}
		printf(" |");
		for (usz i = 0; i < left; i++) {
			printf("%c", isprint(*mem) ? *mem : '.'); // chars
			mem++;
		}
		for (usz i = left; i < 8; i++) printf(" ");
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
