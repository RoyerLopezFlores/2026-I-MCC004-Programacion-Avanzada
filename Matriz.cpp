#include <iostream>
#include <sstream>

#include "types.h"
#include "matrix1.h"
#include "matrix1_base.h"

using namespace std;

void DemoMatrixType1And2() {
    cout << "Demo Matrix1 vs Matrix2\n";

    istringstream aData(
        "MATRIZ:\n"
        "2 2\n"
        "1 2\n"
        "3 4\n"
    );
    istringstream bData(
        "MATRIZ:\n"
        "2 2\n"
        "5 6\n"
        "7 8\n"
    );
    istringstream dData(
        "MATRIZ:\n"
        "2 2\n"
        "1 1\n"
        "1 1"
    );

    Matrix1<TI> a1, b1, d1;
    Matrix2<TI> a2, b2, d2;

    if (!(aData >> a1) || !(bData >> b1) || !(dData >> d1)) {
        cout << "Error leyendo matrices para Matrix1\n";
        return;
    }

    istringstream aData2(
        "MATRIZ:\n"
        "2 2\n"
        "1 2\n"
        "3 4\n"
    );
    istringstream bData2(
        "MATRIZ:\n"
        "2 2\n"
        "5 6\n"
        "7 8\n"
    );
    istringstream dData2(
        "MATRIZ:\n"
        "2 2\n"
        "1 1\n"
        "1 1"
    );

    if (!(aData2 >> a2) || !(bData2 >> b2) || !(dData2 >> d2)) {
        cout << "Error leyendo matrices para Matrix2\n";
        return;
    }
    cout<<"Matrices leidas correctamente\n";
    cout<<"A1:\n"<<a1<<endl;
    cout<<"B1:\n"<<b1<<endl;
    cout<<"D1:\n"<<d1<<endl;

    try {
        cout << "\n[Matrix1]\n";
        Matrix1<TI> sum1 = a1 + b1;
        Matrix1<TI> sub1 = a1 - b1;
        Matrix1<TI> mul1 = a1 * b1;
        cout<<"Operaciones lineales (A+A)*B*2 + 4 - D:\n\n";
        Matrix1<TI> lin1 = (a1 + a1) * b1 * 2 + 4 - d1;
        cout<<"Fin de la combinacion lineal\n\n";
        cout << "A + B:\n" << sum1;
        cout << "A - B:\n" << sub1;
        cout << "A * B:\n" << mul1;
        cout << "(A+A)*B*2 + 4 - D:\n" << lin1;

        cout << "\n[Matrix2]\n";
        Matrix2<TI> sum2 = a2 + b2;
        Matrix2<TI> sub2 = a2 - b2;
        Matrix2<TI> mul2 = a2 * b2;
        Matrix2<TI> lin2 = (a2 + a2) * b2 * 2 + 4 - d2;

        cout << "A + B:\n" << sum2;
        cout << "A - B:\n" << sub2;
        cout << "A * B:\n" << mul2;
        cout << "(A+A)*B*2 + 4 - D:\n" << lin2;
        cout<<"Fin de la combinacion lineal\n";
        cout<<a1<<endl;

        cout<<"ApplyFunctionToAll + 2"<<endl;
        a1.ApplyFunctionToAll([](TI &elem) { elem += 2; });
        cout << "A1 + 2:\n" << a1;
        cout<<"A1 * B1:\n" << a1 * b1;
        cout<<"A1 *A1:\n";
        a1 *= a1;
        cout << a1;
        cout<<endl;
        cout<<"Acceso a elemento A1[0][1]: "<<a1[0][1]<<endl;
        cout<<"Acceso a elemento A1[0]"<< a1[0] <<endl;

    } catch (const exception &ex) {
        cout << "Error en demo Matrix1/Matrix2: " << ex.what() << "\n";
    }
}
