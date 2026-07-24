#include "Bus.h"
#include "Cartridge.h"

uint8_t Bus::cpuRead(uint16_t addr)
{
    // Rango $0000 - $1FFF: RAM interna del sistema (2 KB reales + 3 espejos/mirrors de 2 KB cada uno).
    if (addr < 0x2000)
    {
        // La operación Bitwise AND (addr & 0x07FF) aplica la máscara del espejo (Mirroring):
        // Mapea las direcciones de $0800 a $1FFF directamente a los primeros 2 KB ($0000-$07FF).
        return ram[addr & 0x07FF];
    }

    // Rango $8000 - $FFFF: Espacio reservado para la PRG-ROM del Cartucho (Mappers).
    if (addr >= 0x8000)
    {
        return cartridge->ReadPRG(addr);
    }

    // Si se lee en registros no mapeados todavía (ej. PPU $2000-$2007, APU/IO $4000-$4017), retorna 0 por defecto.
    return 0;
}

void Bus::cpuWrite(uint16_t addr, uint8_t data)
{
    // Rango $0000 - $1FFF: Escritura en la RAM interna aplicando la misma regla de espejo (0x07FF).
    if (addr < 0x2000)
    {
        ram[addr & 0x07FF] = data;
    }
}