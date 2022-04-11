.POSIX:

ARCH=x86_64

.c.o:
	@echo ' CC' $@
	@$(CC) $(CFLAGS) -c $< -o $@

OBJS=efiutil/efiutil.o efiutil/print.o efiutil/string.o src/config.o src/ldr.o src/menu.o

CFLAGS=-Iinclude -Iinclude/$(ARCH) -Iefiutil/include \
--target=$(ARCH)-unknown-windows \
-ffreestanding \
-fshort-wchar \
-mno-red-zone \
-DBUILD_ARCH=\"$(ARCH)\"

LDFLAGS=--target=$(ARCH)-unknown-windows \
-nostdlib \
-Wl,-entry:efi_main \
-Wl,-subsystem:efi_application \
-fuse-ld=lld-link

oslo.efi: $(OBJS)
	@echo ' LD' $@
	@$(CC) $(LDFLAGS) $(OBJS) -o $@

.PHONY: qemu

qemu: qemu_$(ARCH)

QEMU_OPTS=-hda fat:rw:. -nographic -vga none -serial mon:stdio -net none -m 1024

qemu_x86_64: oslo.efi
	cp oslo.efi efi/boot/bootx64.efi
	qemu-system-$(ARCH) -bios fw/x64/OVMF.fd $(QEMU_OPTS)

qemu_aarch64: oslo.efi
	cp oslo.efi efi/boot/bootaa64.efi
	qemu-system-aarch64 -M virt -cpu cortex-a57 -bios /usr/local/share/qemu/edk2-aarch64-code.fd $(QEMU_OPTS)


clean:
	rm -f $(OBJS)
	rm -f oslo.efi
