#encoding=utf-8

import serial
import sys
import crcmod.predefined
import struct
import time

crc = crcmod.predefined.mkCrcFun('kermit')

s= serial.Serial("/dev/ttyUSB1", 230400,timeout=0.025)

s.setRTS(True)	#Turn off processor
s.setDTR(True)	#Flash boot mode selected
s.setRTS(False)	#Turn on processor


def nullfun(*args, **kwargs):
	pass

#TODO: fixa så man märker om ett underkommando misslyckas

debug_print = nullfun
#debug_print = print


def cmd(target, cmd_code, send_data=None):
	
	send_datalen = len(send_data) if send_data else 0

	req_buf = struct.pack('hhh', target, cmd_code, send_datalen)
	req = req_buf + struct.pack('H', crc(req_buf))
	debug_print("Writing request", req)
	s.write(req)

	if target != 0:

		debug_print("Reading response")

		data = s.read(6)
		debug_print("Response raw data", data)
		source, answer, datalen = struct.unpack('hhh', data)
		debug_print("Response data", source, answer, datalen)

		checksum, = struct.unpack('H', s.read(2))
		debug_print("Response checksum", checksum)
		expected_checksum = crc(data)
		debug_print("Expected checksum", expected_checksum)

		if checksum != expected_checksum:
			raise Exception("Checksum mismatch! %i != %i" % (checksum, expected_checksum))

		response_data = None

		if datalen:
			debug_print("Reading response data")
			response_data = s.read(datalen)
			debug_print("Response data", response_data)

			response_data_checksum, = struct.unpack('H', s.read(2))
			debug_print("Response data checksum", response_data_checksum)


			expected_response_data_checksum = crc(response_data)
			debug_print("Expected response data checksum", expected_response_data_checksum)

			if expected_response_data_checksum != response_data_checksum:
				raise Exception("Response data checksum mismatch! %i != %i" % (response_data_checksum, expected_response_data_checksum))


		if answer != 0:
			raise Exception("Command to target %i failed with code %i (Response data: %r)" %(source, answer, response_data))


		if send_datalen:
			assert datalen == 0  #Vet inte när man vill skicka så man får data i första skedet och sedan skickar mer data, det verkar onödigt



			send_data_crc = crc(send_data)
			debug_print("Sending data", send_data)
			s.write(send_data)
			debug_print("Sending data checksum", send_data_crc)
			s.write(struct.pack('H', send_data_crc))

			#pass 2
			debug_print("Recieving answer")
			data = s.read(6)
			debug_print("Answer raw data", data)
			source, answer, datalen = struct.unpack('hhh', data)
			debug_print("Answer data", source, answer, datalen)
			
			checksum, = struct.unpack('H', s.read(2))
			expected_checksum = crc(data)
			if checksum != expected_checksum:
				raise Exception("Checksum mismatch! %i != %i" % (checksum, expected_checksum))

			if datalen:			
				response_data = s.read(datalen)
				response_data_checksum, = struct.unpack('H', s.read(2))
				expected_response_data_checksum = crc(response_data)
				if expected_response_data_checksum != response_data_checksum:
					raise Exception("Response data checksum mismatch! %i != %i" % (response_data_checksum, expected_response_data_checksum))

				return response_data

			else:
				return True


		return response_data
	

LAMP_TYPE = {
	1:	'rxlamp',
	2:	'5strip',
}


def get_desc(adr):
	lamp_type, lamp_id, lamp_channels, lamp_top = struct.unpack('iiii', cmd(adr, 0x3))
	print("Lamp information:")
	print((
		"\tType:\t\t%i (%s)\n"
		"\tID:\t\t%i\n"
		"\tChannels:\t%i\n"
		"\tPWM Top:\t%i\n") 
		% (lamp_type, LAMP_TYPE.get(lamp_type, 'Unknown'), lamp_id, lamp_channels, lamp_top)
	)



def qcmd(*args):
	try:
		return cmd(*args)
	except:
		pass


def oscilloskop():
	while 1:
		print (qcmd(0x1, 0x1))
		time.sleep(0.01)


import math

def oscilloskop2():
	while 1:
		try:
			print (cmd(1, 100, struct.pack('ii', 0, int(math.sin(time.time())*2047+2047))))
		except:
			time.sleep(0.05)


def test2():
	while 1:
		cmd(1, 100, struct.pack('ii', 0, int(math.sin(time.time())*2047+2047)))
		cmd(1, 100, struct.pack('ii', 1, int(math.sin(time.time()*1.2)*2047+2047)))
		cmd(1, 100, struct.pack('ii', 2, int(math.sin(time.time()*1.6)*2047+2047)))

def test3():
	while 1:
		for i in range(3):
			cmd(1, 100, struct.pack('ii', i, 4095))
			cmd(1, 100, struct.pack('ii', i, 0))

def set_pwm(adr, channel, value):
	cmd(adr, 100, struct.pack('ii', channel, value))	

def test4():
	while 1:
		try:
			set_pwm(1, 0, 0)
			time.sleep(0.1)
			set_pwm(2, 0, 0)
			time.sleep(0.1)
		except:
			time.sleep(1.0)


#test4()

#test2()

#print (cmd(0x2, 0x0))

# cmd(0x0, 0x2)
# s.write(b'!')
# cmd(0x0, 0x1)

# time.sleep(1.2)
# cmd(0x0, 0x1)


# while 1:	
# 	cmd(0x0, 0x2)
# 	time.sleep(0.1)
# 	cmd(0x0, 0x1)
# 	time.sleep(0.3)
