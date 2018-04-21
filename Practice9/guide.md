# Practice 3.6

Usaremos 2 MVs (left, right), conectadas a una red interna (internet).

En ambas:
 - Dejar eth0 para internet.
 - Añadir eth1, llamada internet.


----

## Configurar left
```sh
sudo ifconfig eth1 192.168.1.1/24 up
sudo apt-get update
sudo apt-get install openvpn ssh
```

Editar el fichero _/etc/apt/sources.list_.
> sudo nano /etc/apt/sources.list

Añadir la siguiente linea al final.
```sh
deb http://ftp.es.debian.org/debian wheezy-backports main
```

Una vez editado ejecutar el siguiente código.
```sh
sudo apt-get update
sudo apt-get -t wheezy-backports install wireshark
```

## Configurar right
```sh
sudo ifconfig eth1 192.168.1.2/24 up
sudo apt-get update
sudo apt-get install openvpn ssh
```

----

## (OpenVPN) Uso de una clave estática previamente compartida

En left ejecutar.

```sh
/usr/sbin/openvpn --genkey --secret static.key
scp static.key usuario@192.168.1.2:.
sudo openvpn --remote 192.168.1.2 --dev tun --ifconfig 10.4.0.1 10.4.0.2 --secret static.key
```

En right.

```sh
sudo openvpn --remote 192.168.1.1 --dev tun --ifconfig 10.4.0.2 10.4.0.1 --secret static.key --verb 4
```

Comprobar que se ha creado en right la interfaz tun0.
```sh
sudo ifconfig -a
```

Comprobar que la VPN funciona.
 1. Abrir wireshark en left por la interfaz eth1.
 2. Hacer un ping.
 3. Mirar si los mensajes están cifrados.
 4. Capturar por tun0.
 5. Repetir el ping.
 6. Comprobar que mensajes se capturan.

> ping -c 4 10.4.0.2

Cerrar la VPN en left y right.
> Ctrl + C

En left.

> sudo nano left.conf

```sh
remote 192.168.1.2
dev tun
ifconfig 10.4.0.1 10.4.0.2
secret static.key
```

Una vez creado ejecutar la VPN.
> sudo openvpn left.conf

En right.

> sudo nano right.conf

```sh
remote 192.168.1.1
dev tun
ifconfig 10.4.0.2 10.4.0.1
secret static.key
```

Una vez creado ejecutar la VPN.
> sudo openvpn right.conf


----

## (OpenVPN) Uso de TLS con certificados

Copiar los certificados en left.

```sh
cp /usr/share/doc/openvpn/examples/sample-keys/{client.*,ca.crt} .
gunzip client.crt.gz
```
Copiar los certificados en right.

```sh
cp /usr/share/doc/openvpn/examples/sample-keys/{server.*,ca.crt,dh1024.pem} .
gunzip server.crt.gz
```

Iniciar la VPN en left.
```sh
sudo openvpn --remote 192.168.1.2 --dev tun --ifconfig 10.4.0.1 10.4.0.2 --tls-client --ca ca.crt --cert client.crt --key client.key
```

Iniciar la VPN en right.
```sh
sudo openvpn --remote 192.168.1.1 --dev tun --ifconfig 10.4.0.2 10.4.0.1 --tls-server --dh dh1024.pem --ca ca.crt --cert server.crt --key server.key --verb 4
```

Crear left2.conf.
> sudo nano left2.conf
```sh
client

dev tun
proto tcp
remote 192.168.1.2 1194

ca ca.crt
cert client.crt
key client.key

remote-cert-tls server
tls-remote Test-Server
```

Crear right2.conf.
> sudo nano right2.conf
```sh
local 192.168.1.2
port 1194
proto tcp

dev tun

ca ca.crt
cert server.crt
key server.key

dh dh2048.pem

server 10.8.0.0 255.255.255.0

ifconfig-pool-persist ipp.txt
```

Crear el fichero dh en right.
> openssl dhparam -out dh2048.pem 2048

Cambiar wireshark para que capture por la interfaz 1.

Cerrar la VPN en left y right.
> Ctrl + C

VPN en left.
> sudo openvpn left2.conf

VPN en right.
> sudo openvpn right2.conf

Probar la VPN haciendo un ping desde left y comprobando que los mensajes capturados estan cifrados y son de tipo P_DATA_V1.
> ping 10.8.0.1

-----

## (OpenSSH) Autenticación con clave pública

En left ejecuta.
> ssh-keygen

Selecciona la ubicación por defecto (_/home/usuario/.ssh/id\_rsa_) para la clave privada.
Contraseña: escarlata

Copia la clave pública a right ejecutando en left.
> ssh-copy-id 192.168.1.2

Accede con ssh a right desde left, pedira la contraseña (la que se ha usado en la creación).
> ssh -v 192.168.1.2

Ejecutar en left los siguientes comandos para prevenir que se vuelva a solicitar la contraseña.

```sh
eval `ssh-agent`
ssh-add
```

-----

## (OpenSSH) Reenvío de puertos

Crear un tunel en left.
 - Puerto 8080 de left.
 - Puerto 80 de right.
 - Conexión a www.ucm.es.

> ssh -v -N -L 8080:www.ucm.es:80 usuario@192.168.1.2

Abrir el navegador en left e ir a
> http://localhost:8080

Crear el tunel iniciando el navegador remotamente.

> ssh -v -X -R 8080:www.ucm.es:80 usuario@192.168.1.2 chromium

En el navegador que se ha abierto ir a
> http://localhost:8080

Crear un nuevo tunel en left.

> ssh -v -N -D 1080 usuario@192.168.1.2

Configurar el navegador para usar un proxy socks.

> chromium --proxy-server="socks5://localhost:1080"

En el navegador acceder a
> http://www.ucm.es