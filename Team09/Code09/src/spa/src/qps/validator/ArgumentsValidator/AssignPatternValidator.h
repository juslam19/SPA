#include <string>
#include <vector>

#include "ArgumentsValidator.h"
#include "qps/GrammarUtils.h"
#include "qps/exceptions/QPSSemanticError.h"
#include "qps/exceptions/QPSSyntaxError.h"

class AssignPatternValidator : public ArgumentsValidator {
public:
    AssignPatternValidator(const std::vector<std::string>& args);
    virtual void validateSyntax() override;
    virtual void validateSemantic(SynonymStore* store) override;
};
