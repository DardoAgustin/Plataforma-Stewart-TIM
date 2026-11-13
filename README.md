# Plataforma-Stewart-TIM
# Plataforma Stewart autonivelada

Plataforma Stewart de 6 grados de libertad (6-DOF) con capacidad de autoestabilización, basada en un ESP32, un sensor inercial MPU6050 y seis servomotores MG90S. Proyecto de la cátedra Taller Inicial de Mecatrónica, Facultad de Ingeniería, Universidad Nacional de Cuyo (UNCuyo).

## Descripción

El sistema mide la inclinación de la base (pitch y roll) con el MPU6050, aplica un filtro pasabajos a las lecturas y calcula mediante cinemática inversa la posición de cada servomotor para mantener la plataforma superior paralela a la horizontal.

## Características

- Plataforma Stewart de 6 servomotores MG90S controlados por PWM directamente desde los GPIO del ESP32.
- Estabilización de inclinación (pitch y roll) con filtro pasabajos y constante de ganancia proporcional (`Kp`).
- Límites mecánicos de seguridad ajustables por software.


## Hardware

- 1 x ESP32
- 1 x sensor MPU6050 (acelerómetro y giroscopio)
- 6 x servomotores MG90S

## Software

- Arduino IDE con el núcleo `arduino-esp32` de Espressif.
- Bibliotecas:
  - `ESP32Servo.h`
  - `Wire.h`
  - `MPU6050_light.h`



## Créditos

La cinemática inversa se basa en el repositorio [NicHub/stewart-platform-esp32](https://github.com/NicHub/stewart-platform-esp32), adaptado para controlar los servomotores directamente desde los GPIO del ESP32 (sin controlador PWM externo I2C) y migrado de PlatformIO al Arduino IDE.


## Autores

- Sosa, Dardo Agustín
- Vale, Brandon
- Gomez, Tomás

Taller Inicial de Mecatrónica, Primer Año, Ingeniería Mecatrónica, Facultad de Ingeniería, UNCuyo. Docente: Msc. Ing. Mauricio Cáceres.

## Licencia

Bajo GPLv3.
