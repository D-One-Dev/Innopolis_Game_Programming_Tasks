#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <memory>
#include <vector>

class Shape {
public:
    virtual ~Shape() = default;

    virtual double area() const noexcept = 0;
    virtual double perimeter() const noexcept = 0;
    virtual std::unique_ptr<Shape> clone() const = 0;
};

class Circle final : public Shape {
public:
    explicit Circle(double radius);

    double area() const noexcept override;
    double perimeter() const noexcept override;
    std::unique_ptr<Shape> clone() const override;

    double radius() const noexcept { return radius_; }

private:
    double radius_;
};

class Square final : public Shape {
public:
    explicit Square(double side);

    double area() const noexcept override;
    double perimeter() const noexcept override;
    std::unique_ptr<Shape> clone() const override;

    double side() const noexcept { return side_; }

private:
    double side_;
};

class Triangle final : public Shape {
public:
    Triangle(double a, double b, double c);

    double area() const noexcept override;
    double perimeter() const noexcept override;
    std::unique_ptr<Shape> clone() const override;

    double a() const noexcept { return a_; }
    double b() const noexcept { return b_; }
    double c() const noexcept { return c_; }

private:
    double a_;
    double b_;
    double c_;
};

double totalArea(const std::vector<std::unique_ptr<Shape>>& shapes) noexcept;
double totalPerimeter(const std::vector<std::unique_ptr<Shape>>& shapes) noexcept;

std::vector<std::unique_ptr<Shape>> cloneShapes(
    const std::vector<std::unique_ptr<Shape>>& shapes);

#endif
