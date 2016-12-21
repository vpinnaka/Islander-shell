//*********************************************************
//
//                    Includes and Defines
//
//*********************************************************
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <sstream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <stdlib.h> 
#include <list> 
#include <ctime>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <stdio.h>
#include <cstddef>
#include <libgen.h>
#include <sys/time.h>


#define EXIT_FLAG -1
#define SINGLE_ARG 1
#define NO_ARG 0




using namespace std;




//*********************************************************
//
//                    Extern Declarations
// 
//
//*********************************************************
using namespace std;
extern "C"
{
  extern char **gettoks();
} 

list<char**> history_list;                                  // linked list defined as extern to implement 'hist' command

char** toks;                                                // 'toks' is a double pointer of  char type defined extern tokanized commands are stored in this pointer 



//*********************************************************
//
//                   Function Prototypes
// 
//
//*********************************************************

int shellReadCommand();
void shellPrompt(int);
void shellStartHere(int);
void signalint(int);
void insertCmdHistory(int);
void displayCmdHistory(int);
int retriveCmd(int,int);
int ioRedirection(int);



//*********************************************************
//
//                    Main Function
//
//*********************************************************
int main( int argc, char *argv[] )
{
  // local variables declaration
  int ii,stat,ioret,pid;
  int retval,arguments;
  static int cmdCount;
  int current_in,current_out;

  // initialize local variables
  ii = 0;
  toks = NULL;
  retval = 0;
  arguments = 0;
  cmdCount = 1;

  //********************************************************
  //
  //                        Signal captueing
  //
  //********************************************************
  signal(SIGINT,signalint);                                  // captures ctrl+C and redirects to "signalint" function
  signal(SIGQUIT,signalint);                                 // captures ctrl+\ and redirects to "signalint" function
  signal(SIGTSTP,signalint);                                 // captures ctrl+Z and redirects to "signalint" function
 

  // main (infinite) loop
  while( true )
    {
 
      shellPrompt(cmdCount++);                              // function calling; to print the shell prompt

      arguments = shellReadCommand();                       // function calling with return value; to read the command from user

      
      if(arguments == EXIT_FLAG)                            // Checking for exit flag returned by the 'shellReadCommand' function 
          break;                                            // if the exit flag match occurs beak the infinite loop and exit the program
      else if(arguments == NO_ARG)
        {
          cmdCount--;                                       // else continue to the next command
          continue;
        }
          

      if((arguments == SINGLE_ARG) && !(strcmp(toks[0],"history"))) // original functionality of 'history' shell command is disabled here
          continue;


      
      if(!(strcmp(*toks,"r")))                            // Check for the 'r' command to call the function 'retriveCmd'
        {
          arguments = retriveCmd(cmdCount-1,arguments);   // function calling with return value; retrived command is assigned to 'toks' and argument count is returned
        
          if(*toks == NULL)
              cout << "Illegal command retreval" <<endl;
        }
          
     
      if((arguments == SINGLE_ARG) && !(strcmp(*toks,"hist")))// Check for shell inbuilt 'hist' command
          {
            insertCmdHistory(arguments);                  // function call; Insert the command in the history
            displayCmdHistory(cmdCount);                  // function call; Display the commands in the history
            continue;
          }
//********************************************************
//
//                 Fork the process
//
//********************************************************

      pid = fork();                                       // FORK system call;  return value is captured

      if(pid == EXIT_FLAG)                                // Validating the fork system call return value
          cout << "fork returned with error " << pid << endl;
      else if (pid != 0)                                  // if fork returned value is non zero  I am In the parent wait for child to exit
          waitpid(EXIT_FLAG,&stat,0);
      else                                                // if fork returned value is zero I am in child continue the execution
          shellStartHere(arguments);

      insertCmdHistory(arguments);                        // function call; insert the command entered by the user in history
    }                                                     // end of infinite while loop

  signalint(EXIT_FLAG);                                   // function call; to display the count of signals captured
  // return to calling environment
  return( retval ); 
}

//********************************************************
//
//  Insert command in History Function
//
//  This function stores the each command entered by the user 
//  in a linked list. Functionally this function creates a new 
//  address space and copy the tokens then it stores the new address
//  space in the linked list  
//
//  Return Value
//  ------------  
//  void
//
//  Value Parameters
//  ----------------
//  argCount       int          The argumets count of the command that to be stored
//
//  Extern Parameters
//  -----------------
//  toks           char**       The tokenized command that to be stored
//  history.list   char**       The linked list variable used to push the command into the list
//  Local Variables  
//  ---------------
//  dummy_count    int          Loop iteration variable
//  new_toks       char**       new pointer address that to be stored in the list
//********************************************************

