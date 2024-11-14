#include "plotter.h"

#include "usart.h"
#include <stdio.h>
#include <string.h>

uint32_t plotter_get_time_us(void) {
    uint32_t cycles = DWT->CYCCNT;
    uint32_t us = cycles / 170; // clock is 170Mhz. us = cycles / clock * 1M
    return us;
}

void plotter_transmit_data(data_point_t *data, size_t size) {
    const uint8_t buffer_size = 128;
    char buffer[buffer_size];

    for (size_t i = 0; i < size; i++) {
        if (data[i].time == ~0) {
            snprintf(buffer, buffer_size, "* %lu\n", data[i].value);
        } else {
            snprintf(buffer, buffer_size, "%lu %lu\n", data[i].time,
                     data[i].value);
        }
        HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, strlen(buffer), 100);
    }
}

void plotter_send_signal(const char *name, data_point_t *data, size_t size) {
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)name, strlen(name), 100);
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)"\n", 1, 100);

    plotter_transmit_data(data, size);

    const char *buffer = "END_OF_SIGNAL\n";
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, strlen(buffer), 100);
}
