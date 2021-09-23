#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>


struct commands
{
  char* commandName;
  char* functionName;

  /*
  commands(char* inputName, char* inputFunction)
  {
    commandName = inputName;
    functionName = inputFunction;
  }
  */
};


void exitProcess()
{
  printf("exiting...\n");
  exit(0);
}

void cdProcess(char* path)
{
  char currentDir[PATH_MAX];
  printf(path);
  /*
  if (strlen(path)==1)
  {
    printf("%s\n", getcwd(currentDir,PATH_MAX));
    chdir("..");
    printf("%s\n", getcwd(currentDir,PATH_MAX));
  }
  else
  {
    //(path);
    chdir(path);
    printf(path);
    printf("%s\n", getcwd(currentDir,PATH_MAX));

  }
  */
}
void commandHandlers(char* command)
{

  char* commandList[3];
  commandList[0] = "exit";
  commandList[1] = "cd";
  commandList[2] = "path";

  int switchValue = -1;


  for(int counter = 0; counter < 3; counter++)
  {
    if(strcmp(commandList[counter], command) == 0)
    {
      switchValue = counter;
      break;
    }
  }

  switch(switchValue)
  {
    case 0:
      exitProcess();
      break;
    case 1:
      command = strtok(NULL, " ");
      cdProcess(command);
      break;
    default:
      printf("invalid\n");

  }

}
int main()
{

  //Reading input...
  char input[200];
  while(1)
  {
    printf("\n$ ");
    fgets(input, 200, stdin);


    //If there's a lack of input at startup, then just wait for input.
    if (strlen(input) == 0)
    {

    }
    //If there is input, then execute the command if valid.
    else
    {
      //const char delim[2] = " ";
      char* token;
      token = strtok(input, " \n\r\t");

      commandHandlers(token);

    }
  }
  //If there is input, then execute the command if valid.

};
