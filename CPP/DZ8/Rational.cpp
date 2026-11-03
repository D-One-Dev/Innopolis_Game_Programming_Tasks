#include "Rational.hpp"

#include <istream>
#include <numeric>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace {
long long magnitude(long long value) noexcept {
    return value < 0 ? -value : value;
}
}

Rational::Rational(long long numerator, long long denominator)
    : numerator_(numerator), denominator_(denominator) {
    normalize();
}

Rational::Rational(const Rational& other) noexcept
    : numerator_(other.numerator_), denominator_(other.denominator_) {}

Rational& Rational::operator=(const Rational& other) noexcept {
    if (this != &other) {
        numerator_ = other.numerator_;
        denominator_ = other.denominator_;
    }
    return *this;
}

void Rational::normalize() {
    if (denominator_ == 0) {
        throw std::invalid_argument("Rational: denominator must not be zero");
    }

    if (denominator_ < 0) {
        numerator_ = -numerator_;
        denominator_ = -denominator_;
    }

    const long long divisor = std::gcd(magnitude(numerator_), denominator_);
    numerator_ /= divisor;
    denominator_ /= divisor;
}

Rational& Rational::operator+=(const Rational& rhs) {
    const long long common = std::gcd(denominator_, rhs.denominator_);
    const long long lhsMultiplier = rhs.denominator_ / common;
    const long long rhsMultiplier = denominator_ / common;

    numerator_ = numerator_ * lhsMultiplier + rhs.numerator_ * rhsMultiplier;
    denominator_ *= lhsMultiplier;
    normalize();
    return *this;
}

Rational& Rational::operator-=(const Rational& rhs) {
    const long long common = std::gcd(denominator_, rhs.denominator_);
    const long long lhsMultiplier = rhs.denominator_ / common;
    const long long rhsMultiplier = denominator_ / common;

    numerator_ = numerator_ * lhsMultiplier - rhs.numerator_ * rhsMultiplier;
    denominator_ *= lhsMultiplier;
    normalize();
    return *this;
}

Rational& Rational::operator*=(const Rational& rhs) {
    numerator_ *= rhs.numerator_;
    denominator_ *= rhs.denominator_;
    normalize();
    return *this;
}

Rational& Rational::operator/=(const Rational& rhs) {
    if (rhs.numerator_ == 0) {
        throw std::domain_error("Rational: division by zero");
    }

    return *this *= Rational(rhs.denominator_, rhs.numerator_);
}

void Rational::swap(Rational& other) noexcept {
    std::swap(numerator_, other.numerator_);
    std::swap(denominator_, other.denominator_);
}

Rational operator+(Rational lhs, const Rational& rhs) {
    lhs += rhs;
    return lhs;
}

Rational operator-(Rational lhs, const Rational& rhs) {
    lhs -= rhs;
    return lhs;
}

Rational operator*(Rational lhs, const Rational& rhs) {
    lhs *= rhs;
    return lhs;
}

Rational operator/(Rational lhs, const Rational& rhs) {
    lhs /= rhs;
    return lhs;
}

bool operator==(const Rational& lhs, const Rational& rhs) noexcept {
    return lhs.numerator() == rhs.numerator()
        && lhs.denominator() == rhs.denominator();
}

bool operator!=(const Rational& lhs, const Rational& rhs) noexcept {
    return !(lhs == rhs);
}

bool operator<(const Rational& lhs, const Rational& rhs) noexcept {
    return lhs.numerator() * rhs.denominator()
         < rhs.numerator() * lhs.denominator();
}

bool operator>(const Rational& lhs, const Rational& rhs) noexcept {
    return rhs < lhs;
}

bool operator<=(const Rational& lhs, const Rational& rhs) noexcept {
    return !(rhs < lhs);
}

bool operator>=(const Rational& lhs, const Rational& rhs) noexcept {
    return !(lhs < rhs);
}

std::ostream& operator<<(std::ostream& os, const Rational& value) {
    return os << value.numerator() << '/' << value.denominator();
}

std::istream& operator>>(std::istream& is, Rational& value) {
    long long numerator = 0;
    long long denominator = 1;
    char slash = '\0';

    if (is >> numerator >> slash >> denominator && slash == '/') {
        value = Rational(numerator, denominator);
    } else {
        is.setstate(std::ios_base::failbit);
    }

    return is;
}
