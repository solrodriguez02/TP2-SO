#ifndef INTERRUPS_H_
#define INTERRUPS_H_

extern void _irq00Handler(void);
extern void _irq01Handler(void);
extern void _irq02Handler(void);
extern void _irq03Handler(void);
extern void _irq04Handler(void);
extern void _irq05Handler(void);
extern void _irq08Handler(void);
extern void _irq08init(void);

extern int syscallsHandler(void);

extern void _exception0Handler(void);
extern void _exception6Handler(void);

void _cli(void);

void _sti(void);

void _hlt(void);

void picMasterMask(uint8_t mask);

void picSlaveMask(uint8_t mask);

void haltcpu(void);

#endif