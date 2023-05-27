#include <env.h>
#include <ipc.h>
#include <string.h>

#include <seahorn/seahorn.h>
#include <stdbool.h>
#include <stdlib.h>

#define ND __declspec(noalias)

extern ND void memhavoc(void *ptr, size_t size);
extern ND size_t nd_size_t(void);
extern ND int nd_int(void);
extern ND bool nd_bool(void);
extern void sea_printf(const char *format, ...);

extern void sea_reset_modified(char *);
extern bool sea_is_modified(char *);
extern void sea_tracking_on(void);
extern void sea_tracking_off(void);

#define MAX_SIZE 4096

// function summary env begins
int create_channel() { /* NOP */
  return nd_int();
}
int wait_for_msg() { /* NOP */
  return nd_int();
}

int get_msg(int chan, size_t *len) {
  sassert(chan > 0 && len != NULL);
  *len = nd_size_t();
  return nd_int(); // error_code
}

int read_msg(int chan, char *msg) {
  sassert(chan > 0 && msg != NULL);
  char buf[MAX_SIZE];
  size_t len = nd_size_t();
  assume(len < MAX_SIZE);
  memhavoc(&buf, len);
  memcpy(msg, buf, len);
  sea_reset_modified(msg);
  return nd_bool() ? -1 : len;
}

int put_msg(int chan) {
  sassert(chan > 0);
  return nd_int();
}

// unit proof begins
static int test_msg_handler(char *msg, size_t msg_size) {
  sassert(!sea_is_modified((char *)msg));
  return nd_int();
}

int main(void) {
  // create 2 channels
  create_channel();
  create_channel();
  int chan = wait_for_msg();
  do_handle_msg(&test_msg_handler, chan);
  return 0;
}
