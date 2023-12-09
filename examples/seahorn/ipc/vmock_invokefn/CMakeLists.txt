add_executable(mock_env_unit_invokefn
  mock_env.cc)
target_include_directories(mock_env_unit_invokefn
  PRIVATE ${IPC_ROOT}/include
  PRIVATE ${SEAMOCK_ROOT}/include
  )
sea_link_libraries(mock_env_unit_invokefn ipc_lib.ir)
sea_attach_bc_cc(mock_env_unit_invokefn)
sea_add_unsat_test(mock_env_unit_invokefn)
