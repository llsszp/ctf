#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[]) {
    printf("%p\n", argv[1]);

    __asm__("push %ebp");
    __asm__("push $0");
    printf("%1$#x\n");
    __asm__("pop %eax");
    __asm__("pop %eax");
    return 0;
}
