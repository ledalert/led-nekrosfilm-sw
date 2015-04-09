#include "uart.h"
#include "protocol.h"
#include <libopencmsis/core_cm3.h>

#define debug_print(msg) uart_send_data(msg, sizeof(msg)-1)


static sUART_OutQueueItem uart_output_queue_items[32];

static sUART_OutputQueue uart_output_queue = {
	.item = uart_output_queue_items,
	.size = 32,
	.count = 0,
	.get_pos = 0,
	.put_pos = 0,
	.sending = 0,
};


int uart_out_flush() {

	__disable_irq();

	if (!uart_output_queue.sending && uart_output_queue.count > 0) {
		//We are not sending so let's send stuff
		uart_send_data(uart_output_queue.item[uart_output_queue.get_pos].data, uart_output_queue.item[uart_output_queue.get_pos].datalen);
		uart_output_queue.sending = 1;
		uart_output_queue.count--;
		uart_output_queue.get_pos = (uart_output_queue.get_pos + 1) % uart_output_queue.size;
		__enable_irq();
		return 0;

	} else if (uart_output_queue.count > 0) {
		//We are still sending
		__enable_irq();		
		return 0;

	} else {	//All flushed
		__enable_irq();
		return 1;
	}


}


int uart_out(volatile void* data, int length) {
	//TODO atomic!
	__disable_irq();

	if (uart_output_queue.count == uart_output_queue.size) {
		__enable_irq();
		return 0;
	} else {

		uart_output_queue.item[uart_output_queue.put_pos].data = data;
		uart_output_queue.item[uart_output_queue.put_pos].datalen = length;

		uart_output_queue.count++;		
		uart_output_queue.put_pos = (uart_output_queue.put_pos + 1) % uart_output_queue.size;

		
		__enable_irq();

		uart_out_flush();

		return 1;
	}
}


void uart_send_data(volatile void* data, int length) {

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO9);

	dma_channel_reset(DMA1, DMA_CHANNEL4);

	dma_set_peripheral_address(DMA1, DMA_CHANNEL4, (uint32_t) &USART1_DR);
	dma_set_memory_address(DMA1, DMA_CHANNEL4, (uint32_t) data);
	dma_set_number_of_data(DMA1, DMA_CHANNEL4, length);
	dma_set_read_from_memory(DMA1, DMA_CHANNEL4);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL4);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL4, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, DMA_CHANNEL4, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, DMA_CHANNEL4, DMA_CCR_PL_VERY_HIGH);

	dma_enable_channel(DMA1, DMA_CHANNEL4);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL4);
	usart_enable_tx_dma(USART1);


}

void uart_recieve(volatile void* data, int length) {

	protocol_timeout = 1000;

	dma_channel_reset(DMA1, DMA_CHANNEL5);

	dma_set_peripheral_address(DMA1, DMA_CHANNEL5, (uint32_t) &USART1_DR);
	dma_set_memory_address(DMA1, DMA_CHANNEL5, (uint32_t) data);
	dma_set_number_of_data(DMA1, DMA_CHANNEL5, length);
	dma_set_read_from_peripheral(DMA1, DMA_CHANNEL5);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL5);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL5, DMA_CCR_PSIZE_8BIT);
	dma_set_memory_size(DMA1, DMA_CHANNEL5, DMA_CCR_MSIZE_8BIT);
	dma_set_priority(DMA1, DMA_CHANNEL5, DMA_CCR_PL_VERY_HIGH);

	dma_enable_channel(DMA1, DMA_CHANNEL5);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);
	usart_enable_rx_dma(USART1);

}


void uart_rcc_init() {
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_DMA1);
	rcc_periph_clock_enable(RCC_USART1);
}

void uart_gpio_init() {
	//Configure mode
	//gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO9);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO9);		//We start floating

}

void uart_init() {
	uart_rcc_init();
	uart_gpio_init();

	usart_set_baudrate(USART1, 230400);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX | USART_MODE_RX);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);

	usart_enable(USART1);

	nvic_set_priority(NVIC_DMA1_CHANNEL4_IRQ, 0);
	nvic_enable_irq(NVIC_DMA1_CHANNEL4_IRQ);
	nvic_set_priority(NVIC_DMA1_CHANNEL5_IRQ, 0);
	nvic_enable_irq(NVIC_DMA1_CHANNEL5_IRQ);
	nvic_set_priority(NVIC_USART1_IRQ  , 0);
	nvic_enable_irq(NVIC_USART1_IRQ);

}




void usart1_isr(void) {
	USART_CR1(USART1) &= ~USART_CR1_TCIE;
	USART_SR(USART1) &= ~USART_SR_TC;
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO9);		//We start floating

}


void dma1_channel4_isr(void) {
	DMA1_IFCR |= DMA_IFCR_CTCIF4;
	uart_output_queue.sending = 0;
	if (uart_out_flush()) {
		//Allt är flushat men vi kan inte stänga av UART förren den är klar så vi aktiverar transmission complete interrupt
		USART_SR(USART1) &= ~USART_SR_TC;
		USART_CR1(USART1) |= USART_CR1_TCIE;
	}
}