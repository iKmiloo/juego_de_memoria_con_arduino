/*
  Juego de Memoria con Arduino - Firmware de entrada (Arduino Uno/Nano)
  ----------------------------------------------------
  Rol: dispositivo de ENTRADA. Lee 4 botones y envía eventos por
  Serial. La lógica del juego vive en la web (Node + navegador).

  Conexiones:
    Botones (INPUT_PULLUP, activos en LOW):
      Boton 1 (Rojo)     -> pin 2  -> envia BTN:0
      Boton 2 (Verde)    -> pin 3  -> envia BTN:1
      Boton 3 (Azul)     -> pin 4  -> envia BTN:2
      Boton 4 (Amarillo) -> pin 5  -> envia BTN:3
      (el otro terminal de cada botón va a GND)

  Protocolo Serial (9600 baudios):
    Arduino -> PC:  "BTN:0" .. "BTN:3"   (botón presionado)
*/

const int boton1 = 2;
const int boton2 = 3;
const int boton3 = 4;
const int boton4 = 5;

bool estadoAnterior1 = HIGH;
bool estadoAnterior2 = HIGH;
bool estadoAnterior3 = HIGH;
bool estadoAnterior4 = HIGH;

void setup() {
  Serial.begin(9600);

  pinMode(boton1, INPUT_PULLUP);
  pinMode(boton2, INPUT_PULLUP);
  pinMode(boton3, INPUT_PULLUP);
  pinMode(boton4, INPUT_PULLUP);

  Serial.println("Sistema iniciado");
  Serial.println("Esperando botones...");
}

void loop() {

  bool estado1 = digitalRead(boton1);
  bool estado2 = digitalRead(boton2);
  bool estado3 = digitalRead(boton3);
  bool estado4 = digitalRead(boton4);

  // Boton 1 (pin 2) -> BTN:0
  if (estado1 != estadoAnterior1) {
    if (estado1 == LOW) {
      Serial.println("BTN:0");
    }
    estadoAnterior1 = estado1;
  }

  // Boton 2 (pin 3) -> BTN:1
  if (estado2 != estadoAnterior2) {
    if (estado2 == LOW) {
      Serial.println("BTN:1");
    }
    estadoAnterior2 = estado2;
  }

  // Boton 3 (pin 4) -> BTN:2
  if (estado3 != estadoAnterior3) {
    if (estado3 == LOW) {
      Serial.println("BTN:2");
    }
    estadoAnterior3 = estado3;
  }

  // Boton 4 (pin 5) -> BTN:3
  if (estado4 != estadoAnterior4) {
    if (estado4 == LOW) {
      Serial.println("BTN:3");
    }
    estadoAnterior4 = estado4;
  }

  delay(20);
}