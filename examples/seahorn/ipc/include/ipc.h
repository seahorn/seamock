#ifndef IPC_H_
#define IPC_H_

#include <stddef.h>

typedef int (*msg_handler_t)(char *msg, size_t msg_size);
int do_handle_msg(msg_handler_t, int);

#endif // IPC_H_
