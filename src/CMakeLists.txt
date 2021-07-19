cmake_minimum_required(VERSION 3.18)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(src
        ahci/ahci.cpp
        ahci/ahci.h
        GDT/GDT.asm
        GDT/GDT.cpp
        GDT/GDT.h
        interrupts/IDT.cpp
        interrupts/IDT.h
        interrupts/interrupts.cpp
        interrupts/interrupts.h
        memory/Paging/PageFrameAllocator.cpp
        memory/Paging/PageFrameAllocator.h
        memory/Paging/PageMapIndexer.cpp
        memory/Paging/PageMapIndexer.h
        memory/Paging/PageTableManager.cpp
        memory/Paging/PageTableManager.h
        memory/Paging/paging.cpp
        memory/Paging/paging.h
        memory/efiMemory.cpp
        memory/efiMemory.h
        memory/heap.cpp
        memory/heap.h
        memory/memutils.cpp
        memory/memutils.h
        PCI/acpi.cpp
        PCI/acpi.h
        PCI/pci.cpp
        PCI/pci.h
        PCI/pciDescriptors.cpp
        scheduling/pit/pit.cpp
        scheduling/pit/pit.h
        userinput/cursor.h
        userinput/kbScancodeTranslation.cpp
        userinput/kbScancodeTranslation.h
        userinput/keyboard.cpp
        userinput/keyboard.h
        userinput/mouse.cpp
        userinput/mouse.h
        BasicRenderer.cpp
        BasicRenderer.h
        Bitmap.cpp
        Bitmap.h
        cstr.cpp
        cstr.h
        Framebuffer.h
        IO.cpp
        IO.h
        kernel.cpp
        kernelutils.cpp
        kernelutils.h
        math.h
        panic.cpp
        panic.h
        SimpleFont.h
        types.h filesystem/fat.cpp filesystem/fat.h string.cpp string.h)
