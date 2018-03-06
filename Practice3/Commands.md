# Practice
Authors
 - David Antuña Rodríguez
 - Javier Carrión García


# OpenSSL

## Creación de una CA

```sh
mkdir demoCA
mkdir demoCA/newcerts
mkdir demoCA/private
touch demoCA/index.txt
echo 01 > demoCA/serial
echo 01 > demoCA/crlnumber
openssl req -x509 -new -days 3650 -keyout demoCA/private/cakey.pem -out demoCA/cacert.pem
```

------
## Creación de solicitudes de firma de certificado

```sh
openssl req -new -keyout userkey1.pem -out usercsr1.pem     //CN: usuario1
openssl req -in usercsr1.pem -noout -text>usercsr1.txt
openssl req -verify -in usercsr1.pem
openssl req -new -keyout userkey2.pem -out usercsr2.pem     //CN: usuario2
openssl req -in usercsr2.pem -noout -text > usercsr2.txt
openssl req -verify -in usercsr2.pem
```

------
## Creación y verificación de certificados

```sh
openssl ca -in usercsr1.pem -out usercert1.pem
openssl verify -CAfile demoCA/cacert.pem usercert1.pem
openssl ca -in usercsr2.pem -out usercert2.pem
openssl verify -CAfile demoCA/cacert.pem usercert2.pem
```

------
## Consulta y manipulación de certificados

```sh
openssl x509 -in usercert1.pem -noout -text > usercert1.txt
openssl x509 -in usercert1.pem -noout -pubkey > usercert1pubkey.txt
openssl x509 -in usercert2.pem -noout -text > usercert2.txt
openssl x509 -in usercert2.pem -noout -pubkey > usercert2pubkey.txt
openssl x509 -indemoCA/cacert.pem -noout -text  cacert.txt
openssl x509 -in demoCA/cacert.pem -noout -pubkey > cacertpubkey.txt
```

_PEM_
```sh
openssl x509 -in usercert1.pem -out usercert1.der -outform DER
openssl x509 -in usercert1.der -inform DER -out usercert1.pem
openssl pkcs12 -export -in usercert1.pem -inkey userkey1.pem -out usercert1.p12
```

------
## Revocación de certificados

```sh
openssl ca -revoke usercert2.pem
openssl ca -gencrl -out crl.pem
openssl crl -in crl.pem -noout -text>crl.txt
openssl crl -CAfile demoCA /cacert.pem -in crl.pem
openssl verify -crlcheck -CAfile demoCA/cacert.pem -CRLfile crl.pem usercert2.pem
```

------
# GnuPG

## Firma de claves (Web of trust)

```sh
gpg2 --gen-key       //nombre: claveA, psw: seguridad
gpg2 --import pubkeysSER.gpg
gpg2 --sign-key --local-user idA idB
gpg2 --sign-key --local-user idB idC
gpg2 --edit-key idB trust quit      //Nivel 4
gpg2 --edit-key idC trust quit      //Nivel 1
gpg2 --check-trustdb > checktrustdb.txt
gpg2 --list-options show-uid-validity –list-keys > validity.txt
```

```sh
gpg2 --edit-key idB trust quit       //Nivel 3
gpg2 --check-trustdb > checktrustdb2.txt
gpg2 --list-options show-uid-validity –list-keys > validity2.txt
```

```sh
gpg2 --sign-key --local-user idA idD
gpg2 --sign-key --local-user idA idE
gpg2 --edit-key idD trust quit      //Nivel 3
gpg2 --edit-key idE trust quit      //Nivel 3
gpg2 --sign-key --local-user idD idC
gpg2 --sign-key --local-user idE idC
gpg2 --check-trustdb > checktrustdb3.txt
gpg2 --list-options show-uid-validity –list-keys > validity3.txt
```