add_executable(fake_env_unit
  ${IPC_SRC}
  fake_env.c)
target_include_directories(fake_env_unit
  PRIVATE ${IPC_ROOT}/include
  )
sea_attach_bc_link(fake_env_unit)
sea_add_unsat_test(fake_env_unit)
