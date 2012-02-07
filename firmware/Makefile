CC=avr-gcc -Os -std=gnu99 -lm -mmcu=atmega32u4 -g -ggdb
WARN=-Wall -Wextra
SOURCES=\
	main_ikea.c\
	main_alpha.c\
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
	hc595.c
PLATFORMS=ikea alpha

all: dep
	@for p in $(PLATFORMS); do \
		make $$p; \
	done

ikea: prep
	make bin/ukbdc-ikea.hex
alpha: prep
	make bin/ukbdc-alpha.hex

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
	bin/platforms/ikea/layout.o
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
	bin/platforms/alpha/hc595.o
	@echo LINK $@
	@$(CC) -o $@ $^

bin/%.hex: bin/%.elf
	avr-objcopy -O ihex $^ $@

program-ikea: bin/ukbdc-ikea.hex
	sudo ../ukbdc_cli/bin/ukbdc_cli
	sleep 1;
	sudo dfu-programmer atmega32u4 erase
	sudo dfu-programmer atmega32u4 flash bin/ukbdc-ikea.hex
	sudo dfu-programmer atmega32u4 start

program-alpha: bin/ukbdc-alpha.hex
	sudo dfu-programmer atmega32u4 erase
	sudo dfu-programmer atmega32u4 flash bin/ukbdc-alpha.hex
	sudo dfu-programmer atmega32u4 start

clean:
	rm -fr bin .Makefile.dep-*
