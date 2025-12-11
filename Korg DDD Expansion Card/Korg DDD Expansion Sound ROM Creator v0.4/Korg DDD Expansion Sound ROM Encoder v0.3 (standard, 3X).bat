@echo off
if exist temp rmdir /s /q .\temp
cls
echo ##############################################
echo # Korg DDD Expansion Sound ROM Encoder v0.30 # (3 samples)
echo ##############################################
echo.
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
if not exist .\tools\meta\3x.ddd (
	echo Critical Error: Metadata cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
rem -- check presence of any WAVs
if not exist .\[WAV_IN]\*.WAV (
 	echo Critical Error: No WAVs found! Aborting!
 	echo.
 	pause
 	EXIT /B
)
rem -- check each individual WAV
if not exist .\[WAV_IN]\1.WAV (
	echo Critical Error: 1.WAV cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
if not exist .\[WAV_IN]\2.WAV (
	echo Critical Error: 2.WAV cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
if not exist .\[WAV_IN]\3.WAV (
	echo Critical Error: 3.WAV cannot be found. Aborting!
	echo.
	pause
	EXIT /B
)
echo Place three WAV files into "[WAV_IN]" with the following naming scheme: 1.WAV,
echo 2.WAV, and 3.WAV. For ideal results, each sound should be shorter than 0.42
echo seconds in length. WAVs of any bit-depth or sample rate are allowed.
echo.
pause
echo.
rem -- preparing tools, folders, and metadata
mkdir .\temp
copy .\tools\meta\*.* .\temp >NUL
rem -- WAV handling stage
echo Conforming samplerate and lengths...
echo.
rem -- silence (anything less than 0.20%) is trimmed from start of samples until 1 sample of non-silence is encountered
.\tools\sox\sox.exe -V1 -D --norm=-0.3 .\[WAV_IN]\1.WAV .\temp\1_RT.WAV rate 25640 channels 1 silence 1 1 0.20% fade p 2s 10820s 50s
.\tools\sox\sox.exe -V1 -D --norm=-0.3 .\[WAV_IN]\2.WAV .\temp\2_RT.WAV rate 25640 channels 1 silence 1 1 0.20% fade p 2s 10820s 50s
.\tools\sox\sox.exe -V1 -D --norm=-0.3 .\[WAV_IN]\3.WAV .\temp\3_RT.WAV rate 25640 channels 1 silence 1 1 0.20% fade p 2s 10820s 50s

echo Dithering to 8-bit and converting WAVs into raw PCM data...
echo.
.\tools\soxdither\sox.exe -V1 .\temp\1_RT.WAV --bits 8 --encoding unsigned-integer .\temp\1.RAW dither -r
.\tools\soxdither\sox.exe -V1 .\temp\2_RT.WAV --bits 8 --encoding unsigned-integer .\temp\2.RAW dither -r
.\tools\soxdither\sox.exe -V1 .\temp\3_RT.WAV --bits 8 --encoding unsigned-integer .\temp\3.RAW dither -r

echo Merging singular raw waves into Korg DDD ROM image...
cd temp
copy /b 3x.ddd + 1.RAW + 2.RAW + 3.RAW DDD_3X.BIN >NUL
move DDD_3X.BIN .. >NUL
cd..
echo.
echo Procedure complete.
echo.
echo Please check that the resulting Korg DDD external sound ROM file is 32,768
echo bytes in size exactly. If the ROM is not the correct size then the DDD will
echo not function as intended, and the ROM should not be burned.
echo.

rem -- check files sizes of created ROM
if exist "DDD_3X.BIN" (
  for %%G in ("DDD_3X.BIN") do set "file_sizeA=%%~zG"
)
if defined file_sizeA if %file_sizeA% NEQ 32768  (
  echo Warning: DDD expansion sound ROM is incorrect size of %file_sizeA%!
) else (
  echo Success: DDD expansion sound ROM appears to be correct size.
)
echo.
pause
rmdir /s /q .\temp
rem -- end of conversion routine
