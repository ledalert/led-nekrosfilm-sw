#include "hwprofile.h"
#include "uart.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>


const sBase_lamp_desc LAMP_DESC = {
	.name = "5strip",
	.type = elt_5strip,
	.id = LAMP_ID,
	.pwm_channels = 15,
	.pwm_top = 4095,
	.pwm_channel = (sPWM_channel[]) {
		{&TIM1_CCR1},
		{&TIM1_CCR2},
		{&TIM1_CCR3},
		{&TIM2_CCR1},
		{&TIM2_CCR2},
		{&TIM2_CCR3},
		{&TIM3_CCR1},
		{&TIM3_CCR2},
		{&TIM3_CCR3},
		{&TIM15_CCR1},
		{&TIM15_CCR2},
		{&TIM16_CCR1},
		{&TIM1_CCR4},
		{&TIM2_CCR4},
		{&TIM3_CCR4},
	},
};



static void gpio_init() {

	gpio_primary_remap(AFIO_MAPR_SWJ_CFG_JTAG_OFF_SW_OFF, AFIO_MAPR_TIM2_REMAP_PARTIAL_REMAP2);
	
	//Configuring port GPIOA
	//Init pins
	gpio_clear(GPIOA, GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO6 | GPIO7 | GPIO11);
	//Configure mode
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0 | GPIO1 | GPIO2 | GPIO3 | GPIO6 | GPIO7 | GPIO11);

	//Configuring port GPIOB
	//Init pins
	gpio_clear(GPIOB, GPIO0 | GPIO1 | GPIO8 | GPIO10 | GPIO11 | GPIO13 | GPIO14 | GPIO15);
	//Configure mode
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0 | GPIO1 | GPIO8 | GPIO10 | GPIO11 | GPIO13 | GPIO14 | GPIO15);

}


static void pwm_timers_init() {

	//Configuring timer TIM1
	TIM1_CR1 = TIM_CR1_CKD_CK_INT | TIM_CR1_CMS_EDGE;
	TIM1_ARR = LAMP_DESC.pwm_top;
	TIM1_PSC = 0;
	TIM1_EGR = TIM_EGR_UG;
	//Setup output compare
	TIM1_CCR1 = 0;
	TIM1_CCR2 = 0;
	TIM1_CCR3 = 0;
	TIM1_CCR4 = 0;

	TIM1_BDTR |= TIM_BDTR_MOE; //test

	//TIM1_CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E; //test
	TIM1_CCER |= TIM_CCER_CC1NE | TIM_CCER_CC1P | TIM_CCER_CC2NE | TIM_CCER_CC2P | TIM_CCER_CC3NE | TIM_CCER_CC3P | TIM_CCER_CC4E;
	TIM1_CCMR1 |= TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC1PE | TIM_CCMR1_OC2M_PWM1 | TIM_CCMR1_OC2PE;
	TIM1_CCMR2 |= TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE | TIM_CCMR2_OC4M_PWM1 | TIM_CCMR2_OC4PE;

	TIM1_CR1 |= TIM_CR1_ARPE;
	TIM1_CR1 |= TIM_CR1_CEN;

	//Configuring timer TIM2
	TIM2_CR1 = TIM_CR1_CKD_CK_INT | TIM_CR1_CMS_EDGE;
	TIM2_ARR = LAMP_DESC.pwm_top;
	TIM2_PSC = 0;
	TIM2_EGR = TIM_EGR_UG;
	//Setup output compare
	TIM2_CCR1 = 0;
	TIM2_CCR2 = 0;
	TIM2_CCR3 = 0;
	TIM2_CCR4 = 0;
	TIM2_CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	TIM2_CCMR1 |= TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC1PE | TIM_CCMR1_OC2M_PWM1 | TIM_CCMR1_OC2PE;
	TIM2_CCMR2 |= TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE | TIM_CCMR2_OC4M_PWM1 | TIM_CCMR2_OC4PE;

	TIM2_CR1 |= TIM_CR1_ARPE;
	TIM2_CR1 |= TIM_CR1_CEN;

	//Configuring timer TIM3
	TIM3_CR1 = TIM_CR1_CKD_CK_INT | TIM_CR1_CMS_EDGE;
	TIM3_ARR = LAMP_DESC.pwm_top;
	TIM3_PSC = 0;
	TIM3_EGR = TIM_EGR_UG;
	//Setup output compare
	TIM3_CCR1 = 0;
	TIM3_CCR2 = 0;
	TIM3_CCR3 = 0;
	TIM3_CCR4 = 0;
	TIM3_CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
	TIM3_CCMR1 |= TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC1PE | TIM_CCMR1_OC2M_PWM1 | TIM_CCMR1_OC2PE;
	TIM3_CCMR2 |= TIM_CCMR2_OC3M_PWM1 | TIM_CCMR2_OC3PE | TIM_CCMR2_OC4M_PWM1 | TIM_CCMR2_OC4PE;

	TIM3_CR1 |= TIM_CR1_ARPE;
	TIM3_CR1 |= TIM_CR1_CEN;

	//Configuring timer TIM15
	TIM15_CR1 = TIM_CR1_CKD_CK_INT | TIM_CR1_CMS_EDGE;
	TIM15_ARR = LAMP_DESC.pwm_top;
	TIM15_PSC = 0;
	TIM15_EGR = TIM_EGR_UG;
	//Setup output compare
	TIM15_CCR1 = 0;
	TIM15_CCR2 = 0;
	TIM15_CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
	TIM15_CCMR1 |= TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC1PE | TIM_CCMR1_OC2M_PWM1 | TIM_CCMR1_OC2PE;

	TIM15_BDTR |= TIM_BDTR_MOE; //test


	TIM15_CR1 |= TIM_CR1_ARPE;
	TIM15_CR1 |= TIM_CR1_CEN;

	//Configuring timer TIM16
	TIM16_CR1 = TIM_CR1_CKD_CK_INT | TIM_CR1_CMS_EDGE;
	TIM16_ARR = LAMP_DESC.pwm_top;
	TIM16_PSC = 0;
	TIM16_EGR = TIM_EGR_UG;
	//Setup output compare
	TIM16_CCR1 = 0;
	TIM16_CCER |= TIM_CCER_CC1E;
	TIM16_CCMR1 |= TIM_CCMR1_OC1M_PWM1 | TIM_CCMR1_OC1PE;


	TIM16_BDTR |= TIM_BDTR_MOE; //test


	TIM16_CR1 |= TIM_CR1_ARPE;
	TIM16_CR1 |= TIM_CR1_CEN;	
}


static void rcc_init() {
	rcc_clock_setup_in_hsi_out_24mhz();
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);

	rcc_periph_clock_enable(RCC_TIM1);
	rcc_periph_clock_enable(RCC_TIM2);
	rcc_periph_clock_enable(RCC_TIM3);
	rcc_periph_clock_enable(RCC_TIM15);
	rcc_periph_clock_enable(RCC_TIM16);


}

void hwprofile_init() {

	rcc_init();

	gpio_init();
	pwm_timers_init();
	init_systick();
	uart_init();

}