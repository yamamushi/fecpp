include_directories(${FECPP_SOURCE_DIR}/src/)

set(ZFEC-TEST-SRC
    zfec.cpp
)

add_executable(zfec ${ZFEC-TEST-SRC})
target_link_libraries(zfec fecpp)


set(BENCHMARK-TEST-SRC
    benchmark.cpp
)

add_executable(benchmark ${BENCHMARK-TEST-SRC})
target_link_libraries(benchmark fecpp)



set(RECOVERY-TEST-SRC
    test_recovery.cpp
)

add_executable(test_recovery ${RECOVERY-TEST-SRC})
target_link_libraries(test_recovery fecpp)



set(GENVEC-TEST-SRC
    gen_test_vec.cpp
)

add_executable(gen_test_vec ${GENVEC-TEST-SRC})
target_link_libraries(gen_test_vec fecpp)

