#ifndef H_protocol

#include <stdint.h>
#include "hwprofile.h"
#include "uart.h"
#include <libopencmsis/core_cm3.h>

typedef struct {
	int16_t		target;
	int16_t		command;
	int16_t		datalen;
	uint16_t	checksum;
} sProtocolRequest;

typedef struct {
	int16_t		source;
	int16_t		answer;
	int16_t		datalen;
	uint16_t	checksum;
} sProtocolResponse;

typedef enum {

	eprs_Error_UnkownCommand=-1,
	eprs_Error_WrongSize=-2,
	eprs_Error_NoSuchPWMChannel=-3,
	eprs_Error_PWMOutOfRange=-4,

	eprs_OK=0,

} eProtocolResponse;

typedef enum {

	eprq_Status=0,
	eprq_Blackout=1,
	eprq_Whitein=2,
	eprq_GetDescription=3,

	eprq_SetChannelPWM=100,

} eProtocolRequest;

typedef enum {
	eps_Idle=0,
	eps_RecievingRequest=1,
	eps_ServingRequest=2,
	eps_RecievingRequestData=3,

} eProtocolState;

void protocol_init();
void protocol_timeout_handler();
extern volatile int protocol_timeout;

#define H_protocol
#endif