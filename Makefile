include ../tools/Makefile.efi
# Package specific C flags
CFLAGS += -Iinclude

all: ldr.efi

ldr.efi: ldr.o menu.o config.o
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f *.o *.efi
