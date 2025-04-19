#include <iostream>
#include <fstream>
#include <string>

using namespace std;
void DesHacerXOR(const unsigned char BloqueEnmascarado[3], const unsigned char Clave[3], unsigned char Original[3]);
unsigned int *CargarSemilla(const char *NombreArchivo, int &Semilla, int &N_Pixeles);

int main()
{
    //Se lee la semilla y los datos enmascarados
    int Semilla = 0, N_Pixeles = 0;
    unsigned int *DatosEnmascarados = CargarSemilla("M1.txt", Semilla, N_Pixeles);
    if (!DatosEnmascarados) return 1;

    //Se construye una clave de 3 bits a partir de la semilla
    unsigned char Clave[3];
    Clave[0] = Semilla          & 0xFF;
    Clave[1] = (Semilla >> 8)   & 0xFF;
    Clave[2] = (Semilla >> 16)  & 0xFF;

    //Implementación de la memoria dinamica, la cual almacena los datos recuperados
    unsigned char *DatosRecuperados = new unsigned char[N_Pixeles * 3];

    for (int i = 0; i < N_Pixeles; ++i) {
        unsigned char Bloque[3], Original[3];

        //Obetenmos cada bloque de bits
        Bloque[0] = DatosEnmascarados[i*3];
        Bloque[1] = DatosEnmascarados[i*3 + 1];
        Bloque[2] = DatosEnmascarados[i*3 + 2];

        //Deshace el XOR aplicado en el enmascaramiento
        DesHacerXOR(Bloque, Clave, Original);

        //Guardamos los datos recuperados
        DatosRecuperados[i*3]     = Original[0];
        DatosRecuperados[i*3 + 1] = Original[1];
        DatosRecuperados[i*3 + 2] = Original[2];
    }

    //Liberamos la memoria
    delete[] DatosEnmascarados;
    delete[] DatosRecuperados;

    return 0;
}

//Función para deshacer el XOR
void DesHacerXOR(const unsigned char BloqueEnmascarado[3], const unsigned char Clave[3], unsigned char Original[3]) {
    for (int i = 0; i < 3; i++) {
        //Al aplicar XOR 2 veces a la misma clave se obtiene el valor original
        Original[i] = BloqueEnmascarado[i] ^ Clave[i];
    }
}

unsigned int *CargarSemilla(const char *NombreArchivo, int &Semilla, int &N_Pixeles) {

    //Abrimos el archivo y verificamos que el archivo abrio correctamente
    ifstream archivo(NombreArchivo);
    if (!archivo.is_open()) {
        cout << "No se pudo abrir el archivo: " << NombreArchivo << endl;
        return nullptr;
    }

    //Obtenemos la semilla
    archivo >> Semilla;

    //contamos la cantidad pixeles que hay
    int R, G, B;
    while (archivo >> R >> G >> B) N_Pixeles++;

    //volvemos al principio y leemos nuevamente la semilla
    archivo.clear();
    archivo.seekg(0);
    archivo >> Semilla;

    //Usamos memoria dinamica para obtener los valores RGB
    unsigned int *RGB = new unsigned int[N_Pixeles * 3];

    for (int i = 0; i < N_Pixeles * 3; i += 3) {
        archivo >> R >> G >> B;
        RGB[i] = R;
        RGB[i+1] = G;
        RGB[i+2] = B;
    }

    archivo.close();
    return RGB;
}
