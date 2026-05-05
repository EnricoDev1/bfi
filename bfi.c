#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_LEN 0x4000

char data[MAX_LEN] = {0};
char text[MAX_LEN];

int main(int argc, char *argv[]) {
  /* parse args, if no file is provided, read from STDIN */     
  int fd = 0;
    
  if (argv[1] != NULL) {
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
      perror("open");
      return 1;
    }
  }

  /* read program code */      
  int text_len = read(fd, text, MAX_LEN);
  if (text_len == -1) {
    perror("read");
    return 1;
  }
  close(fd);

  if (text_len == MAX_LEN) {
    fprintf(stderr, "input file might have been truncated...\n");
  }

  /* execute program */
  char *ip = text;
  char *dp = data;
  
  while(ip < text+text_len) {    
     switch(*ip) {
      case '>':
        if (dp+1 < data+MAX_LEN) dp++;
        break;
      case '<':
        if (dp-1 >= data) dp--;
        break;
      case '+':
        (*dp)++;
        break;
      case '-':
        (*dp)--;
        break;
      case '.':
        if (write(STDOUT_FILENO, dp, 1) == -1) {perror("write"); return 1;};
        break;
      case ',':
        if (read(STDIN_FILENO, dp, 1) == -1) {perror("read"); return 1;};
        break;
      case '[':
        if (*dp == 0) {
          int depth = 1;
          while (depth != 0) {
            ip++;
            if (*ip == '[') depth++;
            if (*ip == ']') depth--;
          }          
        }
        break;
      case ']':
        if (*dp != 0) {
          int depth = 1;
          while (depth != 0) {
            ip--;
            if (*ip == ']') depth++;
            if (*ip == '[') depth--;
          }
        }
    }
    ip++;
  }
  
  return 0;
}

