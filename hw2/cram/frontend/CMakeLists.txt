
#-------------------------------------------------------------------------------
#
# [RAM build system -- frontend library
#
#-------------------------------------------------------------------------------

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED) 


if (NOT DEFINED GRAMMAR)
  set (GRAMMAR "parser/grammar.y" CACHE STRING "grammar file" FORCE)
endif ()

file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/lexer)
file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/parser)

flex_target(
  lexer
  lexer/lexer.l
  ${CMAKE_CURRENT_BINARY_DIR}/lexer/lexer_gen.cpp
)

message (STATUS "Cmake current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")

bison_target(
  parser
  ${GRAMMAR}
  ${CMAKE_CURRENT_BINARY_DIR}/parser/parser_gen.cpp
  COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/parser/grammar_tab.hh" #think about it
)
  
add_flex_bison_dependency(lexer parser)

set(SRCS
  frontend.cpp
  ${BISON_parser_OUTPUTS}
  ${FLEX_lexer_OUTPUTS}
)

add_library(frontend STATIC ${SRCS})

set(LEXPARSE
  lexer
  parser
  ast
)

set(LEXPARSE "${LEXPARSE}" PARENT_SCOPE)
#   TODO: Make clang_format
# add_clang_format_run(frontend ${CMAKE_CURRENT_SOURCE_DIR} ${SRCS}) T