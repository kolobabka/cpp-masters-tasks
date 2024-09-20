#include "parser.h"

#include <list>
#include <algorithm>
#include <exception>
#include <utility>
#include <ranges>
#include <sstream>

const char * const str_1 = "char *[]";
const char * const str_2 = "const char const *[]";

std::istringstream test_1(str_1);
std::istringstream test_2(str_2);

std::istream streamtest_1(test_1.rdbuf());
std::istream streamtest_2(test_2.rdbuf());

namespace {
std::pair<std::string, std::string> readInput() {
    std::vector<std::string> Input;
    auto Range = std::ranges::istream_view<std::string>(std::cin) | ranges::views::filter([](char Sym) { return n < 5; }) |;
    
        for (auto &&Str :  std::ranges::split_view(Input, "\n")) {
            std::cout << Str << std::endl;
        }
        // std::string FirstType;
        // std::cin >> FirstType;
        // if (!std::cin)
        //     throw std::runtime_error("Failed to read first type from input");

        // std::string SecondType;
        // std::cin >> SecondType;
        // if (!std::cin)
        //     throw std::runtime_error("Failed to read second type from input");

        // return {FirstType, SecondType};
    exit(1);
}
}

std::ostream & operator<<(std::ostream &Os, Entity Ent) {
    switch(Ent) {
    case Entity::CharT:
        Os << "char";   
        break;
    case Entity::Pointer:
        Os << "*";
        break;
    case Entity::Array:
        Os << "[]";
        break;
    case Entity::Const:
        Os << "const";
        break;
    case Entity::Empty:
        Os << "empty";
        break;
    default:
        assert(0 && "Unexpected entity type");
    }

    return Os;
} 

bool testqual(const std::vector<Entity> &type_1, const std::vector<Entity> &type_2) {
    
    if (type_1.size() != type_2.size())
        return false;

    std::list<Entity> type_3;

    // TODO: zip, please
    for (size_t idx = 0; idx != type_1.size(); ++idx) {
        if (type_1[idx] == Entity::Const || type_2[idx] == Entity::Const) {
            type_3.push_back(Entity::Const);
            continue;
        }
        if (type_1[idx] == Entity::Array || type_2[idx] == Entity::Array) {
            type_3.push_back(Entity::Array);
            continue;
        }

        type_3.push_back(type_1[idx]);
    }

    decltype(type_3.begin()) imposter;
    int i;
    for (imposter = type_3.begin(), i = 0; imposter != type_3.end(); ++imposter, ++i) {
        if (*imposter != type_1[i] || *imposter != type_2[i])
            break;
    }

    if (i != 0) {
        auto stop_it = std::next(type_3.begin(), i);
        for (auto it = std::next(type_3.begin(), 1); it != stop_it; ++it) {
            if (*it == Entity::Empty)
                *it = Entity::Const;
        }
    }

    
    for (auto res : type_3)
        std::cout << res << " ";
    std::cout << std::endl;

    auto result = std::mismatch(type_3.begin(), type_3.end(), type_2.begin());

    if (result.first == type_3.end())
        return true;

    return false;
}

int main() try {
    auto &&[FirstType, SecondType] = readInput();

    std::stringstream test_1(FirstType);
    std::stringstream test_2(SecondType);

    std::istream streamtest_1(test_1.rdbuf());
    std::istream streamtest_2(test_2.rdbuf());


    yy::Parser parser_1(streamtest_1);
    if (parser_1.parse()) {
        parser_1.dump_error();
        return -1;
    }

    for (auto Ent : parser_1.QualDecomp)
        std::cout << Ent << " ";

    std::cout << std::endl;

    yy::Parser parser_2(streamtest_2);
    if (parser_2.parse()) {
        parser_2.dump_error();
        return -1;
    }

    for (auto Ent : parser_2.QualDecomp)
        std::cout << Ent << " ";

    std::cout << std::endl;

    std::cout << "Result : " << testqual(parser_1.QualDecomp, parser_2.QualDecomp) << std::endl;

    return 0;
} catch (std::runtime_error &err) {
    std::cout << err.what () << std::endl;
    return 1;
}