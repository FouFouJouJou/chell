#include <stdint.h>
#include <stddef.h>
#include <history.h>

char *history_buffer[MAX_HISTORY_BUFFER_SIZE]={0};
int8_t idx=0;

void save_to_buffer(char *cmd, size_t len) {}
ssize_t read_from_fs(char *buffer[MAX_HISTORY_BUFFER_SIZE]) { return 0; }
ssize_t save_to_fs(char *cmd, size_t len) { return 0; }
ssize_t sync_buffer() { return 0; }
