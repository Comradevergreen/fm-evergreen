# JIANT FM — Manual de usuario

**Firmware v0.6.30 para el M-VAVE FM-1**

> *English version: [MANUAL.md](MANUAL.md).*

Este manual es para músicos que acaban de instalar JIANT FM y quieren saber cómo tocarlo, cómo armar temas con él
y, sobre todo, **cómo guardar su trabajo sin perder nada**. Sigue el firmware tal como está en la v0.6.30: cada
botón, perilla y mensaje aparece nombrado exactamente como lo muestra el equipo.

Si vas a leer un solo capítulo, leé [12. Guardar y cargar](#12-guardar-y-cargar).

## Contenido

0. [Inicio rápido](#0-inicio-rápido)
1. [El panel](#1-el-panel)
2. [La pantalla](#2-la-pantalla)
3. [Páginas y capas](#3-páginas-y-capas)
4. [Motores](#4-motores)
5. [DRUM-X](#5-drum-x)
6. [El secuenciador](#6-el-secuenciador)
7. [ARRANGE: canciones, secciones y escenas](#7-arrange-canciones-secciones-y-escenas)
8. [Dados y UNDO](#8-dados-y-undo)
9. [Efectos y master](#9-efectos-y-master)
10. [Modulación y macros](#10-modulación-y-macros)
11. [Tocar en vivo](#11-tocar-en-vivo)
12. [Guardar y cargar](#12-guardar-y-cargar)
13. [Sistema: TRACKS, LOAD y el MENU](#13-sistema-tracks-load-y-el-menu)
14. [MIDI y USB](#14-midi-y-usb)
15. [Herramientas web: emulador, instalador, editor, backup](#15-herramientas-web-emulador-instalador-editor-backup)
16. [Problemas y preguntas frecuentes](#16-problemas-y-preguntas-frecuentes)

---

## 0. Inicio rápido

### Instalación

1. Abrí el **instalador web** en Chrome o Edge: <https://juanjiant-bit.github.io/JIANT/webapp/installer/>
2. Conectá el FM-1 **directamente** a la computadora con un cable USB de datos.
3. **¿Venís de otro firmware custom (Felucca, baudgirl o cualquier otro)?** Volvé primero al firmware oficial V15
   con el botón **Return to official V15 first** del instalador (que además guarda un backup completo). Escribir
   JIANT directamente encima de otro firmware custom ya dejó algún FM-1 sin poder arrancar.
4. Apretá **Install**. Primero revisa el firmware del FM-1 y solo escribe sobre el oficial V15 o JIANT (si encuentra otro,
te manda a volver al V15 oficial primero). No desenchufes el cable mientras escribe.

JIANT no tiene samples de usuario: si venís de Felucca, sus samples de usuario (USR1–3) se pierden al instalar.
Guardá tus archivos WAV.

### Tu primer sonido (1 minuto)

1. Encendé el equipo. La firma se dibuja sola y después llegás a **HOME**: seis criaturas, una por pista.
2. Girá **ALGORITHM** para elegir una pista (T1–T6). Con el valor por defecto, **TRACKS 4**, se detiene en T4: T5 y T6 aparecen solo con TRACKS 6 (ver
   [13](#13-sistema-tracks-load-y-el-menu)).
3. Girá **PRESETS** para recorrer sonidos. Tocá las teclas.
4. Girá **KNOB 1–4**: en HOME son los cuatro controles más útiles del motor de la pista.
5. ¿No te gusta? Mantené **EDIT** y girá **SELECT**: el sonido se tira a los dados y sale una variación nueva, que escuchás al
   instante. **OCT+ OK** la deja, **OCT− CANCEL** devuelve el sonido.

### Tu primer tema (5 minutos)

1. En **HOME**, girá **SELECT** dos pasos a la derecha (ECO → BEING → **GEN**). Se abre el **GENERATOR**: elegí un
   **GENRE** con KNOB 1 o PRESETS (o dejá RANDOM) y apretá **OCT+**. Aparece una idea de tema completa: motores,
   sonidos, partes, escala, tempo y efectos. Apretá **PLAY**.
2. ¿No te convence? Apretá **OCT+** otra vez: semilla nueva, pieza nueva. ¿Demasiado cargada? Tocá **HOME** para la
   página 2 y bajá **DRUMS** o **ARP**, después OCT+: la misma pieza con eso cambiado. **OCT−** cierra la pantalla
   (SELECT a la izquierda: vuelve a BEING). Para volver a lo que había antes: mantené **GLO** y apretá **G4** (UNDO).
3. ¿Te gusta? **Pará** (PLAY), apretá **SAVE** (abre SONIDO, los sonidos de usuario) y girá **SELECT** un paso a la
   izquierda: **PROJECT**, las cuatro secciones del proyecto. Girá **KNOB 4** a la derecha para que su tarjeta diga
   **OCT+** y apretá **OCT+**. Escribí un nombre con las teclas (o dejá el que te ofrece) y apretá **OCT+** otra vez. La
   pantalla dice **SAVED**.
4. Hacé una variación (cambiá algunos pasos, muteá una pista) y guardala en la sección **B**: en PROJECT girá
   **KNOB 1** hasta **B** y después KNOB 4 y OCT+ como antes.
5. Apretá **SAVE** para pasar de PROJECT a **ARRANGE** (las filas del tema). Apretá **OCT+** para agregar una fila,
   elegí su sección con **KNOB 2** y su largo con **KNOB 3** (compases). Agregá una fila para A y otra para B. Apretá
   **PLAY**: el tema reproduce las filas en orden.

Ese es todo el ciclo: **hacer música → guardarla en una sección → encadenar secciones en ARRANGE**.

---

## 1. El panel

Los nombres de abajo son los que usa el firmware (la pantalla HARDWARE CALIBRATION los pide con estos nombres).

### Encoders y perillas

| Control | Qué hace |
| --- | --- |
| **SELECT** (encoder) | Solo: **recorre las páginas de la sección en la que estás** (las de EDIT en una página EDIT, las de FX en una de FX …), una por paso, sin dar la vuelta; en una página **SEQ** en cambio hace **zoom**: PATRÓN ↔ COMPÁS ↔ PASO (la lente del secuenciador, LENTE: ver [6](#cómo-moverse-lente)); en **HOME** hace zoom **ECO ↔ BEING ↔ GEN** (GEN: el generador de temas, como HOME + SELECT; ver [LENTE en todo](#lente-en-todo-085)). Con un botón mantenido, los **dados** de ese botón: **HOME** + SELECT **GEN**, **EDIT** + SELECT el sonido, **SEQ** + SELECT la secuencia, **LFO** + SELECT el LFO, **FX** + SELECT los efectos. **GLO** + SELECT: el tempo. Cualquier otro botón de página + SELECT: sus páginas. |
| **ALGORITHM** (encoder) | Elige la pista desde cualquier página: T1–T4, o T1–T6 con TRACKS 6. En el MENU: cambia de pestaña. |
| **PRESETS** (encoder) | En HOME, PRESETS y las páginas de sonido: recorre sonidos (los presets de fábrica de cada motor y después tus presets de usuario). En las páginas SEQ sigue al zoom: en **PATRÓN** (PATTERN, STRETCH, PLAYHEADS, TRACK, MUTATE) elige el **compás**; en **COMPÁS** y **PASO** (STEP, NOTE, COMP, COMP 2, AUTOMATION) mueve el **cursor de paso**, y al pasarse del compás entra solo en el siguiente. En las páginas de lista (USER, PROJECT, ARRANGE, TOOLS, AUTO LIST): mueve la selección. **Mantené un botón de página y girá SELECT** (o PRESETS) para saltar directo por las páginas de ese botón. |
| **KNOB 1–4** | Las cuatro tarjetas (columnas) de la pantalla. Con una capa mantenida, las perillas propias de la capa. Girar rápido recorre todo el rango, despacio ajusta fino (MENU > KNOB ACCEL lo desactiva). |
| **MASTER** | El volumen de salida. |

### Botones

| Botón | Toque | Mantener |
| --- | --- | --- |
| **HOME** | Vuelve a HOME (el ecosistema). | Unos 0,7 s: abre el **MENU**. |
| **ENV** | La lente de ENV (0.8.5): desde otra página, la última página ENV (al encender, ENV); en ENV el otro juego del nivel (ENV ↔ DEST, ENV ↔ ENV DEST). | (sin capa) |
| **LFO** | La lente LFO · MOD (0.8.5): desde otra página, la última página mostrada (al encender, LFO); en ella el juego siguiente del nivel (LFO ↔ LFO DEST, MSEQ ↔ MSEQ 2). | Capa **MACRO**: KNOB 1–4 son M1–M4. |
| **FX** | La lente de FX (0.8.5): desde otra página la última página FX; en FX lo siguiente del nivel (TRACK: su juego; BUSES: abre el módulo; EFFECT: su página siguiente). | Capa de **FX punch-in**. |
| **SCL** | La lente SCL · ARP (0.8.5): **KEY ↔ CHORD** (desde otro lado, la última mostrada). | Capa **SCL**: las teclas fijan la tónica. |
| **EDIT** | La lente de EDIT (0.8.5): desde otra página, la última página EDIT; en EDIT el juego siguiente del nivel (SOUND: EDIT 1 ↔ EDIT 2; TONE: FILTER → MORPH → … → VOICE 2; el detalle: OPERATOR → OP ENV → OPERATOR 2, SOUND 1–4). En STEP: borra el paso; en USER / PROJECT: renombrar. | Capa **EDIT**: INIT, RECALL, mutes de los sonidos de DRUM-X. |
| **GLO** | La lente GLO (0.8.5): MIXER → SENDS → GLOBAL → SYSTEM → MIXER (desde otro lado, MIXER). | Capa **GLO**: mutes, solos, niveles, tap tempo, UNDO. |
| **SAVE** | La lente de SAVE (0.8.5): desde otra página SONIDO (USER, SAVE listo); en SAVE el otro juego del nivel. | Capa **SAVE**: tocar/guardar secciones en vivo. |
| **ARP** | El nivel **ARP** de la lente SCL · ARP; en él ARP ↔ ARP 2. | (sin capa) |
| **SEQ** | Páginas SEQ: en una página SEQ, el siguiente juego de perillas del nivel de zoom en el que estás; desde otro lado, vuelve al nivel y al juego que usaste por última vez en esa pista. | Capa **SEQ TOOLS** (en todas las páginas SEQ). |
| **PLAY** | Arranca / para. | — |
| **REC** | Arma / desarma la grabación en la pista seleccionada. | Capa **REC**: CLEAR, CLICK. |
| **OCT− / OCT+** | Octava del teclado (−3 a +3; los dos juntos: vuelve a 0). En páginas de acción y en diálogos: **OCT−** = atrás / NO, **OCT+** = hacerlo / YES. | — |

### Las teclas

27 teclas de **F3 a G5** (16 blancas, 11 negras). Tocan la pista seleccionada. Con una capa mantenida se convierten en
atajos de esa capa; en la grilla DRUM son pasos y carriles; en la pantalla NAME escriben.

### Combinaciones útiles

| Combinación | Resultado |
| --- | --- |
| **GLO** mantenido + **PLAY** | Reinicia desde el principio sin parar (clock interno). |
| **GLO** mantenido + **SELECT** | Tempo, desde cualquier página. |
| **GLO** mantenido + **G4** | **UNDO** del último cambio grande (otra vez: rehacer). |
| **SEQ** mantenido + **REC** | **CLEAR ALL SEQUENCES?** — los pasos y la automatización de las seis pistas, y el carril de punch-in. |
| **REC** mantenido + **FX / EDIT / ENV / LFO / SCL / ARP / GLO** | Borra la automatización de esa parte en la pista seleccionada (GLO: la automatización de nivel y paneo de todas las pistas; FX: también el carril de punch-in). Sin diálogo. |
| **SAVE** mantenido + **OCT+** / **OCT−** | Guarda la música en la sección que está sonando / recupera esa sección tal como quedó guardada. |
| **HOME / EDIT / SEQ / LFO / FX** mantenido + **SELECT** | Los dados: todo / el sonido / la secuencia / el LFO / los efectos (ver [8](#8-dados-y-undo)). |
| **ENV / SCL / ARP / SAVE** mantenido + **SELECT**, cualquier botón de página + **PRESETS** | Salta por las páginas de ese botón. |
| Un paso mantenido (SEQ > STEP) + **KNOB 1–4** | Un parameter lock en ese paso. |
| **OCT− + OCT+** mantenidos al encender | HARDWARE CALIBRATION. |
| **OCT− + OCT+** mantenidos 5 s con el equipo funcionando | Modo de actualización (UBOOT), con una cuenta regresiva desde los 2 s; soltá para cancelar. |

---

## 2. La pantalla

Desde la 0.8.5 todas las páginas llevan la piel del diseño que aprobó el dueño (la maqueta HTML "LENTE"):

- **Encabezado (arriba)**: una sola línea tranquila sobre una regla fina. A la izquierda el estado de reproducción (un
  cuadrado gris parado, un triángulo naranja sonando, violeta durante la cuenta) y los **BPM** (violeta; naranja
  mientras cambian), después el candado de BPM LOCK y el punto de REC (lleno: la pista seleccionada armada; un aro:
  otra pista). Al medio de qué es la página: **T1 · ANALOG ACID** (la pista, su motor y su sonido); en las páginas del
  tema (SAVE, ARRANGE, TOOLS, GLOBAL) **P1 NOMBRE** (el proyecto); en FX **T1** en naranja. A la derecha la octava
  (**OCT +2**, o la fila del tema mientras suena un tema), el **medidor de carga** (cinco barritas, violetas a medida
  que el sonido ocupa más del tiempo de audio) y la batería (violeta cuando la alimenta el USB). Los mensajes aparecen
  al medio ("SAVED", "T1 CLEARED"...), a veces con una segunda línea.
- **Tarjetas**: las cuatro cajas debajo del encabezado son KNOB 1–4: el nombre chico y gris arriba, el valor debajo en
  blanco y una barra violeta fina debajo del valor. La que estás girando se enciende en naranja, así es el único color
  en las tarjetas. Un valor automatizado o con lock tiene el nombre en violeta; una tarjeta apagada significa "acá
  ahora no hay nada". Una acción armada (SAVE, LOAD, ERASE, CLEAR...) es una tarjeta teñida de violeta, con el nombre
  violeta y **OCT+** como valor. Desde la 0.8.5 las tarjetas no llevan íconos, ni nada más (las tarjetas de MENU > LARGE:
  un punto violeta para un valor que se mueve solo; el encabezado: "!" antes de un mensaje NO FILE, un candado y un
  disco dibujados).
- **Nombre de página**: al cambiar de página (o de pista) a mano, el nombre de la página aparece un momento sobre el pie
  de la pantalla, con la pista y su motor a la derecha; en HOME el motor de la pista, y "T1 HOME" en chico (en una
  página de lente se enciende su miga de pan). La barra de arriba, las tarjetas y el gráfico quedan a la vista.
- **Gránulos**: unos puntitos flotan detrás del gráfico del panel, como en HOME (LOAD FULL con ANIM ON). Se mueven solo cuando
  el panel se redibuja igual (una perilla, el LFO, la envolvente), así que no gastan CPU.
- **Panel (al medio)**: el gráfico de la página — la criatura del motor, una envolvente, el LFO en movimiento, la
  grilla de pasos, una lista...
- **Pie**: las barras de pasos del compás que editás (de a cuatro debajo de cada tarjeta) o ayudas de teclas, y debajo
  una **miga de pan** chica y gris: el botón y la página (p. ej. **ARP · ARP 2/2**, **ENV · ENV DEST 2/2**).
- **Las páginas de lente** (el secuenciador desde la 0.8.3, SAVE y FX desde la 0.8.5) van armadas como la maqueta: debajo del
  encabezado tres **pestañas de nivel** (la del nivel en el que estás rellena de violeta), las tarjetas debajo, un
  panel más alto, y el pie es solo la miga de pan, **LENTE · BAR · STEP** (la lente, el nivel, el juego).

- **Las letras chicas** (desde la 0.8.5): los nombres de las tarjetas, las pestañas de nivel, la miga de pan, las
  palabras del medio del encabezado y los rótulos y fichas chicas de los paneles van en una letra de píxeles de 5 x 7,
  nítida, las mayúsculas mono chicas de la maqueta; los valores siguen en la letra redondeada más grande.

Los **colores** funcionan como una cámara térmica: frío (cian, azul) es poco, caliente (naranja, amarillo, blanco) es
mucho. Violeta es lo seleccionado, naranja lo activo.

**HOME** es el ecosistema: una criatura por pista, su especie según su motor, su forma según sus cuatro perillas, que
se hincha y se calienta con su propio audio (0.8.5: en una grilla de seis celdas, como la maqueta, con bichitos nadando
entre ellas; ver [LENTE en todo](#lente-en-todo-085)). La pista seleccionada se dibuja más grande, una muteada se oscurece, una
pista dormida (TRACKS 4) se dibuja como muteada. En el **MIXER**, las pistas dormidas aparecen en gris y dicen OFF.

Los **diálogos** preguntan antes de cualquier cosa destructiva: **OCT− NO**, **OCT+ YES**. (0.8.5, como la maqueta)
Llevan las pestañas **NOTICE · DICE · NAME**: una pregunta es una tarjeta bajo una regla violeta con sus dos botones
(YES en el acento); el dado de una parte (EDIT, SEQ, LFO, FX + SELECT) muestra **BEFORE** y **NOW** lado a lado (el
ser del sonido antes y ahora, los pasos, los valores del LFO o de los efectos) sobre una franja **KEEP DICED …? ·
SELECT: DICE AGAIN · OCT− CANCEL · OCT+ OK**; un mensaje en una página con lente aparece también en su panel, como
aviso (el encabezado también lo dice; no sobre los seres de HOME); NAME tiene sus tarjetas CURSOR, CHAR, LEN y OCT+
SAVE / WRITE sobre el nombre, las letras de las teclas y las funciones de las teclas negras.

---

## 3. Páginas y capas

### Cómo funciona la navegación

- **Tocá** un botón de página: sus páginas. Tocalo de nuevo: la siguiente página de esa familia. Desde otra familia
  volvés a la página donde estuviste por última vez.
- **HOME** vuelve al ecosistema; en HOME un toque cambia los dos juegos de su nivel (las perillas de HOME / las macros).
- **FX es una lente** (0.8.5; hasta la 0.8.4 secciones y dos toques rápidos): SELECT hace zoom **TRACK ↔ BUSES ↔
  EFFECT** (pista, buses, efecto), un toque de FX va a lo siguiente del nivel; PRESETS elige el módulo en BUSES y
  EFFECT. Ver [9](#la-lente-de-fx-track--buses--effect).
- **Girá SELECT** para recorrer las páginas de la sección en la que estás (las de EDIT en una página EDIT), una por paso, frenando en la primera y la última; el nombre de la página aparece un momento sobre el pie.
  En HOME hace zoom ECO ↔ BEING ↔ GEN (GEN: el generador de temas).
- **SEQ es una lente** (LENTE, 0.8.3): SELECT hace zoom entre el **PATRÓN** entero, un **COMPÁS** y un **PASO**, y un
  toque de SEQ recorre los juegos de perillas del nivel en el que estás. Ver [6](#cómo-moverse-lente).
- **SAVE también es una lente** (0.8.5): SELECT hace zoom **LIBRARY ↔ PROJECT ↔ SOUND** (biblioteca, proyecto,
  sonido) y un toque de SAVE cambia entre los dos juegos del nivel. Ver [12](#la-lente-de-save-library--project--sound).
- **Mantené un botón de página y girá PRESETS** para recorrer sus páginas hacia adelante o hacia atrás (sin dar la
  vuelta); soltá y te quedás ahí. ENV, SCL, ARP y SAVE hacen lo mismo con SELECT; HOME, EDIT, SEQ, LFO y FX + SELECT son
  sus dados, GLO + SELECT el tempo.
- **Mantené** un botón de página (más allá del tiempo HOLD, MENU > HOLD, 0,4 s por defecto) para abrir su **capa**:
  las teclas y perillas cambian de función mientras lo mantenés, y un mapa muestra qué hace cada tecla. Apretar una
  tecla o girar una perilla la abre al instante. Soltá y volvés a donde estabas.
- Algunas páginas solo aparecen cuando corresponden: las páginas de operadores de FM6 en una pista FM6, las de DRUM-X
  en una pista DRUM, RETRIG en LOFI BYTE/FLOAT, y con **LOAD ECO** se ocultan las páginas SLICER y CHORUS.

### LENTE en todo (0.8.5)

Todo el instrumento funciona como la lente del secuenciador (el diseño que aprobó el dueño, la maqueta HTML "LENTE"):
**SELECT hace zoom entre tres niveles** (sin dar la vuelta), un nivel tiene uno o más **juegos** de perillas y el
**toque de su botón los recorre**, las **pestañas de nivel** van debajo del encabezado (la del nivel en el que estás
rellena de violeta), y el pie dice **LENTE · NIVEL · JUEGO** con el lugar del juego a la derecha (**1/2**). Cada parte
recuerda su nivel y el juego de cada nivel; su botón desde otro lado te devuelve ahí.

| Parte | Niveles (SELECT) | Juegos (toque) | PRESETS |
| --- | --- | --- | --- |
| **HOME** | **ECO** (los seis seres) · **BEING** (el ser de la pista, sus perillas como órganos) · **GEN** (el generador) | ECO: HOME / MACROS · BEING: HOME / ROUTES · GEN: GEN 1/3 → 2/3 → 3/3 (un toque de HOME cada uno) | los sonidos (GEN: el GENRE) |
| **GLO** | **MIX** · **GLOBAL** · **SYSTEM** | MIX: MIXER / SENDS (un toque de GLO sigue al nivel siguiente, en ronda) | los sonidos |
| **SCL · ARP** | **KEY** · **CHORD** · **ARP** | KEY · CHORD · ARP: ARP / ARP 2 (SCL alterna KEY ↔ CHORD, ARP va a ARP) | KEY: ROOT · CHORD, ARP: la pista |
| **LFO** | **MOD** (la matriz como red) · **LFO** · **MSEQ** (abre en LFO) | MOD · LFO: LFO / LFO DEST · MSEQ: MSEQ / MSEQ 2 | MOD: el slot · LFO: la pista · MSEQ: el paso |
| **ENV** | **TRACKS** (las seis envolventes) · **ENV** (la de la pista) · **STAGE** (una etapa) | TRACKS: ENV / DEST · ENV: ENV / ENV DEST · STAGE | TRACKS, ENV: la pista · STAGE: la etapa |
| **EDIT** | **SOUND** (EDIT 1 / EDIT 2; DRUM: el kit) · **TONE** (el filtro, las voces) · el detalle: **FOCUS** (un sinte), **OPERATOR** (FM6), **SOUND** (DRUM: un sonido) | SOUND: EDIT 1 · EDIT 2 (DRUM: KIT · KIT 2 · KIT 3) · TONE: FILTER · MORPH · RETRIG · ROTOR · VOICE · VOICE 2, las que tenga el motor (DRUM: X-MOD · FILTER · STEREO · LEVELS · LEVELS 2 · VOICE) · FOCUS · OPERATOR: OPERATOR · OP ENV · OPERATOR 2 · DRUM: SOUND 1–4 | SOUND, TONE: los sonidos · el detalle: lo de KNOB 1 (el parámetro, el operador, el sonido del kit) |

- **ECO**: los seres de las seis pistas en una grilla, cada uno nadando en su celda con su sonido (un golpe lo
  empuja), su nombre debajo (**T1 ANALOG**; MUTE, OFF si duerme), el seleccionado más grande en un aro violeta.
  Alrededor, **bichitos**: ciliados, esporas, células a la deriva, flagelados y diatomeas diminutos que deambulan, se
  juntan alrededor de las pistas que suenan, se dispersan cuando un ser recibe un golpe y laten con la salida (los
  dibuja solo la pantalla: nada del tiempo del sonido). **MACROS**: KNOB 1–4 son **M1–M4** (las macros, como LFO
  mantenido), las rutas contadas arriba a la izquierda, LATCH a la derecha.
- **BEING**: el ser de la pista grande, sus cuatro perillas de HOME como órganos en las esquinas (un medidor cada una,
  su valor, una vena hasta el cuerpo), el nombre del sonido debajo. **ROUTES**: M1–M4 a la izquierda con sus rutas
  dibujadas hasta donde van (**T2 CUT+62**).
- **ENV** › **TRACKS**: las envolventes de las seis pistas como mosaicos (la seleccionada teñida, con su nivel actual
  como punto; DEST: FILTER, PITCH y SHAPE como barras debajo de cada una); las perillas son de la pista seleccionada —
  **PRESETS** (o ALGORITHM) elige la pista. **ENV**: la envolvente grande / las columnas de ENV DEST, como antes.
  **STAGE** (nueva): KNOB 1 elige la etapa (ATK DEC SUS REL; también PRESETS), KNOB 2 la ajusta, KNOB 3 LOOP; la
  envolvente entera con la etapa sombreada sobre la curva propia de la etapa, grande, y su tiempo.
- **GLO** › **MIX**: las seis pistas como tiras (MIXER: LEVEL con el vúmetro, PAN, REV; **SENDS**, nueva: DIST,
  CHORUS, DLY y REV de cada pista como cuatro barras, las perillas de la pista seleccionada). **GLOBAL**: el compás de
  semicorcheas, como antes. **SYSTEM**: las pistas que suenan como fichas, los tres ajustes de LOAD (encendido el
  elegido), la ruta MIDI, la carga de CPU de los últimos segundos como una línea.
- **SCL · ARP** › **KEY**: las doce notas en un círculo (C arriba), las de la escala encendidas y unidas, ROOT en el
  acento con el nombre de la escala en el medio, QUANT y ALL TR a la derecha, la escala en un teclado debajo; PRESETS
  fija ROOT. **CHORD**: el acorde que toca ahora cada grado de la tonalidad (I–VII, como lo tocaría una tecla: una nota
  si CHORD está en OFF o la voz no es POLY). **ARP** / **ARP 2**: "MODE · RATE · OCT" y el patrón que toca sobre una
  tríada (GATE el largo de las celdas, SWING los contratiempos tarde, PROB algunas fuera); en OFF lo dice. PRESETS en
  CHORD y ARP: la pista.
- **LFO · MOD** › **MOD**: la matriz como red — las fuentes a la izquierda (encendidas las que lee un slot), una curva
  de la fuente de cada slot a su caja **D1–D4** a la derecha (DEST, AMT como barra), el slot elegido con KNOB 1
  enmarcado con un pulso que corre por su curva; PRESETS elige el slot. **LFO**: la onda bajo "RATE · WAVE" / **LFO
  DEST**: el LFO chico a la izquierda, los cuatro montos como filas encendidas por lo que el LFO manda ahora; PRESETS la
  pista. **MSEQ** / **MSEQ 2**: los dieciséis pasos bajo su leyenda; PRESETS el paso.
- **EDIT** › **SOUND**: el ser del motor a la izquierda (vivo con el sonido, el nombre de la página arriba, el del
  sonido abajo), el osciloscopio en un pozo y los ocho parámetros E como fichas a la derecha, encendidas las cuatro de
  la página. **TONE**: **FILTER** dibuja la respuesta de TYPE en CUT con su pico de RES (LP, HP, BP, COMB), CUT una
  línea, DRIVE / DIST una fila de celdas; **VOICE** / **VOICE 2** las voces una por fila, tan llenas como suenan,
  VOICE y PRIO (VOICE 2: ALLOC), la curva de GLIDE; las demás conservan su dibujo. **FOCUS** (nueva): un parámetro E
  grande como medidor — KNOB 1 lo elige (E1–E8), KNOB 2 lo ajusta, KNOB 3 la **fuente** del slot de MOD que apunta a
  él (subida desde OFF la toma un slot libre, con ese parámetro como DEST; de vuelta a OFF lo libera), KNOB 4 el
  **AMT** de ese slot; E1–E8 como pestañas abajo.
- **GEN** es el generador (como HOME mantenido + SELECT, desde cualquier página): un **toque de HOME** cambia la
  página, **PRESETS** el GENRE, **OCT+** arma la pieza, **OCT−** lo cierra (a ECO), **SELECT** a la izquierda vuelve a BEING.

### Cada página y sus cuatro perillas

"—" significa una tarjeta vacía. Una tarjeta de "acción" elegida (girá la perilla a la derecha) queda teñida de
violeta, con el nombre violeta, y dice **OCT+**; apretá OCT+ para ejecutarla.

**ENV** (por pista)

| Página | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
| --- | --- | --- | --- | --- |
| ENV | ATK | DEC | SUS | REL |
| ENV DEST | FILTER | PITCH | SHAPE | LOOP |
| STAGE (0.8.5) | STAGE (ATK DEC SUS REL) | su valor | LOOP | — |

**LFO** (por pista)

| Página | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
| --- | --- | --- | --- | --- |
| LFO | RATE | WAVE | PHASE | FADE |
| LFO DEST | PITCH | FILTER | SHAPE | AMP |
| MOD | SLOT (1–4) | SRC | DEST | AMT |
| MSEQ | STEP (1–16) | LEVEL | LEN | SLEW |
| MSEQ 2 | DIV | — | — | — |

**FX** (las páginas TRACK son por pista; el resto son globales)

| Página | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
| --- | --- | --- | --- | --- |
| FX | DIST | CHORUS (envío) | DLY (envío) | REV (envío) |
| DIST | DIST | TYPE | TONE | — |
| AUTO PAN | DEPTH | RATE | SYNC | WAVE |
| BUSES (0.8.5) | MIX (del delay) | SIZE (de la reverb) | DEPTH (del chorus) | DEPTH (del SLICER) |
| SLICER | MODE | PAT | RATE | DEPTH |
| SL BUS | T1 | T2 | T3 | T4 |
| SL BUS 2 | T5 | T6 | FX | — |
| SL PITCH | STEP | PITCH | LEN | QUANT |
| DLY | TIME | FEEDBK | TONE | MIX |
| DLY 2 | WIDE | PITCH | SPRAY | MOD |
| DLY 3 | TYPE (ANALOG / DIGITAL) | PING (OFF / ON) | — | — |
| REVERB | TYPE | SIZE | DAMP (RESO: FOLLOW) | PRE (SHIMMER/CLOUD: AIR; RESO: PITCH) |
| REVERB 2 | MOD | RATE | FILT | WIDE |
| CHORUS | RATE | DEPTH | STEREO | — |
| MASTER | CLIP | PUNCH | DUCK | REL |

**SCL**

| Página | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
| --- | --- | --- | --- | --- |
| SCL | ROOT | SCALE | QUANT | ALL TR (0.7.6; antes TRANS, y SCL 2 tenía solo ALL TR) |

**EDIT** (por pista; qué páginas aparecen depende del motor)

| Página | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 | Aparece en |
| --- | --- | --- | --- | --- | --- |
| EDIT 1 | E1 | E2 | E3 | E4 | todos los motores (ver [4](#4-motores)) |
| EDIT 2 | E5 | E6 | E7 | E8 | todos los motores |
| RETRIG | RTRG | — | — | — | LOFI con WAVE en BYTE o FLOAT |
| MORPH | MORPH | — | — | — | ANALOG (SAW..SIN), PHASE, LOFI (hasta WRAM) |
| OPERATOR | OP | RATIO / FIXED | FINE | LEVEL | FM6 |
| OP ENV | OP | STAGE | RATE | LEVEL | FM6 |
| OPERATOR 2 | OP | MODE | DTUN | VEL | FM6 |
| FILTER | TYPE | CUT | RES | DRIVE (ANALOG) / DIST (los demás) | todos los motores |
| KIT 3 | BP | FEEDBK | HANDS | — | DRUM (todo el kit) |
| X-MOD | RATE | DEPTH | SHAPE | RAND | DRUM (todo el kit) |
| STEREO | IMAGE | — | — | — | DRUM (todo el kit) |
| SOUND | SOUND | SIDE | PITCH | PMOD | DRUM (un sonido) |
| SOUND 2 | SOUND | DECAY | NOISE | COLOR | DRUM (un sonido) |
| SOUND 3 | SOUND | MODE | PM.TYPE | DRIVE | DRUM (un sonido) |
| SOUND 4 | SOUND | LAYER | FEEDBK | SEND | DRUM (un sonido) |
| LEVELS | KICK | SNARE | CLAP | HATCL | DRUM |
| LEVELS 2 | HATOP | TOM | RIM | BELL | DRUM |
| VOICE | VOICE | GLIDE | MODE | PRIO | todos los motores |
| VOICE 2 (0.8.5) | ALLOC | DETUNE (la apertura de UNI) | — | — | todos menos DRUM |
| FOCUS (0.8.5) | PARAM (E1–E8) | su valor | SRC (el slot de MOD sobre él) | AMT | los sintes (no FM6, DRUM) |

**GLO** (MIXER es por pista; GLOBAL y SYSTEM son globales)

| Página | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
| --- | --- | --- | --- | --- |
| MIXER | LEVEL | PAN | REV | MUTE (derecha ON, izquierda OFF) |
| SENDS (0.8.5) | DIST | CHORUS | DLY | REV (los de la pista seleccionada, como FX > SENDS) |
| GLOBAL | BPM | SWING | CLOCK | TUNE |
| SYSTEM | TRACKS | LOAD | ROUTE | CPU (%) |

**SAVE**

| Página | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
| --- | --- | --- | --- | --- |
| PRESETS | No. (el sonido) | ENG (el motor) | FAV (marcar como favorito) | LIST (ALL / FAV) |
| USER | SLOT (U01–U32) | LOAD (acción) | ERASE (acción) | SAVE (acción) |
| PROJECT | PROJECT (1–8; en el nivel PROJECT de SAVE: SECT A–D) | SECT (A–D; en PROJECT: el NAME de la sección) | LOAD (acción) | SAVE (acción) |
| ARRANGE | ROW ("2/6") | SECT (en END: STOP / x2–x8 / LOOP) | BARS | SCENE |
| TOOLS | cualquier perilla o PRESETS elige de la lista: CLEAR SEQ, INIT SOUND, DELETE ROW, CLEAR SONG, NEW PROJECT, CPU TEST | | | |

**ARP** (por pista)

| Página | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
| --- | --- | --- | --- | --- |
| ARP | MODE | RATE | OCT | GATE |
| ARP 2 | SWING | PROB | HOLD | ORDER |

**SEQ** (por pista)

Las páginas SEQ están agrupadas en tres niveles de zoom (LENTE, 0.8.3; ver [6](#cómo-moverse-lente)): **SELECT** pasa
de un nivel al siguiente, un toque de **SEQ** recorre los juegos del nivel en el que estás. NOTE está en dos niveles: en
COMPÁS edita el piano roll, en PASO el paso dibujado grande; las perillas son las mismas.

| Nivel | Juego (toque de SEQ) | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
| --- | --- | --- | --- | --- | --- |
| PATRÓN | PATTERN | LEN | DIV | SWING | GATE |
| PATRÓN | STRETCH | STRETCH (/8 … /2, x1, x2 … x8) | REPEAT (OFF, x2 … x8, TO 64) | NUDGE | ROTATE (−32 … +32) |
| PATRÓN | PLAYHEADS | HEAD | RATE | DIR | INTERVAL |
| PATRÓN | TRACK | PITCH | CHANCE | HUMAN | REC Q |
| PATRÓN | MUTATE | SOUND (cada) | PATTERN (cada) | AMOUNT | MODE |
| COMPÁS | STEP (melódica: piano roll) | STEP | NOTE | TIME (REST ← TIE ← NOTE → 2 ST … 16 ST: la duración de la nota en pasos) | FLAG (– / ACC / SLD / A+S) |
| COMPÁS | STEP (DRUM: grilla) | STEP | LANE | HIT | ACC |
| COMPÁS | NOTE | FOCUS | PITCH | MOVE | MICRO (−8/16 … +7/16) |
| COMPÁS | AUTOMATION | PLAY (ON/OFF) | EVENT (cantidad) | LOCK (cantidad) | CLEAR (acción) |
| COMPÁS | AUTO LIST | ROW | STEP | PARAM | VALUE |
| PASO | NOTE | FOCUS | PITCH | MOVE | MICRO |
| PASO | COMP | STEP | TYPE | (su valor; ARP: ORDER × RANGE) | EVERY (ARP: RATE; RATCH / ECHO: DIV) |
| PASO | COMP 2 | STEP | TYPE (el segundo componente) | (su valor) | EVERY |

PRESETS elige el compás en PATRÓN y mueve el cursor de paso en COMPÁS y PASO (en AUTO LIST, la fila). En pantalla los
niveles se llaman como en el equipo: PATTERN, BAR y STEP.

### Las capas (mantené el botón)

(0.8.5) Una capa mantenida también es una lente, como la maqueta: **girá ALGORITHM mientras mantenés el botón** para
ir **MAP · KNOBS · STATE** (recuerda el nivel; SELECT conserva sus combinaciones: GLO + SELECT el tempo, los dados).
**MAP**: el mapa de las teclas, una celda por tecla con su nombre (sin íconos); **KNOBS**: las cuatro tarjetas como
medidores; **STATE**: lo que hacen las teclas, perillas y botones (las ayudas del pie hasta 0.8.4) y lo que está
activo (FX: los efectos mantenidos). El pie es la miga de pan, **LENTE · MAP · FX**.

**FX — efectos punch-in** (detalles en [11](#11-tocar-en-vivo))

| Teclas | Efecto |
| --- | --- |
| F3 G3 A3 | REPEAT 1/8, 1/16, 1/32 |
| B3 / C4 | LPF / HPF |
| D4 / E4 | OCT− / OCT+ |
| F4 | 1/2 TEMPO |
| G4 / A4 | DEC− / DEC+ |
| B4 / C5 | STUTTER 1/8 / 1/16 |
| D5 | ATK+ |
| E5 | ARP |
| F5 | RANDOM |
| G5 | borra el carril de punch-in |
| Teclas negras 1–6 (F#3 G#3 A#3 C#4 D#4 F#4) | mutean T1–T6 mientras las mantenés |
| A#4 | a qué pistas afectan los efectos de nota: ALL / SYN / DRM |
| **Perillas** | FILTER, CRUSH, THROW, DEPTH |

**GLO — mezcla**

| Teclas | Acción |
| --- | --- |
| Teclas negras 1–6 | MUTE T1–T6 (enganchado: apretá de nuevo para desmutear) |
| Teclas negras 7–10 (G#4 A#4 C#5 D#5) | Mutes de grupo de DRUM-X: KICK, SNARE, HAT, PERC |
| F3–D4 | SOLO T1–T6 mientras las mantenés |
| E4 | desmutea todo (pistas y grupos de batería) |
| F4 | TAP tempo (desde el tercer toque; solo con clock interno) |
| G4 | **UNDO** (otra vez: rehacer) |
| PLAY | reinicia desde el principio |
| SELECT | tempo |
| **Perillas** | LEVEL T1–T4 (con T5 o T6 seleccionada: T3–T6) |

**SCL** — cualquier tecla fija la ROOT (tónica) en el nombre de su nota. Perillas: ROOT, SCALE, ROTATE, PITCH (las dos
últimas son el ROTATE de SEQ > STRETCH y el PITCH de SEQ > TRACK).

**EDIT** — F3 **INIT** (el sonido de la pista vuelve a los valores por defecto de su motor (un INIT limpio, no un preset), seco: todos los envíos a efectos
en 0; en FM6 la voz init, solo OP1 a nivel máximo; en DRUM, también el kit
DRUM-X de fábrica), G3 **RECALL** (el sonido tal como lo guardó la sección actual; en DRUM, con su kit). Los dos
preguntan antes y no tocan los pasos. En una pista DRUM, las teclas negras 1–8 mutean cada sonido de DRUM-X (BD SD CP
CH OH TM RS CB). Perillas: las cuatro de EDIT 1.

**SEQ TOOLS** (en todas las páginas SEQ, en cualquier nivel de zoom) — F3 CLEAR, G3 REVERSE, A3 / B3 SHIFT a la izquierda /
derecha, C4 RANDOM, D4 COOK, E4 BEAT (DRUM), F4 CLR AUTO (la automatización y los locks de la pista; los pasos quedan).
En una pista DRUM, las herramientas de carril: G4 CLR, A4 REV, B4 FILL, C5 RND sobre el carril elegido con las teclas
negras 1–8. **REC** (con SEQ mantenido): CLEAR ALL. Perillas: LEN, DIV, SWING, GATE. Estas herramientas no tienen
UNDO.

**REC** — F3 **CLEAR** borra los pasos y la automatización de la pista seleccionada (sin diálogo; UNDO lo puede
recuperar), G3 **CLICK** (OFF / REC / ON). KNOB 1: CLICK.

**SAVE — capa de tema** (detalles en [7](#7-arrange-canciones-secciones-y-escenas))

| Teclas | Acción |
| --- | --- |
| F3 G3 A3 B3 | tocar la sección A / B / C / D (en el compás siguiente si está sonando; al instante si está parado). Varias en una misma mantenida: una cadena rápida. |
| C4 D4 E4 F4 | STORE: guarda la música en A / B / C / D |
| D5 | LOOP / SONG: qué reproduce PLAY |
| E5 | SONG REC |
| G5 | ir a la página ARRANGE |
| OCT+ / OCT− | guardar en / recuperar la sección que está sonando |
| **KNOB 1** | el proyecto (1–8); se aplica al soltar SAVE, solo con el transporte parado |

**LFO — macros**: KNOB 1–4 son M1–M4. Soltá y quedan **enganchadas** (el encabezado dice [LFO] LATCH, y KNOB 1–4
siguen controlando las macros en todas las páginas) hasta que apretás LFO de nuevo. F3 tira rutas nuevas, G3 las
borra. (LFO + SELECT tira el LFO de la pista, con KEEP / CANCEL: ver los dados.)

---

## 4. Motores

Nueve motores, todos de síntesis, sin samples. Cualquier motor puede ir en cualquier pista. Elegilos con **PRESETS**
(una sola lista con todos los motores) o con KNOB 2 (ENG) en SAVE > PRESETS. También figuran las cuatro perillas de
HOME de cada pista.

**Cambios suaves**: pasar una pista a otro motor apaga el anterior de a poco (unos 12 ms) y lo que estés sosteniendo
(teclas, o las notas que sostiene el secuenciador) sigue en el motor nuevo, entrando de a poco; no se corta nada.
Cambiar un valor de lista (WAVE, CHIP, un tipo) mientras suenan notas hace una pequeña bajada (unos 3 ms) en vez de un
click; las notas siguen.

| Motor | EDIT 1 | EDIT 2 | Perillas de HOME |
| --- | --- | --- | --- |
| **ANALOG** | WAVE, DETUNE, MIX, NOISE | CUT, RES, DRIVE, INT | CUT, RES, ATK, REL |
| **FM6** | ALG, FB, MOD, RATIO | MEG, VMOD, DETUNE, SLOT | MOD, RATIO, MEG, SLOT |
| **PHASE** | WAVE, WAVE2, DCW, ENV | DETUNE, LINE, SUB, FB | DCW, ENV, DETUNE, REL |
| **LOFI** | CHIP, WAVE, DUTY, CRUSH | —, VIB, BEND, MASK | WAVE, DUTY, CRUSH, REL |
| **VOICE** | VOWEL, VOWEL2, TALK, SHIFT | BUZZ, BREATH, Q, RAND | VOWEL, BUZZ, BREATH, TALK |
| **WHEEL** | REG, SUB, BODY, TOP | PERC, CLICK, DRIVE, ROTOR | REG, PERC, DRIVE, ROTOR |
| **NOISE** | MODE, COLOR, FREQ, PEAK | TRK, DENSE, DRIFT, CRUSH | FREQ, PEAK, COLOR, REL |
| **DRUM** (DRUM-X) | MORPH, TUNE, TONE, DECAY | NOISE, FM ENV, FOLD, DRIVE | MORPH, TUNE, TONE, DECAY |
| **ANIMAL** (0.8.3) | MODE, PRESS, CHAOS, SIZE | SWEEP, RATE, GROUP, TEMP | PRESS, SWEEP, RATE, SIZE |

- **ANALOG**: dos osciladores. WAVE: SAW, SQR, TRI, SIN, PWM, SYNC, RING, SAW3. **INT** fija el intervalo del
  oscilador 2 (±24 semitonos). Su filtro es por voz.
- **FM6**: un motor FM de seis operadores (msfa, de Dexed), 32 algoritmos dibujados en pantalla, cuatro voces por
  pista. Las perillas de EDIT 1/2 son macros sobre el patch. **SLOT**: los patches de fábrica F1–F8, OWN (un patch
  editado o importado) y B1–B32 (un banco DX7 de 32 voces enviado desde el editor web; los slots vacíos se saltean). Una voz del editor o de un
  slot B suena tal como viene en el archivo: los macros vuelven a cero (0.8.1).
  **OPERATOR / OP ENV / OPERATOR 2** editan cada uno de los seis operadores en el equipo (KNOB 1 elige el operador);
  editar convierte el patch en propio de la pista (SLOT OWN), y se guarda con el proyecto. Una voz del banco que
  elegís se copia a la pista, así que los proyectos la conservan aunque después mandes otro banco.
- **PHASE**: distorsión de fase. WAVE / WAVE2: SAW, SQR, PLS, DSIN, SPLS, RSAW, RTRI, RTRP. LINE: MIX o RING.
  **FB** realimenta la salida hacia la fase.
- **LOFI**: sonidos de chip. CHIP: 4BIT, 4B/2, 8BIT, 1BIT, STEP. WAVE: PLS, TRI, SAW, NOIS, WRAM, **BYTE**, **FLOAT**.
  - **BYTE** (bytebeat): KNOB 3 pasa a ser **ALGO** (32 fórmulas), KNOB 4 **VAR** (muta la fórmula); EDIT 2 pasa a
    ser **MOTION, GRIT, BEND, LOOP**. MOTION acelera o frena el ritmo de la fórmula (64 = tal como está escrita),
    GRIT es un reductor de frecuencia de muestreo, BEND pliega el tiempo, LOOP repite un tramo mínimo (con LOOP alto
    el ruido se vuelve una nota afinada).
  - **FLOAT** (floatbeat): CHIP pasa a ser la resolución (4BIT, 4B/2, FULL, 1BIT), KNOB 3 **ALGO** (32 fórmulas
    hechas de senos), KNOB 4 **VAR**; EDIT 2: **MOTION, FEEDBK, BEND, LOOP**. Mantené una tecla: la fórmula suena en
    esa tonalidad.
  - Página **RETRIG** (BYTE / FLOAT): **RTRG ON** reinicia la fórmula desde el principio en cada nota.
- **MORPH** (EDIT > MORPH, en ANALOG, PHASE y LOFI): la onda se desliza de forma continua hacia las siguientes (en
  ANALOG SAW > SQR > TRI > SIN, en PHASE hacia su forma siguiente, en LOFI hacia la onda siguiente hasta WRAM). 0 = la
  onda tal como la elegiste. En ANALOG y LOFI la modulación **SHAPE** también mueve el morph, así un LFO o una
  envolvente barren la onda. La página dibuja un ciclo de la onda resultante y dice qué dos ondas mezcla. La KNOB 2 es
  **DRIFT** (0–100 %, 0 = apagado): la afinación de cada voz (hasta unos ±27 cents) y su filtro derivan lento, cada una
  a su manera, como los osciladores analógicos viejos. Un acorde cobra vida; en valores altos se marea.
- **VOICE**: una voz por formantes (basada en klattsch): vocales, habla, aliento.
- **WHEEL**: un órgano de drawbars: registraciones (REG), percusión (PERC: OFF, 2ND, 3RD, 2SOFT, 3SOFT, 2SLOW,
  3SLOW), click de tecla, drive, ROTOR (OFF, SLOW, FAST). EDIT > **ROTOR** (solo WHEEL): **DEPTH** (cuánto gira el
  parlante rotativo: 100 % como antes, 0 % quieto) y **DUST** (0–100 %: el crujido y el siseo de un órgano viejo mientras
  suena).
- **NOISE**: ruido coloreado y metálico. MODE: ANLG, DUST, LFSR, META (DUST es solo el crepitar: con DENS en 0 igual crepita suave). PEAK es su propia resonancia.
- **DRUM**: DRUM-X, ver [5](#5-drum-x).
- **ANIMAL**: cantos de animales con modelos físicos, ver [ANIMAL](#animal-083) más abajo.

### ANIMAL (0.8.3)

Cantos de animales hechos como los hacen los animales: **una fuente que se autooscila o golpea** (la siringe de un
pájaro, la lima y el raspador de un grillo, el tímbalo de una cigarra, la laringe de una rana, un ala) **excita uno o
dos resonadores** (el ala, el abdomen, el saco vocal, el tracto), con una capa lenta de **presión, tensión y ritmo**.
**MODE** elige la fuente; cada modo lee las ocho perillas a su manera y la pantalla les cambia el nombre. La tecla fija
la tensión (la altura, la portadora), la velocidad la presión; **una nota empieza un canto**, así que una secuencia de
notas es una secuencia de cantos, afinada con el resto de la música.

| MODE | EDIT 1 (KNOB 2..4) | EDIT 2 | Qué modela |
| --- | --- | --- | --- |
| **CMOS** | TOUCH, COUPLE, CAP | HYST, RATE, GROUP, FM | la "máquina de pájaros" con 40106 / 4093 (abajo) |
| **PULSE** | CLICK, Q, SIZE | DETUNE, RATE, GROUP, TEMP | grillos, esperanzas, cigarras, ranas y sapos |
| **WHSTL** | PRESS, CHAOS, SIZE | SWEEP, RATE, GROUP, TEMP | silbidos de pájaros (y un murciélago, transpuesto) |
| **WING** | BUZZ, DRIFT, SIZE | DOPPLR, RATE, GROUP, TEMP | mosquito, abeja, mosca |
| **GLOTT** | MOUTH, ROUGH, SIZE | SWEEP, RATE, GROUP, AIR | mamíferos: lobo, perro, rugido, ronroneo |

**El ritmo del canto** (todos los modos menos CMOS): **RATE** es la tasa de sílabas (0,05–40 por segundo); **GROUP**
cómo se agrupan: **HOLD** (una sílaba mientras dure la nota: un arrullo, un aullido, un zumbido), **TRILL** (sin pausas)
o *n*+*m* (*n* sílabas y después *m* en silencio: 3+9 es el chirrido de tres del grillo de campo); **TEMP** (°C, 22 como
está escrito) escala todas las tasas como lo hace la temperatura en insectos y ranas (ley de Dolbear: un grillo a 30 °C
chirría unas 2,4 veces más rápido que a 14 °C). **SIZE** mueve todo el animal hacia arriba o abajo desde la tecla, un
semitono por paso (64: como se toca; menos: más chico y más agudo; en GLOTT es el largo del tracto: todos los
formantes juntos, sin tocar la altura). **SWEEP** es el gesto de altura de cada sílaba: un gancho exponencial desde
abajo (+) o desde arriba (−) hacia la nota, hasta dos octavas; por encima de 16 sílabas por segundo se angosta, como el
trino de un pájaro, que no puede ser a la vez tan rápido y tan ancho (Podos).

- **CMOS**: el circuito de protoboard del dueño, un chip Schmitt 40106 / 4093, dos capacitores grandes, tres potes y una
  punta que se toca con la mano ("una máquina de cantos de pájaros, monos y bichos"). Un oscilador RC rápido (una
  octava sobre la tecla: C5 chirría a ~1,05 kHz, como en el video) carga y descarga su capacitor exponencialmente entre
  dos umbrales; su onda cuadrada se limita en banda en el flanco exacto. Uno lento lo compuertea (la NAND del 4093) y
  la tensión de su capacitor lo barre (la FM del 40106): la carga exponencial es el "gancho" que se oye al principio de
  cada chirrido. **RATE** el lento (chirridos por segundo), **CAP** cuánto carga contra cuánto descarga (pitidos cortos
  .. barridos largos), **HYST** su ventana de umbrales (la profundidad del gancho; + sube hacia la nota, − baja hacia
  ella), **FM** 0 solo la compuerta (pitidos), 20 compuerta y barrido (el pájaro), 40 solo el barrido (una sirena),
  **TOUCH** la mano en la punta (más rápido, menos regular), **COUPLE** un segundo oscilador rápido acoplado en los dos
  sentidos (enganche, después caos: bichos), **GROUP** un tercero, muy lento, que agrupa los chirridos en frases y
  acelera mientras carga (monos).
- **PULSE**: un escape de reloj. Cada diente de la lima de un grillo (cada costilla del tímbalo de una cigarra, cada
  pulso de la laringe de una rana) es un impulso hacia un resonador afinado a la tecla (el ala, el abdomen, el saco
  vocal). **CLICK** 100 %: un pulso por ciclo de la resonancia, el tono puro del grillo de relojería; menos, un pulso
  cada 2 .. 64 ciclos: clics que lo hacen sonar (el zumbido de una cigarra, el croar de una rana; en 1/2, los dos picos
  a una octava de la rana arborícola). **Q**: de banda ancha (el raspado de una esperanza) a puro (un grillo).
  **DETUNE**: los pulsos fuera de la resonancia (batidos, un canto "anómalo") y con jitter.
- **WHSTL**: una siringe como **oscilador de Hopf**: el tono nace cuando la **PRESS**ión del saco aéreo cruza un umbral
  y crece con ella (su nivel ~ la raíz de cuánto lo pasa: debajo del umbral no suena, sea cual sea la envolvente), así
  que cada sílaba tiene un ataque real; la altura sube un poco con el nivel. **CHAOS**: ruido en la presión y en la
  altura, y un segundo armónico.
- **WING**: un pulso limitado en banda al ritmo del aleteo con una **DRIFT** lenta y aleatoria de altura; **BUZZ** lo
  angosta (más brillante); cada sílaba es una pasada: más fuerte y brillante al acercarse, su altura corrida por
  **DOPPLR** (más aguda al venir, más grave al irse). HOLD: un vuelo quieto.
- **GLOTT**: un pulso glótico hacia dos formantes. **MOUTH** cerrada (u) a abierta (a), y de oscura a brillante;
  **ROUGH** en dos mitades de lo que hacen los mamíferos: duplicación de período (un subarmónico, F0/2) hasta 50 %,
  después un mapa logístico que mueve cada ciclo (caos determinista: un rugido); **AIR**: ruido de aliento. Un
  ronroneo: tocá grave (C1–C2), ROUGH baja la altura a la mitad.

**Presets** (24): CMOS BIRD, CMOS TRILL, CMOS MONKEY, CMOS BUG · CRICKET, TREE CRICKT, KATYDID, CICADA, TREEFROG, TOAD,
BULLFROG · CANARY, CARDINAL, DOVE, OWL, PEEPER, BAT · MOSQUITO, BEE, FLY · WOLF, DOG BARK, ROAR, PURR. Están pensados
para sonar bien alrededor de C5 (los de ranas y mamíferos una o dos octavas más abajo: BULLFROG y OWL C4, WOLF G4,
ROAR C3, PURR C2).

ANIMAL va último en la lista de PRESETS, después de DRUM, así que cada sonido anterior conserva su lugar (y su número
de PROGRAM CHANGE). No usa RAM (el estado de cada voz entra en la voz), unos 7,6 KB de flash y, con ocho voces, como
mucho ~85 % de la CPU del motor más pesado (GLOTT; CMOS, PULSE, WHSTL y WING alrededor de la mitad): las ocho voces en
todos los modos.

### FILTER (todos los motores)

EDIT > **FILTER**: **TYPE** LP, HP, BP o **COMB** (un filtro peine afinado a la nota que tocás; CUT lo mueve, RES es
cuánto resuena), **CUT**, **RES**, y KNOB 4: en ANALOG su propio DRIVE, en todos los demás motores el DIST de la
pista. En ANALOG el filtro es por voz; en LOFI la página de filtro maneja su filtro por voz; en los demás es un filtro
sobre la pista que sigue a LFO DEST / ENV DEST FILTER.

### VOICE (todos los motores)

EDIT > **VOICE**: **VOICE** POLY, MONO, LEG (legato), UNI (unísono); **GLIDE**; **MODE** RATE o TIME (cómo se mide el
glide); **PRIO** LAST, LOW, HIGH. Las seis pistas comparten un presupuesto de 8 voces; FM6 toca como máximo cuatro por
pista.

---

## 5. DRUM-X

DRUM-X es un kit sintetizado de ocho sonidos, **BD SD CP CH OH TM RS CB**, generados en tiempo real.

- (0.7.2) **Nueve kits como presets del motor DRUM**: DRUM-X (el kit de fábrica), 909 CLUB, 808 BOOM, LOFI DUST,
  INDUSTRIAL, ACOUSTIC, DUB ROOTS, TECHNO RAW, BREAKS. Recorrer uno (PRESETS en HOME, el editor, un PROGRAM CHANGE)
  carga el kit completo en la sección: los lados A y B de los ocho sonidos, sus LAYER y SEND, y las perillas KIT de la
  pista. Cambiar de motor, INIT SOUND, cargar un proyecto y GEN dejan el kit de la sección como está; DRUM-X devuelve el
  de fábrica. Editá cualquiera en EDIT > SOUND; la sección guarda el resultado.

- Cada sonido tiene dos lados, **A** y **B**. **MORPH** (KIT, KNOB 1, también en HOME) mueve todo el kit entre ellos:
  la perilla para tocar en vivo.
- Las páginas van **primero todo el kit, después un sonido, al final los volúmenes y el estéreo** (0.6.58):
  - **KIT** (EDIT 1): MORPH, TUNE, TONE, DECAY. **KIT 2** (EDIT 2): NOISE, **FM ENV** (FM golpeada con cada golpe que se
    va en pocos milisegundos: un parche, un click, un filo metálico en el ataque, la cola limpia), **FOLD** (un
    wavefolder después de la envolvente: ataque brillante, cola limpia), DRIVE.
  - **KIT 3**: **BP**, un pasa-banda sobre los sonidos de metal (hi-hats, rim, cowbell), OFF o 15 centros de 1,5 a 9 kHz
    aprox.; **FEEDBK**, el oscilador de cada sonido realimentado sobre sí mismo (OFF, 1–15: cálido, gruñido, arena,
    ruido); **HANDS** (OFF, 1–15): más gente aplaudiendo en el CLAP. Cada mano extra golpea unos milisegundos después de
    la primera (3 a 45 ms), en su propio momento y nivel, sorteados de nuevo en cada golpe: nunca a tempo. OFF es el clap
    como antes; unas pocas, un grupo chico; 15, una multitud.
  - **X-MOD**: **RATE** (OFF, 4BAR … 1/32, a tempo), **DEPTH** (cuánto oscila MORPH), **SHAPE** (SINE, TRI, SAW, RAMP,
    S&H), **RAND** (cada golpe con su propio MORPH y un poco de afinación).
  - **FILTER**: el del kit.
  - **SOUND 1–4**, un sonido cada una: la KNOB 1 es siempre **SOUND** (cuál; tocar una tecla también lo elige).
    - **SOUND**: SIDE (A / B), PITCH, PMOD (cuánto cae la afinación).
    - **SOUND 2**: DECAY, NOISE, COLOR, del lado elegido en SOUND (la KNOB 1 lo muestra: "SD B").
    - **SOUND 3**: **MODE** (onda SIN / FM / MTL / BEL, filtro de ruido LP / BP / HP, "+" para snap), **PM.TYPE** (cómo
      se mueve la afinación: DECAY, LONG, NOISE, SINE), **DRIVE** (ON / OFF).
    - **SOUND 4**: **LAYER** (OFF, 1–15): un segundo oscilador en un intervalo desafinado, modulado en frecuencia por el
      sonido, más cuanto más alto: de un filo metálico a un golpe sin nota, así el sonido se funde en el ritmo y no
      molesta a la armonía (el kit de fábrica lo tiene en redoblante, hi-hats y cowbell); **FEEDBK** (OFF, 1–15): el de
      ese sonido (se suma al de KIT 3); **SEND**: ese golpe solo hacia un efecto, OFF y después CHO, DLY, REV con 16
      cantidades cada uno, en una sola perilla ("REV 75%"), además de los envíos de la pista. Los dados (EDIT + SELECT en
      una pista DRUM, DICE ALL) mueven fuerte LAYER y FEEDBK. Cada sonido con LAYER usa un poco de CPU (como FEEDBK).
  - **LEVELS / LEVELS 2**: el nivel de cada sonido (KICK, SNARE, CLAP, HATCL, HATOP, TOM, RIM, BELL).
  - **STEREO**: **IMAGE** (automatizable) barre el kit por sus ubicaciones en el estéreo. 0 (MONO): todos al medio; los
    primeros pasos lo abren; desde ahí cada paso es otra ubicación (cada sonido por su propia curva), así que girala
    hasta que la imagen quede bien. El bombo queda cerca del medio. DRV, DIST y el FILTER trabajan sobre la suma del
    kit, así que con ellos la imagen se angosta (la página dibuja los sonidos en gris).
  - KIT 3, X-MOD, LAYER, FEEDBK y SEND se guardan con el kit de la sección.
- **Mutes**: GLO mantenido + teclas negras 7–10 mutean los grupos KICK, SNARE, HAT, PERC; EDIT mantenido + teclas
  negras 1–8 mutean cada sonido.
- **EDIT mantenido + SELECT** tira un kit nuevo alrededor del de fábrica. **EDIT mantenido + F3 (INIT)** trae de
  vuelta el kit de fábrica.

Un kit por sección, compartido por todas las pistas en DRUM: mientras otra pista toca DRUM-X, recorrer kits en una pista distinta deja ese kit como está ("KIT OF T4 KEPT", 0.8.1); cargá los kits en la pista de batería. El kit se guarda con cada sección de un proyecto (no con los presets de usuario — ver [12](#12-guardar-y-cargar)).

---

## 6. El secuenciador

Cada pista tiene su propio patrón de **64 pasos** con su propio largo (LEN), división (DIV), swing y gate, así que
patrones de distinto largo giran unos contra otros.

### Cómo moverse: LENTE

El secuenciador funciona como una lente, como un microscopio (LENTE, 0.8.3). **SELECT** hace zoom entre tres niveles,
**PATRÓN ↔ COMPÁS ↔ PASO** (en pantalla: PATTERN, BAR, STEP), uno por paso, sin dar la vuelta. Cada nivel muestra su
propio dibujo y tiene sus propios juegos de perillas, y un toque de **SEQ** recorre los juegos del nivel en el que
estás. Las **pestañas de nivel de arriba** (0.8.5; hasta la 0.8.4, la primera fila del pie) dicen siempre dónde estás:
tres pestañas iguales **PATTERN · BAR · STEP** justo debajo del encabezado, el nivel en el que estás relleno de violeta
(otra paleta: su propio color de selección), las tarjetas debajo, y la miga de pan del pie dice el juego:
**LENTE · BAR · STEP**. Los compases del patrón (hasta cuatro, con LEN 64) se dibujan en el panel en PATRÓN y COMPÁS
como celdas chicas: el que editás teñido de violeta con su número en violeta, el número del que suena en naranja, un
compás más allá de LEN oscuro.

| Nivel | Qué muestra la pantalla | Un toque de SEQ recorre | PRESETS |
| --- | --- | --- | --- |
| **PATRÓN** | Los compases del patrón uno al lado del otro en miniaturas grandes, cada una sus dieciséis pasos como rayitas (una pista DRUM: sus carriles); debajo, **PLAYHEADS**: el patrón entero como un solo roll chiquito (las filas de la escala teñidas, las líneas de los tiempos más claras, un paso con componente marcado arriba, los pasos más allá de LEN oscuros) con el cabezal y los cabezales extra de PLAYHEADS (una marquita cada uno al pie). | PATTERN (LEN DIV SWING GATE) → STRETCH → PLAYHEADS → TRACK → MUTATE | el **compás** (sin dar la vuelta; el cursor conserva su lugar dentro del compás) |
| **COMPÁS** | Arriba una tira con los compases del patrón en chiquito (el que editás teñido), y debajo el **piano roll** del compás a todo el ancho: 30 semitonos, las filas de la escala teñidas, el cursor un marco violeta alrededor de su columna, el cabezal una línea naranja, un paso con componente una marquita arriba, un lock una marca al pie, una tecla apretada en la pista una marca a la izquierda de su fila (una pista DRUM: la **grilla**, con sus teclas). | STEP → NOTE → AUTOMATION → AUTO LIST | el **cursor de paso**; al pasarse del final del compás entra en el siguiente |
| **PASO** | El paso del cursor dibujado grande: arriba **STEP n** y a la derecha NOTE / TIE / REST (una nota de más de un paso: **LEN n**, que se fija con el KNOB 3 de COMPÁS > STEP); a la izquierda sus notas como barras donde las pone su altura, cada una con su nombre (la nota en foco en naranja, MICRO las corre, RATCH las parte; un TIE muestra las notas que sostiene), y a la derecha sus fichas: **ACC, SLD, MICRO n, COMP1 x, COMP2 x, CHANCE n%** (con un valor puesto: una ficha teñida de violeta; sin nada: apagada). | NOTE (FOCUS PITCH MOVE MICRO) → COMP → COMP 2 | el **cursor de paso** |

Desde la 0.8.5 el roll ya no tiene la tira de teclado ni los nombres de las filas de C (como la maqueta): el nivel PASO
nombra las notas y la tarjeta PITCH dice la nota en foco.

- **SEQ desde otra página** vuelve al nivel y al juego que usaste por última vez **en esa pista** (cada pista recuerda
  el suyo); al encender es el STEP de COMPÁS, el piano roll.
- Ir a otro compás es un paso de PRESETS en PATRÓN (antes eran dieciséis pasos de KNOB 1); COMP 2 es SELECT + dos
  toques (antes eran cinco).
- Todo lo demás sigue donde estaba: **SEQ mantenido** abre SEQ TOOLS en cualquier nivel, **SEQ + SELECT** tira los
  dados de la secuencia, **SEQ + REC** borra todo, un paso mantenido + una perilla es un lock (en el STEP de COMPÁS), la
  grabación paso a paso funciona en STEP y NOTE, **EDIT** borra el paso. **SEQ mantenido + PRESETS** sigue recorriendo
  todas las páginas SEQ de a una.
- PHRASES (el cargador de patrones) cuenta como PATRÓN, CHANCE como PASO, cuando algo te lleva ahí.

### Escribir pasos

- **SEQ > STEP** en una pista melódica es un **piano roll**. KNOB 1 mueve el cursor de paso, KNOB 2 transpone el paso,
  KNOB 3 es **TIME**, KNOB 4 fija acento / slide. Un paso guarda hasta cuatro notas.
- **La duración de una nota** (0.8.4): KNOB 3 (TIME) es un solo recorrido, REST ← TIE ← NOTE → 2 ST → … → 16 ST.
  Sobre una nota, girar a la derecha la **alarga**: los pasos que siguen se vuelven TIEs, uno por paso de perilla (la
  tarjeta dice **3 ST**, el piano roll dibuja la barra más larga); un acorde conserva todas sus notas. Girar a la
  izquierda la **acorta** (sus últimos TIEs pasan a REST) hasta NOTE; después el paso mismo pasa a TIE y luego a REST,
  como antes (un giro rápido se frena primero en NOTE). Solo se alarga sobre RESTs y pasos vacíos: **la nota siguiente
  la frena** (la pantalla dice NEXT NOTE; pasá ese paso a TIE o borralo con EDIT para seguir). Hasta 16 pasos o LEN;
  cerca del final del patrón da la vuelta al paso 1, como lo toca el secuenciador. Así cualquier nota, grabada o
  escrita, se alarga o se acorta después, sin REC. (En una pista DRUM no cambia nada: los golpes no tienen duración.)
- **Grabación por pasos**: en STEP, con el transporte **parado**, apretá **REC** (la pantalla dice KEYS WRITE STEPS).
  Tocá una nota o un acorde: entra en el paso del cursor y, cuando soltás, el cursor avanza. Sin armar, las teclas
  solo suenan.
  - **Duración**: con las teclas apretadas, girá **KNOB 1**: la nota o el acorde dura esa cantidad de pasos (1–16,
    con ties). Se escribe cuando soltás y el cursor salta al final.
  - **Arpegio**: con las teclas apretadas, apretá **ARP**: el acorde se escribe arpegiado en el piano roll. KNOB 1 pasa
    a ser la resolución (1/16, 1/8, 1/8., 1/4: una nota cada 1–4 pasos), KNOB 2 el orden (UP, DOWN, UP-DN, RND) y
    KNOB 3 las octavas (1–3). La pantalla muestra lo elegido; al soltar se escribe y el cursor queda después. KNOB 4
    (y 2–3 sin ARP) sigue haciendo parameter locks en el paso.
- **EDIT** (toque) en STEP borra el paso del cursor.
- En una pista **DRUM**, STEP es una **grilla**: las teclas blancas son los 16 pasos de la página, las teclas negras
  1–8 eligen el carril (y lo tocan), la tecla negra 9 mantenida es ACC (las teclas blancas entonces activan o
  desactivan acentos), las teclas negras 10 / 11 bajan / suben de página cuando LEN pasa de 16. Perillas: STEP,
  LANE, HIT, ACC.

### Grabación en vivo

Apretá **REC** en cualquier lado (no en STEP con el transporte parado): la pista queda armada y arranca el transporte.
Lo que tocás se graba en el patrón, cuantizado a sus pasos, en overdub. Los movimientos de perilla en la pista
seleccionada se graban como **automatización**. MENU > AUDIO tiene el **CLICK** (OFF / REC / ON), **CLICK LEVEL** y
**COUNT-IN** (OFF, 1 BAR, 2 BARS).

### Páginas de patrón

- **COMP** (step components, al estilo OP-Z): hasta dos por paso (**COMP** el primero, **COMP 2** el segundo, igual;
  0.6.82). KNOB 1 elige el paso, KNOB 2 su **TYPE**, KNOB 3 su valor, KNOB 4 **EVERY** (0.6.92; en COMP, el RATE de un
  ARP o el DIV de RATCH / ECHO van ahí en su lugar). COMP y COMP 2 viven en el nivel PASO de LENTE: el
  paso dibujado grande, con su ficha COMP o COMP 2 encendida y el tipo que tiene (0.8.3; el dibujo propio del
  componente ya no está, sus valores están en las tarjetas).
  - **CHANCE** (0–100 %) y **RATCH** (x1–x4): la probabilidad y el ratchet propios del paso (solo en COMP). El KNOB 4
    de RATCH es su **DIV**: STEP reparte los golpes en el paso (como antes); 1/2, 1/3, 1/4, 1/6, 1/8 los separa esa
    fracción de paso desde el inicio, los que entren (RATCH x3 a 1/8: tres golpes rápidos y después silencio).
  - **ARP NOTES / SEQ / SCALE**: el paso toca un arpegio y sigue sobre los ties que vienen después (su duración, el
    paso y sus ties). NOTES: las notas del paso, de grave a agudo; SEQ: la melodía del patrón, en el orden en que la
    toca el patrón (0.6.82; antes se ordenaba como NOTES); los dos sobre 1–4 octavas. SCALE: la escala hacia arriba
    desde la nota más grave del paso, 3, 5, 8 o 15 grados. KNOB 3 **ORDER** y rango en una sola lista (UP, DOWN, UP-DN,
    RND sobre 1 OCT, después sobre 2 OCT …; en SCALE 3, 5, 8, 15 DEG). KNOB 4 **RATE** (0.6.92), la velocidad del
    arpegio en tiempo musical: **x1** una nota por paso (el DIV de la pista, 1/16), **1/32** dos por paso, **1/32T**
    tres, **1/64** cuatro, **1/64T** seis, **1/128** ocho (la tarjeta dice la figura según el DIV de la pista; un DIV
    raro: xn). Los x2 … x8 de antes son los mismos valores.
  - **TRIG**: el paso suena solo en algunas vueltas del patrón: 1:2, 2:2, 1:3, 2:3, 3:3, 1:4 … 4:4, 1:8, FIRST, !1ST
    (todas menos la primera).
  - **RAND**: la nota sube o baja por la escala al azar, hasta RANGE grados (1–7).
  - **OCT**: una octava: +1, −1, +1 cada dos vueltas, −1 cada dos vueltas, ALT (arriba y abajo alternando), RND.
  - **TONAL**: desde este paso hasta el final de la vuelta todas las notas se mueven SHIFT grados (−7 a +7) por la escala.
  - **BEND**: la nota se desliza BEND semitonos (−12 a +12) durante el paso.
  - **ECHO**: el paso se repite x2–x8 en su duración, cada repetición más suave. KNOB 4 su **DIV**, como el de RATCH:
    STEP reparte las repeticiones en el paso, 1/2 … 1/8 fija cuán separadas van.
  - **PULSE**: la nota vuelve a sonar en los pasos vacíos que siguen, cada 1–4 pasos (EVERY), hasta la próxima nota.
  - **JUMP**: después de este paso el patrón sigue desde TO STEP.
  - **REV**: desde este paso el patrón vuelve hacia atrás hasta el paso 1 y después sigue hacia adelante (ping-pong).
    JUMP y REV giran el patrón en las vueltas que dice **EVERY** (abajo): EVERY 1:2 salta una vuelta sí y otra no,
    FIRST solo la primera vez (una intro), !FIRST todas menos la primera (0.6.92; antes era TIMES).
  - **FILL**: el paso suena solo mientras mantenés **FX**.
  - **KILL**: el paso primero corta todo lo que suena en la pista (releases largos, notas sostenidas) y después toca.
  - **WALK** y **CLIMB** (0.6.98, a la manera de los componentes evolutivos del OP-Z y el OP-XY): las notas del paso se
    mueven un grado de la escala (WALK) o una octava (CLIMB) **cada tantas vueltas** del patrón (KNOB 4: 1 … 10, 12,
    16 vueltas por movimiento), dentro de un **rango** (WALK 3, 5, 7 o 12 grados; CLIMB 1 … 4 octavas), y vuelven a
    empezar. KNOB 3 el **MODE** y el rango juntos: **UP** (0, 1, 2 … rango, 0 …), **DOWN**, **UP-DN** (ida y vuelta),
    **RND** (un lugar del rango por movimiento, el mismo en cada vuelta completa). Se apilan entre sí y con el resto:
    un WALK en COMP y un CLIMB en COMP 2 se suman, un TONAL o un OCT encima siguen valiendo, un ARP camina con su
    acorde. Cuatro notas con un WALK UP-DN 5 cada 2 vueltas y un CLIMB UP 1 cada 3 vuelven a sí mismas a las 36
    vueltas, distintas en cada pasada. Un kit de batería los ignora. La página dibuja los movimientos del rango, con el
    de esta vuelta encendido.
  - **Dos en un paso**: suenan los dos (un ECHO con un OCT: las repeticiones una octava arriba; un RAND con un OCT:
    los dos movimientos). Gana el que deja la nota en silencio: un **TRIG** o un **FILL** en cualquiera de los dos
    lugares decide primero si el paso suena. De dos que hacen lo mismo gana el primero (un ARP sobre un ECHO, dos ARP:
    el de COMP).
  - **EVERY** (0.6.92, KNOB 4 en COMP 2, y en COMP para todos los tipos menos ARP, RATCH y ECHO): en qué vueltas del
    patrón suenan los componentes del paso — **ALWAYS**; **1:2** (1ª, 3ª, … vuelta), **2:2** (2ª, 4ª, …); **1:3**,
    **2:3**, **3:3**; **1:4** … **4:4**; **1:8**; **FIRST** (solo la primera vuelta); **!FIRST** (todas menos la
    primera); **SKIP** al final de la perilla (se guarda pero no suena: un interruptor). Un solo valor por paso, para
    sus dos componentes, que se fija desde el KNOB 4 de COMP 2 sea cual sea su TYPE (o desde COMP, en los tipos que no
    usan esa perilla): un arpegio que entra cada cuatro compases es ARP en COMP (ORDER, RATE) y EVERY 4:4 en COMP 2.
    La nota propia del paso suena en todas las vueltas (eso lo decide un componente **TRIG**). Reemplaza a WHEN (esa
    página ya no existe), con su A / B leídos como uno solo.
  Los proyectos los guardan. **En un kit de batería** (0.6.83) también funcionan: OCT, RAND y TONAL afinan los golpes
  (RAND y TONAL en la escala de la pista), BEND dobla cada golpe durante el paso; ARP NOTES toca los sonidos del paso
  uno tras otro, SEQ los sonidos del patrón en el orden en que suenan, SCALE re-dispara los golpes del paso, cada parte
  un grado más arriba.
- **Un slide de acordes largo** (0.6.83): el glide hacia un acorde dura lo que dura ese acorde, con sus TIEs; GLIDE
  fija cuánto de ese tramo (127: todo). Un acorde sostenido con TIEs en pasos lentos (p. ej. DIV 1/4 a 120 BPM: diez
  pasos, 5 s) se desliza así de lento y suave; las notas nuevas aparecen a medida que se deslizan.
- **PATTERN** (el nivel PATRÓN de LENTE, su primer juego): LEN (1–64), DIV, SWING, GATE — el largo del patrón, la figura de su paso, su
  swing y la duración de todas las notas (la de un paso solo: el TIE de STEP, o MICRO para su entrada).
- **Mover una nota o un acorde** que entró antes o después: **NOTE**, KNOB 1 FOCUS en el paso, KNOB 3 **MOVE** lo
  corre de a un paso a los costados (FOCUS ALL: el acorde entero con sus TIE; una nota enfocada: esa sola).
- **STRETCH** (0.6.85; nivel PATRÓN, después de PATTERN): cambia la forma de toda la secuencia de la pista elegida **mientras girás la
  perilla** (0.6.92; antes esperaba OCT+). Cada giro parte de la secuencia como estaba antes del primero (su base), así
  que x2 y después x4 es x4, no x8, y volver al medio es volver a donde empezaste; el cartel dice el LEN antes y
  después (LEN 4 > 16); **UNDO** (GLO + G4) lo deshace; si editás el resultado, pasa a ser la nueva base. La otra
  perilla toma el resultado como su base (0.6.96): REPEAT x2 y después STRETCH x2 estira las repeticiones.
  - **KNOB 1 STRETCH**: **x2 … x8** separa los pasos esa cantidad de veces (LEN 4 x4: 16); una nota sostenida sigue
    sostenida y las cortas quedan cortas; pasados los 64 pasos se corta el final. **/2 … /8** la comprime: de cada
    grupo de pasos queda el primero que suena (LEN 32 /2: 16). **x1** en el medio: como estaba. La automatización y los
    locks se mueven con sus pasos.
  - **KNOB 2 REPEAT**: copia los pasos 1..LEN detrás de sí mismos, **x2 … x8** veces o **TO 64** (todas las copias
    enteras que entren), con automatización y locks; **OFF** a la izquierda: como estaba. Cuatro notas pasan a ser una
    base de 64 pasos sobre la que grabar más notas (REC en vivo suma encima).
  - **KNOB 3 NUDGE**: toda la secuencia un paso a la derecha / izquierda, dando la vuelta dentro de LEN (como TOOLS
    A3 / B3).
  - **KNOB 4 ROTATE** (−32 … +32; era el OFFSET de SHIFT): la secuencia leída desde otro paso, sin mover los pasos —
    una rotación en vivo, automatizable (también en el KNOB 3 de la capa SCL).
- **MICRO** (0.6.92, KNOB 4 de NOTE; era el OFFSET de la página TIMING): el lugar propio del paso enfocado fuera de la
  grilla, en dieciseisavos de paso, de −8/16 (medio paso antes) a +7/16 (después); **EDIT** lo devuelve a la grilla.
  Los pasos adelantados suenan realmente antes: el paso anterior termina antes y la grilla se mantiene. El piano roll
  dibuja las notas donde suenan. Un acorde comparte el offset de su paso; mover, estirar o repetir pasos lo conserva;
  se guarda con el proyecto. **REC Q** (KNOB 4 de TRACK) es la cuantización de grabación que lo escribe: en 100 % (por
  defecto) la grabación en vivo se ajusta a la grilla; más abajo, el paso guarda esa parte de dónde lo tocaste (0 %:
  todo), antes o después.
- **TRACK** (0.6.92; era SHIFT + HUMAN): los ajustes de toda la pista. **PITCH** (±24 semitonos; no en baterías).
  **CHANCE**: la probabilidad de toda la pista; 100 % toca cada paso como está escrito, menos lo va raleando (escala la
  probabilidad propia de cada paso, la de los dados o el editor). **HUMAN** (una sola cantidad, 0–100 %): la pista
  toca como una persona, cada nota un poco distinta en cada vuelta, con los pasos como están escritos — su velocity
  sube o baja al azar (hasta ±50 al 100 %), entra un poco tarde (hasta 40 ms, nunca más de un tercio del paso, nunca
  antes; las notas de un acorde siguen juntas y un STRUM mantiene su separación), dura más o menos (hasta ±40 %) y el
  DECAY y el RELEASE de su envolvente se alargan o acortan un poco (hasta ±20). En 0 % toca exactamente lo escrito
  (por defecto). Se mantiene al cambiar el sonido; se puede automatizar. Un proyecto guardado con los viejos VEL / TIME
  / GATE carga con el menor de los tres como HUMAN. **REC Q**: ver MICRO. El viejo **C.LEN** (los componentes como un
  carril con su propio largo) ya no existe: los componentes viven en sus pasos y EVERY dice cada cuánto suenan.
- **AUTOMATION**: PLAY activa o desactiva la automatización de la pista, EVENT y LOCK cuentan los registros, CLEAR
  los borra (OCT+, con diálogo). La automatización es lo que grabás girando una perilla con REC encendido mientras la
  pista suena (un valor que se mueve a lo largo del patrón), más los parameter locks (un valor por paso, ver abajo):
  esta página muestra cuántos de cada uno tiene la pista y si suenan; la pista de ayuda de la página lo dice. En AUTO
  LIST se ven y se editan uno por uno.
- **AUTO LIST**: cada evento de automatización y cada lock en forma de lista. KNOB 1 la fila, 2 el paso, 3 el
  parámetro, 4 el valor. En **+ ADD LOCK**, OCT+ agrega un lock; sobre un registro, OCT+ lo alterna entre LOCK y AUTO;
  **EDIT** lo borra.
- **MUTATE**: la pista cambia sola, sincronizada con el clock, para que una textura evolucione durante minutos.
  **SOUND** y **PATTERN**: cada OFF, 1, 2, 4, 8, 16 o 32 compases (el cambio cae en el 1 del compás). **AMOUNT**:
  cuánto (al activar uno queda en 50 %). La página (0.6.92) muestra los próximos 32 compases de la pista elegida como
  una sola tira, una marca en cada compás donde cae un cambio (naranja: SOUND, violeta: PATTERN), el cabezal encima y
  una línea que dice qué hace cada perilla. **MODE**: **ORBIT** varía alrededor del ancla (la música como estaba al
  activar MUTATE, al dar PLAY, cuando apretás **OCT+** acá: KEEP, o cuando escribiste o borraste pasos por última
  vez), así nunca pierde su identidad y una secuencia que borrás queda borrada; MUTATE nunca llena un patrón vacío; **DRIFT** sigue desde
  como está ahora. SOUND cambia los valores del motor, el filtro, su envolvente, DEC, REL y los envíos (nunca LEVEL, PAN
  ni MUTE); PATTERN mueve notas por la escala, cambia acentos, agrega o saca una nota fuera del tiempo (nunca en el
  tiempo); en DRUM cambia golpes, nunca el bombo. **OCT−** acá (o **GLO + A4**, FREEZE) frena todas las mutaciones
  hasta volver a apretarlo. En HOME cada bichito tiene un anillo de puntos que se llena hasta su próximo cambio y se
  enciende entero cuando cambia.
- **PLAYHEADS** (nivel PATRÓN; antes FUGUE), a la Fugue Machine: tres **cabezales** más (2, 3 y 4) leen los mismos pasos que el
  secuenciador de la pista. KNOB 1 **HEAD** elige uno, KNOB 2 **RATE** (OFF, 1/4, 1/3, 1/2, 2/3, x1, 3/2, x2, x3, x4 del
  de la pista), KNOB 3 **DIR** (FWD, REV, PING, RND), KNOB 4 **INTVL**, su intervalo en grados de la escala (−14 a +14; una
  pista de batería mantiene sus carriles). Un cabezal toca las notas de cada paso con su velocidad, acento y probabilidad
  y el GATE de la pista (sin components, locks ni ratchets: esos son del secuenciador): cánones, contrapunto, patrones
  que se desfasan consigo mismos. La página dibuja los pasos, el cabezal del secuenciador y dónde va cada cabezal. Se
  guarda con la sección; los cabezales arrancan y paran con el transporte.

### Parameter locks

En **SEQ > STEP** (el nivel COMPÁS de LENTE), **mantené un paso** (una tecla de la grilla, o la nota que estás entrando en el roll) y girá una
perilla: ese paso recibe su propio valor del parámetro que esa perilla edita en la última página de sonido que
visitaste. Tocá EDIT mientras mantenés el paso para borrar sus locks. Los eventos de automatización y los locks
comparten **64 registros por sección** (todas las pistas juntas); cuando se acaban, la pantalla dice
**AUTOMATION FULL**.

### Escala

**ROOT, SCALE y QUANT son globales**: una sola escala para todo el tema. En la pista que sea que los fijes (perilla,
capa SCL, MIDI, la carga de un proyecto), todas las pistas melódicas la siguen al instante; las pistas de batería no
tienen. Cada pista conserva su propio TRANS.

- SCALE: CHR, MAJ, MIN, DOR, MIX, PEN, MPEN, HARM, PHRY, LYD, LOC, MEL, BLUES, WHOLE, DIMHW, DIMWH.
- QUANT: OFF; SNAP (las teclas se ajustan a la escala); WHITE (las teclas blancas recorren la escala, las negras no
  suenan); SEQ (como SNAP, y además las notas del secuenciador se ajustan mientras suenan).
- **SCL > ALL TR**, KNOB 4 (0.7.6; hasta entonces en SCL 2; antes se llamaba STRN) mueve todas las secuencias hacia arriba o hacia abajo por la escala (±14
  grados), en todas las pistas a la vez. Es global y se guarda con el proyecto: si tus notas suenan más graves o más
  agudas de lo que muestra el piano roll, fijate acá. Cuando no está en 0 el piano roll muestra **ALL TR** y su valor, y
  al dar PLAY también lo avisa. 0 = apagado.
- La reverb RESO y los pasos de afinación del SLICER también siguen la escala.
- (0.8.3) **Cada sección guarda su tonalidad**: ROOT y SCALE se guardan con cada sección, así que un tema puede cambiar
  de tonalidad de una sección a otra (el PARTS de GEN lo hace en los géneros pop: la última sección un tono arriba).
  Cuando el tema entra a una sección, las teclas, CHORD, ALL TR y la reverb RESO siguen su tonalidad, también con una
  pista de batería seleccionada (entonces cuenta la de la primera pista melódica; hasta 0.8.3 una pista de batería
  seleccionada se quedaba con la tonalidad anterior).

### CHORD (0.6.81)

**SCL > CHORD** hace que una tecla toque un acorde en la pista (inspirado en el Omnichord y el Orchid):

- **CHORD**: OFF; en la escala (todos los acordes afinados, sin pensar): **TRIAD**, **7TH**, **ADD9**, **SUS2**,
  **SUS4**, armados sobre el grado de la tecla (una tecla fuera de la escala toma la nota de la escala de abajo; CHR: la
  escala mayor de ROOT); formas en cualquier tecla: **MAJ**, **MIN**, **7**, **MAJ7**, **MIN7**, **POWER**. En un modo
  de siete notas (MAJ MIN DOR MIX HARM PHRY LYD LOC MEL) el acorde es el propio del modo en ese grado (una nota de la
  escala sí, una no). (0.8.3) En las escalas de cinco, seis u ocho notas (PEN, MPEN, BLUES, WHOLE, DIMHW, DIMWH) saltear
  notas armaba pilas raras (la "tríada" de C pentatónica menor sobre C era C F Bb), así que ahora cada nota es el
  intervalo que pide el acorde donde la escala lo tiene, o si no el más cercano que ofrece: una tercera mayor o menor
  (si no hay: el acorde queda suspendido), una quinta justa (si no, disminuida sobre tercera menor, aumentada sobre
  mayor), una séptima menor o mayor (si no, una sexta), la novena. Todas las notas quedan en la escala: una tonalidad
  pentatónica da tríadas, acordes sus4 / sus2 y sextas (C pentatónica menor: Cm, Eb, Fsus4, Eb/G, Bbsus4; 7TH sobre F:
  F7sus4), la de tonos enteros acordes aumentados, las disminuidas acordes disminuidos y séptimas de dominante.
- **VOICING**: CLOSE (posición fundamental); **LEAD** (cada acorde toma la inversión más cercana al anterior: el voice
  leading, la suavidad del Orchid); OPEN (la segunda nota una octava arriba); +BASS (suma la fundamental una octava
  abajo).
- **STRUM**: 0 todas a la vez; desde ahí, las notas entran una tras otra de grave a agudo (hasta ~63 ms entre ellas),
  como un arpa.

Al soltar la tecla terminan todas las notas del acorde (las que faltaban del strum ya no entran). El acorde se graba en
el paso, en vivo y en la grabación por pasos (0.6.83: la grabación por pasos tomaba solo la nota de la tecla), y el ARP
lo arpegia. Se encienden las teclas de todas las notas que suenan, la forma completa del acorde también: sirve para ver
y aprender los acordes. Con **STRUM** (0.6.84) un acorde grabado entra entero en el paso donde bajaste la tecla (sus
notas siguientes ya no se van al paso de al lado), y el secuenciador rasguea cada paso con acorde de la misma manera,
de grave a agudo (dentro de la mitad del gate del paso). STRUM en 0: los acordes de los pasos como antes, todos juntos. Una pista de batería, MONO, LEGATO y UNISON tocan solo la fundamental. Las notas MIDI
suenan tal cual llegan.

### ARP

ARP: MODE, RATE, OCT (1–4), GATE. ARP 2: SWING, PROB, HOLD, ORDER (NOTE / PLAY). Modos: OFF, UP, DN, UPDN, RND, ORD,
REPEAT, DNUP, UP+8, CONV, DIVG, PINKY, THUMB, WALK, CHORD, **TRNS** (las teclas y el MIDI entrante transponen la
secuencia de la pista según su distancia a C4, sin tocar notas). **Latch de teclas**: con MODE OFF, **HOLD ON** deja
las notas sonando después de soltarlas; el acorde siguiente las reemplaza.

### Borrar

| Para borrar | Hacé |
| --- | --- |
| Los pasos y la automatización de la pista seleccionada | **REC mantenido + F3**, o SAVE > TOOLS > CLEAR SEQ, o SEQ TOOLS F3 |
| Los pasos y la automatización de las seis pistas, y el carril de punch-in | **SEQ mantenido + REC**, después OCT+ |
| **Todo** (NEW PROJECT): cada pista con el sonido por defecto de su motor (seco, FM6 con la voz init), sin pasos ni automatización; mezcla, efectos, tempo y SLICER por defecto; el arreglo vacío, las macros en 0, el kit DRUM-X de fábrica. Quedan los proyectos guardados, los presets de usuario, el MENU, CLOCK, TUNE y MIDI | **MENU > SYSTEM > INIT SYSTEM** o **SAVE > TOOLS > 5 NEW PROJECT**, después OCT+ (UNDO devuelve la música) |
| *(no borra)* Una canción de prueba pesada, para medir la CPU en el equipo y escuchar si hay clics (cada pista viva con acordes de cuatro notas, dos componentes por paso y glides largos, el kit con todos los carriles, todos los FX, 160 BPM) | **SAVE > TOOLS > 6 CPU TEST**, OCT+, después PLAY (UNDO devuelve tu música) |
| Solo la automatización de una parte en la pista seleccionada | **REC mantenido + FX / EDIT / ENV / LFO / SCL / ARP / GLO** |
| La automatización de la pista, conservando los pasos | SEQ TOOLS **F4 CLR AUTO**, o SEQ > AUTOMATION > CLEAR |

Al borrar una pista aparece **T3 CLEARED** con **SEQ+REC: ALL TRACKS** debajo, como recordatorio de la combinación
para todas las pistas.

---

## 7. ARRANGE: canciones, secciones y escenas

### Las piezas

- Un **proyecto** es un tema. Hay **8 proyectos**.
- Cada proyecto tiene **4 secciones, A–D**. Una sección es una foto completa de la música: el sonido, los pasos y la
  automatización de cada pista, el kit DRUM-X y sus mutes, el carril de punch-in, el SLICER, las macros, más el tempo
  y los efectos globales.
- Cada proyecto tiene un **arreglo**: hasta **16 filas**, cada una una sección que suena durante **1–64 compases**,
  opcionalmente con una **escena**, y una regla de **END** (final).

Son 32 secciones en total (8 × 4).

### La página ARRANGE (SAVE > ARRANGE)

ARRANGE es el segundo juego del nivel PROJECT de SAVE (0.8.5): arriba las cuatro secciones del proyecto en grande
(guardadas: en su color, la que suena dice **NOW**), después **ARRANGE**: las filas del tema como fichas (**A16**: la
sección y sus compases, en el color de la sección; una escena: un punto blanco; la fila en la que estás encendida y
enmarcada), y debajo todo el tema como una sola tira (sus filas según su largo, una línea blanca donde suena) con su
largo y su END (**120 BARS · LOOP**).

| Perilla | Sobre una fila | Sobre + ADD | Sobre END |
| --- | --- | --- | --- |
| KNOB 1 (o PRESETS) | elige la fila | | |
| KNOB 2 | su sección A–D; girada a la izquierda más allá de A: **DELETE SONG ROW?** | agrega una fila (misma sección que la fila de arriba, 1 compás) | **STOP**, **x2 … x8**, **LOOP** |
| OCT+ | agrega una fila después de la última (su sección y sus compases) y la elige (0.8.5) | | |
| KNOB 3 | sus compases (1–64) | agrega una fila | |
| KNOB 4 | derecha: guarda una **escena** (SCENE STORED); izquierda: la borra | | |

- **PLAY** en ARRANGE reproduce el tema desde el principio; otra vez: para. (Hasta la 0.8.4 OCT+ también; ahora agrega una fila.)
- Cuando el tema termina (o lo parás), vuelve la música que tenías antes de apretar PLAY (SONG END).
- END: **STOP** reproduce el tema una vez, **x2–x8** esa cantidad de veces, **LOOP** para siempre.
- Si una fila apunta a una sección vacía, el tema no arranca: la pantalla dice **PATTERN A EMPTY** (o B, C, D).
- Mientras suena un tema, la edición de pasos está bloqueada (**STOP TO EDIT**).
- Para borrar una fila también podés usar SAVE > TOOLS > DELETE ROW (la fila que tiene seleccionada ARRANGE). Para
  borrar todo el arreglo: TOOLS > CLEAR SONG (**CLEAR SONG ORDER?**). Ninguna de las dos cosas se puede deshacer.
- El arreglo se guarda automáticamente (unos 2 segundos después de un cambio, con el transporte parado). Ver
  [12](#12-guardar-y-cargar).

### Escenas

Una escena se guarda por fila y, cuando esa fila arranca, recupera: los mutes de pista (T1–T6), los mutes de grupo de
DRUM-X, las macros M1–M4 y los efectos de nota punch-in que estén mantenidos. Sigue vigente durante las filas
siguientes que no tengan escena. Las escenas se aplican cuando suena un tema, no en una cadena rápida. SONG REC también
guarda una por fila a medida que empieza cada una.

### Tocar secciones en vivo (capa SAVE)

Mantené **SAVE**:

- **F3–B3**: tocar la sección A–D. Si está sonando, entra en el **compás siguiente**; si está parado, se carga al
  instante.
- Tocá **dos o más** de F3–B3 en una misma mantenida: una **cadena rápida** — suenan por turno, cada una durante el
  largo de su patrón más largo, una y otra vez, empezando en el compás siguiente (parado: al instante).
- **C4–F4**: **guardar** la música actual en A–D (funciona mientras suena; ver [12](#12-guardar-y-cargar) para saber
  cuándo llega a la flash).
- **OCT+**: guardar en la sección que está sonando (PLAY A SECTION FIRST si no hay ninguna). **OCT−**: recuperarla tal
  como quedó guardada.
- **D5**: **LOOP / SONG** — en modo SONG, PLAY desde cualquier página reproduce el arreglo.
- **E5**: **SONG REC** — las secciones que tocás en vivo (y sus compases) se escriben en las filas del arreglo, con
  una escena cada una. Reemplaza las filas actuales.
- **G5**: saltar a ARRANGE.
- **KNOB 1**: elegir otro proyecto; cambia al soltar SAVE (solo parado) y **carga su sección A**.

---

## 8. Dados y UNDO

### GEN — el generador de temas (HOME + SELECT)

En **HOME**, girá **SELECT** dos pasos a la derecha (0.8.5: ECO → BEING → GEN, la lente de HOME), o mantené HOME y
girá SELECT, desde cualquier página. El **GENERATOR** se abre como el último nivel de HOME: tres páginas de ajustes,
un **toque de HOME** cambia de página (0.8.5; antes SELECT), **KNOB 1–4** fijan los cuatro valores que se ven
(**PRESETS**: el GENRE), **OCT+** arma la pieza y **OCT−** cierra la pantalla (la pieza queda; **UNDO**, GLO + G4,
devuelve lo que había antes); **SELECT** a la izquierda vuelve a BEING. El panel muestra las cinco partes como filas
(las que pesan las perillas de la página, encendidas) con un adelanto de su densidad y su calor, las secciones de la
FORM como fichas de colores, la última pieza y **OCT+ MAKE**. **PLAY** arranca y para el transporte sin salir de la
pantalla (0.6.97).

| Página | KNOB 1 | KNOB 2 | KNOB 3 | KNOB 4 |
|---|---|---|---|---|
| 1/3 | **GENRE**: RANDOM (la pantalla abre ahí; la mitad de sus tiradas son mutantes de dos géneros, con nombre A+B) o uno de 43 | **SEED** 0000–9999 | **ENERGY** 0–100 % | **FORM**: LOOP, PARTS, SONG |
| 2/3 | **DRUMS** | **BASS** | **CHORDS** | **ARP** — cada uno OFF, 10–100 %, o **AUTO** (lo que pide el género) |
| 3/3 | **LEAD** (igual) | **COMPLEX** 0–100 % | **SWING** AUTO o 0–100 % | **HUMAN** 0–100 % |

La pieza es **determinista**: los mismos ajustes dan siempre la misma pieza, así que una semilla que te gustó se
puede volver a marcar. **OCT+ con los ajustes sin tocar tira una semilla nueva** (otra pieza); girá una perilla y
apretá OCT+ y tenés **la misma pieza con esa sola cosa cambiada**: cada parte saca sus números al azar de su propio
flujo, así que mover BASS deja la batería y el lead como estaban. La pantalla muestra **LAST: HOUSE 0421** (el género
y la semilla).

Los géneros: HOUSE, DEEP HOUSE, MICRO HOUSE, TECH HOUSE, ACID HOUSE, TECHNO, DETROIT, DUB TECHNO, HARD TECHNO,
HYPNOTIC, TRANCE, ELECTRO, EBM, ITALO, SYNTH POP, DARKWAVE, POST PUNK, TRIBAL PUNK, MINIMAL SYNTH, MAN MACHINE,
MOTORIK, INDIE, SHOEGAZE, POST ROCK, FUNK, DISCO, BOOM BAP, TRAP, UK GARAGE, DUBSTEP, BREAKS, DRUM & BASS, DUB,
STEPPERS, JAZZ, BOSSA, AFROBEAT, AMBIENT, NEO CLASSIC, IMPRESSION, SOUNDTRACK, IDM, BROKEN. Cada uno sabe su rango de
tempo, sus escalas y progresiones, su mapa de batería y su kit, sus arquetipos de bajo y su registro, cómo hablan sus
acordes, su arpegio y su lead, qué receta de sonido toma cada rol, qué tan humano toca y su espacio (tipo de reverb,
tipo y tiempo del delay, cuánta humedad).

De qué está hecha una pieza:

- **Batería** a partir de un **mapa de prioridades** (como el Grids de Mutable Instruments): cada paso de cada carril
  tiene un nivel, y suena donde el nivel supera la densidad que fija DRUMS; al bajarla se van primero los ghosts y al
  final las anclas del bombo. Después los ghost notes del redoblante en las mejores posiciones (primero la
  semicorchea antes del 2 y del 4), la célula de acentos de los hats (house, funk, garage, máquina), una pequeña
  variación en los compases pares (COMPLEX), el **fill** del género en el último compás de la frase (redoble, corrida
  de toms, el bombo que se va, redoble de hats, fill lineal de funk, los bombos en semicorcheas de Blue Monday, un rim
  de dub, un corte antes del uno) y el redoblante atrasado donde el género toca atrás (TIMING). ENERGY y las
  cantidades deciden cuánto suena; el bombo nunca se cae del uno.
- **La tonalidad** (0.8.3): un modo de siete notas para cada género (de su lista: mayor, menor, dórico, mixolidio,
  frigio, lidio, menor armónica y melódica), **escrito en la página SCALE** (ROOT, SCALE, y QUANT SNAP salvo que hayas
  elegido otro QUANT) en todas las pistas, así que las teclas que toques después, la página CHORD, ALL TR y la reverb
  RESO están en la tonalidad de la pieza. Un género que canta en pentatónica o blues (deep house, funk, boom bap, D&B,
  afrobeat ..) recibe el modo que la contiene (dórico o eólico; mayor para la pentatónica mayor) y su **lead** se queda
  en las notas de la pentatónica: los acordes siguen siendo acordes de verdad y todas las notas siguen en la escala de
  la página SCALE. La **página CHORD** de la pista de acordes queda con los acordes de la pieza (TRIAD, 7TH, ADD9, SUS2
  o SUS4, según cómo esté coloreada su progresión), VOICING LEAD, un strum donde los toca una guitarra; CHORD queda
  apagado en las demás pistas.
- **Armonía** (0.8.3): cada género tiene una familia (loops modales; loops de séptimas; pop funcional; colores
  lavados; cine; jazz) y progresiones escritas como grados del modo, cada loop terminando en el acorde que vuelve al
  primero (IV–I, bVII–i, V–i, bII–i ..): i bVII, i iv7, i bII (frigio), I V vi IV, vi IV I V, I vi IV V, i bVI bIII
  bVII, la andaluza i bVII bVI V, la eólica bVI bVII i, i7 bIIImaj7 bVII7 iv7, ii7 V7 Imaj7, Imaj7 vi7 ii7 V7, la de
  bossa Imaj7 II7 ii7 V7, I bVII IV I .. Un modo que volvería disminuido uno de sus acordes (el vi del dórico, el #iv
  del lidio) nunca toma esa progresión: la siguiente del género, si no el loop propio del modo. Después las cadencias y
  colores que el género permite: la **dominante de la tonalidad menor subida antes de su tónica** (la sensible: trance,
  italo, synth pop, cine, jazz), el **turnaround del jazz** (ii–V en el último compás), una **dominante secundaria** en
  la segunda mitad del acorde anterior a su destino (V/vi, V/ii, V/IV: indie, disco, jazz, cine; con COMPLEX), el **IV
  que se vuelve menor** antes del I en mayor (mixtura: synth pop, indie, shoegaze, cine), el lado bemol prestado en
  mayor (bVII, bVI: motorik, indie, soundtrack). Ritmo armónico por género: un acorde por compás, cada dos o cuatro
  compases, o un solo acorde para todo el loop. Los acordes son tríadas, séptimas, novenas (nunca una b9), add9, sus2 /
  sus4 (solo con cuarta justa), sextas, pilas cuartales, power chords, recoloreados por el género (simples, ricos,
  suspendidos, cuartales, power, paralelos).
- **Bajo** a partir de unos cincuenta **arquetipos escritos contra el bombo**: offbeat y bounce de house, walking de
  semicorcheas deep, semicorcheas rodantes y rebote de octava de techno, Detroit, EBM, italo, psy, UK garage,
  Jamerson, alrededor del bombo, on the one, slap, Good Times, walking y two-feel de jazz, boogie, country, ska,
  tumbao, bossa, corcheas de rock, semicorcheas punk, McCartney, Peter Hook, motorik, post punk angular, indie, one
  drop, steppers, rockers, dub, boom bap, 808 de trap, DnB, jungle, wobble, pedal, quintas, Suicide, Kraftwerk; y
  **acid** con las reglas de la 303 (acentos en 3–5 pasos, slides en 2–3, un salto de octava después de un slide, los
  acentos variando por compás). **La raíz en cada cambio de acorde** (un acorde que entra a mitad de compás tiene su raíz
  ahí), la tercera, la quinta y la séptima del arquetipo las del acorde (también las de un acorde prestado o con la
  sensible), aproximaciones por semitono a la raíz siguiente en un tiempo débil (siempre resuelven: un bajo más fino
  nunca deja una colgada), un pickup al final del cuarto compás; el bombo **doblado**, **respondido** (el bajo no pisa
  los pasos del bombo), **entrelazado** (y una nota justo después) o sostenido debajo, según el género.
- **Acordes voiceados** (0.8.3): de las inversiones y aperturas de cada acorde, la más cercana al acorde anterior, voz
  por voz (las notas comunes sostenidas, el menor movimiento: unos 4,6 semitonos por cambio en total, contra 5,5 antes),
  en C3..G5, sin segundas debajo de G3 ni una tercera cerrada en el fondo; cerrados en la mano para stabs y comping,
  **abiertos** (drop 2) para los colores lavados y el cine, **sin fundamental** (3 5 7 9) donde el jazz y el deep house
  tienen el bajo en la raíz, movidos en paralelo para el techno y el impresionismo; tocados sostenidos, creciendo, en
  stabs a contratiempo, pulsados, dubeados por el delay, en skank, comping, con empuje, como drone, escasos, broken (UK
  garage) o con gate de trance, cada uno sostenido hasta el final de su acorde.
- **Arpegio**: un orden sobre el acorde voiceado — arriba, arriba y abajo, abajo, Alberti, los grupos de Glass, un
  pulso de Reich, un secuenciador, una caminata, ciclos de 3 o 5 contra el compás (polimetría), un gate de trance,
  octavas — en semicorcheas o corcheas, con huecos (ARP), en D3..D5; la figura de cada acorde desde su primera nota;
  solo las notas del acorde.
- **Lead** (0.8.3: compuesto, no caminado): un **motivo** de un compás — una célula rítmica (una nota larga y dos cortas,
  un tresillo, un pickup al tres, un empuje con puntillo ..) y un contorno de pasos (una tercera de vez en cuando, un
  salto devuelto por un paso, un arco, una caída, una subida o una onda) — desarrollado en la **frase** de cuatro
  compases por una plantilla: un **período** (antecedente que cierra abierto en la 2.ª o la 5.ª, consecuente que cierra
  en la tónica), una **oración** (el motivo, su secuencia, una continuación que sube al clímax a dos tercios, la
  cadencia), **A A B A'**, **pregunta y respuesta** (la respuesta con el contorno de la pregunta dado vuelta), el **hook**
  de techno A A A A', o la del dub con un compás de aire; un operador en un compás variado (su última nota omitida, su
  final movido, una bordadura hacia su última nota). El motivo pasa al acorde de cada compás por conducción de voces
  (su primera nota la nota del acorde a un paso de la última; el mismo acorde: el motivo como era). Las reglas de todo
  lead: **una nota del acorde en los tiempos 1 y 3**, en el 2 y el 4 una nota del acorde o una de paso que se mueve por
  pasos, notas de paso y bordaduras entre medio, **mayormente pasos** (el 60 % de sus movimientos), nunca más de una
  octava, un salto mayor que una cuarta seguido de un paso de vuelta, un ámbito de una décima más o menos, la frase
  **cerrando en un grado estable** (la tónica, la tercera o la quinta sobre una nota del acorde) y sostenida, ni novena
  menor ni tritono sobre la raíz del acorde, ninguna nota a evitar sostenida (una cuarta sobre una tercera mayor), las
  notas de un acorde prestado o con la sensible respetadas (la b7 de la tonalidad menor se vuelve sensible sobre su V).
  O si no: un **ostinato** hipnótico sobre un euclidiano (las notas de su célula en el acorde en los tiempos), una figura
  de máquina que se mueve con el acorde, **notas largas** como una línea de notas guía (cada una la nota del acorde a un
  paso o una tercera, de vez en cuando una figura corta que entra por pasos), una línea **tintinnabuli** (por pasos, una
  nota del acorde en cada nota larga, una de paso entre medio). Nunca por encima de A5 (B5 en una sección subida un
  tono); los agudos más suaves. Cada lead, figura y patrón de batería es propio del género para una semilla (la misma
  semilla en dos géneros: dos temas).
- **Sonidos** por **receta** según el rol: sub, acid, Reese, bajo FM, bajo de rock, el bajo con chorus de Hook, wow
  bass; pad, cuerdas, coro, órgano (WHEEL), piano eléctrico, stab, acorde de dub, pluck, campana, lead, lead vocal,
  chip, teclas, drone, ruido, textura, hoover, un muro shoegaze, una guitarra con trémolo. Una receta elige un motor,
  un sonido de fábrica cuyo nombre encaja, mueve un poco sus parámetros, da forma a la envolvente, al filtro (tipo,
  corte, resonancia, la envolvente sobre él), un LFO lento, saturación, chorus, glide, el modo de voz. El **kit** es
  uno de siete (909, 808, click, industrial, suave, dub, breaks) escrito sobre el kit DRUM-X de fábrica, con el TONE,
  DECAY, NOISE y DRIVE de la pista de batería.
- **Mezcla**: nivel, paneo y envíos por rol y según la humedad del género; **HUMAN** fija el HUMAN de SEQ > TRACK en cada pista
  según el rol (hats y leads más, el bombo nada) escalado por qué tan humano es el género (una banda más que una
  máquina); SWING el del género salvo que lo fijes; DUCK, CLIP y PUNCH en el master; el TYPE, PING-PONG y TIME del
  delay y el tipo de reverb, los del género.
- Con **TRACKS 6** la sexta pista es una **textura** (un drone, ruido, un coro, un órgano, cuerdas) en notas largas.
- **FORM**: **LOOP** escribe un loop (2 o 4 compases). **PARTS** además guarda las secciones **A** (la pieza), **B**
  (un break: sin batería ni bajo) y **C** (un drop: la batería más densa) y llena ARRANGE con A B A C (PLAY toca el
  tema, como hacía el DICE ALL de 0.6.81). (0.8.3) Las secciones son una misma pieza en una tonalidad: B empieza el
  loop en su predominante (los mismos acordes rotados: IV–I–V–vi para I–V–vi–IV; un loop de un acorde va a su
  subdominante y vuelve), el motivo y las figuras los mismos, llevados a esos acordes; en los géneros pop (ITALO, SYNTH
  POP, INDIE, DISCO) **C está un tono arriba**, un cambio de tonalidad real guardado en la sección con su página SCALE (el
  último estribillo del pop). Los loops modales (techno, house minimal, dub ..) conservan su loop en todas las secciones.
  **SONG** escribe una forma de club: **A** el drop, **B** la intro (batería y
  bajo), **C** el build (entran los acordes, su filtro se abre a lo largo del loop, un redoble al final, el delay
  lanzado en el último paso) y **D** el break (sin batería ni bajo, los acordes más húmedos; 0.8.3: en su predominante,
  salvo en los loops modales), en ARRANGE como B C A D C A con filas de 16 compases (el drop 32). Los géneros ambient y
  post-rock reciben un arco (fino, entra, ápice: en su predominante). Las secciones tuyas (guardadas por vos, no por GEN) nunca se pisan: entonces la pieza queda en un loop.

Cada pieza reemplaza los pasos, sonidos, kit, mezcla y automatización de todas las pistas. Los mutes quedan como
estaban. Los roles que suenan van en las primeras pistas (la batería en la pista DRUM), así que con TRACKS 4 escuchás
los más importantes.

### Dados de sonido (EDIT + SELECT)

Mantené **EDIT** y girá SELECT (desde cualquier página): la pista seleccionada carga un preset de fábrica al azar de su motor y cada
parámetro del motor se mueve hasta un cuarto de su rango. En DRUM, los dos lados de cada sonido del kit se tiran
alrededor del kit de fábrica. Lo escuchás al instante bajo **KEEP DICED SOUND?** (DRUM: **KEEP DICED KIT?**): girá
SELECT otra vez para otra tirada, **OCT+ OK** la deja, **OCT− CANCEL** devuelve el sonido que tenías antes de la
primera tirada (solo el sonido de esa pista: la música sigue). Después de OK, **GLO mantenido + G4** también lo deshace.

### Dados de secuencia, LFO y FX (SEQ / LFO / FX + SELECT)

La misma idea para las otras partes de la pista seleccionada, cada una se escucha al instante bajo su diálogo **KEEP
DICED …?** (SELECT otra vez: otra tirada, **OCT+ OK** la deja, **OCT− CANCEL** devuelve lo que había, el resto sigue
sonando):

- **SEQ + SELECT**: una secuencia nueva como escribe una parte DICE ALL, en la escala de la pista: un beat en una pista
  DRUM; si no, el rol al que suenan sus pasos (notas graves: un bajo, acordes: un pad, si no una lead o un arpegio). Su
  sonido, su mezcla y su automatización quedan.
- **LFO + SELECT**: el LFO de la pista: un rate en el medio del rango, cualquier onda y fase, a veces un fade, apuntado
  sobre todo al filtro, un poco a la forma, rara vez al pitch o al volumen.
- **FX + SELECT**: el drive de la pista (la mitad de las veces nada), sus envíos de chorus, delay y reverb, un AUTO PAN
  una vez de cada tres, y el tipo, tamaño y damping del reverb compartido y el tiempo y feedback del delay (con pitch una
  vez de cada cinco).

### Dados de macros

Con las macros abiertas (LFO mantenido) o enganchadas: **F3** tira rutas de macro nuevas en las pistas que están
sonando; G3 las borra. Ver [10](#10-modulación-y-macros).

### UNDO (GLO mantenido + G4)

Un paso atrás sobre el **último cambio grande**. Apretarlo otra vez va hacia adelante (rehacer). El transporte sigue
sonando. G4 "respira" en el mapa de GLO cuando hay algo para deshacer; si no hay nada, dice **NOTHING TO UNDO**.

**UNDO cubre**: DICE ALL, un sonido tirado a los dados, una pista borrada (REC F3, TOOLS CLEAR SEQ), CLEAR ALL
SEQUENCES (SEQ + REC), NEW PROJECT, INIT y RECALL (capa EDIT o TOOLS), borrar la automatización
(SEQ > AUTOMATION) y una sección cargada con **SAVE > PROJECT > LOAD**. Restaura la música tal como estaba: sonidos,
pasos, automatización, el kit, el carril de punch-in, las macros, el tempo y los efectos globales.

**UNDO no cubre**: saltos de sección, cadenas rápidas y RECALL desde la capa SAVE, cambiar de proyecto con KNOB 1 de
la capa SAVE, CLEAR SONG, borrar una fila, SONG REC, nada de lo guardado (secciones, presets de usuario), cargas de
presets de usuario, sonidos elegidos con PRESETS, acciones de SEQ TOOLS, ni giros comunes de perilla. No está
disponible mientras suena un tema (**STOP TO EDIT**). Guarda un solo paso, en RAM: se pierde al apagar.

---

## 9. Efectos y master

### La lente de FX: TRACK · BUSES · EFFECT

Desde la 0.8.5 FX funciona como la lente del secuenciador y se dibuja como el diseño aprobado: tres pestañas de nivel,
**TRACK · BUSES · EFFECT** (pista, buses, efecto); **SELECT** hace zoom entre ellas (sin dar la vuelta). Las páginas y
sus perillas son las de siempre, más una: **BUSES**.

| Nivel | Qué muestra la pantalla | Un toque de FX | PRESETS |
| --- | --- | --- | --- |
| **TRACK** | El juego de la pista: su lugar en puntitos y "DIST · T1" arriba, su dibujo (los cuatro envíos como barras, la curva de DIST con TYPE y TONE, el vaivén de AUTO PAN con WAVE y RATE) y la curva de DIST en chiquito debajo. | **SENDS** (la página FX: DIST CHORUS DLY REV) → **DIST** → **AUTO PAN** → SENDS | el sonido (como en todos lados) |
| **BUSES** | Los seis módulos como fichas, cada uno con su dibujo en chiquito: **DELAY** (sus ecos), **REVERB** (su caída), **CHORUS** (tres ondas), **SLICER** (sus 16 pasos, el que suena enmarcado), **MASTER** (la salida ahora, la curva del duck), **PUNCH** (el carril). El módulo elegido teñido y enmarcado en violeta; con LOAD ECO CHORUS y SLICER quedan oscuros ("ECO") y se saltean. Perillas: **DLY MIX, REV SIZE, CHORUS DEPTH, SLICER DEPTH**. | abre el módulo elegido (EFFECT) | el módulo (sin dar la vuelta) |
| **EFFECT** | El módulo elegido en grande: sus páginas en puntitos, "DELAY · DLY 2" a la derecha, su dibujo. | su página siguiente, en ronda (DLY → DLY 2 → DLY 3 → DLY; REVERB → REVERB 2; SLICER → SL BUS → SL BUS 2 → SL PITCH) | el módulo siguiente, en su primera página |

- **FX desde otra página** vuelve a la página FX que dejaste. SELECT desde EFFECT vuelve a BUSES (el módulo queda
  elegido), desde BUSES a TRACK en el juego en el que lo dejaste.
- El pie dice **FX · BUSES** (la lente, el nivel); el encabezado muestra la pista, **T1**, en naranja.
- Los dibujos siguen a los valores (un giro los redibuja); el nivel de MASTER se mueve con el sonido.

### Por pista

- **FX**: cantidad de DIST y los tres envíos (CHORUS, DLY, REV).
- **DIST**: cantidad, **TYPE** (SOFT, HARD, FOLD, CRUSH, RECT, **AMP**), **TONE** (más oscuro / más brillante). **AMP**
  es un amplificador de guitarra: dos etapas de válvula que saturan desparejo (DIST bajo limpio, después crunch, después
  lead), el bias respirando con el nivel, y un bafle con su pico de presencia; TONE mueve el brillo del bafle.
- **AUTO PAN**: el paneo de la pista oscilando a izquierda y derecha alrededor de su PAN. **DEPTH** (0 = apagado),
  **RATE**, **SYNC** (BEAT: RATE en divisiones 4BAR … 1/32, sincronizado al compás; FREE: RATE en Hz), **WAVE** (SIN,
  TRI, SAW, SQR, S&H).
- **FILTER**: ver [4](#4-motores).

### SLICER (uno solo, en un bus)

- **SLICER**: **MODE** OFF, GATE, STUT; **PAT** 1–16; **RATE** 1/8 … 32T; **DEPTH**.
- **SL BUS / SL BUS 2**: ON / OFF para lo que entra: **T1–T6** y **FX** (los retornos de delay, reverb y chorus).
  Con todo en ON corta toda la mezcla después de los efectos; con FX en OFF las colas suenan libres.
- **SL PITCH**: las repeticiones siguen una secuencia de afinación de 16 pasos (±24 semitonos). KNOB 1 el paso,
  KNOB 2 su afinación, LEN, QUANT (CHR o SCL: la escala del tema).
- Cada sección conserva su propia configuración del SLICER. Con **LOAD ECO** no hay SLICER (sus pistas van directo a
  la mezcla).

### Delay

- **DLY**: **TIME** (divisiones sincronizadas 4BAR … 1/32, o tiempos libres de 5 ms a 2,95 s), **FEEDBK**, **TONE**
  (por debajo del centro más oscuro, por encima más fino), **MIX**. FEEDBK se comporta como el de un delay analógico
  (una cinta, un PT2399; 0.6.86, 0.6.89): con **TONE al centro (50 %)** cada repetición es una copia de lo que entró;
  en **100 %** los ecos mantienen su volumen mucho tiempo (bajan muy de a poco y nunca se cortan), y **pasado 100 %**
  se realimentan y se sostienen, saturando suave en vez de recortar (bajalo para que se apaguen). Hasta 80 % es igual
  que antes. **TONE** es resonante: del centro hacia abajo un pasabajos que barre de ~8 kHz a ~250 Hz, del centro
  hacia arriba un corte de graves que barre de ~70 Hz a ~3 kHz, los dos con un pico en el corte: las repeticiones se
  oscurecen o afinan y se angostan a una banda en cada pasada (estilo dub), y en los extremos con FEEDBK alto el lazo
  autooscila en esa banda.
- **Tiempos largos** (0.8.4): cada división suena con su largo real hasta **2,97 s**, el máximo (antes todo lo que
  pasaba de 1,49 s se cortaba a 1,49 s: 1/2 por debajo de 81 BPM, 1/1 por debajo de 162 BPM, 2BAR, 4BAR). Para que
  entren, la línea del delay corre a tres velocidades, que elige el TIME solo:

  | TIME | Línea | Agudos de las repeticiones |
  | --- | --- | --- |
  | hasta 0,74 s | velocidad completa | todo el rango, como antes |
  | 0,74 s – 1,49 s | media velocidad | planos hasta ~9,5 kHz, -3 dB en ~10,4 kHz |
  | 1,49 s – 2,97 s | un cuarto | planos hasta ~4,7 kHz, -3 dB en ~5,2 kHz |

  Así, cuanto más largo el tiempo, más oscura la cola, como en un delay de cinta o de BBD. Solo se pierden agudos:
  no hay suciedad ni aliasing (lo que se repliega queda más de 70 dB abajo) ni ruido de más. La velocidad se aplica
  una sola vez, así que las repeticiones no se oscurecen en cada pasada (eso lo hacen solo TONE y FEEDBK). MOD, el
  delay GRAIN, PING, WIDE, TYPE y TONE funcionan igual en todas las velocidades. En las dos velocidades más lentas el
  primer eco llega ~1 ms (media) o ~4 ms (un cuarto) tarde; las repeticiones siguen exactamente a TIME entre sí.
  Cuando TIME cruza 0,74 s o 1,49 s (perilla, tempo, automatización, carga de proyecto o sección, reloj externo) el
  delay se apaga en 3 ms y vuelve a empezar vacío. Queda un hueco corto: los ecos que había en la línea se pierden,
  pero no hay clic. Un reloj que oscila alrededor de un límite no lo reinicia una y otra vez. Los ecos largos vuelven
  a pleno volumen aunque no suene nada más.
- **DLY 2**: **WIDE** (el eco derecho más tarde), y el delay **GRAIN**: **PITCH** (±12 semitonos por repetición) y
  **SPRAY**. Con LOAD LITE o ECO el delay GRAIN suena como un delay común (la tarjeta dice LITE / ECO). **MOD**
  (0.6.86): la deriva de la cinta, WOW lento y FLUTTER rápido sobre las repeticiones (0, por defecto: nada, copias
  exactas; ~20 como una cinta; hasta un chorus mareado); las repeticiones siguen brillantes mientras derivan (0.6.89);
  se guarda con el proyecto.
- **DLY 3** (0.6.90): **TYPE**: **ANALOG** (por defecto: la saturación suave, el TONE resonante, FEEDBK pasado 100 %
  autooscila) o **DIGITAL** (repeticiones limpias, TONE un filtro común, FEEDBK 100 % como máximo: pasado eso las
  repeticiones se mantienen, nunca crecen). **PING**: **ON** manda las repeticiones izquierda, derecha, izquierda … por
  el estéreo, cada una FEEDBK más suave (TIME hasta 1,49 s desde 0.8.4, el lado derecho al doble de TIME; sin WIDE ni el delay GRAIN). Se guarda con el proyecto.

### Reverb (cinco modelos)

**REVERB > TYPE** elige el modelo; **SIZE** es el decaimiento en todos los modelos. KNOB 3 y 4 cambian de significado
según el tipo:

| TYPE | Qué es | KNOB 3 | KNOB 4 |
| --- | --- | --- | --- |
| **ROOM** | Sala clásica | DAMP | PRE (pre-delay, ms) |
| **SPRING** | Tanque de resortes de amplificador | DAMP | PRE |
| **SHIMR** (shimmer) | Sube una octava en cada pasada | DAMP | **AIR** (±24 st: más oscuro y grave / más aireado y agudo) |
| **RESO** | Cuatro cuerdas afinadas a la escala del tema | **FOLLOW** (OFF / T1–T6: el acorde sigue la nota más grave que toca esa pista) | **PITCH** (el acorde 0–14 grados hacia arriba en la escala) |
| **CLOUD** | Nube granular; SIZE al máximo la congela | DAMP | **AIR** |

**REVERB 2**: **MOD** (profundidad de modulación), **RATE**, **FILT** (tono de entrada: más oscuro / más brillante;
0.6.86 resonante, una banda en el corte para tallar el lugar del espacio), **WIDE**.

Con **LOAD LITE**, SHIMMER y CLOUD suenan como ROOM y RESO como SPRING; con **LOAD ECO** todas las reverbs suenan como
SPRING. La configuración se conserva; la tarjeta TYPE muestra LITE o ECO debajo de su valor.

Cambiar el modelo (la perilla, la automatización o el motion, un proyecto o una sección de canción, MIDI, el editor, o
LOAD pasando a LITE / ECO y de vuelta) corta la cola del modelo anterior con un fundido breve (menos de un milisegundo,
sin clic) y el nuevo empieza en silencio. Desde 0.8.4, volver a **CLOUD** empieza con la nube vacía, también una
congelada (antes retomaba la nube que había quedado).

### Chorus

**CHORUS**: RATE, DEPTH, **STEREO** (0 como siempre, hacia abajo hasta mono, hacia arriba hasta casi el doble de
ancho). Se oculta con LOAD ECO (la tarjeta de envío de chorus de una pista dice entonces ECO).

### MASTER

- **CLIP**: saturación que agrega carácter, no volumen.
- **PUNCH**: engorda las pistas DRUM.
- **DUCK**: el bombo baja todo lo demás; **REL** es su release.

---

## 10. Modulación y macros

- **ENV DEST**: la envolvente hacia FILTER, PITCH, SHAPE; **LOOP ON**: con la nota mantenida, la envolvente vuelve al
  ataque cuando llega al sustain (un LFO con forma de ADSR).
- **LFO**: RATE, WAVE (SIN, TRI, SAW, SQR, S&H), PHASE, FADE. **LFO DEST**: PITCH, FILTER, SHAPE, AMP.
  - **RATE** es libre en Hz; girándolo a la izquierda pasado el más lento queda **a tempo** (4BAR, 2BAR, 1BAR, 1/2,
    1/4, 1/8D, 1/4T, 1/8, 1/8T, 1/16, 1/16T, 1/32), enganchado al compás mientras el transporte toca (PHASE es su
    desfase).
  - **PHASE** es dónde reinicia el LFO una frase nueva; girándolo a la izquierda pasado 0 queda en **FREE**: nunca se
    reinicia, corre libre.
- **Matriz MOD**: 4 slots por pista (KNOB 1 elige el slot).
  - Fuentes: OFF, LFO, ENV, VEL, KEY, RAND, MODW (MIDI CC1), AT (aftertouch), EXPR (CC11), **M1–M4**, **STEP** (MSEQ), **NOISE** (un vagar aleatorio suave, un valor nuevo cada
    ~46 ms: un temblor vivo sobre el filtro, el paneo, un envío).
  - Destinos: PITCH, CUT, SHP, AMP, PAN, DIST, CHO, DLY, REV, RATE (velocidad del LFO), VIB, **E1–E8** (las perillas
    EDIT propias del motor, mostradas por nombre), **CLIP** y **PNCH** (el master), y (0.7.0) los buses de FX: **D.TIM D.FB
    D.TON D.MIX** (DELAY TIME, FDBK, TONE, MIX), **R.SIZ R.DMP** (REVERB SIZE, DAMP), **C.RAT C.DEP** (CHORUS RATE,
    DEPTH). AMT 64 con la fuente a tope = todo el rango del parámetro; los slots de todas las pistas sobre el mismo bus se
    suman. Una fuente STEP (la secuencia de modulación) sobre D.TIM secuencia el tiempo del delay.
- **MSEQ**: un secuenciador de modulación de 16 pasos por pista, como un secuenciador de CV. KNOB 1 elige el paso,
  KNOB 2 fija su nivel, LEN y SLEW; DIV en MSEQ 2. Usalo desde MOD con SRC **STEP**.

### Macros M1–M4

- **Mantené LFO**: KNOB 1–4 pasan a ser M1–M4, y el mapa muestra a dónde va cada una ("NO ROUTE: MOD SRC" si no van a
  ningún lado).
- Soltá: quedan **enganchadas** — KNOB 1–4 siguen moviendo las macros en cualquier página — hasta que apretás LFO de
  nuevo.
- Cada encendido arranca con rutas al azar (dos por pista) en las pistas que no tienen ninguna. **F3**
  tira rutas nuevas en las pistas que están sonando; **G3** las borra.
- Las rutas son slots MOD comunes (SRC M1–M4): editalas a mano en LFO > MOD. Los valores de las macros se guardan con
  cada sección.

---

## 11. Tocar en vivo

### FX punch-in (mantené FX)

Efectos que actúan mientras mantenés su tecla y vuelven exactamente atrás cuando la soltás. **FX > PUNCH** (0.6.99)
es su secuenciador: el carril que los graba. **LEN** su largo, 1 … 64 pasos; **RES** el paso, 1/8, 1/16 o 1/32 de
tiempo (64 pasos de 1/16 son cuatro compases, de 1/32 dos, de 1/8 ocho); el KNOB 3 dice qué está haciendo (REC
mientras graba, PLAY mientras suena, si no cuántos pasos tienen un efecto); **CLEAR** (KNOB 4 y OCT+) lo vacía. La
página dibuja las celdas del carril con el color de cada efecto y el cabezal encima, el nombre del último efecto y
las pistas sobre las que actúa (ALL, SYN, DRM). Para grabar: REC armado y tocando (no en una canción), mantené FX y
la tecla del efecto; los pasos que pasa lo reciben; **G5** sostenido borra por donde pasa el cabezal. Se guarda con
la sección.

- **Sobre el audio**: REPEAT 1/8, 1/16, 1/32 (F3 G3 A3), LPF (B3), HPF (C4). REPEAT arranca en la siguiente 1/16
  mientras suena. Por debajo de unos 81 BPM, REPEAT 1/8 es demasiado largo para su buffer y aparece atenuado.
- **Sobre las notas**: OCT− / OCT+ (D4 E4), 1/2 TEMPO (F4), DEC− (corto, punteado) / DEC+ (largo) (G4 A4), STUTTER
  1/8 / 1/16 (B4 C5), ATK+ (ataques lentos, baterías incluidas) (D5), ARP (E5; en una pista DRUM, un fill nuevo en cada
  pulsación), RANDOM (F5). Entran en la siguiente 1/16. **A#4** elige a qué pistas afectan: ALL, SYN (sintes) o DRM
  (baterías); el encabezado lo muestra.
- **Perillas** con FX mantenido: **FILTER** (izquierda pasa-bajos, derecha pasa-altos), **CRUSH**, **THROW** (manda la
  mezcla seca al delay y la reverb), **DEPTH** (el nivel del REPEAT).
- **Teclas negras 1–6**: mutean T1–T6 mientras las mantenés (no se guarda).

**El carril de punch-in**: cada sección tiene un carril de 64 pasos de 1/16 (4 compases). Con **REC armado y sonando**
(no durante un tema), los **efectos de nota (D4–F5)** que mantenés se graban en él, uno por paso (REPEAT, LPF, HPF y
los mutes no se graban). Mantené **FX + G5** mientras suena para borrar por donde pasa el cabezal; parado, FX + G5
borra todo el carril. **REC mantenido + FX** también lo borra (junto con la automatización de FX de la pista).

**Traba**: mantené FX hasta que se abra su mapa y soltalo sin tocar nada: la capa FX queda abierta (**[FX] LOCKED**),
como las macros con LFO; las teclas tocan los efectos (momentáneos: apretada suena, soltada se apaga), las perillas son
las macros del FX y OCT− es ALL OFF, con las dos manos libres. Apretá **FX** otra vez para destrabar (los efectos
trabados que dejaste prendidos siguen). Si lo mantenés tocando teclas o perillas, FX funciona como siempre y se cierra al soltar.

**FX LATCH** (MENU > CONTROL > FX LATCH, **OFF por defecto**; ON por defecto de 0.6.44 a 0.6.63): en ON, una tecla
apretada con FX activa y desactiva su efecto, las macros de las perillas conservan sus valores, **FX + OCT−** apaga
todo (FX ALL OFF). Los efectos trabados no se graban en el carril.

### Mutes y solos

- **GLO mantenido**: las teclas negras 1–6 mutean T1–T6 (enganchado, se guarda con el proyecto y en las escenas);
  F3–D4 hacen solo mientras las mantenés; las teclas negras 7–10 mutean los grupos de DRUM-X; E4 desmutea todo.
- **MIXER** KNOB 4: MUTE de la pista seleccionada.
- **EDIT mantenido** en una pista DRUM: las teclas negras 1–8 mutean cada sonido de DRUM-X.
- **FX mantenido**: las teclas negras 1–6 mutean mientras las mantenés.

### Capas en vivo y el tema

La capa SAVE (saltos de sección en el compás, cadenas rápidas, guardar, recuperar, SONG REC) está descripta en
[7](#7-arrange-canciones-secciones-y-escenas). GLO + PLAY reinicia desde el principio sin parar.

---

## 12. Guardar y cargar

Este es el capítulo para leer con atención. JIANT guarda varios tipos de datos en lugares distintos y de maneras
distintas.

### El panorama general

- **La música** es lo que está sonando ahora, en RAM: los sonidos y pasos de las seis pistas, la automatización, los
  efectos, el tempo.
- Las **secciones** (A–D de cada uno de los 8 proyectos) son donde **guardás** la música de forma permanente, en
  flash.
- El **arreglo** (filas, END, escenas) pertenece al proyecto y se guarda solo.
- Los **presets de usuario** (32 slots) guardan sonidos sueltos.
- La **configuración** (el MENU, TRACKS, LOAD) se guarda sola.
- El **autoguardado** (autosave) es una copia de seguridad de la música, para que la vuelvas a encontrar después de
  apagar. **No es un guardado en una sección**, y no ocurre al apagar.

### Qué se guarda dónde

| Qué | Dónde vive | Cómo se guarda | ¿Sobrevive al apagado? |
| --- | --- | --- | --- |
| La música actual (sonidos, pasos, automatización, tempo, efectos, kit, macros, carril de punch-in, SLICER) | RAM | No se guarda sola — guardala en una sección. El **autoguardado** conserva una copia. | Solo a través del autoguardado |
| Una **sección** (A–D de un proyecto) | Flash | **SAVE > PROJECT > SAVE** (parado), o **capa SAVE C4–F4 / OCT+** | Sí (guardados desde la capa: una vez que parás) |
| El **arreglo** (filas, compases, END, escenas) de cada proyecto | Flash (índice de temas) | Automáticamente, ~2 s después de un cambio con el transporte parado | Sí |
| **Presets de usuario** U01–U32 (un sonido + un patrón de 16 pasos) | Flash | **SAVE > USER > SAVE** (parado) | Sí |
| **Configuración** (MENU, TRACKS, LOAD, favoritos) | Flash | Automáticamente (MENU: al cerrarlo; mientras suena: después de parar) | Sí |
| El **autoguardado** | Flash, en su propio lugar | 10 s después del último cambio, parado y sin actividad, como máximo una vez por minuto | Sí (se restaura al encender si RESTORE LAST está en ON) |
| El **banco de voces FM6** B1–B32 | Flash, en su propio lugar | Se envía desde el editor web | Sí (no está en el backup: guardá el .syx) |
| **UNDO** | RAM | — | No |

### Cómo funciona realmente el autoguardado

- Escribe una copia de la música **después de 10 segundos sin cambios**, solo cuando el transporte está **parado**,
  no hay ninguna tecla ni botón mantenido y no está sonando nada, y **como máximo una vez por minuto**.
- **No** guarda cuando apagás. Si cambiás algo y apagás dentro de esos segundos, ese último cambio se pierde. Para ir
  a lo seguro: **pará, esperá ~10 s**, o mejor, guardá en una sección.
- Al encender, el equipo arranca en cero, como lo deja **NEW PROJECT** (cada pista con el sonido por defecto de su motor,
  sin pasos). Con **MENU > SYSTEM > RESTORE LAST ON** el autoguardado pasa a ser la música (la pantalla dice
  **RESTORED**). Desde 0.6.40 RESTORE LAST viene en **OFF**: sin autoguardado.
- Después de un crash, un cuelgue o un arranque fallido, el autoguardado no se restaura (la música que estaba sonando
  podría ser lo que causó el crash).

### La lente de SAVE: LIBRARY · PROJECT · SOUND

Desde la 0.8.5 SAVE funciona como la lente del secuenciador y se dibuja como el diseño aprobado: tres pestañas de
nivel arriba, **LIBRARY · PROJECT · SOUND** (biblioteca, proyecto, sonido). **SELECT** hace zoom entre ellas (sin dar la
vuelta), un toque de **SAVE** cambia entre los dos juegos del nivel, y cada nivel vuelve al juego en el que lo
dejaste. Las páginas, sus perillas, OCT+, los diálogos y la pantalla NAME son los de siempre.

| Nivel | Qué muestra la pantalla | Un toque de SAVE cambia | Perillas |
| --- | --- | --- | --- |
| **LIBRARY** | Los ocho proyectos como fichas: el número, el nombre (el de su sección A), sus cuatro secciones A–D como chips (guardadas: en el color de la sección; vacías: oscuras). El proyecto abierto teñido y enmarcado en violeta, su sección elegida enmarcada en blanco. | **PROJECT** ↔ **TOOLS** (las seis herramientas en lista) | PROJECT, SECT, LOAD, SAVE (PRESETS: el proyecto) / la herramienta |
| **PROJECT** | Las cuatro secciones del proyecto en grande, las filas de ARRANGE como fichas, el tema como una tira (ver [7](#7-arrange-canciones-secciones-y-escenas)). | **SECTIONS** ↔ **ARRANGE** | SECT, NAME (se muestra; EDIT lo renombra), LOAD, SAVE / ROW, SECT, BARS, SCENE (OCT+ agrega una fila) |
| **SOUND** | El ser de la pista (la criatura de su motor, viva con su sonido), la pista y su sonido debajo; a la derecha los 32 lugares de usuario (uno guardado: la criatura de su motor en chiquito y su nombre) o los sonidos de fábrica. | **USER** ↔ **PRESETS** | SLOT, LOAD, ERASE, SAVE / No., ENG, FAV, LIST |

- **SAVE desde otra página** abre USER de SOUND con SAVE listo: SAVE, OCT+, OCT+ guarda el sonido, como antes.
- Las fichas de los otros proyectos se leen de la flash de a un proyecto por cuadro (solo los encabezados: no se carga
  nada, el audio no lo nota) y se guardan hasta que se guarda, carga o renombra un proyecto.
- El pie dice **SAVE · LIBRARY** (la lente, el nivel). Las ayudas OCT+ / EDIT / OCT− no se dibujan en las páginas de
  la lente: la tarjeta armada dice OCT+.

### Para guardar una sección de un proyecto

1. **Pará** el transporte (guardar necesita el transporte parado: si no, **STOP TO SAVE**).
2. Apretá **SAVE** (se abre SOUND) y girá **SELECT** a la izquierda: **PROJECT** (un paso) o **LIBRARY** (dos).
3. En **LIBRARY**, **KNOB 1** (o PRESETS): el proyecto (1–8), su ficha enmarcada; **KNOB 2** su sección. Girar KNOB 1
   **no** cambia la música que estás tocando — solo elige dónde guardar. (Cambia de proyecto, así que hacelo con el
   transporte parado.)
4. En **PROJECT**, **KNOB 1** (o PRESETS): la sección A–D, dibujada en grande con su nombre (o USED / EMPTY) y **NOW**
   en la sección de la que vino la música; la tarjeta de KNOB 2 muestra su nombre.
5. **KNOB 4** a la derecha: SAVE queda elegido ([OCT+]). Apretá **OCT+**.
6. Si la sección ya tiene algo: **OVERWRITE PROJECT A?** (con su nombre). OCT+ YES.
7. Se abre la pantalla **NAME** con un nombre listo. Editalo o no, y después **OCT+** escribe. **OCT−** cancela y no
   escribe nada.
8. La pantalla dice **SAVED**.

Una sección guarda la música **incluyendo el tempo y los efectos globales**. El **nombre del proyecto** es el nombre
de su sección A.

### Para cargar una sección

1. En LIBRARY de SAVE: **KNOB 1** el proyecto, **KNOB 2** la sección (o en PROJECT: **KNOB 1** la sección).
2. **KNOB 3** a la derecha (LOAD), apretá **OCT+**. La pantalla dice **LOADED**.

LOAD **detiene la reproducción** y reemplaza la música: sonidos, pasos, automatización, el kit, el carril de punch-in,
las macros, y el **tempo, los efectos, el ruteo MIDI y la configuración de clock** guardados con ella. El arreglo se
queda. Si no era lo que querías, **GLO + G4** (UNDO) trae de vuelta lo que tenías.

### Guardado rápido mientras suena (capa SAVE)

Mantené SAVE y apretá **C4–F4** para guardar la música en la sección A–D del proyecto actual, u **OCT+** para guardar
en la sección que está sonando. Esto funciona **mientras suena**, sin poner nombre. El guardado queda en **RAM al
instante y pasa a la flash cuando parás**. Si apagás antes de parar, se pierde — así que pará antes de apagar.

### Copiar

- **Una sección a otra letra**: cargala (LOAD) y después guardala (SAVE) en la otra letra.
- **Una sección a otro proyecto**: cargala, girá KNOB 1 hasta el otro proyecto (la música se queda) y guardá en una
  letra.
- **"Guardar como" un proyecto nuevo**: con tu música sonando, pará, andá a LIBRARY de SAVE, girá KNOB 1 hasta un
  proyecto vacío y guardá en A.

### Cambiar de proyecto

- En **LIBRARY de SAVE, KNOB 1** cambia de proyecto pero **conserva la música** (para guardarla ahí o cargar una de
  sus secciones).
- Con **KNOB 1 de la capa SAVE** (mantené SAVE, girá, soltá) cambia el proyecto **y su sección A pasa a ser la
  música** (si tiene una). La música anterior se reemplaza y **UNDO no la puede traer de vuelta** — guardala antes.
- Las dos cosas necesitan el transporte parado (**STOP TO CHANGE PROJECT** / **STOP TO CHANGE SONG**). Las secciones
  guardadas con la capa SAVE se escriben en la flash antes del cambio.

### Poner nombres (la pantalla NAME)

Se abre cuando guardás una sección o un preset de usuario, y cuando apretás **EDIT** en SAVE > USER o SAVE > PROJECT
(renombra el slot seleccionado; solo se escribe el nombre). Hasta 12 caracteres, en mayúsculas.

| Control | Hace |
| --- | --- |
| Teclas blancas | Letras al estilo teléfono: AB, CD, EF, GH, IJK, LM, NO, PQ, RS, TU, VW, XYZ, después 123, 456, 789, 0-. — tocá de nuevo dentro de 0,8 s para el siguiente carácter |
| Teclas negras (por nombre) | **F#** cursor a la izquierda, **G#** espacio, **A#** cursor a la derecha, **C#** borrar, **D#** alternar ABC / 123 (dígitos y símbolos, uno por tecla) |
| KNOB 1 | mover el cursor |
| KNOB 2 | cambiar el carácter en el cursor |
| OCT+ / OCT− | escribir / cancelar |

Un nombre vacío: una sección de proyecto queda sin nombre, un preset de usuario recibe uno automático.

### Presets de usuario (SAVE > USER)

32 slots para sonidos sueltos. **SAVE** abre directamente en esta página con SAVE ya elegido, así que **SAVE, OCT+,
OCT+** guarda el sonido de la pista seleccionada con el nombre que te ofrece.

- **KNOB 1**: el slot (U01–U32). **KNOB 2** LOAD, **KNOB 3** ERASE (pregunta antes), **KNOB 4** SAVE (pregunta antes
  de sobrescribir, después NAME). Girá la perilla a la derecha para elegir, **OCT+** para ejecutar. Guardar y borrar
  necesitan el transporte parado.
- Un preset de usuario guarda el **motor, sus parámetros de sonido y un patrón de 16 pasos**; un sonido FM6 conserva
  su patch.
- **No** guarda el **kit DRUM-X** (ese vive en las secciones del proyecto).
- Cargar uno carga solo el sonido.
- Tus presets de usuario también aparecen al recorrer con PRESETS, después de los sonidos de fábrica.

### Preguntas frecuentes sobre guardar

**Apagué y se perdieron mis últimos cambios.** El autoguardado solo escribe después de 10 s sin cambios con el
transporte parado, como máximo una vez por minuto, nunca al apagar. Pará y esperá un momento, o guardá en una sección.

**Dice STOP TO SAVE.** Guardar en una sección o en un preset de usuario escribe la flash, lo que pausa el audio un
instante, así que solo se hace con el transporte parado. Apretá PLAY para parar y probá de nuevo (la pantalla NAME
queda abierta; PLAY ahí solo para).

**Dice SAVED (NO FLASH).** No se detectó el chip de flash, así que el guardado quedó solo en RAM. Sobrevive a un
reset, no a un apagado. Reiniciá el equipo; si lo sigue diciendo, ver [16](#16-problemas-y-preguntas-frecuentes).

**Dice SECTION 90% FULL / TOO FULL TO SAVE / SECTION FULL.** Una sección tiene un tamaño fijo. Una música muy cargada
(seis pistas de acordes en los 64 pasos más mucha automatización) puede no entrar. Guardar una sección que no entra
dice SECTION FULL y deja intacta la que estaba guardada. Acortá patrones, borrá pasos o automatización que no uses.

**Guardé con la capa SAVE mientras sonaba y lo perdí.** Lo guardado desde la capa llega a la flash cuando parás. Pará
antes de apagar.

**¿Mi arreglo se guarda?** Sí, automáticamente, un par de segundos después de cada cambio con el transporte parado.

**¿Dónde está mi kit de batería en un preset de usuario?** Los presets de usuario no guardan el kit DRUM-X; guardá la
sección del proyecto.

**¿Puedo hacer un backup de todo?** Sí: el backup completo del editor web incluye los 8 proyectos con sus secciones,
arreglos y escenas, los presets de usuario y la configuración. Ver
[15](#15-herramientas-web-emulador-instalador-editor-backup).

---

## 13. Sistema: TRACKS, LOAD y el MENU

### TRACKS (GLO > SYSTEM, KNOB 1 — también MENU > AUDIO > TRACKS)

- **4** (por defecto): suenan T1–T4; **T5 y T6 están dormidas**. Conservan su sonido, sus pasos y su mezcla, y no
  consumen nada. Si se le manda una nota a una pista dormida (una tecla, MIDI, sus pasos) o PLAY arranca con pasos en
  ellas, la pantalla dice **T5-T6 ASLEEP** y **GLO>SYSTEM>TRACKS 6**. Las pistas dormidas aparecen tenues en el mapa
  de GLO y en gris en HOME y en el MIXER.
- **6**: suenan las seis pistas. Vigilá el medidor de carga.

### LOAD (GLO > SYSTEM, KNOB 2 — también MENU > SYSTEM > LOAD)

Cuán pesado puede ponerse el sonido.

| LOAD | Qué cambia mientras suena |
| --- | --- |
| **FULL** (por defecto) | Todo tal como está configurado. |
| **LITE** | Las reverbs SHIMMER y CLOUD suenan como ROOM, RESO como SPRING; el delay GRAIN suena común; FM6 toca dos voces. La pantalla se dibuja más liviana: los seres y gráficos se animan a unos 20 cuadros por segundo con contornos más simples, y los valores cambian al instante (sin dígitos que ruedan). |
| **ECO** | LITE, más: todas las reverbs suenan como un SPRING liviano, **sin SLICER** (sus pistas van directo a la mezcla), **sin chorus** (sus páginas se ocultan), como máximo **4 voces** por pista de sinte y **6 voces** entre todas las pistas (8 si no; la batería también cuenta). Cuando la carga sube, las colas casi en silencio se sueltan antes. La pantalla a unos 14 cuadros por segundo, los seres aún más simples. |

LOAD solo cambia cómo se renderizan las cosas: **los proyectos conservan su configuración** y suenan como se guardaron
en FULL (o en otro FM-1). Las tarjetas de los efectos sustituidos dicen LITE o ECO. LOAD libera **CPU**, no RAM: el
firmware reserva toda su memoria al arrancar. La tarjeta **CPU** (KNOB 4 en SYSTEM) y el medidor de carga del
encabezado muestran la carga de audio. TRACKS y LOAD son configuraciones del equipo, no forman parte de un proyecto.

### ROUTE (GLO > SYSTEM, KNOB 3)

**CH1-6**: los canales MIDI 1–6 tocan las pistas 1–6, los canales 7–16 se ignoran. **SEL**: todos los canales tocan
la pista seleccionada.

### GLOBAL (GLO > GLOBAL)

**BPM** (40–240), **SWING**, **CLOCK** (INT, USB, TRS: sigue el clock MIDI y el start/stop de esa entrada), **TUNE**
(±50).

### El MENU (mantené HOME)

(0.8.5) Es una lente, como la maqueta: **SELECT** hace zoom **MENU · TAB · ROW** (abre en TAB). **MENU**: las cuatro
pestañas como mosaicos con sus filas y valores (PRESETS elige la pestaña); **TAB**: las filas de la pestaña, las cuatro
de la fila elegida como tarjetas (ROWS 1–4 / ROWS 5–6) con el número de su perilla; **ROW**: los valores de la fila
elegida como fichas (COLOR: los colores de cada paleta), las tarjetas TAB, ROW y su valor. El encabezado dice MENU ·
DISPLAY. ALGORITHM, PRESETS, cualquier perilla y OCT+ / OCT− como antes.

**ALGORITHM** cambia de pestaña, **PRESETS** se mueve entre filas, **cualquier perilla** u **OCT+ / OCT−** cambia el
valor. HOME lo cierra (en ese momento se guarda la configuración).

| Pestaña | Fila | Valores |
| --- | --- | --- |
| DISPLAY | COLOR | GREY, GREEN, AMBER, ICE, VIOLET, ROSE, PAPER, HI-CON, NIGHT, MONO, **JIANT** (por defecto) |
| DISPLAY | STYLE | FLAT, LINE |
| DISPLAY | LARGE | OFF, ON (etiquetas y valores grandes) |
| DISPLAY | ANIM | ON, OFF (los valores saltan directo; la firma de encendido aparece entera) |
| DISPLAY | LEDS | OFF, DIM LO, **DIM HI** (por defecto), INV — (0.7.3) los LEDs del brillo de reposo se turnan cuadro a cuadro, así el brillo ya no zumba por el parlante cuando la música para |
| DISPLAY | SCREEN OFF | **NEVER** (por defecto), 5 MIN, 15 MIN, 30 MIN, 60 MIN |
| CONTROL | HOLD | 0.3 s, **0.4 s** (por defecto), 0.5 s, 0.6 s — cuánto tarda un botón mantenido en abrir su capa |
| CONTROL | KNOB ACCEL | **ON** (por defecto), OFF |
| CONTROL | FX LATCH | **OFF** (por defecto desde 0.6.64), ON |
| CONTROL | BPM LOCK | OFF, ON — DICE ALL conserva el tempo (el tempo es GLO + SELECT, F4 TAP de GLO o el BPM de GLOBAL) |
| CONTROL | SCALE LEDS | OFF, ON — las teclas de la escala se iluminan, la tónica más brillante |
| CONTROL | MIDI OUT | **KEYS**, CLOCK, ALL, OFF — qué sale por MIDI USB (ver [14](#14-midi-y-usb)) |
| AUDIO | SPEAKER EQ | FLAT, LOWCUT, BASS+ (un EQ en el master para el parlante chico; también llega a las salidas y al USB) |
| AUDIO | USB LEVEL | MASTER (el USB sigue a la perilla MASTER), FIXED (USB a nivel completo), −6 / −12 / −18 / −24 dB (USB fijo y atenuado: para un celular o una computadora cuya entrada satura a nivel completo) |
| AUDIO | CLICK | OFF, REC (mientras una pista está armada y sonando), ON |
| AUDIO | CLICK LEVEL | LOW, MID, HIGH |
| AUDIO | COUNT-IN | OFF, 1 BAR, 2 BARS |
| AUDIO | TRACKS | **4** (por defecto), 6 |
| SYSTEM | USB | CONSOLE (la consola serie con audio USB estéreo, por defecto; era USB SERIAL ON), STEREO (solo audio + MIDI, clase 0; era OFF: ayuda a que macOS 13–15 vea la entrada de audio USB), STEMS (0.7.1: entrada de audio de 8 canales, ver "Stems" más abajo), IN+OUT (0.7.5: audio estéreo en los dos sentidos, ver "Entrada de audio USB" más abajo), STEMS+IN (0.8.0: los stems con la salida de audio también, ver "Stems con una voz" más abajo) |
| SYSTEM | RESTORE LAST | **OFF** (por defecto: sin autoguardado, el encendido arranca en cero), ON (la última sesión vuelve al encender) |
| SYSTEM | LOAD | ECO, LITE, **FULL** (por defecto) |
| SYSTEM | CALIBRATION | OCT+ abre HARDWARE CALIBRATION (enseñarle cada botón y perilla) |
| SYSTEM | INIT SYSTEM | OCT+ pregunta, y después todo vuelve a cero para una canción nueva (como SAVE > TOOLS > NEW PROJECT; UNDO devuelve la música) |
| SYSTEM | ABOUT | OCT+ abre: versión, código QR, AUDIO LOAD (NOW, PEAK, LATE, SHED desde el encendido) y créditos (se desplaza con PRESETS, OCT− vuelve) |

Si escuchás clicks, abrí ABOUT y mirá **LATE** (bloques de audio que llegaron tarde) y **SHED** (voces que la
protección desvaneció): una foto de esa pantalla ayuda cuando reportás un problema.

---

## 14. MIDI y USB

- **USB** es MIDI y audio class-compliant: la salida master le llega a la computadora como una entrada de audio USB.
- **MIDI IN** por USB y por el jack **TRS**. **ROUTE** decide qué pista toca cada canal (ver
  [13](#13-sistema-tracks-load-y-el-menu)). Las notas entrantes en una pista armada se graban igual que las teclas.
- **MIDI OUT** (por USB; MENU > CONTROL > **MIDI OUT**, 0.6.72), cada pista por su canal (T1 = canal 1 … T6 =
  canal 6):
  - **KEYS** (por defecto): las teclas mandan sus notas, como siempre.
  - **CLOCK**: y además el clock (24 por negra) con **START / STOP**, mientras el tempo es el del FM-1 (CLOCK INT): una
    caja de ritmos o un DAW lo siguen. El clock corre también parado, así el que sigue conoce el tempo antes del PLAY.
  - **ALL**: y además todas las notas que toca el FM-1: el secuenciador, el arpegiador, PLAYHEADS y las repeticiones de
    los punch-in, con el OCT− / OCT+ y el RANDOM de los punch-in aplicados; cada nota tiene su note-off, y STOP, un
    pánico o un cambio del ajuste apagan lo que quede sonando. Grabá la música del FM-1 en un DAW como MIDI, o tocá
    otros sintes con él.
  - **OFF**: nada.
- **PROGRAM CHANGE** (0.6.72): el programa *n* en el canal que toca una pista carga en esa pista el sonido número
  *n* + 1 de la lista de PRESETS (los presets de cada motor en su orden, después los presets de usuario); la pista
  seleccionada no cambia. Los programas llegan a los primeros 128 sonidos de la lista: desde 0.8.3 eso termina dentro
  de los presets de ANIMAL (va último), así que los presets de usuario que vienen después se eligen en el equipo o
  desde el editor.
- **Secciones y proyectos** (0.6.80): el **canal 16** es el de la canción: su PROGRAM CHANGE nunca carga un sonido
  (con cualquier ROUTE). Los programas **0–3** saltan a la sección **A–D** del proyecto (sonando: en el compás
  siguiente, como las teclas de la capa de la canción; parado: en el momento; si está vacía, lo avisa); los programas
  **16–23** cargan el proyecto **1–8** con su sección A (solo parado, como SAVE > PROJECT). Un pedal MIDI o un DAW
  pueden llevar un set entero.
- **No es posible**: MIDI OUT por el jack **TRS** (el jack del FM-1 es solo de entrada: no le llega ninguna línea de
  salida) ni **MIDI por Bluetooth** (el chip tiene radio, pero su software de Bluetooth es el SDK cerrado de JieLi,
  que JIANT no usa).
- **Clock**: GLO > GLOBAL > CLOCK **USB** o **TRS** sigue el clock y los Start / Continue / Stop de esa entrada. Con un
  clock externo, TAP y el reinicio con GLO + PLAY quedan desactivados.
- **Controladores** (en la pista que toca el canal): CC5 glide, CC7 nivel, CC10 paneo, CC71 resonancia y CC74 cutoff
  (los propios del motor, donde los tiene), CC72 release, CC73 attack, CC75 decay, CC91 envío de reverb, CC93 envío
  de chorus, CC94 envío de delay; **CC16–23** los ocho valores propios del motor (las perillas de EDIT 1 / EDIT 2,
  E1–E8, sea cual sea el motor: CC16 es el KNOB 1 de EDIT 1), **CC102 / CC103** cutoff / resonancia del filtro post,
  **CC104** drive, **CC105** sustain (0.6.96). CC1 (rueda de modulación), CC11 (expresión) y el aftertouch de canal son fuentes de
  la matriz MOD (MODW, EXPR, AT). Funcionan el pitch bend, el sustain (CC64), All Sound Off (CC120) y All Notes Off
  (CC123). Un CC se graba como automatización igual que un movimiento de perilla.
- Nombre USB MIDI y audio: "JIANT FM" (0.7.0; "JIANT Update" mientras se instala un paquete). El editor web y el instalador aceptan también un equipo que todavía se llame "Felucca".

### Stems (0.7.1)

Dos maneras de sacar cada pista por separado, para procesarlas de a una en un DAW:

- **Por USB, en vivo: MENU > SYSTEM > USB > STEMS.** El equipo se vuelve a enumerar como una entrada de audio USB de 8
  canales a 44.1 kHz, 16 bits (en el DAW, la entrada del equipo con 8 canales): los canales 1–6 son las seis pistas, cada
  una en mono con su LEVEL, DIST, ruteo al SLICER y mutes incluidos y sin su PAN (lo que suma a la mezcla, L + R a la
  mitad); los canales 7–8 son el retorno de FX en estéreo (los buses DELAY, REVERB y CHORUS, y el del SLICER cuando está
  encendido). Todo antes del master: sin leveler, limiter, CLIP ni perilla MASTER, así la suma de los ocho es la mezcla
  tal como la oyen los buses. USB LEVEL a −6 dB o menos deja margen. La consola serie queda apagada en este modo; el
  USB-MIDI, el editor y el instalador siguen funcionando. El parlante y los auriculares tocan el master como siempre.
- **Desde un backup, fuera de línea: "Stems" en la página del emulador.** Hacé un backup del FM-1 ("Back up" en el
  editor o el instalador), abrí la página del emulador, desplegá "Stems", cargá el archivo, elegí una canción (sus filas
  se tocan una vez; una canción en LOOP, una vuelta) o la sección en vivo con una cantidad de compases, y apretá Render.
  Cada pista se renderiza sola con su propia parte de DELAY, REVERB y CHORUS (sin los envíos de las otras), más la mezcla
  completa MIX (el master tal como lo toca el FM-1), todas del mismo largo, estéreo 16 bits 44.1 kHz, en un ZIP: T1.wav ..
  T6.wav y MIX.wav (una pista en silencio no se incluye). Más rápido que el tiempo real, sin límites del USB y sin tocar
  el equipo.

### Entrada de audio USB (0.7.5, versión de prueba)

**MENU > SYSTEM > USB > IN+OUT** hace que el FM-1 sea, además de entrada, una salida de audio USB de la computadora o
el celular: el equipo se vuelve a enumerar (como con STEMS, la consola serie se apaga) y el host ve "JIANT FM" como
parlante, estéreo, 16 bits, 44.1 kHz. Lo que el host le mande entra por la **última pista** (T6, o T4 con TRACKS 4) y
se mezcla como el sonido de esa pista:

- su **LEVEL** fija el nivel de la entrada, su **PAN** funciona como balance, sus envíos de **CHORUS, DELAY y REVERB**
  llevan la entrada a los efectos (desde la suma de izquierda y derecha), el **DUCK** la atenúa, su **MUTE** y la capa
  de mutes la silencian;
- el motor propio de la pista sigue sonando (con el patrón vacío queda solo la entrada); su DIST y su filtro no se
  aplican a la entrada;
- lo que se graba por USB es el master estéreo, como con STEREO, con la entrada incluida.

Elegí el FM-1 como salida en la configuración de sonido del host (o en el DAW o la app), poné 44.1 kHz y reproducí.
El FM-1 sigue el reloj del host (lee el audio un poco más rápido o más lento, con ~3.6 ms de retardo); si el host deja
de mandar, la entrada se apaga suave y vuelve cuando retoma. Algunos sistemas ponen como salida por defecto al
dispositivo USB recién conectado: por eso IN+OUT es una opción aparte y no viene activada. Versión de prueba: contanos
cómo anda con tu computadora o tu celular.

### Stems con una voz (0.8.0, versión de prueba)

**MENU > SYSTEM > USB > STEMS+IN** es STEMS e IN+OUT juntos, para la app del celular: el micrófono del celular va al
FM-1, entra en la **última pista** como la entrada de IN+OUT (su LEVEL, PAN, MUTE y sus envíos de CHORUS, DELAY y
REVERB: tu voz por los efectos de esa pista) y vuelven **10 canales**: T1–T6, el retorno de efectos L R, después **la
voz como la mezcló la pista** y **la voz como llegó**. La voz tiene un canal propio (no está en el stem de la pista),
así la app la corre hacia atrás según la latencia de ida y vuelta y la voz cae a tiempo con lo que escuchaste; el
segundo canal de voz es la referencia contra la que la app mide esa latencia. La parte de la voz en el retorno de
efectos no se puede correr (está mezclada con la de todas las pistas): su reverb y sus delays quedan atrasados esa
latencia (~20–30 ms: en una reverb suena como pre-delay). La app lee y escribe el FM-1 con su propio driver USB
(Android pasa a las apps solo algunos canales de un dispositivo USB). Versión de prueba: por ahora la app es JIANT
Probe (su prueba 7).

---

## 15. Herramientas web: emulador, instalador, editor, backup

Las tres funcionan en el navegador y se actualizan solas con cada cambio del firmware.

### Emulador — <https://juanjiant-bit.github.io/JIANT/emu/>

El mismo código que el FM-1, con sonido, en el navegador. Tocalo con el mouse o con el teclado de la computadora: las
filas **Z–'** y **Q–O** son las teclas, **1** a **=** los botones de FX a REC, **[ ]** OCT− / OCT+, **↑ ↓** SELECT,
**← →** ALGORITHM; **Shift-clic** mantiene un botón. Ideal para aprender las capas sin el equipo.

### Instalador — <https://juanjiant-bit.github.io/JIANT/webapp/installer/>

Chrome o Edge, con el FM-1 conectado directo a la computadora.

- **Install**: instala JIANT (primero revisa el FM-1: solo sobre el oficial V15 o JIANT).
- **Return to official V15**: dale el **FM-1.fwsc** original sin modificar de M-VAVE (se verifica localmente en el
  navegador). Primero guarda un backup completo (música actual, música guardada, bancos de sonido y configuración; no
  los samples de usuario). Usalo antes de instalar JIANT si tu FM-1 tiene **otro firmware custom** (Felucca,
  baudgirl…), y siempre que quieras volver al firmware de fábrica. El V15 oficial no puede abrir música de JIANT; para
  restaurar el backup más adelante, reinstalá JIANT y usá el editor.

### Editor — <https://juanjiant-bit.github.io/JIANT/webapp/editor/>

Todos los parámetros desde la computadora, por USB. Pestañas: Sound, 6-OP FM, Sequencer, Tracks, Library, Projects,
Settings.

- **6-OP FM**: editá patches FM6, importá .syx y **Send to the device's voice bank (B1–B32)** (enviá al banco de voces
  del equipo).
- **Projects**: los slots de proyecto y el **Complete backup**: **Save backup** y **Restore backup**.

### El backup completo

**Save backup** descarga un único archivo con: la música actual, **los 8 proyectos con sus 4 secciones cada uno**, los
arreglos y las escenas, los 32 presets de usuario (con sus patches FM6) y la configuración. **Restore backup**
reemplaza todo eso en el equipo (mantené el USB conectado). El **banco de voces FM6 (B1–B32) no está incluido**:
guardá tus archivos .syx.

Hacé un backup antes de actualizar, antes de volver al firmware de fábrica, y cada vez que terminás algo que te
encanta.

---

## 16. Problemas y preguntas frecuentes

**T5 o T6 no suenan.** TRACKS está en 4 (el valor por defecto): T5 y T6 están dormidas. GLO > SYSTEM > TRACKS **6** (o
MENU > AUDIO > TRACKS). La pantalla te avisa cuando tocás en una pista dormida.

**Después de borrar una secuencia, seguían sonando notas en todas las pistas.** Esto se corrigió en la v0.6.28: ahora
un borrado termina las repeticiones del carril de punch-in (STUTTER, ARP), una pista DRUM sin pasos no recibe fill de
ARP, y se cancela un salto de sección o una cadena rápida que estaba esperando el compás. Si algo sigue sonando
después de un borrado, puede ser el **carril de punch-in**: mantené **REC y apretá FX** para borrarlo (o
**SEQ + REC** para borrar todo).

**Dos núcleos (0.6.71, prueba).** El segundo núcleo calcula algunas pistas de sinte; MENU > ABOUT dice **CORES 2**,
o **CORES 1** y por qué (REC al prender, una lectura mala, sin respuesta, una falla: entonces sigue con un núcleo
hasta que lo apagues). Mantené **REC** al prender para tocar con un núcleo.

**Clicks, cortes, el medidor de carga lleno.** Dejá TRACKS en 4, probá **LOAD LITE** o **ECO**, usá menos voces (MONO,
menos pistas FM6), reverbs más livianas (CLOUD y SHIMMER son las más pesadas). Revisá MENU > SYSTEM > ABOUT >
AUDIO LOAD.

**SELECT no cambia el tempo.** Desde 0.6.67 SELECT solo recorre las páginas; el tempo es **GLO mantenido + SELECT**
(o F4 TAP de GLO, o la perilla BPM de GLO > GLOBAL).

**Las perillas controlan M1–M4 en lugar de la página.** Las macros están enganchadas (el encabezado dice [LFO] LATCH).
Apretá LFO.

**Un tema no arranca: "PATTERN A EMPTY" / "ADD A SONG ROW".** Una fila apunta a una sección vacía, o no hay filas.
Guardá música en esa sección, o cambiá la fila.

**Dice STOP TO EDIT.** Está sonando un tema (arreglo) o una cadena rápida; paralo para editar pasos, usar herramientas
o UNDO.

**Las teclas parecen invertidas / un botón hace otra cosa.** Corré HARDWARE CALIBRATION: mantené OCT− y OCT+ mientras
encendés (o MENU > SYSTEM > CALIBRATION) y enseñale cada botón y perilla. Una tecla del piano cancela.

**La entrada de audio USB no aparece en macOS.** MENU > SYSTEM > USB **STEREO** (o STEMS).

**SAVED (NO FLASH).** No se detectó el chip de flash; lo guardado queda solo en RAM. Apagá y prendé el equipo. Si
persiste, hacé un backup de lo que puedas con el editor y reinstalá.

**El FM-1 queda en negro después de una actualización interrumpida.** Fijate si la computadora lo ve como
**WL80UBOOT** (un dispositivo USB 4C4A:8057): ese es el modo de arranque del chip y el instalador lo puede volver a
escribir (probá con otro cable). Para entrar a propósito al modo de actualización desde un equipo funcionando, mantené
**OCT− + OCT+** durante 5 segundos. Si eso no alcanza,
[FM-1 Transporter](https://github.com/kurogedelic/FM-1-transporter) lee y escribe la flash con una Seeed XIAO RP2040.

**Quiero volver al firmware de fábrica.** Usá la sección **Return to official V15** del instalador (hace un backup
antes), o el actualizador propio de M-VAVE.

**Vengo de baudgirl / Felucca. ¿Puedo instalar directamente?** No: primero volvé al V15 oficial con el instalador y
después instalá JIANT. Pasar directamente desde otro firmware custom ya brickeó algún FM-1.

**¿Adónde fueron mis samples de Felucca?** JIANT no tiene samples de usuario; esa flash ahora guarda los temas.

---

Si mi trabajo resuena con vos, considerá apoyarme en Ko-fi (https://ko-fi.com/juanjiant) — en Argentina: Cafecito (https://cafecito.app/juanjiant).