void insertCmdHistory(int argCount)
{
  int dummy_count = argCount;
  
  char** new_toks =(char**)calloc(argCount,argCount*sizeof(toks));

  
  for(int i=0;argCount > 0;--argCount,++new_toks,++toks)
  {
        memcpy(new_toks,toks,sizeof(toks));
  }
    
  for(; dummy_count > 0;--dummy_count,--new_toks);
  
  
  history_list.push_front(new_toks);
  

}


//********************************************************
//
//  Display command  History Function
//
//  This function displays past 10 commands  that is stored in the linked list .
//  This function has a linked list iterator which travarses through the list 
//  and assign the address stored in the list to a double pointer. A loop runs on
//  this double pointer to display each token stored. I user gives 'hist' command
//  this function is called
//
//  Return Value
//  ------------  
//  void
//
//  Value Parameters
//  ----------------
//  Count       int             The count of the command is displayed opposite to the command
//
//  Referance Parameters
//  --------------------
//  position    char**          The linked list iterator to travarse the list 
//
//  Local Variables  
//  ---------------
//  i,ii           int          Loop iteration variable
//  display_toks   char**       pointer address that is stored is retrived into this pointer
//********************************************************

void displayCmdHistory(int Count)
{
    int i;
    char** display_toks;
    list<char**>::iterator position;
    
    for(position = history_list.begin(),i=1;(position != history_list.end())&&(i <= 10);position++, ++i)
    {
        display_toks = *position;
        cout << "Hist CMD  [" << --Count <<"]  : ";
        for(int ii=0; display_toks[ii] != NULL; ++ii )
            cout <<display_toks[ii] << " ";
        cout << endl;

    }
}


//********************************************************
//
//  Retrive command Function
//
//  This function retrives the command entered by the user from the linked list.
//  offset from the current command number is obtained from n value in the command r n
//  Linked list iterator is travarsed to the offset and the address in the list is copied to
//  the pointer ret_toks which is later assigned to the extern pointer toks
//
//  Return Value
//  ------------  
//  int                         return the arguments count of the command that is retrived
//
//  Value Parameters
//  ----------------
//  argCount       int          The arguments count of the 'r' command it can be one or two r or r n
//  Count          int          To caluculate the offset for traversing into the linked list
//
//  Extern Parameters
//  -----------------
//  toks           char**       The retrived command i.e the address of the stored command is assigned back to 'toks'
//
//  Referance Parameters
//  --------------------
//  ret_posi      char**        The linked list iterator to travarse the list to offset and assign the address retrived to pointer 
//  history_list  char**        The linked list variable to assign the iterator with begin and end values of the list
//
//  Local Variables  
//  ---------------
//  int            int          Loop iteration variable
//  offset         int          Command number entered by the user is stored here
//  ret_toks       char**       new pointer address that to be stored in the list
//********************************************************


int retriveCmd(int Count,int argCount)
{
    char** ret_toks;
    
    int offset = 0,ii=0;
    list<char**>::iterator ret_posi;
    ret_posi = history_list.begin();

    if(*++toks != NULL)
    {
        offset =  (**toks) - '0';

        if(*(++(*toks)) != 0)
            offset = 10*offset  + ((**toks) - '0');
    }
    else
      offset=0;
   
    --toks;
    
    
    if(offset == 0)
        ret_posi = history_list.begin();
    else
        for(offset = Count - offset ;offset >1;offset--)
            ret_posi++;
    
        
    ret_toks = *ret_posi;
         
    toks = ret_toks;
    
    if( toks[0] != NULL )
      for( ii=0; toks[ii] != NULL; ii++ );
    else
      return ii;
    return ii;
}


//********************************************************
//
//  Signal handling Function
//
//  This function counts how many times user have presed ctrl+C,ctrl+\,ctrl+Z 
//  and display the count once the program exits  
//
//  Return Value
//  ------------  
//  void
//
//  Value Parameters
//  ----------------
//  signo          int          The signal captured by 'signal' function is passed by this parameter
//
//
//  Local Variables  
//  ---------------
//  ctrlc          static int   Increments when ctrl+C is pressed
//  ctrlz          static int   Increments when ctrl+Z is pressed
//  ctrlq          static int   Increments when ctrl+\ is pressed
//  
//********************************************************


void signalint(int signo)
{
  static int ctrlc;
  static int ctrlz;
  static int ctrlq;
  switch(signo)
  {
    case SIGINT:  ctrlc++;
                  break;
    case SIGQUIT: ctrlq++;
                  break;
    case SIGTSTP: ctrlz++;
                  break;
    case EXIT_FLAG:cout <<"You pressed Ctrl+C  "<< ctrlc << " times\n";
                  cout <<"You pressed Ctrl+\\  "<< ctrlq << " times\n";
                  cout <<"You pressed Ctrl+Z  "<< ctrlz << " times\n";
                  break;
    default:      cout << "Signal not identified" << endl;

  }
     
}


