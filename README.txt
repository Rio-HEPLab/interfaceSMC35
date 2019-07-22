# interfaceSMC35
Programa que controla os motores de passo da validation

**Ja esta fucional, porem necessita de muito mais detalhes
como organizar melhor as informacoes printadas na pro usuario
Criar arquivo de log e da data(json)
E PERMITIR QUE O USUARIO CONSIGA DAR HALT, STOP e START mesmo
quando lendo de um arquivo**

Instalação
 
Instale na maquina as dependencias Lib Boost Program Options e NCurses
Entre no Diretorio do programa
Execute:
  make
  
Executando:
  ./interface.exe
  
Caso preciso de alguma alteração nas consfigurações do programa veja smc35.conf

Comandos
move_x='number' - Numero de passos em milimetros na direção x
move_y='number' - Numero de passos em milimetros na direção y
delay='number' - Tempo em segundos de delay
stop - Após o atual movimento o sistema irá pausar
start - Volta de um stop ou halt
h - halt, para instantaneamente o sistema
