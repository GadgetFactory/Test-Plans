# Copyright 2009-2011 Jack Gassett
# Creative Commons Attribution license
# Made for the Papilio FPGA boards

echo -e "\e[1;33mStarting Papilio DUO Test Plan\e[0m"

#erase any FTDI eeprom to make sure we are at ground 0
#./FT_Prog-CmdLine scan eras 0 cycl 0
#################################################
echo -e "\e[1;33mWriting FT2232H eeprom settings\e[0m"
./FT_Prog-CmdLine scan prog 0 ft2232_eeprom.xml cycl 0
./sleep 5
./fteeprom.exe Papilio_DUO_EEPROM.hex
return_value=$?
if [ $return_value == 1 ] #If EEPROM write failed then show error.
then
	./dialog --msgbox "The EEPROM failed to write, please make sure the EEPROM has at least 128 Bytes." 15 55
	read -n1 -r -p "Press any key to exit, Test Plan has failed..." key
	exit
fi	
./FT_Prog-CmdLine scan cycl 0
./sleep 3

#Find the papilio FPGA com port
COMPORT=`./listComPorts.exe -papilio | ./gawk '{print $1}'`
#Uncomment this if you want to override the com port - in case it does not find it correctly
#COMPORT=COM6

./listComPorts.exe -papilio > /dev/null
return_value=$?
if [ $return_value == 1 ] #If no Papilio COM Port found then show error.
then
	./dialog --msgbox "No Papilio COM Port detected. Manually set the COM port in src/Papilio_Programmer.sh" 15 55
fi	

# putty.exe -serial $COMPORT &
./sleep 1
		echo -e "\e[1;33mPapilio FPGA detected on \e[1;31m$COMPORT.\e[0m"
		echo -e "\e[1;31mIf the correct COM port is not automatically detected then please open src/Papilio_Programmer.sh and specify the COM port manually.\e[0m"
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
		./sleep 3
		echo -e "\e[1;33mWriting Fuses to ATmega32u4\e[0m"
		./avrdude -q -q -patmega32u4 -cstk500v1 -P$COMPORT -b57600 -e -Ulock:w:0x3F:m -Uefuse:w:0xcb:m -Uhfuse:w:0x98:m -Ulfuse:w:0xff:m
		echo -e "\e[1;33mLoading Bootloader to ATmega32u4\e[0m"
		echo -e "\e[1;31mPlease verify that you see LED, RX, and TX LEDs Light up. \e[0m"
		./avrdude -F -q -q -patmega32u4 -cstk500v1 -P$COMPORT -b57600 -Uflash:w:Caterina-Papilio-DUO.hex:i -Ulock:w:0x2F:m
		./sleep 5
		#echo -e "\e[1;31mCheck the list of COM PORTS and make sure Papilio DUO bootloader shows up.\e[0m"
		#./listComPorts.exe	
		BOOTLOADERCOMPORT=`./listComPorts.exe -bootloader | ./gawk '{print $1}'`
		return_value=$?
		echo -e "\e[1;33mPapilio DUO Arduino bootloader detected on \e[1;31m$BOOTLOADERCOMPORT\e[0m"
		if [ $return_value == 1 ] #If no arduino bootloader found then show error.
		then
			./dialog --msgbox "No Arduino bootloader detected on the ATmega32u4. Please make sure the microUSB cable is plugged in, otherwise this is a fail for the Test Plan." 15 55
			read -n1 -r -p "No Arduino bootloader detected, Test Plan failed. Press any key to continue..." key
		fi	
		
		#Loading Blink sketch to ATmega32u4
		./sleep 5
		echo -e "\e[1;33mLoading an example blink sketch to ATmega32u4\e[0m"
		./avrdude -F -q -q -patmega32u4 -cavr109 -P$BOOTLOADERCOMPORT -b57600 -D -Uflash:w:Papilio_DUO_AVR_Blink.cpp.hex:i
		return_value=$?
		#echo -e "\e[1;33mPapilio DUO Arduino sketch detected on \e[1;31m$BOOTLOADERCOMPORT\e[0m"
		if [ $return_value == 1 ] #If no arduino sketch found then show error.
		then
			./dialog --msgbox "No Arduino sketch loaded on the ATmega32u4. Please make sure the microUSB cable is plugged in, otherwise this is a fail for the Test Plan." 15 55
			read -n1 -r -p "No Arduino sketch loaded, Test Plan failed. Press any key to continue..." key
		fi		
			
#####################################################################################################				
		echo -e "\e[1;33mLoading Memory and IO test to FPGA\e[0m"
		./papilio-prog.exe -v -f $memtestbitfile -b $bscan_bitfile -sa -r
		return_value=$?	
		#Cause the Papilio to restart
		./papilio-prog.exe -c
		# return_value=$?	
		
if [ $return_value == 1 ] #If programming failed then show error.
then
	./dialog --msgbox "The bit file failed to program to the Papilio, please check any issues with the FTDI chip, USB connector, voltage regulators, or solder joints." 15 55
	read -n1 -r -p "Press any key to continue..." key
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
	./dialog --msgbox "The bit file failed to program to the Papilio, please check any issues with the FTDI chip, USB connector, voltage regulators, or solder joints" 15 55
	read -n1 -r -p "Press any key to continue..." key
fi		

read -n1 -r -p "Verification is complete, please press any key to quit." key