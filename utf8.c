#include "utf8.h"

static const int UTF8_MAX = 4;

struct utf8 {
    char mask;
    char lead;
    rune beg;
    rune end;
};

static struct utf8 utf8[UTF8_MAX] = {
    {0b01111111, 0b00000000, 0x00000000, 0x0000007f},
    {0b00011111, 0b11000000, 0x00000080, 0x000007ff},
    {0b00001111, 0b11100000, 0x00000800, 0x0000ffff},
    {0b00000111, 0b11110000, 0x00010000, 0x001fffff},
};

static int utf8_len(char c)
{
    int i;
    for (i = 0; i < UTF8_MAX; ++i) {
        if ((c & ~utf8[i].mask) == utf8[i].lead)
            break;
    }
    return i + 1;
}

int RuneLen(rune r)
{
    int i;
    for (i = 0; i < UTF8_MAX; ++i) {
        if ((r >= utf8[i].beg) && (r <= utf8[i].end))
            break;
    }
    return i + 1;
}

int RuneStart(char c)
{
    int len = utf8_len(c);
    return len <= UTF8_MAX;
}

rune RuneDecode(const char* s, int* len)
{
    rune r;
    int i;
    *len = utf8_len(s[0]);
    r = s[0] & utf8[*len - 1].mask;
    for (i = 1; i < *len; ++i) {
        r <<= 6;
        r |= (s[i] & 0x3f);
    }
    return r;
}

int RuneEncode(rune r, char* s)
{
    int len, sft, i;
    len = RuneLen(r);
    sft = 6 * (len - 1);
    s[0] = ((r >> sft) & utf8[len - 1].mask) | utf8[len - 1].lead;
    for (i = 1; i < len; ++i) {
        sft -= 6;
        s[i] = ((r >> sft) & 0x3f) | 0x80;
    }
    s[len] = '\0';
    return len;
}

int RuneCount(const char* s)
{
    int n;
    for (n = 0; *s; ++n)
        s += utf8_len(*s);
    return n;
}

int ValidString(const char* s)
{
    int len;
    while (*s) {
        len = utf8_len(*s);
        if (len > UTF8_MAX)
            return 0;
        s += len;
    }
    return 1;
}
