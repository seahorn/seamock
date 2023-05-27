add_executable(mock_env_unit
  mock_env.cc)
target_include_directories(mock_env_unit
  PRIVATE ${IPC_ROOT}/include
  PRIVATE ${SEAMOCK_ROOT}/include
  )
sea_link_libraries(mock_env_unit ipc_lib.ir)
sea_attach_bc_cc(mock_env_unit)
sea_add_unsat_test(mock_env_unit)
