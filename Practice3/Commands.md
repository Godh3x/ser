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
openssl req -in usercsr1.pem -noout -text > usercsr1.txt
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
openssl x509 -in demoCA/cacert.pem -noout -text > cacert.txt
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
openssl crl -in crl.pem -noout -text > crl.txt
openssl crl -CAfile demoCA/cacert.pem -in crl.pem
openssl verify -crl_check -CAfile demoCA/cacert.pem -CRLfile crl.pem usercert2.pem
```

------
# GnuPG

## Firma de claves (Web of trust)

```sh
gpg2 --gen-key       //nombre: claveA, psw: seguridad
gpg2 --import pubkeysSER.gpg
gpg2 --sign-key --local-user 2DDAF451 F99D95F1
gpg2 --sign-key --local-user F99D95F1 2B42A0BF
gpg2 --edit-key F99D95F1 trust quit      //Nivel 4
gpg2 --edit-key 2B42A0BF trust quit      //Nivel 1
gpg2 --check-trustdb 2> checktrustdb.txt
gpg2 --list-options show-uid-validity –list-keys > validity.txt
```

```sh
gpg2 --edit-key F99D95F1 trust quit       //Nivel 3
gpg2 --check-trustdb 2> checktrustdb2.txt
gpg2 --list-options show-uid-validity –list-keys > validity2.txt
```

```sh
gpg2 --sign-key --local-user 2DDAF451 3B8A48F6
gpg2 --sign-key --local-user 2DDAF451 AA706CE9
gpg2 --edit-key 3B8A48F6 trust quit      //Nivel 3
gpg2 --edit-key AA706CE9 trust quit      //Nivel 3
gpg2 --sign-key --local-user 3B8A48F6 2B42A0BF
gpg2 --sign-key --local-user AA706CE9 2B42A0BF
gpg2 --check-trustdb 2> checktrustdb3.txt
gpg2 --list-options show-uid-validity –list-keys > validity3.txt
```
