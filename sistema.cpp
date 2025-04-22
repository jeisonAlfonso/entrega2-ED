#include "sistema.h"

using namespace std;

// ————————————
// Implementación de los constructores y el comparador
//————————————
HuffmanNode::HuffmanNode(int s, unsigned long f)
  : symbol(s), freq(f), left(nullptr), right(nullptr) {}

bool CompareNode::operator()(HuffmanNode* a, HuffmanNode* b) const {
    // para que la priority_queue ponga primero el de menor freq
    return a->freq > b->freq;
}

// ————————————
// Helper recursivo para llenar vector<string> codes
//————————————
void ImageProcessingSystem::buildCodes(HuffmanNode* node,
                                       const string& prefix,
                                       vector<string>& codes) const {
    if (!node) return;
    if (node->symbol >= 0)            // hoja
        codes[node->symbol] = prefix;
    buildCodes(node->left,  prefix + '0', codes);
    buildCodes(node->right, prefix + '1', codes);
}

void ImageProcessingSystem::start() {
    cout << "Bienvenido al Sistema de Procesamiento de Imágenes. Escriba 'ayuda' para ver los comandos disponibles." << endl;
    string command;

    while (true) {
        cout << "$ ";
        getline(cin, command);

        if (command == "salir") {
            cout << "Saliendo del sistema..." << endl;
            break;
        }

        handleCommand(command);
    }
}

void ImageProcessingSystem::handleCommand(string command) {
    string cmd, param, extra;
        size_t spacePos = command.find(' ');

        if (spacePos != string::npos) {
            cmd = command.substr(0, spacePos);
            param = command.substr(spacePos + 1);
        } else {
            cmd = command;
        }

        if (cmd == "ayuda") {
            showHelp(param);
        } else if (cmd == "cargar_imagen") {
            loadImage(param);
        } else if (cmd == "cargar_volumen") {
            loadVolume(param);
        } else if (cmd == "info_imagen") {
            infoImage();
        } else if (cmd == "info_volumen") {
            infoVolume();
        } else if (cmd == "proyeccion2D") {
            projection2D(param);
        } else if (cmd == "codificar_imagen") {
            encodeImage(param);
        } else if (cmd == "decodificar_archivo") {
            decodeFile(param);
        } else if (cmd == "segmentar") {
            segmentImage(param);
        } else {
            cout << "Comando no reconocido. Escriba 'ayuda' para ver los comandos disponibles." << endl;
        }
}

void ImageProcessingSystem::showHelp(string cmd) {
     if (cmd.empty()) {
            cout << "Comandos disponibles:\n"
                 << "  cargar_imagen <nombre_imagen.pgm>\n"
                 << "  cargar_volumen <nombre_base> <n_im>\n"
                 << "  info_imagen\n"
                 << "  info_volumen\n"
                 << "  proyeccion2D <dirección> <criterio> <nombre_archivo.pgm>\n"
                 << "  codificar_imagen <nombre_archivo.huf>\n"
                 << "  decodificar_archivo <nombre_archivo.huf> <nombre_imagen.pgm>\n"
                 << "  segmentar <salida_imagen.pgm> <sx1> <sy1> <sl1> [<sx2> <sy2> <sl2> ...]\n"
                 << "  salir" << endl;
        } else {
            if (cmd == "cargar_imagen") {
                cout << "Uso: cargar_imagen <nombre_imagen.pgm>\nCarga una imagen en memoria." << endl;
            } else if (cmd == "cargar_volumen") {
                cout << "Uso: cargar_volumen <nombre_base> <n_im>\nCarga un volumen de imágenes en memoria." << endl;
            } else if (cmd == "info_imagen") {
                cout << "Uso: info_imagen\nMuestra información de la imagen cargada." << endl;
            } else if (cmd == "info_volumen") {
                cout << "Uso: info_volumen\nMuestra información del volumen cargado." << endl;
            } else if (cmd == "proyeccion2D") {
                cout << "Uso: proyeccion2D <dirección> <criterio> <nombre_archivo.pgm>\nGenera una proyección 2D del volumen cargado." << endl;
            } else if (cmd == "codificar_imagen") {
                cout << "Uso: codificar_imagen <nombre_archivo.huf>\nCodifica la imagen cargada usando Huffman." << endl;
            } else if (cmd == "decodificar_archivo") {
                cout << "Uso: decodificar_archivo <nombre_archivo.huf> <nombre_imagen.pgm>\nDecodifica un archivo de Huffman a una imagen." << endl;
            } else if (cmd == "segmentar") {
                cout << "Uso: segmentar <salida_imagen.pgm> <sx1> <sy1> <sl1> [<sx2> <sy2> <sl2> ...]\nSegmenta la imagen cargada utilizando semillas." << endl;
            } else {
                cout << "No hay ayuda disponible para el comando '" << cmd << "'." << endl;
            }
        }
}

