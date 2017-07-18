#!/bin/bash

path=`pwd`

getting_unique()
{
realcounts=$(curl --data "deviceName="$devicename"" http://ec2-52-41-46-86.us-west-2.compute.amazonaws.com/paasmerv2DevAvailability.php)
echo $realcounts
if [ $realcounts = 0 ]
then
	echo "accepted"
else
	echo "devicename already exist"
	read -r -p "do you want to continue with another name or you want to exit? [continue/exit] " status
	if [ $status == "continue" ]
	then
    		getting_devicename
	else
		exit
	fi
fi
}
getting_devicename ()
{
read -r -p "Please enter the device name you want:[alphanumeric only(a-z A-Z 0-9)] " devicename
echo $devicename
if [ ${#devicename} != 0 ]
then
	getting_unique
else
	getting_devicename
fi
}
getting_devicename

cat > .Install.log << EOF3
Logfile for Installing Paasmer...
EOF3

logname=$(who | awk '{print $1}');

echo $path >> .Install.log
user=$(echo $USER)
user='/home/'$user
echo $user >> .Install.log
echo "--> Installing...\n" >> .Install.log
sudo chmod -R 777 ./*

echo "--> Installing requerments......." >> .Install.log


sudo pip install awscli
echo "Configuring data..." >> .Install.log

sudo mkdir -p /root/.aws
sudo chmod -R 777 /root/.aws
sudo mkdir -p $path/certs
sudo chmod -R 777 $path/certs

cat > /root/.aws/config << EOF1
[default]
region = us-west-2
EOF1
echo "U2FsdGVkX1+WF++BqX9N+Bfu/jsDgfM9rxd77LO3I8xVxgLBbNmglZprOCtcyvJs
Jteh6FPrLMKb4r8uSq6C/w==" > .old.txt
accesskey=$(cat .old.txt | openssl enc -aes-128-cbc -a -d -salt -pass pass:asdfghjkl);

keyid=$(echo "U2FsdGVkX19XbOtwglyiBxjyEME74FjnlS5KrbdvXHQGbUC/BulYsgg+a35BR64W" | openssl enc -aes-128-cbc -a -d -salt -pass pass:asdfghjkl);

echo "[default]
aws_secret_access_key = $accesskey
aws_access_key_id = $keyid
" > /root/.aws/credentials



endpoint=$(sudo su - root -c"aws iot describe-endpoint" | grep "endpoint" | awk '{print $2}');
echo $endpoint >> .Install.log

touch $path/certs/output.txt

PAASMER=$devicename;
sudo mkdir -p $path/$PAASMER;
echo $PAASMER >> .Install.log
Thingjson=$(sudo su - root -c "aws iot create-thing --thing-name $PAASMER");
echo $Thingjson >> .Install.log

touch $path/certs/output.txt
sudo su - root -c "aws iot create-keys-and-certificate --set-as-active --certificate-pem-outfile $path/certs/$PAASMER-certificate.pem.crt --public-key-outfile $path/certs/$PAASMER-public.pem.key --private-key-outfile $path/certs/$PAASMER-private.pem.key" > $path/certs/output.txt

sudo chmod -R 777 ./*
cat $path/certs/output.txt >> .Install.log

out=$(sudo cat $path/certs/output.txt | grep "certificateArn" | awk '{print $2}')


ARN=$(echo $out | sed 's/,$//')
echo $ARN >> .Install.log

sudo su - root -c "aws iot create-policy --policy-name $devicename --policy-document '{ \"Version\": \"2012-10-17\", \"Statement\": [{\"Action\": [\"iot:*\"], \"Resource\": [\"*\"], \"Effect\": \"Allow\" }] }'" >> .Install.log




sudo su - root -c "echo \"alias PAASMER_THING='sudo aws iot attach-thing-principal --thing-name $devicename --principal $ARN'\" >> /root/.bashrc"
sudo su - root -c "echo \"alias PAASMER_POLICY='sudo aws iot attach-principal-policy --policy-name $devicename --principal $ARN'\" >> /root/.bashrc"
echo "Added to PAASMER alias...\n" >> .Install.log




sudo chmod -R 777 ./*

echo "************************************************************"
echo "-----------------------------------------------------------"
echo "-->  Run below commands.."
echo "-->  1) sudo su "
echo "-->  2) source ~/.bashrc "
echo "-->  3) PAASMER_THING "
echo "-->  4) PAASMER_POLICY "
echo "-->  5) sed -i 's/alias PAASMER/#alias PAASMER/g' ~/.bashrc "
echo "-->  6) exit "

echo "**************************************************************";
echo "After device registration, edit the config file with credentials and feed details"

echo "File Transfered successfully...." >> .Install.log
echo "#define DeviceName \"$PAASMER\"" > $path/deviceName.h
sudo chmod 777 ./*
echo $PAASMER >> .Install.log
