#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>

#include "frontend.hpp"
#include "backend.hpp"

int main(int argc, char *argv[]) try {
    Frontend front{argc, argv};
    auto ast = front.build_ast ();
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