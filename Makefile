.POSIX:

ARCH?=x86_64

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

OBJS=efiutil/efiutil.o efiutil/print.o efiutil/string.o src/config.o src/ldr.o src/menu.o

CFLAGS=-Iinclude -Iefiutil/include \
--target=$(ARCH)-unknown-windows \
-ffreestanding \
-fshort-wchar \
-mno-red-zone

LDFLAGS=--target=$(ARCH)-unknown-windows \
-nostdlib \
-Wl,-entry:efi_main \
-Wl,-subsystem:efi_application \
-fuse-ld=lld-link

oslo.efi: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.PHONY: qemu

qemu: oslo.efi
	cp oslo.efi efi/boot/bootx64.efi
	qemu-system-$(ARCH) -bios fw/x64/OVMF.fd -hda fat:rw:.
	

clean:
	rm -f $(OBJS)
	rm -f oslo.efi
