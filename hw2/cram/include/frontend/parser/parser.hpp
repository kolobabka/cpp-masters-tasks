#pragma once

#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <string_view>

#include "common/n_ary_tree.hpp"

#include "lexer.hpp"
#include "ast.hpp"
#include "grammar_tab.hh"

namespace yy {

class Parser final {
  std::unique_ptr<Lexer> lexer_;
  std::vector<std::string> error_;
  std::vector<std::string> code_;

public:
  Parser(std::string_view input);

  parser::token_type yylex(parser::semantic_type *yylval,
                           parser::location_type *location);

  int get_lineno() const noexcept { return lexer_->lineno(); }

  std::string get_YYText() const noexcept { return lexer_->YYText(); }

  void push_error(yy::location curLocation, std::string_view err);

  std::vector<std::string>::const_iterator err_begin() const {
    return error_.cbegin();
  }

  std::vector<std::string>::const_iterator err_end() const {
    return error_.cend();
  }

  std::unique_ptr<Tree::NAryTree<AST::Node>> parse();
};

} // namespace yy
