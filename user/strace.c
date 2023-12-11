#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  if(argc < 3)
  {
    printf("Error: Invalid input\n");
    return 1;
  }

  char *command = argv[2];
  int mask = atoi(argv[1]);
	
  trace(mask);
  exec(command, &argv[2]);

  return 0;
}