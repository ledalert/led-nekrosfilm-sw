#ifndef UART_H
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/dma.h>

typedef struct {
	volatile void* data;
	int datalen;
} sUART_OutQueueItem;


typedef struct {
	sUART_OutQueueItem* item;
	const int size;
	int count;
	int get_pos;
	int put_pos;

	int sending;

} sUART_OutputQueue;


void uart_recieve(volatile void* data, int length);
void uart_send_data(volatile void* data, int length);
void uart_rcc_init();
void uart_gpio_init();
void uart_init();

int uart_out(volatile void* data, int length);

#define UART_H
#endif