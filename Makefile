
OSNAME = CustomOS

KERNEL = kernel.elf

GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = kernel.ld
MAKE = make
CC = clang++
ASMC = nasm
LD = ld.lld
EMU=qemu-system-x86_64
DEBUG=gdb

CFLAGS = -ffreestanding -fshort-wchar -fno-exceptions -g -std=c++17 -MD -mno-red-zone -fno-threadsafe-statics -I $(SRCDIR)
ASMFLAGS =
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi


rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)    
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)      
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

all: buildimg

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(OBJDIR)

kernel: $(OBJS) link

bootloader:
	cd $(GNUEFI) && $(MAKE) bootloader

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@echo !==== COMPILING $<
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@echo !==== ASSEMBLING $^
	@mkdir -p $(@D)
	@$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@

link:
	@echo !==== LINKING $^
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/$(KERNEL) $(OBJS)

buildimg: kernel bootloader
	@dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=93750
	@mformat -v $(OSNAME) -i $(BUILDDIR)/$(OSNAME).img -F ::
	@mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	@mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	@mcopy -i $(BUILDDIR)/$(OSNAME).img $(BOOTEFI) ::/EFI/BOOT/BOOTX64.EFI
	@mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	@mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/$(KERNEL) ::
	@mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/zap-light16.psf ::

run: buildimg
	@$(EMU) -machine q35 -s -drive file=$(BUILDDIR)/$(OSNAME).img -m 256M -cpu qemu64 -bios "$(OVMFDIR)/OVMF-pure-efi.fd" -net none

.PHONY: clean
clean:
	@rm -rf $(OBJDIR)/* $(BUILDDIR)/*.elf $(BUILDDIR)/*.img

debug:
	@$(DEBUG) $(BUILDDIR)/$(KERNEL) -ex "target remote :1234"

-include $(OBJS:.o=.d)