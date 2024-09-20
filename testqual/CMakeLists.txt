cmake_minimum_required(VERSION 3.24)
project(TestQual)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED) 


if (NOT DEFINED GRAMMAR)
  set (GRAMMAR "grammar.y" CACHE STRING "grammar file" FORCE)
endif ()

# file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lexer)
# file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/parser)

flex_target(
  lexer
  lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/lexer_gen.cpp
)

bison_target(
  parser
  ${GRAMMAR}
  ${CMAKE_CURRENT_BINARY_DIR}/parser_gen.cpp
  COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/grammar_tab.hh" #think about it
)

add_flex_bison_dependency(lexer parser)

set(SRCS
  main.cc
  ${BISON_parser_OUTPUTS}
  ${FLEX_lexer_OUTPUTS}
)

add_executable(testqual ${SRCS})

target_compile_features(testqual PRIVATE cxx_std_23)

target_include_directories(testqual PUBLIC ${CMAKE_CURRENT_BINARY_DIR})