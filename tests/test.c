int* pointer()
{
    return 0;
}

void main()
{
    int o;
    int p;
    o = p = 10;
    
    short s;
    s = 7000;

    short s2;
    s2 = s;

    int* ptr;
    ptr = &o;
    *ptr = 100;
    ptr = pointer(10);
}