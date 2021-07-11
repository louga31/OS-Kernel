cmake_minimum_required(VERSION 3.18)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(src
        memory/Paging/PageFrameAllocator.cpp
        memory/Paging/PageFrameAllocator.h
        memory/Paging/PageMapIndexer.cpp
        memory/Paging/PageMapIndexer.h
        memory/Paging/paging.h
        BasicRenderer.cpp
        BasicRenderer.h
        Bitmap.cpp
        Bitmap.h
        cstr.cpp
        cstr.h
        memory/efiMemory.cpp
        memory/efiMemory.h
        Framebuffer.h
        kernel.cpp
        math.h
        memory/memutils.cpp
        memory/memutils.h
        SimpleFont.h
        types.h memory/Paging/PageTableManager.cpp memory/Paging/PageTableManager.h memory/Paging/paging.cpp kernelutils.h kernelutils.cpp GDT/GDT.cpp GDT/GDT.h interrupts/IDT.cpp interrupts/IDT.h interrupts/interrupts.cpp interrupts/interrupts.h panic.cpp panic.h IO.cpp IO.h userinput/kbScancodeTranslation.cpp userinput/kbScancodeTranslation.h userinput/keyboard.cpp userinput/keyboard.h userinput/mouse.cpp userinput/mouse.h userinput/cursor.h PCI/acpi.h PCI/acpi.cpp PCI/pci.cpp PCI/pci.h PCI/pciDescriptors.cpp memory/heap.cpp memory/heap.h scheduling/pit/pit.cpp scheduling/pit/pit.h)
