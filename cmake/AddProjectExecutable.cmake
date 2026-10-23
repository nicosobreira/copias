function(add_project_executable name)
  file(GLOB_RECURSE ${name}_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
  )

  add_executable(${name} ${${name}_SOURCES})

  target_include_directories(${name}
    PUBLIC
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
      $<INSTALL_INTERFACE:include>
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

  target_link_libraries(${name} INTERFACE project_flags)
endfunction()
