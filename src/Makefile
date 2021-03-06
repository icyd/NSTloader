
#########  AVR Project Makefile Template   #########
######                                        ######
######    Copyright (C) 2003-2005,Pat Deegan, ######
######            Psychogenic Inc             ######
######          All Rights Reserved           ######
######                                        ######
###### You are free to use this code as part  ######
###### of your own applications provided      ######
###### you keep this copyright notice intact  ######
###### and acknowledge its authorship with    ######
###### the words:                             ######
######                                        ######
###### "Contains software by Pat Deegan of    ######
###### Psychogenic Inc (www.psychogenic.com)" ######
######                                        ######
###### If you use it as part of a web site    ######
###### please include a link to our site,     ######
###### http://electrons.psychogenic.com  or   ######
###### http://www.psychogenic.com             ######
######                                        ######
####################################################


##### This Makefile will make compiling Atmel AVR
##### micro controller projects simple with Linux
##### or other Unix workstations and the AVR-GCC
##### tools.
#####
##### It supports C, C++ and Assembly source files.
#####
##### Customize the values as indicated below and :
##### make
##### make disasm
##### make stats
##### make size
##### make hex
##### make readflash
##### make readeeprom
##### make writeflash
##### make writeeeprom
##### make fuses
##### make gdbinit
##### make clean
##### make cleanall
#####
##### See the http://electrons.psychogenic.com/
##### website for detailed instructions


####################################################
#####                                          #####
#####              Configuration               #####
#####                                          #####
##### Customize the values in this section for #####
##### your project. MCU, PROJECTNAME and       #####
##### PRJSRC must be setup for all projects,   #####
##### the remaining variables are only         #####
##### relevant to those needing additional     #####
##### include dirs or libraries and those      #####
##### who wish to use the avrdude programmer   #####
#####                                          #####
##### See http://electrons.psychogenic.com/    #####
##### for further details.                     #####
#####                                          #####
####################################################


#####         Target Specific Details          #####
#####     Customize these for your project     #####

# Name of target controller
# (e.g. 'at90s8515', see the available avr-gcc mmcu
# options for possible values)
MCU=atmega328p
F_CPU=8000000UL

# id to use with programmer
# default: PROGRAMMER_MCU=$(MCU)
# In case the programer used, e.g avrdude, doesn't
# accept the same MCU name as avr-gcc (for example
# for ATmega8s, avr-gcc expects 'atmega8' and
# avrdude requires 'm8')
PROGRAMMER_MCU=m328p

# Name of our project
# (use a single word, e.g. 'myproject')
PROJECTNAME=boot

# Source files
# List C/Assembly source files:
# (list all files to compile, e.g. 'a.c as.S'):
# Use .cc, .cpp or .C suffix for C++ files, use .S
# (NOT .s !!!) for assembly source code files.
PRJSRC=boot.c crc16.c

# additional includes (e.g. -I/path/to/mydir)
INC=

# libraries to link in (e.g. -lmylib)
LIBS=

# Optimization level,
# use s (size opt), 1, 2, 3 or 0 (off)
OPTLEVEL=0


#####      AVR Dude 'writeflash' options       #####
#####  If you are using the avrdude program
#####  (http://www.bsdhome.com/avrdude/) to write
#####  to the MCU, you can set the following config
#####  options and use 'make writeflash' to program
#####  the device.


# programmer id--check the avrdude for complete list
# of available opts.  These should include stk500,
# avr910, avrisp, bsd, pony and more.  Set this to
# one of the valid "-c PROGRAMMER-ID" values
# described in the avrdude info page.
#
AVRDUDE_PROGRAMMERID=atmelice_isp

# port--serial or parallel port to which your
# hardware programmer is attached
#
AVRDUDE_PORT=usb

####################################################
#####                Memory                    #####
##### Definition of the bootloader section     #####
##### Start in memory (Bytewise)               #####
####################################################
BLSTART=0x7C00

