#encoding=utf-8

import crcmod.predefined

crc = crcmod.predefined.Crc('kermit')

import sys, os
result = os.fdopen(1, 'w')
crc.generateCode('crc_kermit', result, 'uint8_t', 'uint16_t')