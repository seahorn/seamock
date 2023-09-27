#ifndef SEAMOCK_UNIT_PROOF_H_
#define SEAMOCK_UNIT_PROOF_H_

#define DEFINE_UNIT_PROOF(name)                                                \
  extern void postchecks_ok(void);                                             \
  void test_##name(void);                                                      \
  void test_##name(void)

#define CALL_UNIT_PROOF(name)                                                  \
  test_##name();                                                               \
  postchecks_ok();

#endif // SEAMOCK_UNIT_PROOF_H_
