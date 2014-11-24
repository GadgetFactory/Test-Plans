# Copyright 2009-2011 Jack Gassett
# Creative Commons Attribution license
# Made for the Papilio FPGA boards

echo -e "\e[1;33mStarting Papilio DUO Stimulus Board preparation.\e[0m"
COMPORT=`./listComPorts.exe | ./gawk '{print $1}'`
#Uncomment this if you want to override the com port - in case it does not find it correctly
#COMPORT=COM6
echo -e "\e[1;33mSeeeduino ADK detected on \e[1;31m$COMPORT.\e[0m"


echo -e "\e[1;33mWriting sketch to Stimulus Board\e[0m"
./avrdude -patmega2560 -cwiring -P$COMPORT -b115200 -D -Uflash:w:Papilio_DUO_Test_Plan_Stimulus_Board.cpp.hex:i 

read -n1 -r -p "Stimulus board is programmed, please press any key to quit." key