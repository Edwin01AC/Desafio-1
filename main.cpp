#include <iostream>
#include <fstream>
#include <Qstring>

using namespace std;

void AplicarXOR(unsigned char* imagen, unsigned char* IM, int size);
unsigned int *CargarSemilla(const char *NombreArchivo, int &Semilla, int &N_Pixeles);
void CargarPixels(QString input, int &width, int &height);

int main()
{
    //Cargar la imagen transformada final (Io)
    int Width, Height;
    unsigned char *Io = CargarPixels("P3.bmp", Width, Height);
    if (!Io) return 1;

    //Cargar la imagen de distorción (IM)
    int Width_IM, Height_IM;
    unsigned char *IM = CargarPixels("IM.bmp", Width_IM, Height_IM);

    if (!IM) {
        delete[] Io;
        return 1;
    }

    //Cargar la mascara (M) asumiendo que es un archivo bmp de 10 * 10
    int Width_M, Height_M;
    unsigned char *M = CargarPixels("M.bmp", Width_M, Height_M);
    if (!M) {
        delete[] Io;
        delete[] IM;
        return 1;
    }

    //Revertir último XOR (P3.bmp = XOR(IR3, IM))
    AplicarXOR(Io, IM, Width * Height * 3);

    return 0;
}

//Función para aplicar el XOR entre dos imagenes haciendo la opercaion reversible
void AplicarXOR(unsigned char* imagen, unsigned char* IM, int size) {
    for (int i = 0; i < size; ++i) {
        imagen[i] ^= IM[i % size]; // se usa módulo si IM es más pequeña
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

unsigned char* CargarPixels(QString input, int &width, int &height){
    // Cargar la imagen BMP desde el archivo especificado
    QImage imagen(input);

    // Verifica si la imagen fue cargada correctamente
    if (imagen.isNull()) {
        cout << "Error: No se pudo cargar la imagen BMP." << endl;
        return nullptr; // Retorna un puntero nulo si la carga falló
    }

    // Convierte la imagen al formato RGB888 (3 canales de 8 bits sin transparencia)
    imagen = imagen.convertToFormat(QImage::Format_RGB888);

    // Obtiene el ancho y el alto de la imagen cargada
    width = imagen.width();
    height = imagen.height();

    // Calcula el tamaño total de datos (3 bytes por píxel: R, G, B)
    int dataSize = width * height * 3;

    // Reserva memoria dinámica para almacenar los valores RGB de cada píxel
    unsigned char* pixelData = new unsigned char[dataSize];

    // Copia cada línea de píxeles de la imagen Qt a nuestro arreglo lineal
    for (int y = 0; y < height; ++y) {
        const uchar* srcLine = imagen.scanLine(y);              // Línea original de la imagen con posible padding
        unsigned char* dstLine = pixelData + y * width * 3;     // Línea destino en el arreglo lineal sin padding
        memcpy(dstLine, srcLine, width * 3);                    // Copia los píxeles RGB de esa línea (sin padding)
    }

    // Retorna el puntero al arreglo de datos de píxeles cargado en memoria
    return pixelData;
}
