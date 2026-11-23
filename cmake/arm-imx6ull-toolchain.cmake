# CMake Toolchain File for NXP i.MX6ULL (ARMv7-A, hard-float)
# Usage:
#   cmake -B build-arm -DCMAKE_TOOLCHAIN_FILE=cmake/arm-imx6ull-toolchain.cmake -DCMAKE_BUILD_TYPE=Release
#   cmake --build build-arm -j$(nproc)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Adjust prefix if your cross-compiler uses a different triple (e.g. arm-buildroot-linux-gnueabihf-)
set(TOOLCHAIN_PREFIX arm-linux-gnueabihf-)

set(CMAKE_C_COMPILER   ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)

# Optional: specify sysroot if needed by your SDK
# set(CMAKE_SYSROOT /opt/imx6ull-sysroot)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
