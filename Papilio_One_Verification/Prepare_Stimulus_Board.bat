@echo off
set CYGWIN=nodosfilewarning

cd /D %0\..\bin
bash.exe %0\..\src\Stimulus_Papilio_Programmer.sh %1 %0\..

REM uncomment the pause below to troubleshoot.
REM pause