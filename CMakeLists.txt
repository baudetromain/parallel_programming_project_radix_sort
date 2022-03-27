project(radix)

SET(GCC_COVERAGE_COMPILE_FLAGS "-fopenmp -Wall -std=gnu99 -lm")

add_executable(radixsort radixsort.c)
target_link_libraries(radixsort m)