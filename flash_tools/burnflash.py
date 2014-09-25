from optparse import OptionParser
from sys import exit
from intelhex import IntelHex
from serial import Serial
from avrdb import Avr_micro
from crc16 import Crc16_Calc
from array import array
import os

frame1 = 0x01
frame2 = 0x12
eot = 0x04
ack = 0x06
nack = 0x15
prgerr = 0xFF
prgok = 0x55

def main():
    (opts, args) = init_parser()

    ih = IntelHex()
    ih.fromfile(opts.file, format='hex')

    micro = Avr_micro(name = opts.mcu.lower())
    if not micro.load():
        print('Device {} not found in database, create and restart the program\n'.format(opts.mcu))
        exit()
    if (len(ih) >= micro.boot):
        print("The File you are trying to upload is too big for the device:")
        print("File size: 0x{:02X} Available space in {}: 0x{:02X}".format(len(ih), micro.name, micro.boot))
        exit()
    try:
        serial = Serial(opts.port, opts.baud, timeout = 5)
    except:
        print('Port {}, can not been open, check if it is blocked by another process'.format(opts.port))
        exit()
    print("Reset device...")
    sig = serial.read(size=3)
    if not len(sig):
        print("Timeout to receive data, exiting")
        exit()
    sig = int.from_bytes(sig, 'big')
    if micro.id != sig:
        print('Device id in database (0x{:06X}) and id received (0x{:06X}) do not match. Exiting!'.format(micro.id,sig))
        exit()
    else:
        serial.write([ack])
        print('Device found (0x{:06X})'.format(micro.id))
    if (micro.boot > 0x7FFF):
        maskflash = 0xD4D8
    else:
        maskflash = 0xD745
    if (micro.spm > 256):
        maskbuff = 0xD745
    else:
        maskbuff = 0xA6BC
    serial.timeout = 1
    crc = Crc16_Calc(mask = maskbuff)
    crcflash = Crc16_Calc(mask = maskflash)
    crc.gentable()
    crcflash.gentable()
    print("Stanting data sent: {} bytes to write of {}".format(len(ih),micro.boot))
    print('Used space: {:.2f}%, and there is {} bytes free'.format((len(ih)*100/micro.boot),(micro.boot-len(ih))))
    aux = len(ih) % micro.spm
    if aux:
        flashend = len(ih) + (micro.spm - aux)
    for i in range(0,flashend,micro.spm):
        buf = ih[i:i+micro.spm].tobinarray()
        if len(buf) < micro.spm:
            for j in range(micro.spm - aux):
                buf.append(0xFF)
        cf = crcflash.tablecrc(*buf)
        crcflash.init = cf
        buf.insert(0,(i & 0xFF))
        buf.insert(0,((i >> 8) & 0xFF))
        c = crc.tablecrc(*buf)
        buf.append(((c >> 8) & 0xFF))
        buf.append((c & 0xFF))
        buf.insert(0,frame1)
        send_data(serial,i,flashend,*buf)
    del buf[:]
    buf.append((cf>>8) & 0xFF)
    buf.append(cf & 0xFF)
    buf.append((~(cf>>8)) & 0xFF)
    buf.append((~cf) & 0xFF)
    buf.insert(0,frame2)
    send_data(serial,i,flashend,*buf)
    send_data(serial,i,flashend,eot)
    serial.timeout=5
    ver = serial.read()
    ver = int.from_bytes(ver, 'big')
    if (ver == prgok):
        print('Data in flash is OK')
    else:
        print('Data verification FAILED!')
    print('Exiting... Good Bye!')
    serial.close()

def init_parser():
    parser = OptionParser()
    parser.add_option('-f', '--file=', help='Input intelHex file (REQUIRED)', dest='file', action='store')
    parser.add_option('-b', '--baud=', help='UART baud rate (default: 19200)', dest='baud', default=19200, action='store', type='int')
    parser.add_option('-p', '--port=', help='Serial port (default: /dev/ttyUSB0)', dest='port', default='/dev/ttyUSB0', action='store')
    parser.add_option('-m', '--mcu=', help='Name of the mcu to programm (REQUIRED)', dest='mcu', action='store')
    (opts, args) = parser.parse_args()
    mandatory = ['file', 'mcu']
    for m in mandatory:
        if not opts.__dict__[m]:
            parser.print_help()
            exit()
    if not os.access(opts.file, os.R_OK):
        print("File is not readable check permissions!")
        exit()
    return (opts, args)

def send_data(ser,badd,fend,*data):
    attempt = 3
    maxpage = fend // 128
    page = badd //128
    while True:
        if (data[0] == eot):
            ser.write([eot])
            print("Leaving dowload mode... ", end="")
        else:
            if data[0] == frame1:
                print('Sending page {}/{} (Attempt: {}/3)... '.format(page,maxpage,(4-attempt)), end='')
            elif data[0] == frame2:
                print('Sending CRC frame to check flash integrity (Attempt: {}/3)... '.format(4-attempt), end='')
            ser.write(data)
        resp = ser.read()
        respint = int.from_bytes(resp, 'big')
        if (len(resp) == 0) or (respint != ack):
            attempt -= 1
            if not attempt:
                print('Can not send package after 3 attempts, exiting!')
                exit()
        else:
            print('OK')
            break

if __name__ == "__main__":
    main()
