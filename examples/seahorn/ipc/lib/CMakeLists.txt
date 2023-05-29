add_library(ipc_lib
  ${IPC_SRC})
target_include_directories(ipc_lib
  PRIVATE ${IPC_ROOT}/include
  )
sea_attach_bc_link(ipc_lib)
