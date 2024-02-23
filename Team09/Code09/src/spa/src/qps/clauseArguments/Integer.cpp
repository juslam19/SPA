#include "Integer.h"

Integer::Integer(const std::string& v) : value(v) {}

std::string Integer::getValue() const {
    return value;
}

bool Integer::isInteger() const {
    return true;
}

bool Integer::operator==(const ClauseArgument& other) const {
    if (const Integer* ptr = dynamic_cast<const Integer*>(&other)) {
        return this->getValue() == other.getValue();
    }
    return false;
}
