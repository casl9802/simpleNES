#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <cstdint>

// Cabecera estándar iNES de 16 bytes.
struct INesHeader
{
    char magic[4];      // Identificador de formato (Firma constante: "NES" seguido de 0x1A)

    uint8_t prgRomSize; // Cantidad de bancos de PRG-ROM (Cada banco equivale a 16 KB)
    uint8_t chrRomSize; // Cantidad de bancos de CHR-ROM (Cada banco equivale a 8 KB. Si es 0, usa CHR-RAM)

    uint8_t flags6;     // Controla el Mapper (Nibble bajo), Mirroring, Battery-backed RAM y Trainer
    uint8_t flags7;     // Controla el Mapper (Nibble alto), VS Unisystem, PlayChoice-10 y formato iNES 2.0

    uint8_t prgRamSize; // Tamaño de la PRG-RAM (En bloques de 8 KB. Raramente usado en iNES clásico)

    uint8_t flags9;     // Configuración de región (0: NTSC, 1: PAL). Poco común en emuladores básicos
    uint8_t flags10;    // Flags no estándar (Tipo de placa, PRG-RAM en iNES no oficial)

    uint8_t zero[5];    // Bytes de relleno. Deben ser 0, aunque algunas ROMs viejas tienen basura aquí
};

/**
 * @brief Representa el cartucho físico de la NES (ROM).
 * Maneja la carga del archivo y el almacenamiento de los bancos de memoria principales.
 */
class Cartridge
{
public:
    // Almacena el código ejecutable del juego (instrucciones para la CPU 6502).
    // Su tamaño total será siempre un múltiplo de 16 KB (16,384 bytes).
    std::vector<uint8_t> prgROM;

    // Punto de entrada para parsear el archivo .nes y validar su cabecera (iNES).
    bool Load(const std::string& filename);

    /**
     * @brief Traduce la dirección de memoria de la CPU ($8000 - $FFFF) al offset real de prgROM.
     * Implementa la lógica básica de mapeo (Mapper 0 / NROM).
     */
    uint8_t ReadPRG(uint16_t addr);

private:
    INesHeader header;

    // Memoria PRG-ROM (Program ROM): Contiene el código ejecutable del juego (CPU).
    std::vector<uint8_t> m_PRG_ROM;

    // Memoria CHR-ROM (Character ROM): Contiene los patrones de gráficos/sprites (PPU).
    std::vector<uint8_t> m_CHR_ROM;


    // Memoria CHR-ROM (Character ROM): Contiene los patrones visuales (sprites y tiles de fondo).
    // La PPU lee esta memoria directamente. Su tamaño suele ser múltiplo de 8 KB (8,192 bytes).
    std::vector<uint8_t> chrROM;
};