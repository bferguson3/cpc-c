void main(void)
{
    unsigned char* c = 0xc000;
    *c = 0xff;
    while(1){}
}