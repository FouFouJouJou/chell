#ifndef __HISTORY_H__
#define __HISTORY_H__
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#define HISTORY_PATH ".history"
#define HISTORY_BUFFER_SIZE_BYTES 1
#define MAX_HISTORY_BUFFER_SIZE 1<<(HISTORY_BUFFER_SIZE_BYTES*8)

void log_history();
void save_to_buffer(char *cmd, size_t len);
ssize_t read_from_fs(char *path);
ssize_t save_to_fs();
ssize_t sync_buffer();


#endif
