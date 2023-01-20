#include "flux.h"

#include "printk.h"

void kputs(const char *restrict str) {
    for (; *str; str++) {
        KPUTC(*str);
    }
}

void kputhex(uint64_t x, int leading, int spacers) {
    static char hexdigits[] = "0123456789abcdef";

    for (int i = 15; i >= 0; i--) {
        int ind = (x >> 4 * i) & 0xf;

        if (ind && !leading) leading = 1;

        if (leading) {
            if (spacers && i != 15 && (i+1) % 4 == 0) KPUTC(',');
            KPUTC(hexdigits[ind]);
        }
    }
}

void kputdec(uint64_t x) {
    char buf[20] = {0};
    int leading = 0;

    for (int i = 19; x; x /= 10, i--) {
        buf[i] = x % 10;
    }

    for (int i = 0; i < 20; i++) {
        if (buf[i] && !leading) leading = 1;
        if (leading || i == 19) {
            KPUTC('0' + buf[i]);
        }
    }
}

void printk(const char *restrict fmt, ...) {
    va_list ap;

    uint64_t u64_val;
    int64_t i64_val;
    char ch_val;
    char *sp_val;

    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt != '%') {
            KPUTC(*fmt);
        } else {
            switch (*(++fmt)) {
            case '\0':
                return;
            case 'x':
                u64_val = va_arg(ap, uint64_t);
                kputhex(u64_val, 0, 0);
                break;
            case 'd':
                i64_val = va_arg(ap, int64_t);
                kputdec(i64_val);
                break;
            case 'u':
                u64_val = va_arg(ap, uint64_t);
                kputdec(u64_val);
                break;
            case 'p':
                u64_val = va_arg(ap, uint64_t);
                kputhex(u64_val, 1, 1);
                break;
            case 'c':
                ch_val = (char)va_arg(ap, int);
                KPUTC(ch_val);
                break;
            case 's':
                sp_val = va_arg(ap, char*);
                kputs(sp_val);
                break;
            default:
                break;
            }
        }
        fmt++;
    }
}