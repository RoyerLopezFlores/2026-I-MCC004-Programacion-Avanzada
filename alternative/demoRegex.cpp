#include <iostream>
#include <regex>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int main() {

    //------------------------------------------------------
    // 1. Buscar todos los números de una línea
    //------------------------------------------------------

    cout << "==== NUMEROS ====\n";

    std::string texto = "Juan compro 15 lapices, 230 hojas y 8 cuadernos. A 20.6, y ademas vi un numero -1.3";
    //(\d+) enteros sin signo
    //-?\d+(\.\d+)? //enteros o flotantes con signo
    //-?(\d+(\.\d+)?|\.\d+)
    std::regex numero(R"(-?(\d+(\.\d+)?|\.\d+))");

    for (sregex_iterator it(texto.begin(), texto.end(), numero);
         it != sregex_iterator();
         ++it)
    {
        cout << "Numero encontrado: " << it->str() << endl;
    }
    /*
    ==== NUMEROS ====
    Numero encontrado: 15
    Numero encontrado: 230
    Numero encontrado: 8
    Numero encontrado: 20.6
    Numero encontrado: -1.3
    */


    //------------------------------------------------------
    // 2. Detectar correos electrónicos
    //------------------------------------------------------

    std::cout << "\n==== CORREOS ====\n";

    std::string emails =
        "Correos: juan@gmail.com, "
        "pedro123@uni.edu.pe, "
        "correo_invalido@hotmail";

    std::regex email(
        R"([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,})"
    );

    for (sregex_iterator it(emails.begin(), emails.end(), email);
         it != sregex_iterator();
         ++it)
    {
        cout << it->str() << endl;
    }
    /*
    juan@gmail.com
    pedro123@uni.edu.pe
    */
    //------------------------------------------------------
    // 3. Validar formato de fecha
    //------------------------------------------------------

    cout << "\n==== FECHAS ====\n";

    std::vector<std::string> fechas = {
        "13/07/2026",
        "5/2/2020",
        "2026-07-13",
        "31/12/1999"
    };

    std::regex fecha(R"(\d{1,2}/\d{1,2}/\d{4})");

    for (auto &f : fechas)
    {
        cout << f << " -> "
             << (regex_match(f, fecha) ? "Valida" : "No valida")
             << endl;
    }
    /*
    13/07/2026 -> Valida
    5/2/2020 -> Valida
    2026-07-13 -> No valida
    31/12/1999 -> Valida
    */

    //------------------------------------------------------
    // 4. Extraer etiquetas HTML
    //------------------------------------------------------

    std::cout << "\n==== HTML ====\n";

    std::string html =
        "<h1>Titulo</h1>"
        "<p>Hola</p>"
        "<div>Contenido</div>";

    std::regex tag(R"(<([A-Za-z0-9]+)>)");

    for (sregex_iterator it(html.begin(), html.end(), tag);
         it != sregex_iterator();
         ++it)
    {
        cout << "Etiqueta: " << (*it)[1] << endl;
    }
    /*
    Etiqueta: h1
    Etiqueta: p
    Etiqueta: div
    */
    //------------------------------------------------------
    // 5. Reemplazar texto
    //------------------------------------------------------

    std::cout << "\n==== REEMPLAZO ====\n";

    std::string telefono = "Mi telefono es 999888777";

    std::string oculto = regex_replace(
        telefono,
        regex(R"(\d)"),
        "*"
    );

    std::cout << oculto << endl;
    /*
    Mi telefono es *********
    */
    //------------------------------------------------------
    // 6. Dividir texto usando regex
    //------------------------------------------------------

    cout << "\n==== SPLIT ====\n";

    string lista = "rojo,verde;azul naranja";

    regex separador(R"([,; ]+)");

    sregex_token_iterator it(lista.begin(), lista.end(), separador, -1);

    sregex_token_iterator end;

    while (it != end)
    {
        cout << *it << endl;
        ++it;
    }
    /**
    rojo
    verde
    azul
    naranja
     */
    return 0;
}