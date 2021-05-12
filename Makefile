ARCH ?= x86_64
LIBEFI = libefi
include libefi/tools/Makefile-$(ARCH).efi

# Package specific C flags
CFLAGS += -std=c99 -DBUILD_ARCH=\"$(ARCH)\"

# Subsystem ID (EFI Application)
SUBSYSTEM := 10

APP := efi/yaub/yaub.efi
OBJ := src/ldr.o src/menu.o src/config.o

.PHONY: all
all: $(APP)

$(APP): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)

.PHONY: clean
clean:
	rm -f $(APP) $(OBJ)
