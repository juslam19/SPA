#include "NextValidator.h"

NextValidator::NextValidator(const std::vector<std::string>& args) {
    this->args = args;
}

void NextValidator::validateSyntax() {
    if (args.size() != 2) {
        throw QPSSyntaxError();
    }
    if (!isStmtRef(args[0]) || !isStmtRef(args[1])) {
        throw QPSSyntaxError();
    }
}

void NextValidator::validateSemantic(SynonymStore* store) {
    for (const std::string& arg : args) {
        // Ensure that synonym is in the store and statement subtype
        if (isSynonym(arg)) {
            if (!store->containsSynonymName(arg) || store->containsSynonym(arg, QPSConstants::VARIABLE) ||
                store->containsSynonym(arg, QPSConstants::CONSTANT) ||
                store->containsSynonym(arg, QPSConstants::PROCEDURE)) {
                throw QPSSemanticError();
            }
        }
    }
}
