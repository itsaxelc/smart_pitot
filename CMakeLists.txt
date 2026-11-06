
cmake_minimum_required(VERSION 3.22)

project(SMART_PITOT
    VERSION 1.0.0
    LANGUAGES C ASM
)

# ============================================================
# Project configuration
# ============================================================

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
# MCU configuration
# ============================================================

set(MCU_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

# ============================================================
# Local CMSIS and HAL paths
# ============================================================

set(CMSIS_PATH
    ${CMAKE_SOURCE_DIR}/Drivers/CMSIS
)

set(HAL_PATH
    ${CMAKE_SOURCE_DIR}/Drivers/STM32L4xx_HAL_Driver
)

# ============================================================
# Startup and linker script
# ============================================================

set(STARTUP_FILE
    ${CMAKE_SOURCE_DIR}/startup_stm32l476xx.s
)

set(LINKER_SCRIPT
    ${CMAKE_SOURCE_DIR}/STM32L476RGTx_FLASH.ld
)

if(NOT EXISTS ${STARTUP_FILE})
    message(FATAL_ERROR "Startup file not found: ${STARTUP_FILE}")
endif()

if(NOT EXISTS ${LINKER_SCRIPT})
    message(FATAL_ERROR "Linker script not found: ${LINKER_SCRIPT}")
endif()

# ============================================================
# Project sources
# ============================================================

set(PROJECT_SOURCES

    # Core
    Core/Src/main.c
    Core/Src/system_stm32l4xx.c
    Core/Src/stm32l4xx_hal_msp.c
    Core/Src/stm32l4xx_it.c

    # Application
    Application/Src/acquisition.c
    Application/Src/filtering.c
    Application/Src/air_data.c
    Application/Src/display.c

    # Algorithms
    Algorithms/Src/kalman.c
    Algorithms/Src/atmosphere.c

    # Drivers
    Drivers/I2C/i2c.c
    Drivers/BME280/bme280.c
    Drivers/lcd1602_driver/lcd1602.c

    # STM32 HAL
    ${HAL_PATH}/Src/stm32l4xx_hal.c
    ${HAL_PATH}/Src/stm32l4xx_hal_cortex.c
    ${HAL_PATH}/Src/stm32l4xx_hal_flash.c
    ${HAL_PATH}/Src/stm32l4xx_hal_flash_ex.c
    ${HAL_PATH}/Src/stm32l4xx_hal_gpio.c
    ${HAL_PATH}/Src/stm32l4xx_hal_pwr.c
    ${HAL_PATH}/Src/stm32l4xx_hal_pwr_ex.c
    ${HAL_PATH}/Src/stm32l4xx_hal_rcc.c
    ${HAL_PATH}/Src/stm32l4xx_hal_rcc_ex.c

    # Startup
    ${STARTUP_FILE}
)

# ============================================================
# Executable
# ============================================================

add_executable(${PROJECT_NAME}.elf ${PROJECT_SOURCES})

# ============================================================
# Include directories
# ============================================================

target_include_directories(${PROJECT_NAME}.elf PRIVATE

    Core/Inc

    Application/Inc
    Algorithms/Inc

    Drivers/I2C
    Drivers/BME280
    Drivers/lcd1602_driver

    ${CMSIS_PATH}/Include
    ${CMSIS_PATH}/Device/ST/STM32L4xx/Include

    ${HAL_PATH}/Inc
)

# ============================================================
# Preprocessor definitions
# ============================================================

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE

    USE_HAL_DRIVER
    STM32L476xx
)

# ============================================================
# Compiler options
# ============================================================

target_compile_options(${PROJECT_NAME}.elf PRIVATE

    ${MCU_FLAGS}

    -Wall
    -Wextra
    -Wpedantic

    -ffunction-sections
    -fdata-sections

    -O0
    -g3
)

# ============================================================
# Linker options
# ============================================================

target_link_options(${PROJECT_NAME}.elf PRIVATE

    ${MCU_FLAGS}

    -T${LINKER_SCRIPT}

    -Wl,--gc-sections
    -Wl,--print-memory-usage
    -Wl,-Map=${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map

    --specs=nano.specs
    --specs=nosys.specs
)

target_link_libraries(${PROJECT_NAME}.elf PRIVATE
    m
)

# ============================================================
# Output files
# ============================================================

find_program(ARM_OBJCOPY
    NAMES arm-none-eabi-objcopy
    REQUIRED
)

find_program(ARM_SIZE
    NAMES arm-none-eabi-size
    REQUIRED
)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD

    COMMAND ${ARM_OBJCOPY}
        -O ihex
        $<TARGET_FILE:${PROJECT_NAME}.elf>
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex

    COMMAND ${ARM_OBJCOPY}
        -O binary
        $<TARGET_FILE:${PROJECT_NAME}.elf>
        ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin

    COMMAND ${ARM_SIZE}
        $<TARGET_FILE:${PROJECT_NAME}.elf>

    COMMENT "Generating HEX and BIN firmware files..."
)

message(STATUS "SMART_PITOT STM32L476RG configuration loaded")
