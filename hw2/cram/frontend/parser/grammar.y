//TODO: Is it possible to fix leaking of memory after caught error in bison?

%language "c++"
%skeleton "lalr1.cc"

%{
    #include "parser.hpp"
    #include <string>
%}

%defines
%define api.value.type variant
%param {yy::Parser* pars}
%param {Tree::NAryTree<AST::Node> &tree}


%code requires
{
    #include <iostream>
    #include <string>
    #include <unordered_map>
    #include "parser.hpp"

    namespace yy {class Parser;}
}

%code
{
    extern int yylineno;
    #include <string>
    #include "parser.hpp"

    namespace yy {

    parser::token_type yylex(parser::semantic_type* yylval, parser::location_type* location, Parser *pars, Tree::NAryTree<AST::Node> &tree);

    } //namespace yy

    namespace {

    AST::OperNode* makeUnaryOperNode (AST::OperNode::OperType type, AST::Node* child, yy::location loc) {

        AST::OperNode* newOperNode = new AST::OperNode (type, loc);
        newOperNode->push_back (child);
        return newOperNode;
    }

    AST::OperNode* makeBinOperNode (AST::OperNode::OperType type, 
                                    AST::Node* firstChild, AST::Node* secondChild, yy::location loc) {

        AST::OperNode* newOperNode = new AST::OperNode (type, loc);
        newOperNode->push_back (firstChild);
        newOperNode->push_back (secondChild);
        return newOperNode;
    }

    AST::OperNode* makeAssign (AST::Node* firstChild, AST::Node* secondChild, 
                               yy::location assignLoc, yy::location varLoc) {
        AST::OperNode* newNode  = new AST::OperNode (AST::OperNode::OperType::ASSIGN, assignLoc);
        newNode->push_back (firstChild);
        newNode->push_back (secondChild);
        return newNode;
    }

    AST::ArrayNode* makeAddressWithoutBase (AST::Node* expr, 
                               yy::location exprLoc) {
        AST::ArrayNode* newNode  = new AST::ArrayNode(exprLoc);
        newNode->push_back (new AST::DummyNode (exprLoc));
        newNode->push_back (expr);
        return newNode;
    }

    AST::ArrayNode* makeAddressWithBase (AST::Node* lval, AST::Node* expr, 
                               yy::location termLoc, yy::location exprLoc) {
        AST::ArrayNode* newNode  = new AST::ArrayNode(termLoc);
        newNode->push_back(lval);
        newNode->push_back(expr);
        return newNode;
    }
    
    } //anonymous namespace 
}


%locations

%token <int>                NUMBER
%token <std::string>        ID

%token                      ASSIGN      "="

%token                      ADD         "+"
%token                      SUB         "-"

%token                      PRINT       "print"
%token                      INPUT       "input"

%token                      OPSQBRACK   "["
%token                      CLSQBRACK   "]"

%token                      COMMA       ","
%token                      SEMICOLON   ";"

%token                      END         0   "end of file"

%token                      LEXERR

%left '+' '-'
%right '='

/* AST TREE */

%type <AST::Node*> statement
%type <AST::Node*> assign
%type <AST::Node*> expr
%type <AST::Node*> term
%type <AST::Node*> lvalue

%type <std::vector<AST::Node*>*>    statementHandler

%start translationStart

%%

translationStart            :   statementHandler                {
                                                                    AST::ScopeNode* globalScope = new AST::ScopeNode (@1);
                                                                    if ($1) { // transform_if
                                                                        for (auto curStmtNode: *($1))             
                                                                        {   
                                                                            if (!curStmtNode) {
                                                                                continue;
                                                                            }
                                                                            globalScope->push_back (curStmtNode);
                                                                        }
                        
                                                                        delete $1; // bruh
                                                                    }
                                                                    tree.setRoot (globalScope);
                                                               
                                                                };

statementHandler            :   statement SEMICOLON              {
                                                                    if ($1) {
                                                                        $$ = new std::vector<AST::Node*>;
                                                                        $$->push_back ($1);
                                                                    } else
                                                                        $$ = nullptr;
                                                                }
                            |   statement SEMICOLON statementHandler   {
                                                                    if ($3) {
                                                                        $3->push_back ($1);
                                                                        $$ = $3;
                                                                    } else {
                                                                        $$ = nullptr;
                                                                        if ($3) {
                                                                            for (auto v: *($3))
                                                                                delete v;
                                                                        }
                                                                        delete $1; // bruh
                                                                        delete $3; // bruh
                                                                    }
                                                                };


