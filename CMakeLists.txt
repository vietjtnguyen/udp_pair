cmake_minimum_required(VERSION 2.8)

set(LIB_INSTALL_DIR lib CACHE STRING "(default: lib)")
add_library(udp_conn SHARED udp_conn.c)
install(TARGETS udp_conn LIBRARY DESTINATION ${LIB_INSTALL_DIR})
install(FILES udp_conn.h DESTINATION include)

add_executable(udpcat udpcat.c)
target_link_libraries(udpcat udp_conn)
set_target_properties(udpcat PROPERTIES LINK_FLAGS "-pthread")
install(TARGETS udpcat RUNTIME DESTINATION bin)

add_executable(example example.c)
target_link_libraries(example udp_conn)
set_target_properties(example PROPERTIES LINK_FLAGS "-pthread")