//********************************************************
//
//  Shell command read Function
//
//  This function reads the command from the user and tokenize it. 
//  gettoks is the function which does this task. The tokenized command is
//  stored in the two dimentional pointer. Along with this if user enters 'exit' command
//  a exit flag is returned to the main
//
//  Return Value
//  ------------  
//  int                         it may return EXIT_FLAG when user enters exit command, or zero if user have not                     
//                              entered the command else it returns number of arguments 
//
//  Extern Parameters
//  -----------------
//  toks           char**       The tokenized command is  to be stored in this pointer
//
//  Local Variables  
//  ---------------
//  dummy_count    int          Loop iteration variable
//  new_toks       char**       new pointer address that to be stored in the list
//********************************************************


int shellReadCommand()
{
  int ii;

 
  toks = gettoks();
  

  if( toks[0] != NULL )
  {
     
    // simple loop to echo all arguments
    for( ii=0; toks[ii] != NULL; ii++ );
   
    if( !strcmp(toks[0], "exit" ))
     ii = EXIT_FLAG;
      
  }
  else
    return 0;
  return ii;
}


//********************************************************
//
//  Shell prompt Function
//
//  This function displays the prompt to the shell with time, basedir name and command count 
//  It utilizes gettimeofday and get_current_dir_name system calls to print this dynamic prompt
//
//  Return Value
//  ------------  
//  void
//
//  Value Parameters
//  ----------------
//  cmd_num        int          This parameter has dynamic command number passed
//
//
//  Local Variables  
//  ---------------
//  times          char         array to get and display time
//  dir            char*        pointer to store and display the present basedir
//  Value          struct timeval a parameter for gettimeofday system call
//  local          struct tm*   a parameter to set localtime
//********************************************************


void shellPrompt(int cmd_num)
{
      char* dir;
      char times[10];
      struct timeval value;
      struct tm* local;
      dir = get_current_dir_name();
      gettimeofday(&value,NULL);
      local = localtime (&value.tv_sec);
      strftime(times,sizeof(times),"%T",local);

      cout << "[" << "IslandShell " << times << " "<<basename(dir) <<" " <<cmd_num <<"] @";

}



//********************************************************
//
//  Shell start here Function
//
//  The child process created by the fork is overloded with execvp in this function 
//  I/O redirection is done here with the help of dup2 system call. The command entered
//  by the user is passed a parameter through double pointer toks to execvp.
//   
//
//  Return Value
//  ------------  
//  void
//
//  Value Parameters
//  ----------------
//  arguments       int          The argumets count of the command that to be exec
//
//  Extern Parameters
//  -----------------
//  toks           char**       The tokenized command is pointed by this double pointer
//
//  Local Variables  
//  ---------------
//  fd1,fd2        int          File discriptors 
//  ioret          int          returned by the function ioRedirection if any input or output redirections occur
//  ii             int          loop variable
//********************************************************


void shellStartHere(int arguments)
{
  int fd1,fd2,flag,ioret;
  
  

  for(int ii=0; toks[ii] != NULL; ii++ )
    {

      ioret = ioRedirection(ii);
      if(ioret == 1)
      {
        fd1 = open(toks[ii+1],O_RDONLY);
        dup2(fd1,0);
        toks[ii] = NULL;
        close(fd1); 
      }
      else if(ioret == 2)
      {
        fd2 = open(toks[ii+1],O_RDWR | O_CREAT,0777);
        dup2(fd2,1);
        toks[ii] = NULL;
        close(fd2);
      }

    }
      
  flag = execvp(*toks,toks);
  if(flag <= EXIT_FLAG)
  {
    cout <<"-IslandShell  "<< *toks <<": command not found  arguments : "<<arguments-1 << "\n";
    cout <<"\t[Program returned exit code "<< flag << "]" <<endl;
    exit(1); 
  }
}



//********************************************************
//
//  I/O redirection Function
//
//  This function check for any redirection in the give command  
//  and returns a value according to the redirection that exits 
//
//  Return Value
//  ------------  
//  int                         return a value either 1 or 2 or 0 based on the I/O redirection
//
//  Value Parameters
//  ----------------
//  ii             int          index of the tokenized pointer that need to be vedified for I/O redirection
//
//  Extern Parameters
//  -----------------
//  toks           char**       The tokenized command that to be stored
//
//  Local Variables  
//  ---------------
//  count          int          return value based on the I/o redirection that exists
//  
//********************************************************

 
int ioRedirection(int ii)
{
    int count= 0;
   
        if( !strcmp( toks[ii], "<" ))
            count++;
 
        if( !strcmp( toks[ii], ">" ))
            count=+2; 

    return count;
 
}

