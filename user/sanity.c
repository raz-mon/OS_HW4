#include "kernel/param.h"
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/riscv.h"
#include "kernel/fcntl.h"
#include "kernel/spinlock.h"
#include "kernel/sleeplock.h"
#include "kernel/fs.h"
#include "kernel/file.h"
#include "user/user.h"
 
#define fail(msg) do {printf("FAILURE: " msg "\n"); failed = 1; goto done;} while (0);

int check_big_fs(void);
int main_symlink(void);

int
main(int argc, char *argv[])
{
  // check_big_fs();
  main_symlink();
  exit(0);
}




int check_big_fs(void){
  char buf[1024];
  int fd, i, blocks;
  fd = open("big.file", O_CREATE | O_RDWR);
  if (fd < 0){
    fprintf(2, "Cannot open big.file for RDWR\n");
    exit(0);
  }

  blocks = 0;
  for (i = 0; i < 12; i++){
    *(int*)buf = blocks;
    int cc = write(fd, buf, sizeof(buf));
    if (cc <= 0)
      break;
    blocks++;
  }
  fprintf(1, "Finished writing 12KB to the direct blocks\n");
  
  for (i = 0; i < 256; i++){
    *(int*)buf = blocks;
    int cc = write(fd, buf, sizeof(buf));
    if (cc <= 0)
      break;
    blocks++;
  }
  fprintf(1, "Finished writing another 256KB to the singly indirect blocks\n");

  for (i = 0; i < 4096; i++){
    *(int*)buf = blocks;
    int cc = write(fd, buf, sizeof(buf));
    if (cc <= 0)
      break;
    blocks++;
  }
  fprintf(1, "Finished writing another 10MB to the doubly indirect blocks\n");
  
  close(fd);
  return 1;
}




/*
// Check chdir new functionality:
Mkdir dir1
Mkdir /dir1/dir2
Ln -s dir1 dir1_lnk
Cd dir1_lnk
Cd dir2
*/









static int failed = 0;
 
static void testsymlink(void);
static void concur(void);
static void cleanup(void);
 
int
main_symlink()
{
  cleanup();
  testsymlink();
  concur();
  exit(failed);
}
 
static void
cleanup(void)
{
  unlink("/testsymlink/a");
  unlink("/testsymlink/b");
  unlink("/testsymlink/c");
  unlink("/testsymlink/1");
  unlink("/testsymlink/2");
  unlink("/testsymlink/3");
  unlink("/testsymlink/4");
  unlink("/testsymlink/z");
  unlink("/testsymlink/y");
  unlink("/testsymlink");
}
 
// stat a symbolic link using O_NOFOLLOW
static int
stat_slink(char *pn, struct stat *st)
{
  int fd = open(pn, O_RDONLY | O_NOFOLLOW);
  if(fd < 0)
    return -1;
  if(fstat(fd, st) != 0)
    return -1;
  return 0;
}
 
static void
testsymlink(void)
{
  int r, fd1 = -1, fd2 = -1;
  char buf[4] = {'a', 'b', 'c', 'd'};
  char c = 0, c2 = 0;
  struct stat st;
 
  printf("Start: test symlinks\n");
 
  mkdir("/testsymlink");
 
  fd1 = open("/testsymlink/a", O_CREATE | O_RDWR);
  if(fd1 < 0) fail("failed to open a");
 
  r = symlink("/testsymlink/a", "/testsymlink/b");
  if(r < 0)
    fail("symlink b -> a failed");
 
  if(write(fd1, buf, sizeof(buf)) != 4)
    fail("failed to write to a");
 
  if (stat_slink("/testsymlink/b", &st) != 0)
    fail("failed to stat b");
  if(st.type != T_SYMLINK)
    fail("b isn't a symlink");
 
  fd2 = open("/testsymlink/b", O_RDWR);
  if(fd2 < 0)
    fail("failed to open b");

  read(fd2, &c, 1);
  if (c != 'a') {
    fail("failed to read bytes from b");
  }

 
  unlink("/testsymlink/a");
 
  r = symlink("/testsymlink/b", "/testsymlink/a");
  if(r < 0)
    fail("symlink a -> b failed");
 
  r = open("/testsymlink/b", O_RDWR);
  if(r >= 0)
    fail("Should not be able to open b (cycle b->a->b->..)\n");
  // printf("Got here\n");
  r = symlink("/testsymlink/nonexistent", "/testsymlink/c");
  if(r != 0)
    fail("Symlinking to nonexistent file should succeed\n");
  r = symlink("/testsymlink/2", "/testsymlink/1");
  if(r) fail("Failed to link 1->2");
  r = symlink("/testsymlink/3", "/testsymlink/2");
  if(r) fail("Failed to link 2->3");
  r = symlink("/testsymlink/4", "/testsymlink/3");
  if(r) fail("Failed to link 3->4");
 
  close(fd1);
  close(fd2);
  fd1 = open("/testsymlink/4", O_CREATE | O_RDWR);
  if(fd1<0) fail("Failed to create 4\n");
  fd2 = open("/testsymlink/1", O_RDWR);
  if(fd2<0) fail("Failed to open 1\n");
 
  c = '#';
  r = write(fd2, &c, 1);
  if(r!=1) fail("Failed to write to 1\n");
  r = read(fd1, &c2, 1);
  if(r!=1) fail("Failed to read from 4\n");
  if(c!=c2)
    fail("Value read from 4 differed from value written to 1\n");
 
  printf("test symlinks: ok\n");
done:
  close(fd1);
  close(fd2);
}
 
