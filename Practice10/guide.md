# Practice 4.2

Importa una VM, clónala y dale el nombre _debian_. 

----

## Configurar debian
```sh
sudo apt-get update
sudo apt-get install wireshark apache2
sudo apt-get install php5 libapache2-mod-php5
```

Editar el fichero _/etc/apache2/apache2.conf_.
> sudo nano /etc/apache2/apache2.conf

Añadir la siguiente linea al final.
```sh
ServerName debian
```

Una vez editado ejecutar el siguiente código.
```sh
sudo service apache2 restart
sudo ip l s dev eth0 down
```

----

## Autoridad de certificación (CA)

Creamos los ficheros y directorios necesarios.

```sh
mkdir demoCA
mkdir demoCA/newcerts
mkdir demoCA/private
touch demoCA/index.txt
echo 01 > demoCA/serial
```

Ahora generamos un certificado autofirmado para la CA.

```sh
openssl req -x509 -new -days 3650 -keyout demoCA/private/cakey.pem -out demoCA/cacert.pem
```

Al crear el certificado nos pedira ciertos datos.
* Country > SP
* State Name > Madrid
* Locality Name > Madrid
* Organization Name > SER Corporation
* Common Name (CN) > CA

-----

## Certificado de servidor

Creamos una solicitud de certificado para el servidor

```sh
openssl req -new -keyout serverkey.pem -out servercsr.pem -nodes
```

Al crear el certificado nos pedira ciertos datos.
* Country > SP
* State Name > Madrid
* Locality Name > Madrid
* Organization Name > SER Corporation
* Common Name (CN) > debian

Editar el fichero _/etc/ssl/openssl.cnf_ para limitar el uso del certificado.
> sudo nano /etc/ssl/openssl.cnf

Añadimos al final la siguiente linea.

```sh
extendedKeyUsage = serverAuth
```

Firmar la solicitud con el cert. de la CA.

```sh
openssl ca -in servercsr.pem -out servercert.pem
```

-----

## Configuración de HTTPS

Activamos el modulo *mod_ssl* en _/etc/apache2/mods-available/ssl.conf_ y
_/etc/apache2/mods-available/ssl.load_.

```sh
sudo a2enmod ssl
```

Al ejecutar el comando se crean enlaces simbólicos a los ficheros anteriores en
el directorio _/etc/apache2/mods-enabled_.

Reiniciamos apache.

```sh
sudo service apache2 restart
```

Habilitamos el sitio con SSL definido en _/etc/apache2/sites-available/default-ssl_.

```sh
sudo a2ensite default-ssl
```

Al ejecutar el comando se crean enlaces simbólicos al fichero anterior en
el directorio _/etc/apache2/sites-enabled_.


Modifica el valor de los siguientes parámetros en el fichero
_/etc/apache2/sites-enabled/default-ssl_.

> sudo nano /etc/apache2/sites-enabled/default-ssl

```sh
SSLCertificateFile     /etc/ssl/certs/servercert.pem
SSLCertificateKeyFile  /etc/ssl/private/serverkey.pem
```

Copiamos el certificado y la clave del servidor a los directorios especificados.

```sh
sudo cp ./servercert.pem /etc/ssl/certs/servercert.pem
sudo cp ./serverkey.pem /etc/ssl/private/serverkey.pem
```

Recargamos la cfg apache.

```sh
sudo service apache2 reload
```

Ir al navegador e importar los certs de la CA.

1. Ir a _Settings_
2. _Show advanced settings_
3. En _HTTP/SSL_ pulsar en _Manage certificates_
4. Pestaña _Authorities_
5. Pulsar _Import_
6. Seleccionar el certificado de la CA
7. Pulsar en _Edit trust settings_
8. Seleccionar _Trust this certificate for identifying websites_

En el navegador ir a

> https://debian

Comprobar la conexión pulsando en el _candado_ y luego en _Connection_.

Iniciar wireshark por cualquier interfaz.

> sudo wireshark

Comprobar los mensajes del acuerdo TLS al ejecutar la siguiente orden.

```sh
openssl s_client -CAfile demoCA/cacert.pem -connect debian:443
```

Cuando la conexión esté establecida escribir

```sh
GET /
```

Volver a comprobar los mensajes al ejecutar.

```sh
openssl s_client -CAfile demoCA/cacert.pem -connect debian:443 -cipher AES256-SHA
```

Almacenar una sesión.

```sh
openssl s_client -CAfile demoCA/cacert.pem -connect debian:443 -cipher AES256-SHA -sess_out sess
```

Reutilizar la sesión.

```sh
openssl s_client -CAfile demoCA/cacert.pem -connect debian:443 -cipher AES256-SHA -sess_in sess
```

-----

## Certificado de cliente

Crear una solicitud de certificado de cliente.

```sh
openssl req -new -keyout clientkey.pem -out clientcsr.pem
```

Al crear el certificado nos pedira ciertos datos.
* Country > SP
* State Name > Madrid
* Locality Name > Madrid
* Organization Name > SER Corporation
* Common Name (CN) > usuario

Editar el fichero _/etc/ssl/openssl.cnf_.

> sudo nano /etc/ssl/openssl.cnf

Añadiendo la siguiente linea.

```sh
extendedKeyUsage = serverAuth, clientAuth
```

Firmar la solicitud con la CA.

```sh
openssl ca -in clientcsr.pem -out clientcert.pem
```

Convertir el cert. a formato PKCS12-

```sh
openssl pkcs12 -export -in clientcert.pem -inkey clientkey.pem -out client.p12
```

Ir al navegador e importar los certs del usuario.

1. Ir a _Settings_
2. _Show advanced settings_
3. En _HTTP/SSL_ pulsar en _Manage certificates_
4. Pestaña _Your certificates_
5. Pulsar _Import_
6. Seleccionar el certificado de usuario
7. Pulsar en _Edit trust settings_
8. Seleccionar _Trust this certificate for identifying websites_

Modifica el valor de los siguientes parámetros en el fichero
_/etc/apache2/sites-enabled/default-ssl_.

> sudo nano /etc/apache2/sites-enabled/default-ssl

```sh
SSLCACertificateFile /etc/ssl/certs/cacert.pem
SSLVerifyClient require
```

Copiamos el certificado de la CA al directorio especificado.

```sh
sudo cp ./demoCA/cacert.pem /etc/ssl/certs/cacert.pem
```

Recargamos la cfg apache.

```sh
sudo service apache2 reload
```

En el navegador ir a

> https://debian

Crear una página.

> sudo nano /var/www/index.php

Con el siguiente contenido.

```sh
<html>
<?php
if ($_SERVER['HTTPS'] == "on" ) {
      echo "Secure connection.<br><br>";
}
if ($_SERVER['SSL_CLIENT_VERIFY'] == "SUCCESS") {
    echo "Client verified successfully.<br><br>";
}
echo "Client Id: " . $_SERVER['SSL_CLIENT_S_DN'] . "<br>";
echo "Client CA: " . $_SERVER['SSL_CLIENT_I_DN'] . "<br>";
?>
</html>
```

En el navegador ir a

> https://debian/index.php

Comprobar los mensajes del acuerdo TLS al ejecutar la siguiente orden.

```sh
openssl s_client -CAfile demoCA/cacert.pem -cert clientcert.pem -key clientkey.pem -connect debian:443
```

Escribir.

```sh
GET /index.php
```
