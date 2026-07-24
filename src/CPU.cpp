#include "CPU.h"
#include "Bus.h"

#include <iostream>

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

            std::cout 
                << "LDA executed. A = "
                << std::dec
                << (int)A
                << "\n";

            break;
        }

    case 0xA2: // LDX Immediate
    {
        X = bus->cpuRead(PC);
        PC++;

        std::cout << "LDX executed. X = " << (int)X << "\n";
        break;
    }

    case 0xA0: // LDY Immediate
    {
        Y = bus->cpuRead(PC);
        PC++;

        std::cout << "LDY executed. Y = " << (int)Y << "\n";
        break;
    }

    case 0xAA: // TAX
    {
        X = A;

        std::cout << "TAX executed. X = " << (int)X << "\n";
        break;
    }

    case 0xE8: // INX
    {
        X++;

        std::cout << "INX executed. X = " << (int)X << "\n";
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