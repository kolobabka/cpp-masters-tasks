#pragma once

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif

#include "location.hh"

namespace yy {

struct Lexer final : public yyFlexLexer {
  yy::location location_;

  void set_location();
  int yylex() override;
};

} // namespace yy