static void
concur(void)
{
  int pid, i;
  int fd;
  struct stat st;
  int nchild = 2;
 
  printf("Start: test concurrent symlinks\n");
 
  // mkdir("/testsymlink");

  fd = open("/testsymlink/z", O_CREATE | O_RDWR);
  if(fd < 0) {
    printf("FAILED: open failed");
    exit(1);
  }
  close(fd);
 
  for(int j = 0; j < nchild; j++) {
    pid = fork();
    if(pid < 0){
      printf("FAILED: fork failed\n");
      exit(1);
    }
    if(pid == 0) {
      int m = 0;
      unsigned int x = (pid ? 1 : 97);
      for(i = 0; i < 100; i++){
        x = x * 1103515245 + 12345;
        if((x % 3) == 0) {
          symlink("/testsymlink/z", "/testsymlink/y");
          if (stat_slink("/testsymlink/y", &st) == 0) {
            m++;
            if(st.type != T_SYMLINK) {
              printf("FAILED: not a symbolic link\n", st.type);
              exit(1);
            }
          }
        } else {
          unlink("/testsymlink/y");
        }
      }
      exit(0);
    }
  }
 
  int r;
  for(int j = 0; j < nchild; j++) {
    wait(&r);
    if(r != 0) {
      printf("test concurrent symlinks: failed\n");
      exit(1);
    }
  }
  printf("test concurrent symlinks: ok\n");
}




















  // char buf[512];
  // int fd, i, sectors;

  // fd = open("big.file", O_CREATE | O_WRONLY);
  // if(fd < 0){
  //   fprintf(2, "big: cannot open big.file for writing\n");
  //   exit(0);
  // }

  // sectors = 0;
  // while(1){
  //   *(int*)buf = sectors;
  //   int cc = write(fd, buf, sizeof(buf));
  //   if(cc <= 0)
  //     break;
  //   sectors++;
	// if (sectors % 100 == 0)
	// 	fprintf(2, ".");
  // }

  // fprintf(1, "\nwrote %d sectors\n", sectors);

  // close(fd);
  // fd = open("big.file", O_RDONLY);
  // if(fd < 0){
  //   fprintf(2, "big: cannot re-open big.file for reading\n");
  //   exit(0);
  // }
  // for(i = 0; i < sectors; i++){
  //   int cc = read(fd, buf, sizeof(buf));
  //   if(cc <= 0){
  //     fprintf(2, "big: read error at sector %d\n", i);
  //     exit(0);
  //   }
  //   if(*(int*)buf != i){
  //     fprintf(2, "big: read the wrong data (%d) for sector %d\n",
  //            *(int*)buf, i);
  //     exit(0);
  //   }
  // }

  // fprintf(1, "done; ok\n"); 

  // exit(0);





  
  
  
  
  // sanity();

  // char buf1[1024 * 12];
  // // memset(buf1, 50, sizeof(buf1));

  // char buf2[1024 * 256];
  // // memset(buf2, 50, sizeof(buf2));

  // char buf3[1024 * 256 * 256];
  // // memset(buf3, 50, sizeof(buf3));

  // int fd = open("sanity_test_file.file", O_CREATE | O_RDWR);
  // write(fd, buf1, sizeof(buf1));
  // printf("Finished writing 12KB (direct)\n");

  // write(fd, buf2, sizeof(buf2));
  // printf("Finished writing 268KB (single indirect)\n");

  // write(fd, buf3, sizeof(buf3));
  // printf("Finished writing 10MB (double indirect)\n");

  // close(fd);
  // exit(0);
  