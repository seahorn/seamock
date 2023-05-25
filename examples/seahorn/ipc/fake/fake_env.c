#include <env.h>
#include <ipc.h>
#include <string.h>

#include <seahorn/seahorn.h>
#include <stdlib.h>

#define ND __declspec(noalias)

extern ND void memhavoc(void *ptr, size_t size);
extern ND size_t nd_size_t(void);
extern ND int nd_int(void);

extern void sea_printf(const char *format, ...);

extern void sea_reset_modified(char *);
extern bool sea_is_modified(char *);
extern void sea_tracking_on(void);
extern void sea_tracking_off(void);

#define MAX_CHANNELS 10

// The next channel number to initialize
// Incremented on every create_channel call
static int g_next_available_channel;
// The first yet unprocessed channel
// Incremented on every put_msg call
static int g_already_processed_channel;

typedef struct msg {
  char *buf;
  size_t len;
} MSG;

static MSG msgs[MAX_CHANNELS];

int create_channel() {
  sassert(g_next_available_channel < MAX_CHANNELS);
  size_t len = nd_size_t();
  char *msg = (char *)malloc(len);
  memhavoc(msg, len);
  int chan = g_next_available_channel++;
  msgs[chan].buf = msg;
  msgs[chan].len = len;
  return chan;
}

int wait_for_msg() {
  int channel = nd_int();
  assume(channel > g_already_processed_channel &&
         channel < g_next_available_channel);
  return channel;
}

int get_msg(int chan, size_t *len) {
  int err_code = nd_int();
  if (err_code < 0)
    return err_code;
  sassert(chan > 0 && chan < MAX_CHANNELS && len != NULL);
  *len = msgs[chan].len;
  return 0;
}

int read_msg(int chan, char *msg) {
  sassert(chan > 0 && chan < g_next_available_channel && msg != NULL);
  memcpy(msg, msgs[chan].buf, msgs[chan].len);
  sea_reset_modified(msg);
  return 0;
}

int put_msg(int chan) {
  sassert(chan > 0 && chan < g_next_available_channel);
  int err_code = nd_int();
  if (err_code < 0)
    return err_code;
  free(msgs[chan].buf);
  g_already_processed_channel++;
  return 0;
}

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
