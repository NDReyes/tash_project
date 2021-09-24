// Author: Nestor Reyes
// Date: 9/24/2021

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>


//Todo: Currently in the process of transitioning pathsList to a dynamic variable.
// Would need to convert commandHandler's implementation of pathList to a variable rather than an array.
void exitProcess()
{
  printf("exiting...\n");
  exit(0);
}

//To Do: Add a way to detect if a path does not exist and return an error.
void cdProcess(char* path)
{
  char currentDir[PATH_MAX];

  if (path == NULL)
  {
    chdir(getenv("HOME"));
    printf("%s\n", getcwd(currentDir,PATH_MAX));
  }
  else
  {
    chdir(path);
    printf("%s\n", getcwd(currentDir,PATH_MAX));
  }
}

char* setPaths(char* args, char* pathsList)
{

  args = strtok(NULL, " \n\r\t");
  while(args != NULL)
  {
    //Overwrite the pathsList
      char* newList = malloc(strlen(pathsList)+strlen(args)+2);
      strcpy(newList, pathsList);
      strcat(newList, " ");
      strcat(newList, args);
      pathsList = newList;
      args = strtok(NULL, " \n\r\t");
  }

  printf("%s", pathsList);

  return pathsList;

}
char* commandHandlers(char* input, char* pathsList)
{
  //Contains the list of paths. Max number of paths = 200.
  //First path will always be /bin.
  //char* pathsList[200] = {NULL};
  //pathsList[0] = "/bin";
  //List of built-in commands.
  char* commandList[3];
  commandList[0] = "exit";
  commandList[1] = "cd";
  commandList[2] = "path";

  int switchValue = -1;

  char* command = strtok(input, " \n\r\t");

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
      command = strtok(NULL, " \n\r\t");
      cdProcess(command);
      break;
    case 2:
      pathsList = setPaths(command, pathsList);
      break;
    default:
      break;

  }

  //If the command is not a built in command, then execute the command normally.
  if(switchValue == -1)
  {
    //First, collect the arguments of the command.
    //We will set the execv arguments to 100 for now, but they should be variable.
    char* execArgs[200] = {NULL};
    int counter = 0;
    while(command != NULL && counter < 100 && strcmp(command, "&") != 0)
    {
      execArgs[counter] = command;
      command = strtok(NULL, " \n\r\t");
      counter++;
    }

    char* execCommand = execArgs[0];

    pid_t pid;


    //will need an array for the arguments of the command.
    char* pathsToken = strtok(pathsList, " \n\r\t");
    while(pathsToken != NULL)
    {
      //The following code concatenates the path directories with the program name.
      char* path = malloc(strlen(pathsToken)+strlen(execCommand)+2);
      strcpy(path, pathsToken);
      strcat(path, "/");
      strcat(path, execCommand);

      //System call that determines if a file exists.
      //If access returns 0, then it's a success!

      if(access(path, F_OK) == 0)
      {
        execv(path, execArgs);
      }

      pathsToken = strtok(NULL, " \n\r\t");

    }
  }

  return pathsList;
}
int main()
{
  char* pathsList = "/bin";
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
      pathsList = commandHandlers(input, pathsList);
    }
  }
  //If there is input, then execute the command if valid.

};
