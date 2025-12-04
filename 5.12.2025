#include <iostream>
#include <cmath>
#include <stdexcept>
#include <iomanip>

class Vec {
private:
    float x, y, z;
    static constexpr float EPS = 1e-6f;
public:
    Vec() : x(0.0f), y(0.0f), z(0.0f) {}
    Vec(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    Vec(const Vec &other) : x(other.x), y(other.y), z(other.z) {}
    ~Vec() = default;

    Vec& operator=(const Vec &other) {
        if (this != &other) {
            x = other.x; y = other.y; z = other.z;
        }
        return *this;
    }

    float& operator[](size_t i) {
        if (i > 2) throw std::out_of_range("Index out of range");
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }
    const float& operator[](size_t i) const {
        if (i > 2) throw std::out_of_range("Index out of range");
        if (i == 0) return x;
        if (i == 1) return y;
        return z;
    }

    Vec operator+(const Vec &o) const { return Vec(x + o.x, y + o.y, z + o.z); }
    Vec operator-(const Vec &o) const { return Vec(x - o.x, y - o.y, z - o.z); }

    Vec operator*(float s) const { return Vec(x * s, y * s, z * s); }
    float dot(const Vec &o) const { return x*o.x + y*o.y + z*o.z; }
    Vec operator+() const {
        float mag = this->magnitude();
        if (mag < EPS) return Vec(0.0f, 0.0f, 0.0f);
        return (*this) * (1.0f / mag);
    }
    Vec operator-() const {
        float mag = this->magnitude();
        if (mag < EPS) return Vec(0.0f, 0.0f, 0.0f);
        return (*this) * (-1.0f / mag);
    }
    float magnitude() const { return std::sqrt(x*x + y*y + z*z); }
    float magnitudeSq() const { return x*x + y*y + z*z; }

    bool operator==(const Vec &o) const {
        return std::fabs(x - o.x) < EPS && std::fabs(y - o.y) < EPS && std::fabs(z - o.z) < EPS;
    }
    bool operator!=(const Vec &o) const { return !(*this == o); }
    bool operator<(const Vec &o) const { return magnitudeSq() < o.magnitudeSq() - EPS; }
    bool operator>(const Vec &o) const { return magnitudeSq() > o.magnitudeSq() + EPS; }
    bool operator<=(const Vec &o) const { return !(*this > o); }
    bool operator>=(const Vec &o) const { return !(*this < o); }

    // Unary ~ returns magnitude
    float operator~() const { return magnitude(); }

    Vec& operator++() {
        x += 1.0f; y += 1.0f; z += 1.0f;
        return *this;
    }
    Vec operator++(int) {
        Vec tmp(*this);
        ++(*this);
        return tmp;
    }
    Vec& operator--() {
        x -= 1.0f; y -= 1.0f; z -= 1.0f;
        return *this;
    }
    Vec operator--(int) {
        Vec tmp(*this);
        --(*this);
        return tmp;
    }

    friend std::ostream& operator<<(std::ostream &os, const Vec &v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
    friend std::istream& operator>>(std::istream &is, Vec &v) {
        is >> v.x >> v.y >> v.z;
        return is;
    }

    friend Vec operator*(float s, const Vec &v) { return v * s; }
};

int main() {
    /*
    Vec a(1.0f, 2.0f, 3.0f);
    Vec b(2.0f, -1.0f, 0.5f);

    std::cout << "a = " << a << "\n";
    std::cout << "b = " << b << "\n";

    Vec c = a + b;
    std::cout << "a + b = " << c << "\n";

    Vec d = a - b;
    std::cout << "a - b = " << d << "\n";

    Vec s1 = a * 2.0f;
    Vec s2 = 3.0f * b;
    std::cout << "a * 2 = " << s1 << "\n";
    std::cout << "3 * b = " << s2 << "\n";

    std::cout << "|a| = " << ~a << "\n";
    std::cout << "unit(a) = " << +a << "\n";
    std::cout << "-unit(a) = " << -a << "\n";

    std::cout << "a[0] = " << a[0] << ", a[1] = " << a[1] << ", a[2] = " << a[2] << "\n";

    std::cout << std::boolalpha;
    std::cout << "a == b? " << (a == b) << "\n";
    std::cout << "a > b? " << (a > b) << "\n";

    std::cout << "pre-increment ++a: " << ++a << "\n";
    std::cout << "post-increment a++: " << a++ << " (then a = " << a << ")\n";
    std::cout << "pre-decrement --a: " << --a << "\n";
    std::cout << "post-decrement a--: " << a-- << " (then a = " << a << ")\n";

    // reading vector from input (uncomment to test)
    // Vec r; std::cout << "Enter 3 floats for vector r: "; std::cin >> r; std::cout << "r = " << r << "\n";
    */

    Vec v;
    v[1]=2;
    std::cout<<v;
    return 0;
}
