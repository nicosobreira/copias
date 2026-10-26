function(add_project_executable name)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "" "SRC_DIR" "")

  if(NOT ARG_SRC_DIR)
    set(ARG_SRC_DIR "src")
  endif()

  # Aceita caminho relativo (à pasta atual) ou absoluto
  get_filename_component(src_dir "${ARG_SRC_DIR}" ABSOLUTE
    BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

  file(GLOB_RECURSE ${name}_SOURCES CONFIGURE_DEPENDS
    ${src_dir}/*.c
  )

  add_executable(${name} ${${name}_SOURCES})

  target_include_directories(${name}
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
    PRIVATE
     ${CMAKE_CURRENT_SOURCE_DIR}/${src_dir}
  )

  target_link_libraries(${name} INTERFACE project_flags)
endfunction()
