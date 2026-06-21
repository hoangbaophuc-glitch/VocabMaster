@echo off
cd /d "%~dp0"
echo ===========================================
echo       BIEN DICH DU AN VOCABMASTER C++
echo ===========================================
echo Dang bien dich cac tep src/*.cpp...

g++ -std=c++11 -Iinclude src/*.cpp -o VocabMaster.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [OK] Bien dich thanh cong! 
    echo Hay chay file VocabMaster.exe de choi game.
    echo.
) else (
    echo.
    echo [ERROR] Co loi xay ra trong qua trinh bien dich!
    echo.
)
pause

