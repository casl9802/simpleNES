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

        // LDX #Inmediato (Load X Register)
        // Carga un valor de 8 bits directamente en el Registro de Índice X.
        case 0xA2:
        {
            // Lee el valor constante almacenado en la dirección actual del PC.
            X = bus->cpuRead(PC);

            // Incrementa el Program Counter para avanzar a la siguiente instrucción.
            PC++;

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