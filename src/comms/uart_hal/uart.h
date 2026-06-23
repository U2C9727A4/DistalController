#ifndef UARTHAL_H
#define UARTHAL_H
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stream_buffer.h"
#include <hardware/uart.h>


// Initilize UART HAL. rx is produced on, and tx is consumed from.
// Mutable borrows rx and tx stream buffers. (they MUST be uninitialized!)
// on failure, returns true.
// on success, the UART HAL owns the producing end of rx and consuming end of tx and the lifetime of these buffers (An internal reference is kept.)
extern bool uart_hal_init(StreamBufferHandle_t *rx, StreamBufferHandle_t *tx, uart_inst_t* uartid, uint uartirq);

// Deinitilize UART HAL.
// Returns true on error.
extern bool uart_hal_deinit();

// Suspends the UART HAL.
// returns true if suspension fails.
extern bool uart_hal_suspend();


// Resumes the UART HAL after suspension.
// Returns true if resume fails.
extern bool uart_hal_resume();

#endif