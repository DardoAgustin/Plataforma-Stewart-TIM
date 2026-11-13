#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <MPU6050_light.h>

#include <hexapod_Arduino.h>
#include <Hexapod_GPIO.h>
#include <Hexapod_Kinematics.h>
#include <Hexapod_Servo.h>
#include <Hexapod_Config_1.h>

angle_t servo_angles[NB_SERVOS];
Hexapod_Servo hx_servo;
Hexapod_GPIO hx_gpio;

MPU6050 mpu(Wire);
bool mpuOk = false;

float kp = 1.1;

float rollSuave = 0.0, rollCompensacion=0;
float pitchSuave = 0.0, pitchCompensacion=0;

const float ALPHA = 0.20; 
float rollReal=0, pitchReal = 0;
byte status;

//funcion de la IK de la plataforma.
void moverPlataforma(float X, float Y, float Z, float roll, float pitch, float yaw)
{
    // min y max
    Z = constrain(Z, -10.0, 20.0); 
    roll = constrain(roll, -15.0, 15.0);
    pitch = constrain(pitch, -15.0, 15.0);

    platform_t coord;
    coord.hx_x = X;
    coord.hx_y = Y;
    coord.hx_z = Z;
    coord.hx_a = roll  * DEG_TO_RAD;
    coord.hx_b = pitch * DEG_TO_RAD;
    coord.hx_c = yaw   * DEG_TO_RAD;

    int8_t movOK = hx_servo.calcServoAngles(coord, servo_angles);
    
    
        if (movOK == 0) 
            digitalWrite(2, 1);
        else {
            digitalWrite(2, 0);
            delay(50);
            digitalWrite(2, 1);
            delay(50);
            digitalWrite(2, 0);
        }
    
        hx_servo.updateServos(movOK);
}

void setup()
{
    pinMode(2, OUTPUT);
    digitalWrite(2, 1);
    delay(500);
    digitalWrite(2, 0);
    Serial.begin(115200);
    delay(1000); 

    hx_gpio.setupGPIO();
    hx_servo.setupServo();
    delay(1000);

    // Inicializar el MPU6050 en los pines I2C por defecto (21 y 22)
    
    Wire.begin(); 
    
    do {
        status = mpu.begin(0, 0); // 0 = max sensibilidad, rango minimo.
        if (status == 0) {
            digitalWrite(2, 1); //prende la luz azul por 1 segundo si entra al modo calibracion
            delay(1000);
            digitalWrite(2, 0);
            mpuOk = true; 
            mpu.setFilterGyroCoef(0.90f); // Filtro interno del giroscopio
            delay(2000);
            mpu.calcOffsets(true, true); // Calibra acelerómetro y giroscopio
            //aca el MPU ya se calibro.
            digitalWrite(2, 1);
            delay(300);
            digitalWrite(2, 0);
            delay(300);
            digitalWrite(2, 1);
            delay(300);
            digitalWrite(2, 0);
            delay(300);
            digitalWrite(2, 1);
            delay(300);
            digitalWrite(2, 0);

        } else { //señal S O S 
            digitalWrite(2, HIGH); delay(150);
            digitalWrite(2, LOW);  delay(150);
            digitalWrite(2, HIGH); delay(150);
            digitalWrite(2, LOW);  delay(150);
            digitalWrite(2, HIGH); delay(150);
            digitalWrite(2, LOW);  delay(150);
            
            delay(300); // Pausa entre letras

            // --- 3 LARGOS (O) ---
            digitalWrite(2, HIGH); delay(450);
            digitalWrite(2, LOW);  delay(150);
            digitalWrite(2, HIGH); delay(450);
            digitalWrite(2, LOW);  delay(150);
            digitalWrite(2, HIGH); delay(450);
            digitalWrite(2, LOW);  delay(150);
            
            delay(300); // Pausa entre letras

            // --- 3 CORTOS (S) ---
            digitalWrite(2, HIGH); delay(150);
            digitalWrite(2, LOW);  delay(150);
            digitalWrite(2, HIGH); delay(150);
            digitalWrite(2, LOW);  delay(150);
            digitalWrite(2, HIGH); delay(150);
            digitalWrite(2, LOW);  delay(150);
        }

    } while (status!=0);  //nos aseguramos de que si el MPU se inicialice correctamente (devuelve un 0 cuando lo hace correctamente)
    // Arrancamos en HOME
    moverPlataforma(0, 0, 0, 0, 0, 0);

    rollSuave = mpu.getAngleX();
    pitchSuave = mpu.getAngleY();
}

void loop()
{
    mpu.update(); //actualiza los datos del MPU
   
    rollReal = mpu.getAngleX();
    pitchReal = mpu.getAngleY();
    //Aplicamos filtro para suavizar el movimiento final
    rollSuave = (ALPHA * rollSuave) + ((1.0 - ALPHA) * rollReal);
    pitchSuave = (ALPHA * pitchSuave) + ((1.0 - ALPHA) * pitchReal);
    //rotamos los ejes y compensamos
    rollCompensacion = pitchSuave*kp; 
    pitchCompensacion = -rollSuave*kp;
    // 3. Movemos la plataforma (manteniendo la altura actual)
    moverPlataforma(0, 0, 0, rollCompensacion, pitchCompensacion, 0);
    delay(5);       
}