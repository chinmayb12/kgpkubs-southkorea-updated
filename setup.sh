echo "=================================================="
echo "Common setup (Each time)"
echo "=================================================="
read -p "Continue [Y/n]? "
if [[ $REPLY = [yY] || $REPLY = "" ]] 
	then
		echo sudo apt-get update --fix-missing
		# sudo apt-get upgrade --fix-missing
		sudo apt-get install build-essential --fix-missing
		echo -e "Done\n\n"
fi
echo -e "\nDone\n\n"

echo "=================================================="
echo "Installing flex and bison (One time)"
echo "=================================================="
read -p "Continue [Y/n]? "
if [[ $REPLY = [yY] || $REPLY = "" ]] 
	then
		sudo apt-get install flex bison --fix-missing
fi
echo -e "\nDone\n\n"

# echo "=================================================="
# echo "Installing git (One time)"
# echo "=================================================="
# read -p "Continue [Y/n]? "
# if [[ $REPLY = [yY] || $REPLY = "" ]] 
# 	then
# 		sudo apt-get install git-cola --fix-missing
# 		echo -e -n "Enter your full name: "
# 		read -e name
# 		echo -e -n "Enter your email address: "
# 		read -e email
# 		git config --global user.name \"$name\"
# 		git config --global user.email \"$email\"
# fi
# echo -e "\nDone\n\n"

echo "=================================================="
echo "Installing google protocol buffer (One time)"
echo "=================================================="
read -p "Continue [Y/n]? "
if [[ $REPLY = [yY] || $REPLY = "" ]] 
	then
		wget http://protobuf.googlecode.com/files/protobuf-2.4.1.tar.bz2
		tar -xvjf protobuf-2.4.1.tar.bz2
		cd protobuf-2.4.1
		./configure --prefix=/usr CFLAGS=-O3 CPPFLAGS=-O3
		make && make check && sudo make install || echo -en "\007"
		cd ..
		sudo rm protobuf-2.4.1.tar.bz2
		sudo rm -rf protobuf-2.4.1
fi
echo -e "\nDone\n\n"

echo "=================================================="
echo "Installing sslrefereebox (One time)"
echo "=================================================="
read -p "Continue [Y/n]? "
if [[ $REPLY = [yY] || $REPLY = "" ]] 
	then
		sudo apt-get install libgtkmm-2.4-dev make g++ gtk2-engines-pixbuf --fix-missing
		cd sslrefbox
		make && sudo make install && make clean || echo -en "\007"
		cd ..
fi
echo -e "\nDone\n\n"

echo "=================================================="
echo "Installing ssl vision (Each time)"
echo "=================================================="
read -p "Continue [Y/n]? "
if [[ $REPLY = [yY] || $REPLY = "" ]] 
	then
		sudo apt-get install g++ libqt4-dev libeigen2-dev libdc1394-22 libdc1394-22-dev cmake --fix-missing
		cd sslvision
		# Putting the executables into /usr/local/bin folder for easily starting the application
		make && sudo cp bin/vision /usr/local/bin && sudo cp bin/graphicalClient /usr/local/bin || echo -en "\007"
		cd ..
fi
echo -e "\nDone\n\n"

echo "=================================================="
echo "Installing grSim (Every time)"
echo "=================================================="
read -p "Continue [Y/n]? "
if [[ $REPLY = [yY] || $REPLY = "" ]] 
	then
		sudo apt-get install libqt4-dev g++ build-essential libgl1-mesa-dev libglu1-mesa-dev --fix-missing
		cd grSim/proto/pb
		bash compile.sh
		cd ../..
		# Putting the executables into /usr/local/bin folder for easily starting the application
		qmake && make && sudo cp bin/grSim /usr/local/bin || echo -en "\007"
		sudo rm /usr/local/bin/settings.xml
		cd ..
fi 
echo -e "\nDone\n\n"

echo "=================================================="
echo "Installing Codelite (One time)"
echo "=================================================="
read -p "Continue [Y/n]? "
if [[ $REPLY = [yY] || $REPLY = "" ]] 
	then
		echo -e '\E[37;40m'"[1] Go to http://sourceforge.net/projects/codelite/files/Releases/codelite-3.5/"
		echo "[2] Select codelite_3.5.5375-ubuntu0_i386.deb from the list. This will initiate the download."
		echo "[3] Double click on the downloaded .deb file to start installation."; tput sgr0
fi
echo -e "\nDone\n\n"

