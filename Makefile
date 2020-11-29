LIBEFI = libefi
include libefi/tools/Makefile.efi

# Package specific C flags
CFLAGS += -Iinclude

# Subsystem ID (EFI Application)
SUBSYSTEM := 10

APP := efi/yaub/yaub.efi
OBJ := ldr.o menu.o config.o

all: $(APP)

$(APP): $(OBJ)
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(APP) $(OBJ)
