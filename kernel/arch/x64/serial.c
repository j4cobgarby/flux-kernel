#include "serial.h"
#include "io.h"
#include "flux.h"

#include <stdarg.h>
#include <stdint.h>

struct com_port com_ports[COM_PORT_COUNT] = {
    INIT_COM_PORT(0x3f8),
    INIT_COM_PORT(0x2f8),
    INIT_COM_PORT(0x3e8),
    INIT_COM_PORT(0x2e8),
    INIT_COM_PORT(0x5f8),
    INIT_COM_PORT(0x4f8),
    INIT_COM_PORT(0x5e8),
    INIT_COM_PORT(0x4e8),
};

struct com_port *primary_com_port = NULL;

void serial_init() {
    for (int i = 0; i < COM_PORT_COUNT; i++) {
        if (com_port_setup(&com_ports[i], 3) == 0) {
            if (!primary_com_port) primary_com_port = &(com_ports[i]);

            // Say about the working com port to the screen
            LIMINE_WRITE("Found working UART\n", 19);

            // And send a message to the com port too
            com_printf(com_ports[i], "Flux detected you as a working UART com port (IO base %x)\n", 
                com_ports[i].io_port_base);
        }
    }

    com_printf(*primary_com_port, "#### Flux serial console initialised!\n");
}

int com_port_setup(struct com_port *port, uint16_t divisor) {
    outb(port->io_port_base + COM_PORT_OFFSET_INT_ENABLE, 0x00);

    // The most significant bit of the line control register is the DLAB bit.
    // When this is set, register offsets +0 and +1 are used to control the
    // baud rate division. We want to do this, so first we set DLAB:
    outb(port->io_port_base + COM_PORT_OFFSET_LINE_CTRL, 0x80);

    // And then split the 16 bit divisor value across the two 8 bit fields.
    outb(port->io_port_base + COM_PORT_OFFSET_BAUD_DIVISOR_LOW, divisor & 0xff);
    outb(port->io_port_base + COM_PORT_OFFSET_BAUD_DIVISOR_HIGH, (divisor >> 8) & 0xff);

    // Now we use the LINE_CTRL register to set the parameters of the serial.
    outb(port->io_port_base + COM_PORT_OFFSET_LINE_CTRL, SET_LINE_CTRL(8, STOP_BITS_1, PARITY_NONE));
    outb(port->io_port_base + COM_PORT_OFFSET_INT_ID, 0xc7);
    outb(port->io_port_base + COM_PORT_OFFSET_MODEM_CTRL, 0x0b);

    // Now set the controller to loopback mode, so we can test if this port works.
    outb(port->io_port_base + COM_PORT_OFFSET_MODEM_CTRL, 0x1e);

    // Send an arbitrary test byte to it.
    outb(port->io_port_base + COM_PORT_OFFSET_DATA, 0xc4);

    // If, when we read data back, we get the byte we just sent, then the port
    // is fine.
    if (inb(port->io_port_base + COM_PORT_OFFSET_DATA) != 0xc4) {
        port->status = COM_PORT_STATUS_BROKEN;
        return -1;
    } else {
        port->status = COM_PORT_STATUS_READY;

        outb(port->io_port_base + COM_PORT_OFFSET_MODEM_CTRL, 0x0f);
        return 0;
    }
}

static int can_send(struct com_port port) {
    return inb(port.io_port_base + COM_PORT_OFFSET_LINE_STAT) & LINE_STAT_THRE;
}

static int can_read(struct com_port port) {
    return inb(port.io_port_base + COM_PORT_OFFSET_LINE_STAT) & LINE_STAT_DR;
}

void com_send(struct com_port port, char c) {
    if (port.status == COM_PORT_STATUS_READY) {
        while (!can_send(port));

        outb(port.io_port_base + COM_PORT_OFFSET_DATA, c);
    }
}

void com_sends(struct com_port port, char *s) {
    for (; *s; s++) {
        com_send(port, *s);
    }
}

char com_read(struct com_port port) {
    if (port.status == COM_PORT_STATUS_READY) {
        while (!can_read(port));

        return inb(port.io_port_base + COM_PORT_OFFSET_DATA);
    } 
    return 0;
}

void com_reads(struct com_port port, char *buff, char delim) {
    char c;

    while ((c = com_read(port)) != delim) {
        *buff++ = c;
    }
}

void com_printhex(struct com_port port, uint64_t x, int leading, int spacers) {
    static char hexdigits[] = "0123456789abcdef";

    for (int i = 15; i >= 0; i--) {
        int ind = (x >> 4 * i) & 0xf;

        if (ind && !leading) leading = 1;
        if (leading) {
            if (spacers && i != 15 && (i+1) % 4 == 0) com_send(port, ',');
            com_send(port, hexdigits[ind]);
        }
    }
}

void com_printdec(struct com_port port, int64_t x) {
    // 20 chars is enough to fit the largest 64 bit integer
    char buf[20] = {0};
    int leading = 0;

    for (int i = 19; x; x /= 10, i--) {
        buf[i] = x % 10;
    }

    for (int i = 0; i < 20; i++) {
        if (buf[i] && !leading) leading = 1;
        if (leading || i == 19) {
            com_send(port, '0' + buf[i]);
        }
    }
}

void com_printf(struct com_port port, char *fmt, ...) {
    va_list ap;

    uint64_t u64_val;
    int64_t i64_val;
    char ch_val;
    char *sp_val;

    va_start(ap, fmt);
    while (*fmt) {
        if (*fmt != '%') {
            com_send(port, *fmt);
        } else {
            switch (*(++fmt)) {
            case '\0':
                return;
            case 'x':
                u64_val = va_arg(ap, uint64_t);
                com_printhex(port, u64_val, 0, 0);
                break;
            case 'd':
                i64_val = va_arg(ap, int64_t);
                com_printdec(port, i64_val);
                break;
            case 'u':
                u64_val = va_arg(ap, uint64_t);
                com_printdec(port, u64_val);
                break;
            case 'p':
                u64_val = va_arg(ap, uint64_t);
                com_printhex(port, u64_val, 1, 1);
                break;
            case 'c':
                ch_val = (char)va_arg(ap, int);
                com_send(port, ch_val);
                break;
            case 's':
                sp_val = va_arg(ap, char*);
                com_sends(port, sp_val);
                break;
            default:
                break;
            }
        }
        fmt++;
    }
}
