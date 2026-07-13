// SPDX-License-Identifier: BSD-3-Clause

#include "uart.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "comms/comms.h"
#include "pico.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "portmacro.h"
#include <hardware/regs/intctrl.h>
#include <hardware/structs/io_bank0.h>
#include <pico/platform/panic.h>
#include <stdbool.h>
#include <stddef.h>


enum driver_state {
    INITIALIZED,
    DEINITIALIZED,
};

/* Driver state machine
    INITIALIZED <-> DEINITIALIZED
*/

struct uart_description_t {
    struct uPipe_t* pipe;
    struct uart_opts_t opts;
    TaskHandle_t tx_task; // Pointer not needed since its a pointer type
    uart_inst_t* id;
    int irq;    
    enum driver_state state;
};

static struct uart_description_t uart0_desc = {
    NULL,
    {0, 0, 0, 0, 0, 0},
    NULL,
    NULL,
    UART0_IRQ,
    DEINITIALIZED
};

static struct uart_description_t uart1_desc = {
    NULL,
    {0, 0, 0, 0, 0, 0},
    NULL,
    NULL,
    UART1_IRQ,
    DEINITIALIZED
};


void uart0_irq_handler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (uart0_desc.state != INITIALIZED || uart0_desc.pipe == NULL) panic("UART0: Corrupted UART Driver state.");


    while (uart_is_readable(uart0)) {
        uint8_t ch = uart_getc(uart0);

        xStreamBufferSendFromISR(
            uart0_desc.pipe->rx,
            &ch,
            1,
            &xHigherPriorityTaskWoken
        );
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void uart1_irq_handler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (uart1_desc.state != INITIALIZED || uart1_desc.pipe == NULL) panic("UART0: Corrupted UART Driver state."); 

    while (uart_is_readable(uart1)) {
        uint8_t ch = uart_getc(uart1);

        xStreamBufferSendFromISR(
            uart1_desc.pipe->rx,
            &ch,
            1,
            &xHigherPriorityTaskWoken
        );
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void uart_tx_task(void* desc_ptr) {
    struct uart_description_t* desc = (struct uart_description_t*)desc_ptr;
    while (true) {
        if (desc->state != INITIALIZED || desc->pipe == NULL) panic("UART0: Corrupted UART0 Driver state."); 
        uint8_t byte;
        size_t bytes_read = xStreamBufferReceive(desc->pipe->tx, &byte, 1, 1);
        for (size_t i = 0; i < bytes_read; i++) {
            uart_putc_raw(desc->id, (char)byte);
        }
    }
}

// Checks if expected and actual baud rates are within tolerence (2% error). Returns true if it isn't within tolerence, false if it is.
bool baud_within_tolarence(uint expected, uint actual) {
    uint lower_bound = (expected / 100) * 98;
    uint upper_bound = (expected / 100) * 102;
    if ((lower_bound <= actual) && (actual <= upper_bound)) return false;
    return true;
}


bool uart_drv_init(struct uPipe_t* pipe, struct uart_opts_t opts, uart_inst_t* uart_inst) {
    struct uart_description_t* desc = NULL;
    if (uart_inst == uart1) desc = &uart1_desc;
    else if (uart_inst == uart0) desc = &uart0_desc;
    else return true;

    if (desc->state != DEINITIALIZED) return true; // Cannot initilize if we already are.

    uint baud_real = uart_init(uart_inst, opts.baud);
    if (baud_within_tolarence(opts.baud, baud_real)) {
        // UART did not init at our desired baud.
        uart_deinit(uart_inst);
        return true;
    }
    gpio_set_function(opts.Txpin, GPIO_FUNC_UART);
    gpio_set_function(opts.Rxpin, GPIO_FUNC_UART);


    uart_set_hw_flow(uart_inst, false, false);

    pipe->internal_data = desc;
    pipe->close = uart_drv_deinit;
    desc->pipe = pipe;
    desc->opts.Txpin = opts.Txpin;
    desc->opts.Rxpin = opts.Rxpin;
    desc->id = uart_inst;

    if (xTaskCreate(uart_tx_task, "uart_tx", configMINIMAL_STACK_SIZE, desc, HARDCOMM_DEFAULT_PRIORITY, &desc->tx_task) != pdPASS) {
        // Failed to create TX drainer.
        gpio_set_function(opts.Txpin, GPIO_FUNC_NULL);
        gpio_set_function(opts.Rxpin, GPIO_FUNC_NULL);

        desc->pipe = NULL;

        uart_deinit(uart_inst);

        return true;
    }

    uart_set_fifo_enabled(uart_inst, true);
    
    uart_set_format(uart_inst, opts.data_bits, opts.stop_bits, opts.parity_bits);

    irq_set_priority(desc->irq, (1 << 7));

    if (uart_inst == uart0) irq_set_exclusive_handler(desc->irq, uart0_irq_handler);
    if (uart_inst == uart1) irq_set_exclusive_handler(desc->irq, uart1_irq_handler);

    irq_set_enabled(desc->irq, true);

    uart_set_irq_enables(uart_inst, true, false);
    desc->state = INITIALIZED;
    return false;
}



// deinits and garbage collects the UART driver.
void uart_drv_deinit(struct uPipe_t* consumed) {
    struct uart_description_t* desc = (struct uart_description_t*)consumed->internal_data;
    irq_set_enabled(desc->irq, false);
    if (desc->state == DEINITIALIZED) return; // no-op 

    uart_set_irq_enables(desc->id, false, false);
    vTaskDelete(desc->tx_task);
    desc->tx_task = NULL;
    desc->pipe = NULL;


    gpio_set_function(desc->opts.Rxpin, GPIO_FUNC_NULL);
    gpio_set_function(desc->opts.Txpin, GPIO_FUNC_NULL);

    uart_deinit(desc->id);
    desc->state = DEINITIALIZED;
}


