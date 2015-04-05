#!/bin/bash

# coletar os dados por ordem crescente pelo prefixo
# 'select * from data order by Prefix;'

# deletar todos os dados fisicamente
# 'delete from data;'

# selecionar o ip. Mudar apenas o ultimo octeto, variando de 1-10. Ex:
# 192.168.0.1; 192.168.0.2; 192.168.0.5;
ip='192.168.0.5'

ssh pi@$ip "mysql -u root -p123 monitorAbelhas -e 'select * from data;'"\
  > dados_abelhas.txt
