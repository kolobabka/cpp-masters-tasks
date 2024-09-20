//TODO: Is it possible to fix leaking of memory after caught error in bison?

%language "c++"
%skeleton "lalr1.cc"

%{
    #include "../parser.h"
    #include <string>
    #include <sstream>
    #include <exception>
%}
%locations
%defines
%define api.value.type variant
%param {yy::Parser* pars}

%code requires
{
    #include <iostream>
    #include <string>
    #include "../parser.h"

    namespace yy {class Parser;}
}

%code
{
    extern int yylineno;
    #include <string>
    #include "../parser.h"

    namespace yy {

    parser::token_type yylex(parser::semantic_type* yylval, parser::location_type* location, Parser *pars);


    } //namespace yy
}



%token                      CHAR        "char"

%token                      POINTER     "*"
%token                      ARRAY       "[]"

%token                      CONST       "const"

%token                      END         0   "end of file"
%token                      LEXERR


%start type

%%

    type                    :   CHAR tail                       {   
                                                                    pars->QualDecomp.push_back(Entity::Empty);              
                                                                }
                            |   CHAR CONST tail                 { 
                                                                    pars->QualDecomp.push_back(Entity::Const);              
                                                                }
                            |   CONST CHAR tail                 { 
                                                                    pars->QualDecomp.push_back(Entity::Const);              
                                                                };

tail                        :   POINTER tail                    { 
                                                                    pars->QualDecomp.push_back(Entity::Empty);  
                                                                    pars->QualDecomp.push_back(Entity::Pointer);  
                                                                }
                            |   POINTER CONST tail              { 
                                                                    pars->QualDecomp.push_back(Entity::Const);
                                                                    pars->QualDecomp.push_back(Entity::Pointer);
                                                                }
                            |   ARRAY tail                      { 
                                                                    pars->QualDecomp.push_back(Entity::Empty);              
                                                                    pars->QualDecomp.push_back(Entity::Array);              
                                                                }
                            |   ARRAY CONST tail                { 
                                                                    pars->QualDecomp.push_back(Entity::Const);      
                                                                    pars->QualDecomp.push_back(Entity::Array);   
                                                                }
                            |   %empty                          {                                                         }
                            |   error                           { pars->push_error(@1,  "something get wrong :)");        };
%%


namespace yy {

// Lexer::Lexer(std::istream &buf) : yyFlexLexer(buf, std::cout) {}

// Parser::Parser (std::istream &buf) : lexer_ (std::unique_ptr<Lexer>{new Lexer(buf)}) {}

bool Parser::parse () {    
    parser parser (this); //TODO Think about the first param
    bool res = parser.parse ();
    return res;
}

parser::token_type Parser::yylex (parser::semantic_type *yylval, parser::location_type *location) {
    parser::token_type tokenT = static_cast<parser::token_type> (lexer_->yylex ());
#if 0 
    parser::token_type tokenT;
    try {
        tokenT = static_cast<parser::token_type> (lexer_->yylex ());
    } catch(std::exception &Err) {
        std::cout << Err.what() << "\n";
    }
#endif

    switch (tokenT) {
        //!TODO try catch
        // case yy::parser::token_type::NUMBER: {
        //     yylval->build<int> () = std::stoi (lexer_->YYText ());
        //     break;
        // }
        // case yy::parser::token_type::ID: {
        //     yylval->build<std::string> () = lexer_->YYText ();
        //     break;
        // }
        case yy::parser::token_type::LEXERR: {
            throw std::runtime_error ("Unexpected word");
            break;
        }
        default:
            break;
    }
#if 0
    *location = lexer_->location_;
#endif
    return tokenT;
}

void Parser::push_error (yy::location curLocation, const std::string &err)
{
    std::string errPos = std::string ("#") + std::to_string (curLocation.begin.line) + std::string (", ") + std::to_string (curLocation.begin.column) + std::string (": ");
    std::string errMsg = err + std::string (": ");

    std::string underLine ("\n");
    underLine.insert (1, curLocation.begin.column + errPos.size () + errMsg.size (), '~');
    underLine += std::string ("^");

    error_.push_back (errPos + errMsg + underLine);
}


parser::token_type yylex (parser::semantic_type* yylval, parser::location_type* location, Parser* pars) {
    try {
        return pars->yylex (yylval, location);
    } catch (std::runtime_error& err) {
        throw err; 
    }
}

void parser::error (const parser::location_type& location, const std::string& what) {
    /* DO NOTHING */
}

}//namespace yy