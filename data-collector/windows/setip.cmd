@echo off
echo Escolha:
echo [A] Conectar na Rede Local
echo [B] Arrumar Internet
echo.
:choice
SET /P C=[A,B]?
for %%? in (A) do if /I "%C%"=="%%?" goto A
for %%? in (B) do if /I "%C%"=="%%?" goto B
goto choice
:A
@echo off
netsh interface ip set address "Conex∆o de Rede sem Fio" static 192.168.0.200 255.255.255.0
pause
goto end

:B
@ECHO OFF
ECHO Arrumando a Internet
netsh int ip set address name = "Conex∆o de Rede sem Fio" source = dhcp
ipconfig /renew
pause
goto end
:end