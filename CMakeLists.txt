# set cmake min version
cmake_minimum_required(VERSION 3.7)

# enable cmake cross compile
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# specify the cross compiler
set(CMAKE_ASM_COMPILER_WORKS 1)
set(CMAKE_ASM_COMPILER /usr/local/Cellar/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-gcc)
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_C_COMPILER /usr/local/Cellar/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER /usr/local/Cellar/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-g++)
set(AS /usr/local/Cellar/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-as)
set(AR /usr/local/Cellar/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-ar)
set(OBJCOPY /usr/local/Cellar/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-objcopy)
set(OBJDUMP /usr/local/Cellar/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-objdump)
set(SIZE /usr/local/Cellar/gcc-arm-none-eabi-8-2018-q4-major/bin/arm-none-eabi-size)

# set project name
project(STM32H7 C CXX ASM)
set(CMAKE_CXX_STANDARD 14)

# set asm gcc and linker flags
set(MCU_CONFIG              "-mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -mthumb-interwork")
set(LINKER_SCRIPT           "${CMAKE_SOURCE_DIR}/core/linker/STM32H743ZITx_FLASH.ld")

set(COMPILE_ASSEMBLER_FLAGS "${MCU_CONFIG} -specs=nano.specs -c -x assembler-with-cpp -g3")
set(COMPILE_COMPILER_FLAGS  "${MCU_CONFIG} -specs=nano.specs \
                             -ffunction-sections -fdata-sections -fno-common -fmessage-length=0 -fstack-usage -O0 -g3 \
                             -Wall -Wextra -Wfatal-errors -pedantic -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -Wconversion")
set(LINK_FLAGS              "${MCU_CONFIG} -specs=nosys.specs -static \
                             -Wl,--gc-sections -Wl,--cref -Wl,--print-memory-usage -Wl,-Map=${PROJECT_BINARY_DIR}/${PROJECT_NAME}.map \
                             -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group") # -u _printf_float

set(CMAKE_ASM_FLAGS         "${COMPILE_ASSEMBLER_FLAGS}")
set(CMAKE_C_FLAGS           "${COMPILE_COMPILER_FLAGS} -std=gnu11")
set(CMAKE_CXX_FLAGS         "${COMPILE_COMPILER_FLAGS} -std=gnu++14 -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit")
set(CMAKE_EXE_LINKER_FLAGS  "${LINK_FLAGS} -T ${LINKER_SCRIPT}")

# set definitions
add_definitions(-D__FPU_PRESENT=1
                -DARM_MATH_CM4 -DARM_MATH_MATRIX_CHECK -DARM_MATH_ROUNDING )
add_definitions(-D__weak=__attribute__\(\(weak\)\)
                -D__packed=__attribute__\(\(__packed__\)\)
                -DUSE_HAL_DRIVER
                -DSTM32H743xx)

# set include path and generate elf
include_directories(${CMAKE_SOURCE_DIR}/core/cmsis
                    ${CMAKE_SOURCE_DIR}/core/device
                    ${CMAKE_SOURCE_DIR}/driver/bsp
                    ${CMAKE_SOURCE_DIR}/driver/hal
                    ${CMAKE_SOURCE_DIR}/middleware/freertos/include
                    ${CMAKE_SOURCE_DIR}/middleware/freertos/portable
                    ${CMAKE_SOURCE_DIR}/middleware/ucos/config
                    ${CMAKE_SOURCE_DIR}/middleware/ucos/core
                    ${CMAKE_SOURCE_DIR}/middleware/ucos/core/port
                    ${CMAKE_SOURCE_DIR}/middleware/ucos/cpu
                    ${CMAKE_SOURCE_DIR}/middleware/ucos/cpu/port
                    ${CMAKE_SOURCE_DIR}/middleware/ucos/lib
                    ${CMAKE_SOURCE_DIR}/middleware/ucos/lib/port
                    ${CMAKE_SOURCE_DIR}/middleware/trace/config
                    ${CMAKE_SOURCE_DIR}/middleware/trace/rtos
                    ${CMAKE_SOURCE_DIR}/middleware/trace/segger)

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/core/*.h
                          ${CMAKE_SOURCE_DIR}/core/*.c
                          ${CMAKE_SOURCE_DIR}/core/*.s
                          ${CMAKE_SOURCE_DIR}/driver/*.h
                          ${CMAKE_SOURCE_DIR}/driver/*.c
                          ${CMAKE_SOURCE_DIR}/driver/*.cc
                          ${CMAKE_SOURCE_DIR}/middleware/*.h
                          ${CMAKE_SOURCE_DIR}/middleware/*.c
                          ${CMAKE_SOURCE_DIR}/middleware/*.s
                          ${CMAKE_SOURCE_DIR}/middleware/*.S
                          ${CMAKE_SOURCE_DIR}/user/*.h
                          ${CMAKE_SOURCE_DIR}/user/*.c
                          ${CMAKE_SOURCE_DIR}/utils/*.h
                          ${CMAKE_SOURCE_DIR}/utils/*.c
                          ${CMAKE_SOURCE_DIR}/main.cc)

add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})

# generate hex and bin
set(HEX_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${PROJECT_NAME}.bin)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
                   COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}.elf> ${HEX_FILE}
                   COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${PROJECT_NAME}.elf> ${BIN_FILE}
                   COMMENT "Building ${HEX_FILE} and Building ${BIN_FILE}")

#add_custom_target(UPLOAD
#        arm-none-eabi-gdb -iex "target remote tcp:127.0.0.1:3333"
#        -iex "monitor program $<TARGET_FILE:${PROJECT_NAME}.elf>"
#        -iex "monitor reset init"
#        -iex "disconnect" -iex "quit")
