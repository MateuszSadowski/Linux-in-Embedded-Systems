#!/bin/sh
#function to display commands
print() { echo "\$ $@" ; "$@" ; }

workingdirpath=`echo $PWD`
while true; do
    read -p "Do you wish to download and install Buildroot?(y/n) " yn
    case $yn in
        [Yy]* )	print wget https://buildroot.org/downloads/buildroot-2018.02.tar.bz2;
				print tar -xjf buildroot-2018.02.tar.bz2;
				destdirpath=buildroot-2018.02;
				print cd $destdirpath;
				echo "Initalizing with configuration for RaspberryPi...";
				print make raspberrypi_defconfig;
				break;;
        [Nn]* ) 
				read -p "Provide path to Buildroot root directory: " destdirpath
				print cd $destdirpath
				break;;
    esac
done

echo "Copying configuration files...";
print cp $workingdirpath/.config $destdirpath;
print cp -r $workingdirpath/mini $destdirpath/board;
echo "Done copying configuration files. Building image...";
print make;
echo "Done. Target image is located at ./buildroot-2018.02/output/images"
