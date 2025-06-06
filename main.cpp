#include <iostream>
#include <fstream>
#include <QImage>
#include <QString>

using namespace std;

void AplicarXOR(unsigned char* imagen, unsigned char* IM, int size);
void RotarBitsIzquierda(unsigned char *Datos, int Size, int Bits);
void RotarBitsDerecha(unsigned char *Datos, int Size, int Bits);
void DesplazarBitsIzquierda(unsigned char *Datos, int Size, int Bits);
void DesplazarBitsDerecha(unsigned char *Datos, int Size, int Bits);
unsigned int *CargarSemilla(const char *NombreArchivo, int &Semilla, int &N_Pixeles);
unsigned char *CargarPixels(QString input, int &width, int &height);
bool exportImage(unsigned char* pixelData, int width,int height, QString archivoSalida);
bool CompararImagenes(unsigned char* pixelData1, int width1, int height1, unsigned char* pixelData2, int width2, int height2);

int main()
{
    //Cargar la imagen transformada final (I_D) imagen de distorción
    int Width, Height;
    unsigned char *Io_F = CargarPixels("I_D.bmp", Width, Height);
    if (!Io_F) return 1;

    //Cargar la imagen de distorción (I_M) usada para distorcionar la imagen
    int Width_IM, Height_IM;
    unsigned char *IM = CargarPixels("I_M.bmp", Width_IM, Height_IM);

    //liberar memoria en caso de que (I_M) no cargue correctamente
    if (!IM) {
        delete[] Io_F;
        return 1;
    }

    //Cargar la mascara (M) para validar que las operaciones a nivel de bits sean correctas
    int Width_M, Height_M;
    unsigned char *M = CargarPixels("M.bmp", Width_M, Height_M);

    //liberar memoria en caso de que (M) no cargue correctamente
    if (!M) {
        delete[] Io_F;
        delete[] IM;
        return 1;
    }

    //Revertir último XOR (P3.bmp = XOR(IR3, IM))
    AplicarXOR(Io_F, IM, Width * Height * 3);

    //Cargar y validar enmascaramiento con el archivo M2.txt
    bool Iguales = true;
    int Semilla2, N_Pixeles2;
    unsigned int *DatosM2 = CargarSemilla("M2.txt", Semilla2, N_Pixeles2);
    if (DatosM2) {
        //Aplicar la formula dada para validar los datos
        for (int i = 0; i < N_Pixeles2 * 3; i++) {
            int SumaM2 = int(Io_F[Semilla2 + i]) + int(M[i]);

            //verificar que cada suma sea correcta
            if (SumaM2 != DatosM2[i]) {
                cout << "Los datos no coinciden, se esperaba: " << int(DatosM2[i]) << " y se obtuvo: " << SumaM2 << endl;
                Iguales = false;
                break;
            }
        }

        delete[] DatosM2;
    }

    if (Iguales) {
        cout << "La aplicación del XOR fue exitosa." << endl;
    } else {
        cout << "La aplicación del XOR fue erronea." << endl;
        return 1;
    }

    //Revertir rotación de 3 bits a la Derecha
    RotarBitsIzquierda(Io_F, Width * Height * 3, 3);

    //Cargar y validar enmascaramiento con el archivo M1.txt
    Iguales = true;
    int Semilla1, N_Pixeles1;
    unsigned int *DatosM1 = CargarSemilla("M1.txt", Semilla1, N_Pixeles1);
    if (DatosM1) {
        //Aplicar la formula dada para validar los datos
        for (int i = 0; i < N_Pixeles1 * 3; i++) {
            int SumaM1 = int(Io_F[Semilla1 + i]) + int(M[i]);

            //verificar que cada suma sea correcta
            if (SumaM1 != DatosM1[i]) {
                cout << "Los datos no coinciden, se esperaba: " << int(DatosM1[i]) << " y se obtuvo: " << SumaM1 << endl;
                Iguales = false;
                break;
            }
        }

        delete[] DatosM1;
    }

    if (Iguales) {
        cout << "La aplicación de la Rotación de Bits fue exitosa." << endl;
    } else {
        cout << "La aplicación de la Rotación de Bits fue erronea." << endl;
        return 1;
    }

    //Revertir primer XOR (P1.bmp = XOR(Io_original, IM))
    AplicarXOR(Io_F, IM, Width * Height * 3);

    Iguales = true;
    int SemillaMO, N_PixelesMO;
    unsigned int *DatosMO = CargarSemilla("M0.txt", SemillaMO, N_PixelesMO);
    if (DatosMO) {
        //Aplicar la formula dada para validar los datos
        for (int i = 0; i < N_PixelesMO * 3; i++) {
            int SumaMO = int(Io_F[SemillaMO + i]) + int(M[i]);

            //verificar que cada suma sea correcta
            if (SumaMO != DatosMO[i]) {
                cout << "Los datos no coinciden, se esperaba: " << int(DatosMO[i]) << " y se obtuvo: " << SumaMO << endl;
                Iguales = false;
                break;
            }
        }

        delete[] DatosMO;
    }

    if (Iguales) {
        cout << "La aplicación del XOR fue exitosa." << endl;
    } else {
        cout << "La aplicación del XOR fue erronea." << endl;
        return 1;
    }

    // Guardar imagen recuperada
    exportImage(Io_F, Width, Height, "Io_recuperada.bmp");

    //Comparar si la imagen que se recupero es igual a la imagen Original
    int WidthRef, HeightRef;
    unsigned char *ImagenRef = CargarPixels("I_O.bmp", WidthRef, HeightRef);
    if (ImagenRef) {
        bool coinciden = CompararImagenes(Io_F, Width, Height, ImagenRef, WidthRef, HeightRef);
        if (coinciden) {
            cout << "Proceso exitoso." << endl;
        } else {
            cout << "Proceso Fallido." << endl;
        }
        delete[] ImagenRef;
    }

    // Liberar memoria
    delete[] Io_F;
    delete[] IM;
    delete[] M;

    return 0;
}

