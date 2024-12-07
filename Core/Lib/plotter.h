#ifndef PLOTTER_H
#define PLOTTER_H

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    uint32_t time;
    uint32_t value;
} data_point_t;

void plotter_send_signal(const char* name, data_point_t* data, size_t size);
// the lerp is computed on the plotter side, not on mcu side
// size is the number of elements for each signal, NOT the size of data (neither in elements nor in bytes)
void plotter_send_2_interleaved_u16_signals_lerp_time(const char** names, uint16_t* data, size_t size, uint32_t start_time, uint32_t end_time);
uint32_t plotter_get_time_us(void);


#endif // PLOTTER_H
