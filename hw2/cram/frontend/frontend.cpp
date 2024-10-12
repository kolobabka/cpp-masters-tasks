#include "frontend.hpp"

#include <fstream>
#include <iostream>

Frontend::Frontend(int argc, char **argv) {

  static constexpr auto HelpMsg = "cram input filename [--dump-ast]";
  if (argc < 2)
    throw std::runtime_error(HelpMsg);

  Args.reserve(argc);
  Args.emplace_back(argv[1]);

  code_.open(Args.front(), std::ios::in);
  if (!code_.is_open())
    throw std::runtime_error("Can't open input file");

  if (argc == 2)
    return;

  if ((argc == 3) && (std::string_view(argv[2]) == "--dump-ast"))
    Args.emplace_back(argv[2]);
  else
    throw std::runtime_error(HelpMsg);
}

std::unique_ptr<Tree::NAryTree<AST::Node>> Frontend::build_ast() {
  ProxyChangeBuffer BufChanger(code_);
  yy::Parser parser(Args.front());

  auto ast = parser.parse();

  auto error_st = parser.err_begin();
  auto error_fin = parser.err_end();

  if (error_st != error_fin) {
    while (error_st != error_fin) {
      std::cout << *error_st << std::endl;
      error_st = std::next(error_st, 1);
    }

    return {};
  }
  if (Args.back() == "--dump-ast")
    ast->dump(std::cout);
  return ast;
}
