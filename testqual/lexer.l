%option c++
%option yylineno
/* %option noyywrap */
%option yyclass="yy::Lexer"

%{
    #include <iostream>
    #include <exception>

    #include "../parser.h"   
    #define YY_USER_ACTION set_location ();
    #include "../lexer.h"

    int yyFlexLexer::yywrap () { return 1; }
    namespace yy { 

    void Lexer::set_location () {
        location_.begin.line = location_.end.line = lineno ();
        int endCol = location_.end.column;
        
        std::cout << "yytext : " << yytext << "\n";
        std::cout << "yyleng : " << yyleng << "\n";

        for (size_t charIdx = 0; yytext[charIdx]; charIdx++) {
            char curChar = yytext[charIdx];
            switch (curChar) {
                case '\n':
                case '\v':
                case '\r':
                    location_.begin.column = location_.end.column = 1;
                    location_.end.line = lineno ();
                    break;
                case '\t':
                    location_.begin.column += 8;
                    location_.end.column += 8;
                    break;
                case ' ':
                    location_.begin.column++;
                    location_.end.column++;
                    break;
                default:
                    location_.begin.column = endCol;
                    location_.end.column = location_.begin.column + YYLeng ();
            }
        }
    }

    }// namespace yy
%}

WS                      [ \t\r\n\v]+
%%

{WS}                    {   /*skip*/    }

"char"                  {   return yy::parser::token_type::CHAR;        }

"*"                     {   
                            return yy::parser::token_type::POINTER;     }
"[]"                    {   return yy::parser::token_type::ARRAY;       }

"const"                 {   return yy::parser::token_type::CONST;       }

.                       {   
                            return yy::parser::token_type::LEXERR;      
                        }
%%