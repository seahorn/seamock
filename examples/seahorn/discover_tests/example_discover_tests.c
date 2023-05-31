#include <seahorn/seahorn.h>
#include <stddef.h>

#define TEST(name)                                                             \
  void test_##name(void);                                                      \
  void test_##name()

extern size_t nd_size_t(void);

TEST(testSum1) {
  size_t a = nd_size_t();
  size_t b = nd_size_t();
  assume(a < 10);
  assume(b < 10);
  sassert(a + b < 20);
}

TEST(testSum2) {
  size_t a = nd_size_t();
  size_t b = nd_size_t();
  assume(a < 20);
  assume(b < 20);
  sassert(a + b < 40);
}
