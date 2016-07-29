/*****************************************************
This is the demo sketch for the command line interface
by FreakLabs. It's a simple command line interface
where you can define your own commands and pass arguments
to them. 
*****************************************************/
#include <Cmd.h>

void setup()
{
  // init the command line and set it for a speed of 57600
  cmdInit(57600);
  
  // add the commands to the command table. These functions must
  // already exist in the sketch. See the functions below. 
  // The functions need to have the format:
  //
  // void func_name(int arg_cnt, char **args)
  //
  // arg_cnt is the number of arguments typed into the command line
  // args is a list of argument strings that were typed into the command line
  cmdAdd("args", arg_display);
}

void loop()
{
  cmdPoll();
}

// Example to show what the argument count and arguments look like. The
// arg_cnt is the number of arguments typed in by the user. "char **args" is 
// a bit nasty looking, but its a list of the arguments typed in as ASCII strings. 
// In C, char *something means an array of characters, aka a string. So
// char **something is an array of an array of characters, or a string array.
// 
// Usage: At the command line, type
// args hello world i love you 3 4 5 yay
//
// The output should look like this:
// Arg 0: args
// Arg 1: hello
// Arg 2: world
// Arg 3: i
// Arg 4: love
// Arg 5: you
// Arg 6: 3
// Arg 7: 4
// Arg 8: 5
// Arg 9: yay
void arg_display(int arg_cnt, char **args)
{
  for (int i=0; i<arg_cnt; i++)
  {
    Serial.print("Arg ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(args[i]);
  }
}