void ImageProcessingSystem::loadImage(string filename) {
    if (filename.empty()) {
        cout << "Uso incorrecto del comando. Escriba 'ayuda cargar_imagen' para más información." << endl;
        return;
    }

    ifstream file(filename);
    if (!file) {
        cout << "La imagen " << filename << " no ha podido ser cargada." << endl;
        return;
    }

    string format;
    file >> format;
    if (format != "P2") {
        cout << "Error: El archivo no está en formato PGM (P2)." << endl;
        return;
    }

    // Leer width, height y maxPixelValue, ignorando comentarios
    width = height = maxPixelValue = -1;
    string line;
    while (getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;  // Saltar líneas vacías y comentarios

        stringstream ss(line);
        if (width == -1) {
            ss >> width >> height;  // Leer dimensiones
        } else {
            ss >> maxPixelValue;  // Leer valor máximo de píxel
            break;  // Una vez leído, salimos del bucle
        }
    }

    if (width <= 0 || height <= 0 || maxPixelValue <= 0 || maxPixelValue > 255) {
        cout << "Error: Formato PGM inválido." << endl;
        return;
    }

    // Leer los datos de la imagen
    imageData.assign(height, vector<int>(width));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (!(file >> imageData[i][j]) || imageData[i][j] < 0 || imageData[i][j] > maxPixelValue) {
                cout << "Error: Datos de imagen corruptos." << endl;
                return;
            }
        }
    }

    imageFilename = filename;
    cout << "La imagen " << filename << " ha sido cargada." << endl;
}

/**
 * @brief Carga un volumen de imágenes en formato PGM y las redimensiona al tamaño de la imagen más grande.
 *
 * @param param Nombre base de las imágenes seguido de la cantidad de imágenes a cargar.
 *
 * Ejemplo de uso:
 *     cargar_volumen imagen 10
 *
 * Esto intentará cargar las imágenes: imagen01.pgm, imagen02.pgm, ..., imagen10.pgm
 */

void ImageProcessingSystem::loadVolume(string param) {
    // Buscar la posición del primer espacio en la cadena
    size_t spacePos = param.find(' ');
    if (spacePos == string::npos) {
        cout << "Uso incorrecto del comando. Escriba 'ayuda cargar_volumen' para más información." << endl;
        return;
    }

    // Separar el nombre base de las imágenes y la cantidad de imágenes a cargar
    string base = param.substr(0, spacePos);
    string numStr = param.substr(spacePos + 1);

    int num_images;
    try {
        num_images = stoi(numStr);  // Convertir el número de imágenes a entero
    } catch (exception &e) {
        cout << "Error: Número de imágenes no válido." << endl;
        return;
    }

    // Validar el rango del número de imágenes
    if (num_images < 1 || num_images > 99) {
        cout << "Error: La cantidad de imágenes debe estar entre 1 y 99." << endl;
        return;
    }

    // Limpiar la estructura que almacenará el volumen de imágenes
    volumeData.clear();
    int maxWidth = -1, maxHeight = -1;  // Variables para almacenar las dimensiones máximas

    vector<vector<vector<int>>> tempVolumeData; // Almacenamiento temporal de las imágenes

    /**
     * Paso 1: Determinar el tamaño máximo de las imágenes
     */
    for (int i = 1; i <= num_images; i++) {
        // Generar el nombre del archivo con formato "nombre_baseXX.pgm"
        stringstream ss;
        ss << base << (i < 10 ? "0" : "") << i << ".pgm";
        string filename = ss.str();

        // Intentar cargar la imagen
        loadImage(filename);

        // Si la imagen no se cargó correctamente, se muestra un mensaje de error y se detiene la carga
        if (imageFilename.empty()) {
            cout << "Error: No se pudo cargar " << filename << endl;
            return;
        }

        // Actualizar las dimensiones máximas encontradas
        maxWidth = max(maxWidth, width);
        maxHeight = max(maxHeight, height);

        // Almacenar la imagen temporalmente
        tempVolumeData.push_back(imageData);
    }

    /**
     * Paso 2: Redimensionar todas las imágenes al tamaño máximo encontrado
     */
    volumeData.clear();  // Limpiar la estructura para almacenar las imágenes finales

    for (auto &image : tempVolumeData) {
        // Crear una nueva imagen con las dimensiones máximas, rellena con ceros (negro)
        vector<vector<int>> resizedImage(maxHeight, vector<int>(maxWidth, 0));

        // Copiar los valores de la imagen original a la nueva imagen redimensionada
        for (size_t i = 0; i < image.size(); i++) {
            for (size_t j = 0; j < image[i].size(); j++) {
                resizedImage[i][j] = image[i][j];  // Copiar el valor original
            }
        }

        // Agregar la imagen redimensionada al volumen final
        volumeData.push_back(resizedImage);
    }

    // Mensaje de éxito indicando la cantidad de imágenes cargadas y sus dimensiones unificadas
    cout << "El volumen " << base << " ha sido cargado con " << num_images
         << " imágenes, todas redimensionadas a " << maxWidth << "x" << maxHeight << "." << endl;
         height = maxHeight;
         width = maxWidth;
}

