@echo off
set CYGWIN=nodosfilewarning

cd /D %0\..\bin
bash.exe %0\..\src\prepare_stimulus_board.sh %1 %0\..

REM uncomment the pause below to troubleshoot.
REM pause