%option c++
%option yylineno
%option yyclass="yy::Lexer"

%{
    #include <iostream>
    #include "parser.hpp"    
    #include "lexer.hpp"

    #define YY_USER_ACTION set_location ();

    int yyFlexLexer::yywrap () { return 1; }
    namespace yy { 

    void Lexer::set_location () {

        location_.begin.line = location_.end.line = lineno ();
        int endCol = location_.end.column;

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
DIGIT                   [0-9]
ALPHA                   [a-zA-Z]
WORD                    ({ALPHA}|"_")({ALPHA}|{DIGIT}|"_")*
ONE_LINE_COMMENT        "//".*
/* TODO: multiline comments */
%%

{WS}                    {   /*skip*/    }
{ONE_LINE_COMMENT}      {   /*skip*/    }

{DIGIT}*                {   return yy::parser::token_type::NUMBER;      }


"print"                 {   return yy::parser::token_type::PRINT;       }
"input"                 {   return yy::parser::token_type::INPUT;       }

";"                     {   return yy::parser::token_type::SEMICOLON;   }

"["                     {   return yy::parser::token_type::OPSQBRACK; }
"]"                     {   return yy::parser::token_type::CLSQBRACK; }


"+"                     {   return yy::parser::token_type::ADD;         }
"-"                     {   return yy::parser::token_type::SUB;         }


"="                     {   return yy::parser::token_type::ASSIGN;      }

{WORD}                  {   return yy::parser::token_type::ID;          }

.                       {   return yy::parser::token_type::LEXERR;      }
%%