#ifndef __GENERIC__PRINTK_H__
#define __GENERIC__PRINTK_H__

#include "flux.h"

#include <stdarg.h>

#ifdef __ARCH_X64__
#include "arch/x64/serial.h"
#define KPUTC(ch) com_send(*primary_com_port, ch)
#endif

#ifdef PRINTK_COLOUR
#define PSTD(s) "\x1b[37m" s
#define PGOOD(s) "\x1b[32m" s "\x1b[37m"
#define PWARN(s) "\x1b[33m" s "\x1b[37m"
#define PBAD(s) "\x1b[31m" s "\x1b[37m"
#define PACCENT(s) "\x1b[36m" s "\x1b[37m"
#else
#define PSTD(s) s
#define PGOOD(s) s
#define PWARN(s) s
#define PBAD(s) s
#define PACCENT(s) s
#endif

void kputs(const char *restrict str);
void kputhex(uint64_t x, int leading, int spacers);
void kputdec(uint64_t x);

void printk(const char *restrict fmt, ...);

#endif /* __GENERIC__PRINTK_H__ */