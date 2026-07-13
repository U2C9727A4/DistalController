// TODO: Inter subsystem communication queues and stuff

#include <FreeRTOS.h>
#include "queue.h"
#include <stdint.h>

typedef enum robot_state_t {
    IDLE,           // Robot is idle with no internal program loaded.
    STOP,           // Robot is idle with an internal program loaded.
    RUN,            // Robot is moving (DANGER)
    ERROR_RUN,      // Robot is moving (DANGER), however a recoverable error has occurred.
    PANIC           // Irrecoverable error. All joints halted and braking.
} robot_state_t;

typedef struct {
    float x;
    float y;
    float z;

    float yaw;
    float pitch;
    float roll;
} point_3d_t;

typedef enum {
    LINEAR,
    CIRCULAR,
    FREE, // Free as in the motion kernel is free to decide how to go there
} motion_t;

// TODO: Make documentation for the movement types

typedef struct {
    point_3d_t points[3];


    float speed; // Movement speed in mm/s
    uint32_t opaque_optionals[4]; // Optional opaque (typeless) bits for motion types to use.

    motion_t requested_motion;
    uint8_t movement_complete; // 1 if complete, 0 if not.
} motion_message_t;

