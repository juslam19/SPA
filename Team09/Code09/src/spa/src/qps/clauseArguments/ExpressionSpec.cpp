#include "ExpressionSpec.h"

ExpressionSpec::ExpressionSpec(const std::string& v) : value(v) {}

std::string ExpressionSpec::getValue() const {
    return value;
}

bool ExpressionSpec::isExpressionSpec() const {
    return true;
}

bool ExpressionSpec::operator==(const ExpressionSpec& other) const {
    return value == other.value;
}
