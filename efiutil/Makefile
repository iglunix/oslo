ARCH ?= x86_64
LIBEFI := ..
include ../tools/Makefile-$(ARCH).efi

# Enable EFI 1.10+ APIs
CFLAGS += -DUSE_EFI110

.PHONY: all
all: libefiutil-$(ARCH).a

libefiutil-$(ARCH).a: efiutil.o string.o print.o
	$(AR) -cr $@ $^

.PHONY: clean
clean:
	rm -f *.o *.a
