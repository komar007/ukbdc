CC=gcc -O2 -std=gnu99 -lm -g -ggdb
WARN=-Wall -Wextra
SOURCES=main.c
PLATFORMS=linux

all: dep
	@for p in $(PLATFORMS); do \
		make $$p; \
	done

linux: prep
	make bin/ukbdc_cli

prep:
	@for p in $(PLATFORMS); do \
		mkdir -p bin/platforms/$$p; \
	done;

dep:
	@for p in $(PLATFORMS); do \
		echo -en > Makefile.dep-$$p; \
		for s in $(SOURCES); do \
			gcc -DPLATFORM_$$p -M -MT bin/platforms/$$p/$${s%.c}.o $$s >> Makefile.dep-$$p; \
			echo -e '\t'@echo CC $$s >> Makefile.dep-$$p; \
			echo -e '\t'@$(CC) -DPLATFORM_$$p $(WARN) -c $$s -o bin/platforms/$$p/$${s%.c}.o >> Makefile.dep-$$p; \
		done \
	done

-include Makefile.dep*

bin/ukbdc_cli: \
	bin/platforms/linux/main.o
	@echo LINK $@
	@$(CC) -o $@ $^

clean:
	rm -fr bin Makefile.dep-*
