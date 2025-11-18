#ifndef TIME_H
#define TIME_H

#include <stdint.h>

// Timerinitialisatie en interrupt handler
void init_timer(uint32_t frequency);
void timer_handler(void);

// Slaapfuncties
void sleep_ms(uint32_t ms);
void sleep(uint32_t ms); // alias

// Debugfuncties
uint64_t ticks_read(void);
uint32_t timer_hz(void);


#endif /* TIME_H */
