
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# ------------------------------------------------------------
# ARM GNU Toolchain
# ------------------------------------------------------------

find_program(ARM_GCC
    NAMES arm-none-eabi-gcc
    REQUIRED
)

find_program(ARM_GXX
    NAMES arm-none-eabi-g++
    REQUIRED
)

find_program(ARM_OBJCOPY
    NAMES arm-none-eabi-objcopy
    REQUIRED
)

find_program(ARM_SIZE
    NAMES arm-none-eabi-size
    REQUIRED
)

set(CMAKE_C_COMPILER   "${ARM_GCC}")
set(CMAKE_ASM_COMPILER "${ARM_GCC}")
set(CMAKE_CXX_COMPILER "${ARM_GXX}")

set(CMAKE_OBJCOPY "${ARM_OBJCOPY}")
set(CMAKE_SIZE    "${ARM_SIZE}")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ------------------------------------------------------------
# MCU: STM32L476RG - Cortex-M4F
# ------------------------------------------------------------

set(MCU_FLAGS
    "-mcpu=cortex-m4"
    "-mthumb"
    "-mfpu=fpv4-sp-d16"
    "-mfloat-abi=hard"
)

# ------------------------------------------------------------
# C compiler flags
# ------------------------------------------------------------

string(JOIN " " MCU_FLAGS_STRING ${MCU_FLAGS})

set(CMAKE_C_FLAGS_INIT
    "${MCU_FLAGS_STRING} -Wall -Wextra -Wpedantic -ffunction-sections -fdata-sections"
)

set(CMAKE_ASM_FLAGS_INIT
    "${MCU_FLAGS_STRING} -x assembler-with-cpp"
)

# ------------------------------------------------------------
# Search configuration
# ------------------------------------------------------------

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

message(STATUS "ARM GCC: ${ARM_GCC}")
message(STATUS "ARM OBJCOPY: ${ARM_OBJCOPY}")
message(STATUS "ARM SIZE: ${ARM_SIZE}")
