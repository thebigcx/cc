int o, p;

int hello()
{
    return 1;
}

int* pointer()
{
    return 0;
}

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

    

    char* str;
    str = "Hello, world!";
    int h_char;
    h_char = 'H';
    h_char = str[0];

    int array[10];
    array[3] = 10;
}