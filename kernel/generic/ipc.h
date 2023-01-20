#ifndef __GENERIC_IPC_H__
#define __GENERIC_IPC_H__

#include "flux.h"

#include <stdint.h>
#include <stddef.h>

typedef unsigned short ipc_endpoint_id;
typedef unsigned short ipc_msg_seq;

struct ipc_msg {
    ipc_msg_seq seq;

    union {
        ipc_endpoint_id from;
        ipc_endpoint_id to;
    };

    int code;
    void *data;
    size_t len;
};

#endif /* __GENERIC_IPC_H__ */