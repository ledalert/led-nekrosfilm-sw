#include "hwprofile.h"
#include "uart.h"
#include "protocol.h"

#define print(t) uart_out(t, sizeof(t)-1)



int main() {

	hwprofile_init();


	protocol_init();


	//protocol_timeout_handler();

	while(1);
	//  {

	// 	pwm_set_all(1024);
	// 	uart_send_data("Hello World!\n", 13);
	// 	pwm_set_all(0);

		
	// }

	

	return 0;
}