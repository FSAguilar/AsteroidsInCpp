# Proyecto C++ con SFML

Proyecto de C++ utilizando la biblioteca SFML 2.6.2.

## Requisitos

- MinGW (compilador C++)
- SFML 2.6.2

## Compilación

Para compilar el proyecto, ejecuta:

```bash
compile.bat
```

## Ejecución

Si descargas el repositorio para **solo ejecutar el juego** (sin compilar):

1. Asegúrate de que todas las DLLs estén en la misma carpeta que `juego.exe`
2. Ejecuta:
   ```bash
   juego.exe
   ```

## DLLs Necesarias

El repositorio incluye todas las DLLs necesarias para ejecutar el juego:

### DLLs de SFML 2.6.2:
- `sfml-graphics-2.dll`
- `sfml-window-2.dll`
- `sfml-system-2.dll`
- `sfml-audio-2.dll`
- `openal32.dll` (requerida por sfml-audio)

### DLLs del Runtime de MinGW (GCC):
- `libstdc++-6.dll`
- `libgcc_s_seh-1.dll`
- `libwinpthread-1.dll`

**Importante:** Si al ejecutar `juego.exe` obtienes un error sobre "punto de entrada del procedimiento" o DLLs faltantes, asegúrate de que **todas** estas DLLs estén en la misma carpeta que el ejecutable.

## Estructura del Proyecto

- `main.cpp` - Archivo principal del programa
- `compile.bat` - Script de compilación
- `juego.exe` - Ejecutable compilado
- `*.dll` - Bibliotecas dinámicas necesarias
- `images/` - Recursos de imágenes
- `fonts/` - Fuentes del juego
- `compile_flags.txt` - Configuración para clangd

## Solución de Problemas

### Error: "No se encuentra el punto de entrada del procedimiento..."
Este error ocurre cuando faltan las DLLs del runtime de MinGW. Asegúrate de descargar **todas** las DLLs del repositorio o cópialas manualmente desde tu instalación de MinGW (`D:\msys64\mingw64\bin\` o similar).

### Error: "No se puede iniciar el programa porque falta SFML..."
Verifica que las tres DLLs de SFML estén en el mismo directorio que `juego.exe`.
