/**
 * S T E W A R T    P L A T F O R M    O N    E S P 3 2
 *
 * Copyright (C) 2019  Nicolas Jeanmonod, ouilogique.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <Hexapod_Servo.h>
#include <Hexapod_GPIO.h>

extern angle_t servo_angles[NB_SERVOS];
extern Hexapod_GPIO hx_gpio;

/**
 *
 */
Hexapod_Servo::Hexapod_Servo()
{
}

/**
 * Inicializa los servos y va a HOME
 */
void Hexapod_Servo::setupServo()
{
    // Inicializar los 6 servos
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++) {
        servos[sid].attach(PIN_SERVO[sid], 500, 2500);
    }

    // Ir a posición HOME
    int8_t movOK = home(servo_angles);
    this->updateServos(movOK);
}

/**
 * Envía las posiciones a los servos de manera directa y estable.
 */
void Hexapod_Servo::updateServos(int8_t movOK, unsigned long safetyWait_ms)
{
    static double nbMov = 0;
    static double nbMovNOK = 0;
    static double NOKrate = 0;
    nbMov++;

    if (movOK == 0)
    {
        // Escribimos directamente en los servos. 
        // Al no usar detach ni delays, la plataforma se mantiene firme 
        // y obedece exactamente lo que dicta la cinemática.
        for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
        {
            servos[sid].write(servo_angles[sid].pwm_us);
        }
    }
    else
    {
        // Manejo de errores si la cinemática no puede llegar a la posición
        hx_gpio.setBuiltInLED();
        nbMovNOK++;
        NOKrate = (nbMovNOK / nbMov) * 100.0;
        
        Serial.printf("%10lu | BAD MOVE | movOK = %d", millis(), movOK);
        Serial.printf(" | NB MOV = %10.0f", nbMov);
        Serial.printf(" | NOK rate = %4.1f %%\n", NOKrate);
    }

    hx_gpio.clearBuiltInLEDDelayed(20);
}

/**
 * Imprime los ángulos actuales de los servos (para debug)
 */
void Hexapod_Servo::printServoAngles()
{
    Serial.print("\nSERVO COORD        = ");
    Serial.print(getHX_X());
    Serial.print(" ");
    Serial.print(getHX_Y());
    Serial.print(" ");
    Serial.print(getHX_Z());
    Serial.print(" ");
    Serial.print(getHX_A());
    Serial.print(" ");
    Serial.print(getHX_B());
    Serial.print(" ");
    Serial.print(getHX_C());

    Serial.print("\nSERVO_ANGLES (rad) = ");
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        Serial.print(servo_angles[sid].deg);
        Serial.print(" ");
    }

    Serial.print("\nSERVO_ANGLES (us) = ");
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        Serial.print(servo_angles[sid].us);
        Serial.print(" ");
    }

    Serial.print("\nSERVO_ANGLES (debug) = ");
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        Serial.print(servo_angles[sid].debug);
        Serial.print(" ");
    }
    Serial.print("\n");
}

/**
 * Imprime las coordenadas geométricas (para debug)
 */
void Hexapod_Servo::printJointAndServoAxisCoord()
{
    Serial.println("P_COORDS");
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        Serial.print(P_COORDS[sid][0]);
        Serial.print("  ");
        Serial.println(P_COORDS[sid][1]);
    }

    Serial.println("\nB_COORDS");
    for (uint8_t sid = 0; sid < NB_SERVOS; sid++)
    {
        Serial.print(B_COORDS[sid][0]);
        Serial.print("  ");
        Serial.println(B_COORDS[sid][1]);
    }
}