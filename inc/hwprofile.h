#ifndef H_hwprofile

#include <stdint.h>
#include "systick.h"

void hwprofile_init();

typedef struct {
	volatile uint32_t* timer_ccr;
} sPWM_channel;


typedef enum {
	elt_rxlamp=1,
	elt_5strip=2,
	elt_robotkort=3,
} eLampType;

typedef struct {
	//4 integers med stuff
	eLampType type;
	int id;
	const int pwm_channels;
	int pwm_top;


	//Dynamic length stuff
	char* name;
	sPWM_channel* pwm_channel;


} sBase_lamp_desc;


void pwm_set_all(int value);
void pwm_set_channel(int ch, int value);


extern const sBase_lamp_desc LAMP_DESC;


#define H_hwprofile
#endif