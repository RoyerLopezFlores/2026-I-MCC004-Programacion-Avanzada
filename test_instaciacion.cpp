#include <iostream>
#include <utility>

class Temporal {
private:
    int value;

public:
    Temporal(int v = 0) : value(v) {
        std::cout << "Constructor Temporal(" << value << ")\n";
    }

    Temporal(const Temporal& other)
        : value(other.value)
    {
        std::cout << "Copy Constructor Temporal\n";
    }

    Temporal(Temporal&& other) noexcept
        : value(std::exchange(other.value, 0))
    {
        std::cout << "Move Constructor Temporal\n";
    }

    ~Temporal() {
        std::cout << "Destructor Temporal(" << value << ")\n";
    }

    Temporal create() {
        std::cout << "\nEntrando a create()\n";

        Temporal temp(value + 1);

        std::cout << "Retornando temp\n";

        return temp;   // NRVO candidato
    }
    Temporal create2() {
        std::cout << "\nEntrando a create2()\n";

        return Temporal(value + 1);   // RVO candidato
    }
};

int main() {

    std::cout << "\n=== t1 ===\n";
    Temporal t1(10);

    std::cout << "\n=== t2 = t1 ===\n";
    Temporal t2 = t1;

    std::cout << "\n=== t3 = t2.create() ===\n";
    Temporal t3 = t2.create();

    std::cout << "\n=== t4 = t2.create2() ===\n";
    Temporal t4 = t2.create2();

    std::cout << "\n=== Fin de main ===\n";



    return 0;
}