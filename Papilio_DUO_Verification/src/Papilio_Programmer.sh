# Copyright 2009-2011 Jack Gassett
# Creative Commons Attribution license
# Made for the Papilio FPGA boards

COMPORT=COM6

# putty.exe -serial $COMPORT &
sleep 1
		echo -e "\e[1;33mStarting Papilio DUO Test\e[0m"
		# Find device id and choose appropriate bscan bit file
	
		device_id=`./papilio-prog.exe -j | ./gawk '{print $9}'`
		return_value=$?
		
		case $device_id in
			XC3S250E)
				echo -e "\e[1;33mThis test is not for the Papilio One 250K\e[0m"
				exit
				;;	
			XC3S500E)
				echo -e "\e[1;33mThis test is not for the Papilio One 500K\e[0m"
				exit
				;;
			XC6SLX4)
				echo -e "\e[1;33mPapilio Pro LX4 detected\e[0m"
				echo -e "\e[1;33mThis test is not for the Papilio Pro LX4\e[0m"
				exit			
				;;			
			XC6SLX9)
				echo -e "\e[1;33mPapilio DUO LX9 detected\e[0m"
				bitfile=Papilio_Test_Plan_Board_Under_Test_LX9.bit
				bscan_bitfile=bscan_spi_xc6slx9.bit
				arduino_ISP=Papilio_DUO_ArduinoISP.bit
				quickstartbitfile=QuickStart-zpuino-1.0-PapilioDUO-S6LX9-1.0.bit
				memtestbitfile=Mem-IO-Test-zpuino-1.0-PapilioDUO-S6LX9-1.0.bit
				;;						
			*)
				echo -e "\e[1;31mUnknown Papilio Board\e[0m"
				;;
		esac

############################################################################		
		#Load the Papilio DUO bootloader to the ATmega32u4
		echo -e "\e[1;33mLoading ArduinoISP\e[0m"
		./papilio-prog.exe -v -f $arduino_ISP -v
		echo -e "\e[1;33mWriting Fuses to ATmega32u4\e[0m"
		./avrdude -q -q -patmega32u4 -cstk500v1 -P$COMPORT -b57600 -e -Ulock:w:0x3F:m -Uefuse:w:0xcb:m -Uhfuse:w:0xd8:m -Ulfuse:w:0xff:m
		echo -e "\e[1;33mLoading Bootloader to ATmega32u4\e[0m"
		echo -e "\e[1;31mPlease verify that you see RX, TX, and LED LEDs Light up.\e[0m"
		./avrdude  -q -q -patmega32u4 -cstk500v1 -P$COMPORT -b57600 -Uflash:w:Caterina-Papilio-DUO.hex:i -Ulock:w:0x2F:m
		sleep 5
		echo -e "\e[1;31mCheck the list of COM PORTS and make sure Papilio DUO bootloader shows up.\e[0m"
		./listComPorts.exe		
			
#####################################################################################################				
		echo -e "\e[1;33mLoading Memory and IO test to FPGA\e[0m"
		./papilio-prog.exe -v -f $memtestbitfile -b $bscan_bitfile -sa -r
		return_value=$?	
		#Cause the Papilio to restart
		./papilio-prog.exe -c
		# return_value=$?	
		
if [ $return_value == 1 ] #If programming failed then show error.
then
	./dialog --timeout 5 --msgbox "The bit file failed to program to the Papilio, please check any issues with the FTDI chip, USB connector, voltage regulators, or solder joints.\e[0m" 15 55
	read -n1 -r -p "Press any key to continue...\e[0m" key
fi		
		
echo
echo
echo -e "\e[1;31m\e[1;31mConnect the Stimulus board now and complete the I/O and Memory test close the Putty window when done to continue.\e[0m"
#echo m > \\.\com6:
./putty -serial $COMPORT -sercfg 9600,8,n,1,N

####################################################################################################				
		echo -e "\e[1;33mLoading Quickstart bit file to FPGA\e[0m"
		./papilio-prog.exe -v -f $quickstartbitfile -b $bscan_bitfile -sa -r
		return_value=$?	
		#Cause the Papilio to restart
		./papilio-prog.exe -c
		# return_value=$?	
		
if [ $return_value == 1 ] #If programming failed then show error.
then
	./dialog --timeout 5 --msgbox "The bit file failed to program to the Papilio, please check any issues with the FTDI chip, USB connector, voltage regulators, or solder joints.\e[0m" 15 55
	read -n1 -r -p "Press any key to continue...\e[0m" key
fi		

read -n1 -r -p "Verification is complete, please press any key to quit." key