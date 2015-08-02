Wireless Sensor/Control Framework
=================================

* [English](#english)
* [Português](#português)

###English

Generic system for development of applications to capture, process and control data
through a interest centered network.

### Dependencies

To compile the examples it is necessary to install the following tools and libraries:

- g++ >= 4.8;
- cmake >= 2.8;
- libmysqlclient-dev >= 5.5.41;
- libmsgpack-dev >= 0.5.7

### Compiling the examples

After installing the dependencies, the compilation is done by a simple way. On command line, do:
```sh
$ mkdir build
$ cd build
$ cmake ..
$ make extern_lib_repa
$ make
$ make install
```
The binaries will be on "build" folder, following the respective example. Note that on
"raspberry" folder, the exec file will only be generated for an arm_v6 system, hence, raspberry.

### Running the examples

Before running any example, it is necessary to start the repd protocol for the client/server communication
be possible. On "repd" folder, there are many exec files for specific systems, execute the one that is
compatible with your system.
```sh
$ sudo ./repd/repad_x86_64 -- (Compatible with a x86-64 system);
```
Next, run the examples. For each example there is a help call.
```sh
$ ./build/clientsim -h -- (Show a list with possible commands for the clients)
```
To run the server, it is necessary that a mysql database is installed on your system.
```sh
$ mysql -u root -p123 -e "create database monitorAbelhas";
$ mysql -u root -p123 monitorAbelhas < config/dbname.sql
```
Note that the user and the pass must be defined when the database is created. If it is different from the
above, the data on "db.conf" file must be changed.

After everything with the database is done, the server can be run and there won't be any problem, saving
the data received from client.

### Using the WEB

The WEB part has been built using the [NodeJS](https://nodejs.org/) Framework, and it's required a version
no less than 0.10. After installing the tool, do:
```sh
$ cd src/web-UI
$ npm install
```
This will install all the required packages to make the web service be alive. To run, do:
```sh
$ node src/web-UI/app.js
```
Note that it is required the database user and pass. If the pass is different from '123' and the user
from 'root', change the value from MYSQL_USER and MYSQL_PASS on app.js file.

When the app is running, open a browser and type localhost:3000 to see.

###Português

Sistema genérico para desenvolvimento de aplicações de captura, processamento e
controle de dados através de uma rede centrada em interesse.

### Pré-Requisitos

Para a compilação dos exemplos será necessário a instalação das seguintes ferramentas
e bibliotecas:

- g++ >= 4.8;
- cmake >= 2.8;
- libmysqlclient-dev >= 5.5.41;
- libmsgpack-dev >= 0.5.7

### Compilando os exemplos
Após a instalação, a compilação é realizada de maneira simples. Na linha de comando, faça:
```sh
$ mkdir build
$ cd build
$ cmake ..
$ make extern_lib_repa
$ make
$ make install
```
Os binários ficarão na pasta "build", seguindo o respectivo modo de exemplo. Note que
na pasta "raspberry", o executável será gerado apenas em um sistema arm_v6, ou seja, em
um raspberry.

### Execução dos exemplos

Antes de executar algum exemplo, é necessário iniciar o protocolo repd para que a comunicação
cliente/servidor seja possível. Na pasta "repd", há vários executáveis para determinada arquitetura,
execute aquele compátivel com o seu sistema e deixe o processo em background.
```sh
$ sudo ./repd/repad_x86_64 -- (Compatível com um sistema x86 de 64 bits);
```
Em seguida, execute os exemplos. Para cada
exemplo há uma chamada de ajuda.
```sh
$ ./build/clientsim -h -- (Exibe uma lista com os possíveis comandos para o cliente)
```
Para a execução do servidor é necessário que um banco de dados mysql esteja inserido no sistema. Confirmada
a inserção do banco de dados será necessário a criação do banco de dados para o sistema.
```sh
$ mysql -u root -p123 -e "create database monitorAbelhas";
$ mysql -u root -p123 monitorAbelhas < config/dbname.sql
```
Vale notar que o usuário e a senha deve ser a mesma definida ao criar o banco de dados. Se for diferente
do citado, os dados do arquivo db.conf devem ser alterados.

Ao criar o banco de dados, o servidor poderá ser executado e não haverá problemas de armazenamento de dados
recebido do cliente.

### Utilizando o sistema WEB

A parte web foi feita utilizando o Framework [NodeJS](https://nodejs.org/), sendo necessário o uso
de uma versão >= 0.10. Após instalada a ferramenta, faça:
```sh
$ cd src/web-UI
$ npm install
```
Com isso instalará os pacotes necessários para que o servidor web fique ativo. Para a execução, faça:
```sh
$ node src/web-UI/app.js
```
Note que é necessário do usuário e a senha do banco de dados para que realize consultas. Se a senha
for diferente de '123' e do usuário 'root', altere os dados no arquivo app.js referente às variáveis
MYSQL_USER e MYSQL_PASS.

Feita a execução, inicie o browser e acesse localhost:3000 para visualização.
