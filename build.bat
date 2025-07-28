@echo off
echo ========================================
echo Tic-Tac-Toe Game Build Script
echo ========================================

:: 检查是否安装了g++
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo Error: g++ compiler not found!
    echo Please install MinGW or configure your PATH environment variable.
    pause
    exit /b 1
)

:: 检查EasyX库
if not exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\VS\include\graphics.h" (
    if not exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\VS\include\graphics.h" (
        echo Warning: EasyX library not found in standard Visual Studio locations.
        echo Please ensure EasyX is properly installed.
    )
)

echo Building demo1.cpp (Mouse Interaction Demo)...
g++ -o exe\demo1.exe EasyX_Demo\demo1.cpp -leasyx -static-libgcc -static-libstdc++
if %errorlevel% equ 0 (
    echo ✓ demo1.exe built successfully!
) else (
    echo ✗ Failed to build demo1.exe
)

echo.
echo Building demo2.cpp (Tic-Tac-Toe Game)...
g++ -o exe\Tic-Tac-Toe.exe EasyX_Demo\demo2.cpp -leasyx -static-libgcc -static-libstdc++
if %errorlevel% equ 0 (
    echo ✓ Tic-Tac-Toe.exe built successfully!
) else (
    echo ✗ Failed to build Tic-Tac-Toe.exe
)

echo.
echo ========================================
echo Build completed!
echo ========================================
echo.
echo Generated executables:
if exist "exe\demo1.exe" echo - exe\demo1.exe (Mouse Interaction Demo)
if exist "exe\Tic-Tac-Toe.exe" echo - exe\Tic-Tac-Toe.exe (Tic-Tac-Toe Game)
echo.
echo To run the game, execute: exe\Tic-Tac-Toe.exe
echo.
pause 