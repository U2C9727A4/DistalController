#ifndef __comms__
#define __comms__

#include "FreeRTOS.h"
#include "stream_buffer.h"

struct uPipe_t;
typedef void (*close_pipe_fn)(struct uPipe_t*);

// Boots the comms stack, returns false if boot is successful, true if a failiure has occurred.
extern bool comms_init(void);

// Suspends the comms stack Returns true if suspension fails, false if suspension is successful.
extern bool comms_suspend(void);

// Terminates comms
extern void comms_deinit(void);

// Resumes comms
extern void comms_resume(void);

struct uPipe_t {
    StreamBufferHandle_t rx;
    StreamBufferHandle_t tx;
    void* internal_data;
    close_pipe_fn close;
};
#endif