#include <stdio.h>
#define BASE_IMPL
#include "base.h"

typedef struct mem_s mem_t;
typedef mem_t *mem_p;
struct mem_s {
	usz size;
	u8p data;
};

mem_p mem_new(usz size) {
	mem_p mem = calloc(1, sizeof(*mem));
	b_assert(mem);
	mem->size = size;
	mem->data = calloc(size, sizeof(*mem->data));
	b_assert(mem->data);
	return (mem);
};

mem_p mem_del(mem_p mem) {
	free(mem);
	free(mem->data);
	return (NULL);
}

usz mem_sizeof(mem_p mem) {
	return (mem->size);
}

mem_p mem_resize(mem_p mem) {
}

mem_p program() {
	mem_p mem_new(1024);
}

i32 main() {
	return (OK);
}
