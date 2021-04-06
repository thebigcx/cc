int* pointer()
{
    return 0;
}

int o, p;

void main()
{
    o = p = sizeof(long);
    
    int s, s2;
    s = (char)700;

    s2 = s;

    int* ptr;
    ptr = &o;
    *ptr = 100;
    ptr = pointer(10);

    int array[10];
    array[3] = 10;
}