Wireless Sensor/Control Framework
===

Sistema genérico para desenvolvimento de aplicações de captura, processamento e
controle de dados através de uma rede centrada em interesse.

### Compilação dos Exemplos

Para a compilação dos exemplos será necessário a instalação das seguintes ferramentas
e bibliotecas:

- cmake;
- libmysqlclient-dev;
- libmsgpack-dev;

Após a instalação, a compilação é realizada de maneira simples. Na linha de comando, faça:

- :$ mkdir build; cd build; cmake ..; make; make install

Os binários ficarão na pasta exemplo, seguindo o respectivo modo de exemplo. Note que
na pasta "raspberry", o executável será gerado apenas em um sistema arm_v6, ou seja, em
um raspberry.

### Execução dos exemplos

Antes de executar algum exemplo, é necessário iniciar o protocolo repd para que a comunicação
cliente/servidor seja possível. Na pasta "repd", há vários executáveis para determinada arquitetura,
execute aquele compátivel com o seu sistema.

- :$ ./repad_x86_64 -- (Compatível com um sistema x86 de 64 bits);

Depois de deixado em backgroud a execução do protocolo de comunicação, execute os exemplos. Para cada
exemplo há uma chamada de ajuda.

- :$ ./examples/simuladorFazenda/clientsim -h -- (Mostrará uma lista com os possíveis comandos para o cliente)

Para a execução do servidor é necessário que um banco de dados mysql esteja inserido no sistema. Confirmada
a inserção do banco de dados será necessário a criação do banco de dados para o sistema.

- :$ mysql -u root -p123 -e "create database monitorAbelhas";
- :$ mysql -u root -p123 monitorAbelhas < config/dbname.sql

Vale notar que o usuário e a senha deve ser a mesma definida ao criar o banco de dados. Se for diferente
do citado, os dados do arquivo db.conf devem ser alterados.

Ao criar o banco de dados, o servidor poderá ser executado e não haverá problemas de armazenamento de dados
recebido do cliente.
