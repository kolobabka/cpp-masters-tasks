#ifndef _PARSER_H_
#define _PARSER_H_

// TODO: think about redundant includes
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <cassert>

#include "lexer.h"
#include "grammar_tab.hh"

enum class Entity {
    CharT,
    Pointer,
    Array,
    Const,
    Empty
};

namespace yy {

class Parser final { // think about private inheritance
    std::unique_ptr<Lexer> lexer_;
    std::vector<std::string> error_;
    std::vector<std::string> code_;
    
public:
    std::vector<Entity> QualDecomp; 

    Parser (std::istream &buf) : lexer_ (std::unique_ptr<Lexer>{new Lexer(buf)}) {}

    parser::token_type yylex (parser::semantic_type *yylval, parser::location_type *location);

    int get_lineno () const noexcept { return lexer_->lineno (); }

    std::string get_YYText () const { return lexer_->YYText (); }

    void push_error (yy::location curLocation, const std::string &err);

    void dump_error() const {
        for (auto err : error_)
            std::cerr << err << std::endl;
    }

    std::vector<std::string>::const_iterator err_begin () const { return error_.cbegin (); }

    std::vector<std::string>::const_iterator err_end () const   { return error_.cend ();   }

    bool parse ();
};

} // namespace yy

#endif // _PARSER_H_