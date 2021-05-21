# YAUB
Boot manager for (U)EFI, using my `libefi` development kit.

## Usage
1. Either build from source by running `make`, or download one of the pre-built releases
2. Copy the `efi` directory to the EFI System partition (usually mounted at `/boot/efi/`)
3. Add an entry to the firmware's boot manager to start YAUB:
  * AMD64 (x86_64):
  ```shell
  efibootmgr -c -l "\efi\yaub\yaubx64.efi" -L "YAUB (64-bit)"
  ```
  * i386 (IA-32):
  ```shell
  efibootmgr -c -l "\efi\yaub\yaubia32.efi" -L "YAUB (32-bit)"
  ```
4. Edit the configuration file at `\efi\yaub\entries.ini` (on the ESP) to add your
  desired boot options
