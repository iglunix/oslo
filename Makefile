ifeq ($(USE_MINGW),1)
CC          = x86_64-w64-mingw32-gcc
LD          = $(CC)
EFICFLAGS   = -Iapi -Iinclude -ffreestanding -mno-red-zone -fno-stack-protector -fshort-wchar
EFILDFLAGS  = -nostdlib -shared -T tools/x86_64-efi.ld -e efi_main -Wl,-subsystem,10
else
CC          = gcc
LD          = ld
EFICFLAGS   = -Iapi -Iinclude -ffreestanding -mno-red-zone -fno-stack-protector -fshort-wchar
EFILDFLAGS  = -shared -e efi_main -T tools/x86_64-efi.ld -m i386pep -subsystem 10
endif

all: ldr.efi

ldr.efi: ldr.o util.o menu.o

%.o: %.c
	$(CC) $(EFICFLAGS) -c $^ -o $@

%.efi: %.o
	$(LD) $(EFILDFLAGS) $^ -o $@

clean:
	rm -f *.o *.efi
