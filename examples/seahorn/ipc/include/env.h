#ifndef ENV_H_
#define ENV_H_

#include <stddef.h>

int create_channel(void);
int wait_for_msg(void);
int get_msg(int, size_t *);
int read_msg(int, char *);
int put_msg(int);

#endif // ENV_H_
