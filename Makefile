SHELL=/bin/bash
.DEFAULT_GOAL := all
# Clear suffixes to disable implicit rules
.SUFFIXES:
include Makefile.config

ifeq ($(OS), Windows_NT)
	RM = rmdir /s /q
	MKDIR = mkdir
	fix_path = $(subst /,\,$1)
else
	RM = rm -fr
	MKDIR = mkdir -p
	fix_path = $1
endif

define RULES_TEMPLATE=
obj/$1:
	$(MKDIR) $(call fix_path, obj/$1)
obj/$1/%.d: %.c | obj/$1
	@$(CC) $(CFLAGS) $$(OPTS_$1) -DPLATFORM_$1 -MM -MT "$$(@:.d=.o) $$@" $$< > $$@
obj/$1/%.o: %.c | obj/$1
	$(CC) $(CFLAGS) $$(OPTS_$1) -DPLATFORM_$1 -c -o $$@ $$<
endef
$(foreach target,$(TARGETS),$(eval $(call RULES_TEMPLATE,$(target))))

define INCLUDE_TEMPLATE=
-include $$(SOURCES_$1:%.c=obj/$1/%.d) $$(BASE_SOURCES:%.c=obj/$1/%.d)
endef
$(foreach target,$(TARGETS),$(eval $(call INCLUDE_TEMPLATE,$(target))))

define RESULT_DEPS_TEMPLATE=
bin/$$(RESULT_$1).elf: $$(SOURCES_$1:%.c=obj/$1/%.o) $(BASE_SOURCES:%.c=obj/$1/%.o) | bin/
	$(CC) $(CFLAGS) $(LDFLAGS) $$(OPTS_$1) -o $$@ $$^
endef
$(foreach target,$(TARGETS),$(eval $(call RESULT_DEPS_TEMPLATE,$(target))))

bin/:
	$(MKDIR) bin

bin/%.hex: bin/%.elf
	avr-objcopy -O ihex $^ $@

define TARGET_TEMPLATE=
.PHONY: $1
$1: bin/$$(RESULT_$1).hex
endef
$(foreach target,$(TARGETS),$(eval $(call TARGET_TEMPLATE,$(target))))

.PHONY: clean all
all: $(TARGETS)

clean:
	$(RM) bin obj
