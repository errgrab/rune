/* Glyph VM tests */
#define GLYPH_IMPL
#include "glyph.h"
#include <stdio.h>

#define TEST(name) static void test_##name(void)
#define RUN(name) printf("%-20s", #name); test_##name(); printf("OK\n")
#define ASSERT(x) do { if(!(x)) { printf("FAIL: %s\n", #x); return; } } while(0)

static Glyph vm;
static uint8_t mem[256];

static void run(const char *prog) {
    glyph_init(&vm, mem, sizeof(mem));
    memcpy(mem, prog, strlen(prog) + 1);
    glyph_run(&vm);
}

TEST(arithmetic) {
    run(":ax5 :bx3 +cab -dab *eab /fab");
    ASSERT(vm.reg['c'] == 8);
    ASSERT(vm.reg['d'] == 2);
    ASSERT(vm.reg['e'] == 15);
    ASSERT(vm.reg['f'] == 1);
}

TEST(bitwise) {
    run(":axF :bx7 &cab |dab ^eab ~fa");
    ASSERT(vm.reg['c'] == 7);
    ASSERT(vm.reg['d'] == 15);
    ASSERT(vm.reg['e'] == 8);
    ASSERT(vm.reg['f'] == ~15u);
}

TEST(shifts) {
    run(":ax4 :bx2 <cab >dab");
    ASSERT(vm.reg['c'] == 16);
    ASSERT(vm.reg['d'] == 1);
}

TEST(memory) {
	run(":ag2 :bg* !ab @ca");
    ASSERT(vm.reg['c'] == '*');
    ASSERT(mem['2'] == '*');
}

TEST(ports) {
	run(":ax5 :bgc #>ab");
    ASSERT(vm.port[5] == 99);
	const char test[] = ":axa #<ba";
    glyph_init(&vm, mem, sizeof(mem));
    vm.port[10] = 77;
    memcpy(mem, test, sizeof(test));
    glyph_run(&vm);
    ASSERT(vm.reg['b'] == 77);
}

TEST(jump) {
	run(":jxf .j :ax1");
    ASSERT(vm.reg['a'] == 0);  /* skipped */
}

TEST(conditional_eq) {
    /* ?=bct: jump to R(t) if R(b) == R(c) */
    /* When equal: jump over ":rx9" to continue at ":ax1" */
    run(":jg\x1a :ax5 :bx5 ?=abj :rx9 :ax1");
    ASSERT(vm.reg['a'] == 1);   /* jumped, then set to 1 */
    ASSERT(vm.reg['r'] == 0);   /* skipped */

    /* When not equal: no jump, execute ":rx9" */
    run(":jg\x1a :ax5 :bx3 ?=abj :rx9 :ax1");
    ASSERT(vm.reg['a'] == 1);   /* still runs */
    ASSERT(vm.reg['r'] == 9);   /* not skipped */
}

TEST(conditional_neq) {
    /* ?!bct: jump if R(b) != R(c) */
    run(":jg\x19 :ax5 :bx3 ?!abj :rx9 :ax1");
    ASSERT(vm.reg['r'] == 0);   /* jumped over */
    ASSERT(vm.reg['a'] == 1);

    run(":jg\x19 :ax5 :bx5 ?!abj :rx9");
    ASSERT(vm.reg['r'] == 9);   /* no jump, executed */
}

TEST(conditional_gt) {
    /* ?>bct: jump if R(b) > R(c) */
    run(":jg\x18 :ax5 :bx3 ?>abj :rx9 :ax1");
    ASSERT(vm.reg['r'] == 0);   /* jumped over */

    run(":jg\x18 :ax3 :bx5 ?>abj :rx9");
    ASSERT(vm.reg['r'] == 9);   /* 3 not > 5, no jump */

    run(":jg\x18 :ax5 :bx5 ?>abj :rx9");
    ASSERT(vm.reg['r'] == 9);   /* 5 not > 5, no jump */
}

TEST(conditional_lt) {
    /* ?<bct: jump if R(b) < R(c) */
    run(":jg\x18 :ax3 :bx5 ?<abj :rx9 :ax1");
    ASSERT(vm.reg['r'] == 0);   /* jumped over */

    run(":jg\x18 :ax5 :bx3 ?<abj :rx9");
    ASSERT(vm.reg['r'] == 9);   /* 5 not < 3, no jump */
}

TEST(call_return) {
    /* ;a calls R(a), comma returns */
    /* Program: set j to subroutine addr, call it, subroutine sets r=7 and returns, then set a=1 */
    const char prog[] = ":jg\x13 :axf ;j :ax1 \0 :rxf ,";
    glyph_init(&vm, mem, sizeof(mem));
    memcpy(mem, prog, sizeof(prog));
    glyph_run(&vm);
    ASSERT(vm.reg['r'] == 15);  /* subroutine executed */
    ASSERT(vm.reg['a'] == 1);   /* returned and continued */
}

TEST(nested_calls) {
    /* Test nested calls: main -> sub1 -> sub2 -> return -> return */
    /* Layout: 
       0x00: :1g\x13 :2g\x21 ;1 :ax1 \0
       0x13: :bx2 ;2 :dx4 ,
       0x21: :cx3 ,
    */
    const char prog[] = 
        ":1g\x13 :2g\x21 ;1 :ax1 \0" /* main: call sub1, then a=1 */
        ":bx2 ;2 :dx4 ,"             /* sub1: b=2, call sub2, d=4, return */
        ":cx3 ,";                    /* sub2: c=3, return */
    glyph_init(&vm, mem, sizeof(mem));
    memcpy(mem, prog, sizeof(prog));
    glyph_run(&vm);
    ASSERT(vm.reg['a'] == 1);   /* after return from sub1 */
    ASSERT(vm.reg['b'] == 2);   /* set in sub1 before sub2 */
    ASSERT(vm.reg['c'] == 3);   /* set in sub2 */
    ASSERT(vm.reg['d'] == 4);   /* set in sub1 after sub2 */
}

TEST(copy) {
	run(":ag* :b.a");
    ASSERT(vm.reg['b'] == 42);
}

int main(void) {
    printf("Glyph VM Tests\n==============\n");
    RUN(arithmetic);
    RUN(bitwise);
    RUN(shifts);
    RUN(memory);
    RUN(ports);
    RUN(jump);
    RUN(conditional_eq);
    RUN(conditional_neq);
    RUN(conditional_gt);
    RUN(conditional_lt);
    RUN(call_return);
    RUN(nested_calls);
    RUN(copy);
    printf("==============\nAll tests passed.\n");
    return 0;
}
