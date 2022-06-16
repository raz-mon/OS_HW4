#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // sanity();

  int buf1[1024 * 12];
  memset(buf1, 50, 1024 * 12);

  int buf2[1024 * 256];
  memset(buf2, 50, 1024 * 256);

  int buf3[1024 * 256 * 256];
  memset(buf3, 50, 1024 * 256 * 256);

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