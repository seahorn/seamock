function(sea_llvm_link name)
  set(options)
  set(oneValueArgs)
  set(multiValueArgs LINK OVERRIDE)
  cmake_parse_arguments(LLVMIR_ATTACH
    "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT LLVMIR_ATTACH_LINK)
    message(FATAL_ERROR "sea_llvm_link: missing LINK option")
  endif()

  set(LLVMIR_TARGETS ${LLVMIR_ATTACH_LINK})
  set(LLVMIR_OVERRIDE_TARGETS ${LLVMIR_ATTACH_OVERRIDE})

  set(TRGT ${name})
  # use the first target as the main dependency
  list(GET LLVMIR_TARGETS 0 DEPENDS_TRGT)


    ## preamble
  foreach(IR ${LLVMIR_TARGETS})
    llvmir_check_target_properties(${IR})
  endforeach()

  set(OUT_LLVMIR_FILES "")
  set(FULL_OUT_LLVMIR_FILES "")

  get_property(IN_LLVMIR_TYPE TARGET ${DEPENDS_TRGT} PROPERTY LLVMIR_TYPE)
  get_property(LLVMIR_EXTERNAL_TYPE TARGET ${DEPENDS_TRGT}
    PROPERTY LLVMIR_EXTERNAL_TYPE)
  get_property(INFILES TARGET ${DEPENDS_TRGT} PROPERTY LLVMIR_FILES)
  get_property(IN_LLVMIR_DIR TARGET ${DEPENDS_TRGT} PROPERTY LLVMIR_DIR)
  get_property(LINKER_LANGUAGE TARGET ${DEPENDS_TRGT} PROPERTY LINKER_LANGUAGE)
  get_property(LINK_DEPENDS TARGET ${DEPENDS_TRGT} PROPERTY LINK_DEPENDS)
  get_property(LINK_FLAGS TARGET ${DEPENDS_TRGT} PROPERTY LINK_FLAGS)
  get_property(LINK_FLAGS_${CMAKE_BUILD_TYPE}
    TARGET ${DEPENDS_TRGT}
    PROPERTY LINK_FLAGS_${CMAKE_BUILD_TYPE})
  get_property(INTERFACE_LINK_LIBRARIES
    TARGET ${DEPENDS_TRGT}
    PROPERTY INTERFACE_LINK_LIBRARIES)
  get_property(LINK_LIBRARIES TARGET ${DEPENDS_TRGT} PROPERTY LINK_LIBRARIES)
  get_property(LINK_INTERFACE_LIBRARIES
    TARGET ${DEPENDS_TRGT}
    PROPERTY LINK_INTERFACE_LIBRARIES)
  get_property(LINK_INTERFACE_LIBRARIES_${CMAKE_BUILD_TYPE}
    TARGET ${DEPENDS_TRGT}
    PROPERTY LINK_INTERFACE_LIBRARIES_${CMAKE_BUILD_TYPE})
  get_property(SHORT_NAME TARGET ${DEPENDS_TRGT} PROPERTY LLVMIR_SHORT_NAME)

  if(NOT "${IN_LLVMIR_TYPE}" STREQUAL "${LLVMIR_BINARY_TYPE}")
    message(FATAL_ERROR "Cannot attach ${TRGT} to a ${IN_LLVMIR_TYPE} target.")
  endif()

  ## main operations
  set(WORK_DIR "${CMAKE_CURRENT_BINARY_DIR}/${LLVMIR_DIR}/${TRGT}")
  file(MAKE_DIRECTORY ${WORK_DIR})

  set(IN_FULL_LLVMIR_FILES "")
  foreach(IR ${LLVMIR_TARGETS})
    get_property(INFILES TARGET ${IR} PROPERTY LLVMIR_FILES)
    get_property(IN_LLVMIR_DIR TARGET ${IR} PROPERTY LLVMIR_DIR)
    foreach(IN_LLVMIR_FILE ${INFILES})
      list(APPEND IN_FULL_LLVMIR_FILES "${IN_LLVMIR_DIR}/${IN_LLVMIR_FILE}")
    endforeach()
  endforeach()

  get_property(IN_LLVMIR_LINK TARGET ${DEPENDS_TRGT} PROPERTY LLVMIR_LINK)
  foreach(IR ${IN_LLVMIR_LINK})
    get_property(INFILES TARGET ${IR} PROPERTY LLVMIR_FILES)
    get_property(IN_LLVMIR_DIR TARGET ${IR} PROPERTY LLVMIR_DIR)
    foreach(IN_LLVMIR_FILE ${INFILES})
      list(APPEND IN_FULL_LLVMIR_FILES "${IN_LLVMIR_DIR}/${IN_LLVMIR_FILE}")
    endforeach()
  endforeach()


  set(IN_FULL_LLVMIR_OVERIDE_FILES "")
  foreach(IR ${LLVMIR_OVERRIDE_TARGETS})
    get_property(INFILES TARGET ${IR} PROPERTY LLVMIR_FILES)
    get_property(IN_LLVMIR_DIR TARGET ${IR} PROPERTY LLVMIR_DIR)
    foreach(IN_LLVMIR_FILE ${INFILES})
      list(APPEND IN_FULL_LLVMIR_OVERRIDE_FILES "${IN_LLVMIR_DIR}/${IN_LLVMIR_FILE}")
    endforeach()
  endforeach()

  get_property(IN_LLVMIR_OVERRIDE TARGET ${DEPENDS_TRGT} PROPERTY LLVMIR_OVERRIDE)
  foreach(IR ${IN_LLVMIR_OVERRIDE})
    get_property(INFILES TARGET ${IR} PROPERTY LLVMIR_FILES)
    get_property(IN_LLVMIR_DIR TARGET ${IR} PROPERTY LLVMIR_DIR)
    foreach(IN_LLVMIR_FILE ${INFILES})
      list(APPEND IN_FULL_LLVMIR_OVERRIDE_FILES "${IN_LLVMIR_DIR}/${IN_LLVMIR_FILE}")
    endforeach()
  endforeach()

  set(IN_OVERRIDE_OPTIONS ${IN_FULL_LLVMIR_OVERRIDE_FILES})
  list(TRANSFORM IN_OVERRIDE_OPTIONS PREPEND "--override=" )

  set(FULL_OUT_LLVMIR_FILE "${WORK_DIR}/${TRGT}.${LLVMIR_BINARY_FMT_SUFFIX}")
  if(SHORT_NAME)
    set(FULL_OUT_LLVMIR_FILE
      "${WORK_DIR}/${SHORT_NAME}.${LLVMIR_BINARY_FMT_SUFFIX}")
  endif()
  get_filename_component(OUT_LLVMIR_FILE ${FULL_OUT_LLVMIR_FILE} NAME)

  list(APPEND OUT_LLVMIR_FILES ${OUT_LLVMIR_FILE})
  list(APPEND FULL_OUT_LLVMIR_FILES ${FULL_OUT_LLVMIR_FILE})

  # setup custom target
  add_custom_target(${TRGT} DEPENDS ${FULL_OUT_LLVMIR_FILES})

  set_property(TARGET ${TRGT} PROPERTY LLVMIR_TYPE ${LLVMIR_BINARY_TYPE})
  set_property(TARGET ${TRGT}
    PROPERTY LLVMIR_EXTERNAL_TYPE ${LLVMIR_EXTERNAL_TYPE})
  set_property(TARGET ${TRGT} PROPERTY LLVMIR_DIR ${WORK_DIR})
  set_property(TARGET ${TRGT} PROPERTY LLVMIR_FILES ${OUT_LLVMIR_FILES})
  set_property(TARGET ${TRGT} PROPERTY LINKER_LANGUAGE ${LINKER_LANGUAGE})
  set_property(TARGET ${TRGT} PROPERTY LINK_DEPENDS ${LINK_DEPENDS})
  set_property(TARGET ${TRGT} PROPERTY LINK_FLAGS ${LINK_FLAGS})
  set_property(TARGET ${TRGT}
    PROPERTY LINK_FLAGS_${CMAKE_BUILD_TYPE} ${LINK_FLAGS_${CMAKE_BUILD_TYPE}})
  set_property(TARGET ${TRGT}
    PROPERTY INTERFACE_LINK_LIBRARIES ${INTERFACE_LINK_LIBRARIES})
  set_property(TARGET ${TRGT}
    PROPERTY LINK_INTERFACE_LIBRARIES ${LINK_INTERFACE_LIBRARIES})
  set_property(TARGET ${TRGT}
    PROPERTY
    LINK_INTERFACE_LIBRARIES_${CMAKE_BUILD_TYPE}
    ${LINK_INTERFACE_LIBRARIES_${CMAKE_BUILD_TYPE}})
  set_property(TARGET ${TRGT} PROPERTY EXCLUDE_FROM_ALL OFF)
  set_property(TARGET ${TRGT} PROPERTY LLVMIR_SHORT_NAME ${SHORT_NAME})

  add_custom_command(OUTPUT ${FULL_OUT_LLVMIR_FILE}
    COMMAND ${LLVMIR_LINK}
    ARGS
    ${LLVMIR_ATTACH_UNPARSED_ARGUMENTS}
    -o ${FULL_OUT_LLVMIR_FILE} ${IN_FULL_LLVMIR_FILES} ${IN_OVERRIDE_OPTIONS}
    DEPENDS ${IN_FULL_LLVMIR_FILES} ${IN_FULL_LLVMIR_OVERRIDE_FILES}
    COMMENT "Linking LLVM bitcode ${OUT_LLVMIR_FILE}"
    VERBATIM)

  ## postamble
endfunction()

function(sea_link_libraries target)
  set_property(TARGET ${target} APPEND PROPERTY LLVMIR_LINK ${ARGN})
endfunction()
function(sea_overlink_libraries target)
  set_property(TARGET ${target} APPEND PROPERTY LLVMIR_OVERRIDE ${ARGN})
endfunction()

function(sea_attach_bc name)
  set(options)
  set(oneValueArgs LINKER_LANGUAGE)
  set(multiValueArgs)
  cmake_parse_arguments(ARGS
    "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT ARGS_LINKER_LANGUAGE)
    set(ARGS_LINKER_LANGUAGE "C")
  endif()

  set(SOURCE_EXE ${name})
  set(TARGET_BC "${SOURCE_EXE}.ir")
  set(TARGET_OBJ "${SOURCE_EXE}.obj")

  # setup target with the options that we need
  set_target_properties(${SOURCE_EXE} PROPERTIES LINKER_LANGUAGE ${ARGS_LINKER_LANGUAGE})
  target_compile_options(${SOURCE_EXE} PUBLIC -O1 -Xclang -disable-llvm-optzns -S)
  target_compile_options(${SOURCE_EXE} PUBLIC -Wno-macro-redefined)
  target_compile_options(${SOURCE_EXE} PUBLIC -g)
  target_compile_options(${SOURCE_EXE} PUBLIC -Werror)  # warnings are errors
  target_compile_options(${SOURCE_EXE} PUBLIC -fdeclspec)
  target_compile_options(${SOURCE_EXE} PUBLIC -Wstrict-prototypes -Wwrite-strings)

  set_property(TARGET ${SOURCE_EXE} PROPERTY EXCLUDE_FROM_ALL ON)

  if (ARGS_LINKER_LANGUAGE STREQUAL "CXX")
    target_compile_options(${SOURCE_EXE} PUBLIC -fno-rtti -fno-exceptions)
  endif()

  target_compile_definitions(${SOURCE_EXE} PRIVATE __SEAHORN__)
  target_compile_definitions(${SOURCE_EXE} PRIVATE VACCHECK)

  # convert ${SOURCE_EXE} target to llvm
  llvmir_attach_bc_target(TARGET ${TARGET_OBJ} DEPENDS ${SOURCE_EXE})
  set_property(TARGET ${TARGET_OBJ} PROPERTY EXCLUDE_FROM_ALL ON)

  get_property(IN_LLVMIR_LINK TARGET ${SOURCE_EXE} PROPERTY LLVMIR_LINK)
  get_property(IN_LLVMIR_OVERRIDE TARGET ${SOURCE_EXE} PROPERTY LLVMIR_OVERRIDE)
  set_property(TARGET ${TARGET_OBJ} PROPERTY LLVMIR_LINK ${IN_LLVMIR_LINK})
  set_property(TARGET ${TARGET_OBJ} PROPERTY LLVMIR_OVERRIDE ${IN_LLVMIR_OVERRIDE})
  # link all files of ${TARGET_OBJ} together
  # llvmir_attach_link_target(TARGET ${TARGET_BC} DEPENDS ${TARGET_OBJ})
  sea_llvm_link(${TARGET_BC} LINK ${TARGET_OBJ})
  add_dependencies(${TARGET_BC} ${TARGET_OBJ})

  set_property(TARGET ${TARGET_BC} PROPERTY EXCLUDE_FROM_ALL OFF)
endfunction()

# for C++ code
function(sea_attach_bc_cc name)
  sea_attach_bc(${name} LINKER_LANGUAGE CXX)
endfunction()

function(sea_get_file_name VAR LLVMIR_TARGET)
  get_property(DIR TARGET ${LLVMIR_TARGET} PROPERTY LLVMIR_DIR)
  get_property(FILES TARGET ${LLVMIR_TARGET} PROPERTY LLVMIR_FILES)
  set(${VAR} "${DIR}/${FILES}" PARENT_SCOPE)
endfunction()

function(sea_attach_bc_link name)
  sea_attach_bc(${name})
endfunction()

function(sea_attach_bc_cc_link name)
  sea_attach_bc_cc(${name})
endfunction()

function(add_subset_test_target TEST_TARGET_NAME TEST_EXEC SET_NAME)

# This adds the "make subset test" target.
add_custom_target(${TEST_TARGET_NAME}
    COMMAND ${CMAKE_CTEST_COMMAND} -R ${TEST_EXEC}
    COMMENT "Running ${SET_NAME} unit tests.")
endfunction()

function(sea_add_unsat_test TARGET)
  sea_get_file_name(BC ${TARGET}.ir)
  add_test(NAME "${TARGET}_unsat_test" COMMAND ${VERIFY_CMD} ${VERIFY_FLAGS} --expect=unsat ${BC})
endfunction()

function(sea_add_will_fail_unsat_test TARGET)
  sea_get_file_name(BC ${TARGET}.ir)
  add_test(NAME "${TARGET}_unsat_test" COMMAND ${VERIFY_CMD} ${VERIFY_FLAGS} --expect=unsat ${BC})
  set_property(TEST "${TARGET}_unsat_test" PROPERTY WILL_FAIL TRUE)
endfunction()

function(sea_discover_tests TARGET)
  sea_get_file_name(BC ${TARGET}.ir)
  cmake_path(SET bcpath "${BC}")
  cmake_path(GET bcpath PARENT_PATH bc_dir_path)
  cmake_path(APPEND bc_dir_path "CTestTestfile.cmake" OUTPUT_VARIABLE ctest_file)
  string(CONCAT target_ir ${TARGET} ".ir")
  add_custom_command(
    OUTPUT ${ctest_file}
    COMMAND ${CMAKE_COMMAND}
    ARGS
    -D "TARGET=${TARGET}"  # name of target
    -D "TEST_TARGET=${BC}"  # full path of linked bitcode IR file
    -D "VERIFY_CMD=${VERIFY_CMD}"  # verify cmd to use
    -D "SYS_LLVM_NM=${LLVM_NM}"
    -D "SYS_AWK=${AWK}"
    -D "SYS_CPP_FILT=${CPP_FILT}"
    -P ${EXTRACT_TEST_CMD}  # extract tests script
    DEPENDS ${BC}
    COMMENT "Writing discovered tests to ${ctest_file}"
    VERBATIM
    )

  # Add discovered tests to directory TEST_INCLUDE_FILES
  set_property(DIRECTORY
    APPEND PROPERTY TEST_INCLUDE_FILES "${ctest_file}"
  )
  # Test discovery should run only after the linking of the final IR. For this,
  # we need to create a custom target (discovery) that depends on the custom cmd
  # (discovery) output and then declare that the custom target (discovery)
  # depends on the custom cmd (linking).
  add_custom_target("${TARGET}_disc_tests" ALL  # ALL adds it to default target list in Ninja
    DEPENDS ${ctest_file})
  add_dependencies("${TARGET}_disc_tests" ${target_ir})
endfunction()

# Set upper bound on data structures
set(MSG_BUF_MAX_SIZE 4096)

# Compile-time definitions
add_definitions(
  -DMSG_BUF_MAX_SIZE=${MSG_BUF_MAX_SIZE}
)

configure_file(sea.yaml sea.yaml @ONLY)
configure_file(sea_cex_base.yaml sea.cex.yaml @ONLY)
configure_file(sea_vac.yaml sea.vac.yaml @ONLY)

add_subdirectory(ipc)
add_subdirectory(discover_tests)
