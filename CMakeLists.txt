cmake_minimum_required(VERSION 3.14)
project(StuckFish CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Testsuite
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

add_library(ChessEngine src/chess/bitboard.cc src/chess/attack.cc src/chess/magic.cc src/chess/move.cc)
target_include_directories(ChessEngine PUBLIC src/chess)

add_executable(ChessTests tests/test_bitboard.cc tests/test_attack.cc tests/test_move.cc)
target_link_libraries(ChessTests PRIVATE ChessEngine GTest::gtest_main)

include(CTest)
add_test(NAME BitboardTests COMMAND ChessTests)
add_test(NAME AttackTests COMMAND ChessTests)
add_test(NAME MoveGenerationTests COMMAND ChessTests)
