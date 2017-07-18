#!/bin/sh
# Detects which OS and if it is Linux then it will detect which Linux
# Distribution.

path=`pwd`
OS=`uname -s`
REV=`uname -r`
MACH=`uname -m`
board=`uname -a`

GetVersionFromFile()
{
    VERSION=`cat $1 | tr "\n" ' ' | sed s/.*VERSION.*=\ // `
}
echo ${OS}
echo ${board}
#cd '/external_libs/mbedTLS/'
make clean --directory ./external_libs/mbedTLS/
make --directory ./external_libs/mbedTLS/
#cd ../../
if [ "${OS}" = "SunOS" ] ; then
    OS=Solaris
    ARCH=`uname -p` 
    OSSTR="${OS} ${REV}(${ARCH} `uname -v`)"

elif [ "${OS}" = "AIX" ] ; then
    OSSTR="${OS} `oslevel` (`oslevel -r`)"
elif [ "${OS}" = "Darwin" ] ; then
    OSSTR="${OS} ${DIST} ${REV}(${PSUEDONAME} ${KERNEL} ${MACH})"

elif [ "${OS}" = "Linux" ] ; then
    KERNEL=`uname -r`
    if [ -f /etc/redhat-release ] ; then
        DIST='RedHat'
        PSUEDONAME=`cat /etc/redhat-release | sed s/.*\(// | sed s/\)//`
        REV=`cat /etc/redhat-release | sed s/.*release\ // | sed s/\ .*//`
        sudo yum update
        sudo yum install -y python2.7 libssl-dev python-pip
        sudo yum install -y python-dev
        sudo yum install -y xterm
        sudo yum install -y expect
        sudo yum install -y mysql-client-core-5.7
	sudo yum install -y mysql-client

    elif [ -f /etc/SuSE-release ] ; then
        DIST=`cat /etc/SuSE-release | tr "\n" ' '| sed s/VERSION.*//`
        REV=`cat /etc/SuSE-release | tr "\n" ' ' | sed s/.*=\ //`
        sudo zypper update
        sudo zypper install -y python2.7 libssl-dev python-pip
        sudo zypper install -y python-dev
        sudo zypper install -y xterm
        sudo zypper install -y expect
        sudo zypper install -y mysql-client-core-5.7
	sudo zypper install -y mysql-client
    elif [ -f /etc/mandrake-release ] ; then
        DIST='Mandrake'
        PSUEDONAME=`cat /etc/mandrake-release | sed s/.*\(// | sed s/\)//`
        REV=`cat /etc/mandrake-release | sed s/.*release\ // | sed s/\ .*//`
    elif [ -f /etc/debian_version ] ; then
        DIST="Debian `cat /etc/debian_version`"
        REV=""
        sudo apt-get update
        sudo apt-get install -y python2.7 libssl-dev python-pip
        sudo apt-get install -y python-dev
        sudo apt-get install -y xterm
        sudo apt-get install -y expect
        sudo apt-get install -y mysql-client-core-5.7
	sudo apt-get install -y mysql-client
    elif [ -f /etc/lsb_version ] ; then
        DIST="ubuntu or Linux mint `cat /etc/lsb_version`"
        REV=""
        sudo apt-get update
        sudo apt-get install -y python2.7 libssl-dev python-pip
        sudo apt-get install -y python-dev
        sudo apt-get install -y xterm
        sudo apt-get install -y expect
        sudo apt-get install -y mysql-client-core-5.7
	sudo apt-get install -y mysql-client
    elif [ -f /etc/fedora_version ] ; then
        DIST="fedora `cat /etc/lsb_version`"
        REV=""
        sudo dnf update
        sudo dnf install -y python2.7 libssl-dev python-pip
        sudo dnf install -y python-dev
        sudo dnf install -y xterm
        sudo dnf install -y expect
        sudo dnf install -y mysql-client-core-5.7
	sudo dnf install -y mysql-client
    elif [ -f /etc/gentoo_version ] ; then
        DIST="gentoo `cat /etc/lsb_version`"
        REV=""
        sudo dnf update
        sudo dnf install -y python2.7 libssl-dev python-pip
        sudo dnf install -y python-dev
        sudo dnf install -y xterm
        sudo dnf install -y expect
        sudo dnf install -y mysql-client-core-5.7
	sudo dnf install -y mysql-client
    fi
    if [ -f /etc/UnitedLinux-release ] ; then
        DIST="${DIST}[`cat /etc/UnitedLinux-release | tr "\n" ' ' | sed s/VERSION.*//`]"
    fi

    OSSTR="${OS} ${DIST} ${REV}(${PSUEDONAME} ${KERNEL} ${MACH})"

fi

mac=$(ifconfig | grep 'HWaddr' |awk '{print $5}' | head -n 1)
echo "#define MAC \"$mac\"" > $path/samples/linux/subscribe_publish_sample/mac.h

echo ${OSSTR}
echo ${board}
echo ${board} | awk '{print $2}'
xv=$(echo ${board} | awk '{print $2}')
echo "#define deviceType \"$xv\"" > $path/samples/linux/subscribe_publish_sample/device.h
#wget -O ./certs/rootCA.crt https://www.symantec.com/content/en/us/enterprise/verisign/roots/VeriSign-Class%203-Public-Primary-Certification-Authority-G5.pem
echo $xv
if [ $xv = "raspberrypi" ] || [ $xv = "bananapi" ] || [ $xv = "orangepi" ] || [ $xv = "odroidxu4" ] ; then
   echo 'correct'
  # sudo tar -xf wiringPi.tar -C ../
   #sudo chmod -R 777 ../../*
   #sudo tar -xf subscribe_publish_sample.tar -C $path/samples/linux/
else if [ $xv = "BBC" ] ; then
   #sudo apt-get install build-essential python-setuptools python-smbus
   echo 'BBC'

else
   echo 'wrong'
fi
fi
