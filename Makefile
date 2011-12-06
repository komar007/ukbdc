CC=avr-gcc -Os -std=c99 -lm -mmcu=atmega32u4 -g -ggdb
WARN=-Wall -Wextra
SOURCES=main.c usb.c usb_config.c usb_hardware.c hid.c

all: dep
	make firmware.hex

dep:
	@echo -en > Makefile.dep
	@for s in $(SOURCES); do \
		avr-gcc -M $$s >> Makefile.dep; \
		echo -e '\t'@echo CC $$s >> Makefile.dep; \
		echo -e '\t'@$(CC) $(WARN) -c $$s >> Makefile.dep; \
	done

-include Makefile.dep

firmware.elf: main.o usb.o usb_config.o usb_hardware.o hid.o
	@echo LINK $@
	@$(CC) -o $@ $^

%.hex: %.elf
	avr-objcopy -O ihex $^ $@

program: firmware.hex
	sudo dfu-programmer atmega32u4 erase
	sudo dfu-programmer atmega32u4 flash firmware.hex
	sudo dfu-programmer atmega32u4 start

clean:
	rm -fr *.o firmware.* Makefile.dep