####################################################
#####                Config Done               #####
#####                                          #####
##### You shouldn't need to edit anything      #####
##### below to use the makefile but may wish   #####
##### to override a few of the flags           #####
##### nonetheless                              #####
#####                                          #####
####################################################


##### Flags ####

# HEXFORMAT -- format for .hex file output
HEXFORMAT=ihex

# compiler
CFLAGS=-I. $(INC) -g2 -gstabs -mmcu=$(MCU) -O$(OPTLEVEL) \
	-std=gnu99 -D F_CPU=$(F_CPU) \
	-fpack-struct -fshort-enums \
	-funsigned-bitfields -funsigned-char \
	-Wall -Werror -Wshadow -Wundef -Winline \
	-Wstrict-prototypes -Wmissing-declarations \
	-Winline -Wmissing-noreturn \
	-ffreestanding -fno-inline \
	-ffunction-sections -fdata-sections \
	-Wa,-ahlms=$(firstword \
	$(filter %.lst, $(<:.c=.lst)))


# assembler
ASMFLAGS =-I. $(INC) -g2 -mmcu=$(MCU)        \
	-x assembler-with-cpp            \
	-Wa,-gstabs,-ahlms=$(firstword   \
		$(<:.S=.lst) $(<.s=.lst))


# linker
LDFLAGS=-Wl,-Map,$(TRG).map \
	-mmcu=$(MCU) -lm $(LIBS) \
	-g -gstabs -nostartfiles \
	-Wl,--relax,--gc-sections \
	-Wl,--section-start=.text=$(BLSTART)


##### executables ####
CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
SIZE=avr-size
AVRDUDE=avrdude
REMOVE=rm -f
CAT=cat
ECHO=echo -n

##### automatic target names ####
TRG=$(PROJECTNAME).elf
DUMPTRG=$(PROJECTNAME).s

HEXROMTRG=$(PROJECTNAME).hex
HEXFLASHDMP='fdump.hex'
HEXEEPROMTRG=$(PROJECTNAME).ee.hex
HEXEEPROMDMP='eedump.hex'
GDBINITFILE=gdbinit-$(PROJECTNAME)
HFUSE='h_fuse'
LFUSE='l_fuse'
EFUSE='e_fuse'
FUSES='fuses.txt'

# Define all object files.

# Start by splitting source files by type
#  C
CFILES=$(filter %.c, $(PRJSRC))
#  Assembly
ASMFILES=$(filter %.S, $(PRJSRC))


# List all object files we need to create
OBJDEPS=$(CFILES:.c=.o)    \
	$(ASMFILES:.S=.o)

# Define all lst files.
LST=$(filter %.lst, $(OBJDEPS:.o=.lst))

# All the possible generated assembly
# files (.s files)
GENASMFILES=$(filter %.s, $(OBJDEPS:.o=.s))


.SUFFIXES : .c .o .out .s .S \
	.hex .ee.hex .h .hh .hpp


.PHONY: writeflash clean stats gdbinit stats

# Make targets:
# all, disasm, stats, hex, writeflash/install, clean
all: $(TRG)

disasm: $(DUMPTRG) stats

stats: $(TRG)
	$(OBJDUMP) -h $(TRG)
	$(SIZE) $(TRG)

size:  $(TRG)
	$(SIZE) $(TRG)

hex: $(HEXROMTRG)

eeprom: $(HEXEEPROMTRG)

readflash:
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID)   \
	 -p $(PROGRAMMER_MCU) -P $(AVRDUDE_PORT) \
	 -U flash:r:$(HEXFLASHDMP):i

writeflash: hex
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID)   \
	 -p $(PROGRAMMER_MCU) -P $(AVRDUDE_PORT) -e \
	 -U flash:w:$(HEXROMTRG)

