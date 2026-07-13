#ifndef __comms__
#define __comms__

#include "FreeRTOS.h"
#include "stream_buffer.h"

// A default priority number of tasks in the comms subsystem that directly access hardware.
#define HARDCOMM_DEFAULT_PRIORITY 5

struct uPipe_t;
typedef void (*close_pipe_fn)(struct uPipe_t*);

// Boots the comms stack, returns false if boot is successful, true if a failiure has occurred.
extern bool comms_init(void);


// Terminates comms
extern void comms_deinit(void);

struct uPipe_t {
    StreamBufferHandle_t rx;
    StreamBufferHandle_t tx;
    void* internal_data;
    close_pipe_fn close;
};

#endif