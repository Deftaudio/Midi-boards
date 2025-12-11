@echo off
setlocal enabledelayedexpansion

if exist temp rmdir /s /q .\temp
if exist banks rmdir /s /q .\banks

set "bank1=%cd%\[WAV_IN]\Bank1"
set "bank2=%cd%\[WAV_IN]\Bank2"
set "bank3=%cd%\[WAV_IN]\Bank3"
set "bank4=%cd%\[WAV_IN]\Bank4"
set "bank5=%cd%\[WAV_IN]\Bank5"
set "bank6=%cd%\[WAV_IN]\Bank6"
set "bank7=%cd%\[WAV_IN]\Bank7"
set "bank8=%cd%\[WAV_IN]\Bank8"

set "count_bank1=0"
set "count_bank2=0"
set "count_bank3=0"
set "count_bank4=0"
set "count_bank5=0"
set "count_bank6=0"
set "count_bank7=0"
set "count_bank8=0"

set "count_errors=0"

if not exist "%bank1%" (
    echo Folder "%bank1%" does not exist.
    exit /b 1
)

if not exist "%bank2%" (
    echo Folder "%bank2%" does not exist.
    exit /b 1
)

if not exist "%bank3%" (
    echo Folder "%bank3%" does not exist.
    exit /b 1
)

if not exist "%bank4%" (
    echo Folder "%bank4%" does not exist.
    exit /b 1
)

if not exist "%bank5%" (
    echo Folder "%bank5%" does not exist.
    exit /b 1
)

if not exist "%bank6%" (
    echo Folder "%bank6%" does not exist.
    exit /b 1
)

if not exist "%bank7%" (
    echo Folder "%bank7%" does not exist.
    exit /b 1
)

if not exist "%bank8%" (
    echo Folder "%bank8%" does not exist.
    exit /b 1
)

echo Checking files in each Bank directory inside [WAV_IN]
echo.

for %%f in ("%bank1%\*.wav") do (
    set /a count_bank1+=1
)

for %%f in ("%bank2%\*.wav") do (
    set /a count_bank2+=1
)

for %%f in ("%bank3%\*.wav") do (
    set /a count_bank3+=1
)

for %%f in ("%bank4%\*.wav") do (
    set /a count_bank4+=1
)

for %%f in ("%bank5%\*.wav") do (
    set /a count_bank5+=1
)

for %%f in ("%bank6%\*.wav") do (
    set /a count_bank6+=1
)

for %%f in ("%bank7%\*.wav") do (
    set /a count_bank7+=1
)

for %%f in ("%bank8%\*.wav") do (
    set /a count_bank8+=1
)

echo Found WAV files in "%bank1%": !count_bank1!
echo Found WAV files in "%bank2%": !count_bank2!
echo Found WAV files in "%bank3%": !count_bank3!
echo Found WAV files in "%bank4%": !count_bank4!
echo Found WAV files in "%bank5%": !count_bank5!
echo Found WAV files in "%bank6%": !count_bank6!
echo Found WAV files in "%bank7%": !count_bank7!
echo Found WAV files in "%bank8%": !count_bank8!
echo.
pause

mkdir .\banks
copy tools\meta\empty.bin banks\bank1.bin > NUL
copy tools\meta\empty.bin banks\bank2.bin > NUL
copy tools\meta\empty.bin banks\bank3.bin > NUL
copy tools\meta\empty.bin banks\bank4.bin > NUL
copy tools\meta\empty.bin banks\bank5.bin > NUL
copy tools\meta\empty.bin banks\bank6.bin > NUL
copy tools\meta\empty.bin banks\bank7.bin > NUL
copy tools\meta\empty.bin banks\bank8.bin > NUL


