# General functions

All communication subprojects ("components") must provide the following functions ("SN" is used as short for subsystem name.)

bool SN_init()
bool SN_suspend()
bool SN_resume()
bool SN_deinit()

the init() function must initilize the component. It may have arguements that the specific component needs in order to initialize. Returns false on success, true on error.
the suspend() function suspends the execution of the component. during suspension, the component may not use any CPU time. Returns false on success, true on error.
the resume() function resumes the execution of the component **after suspension**, the component may resume normal operation after this.
the deinit() function de-initializes the component. after de-initialization, the component must not use any CPU nor RAM (Unless statically allocated or any other allocation method that disallows memory to be taken back).

# I/O Streams

All components must use FreeRTOS Stream Buffers for both input and output streams.

For example, lets take the UART HAL; It provides two pairs of streams; rx and tx.
When TX is written to with the corresponding FreeRTOS functions, the UART HAL sends that to the underlying UART stream.
The UART hal also produces to (writes to) the RX stream buffer.

# Assumptions

All components are expected to be running in the background (ie. not in control for other components or tasks that depend on them)