#pragma once

#include <cstdint>

class Bus;

/**
 * @brief Emulación de la CPU MOS 6502 (Variante Ricoh 2A03 sin modo BCD).
 */
class CPU
{
public:
    // Conexión al bus global de datos para realizar transferencias con memoria e I/O.
    Bus* bus = nullptr;

    // Registros internos de la arquitectura 6502:
    uint8_t A = 0;      // Acumulador (Operaciones aritméticas y lógicas)
    uint8_t X = 0;      // Registro de índice X (Uso general, modificación de direcciones)
    uint8_t Y = 0;      // Registro de índice Y (Uso general, modificación de direcciones)

    uint8_t SP = 0xFD;  // Stack Pointer: Apunta al espacio de la pila en RAM ($0100 - $01FF). Por defecto 0xFD en boot.

    uint16_t PC = 0;    // Program Counter: Apunta a la siguiente instrucción a ejecutar en memoria.

    uint8_t status = 0; // Registro de flags (NV-BDIZC: Negative, Overflow, Break, Decimal, Interrupt Disable, Zero, Carry).

    uint8_t opcode = 0; // Almacena el opcode de 8 bits leído en la fase de Fetch. 

    enum FLAGS6502
    {
        C = 1 << 0,
        Z = 1 << 1,
        I = 1 << 2,
        D = 1 << 3,
        B = 1 << 4,
        U = 1 << 5,
        V = 1 << 6,
        N = 1 << 7
    };
    
    bool GetFlag(FLAGS6502 f);
    void SetFlag(FLAGS6502 f, bool value);

    void UpdateZN(uint8_t value);
    void DumpState();

    // Simula la señal de interrupción física de RESET (Inicialización del hardware).
    void Reset();


    /**
     * @brief Ejecuta un pulso de reloj (o ciclo de instrucción) de la CPU.
     * Realiza la búsqueda de la instrucción (Fetch), avanza el PC y ejecuta
     * la lógica correspondiente según la tabla de Opcodes (Decode & Execute).
     */
    void Clock();

};