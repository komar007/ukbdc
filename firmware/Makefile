CC=avr-gcc -Os -std=gnu99 -lm -mmcu=atmega32u4 -g -ggdb
WARN=-Wall -Wextra
SOURCES=\
	main_ikea.c\
	main_alpha.c\
	main_xt.c \
	main_gh60.c \
	main_ghpad.c \
	io.c\
	platforms.c\
	usb.c\
	usb_config.c\
	usb_hardware.c\
	hid.c\
	rawhid.c\
	dataflash.c\
	rawhid_protocol.c\
	layout.c\
	hc595.c \
	scanner.c \
	matrix.c \
	atmel_bootloader.c \
	crc.c
# removed other platforms while layer support is being rewritten
#PLATFORMS=ikea alpha xt gh60
PLATFORMS=gh60

all: dep
	@for p in $(PLATFORMS); do \
		make $$p; \
	done

ikea: prep
	make bin/ukbdc-ikea.hex
alpha: prep
	make bin/ukbdc-alpha.hex
xt: prep
	make bin/ukbdc-xt.hex
gh60: prep
	make bin/ukbdc-gh60.hex
ghpad: prep
	make bin/ukbdc-ghpad.hex

prep:
	@for p in $(PLATFORMS); do \
		mkdir -p bin/platforms/$$p; \
	done;

dep:
	@for p in $(PLATFORMS); do \
		echo -en > .Makefile.dep-$$p; \
		for s in $(SOURCES); do \
			avr-gcc -DPLATFORM_$$p -M -MT bin/platforms/$$p/$${s%.c}.o $$s >> .Makefile.dep-$$p; \
			echo -e '\t'@echo CC $$s >> .Makefile.dep-$$p; \
			echo -e '\t'@$(CC) -DPLATFORM_$$p $(WARN) -c $$s -o bin/platforms/$$p/$${s%.c}.o >> .Makefile.dep-$$p; \
		done \
	done

-include .Makefile.dep*

bin/ukbdc-ikea.elf: \
	bin/platforms/ikea/main_ikea.o \
	bin/platforms/ikea/io.o \
	bin/platforms/ikea/platforms.o \
	bin/platforms/ikea/usb.o \
	bin/platforms/ikea/usb_config.o \
	bin/platforms/ikea/usb_hardware.o \
	bin/platforms/ikea/hid.o \
	bin/platforms/ikea/rawhid.o \
	bin/platforms/ikea/rawhid_protocol.o \
	bin/platforms/ikea/scanner.o \
	bin/platforms/ikea/layout.o
	@echo LINK $@
	@$(CC) -o $@ $^

bin/ukbdc-gh60.elf: \
	bin/platforms/gh60/main_gh60.o \
	bin/platforms/gh60/io.o \
	bin/platforms/gh60/platforms.o \
	bin/platforms/gh60/usb.o \
	bin/platforms/gh60/usb_config.o \
	bin/platforms/gh60/usb_hardware.o \
	bin/platforms/gh60/hid.o \
	bin/platforms/gh60/rawhid.o \
	bin/platforms/gh60/rawhid_protocol.o \
	bin/platforms/gh60/matrix.o \
	bin/platforms/gh60/scanner.o \
	bin/platforms/gh60/crc.o \
	bin/platforms/gh60/atmel_bootloader.o \
	bin/platforms/gh60/layout.o
	@echo LINK $@
	@$(CC) -o $@ $^

bin/ukbdc-ghpad.elf: \
	bin/platforms/ghpad/main_ghpad.o \
	bin/platforms/ghpad/io.o \
	bin/platforms/ghpad/platforms.o \
	bin/platforms/ghpad/usb.o \
	bin/platforms/ghpad/usb_config.o \
	bin/platforms/ghpad/usb_hardware.o \
	bin/platforms/ghpad/hid.o \
	bin/platforms/ghpad/rawhid.o \
	bin/platforms/ghpad/rawhid_protocol.o \
	bin/platforms/ghpad/matrix.o \
	bin/platforms/ghpad/crc.o \
	bin/platforms/ghpad/atmel_bootloader.o \
	bin/platforms/ghpad/layout.o
	@echo LINK $@
	@$(CC) -o $@ $^

bin/ukbdc-alpha.elf: \
	bin/platforms/alpha/main_alpha.o \
	bin/platforms/alpha/io.o \
	bin/platforms/alpha/platforms.o \
	bin/platforms/alpha/usb.o \
	bin/platforms/alpha/usb_config.o \
	bin/platforms/alpha/usb_hardware.o \
	bin/platforms/alpha/hid.o \
	bin/platforms/alpha/rawhid.o \
	bin/platforms/alpha/rawhid_protocol.o \
	bin/platforms/alpha/dataflash.o \
	bin/platforms/alpha/layout.o \
	bin/platforms/alpha/scanner.o \
	bin/platforms/alpha/hc595.o
	@echo LINK $@
	@$(CC) -o $@ $^

bin/ukbdc-xt.elf: \
	bin/platforms/alpha/main_xt.o \
	bin/platforms/alpha/io.o \
	bin/platforms/alpha/platforms.o \
	bin/platforms/alpha/usb.o \
	bin/platforms/alpha/usb_config.o \
	bin/platforms/alpha/usb_hardware.o \
	bin/platforms/alpha/hid.o \
	bin/platforms/alpha/rawhid.o \
	bin/platforms/alpha/rawhid_protocol.o \
	bin/platforms/alpha/dataflash.o \
	bin/platforms/alpha/layout.o \
	bin/platforms/alpha/scanner.o \
	bin/platforms/alpha/hc595.o
	@echo LINK $@
	@$(CC) -o $@ $^

bin/%.hex: bin/%.elf
	avr-objcopy -O ihex $^ $@

program-ikea: bin/ukbdc-ikea.hex
	#sudo ../ukbdc_cli/bin/ukbdc_cli
	sleep 1;
	sudo dfu-programmer atmega32u4 erase
	sudo dfu-programmer atmega32u4 flash bin/ukbdc-ikea.hex
	sudo dfu-programmer atmega32u4 start

program-alpha: bin/ukbdc-alpha.hex
	sudo dfu-programmer atmega32u4 erase
	sudo dfu-programmer atmega32u4 flash bin/ukbdc-alpha.hex
	sudo dfu-programmer atmega32u4 start

program-gh60: bin/ukbdc-gh60.hex
	#sudo ../ukbdc_cli/bin/ukbdc_cli /dev/hidraw1 dfu
	sleep 2
	sudo dfu-programmer atmega32u4 erase
	sudo dfu-programmer atmega32u4 flash bin/ukbdc-gh60.hex
	sudo dfu-programmer atmega32u4 start

program-xt: bin/ukbdc-xt.hex
	sudo dfu-programmer atmega32u4 erase
	sudo dfu-programmer atmega32u4 flash bin/ukbdc-xt.hex
	sudo dfu-programmer atmega32u4 start

clean:
	rm -fr bin .Makefile.dep-*
