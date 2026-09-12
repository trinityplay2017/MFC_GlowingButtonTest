@echo off
cd /d "%~dp0"
echo.
echo === git status ===
git status
echo.
echo === git add -A ===
git add -A
echo.
set /p msg=Commit message: 
if "%msg%"=="" set msg=update
echo.
echo === git commit ===
git commit -m "%msg%"
echo.
echo === git push ===
git push
echo.
pause
