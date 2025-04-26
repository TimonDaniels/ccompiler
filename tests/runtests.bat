@echo off
REM Run each test and compare against known good output

if not exist ..\compiler.exe (
    echo Need to build ..\compiler.exe first!
    exit /b 1
)

for %%i in (input*) do (
    if not exist "out.%%i" (
        echo Can't run test on %%i, no output file!
    ) else (
        echo %%i
        ..\compiler.exe %%i > NUL 2>&1
        gcc -c -o out.o assembly.s
        gcc -o out.exe out.o ..\compiler\lib\printint.c -e main
        out.exe > trial.%%i
        fc "out.%%i" "trial.%%i" > nul
        if errorlevel 1 (
            echo : failed
            fc "out.%%i" "trial.%%i"
            echo.
        ) else (
            echo : OK
        )
        del out.exe out.o assembly.s "trial.%%i"
    )
)