#NSTloader - Not-So-Tiny-bootloader
##Programmed in C with python tools, for AVR microcontrollers

This bootloader is based on the _AVR109_ application note, using UART serial
protocol communication, to flash data to the microcontroller.

Before any attempt of communication with the microcontroller could be
established, information of signature, device name, bootloader address,
SPM_SIZE, FLASHEND and EEPROM should be added to the database (avr.db)
included with the flashing tools, using the avrdb.py script. All this
information should be checked within the datasheet of the device.

###Protocol description
When a RESET of the microcontroller is perform, it will send a 3 bit string,
containing the signature bytes of the device; to enter into the bootloader
program, a ACK (0x06) byte should be replied. If there no is valid reply within
a period of 1 second (this time could be change in the source files of the
bootloader configuration file 'config.h' --> 'OVRFLW'), the device will lauch
the application saved in the flash address 0x0000.

Once the bootloader is active, a number of commands could be send to performs
the flashing actions:

* Read Flash ('r'):
A frame with the page address (index) of the page in the flash memory that
want to be read, and a 16 bit crc of the data is sent (3 bytes frame).
The device will perform the read action and reply with a data frame of
length equal to SPM_SIZE, plus 2 bytes of crc. This is a 1 page-at-a-time
read function.

* Write Flash ('w'):
A frame consisting of page address index, a SPM_SIZE length of data, and
a 16 bit crc, will be sent to the device. The writing action will be
perform, and once is finish will reply with an ACK. To execute all the
writings in the flash memory the 'rww enable' has to be perform (refer to
Execute flash modification). This is a 1 page-at-a-time writing function.

* Flash data verify ('v'):
A frame of 1 byte (page address index, of the next page to last byte where the
data will be verified, ie. if you want to verify the data till the 0x7BFF byte
(or 0x7B80 bytewise address, or 247th page in flash memory), an address index
of 248 (0x7C00 bytewise) should be sent. Followed by 2 bytes of crc. This
action will perform in a complete page, so when the data in the hex file that
you want to verify against the flash memory, is smaller than a complete full
page, the remaining bytes should be filled with 0xFF.
The device will reply with a frame of 4 bytes, consisting of the 2 bytes of crc,
result of data verifying, and 2 more of the complement of this crc. This is
used to check the data received.

* Flash memory clear ('c'):
This command will erase all the data contained in the flash memory, and will
reply with and ACK byte afterwards.

* EEPROM read ('R'):
Similar to Flash read, but perform on EEPROM memory.

* EEPROM write ('W'):
Will update the data in the EEPROM memory (please refer to Flash write
section) with the data sent. After this command a re-enable of the
memory is not necessary.

* EEPROM data verify ('V'):
Refer to 'Flash data verify' section.

* EEPROM clear ('C'):
Will update the data on all the EEPROM memory with 0xFF bytes (erasing)
the content of this memory. Will reply with and ACK, once completed.

* Execute enable of SPM pages ('X'):
This will perform a RWW section enable, so the pages written to the flash memory
could be read and verified before leaving the bootloader. An alternative is to
perform a reset (not recommended).

* Quit bootloader ('Q'):
This will simply quit the bootloader application and reply with a ACK.

* All other bytes sent to the device will result in a NACK reply. This could be
used to detect that the bootloader is still active.

*NOTE:* Please take into consideration that the commands to
read/write/verify/clear for the flash memory, are similar to the EEPROM
commands, but the first ones are lowercase.

*NOTE:* When refering to the page address index, is the result of performing
a division of the bytewise page address, between the SPM_SIZE
(PAGE ADDRESS / SPM_SIZE or PAGEADDRESS>>LOG2(SPM_SIZE)).

The CRC calculation varies in function of the data length, using the best
mask for that length and a initial value of 0xFFFF.
More information avalaible on: http://users.ece.cmu.edu/~koopman/crc/

###Install bootloader
- Go to src folder
- Modify the 'Makefile' file with the data required for your device and flasher
- Perform a 'make' command, this will compile and link the source files
- Run a 'make hex', this will create a hex file of the bootloader program
- Next will be 'make install' to burn the hex file into the bootloader section
of the Flash memory of the device
* _Makefile_ also has other commands ie. 'make clean', 'make readflash',
'make readeeprom', 'make cleanall' that could be useful in some cases.

###Flashing tool
- Go to tool folder
- Run the 'avrdb.py' script (python avrdb.py)
- A menus with the actions that can be perform are diplayed. Check if the
information for your device is already in the data base, if not add it (all
the information needed is in the datasheet of the device)
- Once the data is in the data base, exit the program
- Now run 'python nstbFlasher.py -h', an the help dialog will be displayed
- Next run the flasher script will all the options required for your
  application

###Example
A hex file is included in the 'example' folder, this is a simple application
that will toggle a led connected to the PB1 pin of the device, with a frequency
of 400 ms. Please use this to test the bootloader.


****************************************************************************
Firmware Copyright (C):
    2015 - Alberto Vazquez aka IcyD

        This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

        This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************
