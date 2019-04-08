/*
** Terminal interativo pra comunicação com sm35 da JVL
** by Rennê Lou, fevereiro de 2019
** ComLib.h foi escrita inicialmente no lhcb e adaptada para esse projeto
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <ComLib.h>

int main(int argc, char **argv)
{
    char *input = NULL;
    size_t inputSize = 0;

    char *awnser = (char*)malloc(1024*sizeof(char));

    int serial_fd = ComInit(argv[1], argv[2]);

    printf("\tComunicação interativa serial\nDigite 'exit' pra sair\n");

    while(1)
    {
      memset(awnser, '\0', 1024);
      inputSize = getline(&input, &inputSize, stdin);
      if(!strcmp(input,"exit\n"))break;

      sscanf(input,"%s\n",input);
      strcat(input,"\r\n");
      awnser = ComWrt(serial_fd,input);
      //awnser = ComRd(serial_fd);

      printf("Resposta: %s",awnser);
    }

    close(serial_fd);
    return 0;
}
