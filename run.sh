#!/bin/sh

function create_fingerprint {
	read -p "Please input file name: " file_name
	device_path=$(sudo fdisk -l | grep Device -A2 | tail -n1 | awk '{print $1}')
	echo "Latest USB device path: $device_path"
	if [[ "$device_path" == *"/dev/mmcblk"* ]];
	then
		echo "Can't detect USB device ..."	
	else
		sleep 1
		echo "Start creating fingerprint ..."
		sudo ./create_fingerprint "$device_path" "$file_name"
	fi
}

function analyzer {
	device_path=$(sudo fdisk -l | grep Device -A2 | tail -n1 | awk '{print $1}')
	echo "Latest USB device path: $device_path"
	sleep 1
	echo "Start running analyzer ..."
	sudo ./analyzer "$device_path"
}

function menu {
	echo 
	echo -e "\t1. Create finger print for latest USB device"
	echo -e "\t2. Run analyzer (fingerprint authentication + packet filter)"
	echo -e "\t3. Exit"
	echo -e "\n\tPlease enter your selection:"
	read -n1 option \n
	echo
}

make

sleep 3

while [ 1 ]
do menu
	case $option in
		1)
			create_fingerprint ;;
		2)
			analyzer ;;
		3)
			break ;;
		*)
			echo "Wrong selection ..." ;;
	esac
done
echo "Exiting the analyzer ..."
make clean
