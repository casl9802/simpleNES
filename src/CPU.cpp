#include "CPU.h"
#include "Bus.h"

#include <iostream>
#include <iomanip>

void CPU::Reset()
{
    // La arquitectura 6502 reserva las direcciones fijas $FFFC y $FFFD para almacenar
    // la dirección de memoria inicial (Reset Vector) donde la CPU debe comenzar a ejecutar código.

    // 1. Lectura del byte bajo (Little-Endian) en $FFFC:
    uint8_t lo = bus->cpuRead(0xFFFC);

    // 2. Lectura del byte alto en $FFFD:
    uint8_t hi = bus->cpuRead(0xFFFD);

    // 3. Reconstrucción de la dirección completa de 16 bits combinando ambos bytes:
    PC = (hi << 8) | lo;

    // Impresión de depuración para confirmar qué dirección tomó de la PRG-ROM.
    std::cout 
        << "CPU Reset Vector: 0x"
        << std::hex
        << PC
        << "\n";
}

void CPU::Clock()
{
    // 1. FASE DE FETCH (Búsqueda):
    // Lee el byte de la instrucción actual en la dirección apuntada por el Program Counter (PC).
    opcode = bus->cpuRead(PC);

    // Incrementa el PC para apuntar al siguiente byte (que puede ser un operador o el siguiente opcode).
    PC++;

    // 2. FASE DE DECODE & EXECUTE (Decodificación y Ejecución):
    switch(opcode)
    {
        // LDA #Inmediato (Load Accumulator)
        // Carga un valor constante de 8 bits directamente en el Registro Acumulador (A).
        case 0xA9: 
        {
            // Lee el argumento del operando que se encuentra inmediatamente después del opcode en memoria.
            uint8_t value = bus->cpuRead(PC);

            // Incrementa nuevamente el PC para dejarlo posicionado en la siguiente instrucción.
            PC++;

            // Asigna el valor leído al registro Acumulador.
            A = value;
            UpdateZN(A);

            std::cout 
                << "LDA executed. A = "
                << std::dec
                << (int)A
                << "\n";

            break;
        }

        // LDX #Inmediato (Load X Register)
        // Carga un valor de 8 bits directamente en el Registro de Índice X.
        case 0xA2:
        {
            // Lee el valor constante almacenado en la dirección actual del PC.
            X = bus->cpuRead(PC);

            // Incrementa el Program Counter para avanzar a la siguiente instrucción.
            PC++;
            
            UpdateZN(X);

            std::cout << "LDX executed. X = " << (int)X << "\n";
            break;
        }

        // LDY #Inmediato (Load Y Register)
        // Carga un valor de 8 bits directamente en el Registro de Índice Y.
        case 0xA0:
        {
            // Lee el valor constante almacenado en la dirección actual del PC.
            Y = bus->cpuRead(PC);

            // Incrementa el Program Counter para avanzar a la siguiente instrucción.
            PC++;
            UpdateZN(Y);

            std::cout << "LDY executed. Y = " << (int)Y << "\n";
            break;
        }

        // TAX (Transfer Accumulator to X)
        // Copia el valor contenido en el Acumulador (A) hacia el Registro de Índice X.
        // Modo de direccionamiento: Implícito / Implied (no requiere operandos adicionales en memoria).
        case 0xAA:
        {
            // Transfiere directamente el contenido del registro A hacia X.
            X = A;
            UpdateZN(X);

            std::cout << "TAX executed. X = " << (int)X << "\n";
            break;
        }

        // INX (Increment X Register)
        // Incrementa en 1 el valor del Registro de Índice X.
        // Modo de direccionamiento: Implícito / Implied.
        case 0xE8:
        {
            // Incrementa X. Al ser un entero de 8 bits (uint8_t), si X es 0xFF (255)
            // desbordará automáticamente a 0x00 (256 % 256 = 0), replicando el "wrap-around" del hardware real.
            X++;
            UpdateZN(X);

            std::cout << "INX executed. X = " << (int)X << "\n";
            break;
        }

        case 0x8D: // STA Absolute
        {
            uint8_t lo = bus->cpuRead(PC);
            PC++;

            uint8_t hi = bus->cpuRead(PC);
            PC++;

            uint16_t addr = (static_cast<uint16_t>(hi) << 8) | lo;

            bus->cpuWrite(addr, A);

            std::cout
                << "STA executed. Stored A="
                << std::dec << static_cast<int>(A)
                << " at 0x"
                << std::hex << addr
                << "\n";

            break;
        }

        case 0x8E: // STX Absolute
        {
            uint8_t lo = bus->cpuRead(PC);
            PC++;

            uint8_t hi = bus->cpuRead(PC);
            PC++;

            uint16_t addr = (static_cast<uint16_t>(hi) << 8) | lo;

            bus->cpuWrite(addr, X);

            std::cout
                << "STX executed. Stored X="
                << std::dec << static_cast<int>(X)
                << " at 0x"
                << std::hex << addr
                << "\n";

            break;
        }

        case 0x8C: // STY Absolute
        {
            uint8_t lo = bus->cpuRead(PC);
            PC++;

            uint8_t hi = bus->cpuRead(PC);
            PC++;

            uint16_t addr = (static_cast<uint16_t>(hi) << 8) | lo;

            bus->cpuWrite(addr, Y);

            std::cout
                << "STY executed. Stored Y="
                << std::dec << static_cast<int>(Y)
                << " at 0x"
                << std::hex << addr
                << "\n";

            break;
        }

        default:
            // Manejo de Opcodes no implementados o instrucciones 'Ilegales' de la 6502.
            std::cout 
                << "Unknown opcode: 0x"
                << std::hex
                << (int)opcode
                << "\n";

            break;
    }
}

bool CPU::GetFlag(FLAGS6502 f)
{
    return (status & f) != 0;
}

void CPU::SetFlag(FLAGS6502 f, bool value)
{
    if (value)
        status |= f;
    else
        status &= ~f;
}

void CPU::UpdateZN(uint8_t value)
{
    SetFlag(Z, value == 0);
    SetFlag(N, value & 0x80);
}

void CPU::DumpState()
{
    std::cout << "===== CPU STATE =====\n";

    std::cout << std::hex << std::uppercase;

    std::cout << "PC: 0x" << std::setw(4) << std::setfill('0') << PC << "\n";
    std::cout << "Opcode actual: 0x" << std::setw(2) << std::setfill('0') << (int)opcode << "\n";

    std::cout << std::dec;
    std::cout << "A:  0x" << std::hex << std::setw(2) << std::setfill('0') << (int)A
               << " (" << std::dec << (int)A << ")\n";
    std::cout << "X:  0x" << std::hex << std::setw(2) << std::setfill('0') << (int)X
               << " (" << std::dec << (int)X << ")\n";
    std::cout << "Y:  0x" << std::hex << std::setw(2) << std::setfill('0') << (int)Y
               << " (" << std::dec << (int)Y << ")\n";

    std::cout << "Status: 0x" << std::hex << std::setw(2) << std::setfill('0') << (int)status << "\n";

    // Desglose de flags individuales (ajusta los nombres a tu enum FLAGS6502 real)
    std::cout << "Flags: "
               << "N=" << GetFlag(N) << " "
               << "V=" << GetFlag(V) << " "
               << "U=" << GetFlag(U) << " "
               << "B=" << GetFlag(B) << " "
               << "D=" << GetFlag(D) << " "
               << "I=" << GetFlag(I) << " "
               << "Z=" << GetFlag(Z) << " "
               << "C=" << GetFlag(C) << "\n";

    std::cout << "======================\n";
}