#include "hwprofile.h"
#include "uart.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>


const sBase_lamp_desc LAMP_DESC = {
	.name = "rxlamp",
	.type = elt_rxlamp,
	.id = LAMP_ID,
	.pwm_channels = 3,
	.pwm_top = 4095,
	.pwm_channel = (sPWM_channel[]) {
		{&TIM3_CCR3},
		{&TIM2_CCR3},
		{&TIM2_CCR2},
	},
};



static void gpio_init() {

	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO1 | GPIO2);
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0);


}

static void pwm_timers_init() {

	//Configuring timer TIM2
	TIM2_CR1 = TIM_CR1_CKD_CK_INT | TIM_CR1_CMS_EDGE;
	TIM2_ARR = LAMP_DESC.pwm_top;
	TIM2_PSC = 0;
	TIM2_EGR = TIM_EGR_UG;
	//Setup output compare
	TIM2_CCR2 = 0;
	TIM2_CCR3 = 0;
	TIM2_CCER |= TIM_CCER_CC2E | TIM_CCER_CC3E;
	TIM2_CCMR1 |= TIM_CCMR1_OC2M_PWM1 | TIM_CCMR1_OC2PE;
	TIM2_CCMR2 |= TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE;

	TIM2_CR1 |= TIM_CR1_ARPE;
	TIM2_CR1 |= TIM_CR1_CEN;

	//Configuring timer TIM3
	TIM3_CR1 = TIM_CR1_CKD_CK_INT | TIM_CR1_CMS_EDGE;
	TIM3_ARR = LAMP_DESC.pwm_top;
	TIM3_PSC = 0;
	TIM3_EGR = TIM_EGR_UG;
	//Setup output compare
	TIM3_CCR3 = 0;
	TIM3_CCER |= TIM_CCER_CC3E;
	TIM3_CCMR2 |= TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE;

	TIM3_CR1 |= TIM_CR1_ARPE;
	TIM3_CR1 |= TIM_CR1_CEN;

}


static void rcc_init() {
	rcc_clock_setup_in_hsi_out_24mhz();
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);

	rcc_periph_clock_enable(RCC_TIM2);
	rcc_periph_clock_enable(RCC_TIM3);


}

void hwprofile_init() {

	rcc_init();

	gpio_init();
	pwm_timers_init();
	init_systick();
	uart_init();


}