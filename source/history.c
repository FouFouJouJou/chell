#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <history.h>

char *history_buffer[MAX_HISTORY_BUFFER_SIZE]={0};
int8_t idx=0;

void log_history() {
  for(int i=0; i<idx; ++i) printf("  %d %s", i+1, history_buffer[i]);
}

void save_to_history_buffer(char *cmd, size_t len) {
  if(idx == MAX_HISTORY_BUFFER_SIZE) {
    printf("[LOG] limit\n");
  }
  history_buffer[idx++]=cmd;
}
ssize_t read_history_from_fs(char *path) { 
  if(path == 0) path=HISTORY_PATH;
  int fd=open(path, O_RDONLY, S_IRUSR);
  if(fd == -1) {
    return 0;
  }
  FILE *file=fdopen(fd, "r");
  char *line=0;
  size_t size;
  while(getline(&line, &size, file) != -1) {
    history_buffer[idx++]=line;
    line=0;
  }
  close(fd);
  return 0;
}

ssize_t save_history_to_fs(char *path) {
  if(path == 0) path=HISTORY_PATH;
  int fd=open(HISTORY_PATH, O_CREAT|O_EXCL|O_WRONLY, S_IRUSR|S_IWUSR);
  if(fd == -1 && errno == EEXIST) {
    fd=open(HISTORY_PATH, O_WRONLY, S_IRUSR|S_IWUSR);
  }
  for(int i=0; i<idx; ++i) {
    write(fd, history_buffer[i], strlen(history_buffer[i])*sizeof(char));
  }
  close(fd);
  return 0;
}
ssize_t sync_buffer() { return 0; }
