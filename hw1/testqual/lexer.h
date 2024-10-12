#ifndef _LEXER_H_
#define _LEXER_H_

#ifndef yyFlexLexer
#include <FlexLexer.h>
#endif


// #undef YY_DECL
// #define YY_DECL int yy::Lexer::yylex()

#include <istream>
#include <sstream>
#include "location.hh"
#include "grammar_tab.hh"

namespace yy {

struct Lexer final : public yyFlexLexer {
    yy::location location_;

    Lexer(std::istream &buf) : yyFlexLexer(buf, std::cout) {}
    void set_location ();

    int yylex() override;
};

} // namespace yy

#endif // _LEXER_H_