void ImageProcessingSystem::infoImage() {
  if (imageFilename.empty()) {
            cout << "No hay una imagen cargada en memoria." << endl;
        } else {
            cout << "Imagen cargada en memoria: " << imageFilename << endl;
            cout << "Dimensiones: " << width << " x " << height << " píxeles" << endl;
            cout << "Valor máximo de píxel: " << maxPixelValue << endl;
        }
}
void ImageProcessingSystem::infoVolume() {
  if ( volumeData.empty()) {
            cout << "No hay un volumen cargado en memoria." << endl;
        } else {
            cout << "Volumen cargado en memoria: " << volume << endl;
            cout << "Cantidad de imágenes: " << volumeData.size() << endl;
        if (!volumeData.empty()) {
            cout << "Dimensiones de cada imagen: " << width << " x " << height << " píxeles" << endl;
            cout << "Valor máximo de píxel: " << maxPixelValue << endl;
        }
        }
    }
void ImageProcessingSystem::projection2D(string param) { 
  stringstream ss(param);
    string direccion, criterio, filename;

    ss >> direccion >> criterio >> filename;

    if (direccion.empty() || criterio.empty() || filename.empty()) {
        cout << "Uso incorrecto del comando. Escriba 'ayuda proyeccion2D' para más información." << endl;
        return;
    }

    if (volumeData.empty()) {
        cout << "Error: No hay un volumen cargado en memoria." << endl;
        return;
    }

    int profundidad = volumeData.size(); // Número de imágenes en el volumen
    vector<vector<int>> resultado;

    if (direccion == "z") {  // Proyección en el eje Z (vista superior)
        resultado.assign(height, vector<int>(width, 0));

        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                vector<int> valoresZ;
                for (int k = 0; k < profundidad; k++) {
                    valoresZ.push_back(volumeData[k][i][j]);
                }

                if (criterio == "max") {
                    resultado[i][j] = *max_element(valoresZ.begin(), valoresZ.end());
                } else if (criterio == "min") {
                    resultado[i][j] = *min_element(valoresZ.begin(), valoresZ.end());
                } else if (criterio == "prom") {
                    resultado[i][j] = accumulate(valoresZ.begin(), valoresZ.end(), 0) / profundidad;
                } else if (criterio == "med") {
                    sort(valoresZ.begin(), valoresZ.end());
                    int n = valoresZ.size();
                    if (n % 2 == 1) {
                        resultado[i][j] = valoresZ[n / 2]; // Si es impar, tomamos el del centro
                    } else {
                        resultado[i][j] = (valoresZ[n / 2 - 1] + valoresZ[n / 2]) / 2; // Si es par, promediamos los dos del centro
                    }
                } else {
                    cout << "Error: Criterio no válido. Use 'max', 'min', 'prom' o 'med'." << endl;
                    return;
                }
            }
        }

    } else if (direccion == "y") {  // Proyección en el eje Y (vista lateral)
        resultado.assign(profundidad, vector<int>(width, 0));

        for (int k = 0; k < profundidad; k++) {
            for (int j = 0; j < width; j++) {
                vector<int> valoresY;
                for (int i = 0; i < height; i++) {
                    valoresY.push_back(volumeData[k][i][j]);
                }

                if (criterio == "max") {
                    resultado[k][j] = *max_element(valoresY.begin(), valoresY.end());
                } else if (criterio == "min") {
                    resultado[k][j] = *min_element(valoresY.begin(), valoresY.end());
                } else if (criterio == "prom") {
                    resultado[k][j] = accumulate(valoresY.begin(), valoresY.end(), 0) / height;
                } else if (criterio == "med") {
                    sort(valoresY.begin(), valoresY.end());
                    int n = valoresY.size();
                    if (n % 2 == 1) {
                        resultado[k][j] = valoresY[n / 2]; // Si es impar, tomamos el del centro
                    } else {
                        resultado[k][j] = (valoresY[n / 2 - 1] + valoresY[n / 2]) / 2; // Si es par, promediamos los dos del centro
                    }
                } else {
                    cout << "Error: Criterio no válido. Use 'max', 'min', 'prom' o 'med'." << endl;
                    return;
                }
            }
        }

    } else if (direccion == "x") {  // Proyección en el eje X (vista frontal)
        resultado.assign(height, vector<int>(profundidad, 0));

        for (int i = 0; i < height; i++) {
            for (int k = 0; k < profundidad; k++) {
                vector<int> valoresX;
                for (int j = 0; j < width; j++) {
                    valoresX.push_back(volumeData[k][i][j]);
                }

                if (criterio == "max") {
                    resultado[i][k] = *max_element(valoresX.begin(), valoresX.end());
                } else if (criterio == "min") {
                    resultado[i][k] = *min_element(valoresX.begin(), valoresX.end());
                } else if (criterio == "prom") {
                    resultado[i][k] = accumulate(valoresX.begin(), valoresX.end(), 0) / width;
                } else if (criterio == "med") {
                    sort(valoresX.begin(), valoresX.end());
                    int n = valoresX.size();
                    if (n % 2 == 1) {
                        resultado[i][k] = valoresX[n / 2]; // Si es impar, tomamos el del centro
                    } else {
                        resultado[i][k] = (valoresX[n / 2 - 1] + valoresX[n / 2]) / 2; // Si es par, promediamos los dos del centro
                    }
                } else {
                    cout << "Error: Criterio no válido. Use 'max', 'min', 'prom' o 'med'." << endl;
                    return;
                }
            }
        }

    } else {
        cout << "Error: Dirección no válida. Use 'x', 'y' o 'z'." << endl;
        return;
    }

    // Guardar la imagen proyectada en un archivo PGM
    ofstream file(filename);
    if (!file) {
        cout << "Error: No se pudo crear el archivo " << filename << endl;
        return;
    }

    file << "P2\n";
    file << "# Proyección 2D generada\n";

    if (direccion == "z") {
        file << width << " " << height << "\n";
    } else if (direccion == "y") {
        file << width << " " << profundidad << "\n";
    } else if (direccion == "x") {
        file << profundidad << " " << height << "\n";
    }

    file << maxPixelValue << "\n";

    for (const auto& fila : resultado) {
        for (int pixel : fila) {
            file << pixel << " ";
        }
        file << "\n";
    }

    file.close();
    cout << "Proyección 2D guardada en " << filename << endl;
}
void ImageProcessingSystem::encodeImage(string param) {
    if (imageFilename.empty()) {                                        // 1. Verifica si hay una imagen cargada
        cout << "No hay una imagen cargada en memoria." << endl;
        return;                                                        //    Si no, sale
    }
    string outName = param;                                             // 2. Nombre de salida (.huf)
    if (outName.find(".huf") == string::npos) outName += ".huf";        //    Añade extensión si falta

    // 3. Contar frecuencias de cada nivel de gris
    vector<unsigned long> freq(maxPixelValue + 1, 0);
    for (int i = 0; i < height; ++i)                                    //    Recorre filas
        for (int j = 0; j < width; ++j)                                //    y columnas
            freq[ imageData[i][j] ]++;

    // 4. Construir el árbol de Huffman
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNode> pq;
    for (int i = 0; i <= maxPixelValue; ++i)                           //    Para cada símbolo
        pq.push(new HuffmanNode(i, freq[i]));                         //    crea un nodo y añade

    while (pq.size() > 1) {                                            //    Hasta que quede solo la raíz
        HuffmanNode* n1 = pq.top(); pq.pop();                          // 1) Extrae nodo de menor freq
        HuffmanNode* n2 = pq.top(); pq.pop();                          // 2) Extrae siguiente menor
        HuffmanNode* parent = new HuffmanNode(-1, n1->freq + n2->freq);// 3) Nodo interno suma freq
        parent->left = n1;                                             //    Hijos
        parent->right = n2;
        pq.push(parent);                                               // 4) Reinsertar
    }
    HuffmanNode* root = pq.top();

    // 5. Generar códigos binarios
    vector<string> codes(maxPixelValue + 1);
    buildCodes(root, "", codes);                                      // Llenar vector codes

    // 6. Abrir archivo binario para escritura
    ofstream out(outName, ios::binary);
    unsigned short w = width, h = height;
    unsigned char m = static_cast<unsigned char>(maxPixelValue);
    out.write(reinterpret_cast<char*>(&w), sizeof(w));                // Escribir ancho
    out.write(reinterpret_cast<char*>(&h), sizeof(h));                // Escribir alto
    out.put(static_cast<char>(m));                                    // Escribir M

    // 7. Escribir frecuencias
    for (int i = 0; i <= maxPixelValue; ++i)
        out.write(reinterpret_cast<char*>(&freq[i]), sizeof(unsigned long));

    // 8. Empaquetar bits y escribir datos comprimidos
    string bitBuffer;
    for (int i = 0; i < height; ++i)
        for (int j = 0; j < width; ++j)
            bitBuffer += codes[ imageData[i][j] ];                   // concatenar códigos

    size_t idx = 0;
    while (idx < bitBuffer.size()) {                                  // Hasta recorrer todo
        unsigned char byte = 0;
        for (int b = 0; b < 8; ++b) {
            byte <<= 1;
            if (idx < bitBuffer.size() && bitBuffer[idx++] == '1')
                byte |= 1;                                           // setea bit a 1 si corresponde
        }
        out.put(static_cast<char>(byte));                            // escribir byte
    }
    out.close();                                                      // cerrar archivo

    cout << "La imagen en memoria ha sido codificada exitosamente y almacenada en el archivo "
         << outName << "." << endl;                                  // Mensaje de éxito
    }
