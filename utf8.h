#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t rune;

void RuneDecodeString(const char *s, rune *out);
rune RuneDecode(const char *s, int *len);
int RuneEncode(rune r, char *s);
int RuneCount(const char *s);
int RuneLen(rune r);
int RuneStart(char c);
int ValidString(const char *s);

#ifdef __cplusplus
}
#endif
