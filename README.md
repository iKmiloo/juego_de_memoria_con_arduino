# Juego de Memoria con Arduino

Proyecto para hackathon: circuito físico (Arduino + botones) como
**entrada**, interfaz web como **salida** y lógica del juego. Leaderboard
paginado guardado en un archivo JSON local. Funciona completamente **sin internet**
una vez cargada la página (sonidos y fuentes están auto-hospedados).

## Estructura

```
juego_de_memoria_con_arduino/
├── simon_input/
│   └── simon_input.ino      → firmware, sube esto al Arduino
├── public/
│   ├── index.html           → interfaz del juego (frontend)
│   └── images/              → imágenes de fondo y logos de universidades
├── server.js                → puente Serial↔WebSocket + API REST + leaderboard
├── package.json
└── leaderboard.json         → se crea solo al guardar el primer puntaje
```

## 1. Armar el circuito

Componentes: **Arduino Uno**, 4 pulsadores, protoboard y cables.
> ⚠️ El montaje actual **no usa LEDs**. El feedback visual se hace
> completamente en la pantalla del navegador.

### Conexiones de botones

Los botones usan `INPUT_PULLUP` — **no necesitas resistencias**.

| Botón | Color | Pin Arduino | Otro terminal |
|-------|-------|-------------|---------------|
| Botón 1 | Rojo | **2** | GND |
| Botón 2 | Verde | **3** | GND |
| Botón 3 | Azul | **4** | GND |
| Botón 4 | Amarillo | **5** | GND |

Conecta un cable desde el pin **GND** del Arduino al rail negativo (−) de la protoboard.

## 2. Subir el firmware

1. Abre `simon_input/simon_input.ino` en el **Arduino IDE**.
2. Ve a **Herramientas → Placa → Arduino Uno**.
3. Ve a **Herramientas → Puerto** y selecciona el puerto del Arduino (ej. `COM5` en Windows).
4. Haz clic en **→ Subir** (`Ctrl+U`).
5. No necesitas librerías extra.

### Protocolo Serial (9600 baudios)

| Dirección | Mensaje | Descripción |
|-----------|---------|-------------|
| Arduino → PC | `BTN:0` .. `BTN:3` | Botón presionado |

### Verificar funcionamiento

Abre **Herramientas → Monitor Serie** a **9600 baudios** y presiona un botón.
Deberías ver `BTN:0`, `BTN:1`, etc.

## 3. Instalar dependencias y levantar el servidor

Este proyecto usa **Node.js**. Instala las dependencias una sola vez:

```bash
npm install
```

Luego levanta el servidor:

```bash
node server.js
```

El servidor **detecta el Arduino automáticamente**. Si ves un warning de
"No se detectó Arduino", copia el puerto listado en la consola (ej. `COM5`)
y ponlo en `SERIAL_PORT_PATH` dentro de `server.js`.

Abre **http://localhost:3000** en el navegador.

> ⚠️ Cuando el servidor esté corriendo ocupa el puerto serial del Arduino.
> Para subir un nuevo sketch debes detener el servidor primero (`Ctrl+C`).

## 4. Jugar

- Escribe tu **nombre**, **apellido** y selecciona tu **universidad** antes de iniciar.
- Haz clic en **INICIAR** para comenzar.
- El juego muestra una secuencia de colores con sonido — memorízala.
- Repite la secuencia presionando los **botones físicos** o haciendo **clic**
  en los colores del tablero en pantalla.
- Al fallar, tu puntaje (nivel alcanzado) se guarda automáticamente en el leaderboard.

## 5. Leaderboard

- Los puntajes se ordenan de mayor a menor nivel.
- Muestra **10 registros por página** con botones **← Anterior** y **Siguiente →**.
- Pasa el mouse sobre una fila y haz clic en **×** para borrar ese registro (pide confirmación).
- Los datos se guardan en `leaderboard.json` (archivo plano, sin base de datos).

## Notas para la demo

- Si el Arduino **no está conectado**, el juego sigue siendo jugable con el mouse
  — útil para probar la lógica sin hardware.
- El leaderboard se crea automáticamente al registrar el primer puntaje.
- **No depende de internet**: los sonidos se generan con Web Audio API y las
  fuentes están auto-hospedadas en `public/`.

