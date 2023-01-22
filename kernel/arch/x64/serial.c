#include "flux.h"

#include "serial.h"
#include "generic/debug.h"
#include "io.h"

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

            // And send a message to the com port too
            printk(SYSTEM_SERIAL PGOOD("Detected as working UART com port (IO base %x)\n"), 
                com_ports[i].io_port_base);
        }
    }

    if (!primary_com_port) {
        for (;;) __asm__ volatile ("hlt");
    }

    printk(SYSTEM_SERIAL "Flux serial console initialised!\n");
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