statement                   :   assign                          {   $$ = $1;    }
                            |   expr                            {   $$ = $1;    }
                            |   PRINT assign                    {   $$ = makeUnaryOperNode (AST::OperNode::OperType::PRINT, $2, @1);     }
                            |   PRINT expr                      {   $$ = makeUnaryOperNode (AST::OperNode::OperType::PRINT, $2, @1);     }
                            |   INPUT lvalue                    {   $$ = makeUnaryOperNode (AST::OperNode::OperType::INPUT, $2, @1);     }
                            |   PRINT error                     {   pars->push_error (@2, "Undefined expression in print");    $$ = nullptr;   }
                            |   INPUT error                     {   pars->push_error (@2, "Undefined expression in input");    $$ = nullptr;   }
                            |   PRINT error END                 {   pars->push_error (@2, "Undefined expression in print");    $$ = nullptr;   };
                            |   error END                       {   pars->push_error (@1, "Undefined statement");  $$ = nullptr;   };

assign                      :   lvalue ASSIGN expr       
                                                                {   $$ = makeAssign ($1, $3, @2, @1);   }
                            |   lvalue ASSIGN error             {   pars->push_error (@2, "Bad expression after assignment");  
                                                                    $$ = nullptr;   
                                                                }
                            |   lvalue error                    {   pars->push_error (@1, "Unexpected operation with variable");   
                                                                    $$ = nullptr;   
                                                                }
                            |   error ASSIGN expr  
                                                                {   
                                                                    pars->push_error (@1, "rvalue can't become lvalue"); 
                                                                    $$ = nullptr; 
                                                                };
                      
lvalue                      :   term                            {   $$ = $1;                               };
                            |   OPSQBRACK expr CLSQBRACK        {   $$ = makeAddressWithoutBase ($2, @2);  }
                            |   term OPSQBRACK expr CLSQBRACK   {   $$ = makeAddressWithBase ($1, $3, @1, @3);  };

expr                        :   lvalue                          {   $$ = $1;        }
                            |   lvalue ADD expr                 {   $$ = makeBinOperNode (AST::OperNode::OperType::ADD, $1, $3, @2);   }
                            |   lvalue SUB expr                 {   $$ = makeBinOperNode (AST::OperNode::OperType::SUB, $1, $3, @2);   };

term                        :   NUMBER                          {   $$ = new AST::NumNode   ($1);                                   }
                            |   ID                              {   $$ = new AST::VarNode   ($1, @1);                               };
                            |   OPSQBRACK                       {   $$ = new AST::VarNode   ("c", @1);    }
                            |   CLSQBRACK OPSQBRACK             {   $$ = new AST::VarNode   ("x", @1);    };
%%


namespace yy {

Parser::Parser (std::string_view input) : lexer_ (std::unique_ptr<Lexer>{new Lexer}) {
    std::fstream inputFile (input.data(), std::ios_base::in);
    while (inputFile) {
        std::string newLine;
        std::getline (inputFile, newLine);
        code_.push_back (newLine);
    }
}

std::unique_ptr<Tree::NAryTree<AST::Node>> Parser::parse () {
    std::unique_ptr<Tree::NAryTree<AST::Node>> tree = std::make_unique<Tree::NAryTree<AST::Node>>();
    parser parser (this, *tree); //TODO Think about the first param
    bool res = parser.parse ();
    return tree;
}

parser::token_type Parser::yylex (parser::semantic_type *yylval, parser::location_type *location) {
    parser::token_type tokenT = static_cast<parser::token_type> (lexer_->yylex ());

    switch (tokenT) {
        //!TODO try catch
        case yy::parser::token_type::NUMBER: {
            yylval->build<int> () = std::stoi (lexer_->YYText ());
            break;
        }
        case yy::parser::token_type::ID: {
            yylval->build<std::string> () = lexer_->YYText ();
            break;
        }
        case yy::parser::token_type::LEXERR: {
            throw std::runtime_error ("Unexpected word");
        }
    }

    *location = lexer_->location_;

    return tokenT;
}

void Parser::push_error (yy::location curLocation, std::string_view err)
{
    std::string errPos = std::string ("#") + std::to_string (curLocation.begin.line) + std::string (", ") + std::to_string (curLocation.begin.column) + std::string (": ");
    std::string errMsg = std::string(err) + std::string (": ");
    std::string codePart = code_[curLocation.begin.line - 1];

    std::string underLine ("\n");
    underLine.insert (1, curLocation.begin.column + errPos.size () + errMsg.size (), '~');
    underLine += std::string ("^");

    error_.push_back (errPos + errMsg + codePart + underLine);
}


parser::token_type yylex (parser::semantic_type* yylval, parser::location_type* location, Parser* pars, Tree::NAryTree<AST::Node> &tree) {
    
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