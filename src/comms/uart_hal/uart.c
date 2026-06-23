#include "uart.h"
#include "FreeRTOS.h"
#include "pico.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "portmacro.h"
#include <hardware/regs/intctrl.h>
#include <hardware/structs/io_bank0.h>
#include <stdbool.h>
#include <stddef.h>


#define BAUDRATE 115200

#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY_BITS 0

#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define RX_BUFFER_SIZE 512
#define TX_BUFFER_SIZE 512

#define TX_TASK_PRIORITY 1

enum hal_state {
    INITIALIZED,
    DEINITIALIZED,
    SUSPENDED
};

/* HAL state machine


    INITIALIZED <-> DEINITIALIZED
       /|\              /|\  
        |                |
       \|/               |
    SUSPENDED  -----------
        
*/

static StreamBufferHandle_t htx;
static StreamBufferHandle_t hrx;
static enum hal_state state = DEINITIALIZED;
static TaskHandle_t tx_handle = NULL;
static uart_inst_t* Uart_id = NULL;
static uint Uart_irq = 0;

void uart_irq_handler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    while (uart_is_readable(Uart_id)) {
        uint8_t ch = uart_getc(Uart_id);

        xStreamBufferSendFromISR(
            hrx,
            &ch,
            1,
            &xHigherPriorityTaskWoken
        );
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void uart_tx_task() {
    while (true) {
        uint8_t bytes[32];
        size_t bytes_read = xStreamBufferReceive(htx, bytes, 32, 5);
        for (size_t i = 0; i < bytes_read; i++) {
            uart_putc_raw(Uart_id, (char)bytes[i]);
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

/*
    Initializes the UART HAL.

    constructs rx and tx stream buffers, and the HAL keeps internal references to them until deinit() is called.
    caller owns the stream buffers and must ensure that they are valid for the lifetime of HAL (ie deinit() call) and free them after deinit().
*/
bool uart_hal_init(StreamBufferHandle_t *rx, StreamBufferHandle_t *tx, uart_inst_t* uartid, uint uartirq) {
    if (state != DEINITIALIZED) return true; // Cannot initilize if we already are.
    Uart_id = uartid;
    Uart_irq = uartirq;

    uint baud = uart_init(Uart_id, BAUDRATE);
    if (baud_within_tolarence(BAUDRATE, baud)) {
        // UART did not init at our desired baud.
        uart_deinit(Uart_id);
        return true;
    }
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);


    uart_set_hw_flow(Uart_id, false, false);

    *rx = xStreamBufferCreate(RX_BUFFER_SIZE, 1);
    *tx = xStreamBufferCreate(TX_BUFFER_SIZE, 1);

    if (*rx == NULL) { // Buffer init failed
        
        if (*tx != NULL) vStreamBufferDelete(*tx);


        gpio_set_function(UART_TX_PIN, GPIO_FUNC_NULL);
        gpio_set_function(UART_RX_PIN, GPIO_FUNC_NULL);
       
        tx_handle = NULL;
        uart_deinit(Uart_id);

        return true; 
    }

    if (*tx == NULL) { // Buffer init failed

        if (*rx != NULL) vStreamBufferDelete(*rx);

        gpio_set_function(UART_TX_PIN, GPIO_FUNC_NULL);
        gpio_set_function(UART_RX_PIN, GPIO_FUNC_NULL);
       
        tx_handle = NULL;
        uart_deinit(Uart_id);

        return true;
    }

    // Set internal references of the HAL
    hrx = *rx;
    htx = *tx;


    if (xTaskCreate(uart_tx_task, "tx_drainer", 512, NULL, TX_TASK_PRIORITY, &tx_handle) != pdPASS) {
        // Failed to create TX drainer.
        gpio_set_function(UART_TX_PIN, GPIO_FUNC_NULL);
        gpio_set_function(UART_RX_PIN, GPIO_FUNC_NULL);

        vStreamBufferDelete(*rx);
        vStreamBufferDelete(*tx);

        hrx = NULL;
        htx = NULL;

        uart_deinit(Uart_id);

        return true;
    }

    uart_set_fifo_enabled(Uart_id, true);
    
    uart_set_format(Uart_id, DATA_BITS, STOP_BITS, PARITY_BITS);

    irq_set_priority(Uart_irq, (1 << 7));

    irq_set_exclusive_handler(Uart_irq, uart_irq_handler);

    irq_set_enabled(Uart_irq, true);

    uart_set_irq_enables(Uart_id, true, false);
    state = INITIALIZED;
    return false;
}

// Suspends UART HAL.
bool uart_hal_suspend(void) {
    // To suspend UART, we first have to suspend the reciever ISR (Disable it)
    // Then suspend the TX task aaand... thats about it
    if (state != INITIALIZED) return true; // Cannot suspend while deinitialized or already suspended.

    vTaskSuspend(tx_handle);
    uart_set_irq_enables(Uart_id, false, false);
    state = SUSPENDED;
    return false;
}

// Resumes the UART HAL.
bool uart_hal_resume(void) {
    if (state != SUSPENDED) return true; // Cannot resume if we are not suspended

    vTaskResume(tx_handle);
    uart_set_irq_enables(Uart_id, true, false);
    state = INITIALIZED;
    return false;
}

// deinits and garbage collects the UART HAL.
bool uart_hal_deinit(void) {
    if (state != INITIALIZED && state != SUSPENDED) return true; // cannot deinit if we are not init()ed

    uart_set_irq_enables(Uart_id, false, false);
    vTaskDelete(tx_handle);
    tx_handle = NULL;

    htx = NULL;
    hrx = NULL;

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_NULL);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_NULL);

    state = DEINITIALIZED;
    return false;
}


