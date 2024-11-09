override MAKEFLAGS += -rR

# Rose Flat Binary
override EXECUTABLE := ROSEBIN.REB

define DEFAULT_VAR =
    ifeq ($(origin $1),default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1),undefined)
        override $(1) := $(2)
    endif
endef

# C++ FLAGS EDITABLE -------------------------------------
override DEFAULT_KCC := cc
$(eval $(call DEFAULT_VAR,KCC,$(DEFAULT_KCC)))
override DEFAULT_KCPP := g++
$(eval $(call DEFAULT_VAR,KCPP,$(DEFAULT_KCPP)))
override DEFAULT_KLD := ld
$(eval $(call DEFAULT_VAR,KLD,$(DEFAULT_KLD)))

# C FLAGS EDITABLE -------------------------------------
override DEFAULT_KCFLAGS := -g -O2 -pipe -fno-stack-protector
$(eval $(call DEFAULT_VAR,KCFLAGS,$(DEFAULT_KCFLAGS)))

override DEFAULT_KCXXFLAGS := -g -O2 -pipe -fno-stack-protector
$(eval $(call DEFAULT_VAR,KCXXFLAGS,$(DEFAULT_KCXXFLAGS)))

# C PREPROCESSOR FLAGS EDITABLE ------------------------------
override DEFAULT_KCPPFLAGS :=
$(eval $(call DEFAULT_VAR,KCPPFLAGS,$(DEFAULT_KCPPFLAGS)))

# NASM FLAGS EDITABLE ----------------------------------------
override DEFAULT_KNASMFLAGS := -F dwarf -g
$(eval $(call DEFAULT_VAR,KNASMFLAGS,$(DEFAULT_KNASMFLAGS)))

# LINKER FLAGS EDITABLE ------------------------------------------
override DEFAULT_KLDFLAGS := -Ttext 0x0 -nostdlib -n -oformat=binary -o $@
$(eval $(call DEFAULT_VAR,KLDFLAGS,$(DEFAULT_KLDFLAGS)))

# DO NOT CHANGE
override KCXXFLAGS += \
    -Wall \
    -Wextra \
    -std=gnu++17 \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -m64 \
    -march=x86-64

# DO NOT CHANGE
override KCFLAGS += \
    -Wall \
    -Wextra \
    -std=gnu11 \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -m64 \
    -march=x86-64

# DO NOT CHANGE
override KCPPFLAGS := \
    -I apps \
    $(KCPPFLAGS) \
    -MMD \
    -MP

# DO NOT CHANGE
override KNASMFLAGS += \
    -Wall \
    -f bin

override KLDFLAGS += \
    -nostdlib \
    -T linker.ld

# FIND ALL C, CPP, ASM, NASM, S FILES
override CFILES := $(shell cd apps && find -L * -type f -name '*.c')
override ASFILES := $(shell cd apps && find -L * -type f -name '*.S')
override NASMFILES := $(shell cd apps && find -L * -type f -name '*.asm')
override OBJ := $(addprefix obj/,$(CFILES:.c=.c.o) $(ASFILES:.S=.S.o) $(NASMFILES:.asm=.asm.o))
override HEADER_DEPS := $(addprefix obj/,$(CFILES:.c=.c.d) $(ASFILES:.S=.S.d))

# DEFAULT TARGET
.PHONY: all
all: bin/$(EXECUTABLE)

# LINK RULES
bin/$(EXECUTABLE): GNUmakefile $(OBJ)
	mkdir -p "$$(dirname $@)"
	$(KLD) $(OBJ) $(KLDFLAGS) -o $@

# C RULES
obj/%.c.o: apps/%.c GNUmakefile
	mkdir -p "$$(dirname $@)"
	$(KCC) $(KCFLAGS) $(KCPPFLAGS) -c $< -o $@

# S RULES
obj/%.S.o: apps/%.S GNUmakefile
	mkdir -p "$$(dirname $@)"
	$(KCC) $(KCFLAGS) $(KCPPFLAGS) -c $< -o $@

# NASM RULES
obj/%.asm.o: apps/%.asm GNUmakefile
	mkdir -p "$$(dirname $@)"
	nasm $(KNASMFLAGS) $< -o $@

# CLEANUP
.PHONY: clean
clean:
	rm -rf bin obj
