# Copyright 2009-2011 Jack Gassett
# Creative Commons Attribution license
# Made for the Papilio FPGA boards

		echo "Step 1 - Programming the Stimulus file to Papilio"
		# Find device id and choose appropriate bscan bit file
	
		device_id=`./papilio-prog.exe -j | ./gawk '{print $9}'`
		return_value=$?
		
		case $device_id in
			XC3S250E)
				echo "Programming a Papilio One 250K"
				bitfile=Papilio_Test_Plan_Stimulus_Board_250K.bit
				bscan_bitfile=bscan_spi_xc3s250e.bit
				quickstartbitfile=Quickstart-Papilio_One_250K-v1.5.bit
				;;	
			XC3S500E)
				echo "Programming a Papilio One 500K"
				bitfile=Papilio_Test_Plan_Stimulus_Board_250K.bit
				bscan_bitfile=bscan_spi_xc3s500e.bit
				quickstartbitfile=Quickstart-Papilio_One_500K-v1.5.bit
				;;
			XC6SLX9)
				echo "Papilio Plus LX9 not supported yet"
				exit
				;;				
			*)
				echo "Unknown Papilio Board"
				;;
		esac

		./papilio-prog.exe -v -f $bitfile -b $bscan_bitfile -sa -r
		#Cause the Papilio to restart
		./papilio-prog.exe -c
		return_value=$?
		
if [ $return_value == 1 ] #If programming failed then show error.
then
	./dialog --timeout 5 --msgbox "The bit file failed to program to the Papilio, please check any issues with the FTDI chip, USB connector, voltage regulators, or solder joints." 15 55
	read -n1 -r -p "Press any key to continue..." key
fi

echo
echo
echo "The Stimulus file has been loaded to SPI Flash" 
read -n1 -r -p "Press any key to quit." key