if !count_bank1! EQU 1 (
  call Deftaudio_PicoROM_Process_1x.bat [WAV_IN]\Bank1
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank1! EQU 2 (
  call Deftaudio_PicoROM_Process_2x.bat [WAV_IN]\Bank1
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank1! EQU 3 (
  call Deftaudio_PicoROM_Process_3x.bat [WAV_IN]\Bank1
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank1! EQU 4 (
  call Deftaudio_PicoROM_Process_4x.bat [WAV_IN]\Bank1
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank1! EQU 5 (
  call Deftaudio_PicoROM_Process_5x.bat [WAV_IN]\Bank1
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank1! EQU 6 (
  call Deftaudio_PicoROM_Process_6x.bat [WAV_IN]\Bank1
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank1! EQU 7 (
  call Deftaudio_PicoROM_Process_7x.bat [WAV_IN]\Bank1
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank1! EQU 8 (
  call Deftaudio_PicoROM_Process_8x.bat [WAV_IN]\Bank1
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank2! EQU 1 (
  call Deftaudio_PicoROM_Process_1x.bat [WAV_IN]\Bank2
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank2! EQU 2 (
  call Deftaudio_PicoROM_Process_2x.bat [WAV_IN]\Bank2
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank2! EQU 3 (
  call Deftaudio_PicoROM_Process_3x.bat [WAV_IN]\Bank2
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank2! EQU 4 (
  call Deftaudio_PicoROM_Process_4x.bat [WAV_IN]\Bank2
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank2! EQU 5 (
  call Deftaudio_PicoROM_Process_5x.bat [WAV_IN]\Bank2
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank2! EQU 6 (
  call Deftaudio_PicoROM_Process_6x.bat [WAV_IN]\Bank2
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank2! EQU 7 (
  call Deftaudio_PicoROM_Process_7x.bat [WAV_IN]\Bank2
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank2! EQU 8 (
  call Deftaudio_PicoROM_Process_8x.bat [WAV_IN]\Bank2
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank3! EQU 1 (
  call Deftaudio_PicoROM_Process_1x.bat [WAV_IN]\Bank3
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank3! EQU 2 (
  call Deftaudio_PicoROM_Process_2x.bat [WAV_IN]\Bank3
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank3! EQU 3 ( call Deftaudio_PicoROM_Process_3x.bat [WAV_IN]\Bank3
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank3! EQU 4 ( call Deftaudio_PicoROM_Process_4x.bat [WAV_IN]\Bank3
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank3! EQU 5 ( call Deftaudio_PicoROM_Process_5x.bat [WAV_IN]\Bank3
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank3! EQU 6 ( call Deftaudio_PicoROM_Process_6x.bat [WAV_IN]\Bank3
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank3! EQU 7 ( call Deftaudio_PicoROM_Process_7x.bat [WAV_IN]\Bank3
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank3! EQU 8 ( call Deftaudio_PicoROM_Process_8x.bat [WAV_IN]\Bank3
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank4! EQU 1 ( call Deftaudio_PicoROM_Process_1x.bat [WAV_IN]\Bank4
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank4! EQU 2 ( call Deftaudio_PicoROM_Process_2x.bat [WAV_IN]\Bank4
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank4! EQU 3 ( call Deftaudio_PicoROM_Process_3x.bat [WAV_IN]\Bank4
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank4! EQU 4 ( call Deftaudio_PicoROM_Process_4x.bat [WAV_IN]\Bank4
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank4! EQU 5 ( call Deftaudio_PicoROM_Process_5x.bat [WAV_IN]\Bank4
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank4! EQU 6 ( call Deftaudio_PicoROM_Process_6x.bat [WAV_IN]\Bank4
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank4! EQU 7 ( call Deftaudio_PicoROM_Process_7x.bat [WAV_IN]\Bank4
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank4! EQU 8 ( call Deftaudio_PicoROM_Process_8x.bat [WAV_IN]\Bank4
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank5! EQU 1 ( call Deftaudio_PicoROM_Process_1x.bat [WAV_IN]\Bank5
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank5! EQU 2 ( call Deftaudio_PicoROM_Process_2x.bat [WAV_IN]\Bank5
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank5! EQU 3 ( call Deftaudio_PicoROM_Process_3x.bat [WAV_IN]\Bank5
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank5! EQU 4 ( call Deftaudio_PicoROM_Process_4x.bat [WAV_IN]\Bank5
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank5! EQU 5 ( call Deftaudio_PicoROM_Process_5x.bat [WAV_IN]\Bank5
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank5! EQU 6 ( call Deftaudio_PicoROM_Process_6x.bat [WAV_IN]\Bank5
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank5! EQU 7 ( call Deftaudio_PicoROM_Process_7x.bat [WAV_IN]\Bank5
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank5! EQU 8 ( call Deftaudio_PicoROM_Process_8x.bat [WAV_IN]\Bank5
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank6! EQU 1 ( call Deftaudio_PicoROM_Process_1x.bat [WAV_IN]\Bank6
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank6! EQU 2 ( call Deftaudio_PicoROM_Process_2x.bat [WAV_IN]\Bank6
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank6! EQU 3 ( call Deftaudio_PicoROM_Process_3x.bat [WAV_IN]\Bank6
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank6! EQU 4 ( call Deftaudio_PicoROM_Process_4x.bat [WAV_IN]\Bank6
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank6! EQU 5 ( call Deftaudio_PicoROM_Process_5x.bat [WAV_IN]\Bank6
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank6! EQU 6 ( call Deftaudio_PicoROM_Process_6x.bat [WAV_IN]\Bank6
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank6! EQU 7 ( call Deftaudio_PicoROM_Process_7x.bat [WAV_IN]\Bank6
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank6! EQU 8 ( call Deftaudio_PicoROM_Process_8x.bat [WAV_IN]\Bank6
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank7! EQU 1 ( call Deftaudio_PicoROM_Process_1x.bat [WAV_IN]\Bank7
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank7! EQU 2 ( call Deftaudio_PicoROM_Process_2x.bat [WAV_IN]\Bank7
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank7! EQU 3 ( call Deftaudio_PicoROM_Process_3x.bat [WAV_IN]\Bank7
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank7! EQU 4 ( call Deftaudio_PicoROM_Process_4x.bat [WAV_IN]\Bank7
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank7! EQU 5 ( call Deftaudio_PicoROM_Process_5x.bat [WAV_IN]\Bank7
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank7! EQU 6 ( call Deftaudio_PicoROM_Process_6x.bat [WAV_IN]\Bank7
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank7! EQU 7 ( call Deftaudio_PicoROM_Process_7x.bat [WAV_IN]\Bank7
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank7! EQU 8 ( call Deftaudio_PicoROM_Process_8x.bat [WAV_IN]\Bank7
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank8! EQU 1 ( call Deftaudio_PicoROM_Process_1x.bat [WAV_IN]\Bank8
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank8! EQU 2 ( call Deftaudio_PicoROM_Process_2x.bat [WAV_IN]\Bank8
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank8! EQU 3 ( call Deftaudio_PicoROM_Process_3x.bat [WAV_IN]\Bank8
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank8! EQU 4 ( call Deftaudio_PicoROM_Process_4x.bat [WAV_IN]\Bank8
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank8! EQU 5 ( call Deftaudio_PicoROM_Process_5x.bat [WAV_IN]\Bank8
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank8! EQU 6 ( call Deftaudio_PicoROM_Process_6x.bat [WAV_IN]\Bank8
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank8! EQU 7 ( call Deftaudio_PicoROM_Process_7x.bat [WAV_IN]\Bank8
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)
if !count_bank8! EQU 8 ( call Deftaudio_PicoROM_Process_8x.bat [WAV_IN]\Bank8
  if %ERRORLEVEL% NEQ 0 (
    set /a count_errors+=1

  )
)

echo.
echo Done with all banks.

if !count_bank1! GTR 8 (set /a count_errors+=1

)
if !count_bank2! GTR 8 (set /a count_errors+=1

)
if !count_bank3! GTR 8 (set /a count_errors+=1

)
if !count_bank4! GTR 8 (set /a count_errors+=1

)
if !count_bank5! GTR 8 (set /a count_errors+=1

)
if !count_bank6! GTR 8 (set /a count_errors+=1

)
if !count_bank7! GTR 8 (set /a count_errors+=1

)
if !count_bank8! GTR 8 (set /a count_errors+=1

)

if !count_bank1! EQU 0 (set /a count_errors+=1
)
if !count_bank2! EQU 0 (set /a count_errors+=1
)
if !count_bank3! EQU 0 (set /a count_errors+=1
)
if !count_bank4! EQU 0 (set /a count_errors+=1
)
if !count_bank5! EQU 0 (set /a count_errors+=1
)
if !count_bank6! EQU 0 (set /a count_errors+=1
)
if !count_bank7! EQU 0 (set /a count_errors+=1
)
if !count_bank8! EQU 0 (set /a count_errors+=1
)

echo.

if !count_errors! EQU 1 (
  echo There a ONE critical error found!
  echo Failed banks were replaced with empty data
)

if !count_errors! GTR 1 (
  echo There are !count_errors! critical errors found!
  echo Failed banks were replaced with empty data
)

copy /b banks\Bank1.bin + banks\Bank5.bin + banks\Bank3.bin + banks\Bank7.bin + banks\Bank2.bin + banks\Bank6.bin + banks\Bank4.bin + banks\Bank8.bin PicoROM.BIN > NUL
echo.
echo PicoROM.bin generated. Use it for programming PicoROM

set /p answer=Do you want to program it now? (Y/N):

if /i "%answer%"=="Y" (
    echo Running the PicoROM tool, using "PicoROM" as the device name...
    call picorom-x86_64-pc-windows-msvc.exe upload -s PicoROM ./PicoROM.BIN 2mbit
) else (
    echo Skipping programming.
)
echo.
echo Done.

pause