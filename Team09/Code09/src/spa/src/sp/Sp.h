

#pragma once
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "SourceLoader.h"
#include "pkb/PKBClient/PKBFacadeWriter.h"
#include "sp/ast/Ast.h"
#include "sp/de/DesignExtractor.h"
#include "sp/tokenizer/SpTokenizer.h"

/*
This is the entry point for all SIMPLE programs to be parsed
*/
class SP {
private:
    SourceLoader sourceLoader;
    SpTokenizer tokenizer;
    AST ast;
    DesignExtractor designExtractor;
    PKBFacadeWriter pkbWriter;

public:
    void processFile(std::string filepath);
    explicit SP(PKBFacadeWriter writer) {
        pkbWriter = writer;
    }
    SP() = default;
};
