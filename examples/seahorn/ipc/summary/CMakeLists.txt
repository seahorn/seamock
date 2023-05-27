add_executable(summ_env_unit
  ${IPC_SRC}
  summ_env.c)
target_include_directories(summ_env_unit
  PRIVATE ${IPC_ROOT}/include
  )
sea_attach_bc_link(summ_env_unit)
sea_add_unsat_test(summ_env_unit)
