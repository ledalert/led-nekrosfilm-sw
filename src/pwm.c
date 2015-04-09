#include "hwprofile.h"

void pwm_set_channel(int ch, int value) {
	*LAMP_DESC.pwm_channel[ch].timer_ccr = value;
}

void pwm_set_all(int value) {
	for (int ch=0; ch<LAMP_DESC.pwm_channels; ch++) {
		pwm_set_channel(ch, value);
	}
}