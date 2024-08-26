# Nuke built-in rules and variables.
override MAKEFLAGS += -rR

override IMAGE_NAME := RedRosesOS

# Convenience macro to reliably declare user overridable variables.
define DEFAULT_VAR =
    ifeq ($(origin $1),default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1),undefined)
        override $(1) := $(2)
    endif
endef

.PHONY: all
all: $(IMAGE_NAME).iso

.PHONY: all-hdd
all-hdd: $(IMAGE_NAME).hdd

.PHONY: run
run: $(IMAGE_NAME).iso
	qemu-system-x86_64 -M q35 -m 2G -cdrom $(IMAGE_NAME).iso -boot d -audiodev pa,id=snd0 -machine pcspk-audiodev=snd0

.PHONY: run-uefi
run-uefi: ovmf $(IMAGE_NAME).iso
	qemu-system-x86_64 -M q35 -m 2G -bios ovmf/OVMF.fd -cdrom $(IMAGE_NAME).iso -boot d

.PHONY: run-hdd
run-hdd: $(IMAGE_NAME).hdd
	qemu-system-x86_64 -M q35 -m 2G -hda $(IMAGE_NAME).hdd -audiodev pa,id=snd0 -machine pcspk-audiodev=snd0

.PHONY: run-img
run-img: $(IMAGE_NAME).img
	qemu-system-x86_64 -M pc -m 2G -drive file=RedRosesOS.img,format=raw -audiodev pa,id=snd0 -machine pcspk-audiodev=snd0 -monitor stdio

.PHONY: run-hdd-uefi
run-hdd-uefi: ovmf $(IMAGE_NAME).hdd
	qemu-system-x86_64 -M q35 -m 2G -bios ovmf/OVMF.fd -hda $(IMAGE_NAME).hdd

ovmf:
	mkdir -p ovmf
	cd ovmf && curl -Lo OVMF.fd https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd

limine/limine:
	rm -rf limine
	git clone https://github.com/limine-bootloader/limine.git --branch=v7.x-binary --depth=1
	$(MAKE) -C limine

.PHONY: kernel
kernel:
	$(MAKE) -C kernel

$(IMAGE_NAME).iso: limine/limine kernel
	rm -rf iso_root
	mkdir -p iso_root/boot
	cp -v kernel/bin/kernel iso_root/boot/
	mkdir -p iso_root/boot/limine
	cp -v limine.cfg limine/limine-bios.sys limine/limine-bios-cd.bin limine/limine-uefi-cd.bin iso_root/boot/limine/
	mkdir -p iso_root/EFI/BOOT
	cp -v limine/BOOTX64.EFI iso_root/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI iso_root/EFI/BOOT/
	xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $(IMAGE_NAME).iso
	./limine/limine bios-install $(IMAGE_NAME).iso
	rm -rf iso_root

$(IMAGE_NAME).hdd: limine/limine kernel
	rm -f $(IMAGE_NAME).hdd
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(IMAGE_NAME).hdd
	sgdisk $(IMAGE_NAME).hdd -n 1:2048 -t 1:ef00
	./limine/limine bios-install $(IMAGE_NAME).hdd
	mformat -i $(IMAGE_NAME).hdd@@1M 
	mmd -i $(IMAGE_NAME).hdd@@1M ::/EFI ::/EFI/BOOT ::/boot ::/boot/limine
	mcopy -i $(IMAGE_NAME).hdd@@1M kernel/bin/kernel ::/boot
	mcopy -i $(IMAGE_NAME).hdd@@1M limine.cfg limine/limine-bios.sys ::/boot/limine
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i $(IMAGE_NAME).hdd@@1M limine/BOOTIA32.EFI ::/EFI/BOOT

$(IMAGE_NAME).img: limine/limine kernel
	rm -f $(IMAGE_NAME).img
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(IMAGE_NAME).img
	parted -s $(IMAGE_NAME).img mklabel msdos
	parted -s $(IMAGE_NAME).img mkpart primary fat32 1M 100%
	parted -s $(IMAGE_NAME).img set 1 boot on
	./limine/limine bios-install $(IMAGE_NAME).img
	mformat -i $(IMAGE_NAME).img@@1M 
	mmd -i $(IMAGE_NAME).img@@1M ::/EFI ::/EFI/BOOT ::/boot ::/boot/limine
	mcopy -i $(IMAGE_NAME).img@@1M kernel/bin/kernel ::/boot
	mcopy -i $(IMAGE_NAME).img@@1M bg.jpg ::/boot
	mcopy -i $(IMAGE_NAME).img@@1M limine.cfg limine/limine-bios.sys ::/boot/limine
	mcopy -i $(IMAGE_NAME).img@@1M limine/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i $(IMAGE_NAME).img@@1M limine/BOOTIA32.EFI ::/EFI/BOOT

.PHONY: clean
clean:
	rm -rf iso_root $(IMAGE_NAME).iso $(IMAGE_NAME).hdd
	$(MAKE) -C kernel clean

.PHONY: distclean
distclean: clean
	rm -rf limine ovmf
	$(MAKE) -C kernel distclean
