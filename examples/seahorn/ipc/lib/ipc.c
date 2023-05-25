#include <env.h>
#include <ipc.h>

#include <stddef.h>
#include <stdlib.h>

#define MAX_SIZE 4096
#define FREE_AND_RET(msg, rc)                                                  \
  do {                                                                         \
    free(msg);                                                                 \
    return rc;                                                                 \
  } while (0)

#define ERROR -1

int do_handle_msg(msg_handler_t msg_handler, int chan) {
  char *msg = (char *)malloc(MAX_SIZE);
  size_t msg_len;
  int rc = get_msg(chan, &msg_len);
  if (rc < 0)
    FREE_AND_RET(msg, ERROR);
  rc = read_msg(chan, msg);
  put_msg(chan);
  if (rc < 0)
    FREE_AND_RET(msg, ERROR);
  if (((size_t)rc) < msg_len)
    FREE_AND_RET(msg, ERROR);
  rc = msg_handler(msg, msg_len);
  FREE_AND_RET(msg, rc);
}
