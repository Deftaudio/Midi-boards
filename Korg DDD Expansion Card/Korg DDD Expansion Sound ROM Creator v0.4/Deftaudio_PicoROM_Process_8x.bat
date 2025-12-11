@echo off
setlocal EnableDelayedExpansion

if exist temp rmdir /s /q .\temp

cls
echo ######################################################################
echo # Deftaudio PicoROM Korg DDD Expansion Sound ROM Encoder (8 samples) #
echo ######################################################################
echo.

if "%~1"=="" (
    echo Usage: %~nx0 folderpath
    exit /b 1
)

set "folder=%~1"

rem -- check presence of any WAVs
if not exist %folder%\*.WAV (
 	echo Critical Error: No WAVs found! Aborting!
 	echo.
 	pause
 	EXIT /b 1
)

set "filecount=0"

for %%f in ("%folder%\*.wav") do (
    set /a filecount+=1
)

if "%filecount%" NEQ "8" (
    echo Not 8 files in directory!
		echo Files: %filecount%. Exiting...
		exit /b 1
)


rem -- checking for mandatory tools and program data

if not exist .\tools\sox\sox.exe (
	echo Critical Error: SoX cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
if not exist .\tools\soxdither\sox.exe (
	echo Critical Error: SoX cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
if not exist .\tools\meta\8x_1.ddd (
	echo Critical Error: Metadata 1 cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
if not exist .\tools\meta\8x_2.ddd (
	echo Critical Error: Metadata 2 cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
if not exist .\tools\meta\8x_3.ddd (
	echo Critical Error: Metadata 3 cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
if not exist .\tools\meta\8x_4.ddd (
	echo Critical Error: Metadata 4 cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
if not exist .\tools\meta\8x_5.ddd (
	echo Critical Error: Metadata 5 cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
if not exist .\tools\meta\8x_6.ddd (
	echo Critical Error: Metadata 6 cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
if not exist .\tools\meta\8x_7.ddd (
	echo Critical Error: Metadata 7 cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
if not exist .\tools\meta\8x_8.ddd (
	echo Critical Error: Metadata 8 cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
if not exist .\tools\meta\8x_9.ddd (
	echo Critical Error: Metadata 9 cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)


rem -- preparing tools, folders, and metadata
mkdir .\temp
mkdir .\banks

rem -- WAV handling stage
echo Processing files in: "%folder%"
echo.
rem -- silence (anything less than 0.20%) is trimmed from start of samples until 1 sample of non-silence is encountered

REM === Loop through files alphabetically ===
for /f "delims=" %%F in ('dir "%folder%\*.wav" /b /a-d /on') do call :process_file "%%F"
goto :after_loop

:process_file
set "fname=%~n1"
set "ext=%~x1"

REM === Truncate to 8 chars or pad with underscores ===
set "shortname=%fname%"
if not "%shortname%"=="" set "shortname=%shortname:~0,8%"
:pad_loop
if not "%shortname:~7,1%"=="" goto :done_padding
set "shortname=%shortname% "
goto :pad_loop
:done_padding

echo Processing File: %~1
echo Short name (8 chars): "%shortname%.wav"

REM --- Replace with your actual external command ---
echo Conforming samplerate and lengths...
.\tools\sox\sox.exe -V1 -D --norm=-0.3 "%folder%\%~1" ".\temp\%shortname%.wav" rate 44100 channels 1 silence 1 1 0.20% fade p 2s 4000s 50s

echo Dithering to 8-bit and converting WAVs into raw PCM data...
.\tools\soxdither\sox.exe -V1 ".\temp\%shortname%.wav" --bits 8 --encoding unsigned-integer ".\temp\%shortname%.raw" dither -r
echo .

goto :eof

:after_loop

echo.
echo Sorting processed files by short name...
rem --- Create a sorted list of temp files by short name
(for /f "delims=" %%F in ('dir /b /on temp\*.raw') do (
    echo %%F
)) > temp\filelist.txt


rem --- Merge files according to sorted short names
echo Merging files...


set "merged_file=%folder%"
REM --- Remove trailing backslash if any ---
if "%merged_file:~-1%"=="\" set "merged_file=%merged_file:~0,-1%"
REM --- Extract last folder name ---
for %%A in ("%merged_file%") do set "merged_file=%%~nxA"

set "merged_file=banks\%merged_file%.bin"

REM --- Delete existing merged file if exists
if exist "%merged_file%" del "%merged_file%"

set "count=1"

for /f "delims=" %%F in (temp\filelist.txt) do (
	type "tools\meta\8x_!count!.ddd" >> "%merged_file%"

	rem --- Create a temporary file containing only the 8-character short name
	(echo|set /p=%%~nF) > temp\name.tmp

	rem --- Append the short name exactly
	type temp\name.tmp >> "%merged_file%"

	set /a count+=1
)

type "tools\meta\8x_9.ddd" >> "%merged_file%"

REM --- Loop through file list and copy contents
for /f "delims=" %%F in (temp\filelist.txt) do (

		rem --- Append file contents (binary) to merged file
    type "temp\%%F" >> "%merged_file%"
		type "tools\meta\57.ddd" >> "%merged_file%"
)

REM Append last 4 bytes
type "tools\meta\4.ddd" >> "%merged_file%"

echo Merge completed: %merged_file%
echo .
echo Veryfying ROM size


rem -- check files sizes of created ROM
if exist %merged_file% (
  for %%G in (%merged_file%) do set "file_sizeA=%%~zG"
)
if defined file_sizeA if %file_sizeA% NEQ 32768  (
  echo Warning: DDD expansion sound ROM is incorrect size of %file_sizeA%!
) else (
  echo Success: DDD expansion sound ROM appears to be correct size.
)
echo.
rem pause
rmdir /s /q .\temp
