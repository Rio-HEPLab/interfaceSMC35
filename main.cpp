/*
** Interface de controle pra comunicação com sm35 da JVL
** by Rennê Lou, fevereiro de 2019
** ComLib.h foi escrita inicialmente no lhcb e adaptada para esse projeto
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include<fcntl.h>
#include <string.h>
#include <ComLib.h>
#include <StepMotor.h>
//#include <iostream>
#include <map>
#include <utility>
#include <string>
//#include <algorithm>
#include <fstream>
#include <boost/program_options.hpp>

#define BUFSIZE 256

using namespace std;
using namespace boost::program_options;

typedef struct _configStruct
{
    char *inputFile;
    char *dev;
    char *baudRate;
}configStruct;

typedef struct _data
{
    int function;
    char eixo;
    float number;
} Data;

int userInterace(int*, int*,string);
int RealTimeControl(int*, int*,variables_map);
Data CommandtoData(char *cmd);
int ReadConfig(variables_map,StepMotor*, StepMotor*);
int forbidenCommand(StepMotor *eixo, float a);


int main(int argc, char **argv)
{
    variables_map commadLine;
    variables_map configOptions;
    try
        {
        options_description generalOptions("General");
        generalOptions.add_options()
          ("help,h", "Help screen")
          ("device", value<string>(), "Comunication Port")
          ("baudrate", value<string>(), "Baud Rate")
          ("file", value<string>(), "Input File");

        options_description fileOptions("File");
        fileOptions.add_options()
          ("accel_x", value<int>(), "Acceleration")
          ("device", value<string>(), "Comunication Port")
          ("baudrate", value<string>(), "Baud Rate")
          ("file", value<string>(), "Input File")
          ("conv_x", value<float>(), "Conversion Factor")
          ("conv_y", value<float>(), "Conversion Factor")
          ("maxspeed_x", value<int>(), "Limit Velocity")
          ("maxspeed_y", value<int>(), "Limit Velocity")
          ("edgespeed_x", value<int>(), "Initial and Final Velocity")
          ("edgespeed_y",value<int>(), "Initial and Final Velocity")
          ("accel_y", value<int>(), "Acceleration")
          ("maxposition_x", value<int>(), "Limit Position")
          ("maxposition_y", value<int>(), "Limit Position")
          ("minposition_x", value<int>(), "Limit Position")
          ("minposition_y", value<int>(), "Limit Position");

        store(parse_command_line(argc, argv, generalOptions), commadLine);

        std::ifstream ifs ("smc35.conf", std::ifstream::in);
        if (ifs)
          store(parse_config_file(ifs, fileOptions), configOptions);

        notify(configOptions);
    }
    catch (const error &ex)
    {
        //std::cout << ex.what() << '\n';
    }

    pid_t parent = -1;
    int parentPipe[2] = {0,0};
    int childPipe[2] = {0,0};

    // error checking for pipe
    if (pipe(parentPipe) < 0)
        exit(1);

    if (pipe(childPipe) < 0)
        exit(1);

    // error checking for fcntl
    if (fcntl(parentPipe[0], F_SETFL, O_NONBLOCK) < 0)
        exit(2);

    parent = fork();
    // continued
    switch (parent)
    {
    // error
    case -1:
        exit(3);
    // 0 for child process
    case 0:
        if (commadLine.count("file"))
            userInterace(parentPipe,childPipe,commadLine["file"].as<string>());
        else if(configOptions.count("file"))
            userInterace(parentPipe,childPipe,configOptions["file"].as<string>());
        else
        {
            string empty;
            userInterace(parentPipe,childPipe,empty);
        }
        break;
    default:
        RealTimeControl(parentPipe,childPipe,configOptions);
        break;
    }
    return 0;
}

int userInterace(int *parentPipe, int *childPipe, string _file)
{
    close(childPipe[1]);

    static char msg[][64] = {
        "Motor Aguardando comando\n",
        "Motor Executando comando\n",
        "Erro de comunicação com smc35\n"
    };
    static int feedback = 1;
    char *input = NULL;
    size_t inputSize = 0;
    Data tosend;
    FILE *inputFile = NULL;

    if(_file.size() > 0)
      inputFile = fopen(_file.c_str(), "r");

    if(inputFile != NULL) stdin = inputFile;
    //else inputFile = stdin;

    while(1)
    {
      if(getline(&input, &inputSize, stdin) == -1)
      {
          tosend =
          {
              .function = -2, // RealTimeControl interpreta como exit
              .eixo = 'c',
              .number =  0,
          };
          write(parentPipe[1], &tosend, sizeof(tosend));
          if(!feedback) break;
          else continue;
      }
      if(!strcmp(input,"exit\n"))
      {
          tosend =
          {
              .function = -2,   //RealTimeControl interpreta como exit
              .eixo = '\0',
              .number =  0,
          };
          write(parentPipe[1], &tosend, sizeof(tosend));
          if(!feedback) break;
          else continue;
      }

      tosend = CommandtoData(input);

      if(tosend.function == -1)
      {
          printf("Comando não encontrado\n");
          continue;
      }

      write(parentPipe[1], &tosend, sizeof(tosend) );

      read(childPipe[0], &feedback, sizeof(int));
      printf(msg[feedback]);
      //printf("read\n");
    }

    close(parentPipe[1]);
    close(childPipe[0]);
    fflush(stdout);

    return 0;
}

int RealTimeControl(int *parentPipe, int *childPipe, variables_map configOptions)
{
    close(parentPipe[1]);

    int nread;
    Data datas;
    int running;
    int err = 0;

    StepMotor EixoX = StepMotor(x,200,100,100,1.);
    StepMotor EixoY = StepMotor(y,100,50,50,1.);

    ReadConfig(configOptions,&EixoX,&EixoY);

    int (**functionTable)(StepMotor*, float);

    functionTable = (int(**)(StepMotor*, float)) malloc( 16 * sizeof(int(*)(StepMotor*, float)));
    functionTable[0] = StepMotor::paramStop;
    functionTable[1] = StepMotor::paramDelay;
    functionTable[2] = StepMotor::paramMove;
    functionTable[3] = StepMotor::paramVelocity;
    functionTable[4] = StepMotor::paramStartRate;
    functionTable[5] = StepMotor::paramAcceleration;
    functionTable[6] = forbidenCommand;
    functionTable[7] = StepMotor::paramStart;
    functionTable[8] = StepMotor::paramPause;

    while(1)
    {
        running = StepMotor::NextMove();

        nread = read(parentPipe[0], &datas, sizeof(datas));
        if ( nread <= 0 ) continue;

        write(childPipe[1],&running,sizeof(int));

        if(datas.function == -2)
        {
            if(!running) break;
            else continue;
        }

        if(datas.eixo == 'x') err = functionTable[datas.function](&EixoX,datas.number);
        else if( datas.eixo == 'y' ) err = functionTable[datas.function](&EixoY,datas.number);
        else if(datas.eixo == '\0') err = functionTable[datas.function](NULL,datas.number);

        if(err == -1) printf("Command not found\n");
        else if(err == -2) printf("Value break the limit\n");
    }

    close(parentPipe[0]);
    close(childPipe[1]);
    fflush(stdout);

    return 0;
}

Data CommandtoData(char *cmd)
{
    static map<string , int> functionMap;
    char *cmd1;// =  (char*) malloc (24*sizeof(char));
    char *cmd2;// =  (char*) malloc (24*sizeof(char));

    char c;
    float a;

    Data ret =
    {
        .function = -1,
        .eixo = c,
        .number =  a,
    };

    if(functionMap.size() == 0)
    {
        functionMap["h"] = 0;
        functionMap["delay"] = 1;
        functionMap["move"] = 2;
        functionMap["maxspeed"] = 3;
        functionMap["edgespeed"] = 4;
        functionMap["accel"] = 5;
        functionMap["conv"] = 6;
        functionMap["start"] = 7;
        functionMap["pause"] = 8;
    }

    sscanf(cmd,"%s\n",cmd);

    cmd1 = strtok(cmd," _=");
    cmd1 = strtok(NULL," _=");
    cmd2 = strtok(NULL," _=");

    if(functionMap.find(cmd) == functionMap.end()) return ret;

    if(cmd1 != NULL && cmd2 != NULL)
    {
      if(sscanf(cmd1,"%c",&c)<=0) return ret;
      if(sscanf(cmd2,"%f",&a)<=0) return ret;
    }
    else if(cmd1 != NULL && cmd2 == NULL)
    {
      c = '\0';
      if(sscanf(cmd1,"%f",&a)<=0) return ret;
    }
    else
    {
      c = '\0';
      a = 0;
    }

    ret =
    {
        .function = functionMap[cmd],
        .eixo = (char)c,
        .number =  (float)a,
    };

    return ret;
}

int ReadConfig(variables_map configOptions, StepMotor *eixoX, StepMotor *eixoY)
{
    char *baudRate = (char*)malloc(32*sizeof(char));
    char *dev =  (char*)malloc(32*sizeof(char));
    int serial_fd;

    if (configOptions.count("device"))
      strcpy(dev,configOptions["device"].as<string>().c_str());
    if (configOptions.count("baudrate"))
      strcpy(baudRate,configOptions["baudrate"].as<string>().c_str());
    if (configOptions.count("conv_x"))
      eixoX->SetConversionFactor(configOptions["conv_x"].as<float>());
    if (configOptions.count("conv_y"))
      eixoY->SetConversionFactor(configOptions["conv_y"].as<float>());
    if (configOptions.count("maxspeed_x"))
      eixoX->SetVelocity(configOptions["maxspeed_x"].as<int>());
    if (configOptions.count("maxspeed_y"))
      eixoY->SetVelocity(configOptions["maxspeed_y"].as<int>());
    if (configOptions.count("edgespeed_x"))
      eixoX->SetStartRate(configOptions["edgespeed_x"].as<int>());
    if (configOptions.count("edgespeed_y"))
      eixoY->SetStartRate(configOptions["edgespeed_y"].as<int>());
    if (configOptions.count("accel_x"))
      eixoX->SetAcceleration(configOptions["accel_x"].as<int>());
    if (configOptions.count("accel_y"))
      eixoY->SetAcceleration(configOptions["accel_y"].as<int>());
    if (configOptions.count("maxposition_x"))
      eixoX->SetMaxPosition(configOptions["maxposition_x"].as<int>());
    if (configOptions.count("maxposition_y"))
      eixoY->SetMaxPosition(configOptions["maxposition_y"].as<int>());
    if (configOptions.count("minposition_x"))
      eixoX->SetMinPosition(configOptions["minposition_x"].as<int>());
    if (configOptions.count("minposition_y"))
      eixoY->SetMinPosition(configOptions["minposition_y"].as<int>());

    serial_fd = ComInit(baudRate,dev);

    StepMotor::Init(serial_fd);

    return 0;
}

int forbidenCommand(StepMotor* eixo, float a)
{
    printf("Comando não acessivel desta forma. Favor leia a documentação\n");
    return 0;
}
