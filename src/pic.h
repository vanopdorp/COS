// pic.h
#ifndef PIC_H
#define PIC_H

#include <stdint.h>

void remap_pic();
void pic_send_eoi(unsigned char irq);

#endif
