static int mem[4096] = {0};
static int mpos = 0;
static int disk[8191] = {0};
static int dpos = 0;

int get_mem()
{
    static int init = 4095;
    if (init) return init--;
    else return (mpos) ? mem[mpos--] : 0;
}

void put_mem(int x)
{
    mem[++mpos] = x;
}

int get_disk()
{
    static int init = 4095;
    if (init) return init--;
    else return (dpos) ? disk[dpos--] : 0;
}

void put_disk(int x)
{
    disk[++dpos] = x;
}