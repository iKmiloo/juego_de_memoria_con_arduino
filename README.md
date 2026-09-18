# Juego de Memoria con Arduino

Proyecto para hackathon: el circuito físico (Arduino + 4 botones) actúa como
**entrada**, la interfaz web como **salida** (tablero, sonido y ranking) y la
lógica del juego vive en el navegador. El ranking (leaderboard) se guarda en un
archivo JSON local a través de un servidor Node que también hace de puente
**Serial ↔ WebSocket**.

Funciona completamente **sin internet** una vez cargada la página: los sonidos
se generan con la Web Audio API y el frontend no carga ningún recurso externo
(CDN, fuentes remotas, etc.).

## Requisitos

| Requisito | Detalle |
|-----------|---------|
| Node.js | **16 o superior** (`serialport@12` lo exige). Probado con Node 26 |
| npm | Viene incluido con Node (probado con npm 11.19.0); se usa para `npm install` y `node server.js` |
| Arduino IDE | 1.x o 2.x, con la placa *Arduino Uno* instalada |
| Hardware | Arduino Uno (o Nano), 4 pulsadores, protoboard y cables |
| Navegador | Chrome / Edge / Firefox recientes (WebSocket + Web Audio API) |
| Driver USB | En Windows, los clones suelen necesitar el driver **CH340/CH341** |

## Inicio rápido

```bash
# 1. Instalar dependencias (crea node_modules/, no se versiona)
npm install

# 2. Levantar el servidor (serial + WebSocket + API del ranking)
node server.js

# 3. Abrir el juego en el navegador
#    http://localhost:3000
```

Antes de jugar con el circuito hay que **subir el firmware** al Arduino
(ver sección 2). Si no tienes el Arduino a mano, igual puedes jugar con el mouse.

## Estructura

```
juego_de_memoria_con_arduino/
├── simon_input/
│   └── simon_input.ino      → firmware: lee 4 botones y envía "BTN:n"
├── public/
│   ├── index.html           → interfaz completa del juego (HTML + CSS + JS)
│   ├── images/              → fotos del hackathon (fondo del collage)
│       └── logos/           → logos de las universidades del ranking
├── server.js                → puente Serial↔WebSocket + API REST + ranking
├── package.json             → dependencias: express, ws, serialport
├── package-lock.json        → versiones exactas (sí se versiona)
├── node_modules/            → dependencias instaladas; NO se versiona
├── leaderboard.json         → puntajes; se crea/actualiza solo
├── LICENSE                  → MIT
└── README.md
```

> `node_modules/` se genera con `npm install` y está en el `.gitignore`: no viaja
> en el repositorio, así que hay que ejecutar `npm install` una vez en cada
> equipo o clon nuevo.

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

Conecta un cable desde el pin **GND** del Arduino al rail negativo (−) de la
protoboard.

> ℹ️ El color del botón físico es solo una etiqueta. En la interfaz web los pads
> tienen índices `0 = rojo`, `1 = verde`, `2 = amarillo` y `3 = azul`, así que
> para que el color del pulsador coincida con el pad que se ilumina en pantalla
> conecta cada botón al pin de su color según la tabla de abajo.

| Pad en pantalla | Índice (`data-color`) | Pin sugerido |
|-----------------|-----------------------|--------------|
| Rojo | `0` | 2 |
| Verde | `1` | 3 |
| Amarillo | `2` | 5 |
| Azul | `3` | 4 |

## 2. Subir el firmware

1. Abre `simon_input/simon_input.ino` en el **Arduino IDE**.
2. Ve a **Herramientas → Placa → Arduino Uno**.
3. Ve a **Herramientas → Puerto** y selecciona el puerto del Arduino (ej. `COM5` en Windows).
4. Haz clic en **→ Subir** (`Ctrl+U`).
5. No necesitas librerías extra.

### Protocolo Serial (9600 baudios)

| Dirección | Mensaje | Descripción |
|-----------|---------|-------------|
| Arduino → PC | `BTN:0` .. `BTN:3` | Botón presionado (`0` = pin 2, `1` = pin 3, `2` = pin 4, `3` = pin 5) |
| PC → Arduino | `LED:n:ON` / `LED:n:OFF` | Comando de LED que el servidor puede reenviar, pero el **firmware actual no lo lee** (no hay LEDs en el montaje), así que no tiene efecto |

### Verificar funcionamiento

Abre **Herramientas → Monitor Serie** a **9600 baudios** y presiona un botón.
Al abrir el puerto verás el saludo del firmware (`Sistema iniciado` /
`Esperando botones...`) y después deberías ver `BTN:0`, `BTN:1`, etc.

## 3. Instalar dependencias y levantar el servidor

