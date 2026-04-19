cmake_minimum_required(VERSION 3.14)
project(fft_project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ── Find GSL ──────────────────────────────────────────────────────────────────
find_package(GSL REQUIRED)

# ── Library: fft_utils ────────────────────────────────────────────────────────
add_library(fft_utils STATIC
    src/fft_utils.c
    include/fft_utils.h
)
target_include_directories(fft_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(fft_utils PUBLIC GSL::gsl GSL::gslcblas m)

# ── Executable: fft_example ───────────────────────────────────────────────────
add_executable(fft_example src/fft_example.c)
target_link_libraries(fft_example PRIVATE fft_utils)

# ── Run target (convenience) ──────────────────────────────────────────────────
# Usage: cmake --build build --target run
#        or pass args: cmake -DFFT_ARGS="1000;1024;out.csv" ..
if(NOT DEFINED FFT_ARGS)
    set(FFT_ARGS "1000;1024;fft_output.csv")
endif()

add_custom_target(run
    COMMAND fft_example ${FFT_ARGS}
    DEPENDS fft_example
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running fft_example with args: ${FFT_ARGS}"
    VERBATIM
)

# ── Install ───────────────────────────────────────────────────────────────────
install(TARGETS fft_example DESTINATION bin)
install(FILES fft_utils.h DESTINATION include)
