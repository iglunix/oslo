.POSIX:

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

OBJS=efiutil/efiutil.o efiutil/print.o efiutil/string.o src/config.o src/ldr.o src/menu.o

CFLAGS=-Iinclude -Iefiutil/include \
--target=x86_64-unknown-windows \
-ffreestanding \
-fshort-wchar \
-mno-red-zone

LDFLAGS=--target=x86_64-unknown-windows \
-nostdlib \
-Wl,-entry:efi_main \
-Wl,-subsystem:efi_application \
-fuse-ld=lld-link

yaub.efi: $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

.PHONY: qemu

qemu: yaub.efi
	cp yaub.efi efi/boot/bootx64.efi
	qemu-system-x86_64 -bios fw/x64/OVMF_CODE.fd -hda fat:rw:.
	

clean:
	rm -f $(OBJS)
	rm -f yaub.efi
