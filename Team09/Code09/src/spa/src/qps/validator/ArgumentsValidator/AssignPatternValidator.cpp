#include "AssignPatternValidator.h"

AssignPatternValidator::AssignPatternValidator(const std::vector<std::string>& args) {
    this->args = args;
}

void AssignPatternValidator::validateSyntax() {}

void AssignPatternValidator::validateSemantic(SynonymStore* store) {}
