#include "Cartridge.h"

#include <fstream>
#include <iostream>
#include <cstdio>

bool Cartridge::Load(const std::string& filename)
{
    std::cout << "Opening: " << filename << "\n";

    // Apertura en modo binario para evitar alteraciones en los bytes de control de la ROM
    std::ifstream file(filename, std::ios::binary);

    if (!file)
    {
        std::cout << "ERROR: ROM not found.\n";
        return false;
    }

    std::cout << "ROM opened successfully.\n";
    
    // Lectura cruda secuencial de los primeros 16 bytes directamente sobre la estructura.
    file.read(reinterpret_cast<char*>(&header), sizeof(INesHeader));

    // Validación obligatoria del "Magic Number" para descartar archivos corruptos o formatos incorrectos.
    if (header.magic[0] != 'N' ||
        header.magic[1] != 'E' ||
        header.magic[2] != 'S' ||
        header.magic[3] != 0x1A)
    {
        std::cout << "Invalid NES ROM.\n";
        return false;
    }

    std::cout << "\n===== iNES HEADER =====\n";

    // El casteo a (int) es necesario para que std::cout muestre el valor numérico y no el carácter ASCII.
    std::cout << "PRG ROM Banks : "
            << (int)header.prgRomSize
            << '\n';

    std::cout << "CHR ROM Banks : "
            << (int)header.chrRomSize
            << '\n';

    // Reconstrucción del número de Mapper (ID del chip de expansión del cartucho):
    // El nibble alto de flags6 representa los 4 bits bajos del Mapper.
    // El nibble alto de flags7 representa los 4 bits altos del Mapper.
    uint8_t mapper =
        (header.flags6 >> 4) |
        (header.flags7 & 0xF0);

    std::cout << "Mapper        : "
            << (int)mapper
            << '\n';

    // El Bit 0 de flags6 define cómo la PPU maneja el desborde de memoria de Nametables (Mirroring).
    // Fundamental para el scroll de la pantalla: Horizontal (0) o Vertical (1).
    std::cout << "Mirroring     : "
            << ((header.flags6 & 1) ? "Vertical" : "Horizontal")
            << '\n';

    std::cout << "=======================\n";

    // Cálculo del tamaño real de la PRG-ROM:
    // Cada unidad (banco) especificada en la cabecera equivale a 16 KB (16,384 bytes).
    const size_t prgSize = header.prgRomSize * 16384;

    // 2. Reserva de memoria contigua en el vector para alojar todos los bytes del código del juego.
    prgROM.resize(prgSize);

    // 3. Lectura directa del archivo:
    // Se pasa el puntero al buffer subyacente del vector (prgRom.data()) casteado a char*.
    // El puntero del archivo avanza automáticamente desde el final de la cabecera (o del Trainer si existiera).
    file.read(reinterpret_cast<char*>(prgROM.data()), prgSize);

    // 4. Verificación de integridad:
    // Asegura que el archivo no esté truncado y que efectivamente contenía los bytes prometidos por el header.
    if (!file)
    {
        std::cout << "Failed reading PRG ROM.\n";
        return false;
    }

    // Saltar trainer si existe
    if (header.flags6 & 0x04)
    {
        file.seekg(512, std::ios::cur);
    }

    // Leer PRG
    prgROM.resize(header.prgRomSize * 16384);
    file.read(reinterpret_cast<char*>(prgROM.data()), prgROM.size());

    // Leer CHR
    chrROM.resize(header.chrRomSize * 8192);

    // Manejo de la memoria de patrones gráficos (CHR):
    // Si header.chrRomSize > 0, el cartucho usa chips de ROM física para los gráficos.
    // Si es 0, el juego no incluye CHR-ROM sino CHR-RAM (se instanciarán 8 KB de RAM en el emulador
    // y la CPU escribirá las texturas durante la ejecución, común en juegos como Mega Man o Final Fantasy).
    if (header.chrRomSize > 0)
    {
        // 1. Cálculo del tamaño: Cada banco de CHR-ROM equivale a 8 KB (8,192 bytes).
        const size_t chrSize = header.chrRomSize * 8192;

        // 2. Redimensionamiento del vector para reservar el espacio exacto.
        chrROM.resize(chrSize);

        // 3. Lectura directa del stream:
        // El puntero del archivo 'file' ya se encuentra posicionado justo al final de la PRG-ROM,
        // por lo que continúa la lectura secuencial de los bytes restantes del archivo .nes.
        file.read(reinterpret_cast<char*>(chrROM.data()), chrROM.size());
    }
    else
    {
        // Si no hay CHR-ROM, se inicializan 8 KB (8,192 bytes) de CHR-RAM por defecto.
        chrROM.resize(8192, 0);
    }

    // --- Resumen de diagnóstico en consola ---
    std::cout << "\n===== ROM DATA =====\n";
    std::cout << "PRG Size : " << prgROM.size() << " bytes\n";
    std::cout << "CHR Size : " << chrROM.size() << " bytes\n";
    std::cout << "====================\n\n";

    return true;
}

uint8_t Cartridge::ReadPRG(uint16_t addr)
{
    // Restamos el offset base ($8000) para normalizar la dirección a un índice de base cero (0x0000).
    uint32_t mapped = addr - 0x8000;

    // Manejo de Mirroring para juegos NROM con un único banco de 16 KB (como Super Mario Bros):
    // Si la PRG-ROM mide 16 KB ($4000 bytes), mapeamos las peticiones de $C000-$FFFF ($4000 bytes arriba)
    // como una copia exacta (espejo) de la región $8000-$BFFF mediante la máscara 0x3FFF (16383).
    if (prgROM.size() == 16384)
    {
        mapped &= 0x3FFF;
    }

    return prgROM[mapped];
}