#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>

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

void setPaths(char* args, char* pathsList[])
{

  int argCounter = 1;
  args = strtok(NULL, " \n\r\t");

  //Should probably also check if those arguments are actually accesible.
  while(args != NULL)
  {
    pathsList[argCounter] = args;
    args = strtok(NULL, " \n\r\t");
    argCounter++;
  }

  //Delete: test to see if it collected all paths.

  for(int x = 0; x < argCounter; x++)
  {
    printf("%s\n", pathsList[x]);
  }
}
void commandHandlers(char* command)
{
  //Contains the list of paths. Max number of paths = 200.
  //First path will always be /bin.
  char* pathsList[200] = {NULL};
  pathsList[0] = "/bin";

  //List of built-in commands.
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
      command = strtok(NULL, " \n\r\t");
      cdProcess(command);
      break;
    case 2:
      setPaths(command, pathsList);
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

    for(int x = 0; x<200; x++)
    {

      if (pathsList[x] != NULL) //(strcmp(paths[x], "") != 0)
      {

        //The following code concatenates the path directories with the program name.
        //printf("%s", pathsList[2]);
        char* path = malloc(strlen(pathsList[x])+strlen(execCommand)+2);
        strcpy(path, pathsList[x]);
        strcat(path, "/");
        strcat(path, execCommand);

        //System call that determines if a file exists.
        //If access returns 0, then it's a success!

        if(access(path, F_OK) == 0)
        {
          printf("hello");
          execv(path, execArgs);
        }


      }
      else
      {
        break;
      }
    }


    //printf("%s",execArgs[0]);
    /*
    pid_t pid;
    char* paths = "/bin/ls"
    char *execArgs[2];
    int counter = 1;
    execArgs[0] = paths;
    //Parse all the arguments in the token...

    while(command != NULL)
    {
      execArgs[counter] = command;
      command = strtok(NULL, " \n\r\t");
    }

    pid = fork();

    if (pid == 0)
    {
      //Child Process
      execv(execArgs[0], execArgs);
    }
    else
    {
      wait(NULL);
    }
    */
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
      char* token;
      token = strtok(input, " \n\r\t");
      commandHandlers(token);

    }
  }
  //If there is input, then execute the command if valid.

};
