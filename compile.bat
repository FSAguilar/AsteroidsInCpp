@echo off
g++ -o juego.exe main.cpp Player.cpp Bullet.cpp -IC:\SFML-2.6.2\include -LC:\SFML-2.6.2\lib -lsfml-graphics -lsfml-window -lsfml-system

if %errorlevel% equ 0 (
    echo OK
    juego.exe
) else (
    echo ERROR
)
