#
#
#
add_executable(testExe
  test_main.cpp
  test.cpp)

target_compile_features(testExe PRIVATE cxx_std_11)

set_target_properties(testExe
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY .. # output exe in project root directory
  )

target_link_libraries(testExe PRIVATE exeLib)

add_custom_target(test-no-color
  COMMAND testExe --use-colour=no
  )

add_custom_target(test
  COMMAND testExe
  )

add_dependencies(test-no-color testExe)
add_dependencies(test testExe)
