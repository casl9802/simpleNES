#pragma once

#include <cstdint>
#include <array>

// Forward declaration para evitar dependencias circulares de compilación.
class Cartridge;

/**
 * @brief Interconexión del bus principal de la CPU (Espacio de direccionamiento de 16-bits / 64 KB).
 * Rutea todas las lecturas y escrituras hacia sus respectivos periféricos (RAM, Cartucho, PPU, APU).
 */
class Bus
{
public:
    // Petición de lectura en el bus de direcciones por parte de la CPU.
    uint8_t cpuRead(uint16_t addr);

    // Petición de escritura en el bus de datos por parte de la CPU.
    void cpuWrite(uint16_t addr, uint8_t data);

    // Referencia al cartucho insertado para rutar el espacio $4020 - $FFFF.
    Cartridge* cartridge = nullptr;

private:
    // RAM interna del sistema (2 KB / 2048 bytes) mapeada en la región $0000 - $07FF.
    uint8_t ram[2048]{};

    std::array<uint8_t, 2048> cpuRam{};
};