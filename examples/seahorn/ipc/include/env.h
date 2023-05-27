#ifndef ENV_H_
#define ENV_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int create_channel(void);
int wait_for_msg(void);
int get_msg(int, size_t *);
int read_msg(int, char *);
int put_msg(int);

#ifdef __cplusplus
}
#endif
#endif // ENV_H_
