LIBEFI = libefi
include libefi/tools/Makefile.efi

# Package specific C flags
CFLAGS += -std=c99

# Subsystem ID (EFI Application)
SUBSYSTEM := 10

APP := efi/yaub/yaub.efi
OBJ := src/ldr.o src/menu.o src/config.o

all: $(APP)

$(APP): $(OBJ)
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(APP) $(OBJ)
