IMG_PREF = fluximage
ELF = kernel/flux.elf
ARCH = X64
export

.PHONY: all
all: $(IMG_PREF).iso

.PHONY: all-hdd
all-hdd: $(IMG_PREF).hdd

.PHONY: run
run: $(IMG_PREF).iso
	qemu-system-x86_64 -M q35 -m 2G -cdrom $< -boot d

.PHONY: run-hdd
run-hdd: $(IMG_PREF).hdd
	qemu-system-x86_64 -M q35 -m 2G -hda $(IMG_PREF).hdd
	
limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v4.x-branch-binary --depth=1
	make -C limine

.PHONY: kernel
kernel:
	$(MAKE) -C kernel

$(IMG_PREF).iso: limine kernel
	rm -rf iso_root
	mkdir -p iso_root $(IMG_PREF)
	cp $(ELF) \
		limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/
	xorriso -as mkisofs -b limine-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-cd-efi.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		iso_root -o $@
	limine/limine-deploy $@
	rm -rf iso_root

$(IMG_PREF).hdd: limine kernel
	rm -f $(IMG_PREF).hdd
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(IMG_PREF).hdd
	parted -s $(IMG_PREF).hdd mklabel gpt
	parted -s $(IMG_PREF).hdd mkpart ESP fat32 2048s 100%
	parted -s $(IMG_PREF).hdd set 1 esp on
	limine/limine-deploy $(IMG_PREF).hdd
	sudo losetup -Pf --show $(IMG_PREF).hdd >loopback_dev
	sudo mkfs.fat -F 32 `cat loopback_dev`p1
	mkdir -p img_mount
	sudo mount `cat loopback_dev`p1 img_mount
	sudo mkdir -p img_mount/EFI/BOOT
	sudo cp -v $(ELF) limine.cfg limine/limine.sys img_mount/
	sudo cp -v limine/BOOTX64.EFI img_mount/EFI/BOOT/
	sync
	sudo umount img_mount
	sudo losetup -d `cat loopback_dev`
	rm -rf loopback_dev img_mount

.PHONY: clean
clean:
	rm -rf iso_root fluximage.iso $(IMG_PREF).hdd
	$(MAKE) -C kernel clean

.PHONY: distclean
distclean: clean
	rm -rf limine ovmf-x64
	$(MAKE) -C kernel distclean
