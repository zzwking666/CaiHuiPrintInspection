# The following variables are used to determine whether to build with hardware or in a test environment.

# Set to "true" to enable building with CUDA hardware support. If set to "false", the build will use a test environment instead.
set(BUILD_WITH_CUDA "true" CACHE STRING "Enable building with CUDA hardware support. Set to false to use a test environment.")

# Set to "true" to enable building with HALCON library support. If set to "false", the build will use a test environment instead.
set(BUILD_WITH_HALCON "true" CACHE STRING "Enable building with HALCON library support. Set to false to use a test environment.")

# Set to "true" to enable building with Hikvision (海康) camera support. If set to "false", the build will use a test environment instead.
set(BUILD_WITH_MVS "true" CACHE STRING "Enable building with Hikvision (海康) camera support. Set to false to use a test environment.")

# Set to "true" to enable building with DuShen (度申) camera support. If set to "false", the build will use a test environment instead.
set(BUILD_WITH_DS "true" CACHE STRING "Enable building with DuShen (度申) camera support. Set to false to use a test environment.")


set(BUILD_CUDA113_WITH_TENSOR86 "false" CACHE STRING "When build with cuda11.3 use tensor8.6")