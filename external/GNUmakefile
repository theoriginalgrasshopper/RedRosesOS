CC = gcc
CFLAGS = -Wall \
    -Wextra \
    -std=gnu11 \
    -ffreestanding \
    -fno-stack-protector \
    -fno-stack-check \
    -fno-lto \
    -nostdlib \
    -m64 \
    -march=x86-64 \
    -mno-80387 \
    -mno-mmx \
    -mno-sse \
    -mno-sse2 \
    -mno-red-zone \
    -fPIE \
    -fPIC

APPSDIR = ./apps/
BINDIR = ./bin/
LDFLAGS = -melf_x86_64 -static -e main
C_FILES = $(shell find $(APPSDIR) -type f -name "*.c")
EXES = $(patsubst %.c,%.reb,$(notdir $(C_FILES)))

# default target
all: $(patsubst %.reb,$(BINDIR)%.reb,$(EXES))

$(BINDIR)%.reb: $(APPSDIR)%.c
	@echo "Compiling $< to $@..."
	@$(CC) -c $(CFLAGS) -o $(@:.reb=.o) -I$(APPSDIR) $<
	@ld $(LDFLAGS) $(@:.reb=.o) -o $@
	@rm -f $(@:.reb=.o)
	@size=$$(($$(wc -c < $@)));\
	newsize=$$((size - $$((size % 512)) + 512));\
	echo "$@" "$$size ($$(printf '0x%02X' $$((size / 512))) sectors) ->" "$$newsize ($$(printf '0x%02X' $$((newsize / 512))) sectors)";\
	dd if=/dev/zero of=$@ bs=1 seek=$$size count=$$((newsize - size)) status=none;

clean:
	rm -f $(BINDIR)*.reb ./*.o
