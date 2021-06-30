cmake_minimum_required(VERSION 3.18)
project(src)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(Paging)

add_executable(src
        Paging/PageFrameAllocator.cpp
        Paging/PageFrameAllocator.h
        Paging/PageMapIndexer.cpp
        Paging/PageMapIndexer.h
        Paging/paging.h
        BasicRenderer.cpp
        BasicRenderer.h
        Bitmap.cpp
        Bitmap.h
        cstr.cpp
        cstr.h
        efiMemory.cpp
        efiMemory.h
        Framebuffer.h
        kernel.cpp
        math.h
        memutils.cpp
        memutils.h
        SimpleFont.h
        types.h Paging/PageTableManager.cpp Paging/PageTableManager.h Paging/paging.cpp kernelutils.h kernelutils.cpp GDT/GDT.cpp GDT/GDT.h interrupts/IDT.cpp interrupts/IDT.h interrupts/interrupts.cpp interrupts/interrupts.h panic.cpp panic.h IO.cpp IO.h userinput/kbScancodeTranslation.cpp userinput/kbScancodeTranslation.h userinput/keyboard.cpp userinput/keyboard.h)
