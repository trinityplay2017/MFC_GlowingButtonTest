@echo off
cd /d "%~dp0"

if not exist ".git" (
    echo.
    echo [ERROR] This folder is not a git repository.
    echo.
    echo You must CLONE the repo, not download the ZIP:
    echo.
    echo   git clone https://github.com/trinityplay2017/MFC_GlowingButtonTest.git
    echo   cd MFC_GlowingButtonTest
    echo.
    pause
    exit /b 1
)

echo.
echo === git pull ===
git pull
echo.
pause
