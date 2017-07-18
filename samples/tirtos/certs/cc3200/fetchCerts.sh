#!/bin/bash
#deviceName=$(sudo cat ../../subscribe_publish_sample/cc3200/config.h | grep "DeviceName" | awk '{print $2}')
deviceName=$(sudo cat ../../../../deviceName.h | grep "DeviceName " | awk -F\" '{print $2}')

certpem=$(while read -r line; do   echo "\"$line\"" |sed -n '/CERTIFICATE/!p' ; done < "../../../../certs/$deviceName-certificate.pem.crt");

privatekey=$(while read -r line; do   echo "\"$line\"" |sed -n '/PRIVATE/!p' ; done <"../../../../certs/$deviceName-private.pem.key");

rootcert=$(while read -r line; do   echo "\"$line\"" |sed -n '/CERTIFICATE/!p' ; done <"../../../../certs/rootCA.crt");

echo "const char root_ca_pem[] = $rootcert ;" >> configure.h;
echo "const char client_private_key_pem[] = $privatekey ;" >> configure.h;
echo "const char client_cert_pem[] = $certpem ;" >> configure.h;
