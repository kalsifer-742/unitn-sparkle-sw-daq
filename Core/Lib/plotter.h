#ifndef PLOTTER_H
#define PLOTTER_H

#include <stdint.h>
#include <stddef.h>

typedef struct
{
    uint32_t time;
    uint32_t value;
} data_point_t;

void plotter_transmit_data(data_point_t* data, size_t size);
void plotter_send_signal(const char* name, data_point_t* data, size_t size);
uint32_t plotter_get_time_us(void);


#endif // PLOTTER_H
