# casts between a handle_t and int.
# This breaks the original spec that handle_t is a 32-bit opaque number
# and hence breaks our impl of type(handle_t) == 32 bit pointer.
add_executable(fake_env_unit
  ${IPC_SRC}
  fake_env.c)
target_include_directories(fake_env_unit
  PRIVATE ${IPC_ROOT}/include
  )
sea_attach_bc_link(fake_env_unit)
sea_add_unsat_test(fake_env_unit)
