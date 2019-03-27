const int a = 42;
int b = 1;

int func(int *parg)
{
    static int s = 0;
    int r;

    if (s == 0) {
        r = *parg + a;
        s = 1;
    } else {
        r = *parg + b;
        b++;
    }

    return r;
}