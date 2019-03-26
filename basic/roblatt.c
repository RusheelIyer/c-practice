#include <stdio.h>

main() {

    printf("Aufgabe 1\n");
    int a = 0x4321;
    int b = 0x8765;

    printf("%x\n", a | b);
    a = 0x4231;
    printf("%x\n", a & b);

    a = 0x4321;
    b = 0x8675;
    printf("%x\n", a ^ b);
    printf("%x\n", ~a);

    a = 4321;
    b = 8765;
    printf("%d\n", a | b);

    a = 0x4321;
    printf("%x\n", a >> 2);
    printf("%x\n", a << 3);

    b = 1337;
    printf("%x\n", b << 2);

    b = 1324;
    printf("%x\n", b >> 3);

    printf("Aufgabe 2\n");
    int f = 8;
    int g = 5;
    int h = f & g;
    printf("%d\n", h);

    h = f | g;
    printf("%d\n", h);

    h = ~f | ~g;
    printf("%d\n", h);

    h = f << g;
    printf("%d\n", h);

    printf("Aufgabe 3\n");
    int z = 9;
    int x = ++z;
    int y = z--;
    printf("x = %d\n", x);
    printf("y = %d\n", y);
    printf("z = %d\n", z);
}
