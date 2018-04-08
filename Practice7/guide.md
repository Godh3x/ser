# Practice 3.4

Usaremos 3 MVs (ids, victima y atacante), conectadas a la misma red interna.\
En todas ellas: Dejar eth0 para internet y crear eth1 para red interna.\
En el interfaz eth1 de ids: Activar "Permitir MVs" situado en el parámetro
  "Modo promiscuo".

----

## Configurar ids
```sh
sudo ifconfig eth1 192.168.1.1/24 up
sudo apt-get update
sudo apt-get install snort
sudo dpkg-reconfigure snort
sudo ip l s dev eth0 down
```
Al ejecutar el último comando indicar que:
 - Se inicie en el arranque.
 - Escuche por eth1.
 - La red local sea 192.168.1.0/24.
 - No se deshabilite el modo promiscuo.
 - Ninguna opción adicional.
 - No se envien resumenes por correo electrónico.

## Configurar victima
```sh
sudo ifconfig eth1 192.168.1.2/24 up
sudo ip l s dev eth0 down
```

## Configurar atacante
```sh
sudo ifconfig eth1 192.168.1.3/24 up
sudo apt-get update
sudo apt-get install nmap dsniff
sudo ip l s dev eth0 down
```

----

## Sniffer y packet logger

Inicia snort en modo sniffer, en **ids**.
```sh
sudo snort -v -i eth1
```

Para comprobar que funciona haremos un ping de **atacante** a victima.
```sh
ping 192.168.1.2
```

----

## NIDS basado en reglas

Comprobar que snort se inicia en modo NIDS, en **ids**.
```sh
sudo service snort status
```
_Salida esperada_
> Status of snort daemon(s): eth1 OK.

Si no se ha iniciado se haria con:
```sh
sudo service snort start
```

Hacer ping a victima desde **atacante**.
```sh
ping 192.168.1.2
```

Al realizar el ping comprobar en **ids** el fichero de alertas de snort, hacer
foto y llamarla **nbra.png**.
```sh
sudo cat /var/log/snort/alert
```

Las alertas llevan el identificador con tres numeros separados entre corchetes.
[XX:YY:ZZ]
- XX. Generator ID (GID), indica que componente de snort genero la alerta.
- YY. Snort ID (SID), indica directamente que regla ha dado la alerta.
- ZZ. Número de revisión, cada revisión de la regla debe incrementarlo.

Buscar las reglas en algun fichero del directorio _/etc/snort/rules/_.

Forma fácil de buscar:
> sudo grep -r "sid:366" /etc/snort/rules

Utilizar dicho fichero para localizar las reglas que han generado las alertas
del fichero de snort y copiarlas en la solución.

----

## Definición de nuevas reglas

Editar _/etc/snort/rules/local.rules_ y añadir la siguiente regla.
> alert icmp any any -> any any  (msg:"Special ping"; ttl:100; dsize:200; sid:10000001; rev:1;)

Reiniciar snort.
```sh
sudo service snort restart
```

Intentar activar la alerta.
```sh
ping -t 100 -s 200 -c 1 192.168.1.2
```

Hacer una foto de la alerta generada, llamarla **nra.png**.
```sh
sudo cat /var/log/snort/alert
```

----

## Preprocesadores

Añadir la siguiente linea a _/etc/snort/snort.conf_ para generar automaticamente
alertas asociadas a los eventos de los preprocesadores.
> config autogenerate_preprocessor_decoder_rules

Descomentar la linea de ejemplo, del preprocesador sfportscan (GID = 122) y
recargar la configuración.

Ejecutar en **atacante**.
```sh
sudo nmap 192.168.1.2
```

Realizar foto de las alertas, en **ids** y llamarla **sfpa.png**.
```sh
sudo cat /var/log/snort/alert
```

Descomentar la linea de ejemplo, del preprocesador arpspoof (GID = 112),
modificandolas para asociar IPs con MACs, y recargar la configuración.
```sh
preprocessor arpspoof
preprocessor arpspoof_detect_host: 192.168.1.1 08:00:27:66:bf:ba
preprocessor arpspoof_detect_host: 192.168.1.2 08:00:27:1f:21:c9
preprocessor arpspoof_detect_host: 192.168.1.3 08:00:27:62:95:fc
```

Ejecutar en **atacante**.
```sh
sudo arpspoof -i eth1 -r -t 192.168.1.1 192.168.1.2
```

Realizar foto de las alertas, en **ids** y llamarla **arpa.png**.
```sh
sudo cat /var/log/snort/alert
```