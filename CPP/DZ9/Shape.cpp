#include "Shape.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
constexpr double kPi = 3.14159265358979323846;

void requireFiniteNonNegative(double value, const char* message)
{
    if (!(value >= 0.0) || !std::isfinite(value))
    {
        throw std::invalid_argument(message);
    }
}

void requireFinitePositive(double value, const char* message)
{
    if (!(value > 0.0) || !std::isfinite(value))
    {
        throw std::invalid_argument(message);
    }
}
}

Circle::Circle(double radius) : radius_(radius)
{
    requireFiniteNonNegative(radius_, "Circle: radius must be non-negative");
}

double Circle::area() const noexcept
{
    return kPi * radius_ * radius_;
}

double Circle::perimeter() const noexcept
{
    return 2.0 * kPi * radius_;
}

std::unique_ptr<Shape> Circle::clone() const
{
    return std::make_unique<Circle>(*this);
}

Square::Square(double side) : side_(side)
{
    requireFiniteNonNegative(side_, "Square: side must be non-negative");
}

double Square::area() const noexcept
{
    return side_ * side_;
}

double Square::perimeter() const noexcept
{
    return 4.0 * side_;
}

std::unique_ptr<Shape> Square::clone() const
{
    return std::make_unique<Square>(*this);
}

Triangle::Triangle(double a, double b, double c) : a_(a), b_(b), c_(c)
{
    requireFinitePositive(a_, "Triangle: sides must be positive");
    requireFinitePositive(b_, "Triangle: sides must be positive");
    requireFinitePositive(c_, "Triangle: sides must be positive");

    if (!(a_ + b_ > c_ && a_ + c_ > b_ && b_ + c_ > a_))
    {
        throw std::invalid_argument("Triangle: side lengths violate the triangle inequality");
    }
}

double Triangle::area() const noexcept
{
    const double semiperimeter = perimeter() / 2.0;
    const double radicand = semiperimeter
                          * (semiperimeter - a_)
                          * (semiperimeter - b_)
                          * (semiperimeter - c_);
    return std::sqrt(std::max(0.0, radicand));
}

double Triangle::perimeter() const noexcept
{
    return a_ + b_ + c_;
}

std::unique_ptr<Shape> Triangle::clone() const
{
    return std::make_unique<Triangle>(*this);
}

double totalArea(const std::vector<std::unique_ptr<Shape>>& shapes) noexcept
{
    double total = 0.0;
    for (const auto& shape : shapes)
    {
        if (shape)
        {
            total += shape->area();
        }
    }
    return total;
}

double totalPerimeter(const std::vector<std::unique_ptr<Shape>>& shapes) noexcept
{
    double total = 0.0;
    for (const auto& shape : shapes)
    {
        if (shape)
        {
            total += shape->perimeter();
        }
    }
    return total;
}

std::vector<std::unique_ptr<Shape>> cloneShapes(const std::vector<std::unique_ptr<Shape>>& shapes)
{
    std::vector<std::unique_ptr<Shape>> result;
    result.reserve(shapes.size());

    for (const auto& shape : shapes)
    {
        if (shape)
        {
            result.push_back(shape->clone());
        }
        else
        {
            result.push_back(nullptr);
        }
    }

    return result;
}