readeeprom:
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID) \
	-p $(PROGRAMMER_MCU) -P $(AVRDUDE_PORT) \
	-U eeprom:r:$(HEXEEPROMDMP):i

writeeeprom: eeprom
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID) \
	-p $(PROGRAMMER_MCU) -P $(AVRDUDE_PORT) \
	-U eeprom:w:$(HEXEEPROMTRG)

fuses:
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID) \
	-p $(PROGRAMMER_MCU) -P $(AVRDUDE_PORT) \
	-U hfuse:r:$(HFUSE):h
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID) \
	-p $(PROGRAMMER_MCU) -P $(AVRDUDE_PORT) \
	-U lfuse:r:$(LFUSE):h
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID) \
	-p $(PROGRAMMER_MCU) -P $(AVRDUDE_PORT) \
	-U efuse:r:$(EFUSE):h
	$(ECHO) "H: "> $(FUSES)
	$(CAT) $(HFUSE) >> $(FUSES)
	$(ECHO) "L: ">> $(FUSES)
	$(CAT) $(LFUSE) >> $(FUSES)
	$(ECHO) "E: ">> $(FUSES)
	$(CAT) $(EFUSE) >> $(FUSES)
	$(REMOVE) $(HFUSE) $(LFUSE) $(EFUSE)

install: writeflash

$(DUMPTRG): $(TRG)
	$(OBJDUMP) -S  $< > $@

$(TRG): $(OBJDEPS)
	$(CC) $(LDFLAGS) -o $(TRG) $(OBJDEPS)


#### Generating assembly ####
# asm from C
%.s: %.c
	$(CC) -S $(CFLAGS) $< -o $@

# asm from (hand coded) asm
%.s: %.S
	$(CC) -S $(ASMFLAGS) $< > $@

#### Generating object files ####
# object from C
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

# object from asm
.S.o :
	$(CC) $(ASMFLAGS) -c $< -o $@


#### Generating hex files ####
# hex files from elf
#####  Generating a gdb initialisation file    #####
.out.hex:
	$(OBJCOPY) -j .text                    \
		-j .data -j .version               \
		-O $(HEXFORMAT) $< $@

.out.ee.hex:
	$(OBJCOPY) -j .eeprom                  \
		--change-section-lma .eeprom=0 \
		-O $(HEXFORMAT) $< $@

#####  Generating a gdb initialisation file    #####
##### Use by launching simulavr and avr-gdb:   #####
#####   avr-gdb -x gdbinit-myproject           #####
gdbinit: $(GDBINITFILE)

$(GDBINITFILE): $(TRG)
	@echo "file $(TRG)" > $(GDBINITFILE)
	@echo "target remote localhost:1234" \
		                >> $(GDBINITFILE)
	@echo "load"        >> $(GDBINITFILE)
	@echo "break main"  >> $(GDBINITFILE)
	@echo "continue"    >> $(GDBINITFILE)
	@echo
	@echo "Use 'avr-gdb -x $(GDBINITFILE)'"


#### Cleanup ####
clean:
	$(REMOVE) $(TRG) $(TRG).map $(DUMPTRG)
	$(REMOVE) $(OBJDEPS)
	$(REMOVE) $(LST) $(GDBINITFILE)
	$(REMOVE) $(GENASMFILES)
	$(REMOVE) $(HEXROMTRG)
	$(REMOVE) $(HEXEEPROMTRG)

cleanall:
	$(REMOVE) $(TRG) $(TRG).map $(DUMPTRG)
	$(REMOVE) $(OBJDEPS)
	$(REMOVE) $(LST) $(GDBINITFILE)
	$(REMOVE) $(GENASMFILES)
	$(REMOVE) $(HEXROMTRG)
	$(REMOVE) $(HEXEEPROMTRG)
	$(REMOVE) $(HEXEEPROMDMP)
	$(REMOVE) $(HEXFLASHDMP)
	$(REMOVE) $(FUSES)
#####                    EOF                   #####
