# 🚀 Asteroids Game - SFML C++

Juego de Asteroids estilo arcade implementado en C++ usando la biblioteca SFML con múltiples modos de juego, sistema de oleadas, UFOs enemigos y una IA competitiva.

---

## 📋 Tabla de Contenidos

- [Características](#características)
- [Modos de Juego](#modos-de-juego)
- [Sistemas Implementados](#sistemas-implementados)
- [Controles](#controles)
- [Instalación](#instalación)
- [Detalles Técnicos](#detalles-técnicos)

---

## ✨ Características

### 🎮 Modos de Juego

1. **Un Jugador**: Modo clásico contra asteroides
2. **Dos Jugadores (Cooperativo)**: Jugadores trabajan juntos contra asteroides
3. **Modo VS**: PvP - Los jugadores pueden atacarse entre sí
4. **VS IA**: Juega contra una inteligencia artificial

### 🌊 Sistema de Oleadas

- **6 niveles progresivos** de dificultad
- Cada oleada aumenta:
  - Cantidad de asteroides (grandes, medianos, pequeños)
  - Velocidad de los asteroides (`speedMultiplier`: 1.0x → 1.5x)
  - Tamaño de los asteroides (`scaleMultiplier`: 1.0x → 1.5x)
  - Frecuencia de aparición de UFOs
- Al completar las 6 oleadas, el juego termina

### 👾 Sistema de UFOs

- Aparecen periódicamente según la oleada actual
- Se mueven horizontalmente atravesando la pantalla
- **Disparan balas en 4 direcciones** aleatorias
- Dan **250 puntos** al ser destruidos
- Sus balas **NO destruyen asteroides** (solo dañan jugadores)

### 🛡️ Sistema Anti-Spawn Kill

- **Invulnerabilidad temporal**: 120 frames (2 segundos) después del respawn
- **Efecto visual de parpadeo** durante la invulnerabilidad
- **Limpieza de asteroides**: Elimina asteroides en un radio de 200px al hacer respawn
- Protección contra todo tipo de daño (asteroides, balas, colisiones)

### 🤖 Inteligencia Artificial (Modo Fácil)

#### Comportamiento:
1. **Detección**: Busca el asteroide más cercano dentro de 600px
2. **Actualización**: Reevalúa su objetivo cada 60 frames (1 segundo)
3. **Rotación**: Gira hacia el objetivo con alta precisión (tolerancia 5°)
4. **Movimiento**: Avanza cuando apunta razonablemente bien (tolerancia 30°)
5. **Disparo**: Dispara automáticamente cuando apunta al objetivo (tolerancia 30°)

#### Visualización Debug:
- Círculo amarillo semitransparente mostrando el rango de visión de 600px

---

## 🕹️ Modos de Juego

### 1️⃣ Un Jugador
- Destruye asteroides para ganar puntos
- Sistema de 3 vidas con respawn
- Progresa a través de 6 oleadas
- **Puntuación**:
  - Asteroide grande: 20 pts
  - Asteroide mediano: 50 pts
  - Asteroide pequeño: 100 pts
  - UFO: 250 pts

### 2️⃣ Dos Jugadores (Cooperativo)
- Ambos jugadores colaboran
- Puntajes separados (P1 y P2)
- Cada jugador tiene 3 vidas independientes
- **NO pueden dañarse entre sí**

### 3️⃣ Modo VS
- Los jugadores pueden atacarse
- **Bala enemiga**: +500 pts al atacante, -1 vida al golpeado
- **Colisión directa**: Ambos pierden 1 vida
- Gana quien tenga más puntos al final

### 4️⃣ VS IA
- Funciona como Modo VS
- La IA controla al Jugador 2
- Compite contra ti por puntos
- Actualmente implementado en **dificultad fácil**

---

## 🎮 Controles

### Jugador 1 (P1)
| Acción | Tecla |
|--------|-------|
| Rotar Izquierda | `←` Flecha Izquierda |
| Rotar Derecha | `→` Flecha Derecha |
| Acelerar | `↑` Flecha Arriba |
| Retroceder | `↓` Flecha Abajo |
| Disparar | `Espacio` |

### Jugador 2 (P2)
| Acción | Tecla |
|--------|-------|
| Rotar Izquierda | `A` |
| Rotar Derecha | `D` |
| Acelerar | `W` |
| Retroceder | `S` |
| Disparar | `Shift Izquierdo` |

**Nota**: En modo VS IA, los controles del P2 están deshabilitados.

---

## 🚀 Instalación

### Requisitos
- **Compilador C++**: MinGW (g++) o equivalente
- **SFML 2.6.2+**: Biblioteca multimedia
- **Sistema Operativo**: Windows (puede adaptarse a Linux/Mac)

### Compilación

```bash
# Usando el script de compilación incluido
.\compile.bat

# O manualmente con g++
g++ -o juego.exe main.cpp -I"ruta/SFML/include" -L"ruta/SFML/lib" ^
    -lsfml-graphics -lsfml-window -lsfml-system -mwindows
```

### Estructura de Archivos

```
Proyecto/
│
├── main.cpp           # Código principal del juego
├── compile.bat        # Script de compilación
├── README.md          # Este archivo
│
├── images/
│   ├── sprite.png        # Nave estática
│   ├── sprite_move.png   # Nave con propulsión
│   ├── bullet.png        # Sprite de bala
│   ├── asteroid.png      # Sprite de asteroide
│   └── ufo.png          # Sprite de UFO
│
└── fonts/
    └── font.ttf       # Fuente para textos (opcional)
```

---

## 🔧 Detalles Técnicos

### Sistema de Colisiones

#### Colisión Círculo-Círculo
```cpp
// Usada para: balas vs asteroides, asteroides vs asteroides
float distance = sqrt((pos1.x - pos2.x)² + (pos1.y - pos2.y)²);
return distance < (radius1 + radius2);
```

#### Colisión Rectángulo Rotado vs Círculo
```cpp
// Usada para: jugadores vs asteroides
// Algoritmo:
// 1. Obtener transformación del rectángulo (posición + rotación)
// 2. Invertir transformación (globales → locales)
// 3. Transformar círculo al espacio local
// 4. Aplicar AABB tradicional
// 5. Encontrar punto más cercano
// 6. Verificar distancia < radio
```

**Ventaja**: Permite colisiones precisas con rectángulos rotados sin cálculos complejos.

### Sistema de Física

#### Jugadores
- **Velocidad máxima**: Limitada por fricción (`0.98f` por frame)
- **Aceleración**: `0.2f` unidades por frame
- **Rotación**: `5.0f` grados por frame
- **Fricción**: Reduce velocidad gradualmente cuando no aceleras

#### Proyectiles
- **Velocidad fija**: `10.0f` unidades por frame
- **Tiempo de vida**: 60 frames (1 segundo)
- **Sin fricción**: Viajan en línea recta

#### Asteroides
- **Velocidad base**: `200.0f / radius`
  - Grandes: más lentos
  - Pequeños: más rápidos
- **Multiplicador por oleada**: 1.0x → 1.5x
- **Fragmentación**: Al ser destruidos se dividen en 2 asteroides más pequeños

### Gestión de Estado del Juego

```cpp
enum GameMode { 
    MENU,           // Menú principal
    SINGLE_PLAYER,  // Un jugador
    TWO_PLAYERS,    // Dos jugadores
    VS_AI,          // VS IA
    PLAYING,        // En juego
    GAME_OVER       // Fin del juego
};
```

### Variables Globales Clave

#### Configuración de Oleadas
```cpp
struct Wave {
    int numBigAsteroids;
    int numMediumAsteroids;
    int numSmallAsteroids;
    float speedMultiplier;
    float scaleMultiplier;
    int ufoCD;        // Cooldown de aparición UFO
    float ufoSpeed;   // Velocidad del UFO
};

Wave waves[] = {
    {3, 0, 0, 1.0f, 1.0f, 600, 5.f},  // Oleada 1
    {4, 2, 0, 1.1f, 1.1f, 550, 5.2f}, // Oleada 2
    {5, 3, 2, 1.2f, 1.2f, 500, 5.4f}, // Oleada 3
    {6, 4, 4, 1.3f, 1.3f, 450, 5.6f}, // Oleada 4
    {7, 5, 5, 1.4f, 1.4f, 400, 5.8f}, // Oleada 5
    {8, 6, 6, 1.5f, 1.5f, 350, 6.f},  // Oleada 6
};
```

#### Sistema de Vidas
```cpp
int lives = -1;   // P1: -1 = menú, 0 = game over, >0 = jugando
int lives2 = -1;  // P2: mismo sistema
```

#### Invulnerabilidad
```cpp
int playerInvulnerable = 0;   // Contador de frames de invulnerabilidad
int player2Invulnerable = 0;

// Activada durante 120 frames (2 segundos) después del respawn
```

### Propietarios de Balas

```cpp
std::vector<int> bulletOwners;
// 0 = Sin dueño / removido
// 1 = Jugador 1
// 2 = Jugador 2
// 3 = UFO
```

### Algoritmo de IA (Modo Fácil)

```cpp
// Parámetros
float aiThinkInterval = 60.f;   // Frecuencia de actualización
float aiVisionRange = 600.f;    // Rango de detección

// Cada 60 frames:
1. Buscar asteroide más cercano en rango
2. Si se encuentra objetivo:
   a. Calcular ángulo hacia el objetivo
   b. Calcular diferencia con ángulo actual
   c. Rotar hacia el objetivo (±5° tolerancia)
   d. Moverse si apunta bien (±30° tolerancia)
   e. Disparar si apunta bien (±30° tolerancia)
```

---

## 📊 Sistema de Puntuación

### Por Destrucción

| Objetivo | Puntos |
|----------|--------|
| Asteroide Grande | 20 |
| Asteroide Mediano | 50 |
| Asteroide Pequeño | 100 |
| UFO | 250 |
| Golpear jugador enemigo (Modo VS) | 500 |

### Pantalla de Game Over

- **Un Jugador / Cooperativo**: Muestra "Puntaje Final"
- **Modo VS / VS IA**: Muestra "Ganador: P1/P2" + puntaje del ganador

---

## 🐛 Errores Corregidos Durante el Desarrollo

### 1. ❌ Colisiones Incorrectas con Rectángulos Rotados
**Problema**: Los jugadores morían a distancia o podían meter la mitad de la nave en un asteroide sin morir.

**Solución**: Implementación de transformaciones inversas para convertir el círculo al espacio local del rectángulo antes de aplicar AABB.

### 2. ❌ Desincronización de Arrays
**Problema**: Al eliminar balas, los arrays `bulletOwners`, `bulletSpeeds`, etc. se desincronizaban.

**Solución**: Asegurar que `removeBullet()` elimine el elemento de **TODOS** los arrays relacionados.

### 3. ❌ Acceso Fuera de Rango en `removeAsteroid()`
**Problema**: Al calcular el multiplicador de escala, se accedía a `waves[currentWave - 1]` cuando `currentWave == 0`.

**Solución**: 
```cpp
int waveIndex = (currentWave > 0) ? currentWave - 1 : 0;
Wave wave = waves[waveIndex];
```

### 4. ❌ Loop Mal Diseñado en `checkBulletUfoCollisions()`
**Problema**: Iteraba hacia adelante con `i++` mientras eliminaba elementos, saltándose elementos.

**Solución**: Cambiar a loop hacia atrás:
```cpp
for (int i = bullets.size() - 1; i >= 0; i--) {
    // Eliminar elementos de forma segura
}
```

---

## 🚧 Futuras Mejoras Sugeridas

### Próximas Implementaciones
- [ ] **IA Medio y Difícil**: Evasión, predicción, ataque al jugador
- [ ] **Selector de Dificultad IA**: En el menú
- [ ] **Power-ups**: Escudo, disparo rápido, disparo triple
- [ ] **Efectos de Sonido**: Disparos, explosiones, colisiones
- [ ] **Música de Fondo**: Soundtrack dinámico
- [ ] **Efectos Visuales**: Explosiones, partículas, estelas
- [ ] **HUD Mejorado**: Barras de vida, indicador de cooldown
- [ ] **Pantalla de Pausa**: Pausar el juego con `ESC`
- [ ] **Highscores**: Sistema de mejores puntajes guardados

### Optimizaciones Posibles
- [ ] Usar pools de objetos en vez de `new`/`delete`
- [ ] Implementar quadtree para colisiones
- [ ] Separar lógica en clases (refactoring OOP)
- [ ] Usar `std::unique_ptr` para gestión de memoria

---

## 👨‍💻 Créditos

**Desarrollado con**:
- **SFML** (Simple and Fast Multimedia Library)
- **C++17**
- **MinGW/GCC**

**Inspirado en**: Asteroids (Atari, 1979)

---

## 📝 Licencia

Este proyecto es de código abierto para fines educativos.

---

## 🎯 Resumen de Implementación

### Lo que se Implementó en Esta Sesión:

✅ Sistema completo de dos jugadores
✅ Modo cooperativo y competitivo (VS)
✅ Sistema anti-spawn kill (invulnerabilidad + limpieza de área)
✅ Sistema de oleadas progresivas (6 niveles)
✅ UFOs enemigos con disparo en 4 direcciones
✅ Inteligencia artificial básica (modo fácil)
✅ Colisiones rectángulo rotado vs círculo con transformaciones
✅ Sistema de propietarios de balas
✅ Múltiples pantallas (menú, juego, game over)
✅ Efecto de parpadeo en invulnerabilidad
✅ Círculo de visión para debug de IA
✅ Corrección de múltiples bugs críticos

---

**Versión**: 1.0  
**Última actualización**: 14 de Diciembre, 2025

¡Disfruta el juego! 🚀👾
