#include "plotter.h"

#include "usart.h"
#include <stdio.h>
#include <string.h>

/*
 * Legacy code
 *
static void plotter_transmit_data(data_point_t* data, size_t size) {
    const size_t buffer_size = 128;
    char buffer[buffer_size];

    for (size_t i = 0; i < size; i++) {
    	int len;
        if (data[i].time == ~0) {
            len = snprintf(buffer, buffer_size, "* %lu\n", data[i].value);
        } else {
            len = snprintf(buffer, buffer_size, "%lu %lu\n", data[i].time, data[i].value);
        }
        HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, len, 100);
    }
}

void plotter_send_signal(const char* name, data_point_t* data, size_t size) {

    HAL_UART_Transmit(&hlpuart1, (uint8_t *)name, strlen(name), 100);
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)"\n", 1, 100);

    plotter_transmit_data(data, size);

    const char* end_msg = "END_OF_SIGNAL\n";
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)end_msg, strlen(end_msg), 100);
}
*/

uint32_t plotter_get_time_us(void) {
    uint32_t cycles = DWT->CYCCNT;
    uint32_t us = cycles / 170; // clock is 170Mhz. us = cycles / clock * 1M
    return us;
}

// stride is the distance between one element to be sent and the next; in number of elements (=sizeof uint32_t), NOT in bytes;
// size is the number of elements that need to be sent, NOT the size of the buffer (neither in bytes nor in elements)
static void plotter_transmit_data_u16_lerp_time(uint16_t* data, size_t stride, size_t size, uint32_t start_time, uint32_t end_time) {
    const size_t buffer_size = 128;
    char buffer[buffer_size];

    int len = snprintf(buffer, buffer_size, "%lu %u\n", start_time, data[0]);
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, len, 100);

    for (size_t i = 1; i < size-1; i++) {
    	len = snprintf(buffer, buffer_size, "* %u\n", data[stride*i]);
        HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, len, 100);
    }
    len = snprintf(buffer, buffer_size, "%lu %u\n", end_time, data[stride*(size-1)]);
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)buffer, len, 100);
}

// size is the number of elements for each signal, NOT the size of data (neither in elements nor in bytes)
void plotter_send_2_interleaved_u16_signals_lerp_time(const char** names, uint16_t* data, size_t size, uint32_t start_time, uint32_t end_time) {
	for(int signal_idx = 0; signal_idx < 2; signal_idx++) {

		const char* name = names[signal_idx];
		HAL_UART_Transmit(&hlpuart1, (uint8_t *)name, strlen(name), 100);
		HAL_UART_Transmit(&hlpuart1, (uint8_t *)"\n", 1, 100);

	    plotter_transmit_data_u16_lerp_time(data + signal_idx, 2, size, start_time, end_time);

	    const char* end_msg = "END_OF_SIGNAL\n";
		HAL_UART_Transmit(&hlpuart1, (uint8_t *)end_msg, strlen(end_msg), 100);
	}
}
