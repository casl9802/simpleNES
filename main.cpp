#include "iostream"

#include "Cartridge.h"
#include "Bus.h"
#include "CPU.h"

int main()
{

    Cartridge cartridge;

    cartridge.Load("../roms/SuperMarioBros.nes");

    // LDA #$42
    // Immediate
    cartridge.prgROM[0] = 0xA9;
    cartridge.prgROM[1] = 0x42;

    // STA $0010
    cartridge.prgROM[2] = 0x8D;
    cartridge.prgROM[3] = 0x10;
    cartridge.prgROM[4] = 0x00;

    // LDA $10
    // Zero Page
    cartridge.prgROM[5] = 0xA5;
    cartridge.prgROM[6] = 0x10;

    // STA $0200
    cartridge.prgROM[7] = 0x8D;
    cartridge.prgROM[8] = 0x00;
    cartridge.prgROM[9] = 0x02;

    // LDA $0200
    // Absolute
    cartridge.prgROM[10] = 0xAD;
    cartridge.prgROM[11] = 0x00;
    cartridge.prgROM[12] = 0x02;

    Bus bus;

    bus.cartridge = &cartridge;

    CPU cpu;

    cpu.bus = &bus;

    // 1. Lee el Reset Vector ($FFFC-$FFFD) desde el cartucho y posiciona el PC al inicio del programa del juego.
    cpu.Reset();

    // 2. Bucle principal de prueba:
    // Simula 6 pulsos de reloj / ciclos de instrucción para ejecutar los primeros opcodes almacenados en la PRG-ROM.
    for(int i = 0; i < 5; i++)
    {
        cpu.Clock();
    }

    std::cout
        << "$0010 = 0x"
        << std::hex
        << static_cast<int>(bus.cpuRead(0x0010))
        << "\n";

    std::cout
        << "$0200 = 0x"
        << std::hex
        << static_cast<int>(bus.cpuRead(0x0200))
        << "\n";

    cpu.DumpState();

    std::cout << "\n===== RAM TEST =====\n";

    std::cout << "$0200 = 0x"
            << std::hex
            << static_cast<int>(bus.cpuRead(0x0200))
            << "\n";

    std::cout << "$0201 = 0x"
            << std::hex
            << static_cast<int>(bus.cpuRead(0x0201))
            << "\n";

    std::cout << "$0202 = 0x"
            << std::hex
            << static_cast<int>(bus.cpuRead(0x0202))
            << "\n";

    std::cout << "====================\n";

    return 0;
}