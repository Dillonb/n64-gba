cmake_minimum_required(VERSION 3.13)
project(n64-gba)
enable_language(C ASM)

# Setup toolchain
set(tools /usr)
set(CMAKE_C_COMPILER ${tools}/bin/mips64-elf-gcc)
set(CMAKE_C_LINKER ${tools}/bin/mips64-elf-ld)
set(CMAKE_CXX_COMPILER ${tools}/bin/mips64-elf-g++)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})
set(CMAKE_OBJCOPY ${tools}/bin/mips64-elf-objcopy)
set(N64TOOL ${tools}/bin/n64tool)
set(CHKSUM64 ${tools}/bin/chksum64)
set(MKDFS ${tools}/bin/mkdfs)

set(LINKER_SCRIPT n64.ld)
include_directories(${tools}/mips64-elf/include)
add_compile_options(-std=gnu99 -march=vr4300 -mtune=vr4300)
set(LIBS_DIR ${tools}/mips64-elf/lib)

set(CMAKE_C_LINK_EXECUTABLE   "${CMAKE_C_LINKER} -o <TARGET> <OBJECTS> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> -L${LIBS_DIR} <LINK_LIBRARIES> -T${LINKER_SCRIPT}")

add_subdirectory(src/arm7tdmi)

# Build and link main code to ELF format
add_executable(code.elf src/main.c src/gba.c src/gba.h src/util.h src/log.h src/bus.c src/bus.h src/ioreg_util.h src/ioreg_names.h src/ppu.c src/ppu.h)
target_compile_options(code.elf PUBLIC -Wall -Werror -O2)
target_link_libraries(code.elf PUBLIC dragon c m dragonsys arm7tdmi)

# Build filesystem
add_custom_command(OUTPUT fs.dfs
        COMMAND ${MKDFS} fs.dfs ${CMAKE_CURRENT_SOURCE_DIR}/fs
        DEPENDS fs/armwrestler.gba) # TODO: glob

# Package as a .z64 ROM
set(N64_HEADER ${tools}/mips64-elf/lib/header)
set(N64_FLAGS -l 2M -h ${N64_HEADER})
add_custom_command(OUTPUT n64-gba.z64
        COMMAND ${CMAKE_OBJCOPY} code.elf n64-gba.bin -O binary
        COMMAND ${N64TOOL} ${N64_FLAGS} -o n64-gba.z64 -t "GBA Emu PoC" n64-gba.bin -s 1M fs.dfs
        COMMAND rm n64-gba.bin
        COMMAND ${CHKSUM64} n64-gba.z64
        DEPENDS code.elf fs.dfs)

add_custom_target(TARGET-n64-gba.z64 ALL DEPENDS n64-gba.z64)

add_custom_target(console UNFLoader -r n64-gba.z64
        DEPENDS n64-gba.z64)
