#include "systick.h"
#include "protocol.h"

volatile static int ms_time_delay;
volatile int protocol_timeout=0;

//Should be put somewhere else later because different HW might need different systick

void init_systick() {
	ms_time_delay=0;
	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_set_reload(24000-1);  // 1 kHz
	systick_interrupt_enable();
	systick_counter_enable();


}

void sys_tick_handler(void) {
	if (ms_time_delay) {
		ms_time_delay--;
	}

	if (protocol_timeout) {

		if (--protocol_timeout == 0) {
			protocol_timeout_handler();
		}
	}
}

void systick_sleep_ms(int t) {
	ms_time_delay = t;
	while (ms_time_delay);
}

