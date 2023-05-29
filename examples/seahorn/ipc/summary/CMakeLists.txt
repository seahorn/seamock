add_executable(summ_env_unit
  ${IPC_SRC}
  summ_env.c)
target_include_directories(summ_env_unit
  PRIVATE ${IPC_ROOT}/include
  )
sea_attach_bc_link(summ_env_unit)
# Test is expected to fail since model is too weak.
sea_add_will_fail_unsat_test(summ_env_unit)
