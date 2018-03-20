# Practica 5

_MACs_

GW 08:00:27:66:bf:ba
AT 08:00:27:7e:9a:1a
VI 08:00:27:bb:76:6b
EX 08:00:27:ec:95:27

spoof
1. El atacante ha modificado la tabla arp de la victima para que todo el tráfico que esta genere pase por él. El ping no funciona porque el atacante actua de router pero no tiene activada la retransmisión.
  arptable_gw arptable_vic

2. Si porque ahora el atacante redirige el tráfico que intercepta, en el wireshark del atacante solo aparecen los echo request porque el ataque es sobre la tabla arp de la victima y por tanto solo esta manda tráfico al atacante.
  arptable_gw2 arptable_vic2 arptable_ex2

3. Ha cambiado la tabla del router, que ahora también reenvia los paquetes al atacante por lo cual en wireshark podemos ver los echo reply.
  at_gw3 at_vic3 at_ex3
 
 route cache
 En las tablas de routa vemos que victima envia a la red 2 mediante el atacante, en externo habla con victima via el atacante.
 En wireshark podemos ver tanto los request como los reply.
  route_vic route_ex
  
 flooding
 El atacante esta enviado request continuamente a la victima, para que esta no pueda continuar atendiendo peticiones (ocupando su ancho de banda).
 
 smurf attack
 Ahora esta realizando un broadcast en la red y todas las maquinas le están contestando por lo que esta boqueando la red 1 y no solo la máquina victima.