Este proyecto usa **Node.js 16 o superior**. Instala las dependencias una sola vez:
esto crea la carpeta `node_modules/` con `express`, `ws`, `serialport` y sus
dependencias transitivas.

```bash
npm install
```

> ℹ️ `node_modules/` **no se versiona** (está en el `.gitignore`) y `serialport`
> incluye binarios precompilados para Windows/macOS/Linux, así que en principio no
> necesitas compiladores. Hay que ejecutar `npm install` en cada equipo o clon.

Luego levanta el servidor (las dos formas son equivalentes):

```bash
node server.js
# o
node server.js
```

El servidor **detecta el Arduino automáticamente**: busca un puerto cuyo
fabricante diga `Arduino` o cuya ruta contenga `usb`/`acm`. Si todo va bien
verás algo así en la consola:

```
🎮 Servidor corriendo en http://localhost:3000
✅ Conectado al Arduino en COM5
```

Si aparece el warning `⚠️ No se detectó Arduino automáticamente`, la consola
lista los puertos disponibles (ej. `COM5`); copia el tuyo y ponlo en
`SERIAL_PORT_PATH` dentro de `server.js` (por defecto `'AUTO'`).

Abre **http://localhost:3000** en el navegador.

> ⚠️ Cuando el servidor esté corriendo ocupa el puerto serial del Arduino.
> Para subir un nuevo sketch debes detener el servidor primero (`Ctrl+C`) y
> cerrar el Monitor Serie del Arduino IDE.

### Constantes configurables en `server.js`

| Constante | Valor por defecto | Para qué sirve |
|-----------|-------------------|----------------|
| `SERIAL_PORT_PATH` | `'AUTO'` | Puerto del Arduino (ej. `'COM5'`, `'/dev/ttyUSB0'`, `'/dev/ttyACM0'`) |
| `SERIAL_BAUD_RATE` | `9600` | Debe coincidir con el `Serial.begin()` del firmware |
| `HTTP_PORT` | `3000` | Puerto del servidor web y del WebSocket |

## 4. Jugar

1. Escribe tu **nombre**, **apellido** y selecciona tu **universidad** en el
   panel derecho (los tres son obligatorios: el botón **INICIAR** permanece
   deshabilitado hasta completarlos).
2. Haz clic en **INICIAR** para comenzar.
3. El juego muestra una secuencia de colores con sonido — memorízala.
4. Repite la secuencia presionando los **botones físicos** o haciendo **clic**
   en los colores del tablero en pantalla.
5. Al fallar, tu puntaje (nivel alcanzado) se guarda automáticamente en el
   leaderboard.

Nombre, apellido y universidad quedan guardados en el `localStorage` del
navegador (`simon_name`, `simon_lastName`, `simon_university`), así no hay que
volver a escribirlos en la siguiente partida.

## 5. Leaderboard

- Los puntajes se ordenan de mayor a menor nivel y, a igual nivel, primero quien
  lo consiguió antes.
- Muestra **10 registros por página** con botones **← Anterior** y **Siguiente →**
  (el paginador solo aparece si hay más de una página).
- Cada fila muestra puesto, logo de la universidad, nombre completo, universidad
  abreviada y nivel.
- Pasa el mouse sobre una fila y haz clic en **×** para borrar ese registro (pide
  confirmación; no se puede deshacer).
- Los datos se guardan en `leaderboard.json` (archivo plano, sin base de datos).
  Si el archivo no existe o está corrupto, el servidor lo trata como lista vacía
  y lo crea al guardar el primer puntaje.

Universidades disponibles en el formulario:

| Universidad | Sigla | Logo |
|-------------|-------|------|
| Universidad Católica Luis Amigó | FUNLAM | `public/images/logos/logo_funlam.png` |
| Universidad Cooperativa de Colombia | UCC | `public/images/logos/logo_ucc.svg` |
| Universidad de Antioquia | UdeA | `public/images/logos/logo_udea.svg` |
| Politécnico Colombiano Jaime Isaza Cadavid | Politecnico | `public/images/logos/logo_politecnico.png` |
| Corporación Universitaria Remington | Uniremington | `public/images/logos/logo_uniremington.png` |
| Universidad Nacional Abierta y a Distancia | UNAD | `public/images/logos/logo_unad.svg` |

Si un logo no se encuentra, la tabla muestra las iniciales de la universidad
sobre un fondo de color (mismo comportamiento para universidades nuevas).

## 6. API y protocolos

### API REST (`http://localhost:3000`)

