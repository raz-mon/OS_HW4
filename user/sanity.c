#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
main(int argc, char *argv[])
{


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
  exit(0);
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
  