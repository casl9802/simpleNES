#include "iostream"

#include "Cartridge.h"
#include "Bus.h"
#include "CPU.h"

int main()
{

    Cartridge cartridge;

    cartridge.Load("../roms/SuperMarioBros.nes");

    // LDA #$42
    cartridge.prgROM[0] = 0xA9;
    cartridge.prgROM[1] = 0x42;

    // STA $0200
    cartridge.prgROM[2] = 0x8D;
    cartridge.prgROM[3] = 0x00;
    cartridge.prgROM[4] = 0x02;

    // LDX #$55
    cartridge.prgROM[5] = 0xA2;
    cartridge.prgROM[6] = 0x55;

    // STX $0201
    cartridge.prgROM[7] = 0x8E;
    cartridge.prgROM[8] = 0x01;
    cartridge.prgROM[9] = 0x02;

    // LDY #$99
    cartridge.prgROM[10] = 0xA0;
    cartridge.prgROM[11] = 0x99;

    // STY $0202
    cartridge.prgROM[12] = 0x8C;
    cartridge.prgROM[13] = 0x02;
    cartridge.prgROM[14] = 0x02;

    Bus bus;

    bus.cartridge = &cartridge;

    CPU cpu;

    cpu.bus = &bus;

    // 1. Lee el Reset Vector ($FFFC-$FFFD) desde el cartucho y posiciona el PC al inicio del programa del juego.
    cpu.Reset();

    // 2. Bucle principal de prueba:
    // Simula 6 pulsos de reloj / ciclos de instrucción para ejecutar los primeros opcodes almacenados en la PRG-ROM.
    for(int i = 0; i < 6; i++)
    {
        cpu.Clock();
    }

    bus.cpuWrite(0x0000, 42);
    bus.cpuWrite(0x0800, 84);

    std::cout << (int)bus.cpuRead(0x0000) << '\n';
    std::cout << (int)bus.cpuRead(0x0800) << '\n';

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