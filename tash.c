// Author: Nestor Reyes
// Date: 9/24/2021

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

char error_message[30] = "An error has occurred\n";
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
    int returnValue = chdir(path);
    if(returnValue == 0)
      printf("%s\n", getcwd(currentDir,PATH_MAX));
    else
      printf("%s", error_message);
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

char* setPathsBatch(char* args[], char* pathsList)
{
  int counter = 1;
  while(args[counter] != NULL)
  {
    //Overwrite the pathsList
      char* newList = malloc(strlen(pathsList)+strlen(args[counter])+2);
      strcpy(newList, pathsList);
      strcat(newList, " ");
      strcat(newList, args[counter]);
      pathsList = newList;
      counter++;
  }
  printf("%s", pathsList);
  return pathsList;
}

void cleanExecArgs(char* execArgs[], int size)
{
  for( int x = 0; x < size-1; x++ )
  {
    execArgs[x]=execArgs[x+1];
  }
  execArgs[size-1] = NULL;
}
char* commandHandlers(char* input, char* pathsList, char* execArgs[], int mode, int execSize)
{

  if (execArgs != NULL)
  {
    cleanExecArgs(execArgs, execSize);
  }
  //List of built-in commands.
  char* commandList[3];
  commandList[0] = "exit";
  commandList[1] = "cd";
  commandList[2] = "path";

  int switchValue = -1;

  char* command;

  //If interactive mode, command will need to be tokenized form of the input.
  //Otherwise, command can just be the input, since it's already tokenized.
  if(mode == 1)
    command = strtok(input, " \n\r\t");
  else
    command = input;

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
      if(mode == 1)
      {
        //In this case, command is actually the filepath that cd needs to print!
        command = strtok(NULL, " \n\r\t");
        cdProcess(command);
      }
      else
        cdProcess(execArgs[1]);
      break;
    case 2:
      if(mode == 1)
        pathsList = setPaths(command, pathsList);
      else
        pathsList = setPathsBatch(execArgs, pathsList);
      break;
    default:
      break;

  }

  //If the command is not a built in command, then execute the command normally.
  if(switchValue == -1)
  {
    //First, collect the arguments of the command.
    //We will set the execv arguments to 100 for now, but they should be dynamic.
    //char* execArgs[200] = {NULL};
    int counter = 0;

    //execCommand is the first parse of the input line, aka the actual command to be executed.
    char* execCommand = command;

    //This parses the input. Only needed if it's in interactive mode.
    //If it's in batch mode, it should have the arguments already.
    if(mode == 1)
    {
      while(command != NULL && counter < 200 && strcmp(command, "&") != 0)
      {
        if(strcmp(command,">") == 0)
        {
          if(fork() == 0)
          {
            command = strtok(NULL, " \n\t\r");
            char* filename = command;
            int fp = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

            dup2(fp, 1);
            dup2(fp, 2);
            close(fp);
            execv("/bin/ls", execArgs);
          }
          else
          {
            wait(NULL);
            goto returnToMain;
          }
        }
        else
        {
          execArgs[counter] = command;
          command = strtok(NULL, " \n\r\t");
        }
        counter++;
      }
    }


    //char* execCommand = execArgs[0];

    pid_t pid;

    pid = fork();

    //only the child process needs to execute the command, if it exists.
    if(pid == 0)
    {

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
    //Parent process needs to wait until the child process finishes.
    else
      wait(NULL);
  }

  returnToMain:
  return pathsList;
}
int main(int argCount, char** argArray)
{

  char* pathsList = "/bin";
  //Reading input...
  char input[500];

  if(argCount > 1)
  {
    //printf("%s", argArray[0]);
    pathsList = commandHandlers(argArray[1], pathsList, argArray, 0, argCount);
  }
  while(1)
  {
    printf("\n$ ");
    char* execArgs[200] = {NULL};
    fgets(input, 500, stdin);

    //Eliminates any trailing characters.
    //Copy must be created in order to not create an unnecessary token of the original input.
    char copy[strlen(input)]; //=a
    strcpy(copy,input);
    char* cleanInput = strtok(copy, " \n\r\t");

      //If there's a lack of input at startup, then just wait for input.
    if (cleanInput != NULL)
    {
      pathsList = commandHandlers(input, pathsList, execArgs, 1, 0);
    }
  }
  //If there is input, then execute the command if valid.

};
