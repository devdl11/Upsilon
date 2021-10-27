#ifndef ION_DEVICE_BOOT_ISR_H
#define ION_DEVICE_BOOT_ISR_H

#ifdef __cplusplus
extern "C" {
#endif

void bf_abort();
void uf_abort();
void nmi_abort();
void start();
void abort();
void isr_systick();

#ifdef __cplusplus
}
#endif

#endif
