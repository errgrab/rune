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
    run(":0a5 :0b3 +cab -dab *eab /fab");
    ASSERT(vm.reg['c'] == 8);
    ASSERT(vm.reg['d'] == 2);
    ASSERT(vm.reg['e'] == 15);
    ASSERT(vm.reg['f'] == 1);
}

TEST(bitwise) {
    run(":0aF :0b7 &cab |dab ^eab ~fa");
    ASSERT(vm.reg['c'] == 7);
    ASSERT(vm.reg['d'] == 15);
    ASSERT(vm.reg['e'] == 8);
    ASSERT(vm.reg['f'] == ~15u);
}

TEST(shifts) {
    run(":0a4 :0b2 <cab >dab");
    ASSERT(vm.reg['c'] == 16);
    ASSERT(vm.reg['d'] == 1);
}

TEST(memory) {
    run(":'a2 :'b* @>ab @<ca");
    ASSERT(vm.reg['c'] == '*');
    ASSERT(mem['2'] == '*');
}

TEST(ports) {
    run(":0a5 :'bc #>ab");
    ASSERT(vm.port[5] == 99);
    const char test[] = ":0aa #<ba";
    glyph_init(&vm, mem, sizeof(mem));
    vm.port[10] = 77;
    memcpy(mem, test, sizeof(test));
    glyph_run(&vm);
    ASSERT(vm.reg['b'] == 77);
}

TEST(jump) {
    /* Unconditional forward skip: {L skips to }L but saves L=PC */
    run("{E :0a9 }E :0a1");
    ASSERT(vm.reg['a'] == 1);  /* skipped over :0a9 */
    ASSERT(vm.reg['E'] == 2);  /* E captured the skip body address */
}

TEST(jump_then_execute) {
    /* {L skips and saves, then ..L jumps back to execute it */
    /* First pass: a=0, skip [=S, execute ..E to jump back */
    /* Inside E: set r=9, a=1, fall through }E */
    /* Second pass: a=1, i=1, equal, skip [=S over ..E, end */
    run(":0a0 :0i1 {E :0r9 :0a1 }E ?ai [=S ..E ]S");
    ASSERT(vm.reg['r'] == 9);
    ASSERT(vm.reg['a'] == 1);
}

TEST(backward_jump) {
    /* Backward jump with label: loop until counter equals 1 */
    run(":0c3 :0i1 'L -cci ?ci .!L :0r1");
    ASSERT(vm.reg['c'] == 1);
    ASSERT(vm.reg['r'] == 1);
}

TEST(conditional_eq) {
    /* [=S skips to ]S if equal */
    run(":0a5 :0b5 ?ab [=S :0r9 ]S :0r1");
    ASSERT(vm.reg['r'] == 1);  /* skipped :0r9 because a==b */

    run(":0a5 :0b3 ?ab [=S :0r9 ]S :0r1");
    ASSERT(vm.reg['r'] == 1);  /* didn't skip, r was set to 9 then 1 */
}

TEST(conditional_neq) {
    /* [!S skips to ]S if not equal */
    run(":0a5 :0b3 ?ab [!S :0r9 ]S :0r1");
    ASSERT(vm.reg['r'] == 1);  /* skipped :0r9 because a!=b */

    run(":0a5 :0b5 ?ab [!S :0r9 ]S :0r1");
    ASSERT(vm.reg['r'] == 1);  /* didn't skip, r was 9 then 1 */
}

TEST(conditional_gt) {
    /* [>S skips to ]S if greater */
    run(":0a5 :0b3 ?ab [>S :0r9 ]S :0r1");
    ASSERT(vm.reg['r'] == 1);  /* skipped because 5>3 */

    run(":0a3 :0b5 ?ab [>S :0r9 ]S :0r1");
    ASSERT(vm.reg['r'] == 1);  /* didn't skip */
}

TEST(conditional_lt) {
    /* [<S skips to ]S if less */
    run(":0a3 :0b5 ?ab [<S :0r9 ]S :0r1");
    ASSERT(vm.reg['r'] == 1);  /* skipped because 3<5 */

    run(":0a5 :0b3 ?ab [<S :0r9 ]S :0r1");
    ASSERT(vm.reg['r'] == 1);  /* didn't skip */
}

TEST(call_return) {
    /* {F sets r[F]=PC and skips to }F, ;F calls to r[F] */
    /* func F: r=5, return. main: set r=1, call F, set s=r+1 */
    run("{F :0r5 , }F :0r1 ;F :0i1 +sri");
    ASSERT(vm.reg['r'] == 5);  /* F set r=5 */
    ASSERT(vm.reg['s'] == 6);  /* s=r+i=5+1=6 */
}

TEST(nested_calls) {
    /* Two functions: F sets r=3, G calls F then adds 1 */
    run("{F :0r3 , }F {G ;F :0i1 +rri , }G :0r0 ;G");
    ASSERT(vm.reg['r'] == 4);  /* G: call F (r=3), r+=1 (r=4) */
}

TEST(copy) {
    run(":'a* :.ba");
    ASSERT(vm.reg['b'] == 42);
}

TEST(labels) {
    /* Test 'a label instruction: a = PC */
    run("'L :0a5");
    ASSERT(vm.reg['L'] == 2);  /* label captured PC after 'L */
}

int main(void) {
    printf("Glyph VM Tests\n==============\n");
    RUN(arithmetic);
    RUN(bitwise);
    RUN(shifts);
    RUN(memory);
    RUN(ports);
    RUN(jump);
    RUN(jump_then_execute);
    RUN(backward_jump);
    RUN(conditional_eq);
    RUN(conditional_neq);
    RUN(conditional_gt);
    RUN(conditional_lt);
    RUN(call_return);
    RUN(nested_calls);
    RUN(copy);
    RUN(labels);
    printf("==============\nAll tests passed.\n");
    return 0;
}
