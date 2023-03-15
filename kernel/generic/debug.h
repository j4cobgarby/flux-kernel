#ifndef __GENERIC_DEBUG_H__
#define __GENERIC_DEBUG_H__

#include "flux.h"

#include "generic/printk.h"

#define ASSERT(x) \
    if (!(x)) { \
        TRACE(PBAD("Assertion (" #x ") failed")); for(;;);\
    }

#define ERROR(msg) TRACE(PBAD(msg)); for(;;);

#define WARN(msg) printk(PWARN("??? " msg))

#define TRACE(msg) \
    printk("*** " msg " at %s:%d [%s]\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)

#endif