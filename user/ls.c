#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define MAXPATH 128

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  char new_path[MAXPATH];

  // Added: Make sure that here no dereference occurs! Can use different system-call or the O_UNFOLLOW flag.
  if((fd = open_no_dereference(path, 0)) < 0){
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  // Added:
  case T_SYMLINK:
    if (readlink(path, new_path, MAXPATH) < 0)       // This is wrong! 'path' is the path of the father directory.
      printf("ls: bad symlink\n");
    char temp1[MAXPATH];
    memset(temp1,0,MAXPATH);
    strcpy(temp1, path);
    strcpy(temp1+strlen(path), "->");
    strcpy(temp1+strlen(path)+2, new_path);
    // strcpy(buf, temp);
    printf("%s %d %d %l\n", fmtname(temp1), new_path, st.type, st.ino, st.size);
    break;
  // End of addition.

  case T_FILE:
    printf("%s %d %d %l\n", fmtname(path), st.type, st.ino, st.size);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("ls: cannot stat %s\n", buf);
        continue;
      }
      // Add here:
      if (st.type==T_SYMLINK){
        if (readlink(buf, new_path, MAXPATH) < 0)
          printf("ls: readlink\n");
        char temp2[strlen(buf)+strlen(new_path)];
        strcpy(temp2, buf);
        strcpy(temp2+strlen(buf), "->");
        strcpy(temp2+strlen(buf)+2, new_path);
        strcpy(buf, temp2);
      }
      // if (st.type==T_SYMLINK){
      //   if (readlink(buf, new_path, MAXPATH) < 0)       // This is wrong!! 'path' is the path of the father!
      //     printf("ls: readlink\n");
      //   printf("%s->%s %d %d %d\n", fmtname(buf), new_path, st.type, st.ino, st.size);
      // }
      printf("%s %d %d %d\n", fmtname(buf), st.type, st.ino, st.size);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit(0);
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit(0);
}
