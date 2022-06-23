<<<<<<< HEAD
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  if(link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
}
=======
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // New ln function:
  int (*ln)(const char*, const char*);
  
  ln = link;
  if(argc > 1 && strcmp(argv[1], "-s") == 0){
    ln = symlink;
    argc--;
    argv++;
  }
  
  if(argc != 3){
    fprintf(2, "usage: ln [-s] old new (%d)\n", argc);
    exit(0);
  }
  if(ln(argv[1], argv[2]) < 0){
    fprintf(2, "%s failed\n", ln == symlink ? "symlink" : "link");
    exit(0);
  }
  exit(0);


  // old ln function:
  /*
  if(argc != 3){
    fprintf(2, "Usage: ln old new\n");
    exit(1);
  }
  if(link(argv[1], argv[2]) < 0)
    fprintf(2, "link %s %s: failed\n", argv[1], argv[2]);
  exit(0);
  */
}
>>>>>>> 1a2348da2425dbd7d704441efb14fc2685e14760
