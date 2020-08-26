include ../tools/Makefile.efi
# Package specific C flags
CFLAGS += -Iinclude

APP := efi/yaub/yaub.efi
OBJ := ldr.o menu.o config.o

all: $(APP)

$(APP): $(OBJ)
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(APP) $(OBJ)
