cmake_minimum_required(VERSION 2.8)

find_package(Threads)

set(LIB_INSTALL_DIR lib CACHE STRING "(default: lib)")
add_library(udp_pair SHARED udp_pair.c)
set_target_properties(
  udp_pair
  PROPERTIES
    VERSION 0.1.0
    SOVERSION 0.1)
install(TARGETS udp_pair LIBRARY DESTINATION ${LIB_INSTALL_DIR})
install(FILES udp_pair.h DESTINATION include)

add_executable(udpcat bin/udpcat.c)
target_link_libraries(udpcat udp_pair ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(
  udpcat
  PROPERTIES
    INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}")
install(TARGETS udpcat RUNTIME DESTINATION bin)

add_executable(spam_hello_world example/spam_hello_world.c)
target_link_libraries(spam_hello_world udp_pair ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(
  spam_hello_world
  PROPERTIES
    INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}")

add_executable(echo_server example/echo_server.c)
target_link_libraries(echo_server udp_pair ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(
  echo_server
  PROPERTIES
    INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}")
