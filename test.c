#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define G_IMPL
#include "glyph.h"

/* Test framework */
static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
	static void test_##name(void); \
	static void run_test_##name(void) { \
		printf("Running test: %s...", #name); \
		tests_run++; \
		test_##name(); \
		tests_passed++; \
		printf(" PASS\n"); \
	} \
	static void test_##name(void)

#define ASSERT(cond) \
	do { \
		if (!(cond)) { \
			printf("\n  FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
			exit(1); \
		} \
	} while(0)

#define ASSERT_EQ(a, b) \
	do { \
		u32 _a = (a), _b = (b); \
		if (_a != _b) { \
			printf("\n  FAIL: %s:%d: %s == %s (%u != %u)\n", \
				__FILE__, __LINE__, #a, #b, _a, _b); \
			exit(1); \
		} \
	} while(0)

/* Tests */

TEST(init) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	ASSERT_EQ(vm.l, 256);
	ASSERT_EQ(vm.halt, false);
	ASSERT_EQ(vm.err, 0);
	ASSERT_EQ(vm.r['.'], 0);
}

TEST(add) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'5 :b'3 +cab */
	u8 prog[] = {
		':', 'a', '\'', 5,
		':', 'b', '\'', 3,
		'+', 'c', 'a', 'b',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['c'], 8);
	ASSERT(vm.halt);
}

TEST(sub) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'10 :b'3 -cab */
	u8 prog[] = {
		':', 'a', '\'', 10,
		':', 'b', '\'', 3,
		'-', 'c', 'a', 'b',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['c'], 7);
}

TEST(mul) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'6 :b'7 *cab */
	u8 prog[] = {
		':', 'a', '\'', 6,
		':', 'b', '\'', 7,
		'*', 'c', 'a', 'b',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['c'], 42);
}

TEST(div) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'20 :b'5 /cab */
	u8 prog[] = {
		':', 'a', '\'', 20,
		':', 'b', '\'', 5,
		'/', 'c', 'a', 'b',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['c'], 4);
}

TEST(bitwise_and) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'15 :b'7 &cab */
	u8 prog[] = {
		':', 'a', '\'', 15,
		':', 'b', '\'', 7,
		'&', 'c', 'a', 'b',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['c'], 7);
}

TEST(bitwise_or) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'12 :b'3 |cab */
	u8 prog[] = {
		':', 'a', '\'', 12,
		':', 'b', '\'', 3,
		'|', 'c', 'a', 'b',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['c'], 15);
}

TEST(bitwise_xor) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'12 :b'10 ^cab */
	u8 prog[] = {
		':', 'a', '\'', 12,
		':', 'b', '\'', 10,
		'^', 'c', 'a', 'b',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['c'], 6);
}

TEST(bitwise_not) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'5 ~ba */
	u8 prog[] = {
		':', 'a', '\'', 5,
		'~', 'b', 'a',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['b'], ~5u);
}

TEST(left_shift) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'5 :b'2 <cab */
	u8 prog[] = {
		':', 'a', '\'', 5,
		':', 'b', '\'', 2,
		'<', 'c', 'a', 'b',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['c'], 20);
}

TEST(right_shift) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'20 :b'2 >cab */
	u8 prog[] = {
		':', 'a', '\'', 20,
		':', 'b', '\'', 2,
		'>', 'c', 'a', 'b',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['c'], 5);
}

TEST(memory_write_read) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'100 :b'42 !ab @ca */
	u8 prog[] = {
		':', 'a', '\'', 100,
		':', 'b', '\'', 42,
		'!', 'a', 'b',  /* mem[100] = 42 */
		'@', 'c', 'a',  /* c = mem[100] */
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['c'], 42);
	ASSERT_EQ(mem[100], 42);
}

TEST(jump) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program structure:
	   0-3: :a'd1 - load 1 into a, then we'll multiply to get target
	        Actually, let's just use raw byte for clarity
	   0-3: :a'10 - load byte 10 into a (: a ' 10)
	   4-5: .a    - jump to address in a (offset 10)
	   6-9: :b'42 - should skip (4 bytes)
	   10-13: :c'99 - offset 10, should execute (4 bytes)
	   14: \0
	*/
	u8 prog[] = {
		':', 'a', '\'', 10,  /* load 10 into a (offset 0-3) */
		'.', 'a',            /* jump to address 10 (offset 4-5) */
		':', 'b', '\'', 42,  /* should skip (offset 6-9) */
		':', 'c', '\'', 99,  /* offset 10: should execute (offset 10-13) */
		'\0'                 /* offset 14 */
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['c'], 99);
	ASSERT_EQ(vm.r['b'], 0);  /* b should not be set */
}

