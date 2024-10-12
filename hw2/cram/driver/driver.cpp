#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "frontend.hpp"
#include "backend.hpp"

int main(int argc, char *argv[]) try {
    std::unique_ptr<Tree::NAryTree<AST::Node>> ast;
    {
        Frontend front{argc, argv};
        ast = front.build_ast ();
    }
    Interpreter I;
    I.run(*ast);

    return 0;
} catch(std::runtime_error &Err) {
    std::printf("%s\n", Err.what());
    return -1;
} catch(...) {
    std::printf("%s\n", "Unexpected error");
    return -1;
}