| Método | Ruta | Cuerpo | Respuesta |
|--------|------|--------|-----------|
| `GET` | `/api/leaderboard` | — | Lista ordenada por nivel desc: `[{ id, name, lastName, university, level, created_at }]` |
| `POST` | `/api/score` | `{ name, lastName, university, level }` | `{ ok: true, score: {...} }`. `400` si falta `name` o `level` no es número; `500` si no se pudo escribir el archivo |
| `DELETE` | `/api/score/:id` | — | `{ ok: true }`. `404` si ese `id` no existe |

- `name` y `lastName` se recortan a 30 caracteres y `university` a 80; un `name`
  vacío se guarda como `Anónimo`.
- El frontend envía el puntaje **solo cuando el jugador falla** (no al cerrar la
  pestaña).
- Los registros guardados sin `id` (versión anterior del ranking) se migran solos:
  el servidor les asigna uno la primera vez que lee el archivo.

### WebSocket (`ws://localhost:3000`)

| Dirección | Mensaje (JSON) | Descripción |
|-----------|----------------|-------------|
| Servidor → navegador | `{ "type": "status", "connected": true }` | Estado de la conexión con el Arduino (también se envía al conectar) |
| Servidor → navegador | `{ "type": "button", "index": 0 }` | Botón físico presionado (llegó como `BTN:0` por serial) |
| Navegador → servidor | `{ "type": "led", "index": 2, "on": true }` | Pide encender/apagar un LED; el servidor lo traduce a `LED:2:ON` |

### Puente serial

`server.js` abre el puerto del Arduino, escucha las líneas `BTN:n` y las reenvía
a **todos** los navegadores conectados. Si el Arduino se desconecta, el servidor
sigue funcionando y avisa por WebSocket (`status.connected = false`).

## 7. Personalización rápida

| Quiero cambiar... | Dónde |
|-------------------|-------|
| Puerto del Arduino o velocidad | `SERIAL_PORT_PATH` y `SERIAL_BAUD_RATE` en `server.js` |
| Puerto del servidor web | `HTTP_PORT` en `server.js` |
| Colores del tema (estilo PCB) y de los pads | variables CSS en `:root` dentro de `public/index.html` |
| Tono de cada color | arreglo `TONE_FREQUENCIES` en `public/index.html` (rojo, verde, amarillo, azul) |
| Lista de universidades y sus logos | `<select id="universitySelect">` y objeto `UNIVERSITIES` en `public/index.html` |
| Registros por página del ranking | constante `PAGE_SIZE` en `public/index.html` |
| Duración de la iluminación de un pad | función `litPad(index, duration)` en `public/index.html` |

## Solución de problemas

| Síntoma | Causa probable / solución |
|---------|---------------------------|
| `No se pudo abrir el puerto serial` o `Access denied` | Otro programa lo tiene abierto: cierra el Monitor Serie del IDE u otra instancia de `server.js` |
| `No se detectó Arduino automáticamente` | Copia el puerto que lista la consola y escríbelo en `SERIAL_PORT_PATH` |
| No aparece ningún puerto en la lista | Falta el driver USB (CH340/CH341 en clones) o el cable es solo de carga |
| `Cannot find module 'express'` / `'ws'` / `'serialport'` | Faltan las dependencias: ejecuta `npm install` en la carpeta del proyecto |
| `EADDRINUSE: address already in use :::3000` | Ya hay otro servidor usando el puerto 3000: ciérralo o cambia `HTTP_PORT` |
| Subir el sketch falla con "puerto en uso" | Detén el servidor con `Ctrl+C` y cierra el Monitor Serie |
| Los botones no reaccionan en la web | Verifica el Monitor Serie a 9600 baudios y que un terminal de cada botón vaya a **GND** |
| El estado dice "sin arduino — el mouse hace de circuito" | Es normal sin hardware: el juego sigue siendo jugable con el mouse |
| El ranking no carga | El servidor debe estar corriendo en el mismo host/puerto desde el que abriste la página (`http://localhost:3000`) |
| En Linux/Mac: `permission denied` al abrir el puerto | Agrega tu usuario al grupo `dialout` (Linux) y vuelve a iniciar sesión |
| Quiero empezar el ranking desde cero | Deja `leaderboard.json` con `[]` |

## Notas para la demo

- Si el Arduino **no está conectado**, el juego sigue siendo jugable con el mouse
  — útil para probar la lógica sin hardware.
- El leaderboard se crea automáticamente al registrar el primer puntaje.
- **No depende de internet**: los sonidos se generan con Web Audio API y la página
  no carga recursos externos.
- Como el servidor ocupa el puerto serial, deja el sketch ya cargado antes de
  empezar la demo.
- En un equipo recién clonado el orden es: `npm install` → subir el firmware →
  `node server.js`. Recuerda que `node_modules/` no viaja en el repositorio.

## Licencia

MIT © 2026 Camilo Sanchez Vasquez — ver [LICENSE](LICENSE).

