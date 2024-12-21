echo off
if "%1" == "/rem" goto REMOVE 
if "%1" == "/REM" goto REMOVE 
C:\VOYETRA\VAPITG.COM /com:1 /addr:3f8
C:\VOYETRA\SAPI!ADG.COM /port:3 %1 %2 %3
goto OUT 
:REMOVE 
C:\VOYETRA\SAPI!ADG.COM /port:3 /rem
C:\VOYETRA\VAPITG.COM /rem
:OUT 
