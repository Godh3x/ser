# Practice 3.5

Usaremos 4 MVs (gw1, gw2, host1, host2), conectadas a 3 redes internas(<span style='color:red'>inet</span>, <span style='color:magenta'>net1</span>, <span style='color:green'>net2</span>).
>host2<span style='color:green'>---</span>gw2<span style='color:red'>---</span>gw1<span style='color:magenta'>---</span>host1

En todas ellas: Dejar eth0 para internet.
- gw1: <span style='color:magenta'>eth1</span> & <span style='color:red'>eth2</span>
- gw2: <span style='color:green'>eth1</span> & <span style='color:red'>eth2</span>
- host1: <span style='color:magenta'>eth1</span>
- host2: <span style='color:green'>eth1</span>

----

## Configurar gw1
```sh
sudo ifconfig eth1 192.168.1.1/24 up
sudo ifconfig eth2 192.168.3.1/24 up
sudo route add -net 192.168.2.0/24 gw 192.168.3.2
sudo sysctl -w net.ipv4.ip_forward=1
sudo apt-get update
sudo apt-get install ssh strongswan wireshark
```

## Configurar gw2
```sh
sudo ifconfig eth1 192.168.2.1/24 up
sudo ifconfig eth2 192.168.3.2/24 up
sudo route add -net 192.168.1.0/24 gw 192.168.3.1
sudo sysctl -w net.ipv4.ip_forward=1
sudo apt-get update
sudo apt-get install ssh strongswan
```

## Configurar host1
```sh
sudo ifconfig eth1 192.168.1.2/24 up
sudo route add default gw 192.168.1.1
```

## Configurar host2
```sh
sudo ifconfig eth1 192.168.2.2/24 up
sudo route add default gw 192.168.2.1
```

----

## strongSwan

```sh
man ipsec
sudo nano /etc/ipsec.conf
```

----

## Conexión IPsec de sitio a sitio con clave secreta
En gw1 añadir al final del fichero _/etc/ipsec.secrets_ la siguiente linea.
> : PSK "Clave secreta muy segura"
```sh
sudo nano /etc/ipsec.secrets
```
Repetir en gw2.

Para configurar la VPN añadir lo siguiente al fichero _/etc/ipsec.conf_ en gw1 y gw2.
> sudo nano /etc/ipsec.conf

```sh
conn secret
      left=192.168.3.2
      leftsubnet=192.168.2.0/24
      leftauth=psk
      right=192.168.3.1
      rightsubnet=192.168.1.0/24
      rightauth=psk
      auto=add
```

Reiniciar el servicio en gw1 y gw2.
> sudo ipsec restart

En gw1 iniciar Wireshark para que escuche eth1 y eth2.

Iniciar la VPN en gw1.
> sudo ipsec up secret

Revisar el log.
> sudo cat /var/log/daemon.log

Ver los detalles de la conexión.
```sh
sudo ipsec status
sudo ipsec statusall
```

Ver las políticas y asociaciones de seguridad.
```sh
sudo ip xfrm policy
sudo ip xfrm state
```

En wireshar ir a: Edit > Preferences > Protocols > ESP.
 - Activar todas las casillas.
 - Editar ESP SAs y rellenar la información con los datos que da la siguiente orden.
> sudo ip xfrm state

Comprueba la VPN, desde host 1.
> sudo ping -c 1 192.168.2.2

Cerrar la conexión.
> sudo ipsec down secret

----

## Conexión IPsec de sitio a sitio con certificado autofirmado

Crea una clave RSA y un certificado autofirmado en gw1.

```sh
sudo sh -c "ipsec pki --gen > /etc/ipsec.d/private/gw1-key.der"
sudo sh -c "ipsec pki --self --in /etc/ipsec.d/private/gw1-key.der --dn "CN=gw1" > /etc/ipsec.d/certs/gw1-cert.der"
```

Repetir para gw2.
```sh
sudo sh -c "ipsec pki --gen > /etc/ipsec.d/private/gw2-key.der"
sudo sh -c "ipsec pki --self --in /etc/ipsec.d/private/gw2-key.der --dn "CN=gw2" > /etc/ipsec.d/certs/gw2-cert.der"
```

Copia el certificado de gw2 en gw1.
```sh
sudo scp usuario@192.168.3.2:/etc/ipsec.d/certs/gw2-cert.der /etc/ipsec.d/certs/
```

Copia el certificado de gw1 en gw2.
```sh
sudo scp usuario@192.168.3.1:/etc/ipsec.d/certs/gw1-cert.der /etc/ipsec.d/certs/
```

Añade en gw1 al final del fichero _/etc/ipsec.secrets_ la siguiente linea.
> : RSA gw1-key.der
```sh
sudo nano /etc/ipsec.secrets
```

Añade en gw2 al final del fichero _/etc/ipsec.secrets_ la siguiente linea.
> : RSA gw2-key.der
```sh
sudo nano /etc/ipsec.secrets
```

Para configurar la VPN añadir lo siguiente al fichero _/etc/ipsec.conf_ en gw1 y gw2.
> sudo nano /etc/ipsec.conf

```sh
conn sscert
      left=192.168.3.2
      leftsubnet=192.168.2.0/24
      leftcert=gw2-cert.der
      leftid="CN=gw2"
      right=192.168.3.1
      rightsubnet=192.168.1.0/24
      rightcert=gw1-cert.der
      rightid="CN=gw1"
      auto=add
```

Reiniciar el servicio en gw1 y gw2.
> sudo ipsec restart

En gw1 iniciar Wireshark para que escuche eth1 y eth2.

Iniciar la VPN en gw1.
> sudo ipsec up sscert

Revisar el log.
> sudo cat /var/log/daemon.log

Ver los detalles de la conexión.
```sh
sudo ipsec status
sudo ipsec statusall
```

Ver las políticas y asociaciones de seguridad.
```sh
sudo ip xfrm policy
sudo ip xfrm state
```