void ImageProcessingSystem::decodeFile(string param) {
  stringstream ss(param);
    string inName, pgmName;
    ss >> inName >> pgmName;                                          // 1. Obtener nombres

    ifstream in(inName, ios::binary);
    if (!in) {                                                        // 2. Verificar apertura
        cout << "El archivo " << inName << " no ha podido ser decodificado." << endl;
        return;
    }

    unsigned short w, h;
    unsigned char m;
    in.read(reinterpret_cast<char*>(&w), sizeof(w));                  // 3. Leer ancho
    in.read(reinterpret_cast<char*>(&h), sizeof(h));                  // 4. Leer alto
    in.read(reinterpret_cast<char*>(&m), 1);                          // 5. Leer M

    int maxVal = static_cast<int>(m);
    vector<unsigned long> freq(maxVal + 1);
    for (int i = 0; i <= maxVal; ++i)
        in.read(reinterpret_cast<char*>(&freq[i]), sizeof(unsigned long)); // 6. Leer frecuencias

    // 7. Reconstruir árbol de Huffman (similar a encode)
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNode> pq;
    for (int i = 0; i <= maxVal; ++i)
        pq.push(new HuffmanNode(i, freq[i]));
    while (pq.size() > 1) {
        HuffmanNode* n1 = pq.top(); pq.pop();
        HuffmanNode* n2 = pq.top(); pq.pop();
        HuffmanNode* parent = new HuffmanNode(-1, n1->freq + n2->freq);
        parent->left = n1; parent->right = n2;
        pq.push(parent);
    }
    HuffmanNode* root = pq.top();

    // 8. Leer el resto como bytes y decodificar
    vector<vector<int>> img(h, vector<int>(w));
    HuffmanNode* node = root;
    int i = 0, j = 0;
    char byte;
    while (in.get(byte)) {
        for (int b = 7; b >= 0; --b) {                                // leer bit a bit
            bool bit = (byte >> b) & 1;
            node = bit ? node->right : node->left;                   // seguir en el árbol
            if (node->symbol >= 0) {                                 // si es hoja
                img[i][j++] = node->symbol;
                node = root;                                         // volver a raíz
                if (j == w) { j = 0; if (++i == h) break; }         // siguiente pixel/fila
            }
        }
        if (i == h) break;                                            // terminado
    }
    in.close();                                                      // cerrar

    // 9. Escribir PGM resultante
    ofstream out(pgmName);
    out << "P2\n" << w << " " << h << "\n" << maxVal << "\n";
    for (auto& row : img) {
        for (int pix : row) out << pix << " ";
        out << "\n";
    }
    out.close();

    cout << "El archivo " << inName << " ha sido decodificado exitosamente y guardado en "
         << pgmName << "." << endl;                                 // Éxito
    }
void ImageProcessingSystem::segmentImage(string param) {
  cout << "No hay una imagen cargada en memoria." << endl;
    }
  
