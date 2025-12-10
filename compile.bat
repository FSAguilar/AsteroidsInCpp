@echo off
echo ========================================
echo   Compilando proyecto SFML
echo ========================================
echo.

g++ -o juego.exe main.cpp -IC:\SFML-2.6.2\include -LC:\SFML-2.6.2\lib -lsfml-graphics -lsfml-window -lsfml-system

if %errorlevel% equ 0 (
    echo.
    echo [OK] Compilacion exitosa!
    echo.
    echo Copiando DLLs necesarias...
    copy /Y C:\SFML-2.6.2\bin\sfml-graphics-2.dll . >nul
    copy /Y C:\SFML-2.6.2\bin\sfml-window-2.dll . >nul
    copy /Y C:\SFML-2.6.2\bin\sfml-system-2.dll . >nul
    echo [OK] DLLs copiadas!
    echo.
    echo Ejecutando juego...
    echo ========================================
    echo.
    juego.exe
) else (
    echo.
    echo [ERROR] Fallo en la compilacion
    echo.
    pause
)
