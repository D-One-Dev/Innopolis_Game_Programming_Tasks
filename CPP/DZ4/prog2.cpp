#include <cmath>
#include <cstdio>
#include <stdexcept>

class Vector3D
{
    private:
        double x;
        double y;
        double z;

    public:
        Vector3D();

        Vector3D(double x, double y, double z);

        double& operator[](int index);
        const double& operator[](int index) const;

        Vector3D& operator*=(double scalar);

        void print() const;

        friend Vector3D operator+(const Vector3D& a, const Vector3D& b);
        friend Vector3D operator-(const Vector3D& a, const Vector3D& b);
        friend Vector3D operator*(const Vector3D& a, const Vector3D& b);
        friend double operator%(const Vector3D& a, const Vector3D& b);
};


Vector3D::Vector3D() : x(0.0), y(0.0), z(0.0) {}

Vector3D::Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

double& Vector3D::operator[](int index)
{
    switch (index)
    {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: throw std::out_of_range("Vector3D::operator[]: index out of range");
    }
}

const double& Vector3D::operator[](int index) const
{
    switch (index)
    {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: throw std::out_of_range("Vector3D::operator[]: index out of range");
    }
}

Vector3D& Vector3D::operator*=(double scalar)
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

void Vector3D::print() const
{
    std::printf("(%9.6f, %9.6f, %9.6f)", x, y, z);
}

Vector3D operator+(const Vector3D& a, const Vector3D& b)
{
    return Vector3D(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vector3D operator-(const Vector3D& a, const Vector3D& b)
{
    return Vector3D(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vector3D operator*(const Vector3D& a, const Vector3D& b)
{
    return Vector3D(a.y * b.z - a.z * b.y,
                    a.z * b.x - a.x * b.z,
                    a.x * b.y - a.y * b.x);
}

double operator%(const Vector3D& a, const Vector3D& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

namespace
{
    const int    VERTICES_COUNT = 8;
    const int    ROTATION_STEPS = 1000;
    const double TOTAL_ANGLE    = 1.0;
    const double PI             = 3.14159265358979323846;

    double length(const Vector3D& v)
    {
        return std::sqrt(v % v);
    }
}

int main()
{
    Vector3D axis(1.0, 2.0, 3.0);
    axis *= 1.0 / length(axis);

    Vector3D vertices[VERTICES_COUNT];

    int index = 0;
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            for (int k = 0; k < 2; ++k)
            {
                vertices[index][0] = (i == 0) ? -1.0 : 1.0;
                vertices[index][1] = (j == 0) ? -1.0 : 1.0;
                vertices[index][2] = (k == 0) ? -1.0 : 1.0;
                ++index;
            }
        }
    }

    Vector3D initial[VERTICES_COUNT];
    for (int i = 0; i < VERTICES_COUNT; ++i)
    {
        initial[i] = vertices[i];
    }

    std::printf("Rotation axis R (unit length): ");
    axis.print();
    std::printf("\n");

    std::printf("\nCube vertices BEFORE rotation:\n");
    for (int i = 0; i < VERTICES_COUNT; ++i)
    {
        std::printf("  v[%d] = ", i);
        vertices[i].print();
        std::printf("   |v| = %.6f\n", length(vertices[i]));
    }

    const double stepAngle = TOTAL_ANGLE / ROTATION_STEPS;

    for (int step = 0; step < ROTATION_STEPS; ++step)
    {
        for (int i = 0; i < VERTICES_COUNT; ++i)
        {
            Vector3D delta = axis * vertices[i];
            delta *= stepAngle;
            vertices[i] = vertices[i] + delta;
        }
    }

    std::printf("\nCube vertices AFTER rotation by %.1f rad around R:\n", TOTAL_ANGLE);
    for (int i = 0; i < VERTICES_COUNT; ++i)
    {
        std::printf("  v[%d] = ", i);
        vertices[i].print();
        std::printf("   |v| = %.6f\n", length(vertices[i]));
    }

    std::printf("\nDisplacement and rotation angle of each vertex:\n");
    for (int i = 0; i < VERTICES_COUNT; ++i)
    {
        const Vector3D shift = vertices[i] - initial[i];

        const double cosine = (initial[i] % vertices[i]) /
                             (length(initial[i]) * length(vertices[i]));
        const double angle = std::acos(cosine);

        std::printf("  v[%d] - v_initial[%d] = ", i, i);
        shift.print();
        std::printf("   angle = %.6f rad (%.3f deg)\n", angle, angle * 180.0 / PI);
    }

    return 0;
}
