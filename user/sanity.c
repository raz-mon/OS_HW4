#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // sanity();

  char buf1[1024 * 12];
  memset(buf1, 50, strlen(buf1));

  char buf2[1024 * 256];
  memset(buf2, 50, strlen(buf2));

  char buf3[1024 * 256 * 256];
  memset(buf3, 50, strlen(buf3));

  int fd = open("sanity_test_file", 1);
  write(fd, buf1, 1024 * 12);
  printf("Finished writing 12KB (direct)\n");

  write(fd, buf2, 1024 * 256);
  printf("Finished writing 268KB (single indirect)\n");

  write(fd, buf3, 1024 * 1024 * 10);
  printf("Finished writing 10MB (double indirect)\n");

  close(fd);
  exit(0);
}