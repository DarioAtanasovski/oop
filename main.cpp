#include<iostream>
using namespace std;

// +, -, *, /, ==, <, >
class Frac {
    int n;
    int d;
public:
    int N () const { return this->n; }
    int D () const { return this->d; }
    void setN(int x) { this->n = x; }
    void setD(int x) { this->d = x; }

    Frac(const int n, const int d=1) {
        this->n = n;
        this->d = d;
    }

    // Copy C-tor
    Frac (const Frac& orig) {
        this->n = orig.n;
        this->d = orig.d;
    }

    Frac& operator=(const Frac& orig) {
        if (this != &orig) { // No Self-assignment
            this->n = orig.n;
            this->d = orig.d;
        }
        return *this;
    }

    void print() {
        cout << this->n << "/" << this->d;
    }


    Frac operator+(const Frac& b) const {
        Frac c(*this);
        c.setN(this->N() * b.D() + b.N() * this->D());
        c.setD(this->D() * b.D());
        return c;
    }

    Frac operator-(const Frac& b) const {
        Frac c(*this);
        c.setN(this->N() * b.D() - b.N() * this->D());
        c.setD(this->D() * b.D());
        return c;
    }

    Frac operator*(const Frac& b) const {
        Frac c(*this);
        c.setN(this->N() * b.N());
        c.setD(this->D() * b.D());
        return c;
    }

    Frac operator/(const Frac& b) const {
        Frac c(*this);
        c.setN(this->N() * b.D());
        c.setD(this->D() * b.N());
        return c;
    }

    bool operator==(const Frac& b) const {
        return (this->N() * b.D()) == (b.N() * this->D());
    }

    bool operator<(const Frac& b) const {
        return (this->N() * b.D()) < (b.N() * this->D());
    }

    bool operator>(const Frac& b) const {
        return (this->N() * b.D()) > (b.N() * this->D());
    }

    ~Frac() {}

};

ostream& operator<<(ostream& o, const Frac& f) {
    o << f.N() << "/" << f.D();
    return o;
}

int main() {
    Frac a(2,3);
    Frac b(4,5);
    cout << "a = " << a << "\n";
    cout << "b = " << b << "\n\n";

    cout << "a + b = " << (a + b) << "\n";
    cout << "a - b = " << (a - b) << "\n";
    cout << "a * b = " << (a * b) << "\n";
    cout << "a / b = " << (a / b) << "\n\n";

    cout << "a == b ? " << (a == b) << "\n";
    cout << "a <  b ? " << (a < b) << "\n";
    cout << "a >  b ? " << (a > b) << "\n";

    return 0;
}