//Función para aplicar el XOR entre dos imagenes haciendo la opercaion reversible
void AplicarXOR(unsigned char* imagen, unsigned char* IM, int size) {
    for (int i = 0; i < size; ++i) {
        imagen[i] ^= IM[i];
    }
    cout << "XOR completado." << endl;
}

//Funcion para revertir la rotación a la derecha
void RotarBitsIzquierda(unsigned char *Datos, int Size, int Bits) {
    Bits %= 8;
    for (int i = 0; i < Size; i++) {
        //Realiza la rotación de bits a la Izquierda en cada uno de los Byte
        Datos[i] = (Datos[i] << Bits) | (Datos[i] >> (8 - Bits));
    }
    cout << "Rotación de bits completada" << endl;
}

void RotarBitsDerecha(unsigned char *Datos, int Size, int Bits) {
    Bits %= 8;
    for (int i = 0; i < Size; i++) {
        //Realiza la rotación de bits a la Izquierda en cada uno de los Byte
        Datos[i] = (Datos[i] >> Bits) | (Datos[i] << (8 - Bits));
    }
    cout << "Rotación de bits completada" << endl;
}

void DesplazarBitsIzquierda(unsigned char *Datos, int Size, int Bits) {
    Bits %= 8; // Asegurarse de que el número de bits no exceda 7
    for (int i = 0; i < Size; i++) {
        // Desplaza los bits a la izquierda y llena con ceros a la derecha
        Datos[i] <<= Bits;
    }
    cout << "Desplazamiento de bits a la izquierda completado." << endl;
}

void DesplazarBitsDerecha(unsigned char *Datos, int Size, int Bits) {
    Bits %= 8; // Asegurarse de que el número de bits no exceda 7
    for (int i = 0; i < Size; i++) {
        // Desplaza los bits a la derecha y llena con ceros a la izquierda
        Datos[i] >>= Bits;
    }
    cout << "Desplazamiento de bits a la derecha completado." << endl;
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
    N_Pixeles = 0;
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
        cout << "Pixel " << i / 3 << ": (" << R  << "," << G << "," << B << ")" << endl;
    }

    archivo.close();
    return RGB;
}

unsigned char *CargarPixels(QString input, int &width, int &height){
    // Cargar la imagen BMP desde el archivo especificado
    QImage imagen;

    // Cargar la imagen con conversión explícita a formato RGB888
    if (!imagen.load(input)) {
        cerr << "Error: No se pudo cargar la imagen: " << input.toStdString() << endl;
        return nullptr;
    }

    // Convertir al formato correcto
    imagen = imagen.convertToFormat(QImage::Format_RGB888);

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

bool exportImage(unsigned char* pixelData, int width,int height, QString archivoSalida){

    // Crear una nueva imagen de salida con el mismo tamaño que la original
    // usando el formato RGB888 (3 bytes por píxel, sin canal alfa)
    QImage outputImage(width, height, QImage::Format_RGB888);

    // Copiar los datos de píxeles desde el buffer al objeto QImage
    for (int y = 0; y < height; ++y) {
        // outputImage.scanLine(y) devuelve un puntero a la línea y-ésima de píxeles en la imagen
        // pixelData + y * width * 3 apunta al inicio de la línea y-ésima en el buffer (sin padding)
        // width * 3 son los bytes a copiar (3 por píxel)
        memcpy(outputImage.scanLine(y), pixelData + y * width * 3, width * 3);
    }

    // Guardar la imagen en disco como archivo BMP
    if (!outputImage.save(archivoSalida, "BMP")) {
        // Si hubo un error al guardar, mostrar mensaje de error
        cout << "Error: No se pudo guardar la imagen BMP modificada.";
        return false; // Indica que la operación falló
    } else {
        // Si la imagen fue guardada correctamente, mostrar mensaje de éxito
        cout << "Imagen BMP modificada guardada como " << archivoSalida.toStdString() << endl;
        return true; // Indica éxito
    }

}

//Funcion para verificar que las imagenes sean iguales
bool CompararImagenes(unsigned char* pixelData1, int width1, int height1, unsigned char* pixelData2, int width2, int height2) {
    // Verificar si las dimensiones coinciden
    if (width1 != width2 || height1 != height2) {
        cout << "Las imagenes tienen diferentes dimensiones." << endl;
        return false;
    }

    // Verificar que cada pixel sea igual
    int dataSize = width1 * height1 * 3;
    for (int i = 0; i < dataSize; ++i) {
        if (pixelData1[i] != pixelData2[i]) {
            cout << "Los pixeles: " << endl;
            cout << "Pixel1: " << (int)pixelData1[i] << endl;
            cout << "Pixel2: " << (int)pixelData2[i] << endl;
            cout << "No coinciden." << endl;
            return false;
        }
    }

    cout << "Las imagenes coinciden." << endl;
    return true;
}