TEST(conditional_skip_true) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'5 :b'5 ?=ab :c'1 :d'2 \0 */
	u8 prog[] = {
		':', 'a', '\'', 5,
		':', 'b', '\'', 5,
		'?', '=', 'a', 'b',  /* if a == b, don't skip */
		':', 'c', '\'', 1,   /* should execute */
		':', 'd', '\'', 2,
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['c'], 1);
	ASSERT_EQ(vm.r['d'], 2);
}

TEST(conditional_skip_false) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'5 :b'3 ?=ab \0 :d'2 \0
	   Since a != b, the condition is false, so skip the next byte (\0)
	   and continue to :d'2 */
	u8 prog[] = {
		':', 'a', '\'', 5,
		':', 'b', '\'', 3,
		'?', '=', 'a', 'b',  /* if a == b, don't skip; but a != b, so skip next */
		'\0',                /* this byte should be skipped */
		':', 'd', '\'', 2,   /* should execute */
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['d'], 2);
}

TEST(immediate_digit) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :ad5 */
	u8 prog[] = {
		':', 'a', 'd', '5',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['a'], 5);
}

TEST(immediate_hex) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :axF */
	u8 prog[] = {
		':', 'a', 'x', 'F',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['a'], 15);
}

TEST(immediate_word) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :aw<bytes for 0x12345678> */
	u8 prog[] = {
		':', 'a', 'w', 0x78, 0x56, 0x34, 0x12,
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['a'], 0x12345678);
}

TEST(io_ports) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Set port 5 to value 42 */
	vm.p[5] = 42;
	
	/* Program: :a'5 (ba :c'99 )ac */
	u8 prog[] = {
		':', 'a', '\'', 5,
		'(', 'b', 'a',   /* read from port[r[a]=5] into b */
		':', 'c', '\'', 99,
		')', 'a', 'c',   /* write r[c]=99 to port[r[a]=5] */
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT_EQ(vm.r['b'], 42);
	ASSERT_EQ(vm.p[5], 99);
}

TEST(error_div_by_zero) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'10 :b'0 /cab */
	u8 prog[] = {
		':', 'a', '\'', 10,
		':', 'b', '\'', 0,
		'/', 'c', 'a', 'b',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT(vm.halt);
	ASSERT_EQ(vm.err, G_ERR_DIV_ZERO);
}

TEST(error_invalid_opcode) {
	u8 mem[256];
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program with invalid opcode '#' */
	u8 prog[] = {
		'#',  /* invalid opcode */
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT(vm.halt);
	ASSERT_EQ(vm.err, G_ERR_INVALID_OP);
}

TEST(error_out_of_bounds) {
	u8 mem[16];  /* small memory */
	g vm;
	g_init(&vm, mem, sizeof(mem));
	
	/* Program: :a'100 .a - tries to jump beyond memory */
	u8 prog[] = {
		':', 'a', '\'', 100,
		'.', 'a',
		'\0'
	};
	g_load(&vm, prog, sizeof(prog));
	g_r(&vm);
	
	ASSERT(vm.halt);
	ASSERT_EQ(vm.err, G_ERR_BOUNDS);
}

int main(void) {
	printf("Running Glyph VM Tests\n");
	printf("======================\n\n");
	
	run_test_init();
	run_test_add();
	run_test_sub();
	run_test_mul();
	run_test_div();
	run_test_bitwise_and();
	run_test_bitwise_or();
	run_test_bitwise_xor();
	run_test_bitwise_not();
	run_test_left_shift();
	run_test_right_shift();
	run_test_memory_write_read();
	run_test_jump();
	run_test_conditional_skip_true();
	run_test_conditional_skip_false();
	run_test_immediate_digit();
	run_test_immediate_hex();
	run_test_immediate_word();
	run_test_io_ports();
	run_test_error_div_by_zero();
	run_test_error_invalid_opcode();
	run_test_error_out_of_bounds();
	
	printf("\n======================\n");
	printf("Tests: %d/%d passed\n", tests_passed, tests_run);
	
	return (tests_passed == tests_run) ? 0 : 1;
}
