#
#
#

add_executable(main main.cpp)

set_target_properties(main
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY .. # output exe in project root directory
  )

target_compile_features(main PRIVATE cxx_std_11)

target_link_libraries(main PRIVATE exeLib)

#add_custom_target(main
#  COMMAND main # change to actual program name
#  )
#
#add_dependencies(main main) # Change 2nd main is actual program name
