#include <vector>
#include <complex>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

#include "QMem.hpp"
#include "Hadamard.hpp"
#include "PauliX.hpp"
#include "PauliZ.hpp"
#include "Control.hpp"

void oracle(QMem& qmem, int randNum, int qubits) {
    std::vector<int> controlBits;
    std::vector<int> notControlBits;
    for (int i = 0; i < qubits; i++) {
        if (randNum >> i & 1) {
            controlBits.push_back(qubits - i - 1);
        } else {
            notControlBits.push_back(qubits -i - 1);
        }
    }
    PauliX pauliX;
    Control gate(pauliX, controlBits, notControlBits, qubits);
    qmem.apply(gate);
}

void diffusion(QMem& qmem, std::vector<int> bits, int index) {
    std::vector<int> empty(0);
    PauliZ pauliZ;
    Control gate(pauliZ, empty, bits, index);
    PauliX pauliX;

    qmem.apply(pauliX, index);
    qmem.apply(gate);
    qmem.apply(pauliX, index);
}

bool correct(int qubits, int guess, int randNum) {
    if (guess == -1) {
        return false;
    }

    PauliX pauliX;
    QMem qmem(qubits + 1);

    for (int i = 0; i < qubits; i++) {
        if (guess >> i & 1) {
            qmem.apply(pauliX, qubits - i -1);
        }
    }
    oracle(qmem, randNum, qubits);
    int measureVal = qmem.measure();
    return true;
    //return measureVal & 1;
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));

    if (argc != 2) {
        std::cout << "Not enough cmd line args\n";
        return -1;
    }
    
    int qubits = std::stoi(argv[1]);
    int guess = -1;
    int totalOptions = (int)pow(2, qubits);
    int randNum = rand() % totalOptions;

    
    std::vector<int> bits;
    for (int x = 0; x < qubits; x++) {
        bits.push_back(x);
    }

    Hadamard hadamard;
    PauliX pauliX;

    std::vector<int> aBits;
    for (int x = 0; x < qubits - 1; x++) {
        aBits.push_back(x);
    }

    double floatIterations = pow(2, qubits/2);
    if (double(int(floatIterations)) != floatIterations) {
        floatIterations ++;
    }
    int iterations = (int)floatIterations;
    std::cout << randNum << "\n";
    while (!correct(qubits, guess, randNum)) {

        QMem qmem(qubits + 1);
        qmem.apply(pauliX, qubits);
        qmem.apply(hadamard, bits);
        qmem.apply(hadamard, qubits);

        for (int i = 0; i < iterations - 1; i++) {
            oracle(qmem, randNum, qubits);

            qmem.apply(hadamard, bits);
            diffusion(qmem, aBits, qubits - 1);
            qmem.apply(hadamard, bits);
        }
        int measurement = qmem.measure();
        guess = measurement >> 1;
        std::cout << guess << "\n";
    }

    return 0;
}
