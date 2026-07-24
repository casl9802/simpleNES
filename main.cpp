#include "iostream"

#include "Cartridge.h"
#include "Bus.h"
#include "CPU.h"

int main()
{

    Cartridge cartridge;


    cartridge.Load("../roms/SuperMarioBros.nes");

    cartridge.prgROM[0] = 0xA9;
    cartridge.prgROM[1] = 0x10;

    cartridge.prgROM[2] = 0xAA;

    cartridge.prgROM[3] = 0xE8;

    cartridge.prgROM[4] = 0xA2;
    cartridge.prgROM[5] = 0x55;

    cartridge.prgROM[6] = 0xA0;
    cartridge.prgROM[7] = 0x00;

    Bus bus;

    bus.cartridge = &cartridge;


    CPU cpu;

    cpu.bus = &bus;

    // 1. Lee el Reset Vector ($FFFC-$FFFD) desde el cartucho y posiciona el PC al inicio del programa del juego.
    cpu.Reset();

    // 2. Bucle principal de prueba:
    // Simula 10 pulsos de reloj / ciclos de instrucción para ejecutar los primeros opcodes almacenados en la PRG-ROM.
    for(int i = 0; i < 5; i++)
    {
        cpu.Clock();
    }

    bus.cpuWrite(0x0000, 42);
    bus.cpuWrite(0x0800, 84);

    std::cout << (int)bus.cpuRead(0x0000) << '\n';
    std::cout << (int)bus.cpuRead(0x0800) << '\n';

    cpu.DumpState();

    return 0;
}