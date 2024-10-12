#pragma once

#include <fstream>
#include <iostream>

#include "parser.hpp"

class Frontend final { // TODO
    std::vector<std::string> Args;
    std::fstream code_;
    std::string msg_;

    struct ProxyChangeBuffer {
        std::streambuf *cinbuf;
        ProxyChangeBuffer(std::fstream &NewBuf) {
            cinbuf = std::cin.rdbuf ();
            std::cin.rdbuf (NewBuf.rdbuf ());
        }

        ~ProxyChangeBuffer() {
            std::cin.rdbuf(cinbuf);
        }
    };
public:
    Frontend (int argc = 0, char **argv_ = nullptr);

    std::unique_ptr<Tree::NAryTree<AST::Node>> build_ast ();
};