find_package(Catch2 3 REQUIRED)

enable_testing ()

add_executable (ARC_tests ARC_tests.cpp)
target_link_libraries(ARC_tests PRIVATE Catch2::Catch2WithMain)

add_test (NAME ARC_tests COMMAND ARC_tests)

add_executable (IdealCache_tests IdealCache_tests.cpp)
target_link_libraries(IdealCache_tests PRIVATE Catch2::Catch2WithMain)

add_test (NAME IdealCache_tests COMMAND IdealCache_tests)

add_executable (LRUC_tests LRUC_tests.cpp)
target_link_libraries(LRUC_tests PRIVATE Catch2::Catch2WithMain)

add_test (NAME LRUC_tests COMMAND LRUC_tests)
