#include <env.h>
#include <ipc.h>
#include <string.h>

#include <seamock.hh>
#include <stdlib.h>

#define ND __declspec(noalias)
#define IS_ALIGN64(n) ((size_t)n << (sizeof(size_t) * 8 - 3)) == 0

extern "C" {
extern ND void memhavoc(void *ptr, size_t size);
extern ND size_t nd_size_t(void);
extern ND int nd_int(void);

extern void sea_printf(const char *format, ...);

extern void sea_reset_modified(char *);
extern bool sea_is_modified(char *);
extern void sea_tracking_on(void);
extern void sea_tracking_off(void);
}
static size_t g_msg_size;

// Begin: Mock arg capture
static constexpr auto set_pointer_fn_get_msg = [](size_t *len) {
  *len = nd_size_t();
  assume(IS_ALIGN64(*len)); // seahorn likes word-aligned copies
  g_msg_size = *len;
};

static constexpr auto set_pointer_fn_read_msg = [](char *msg) {
  char *blob = (char *)malloc(g_msg_size);
  memhavoc(blob, g_msg_size);
  sassert(msg);
  sassert(IS_ALIGN64(g_msg_size)); // make sure copying aligned chunk of mem
  sassert(IS_ALIGN64(msg));        // make sure dest is aligned
  sassert(IS_ALIGN64(blob));       // make sure src is aligned
  memcpy((size_t *)msg, (size_t *)blob,
         g_msg_size); // seahorn likes word-aligned copies
  sea_reset_modified(msg);
};

// *** End: Mock arg capture ***
// *** Begin: mock expect definition ***

constexpr auto get_msg_expectations = seamock::ExpectationBuilder()
                                              .times(seamock::Eq<1>())
                                              .returnFn(nd_int)
                                              .captureArgAndInvoke<1>(set_pointer_fn_get_msg)
                                              .build();

constexpr auto read_msg_expectations = seamock::ExpectationBuilder()
                                              .times(seamock::Lt<2>())
                                              .returnFn(MOCK_UTIL_WRAP_VAL(g_msg_size))
                                              .captureArgAndInvoke<1>(set_pointer_fn_read_msg)
                                              .build();
// *** End: mock expect definition ***

extern "C" {

MOCK_FUNCTION(get_msg, get_msg_expectations, int, (int, size_t *))

MOCK_FUNCTION_W_ORDER(read_msg, read_msg_expectations, MAKE_PRED_FN_SET(get_msg), int, (int, char *))

LAZY_MOCK_FUNCTION(put_msg, int, (int))

SETUP_POST_CHECKS((get_msg, read_msg, put_msg))
// *** End: mock definition ***

// Unit proof begins
static int test_msg_handler(char *msg, size_t msg_size) {
  sassert(!sea_is_modified((char *)msg));
  return nd_int();
}

int main(void) {
  int chan = nd_int();
  do_handle_msg(&test_msg_handler, chan);
  postchecks_ok();
  return 0;
}
}
