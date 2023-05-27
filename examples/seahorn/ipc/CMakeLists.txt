set(IPC_SRC ${CMAKE_CURRENT_SOURCE_DIR}/lib/ipc.c)
set(IPC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(lib)  # function under verification
add_subdirectory(fake)  # fake env unitproof
add_subdirectory(summary) # summary env unitproof
add_subdirectory(vmock) # vmock env unitproof
