cmake_minimum_required(VERSION 3.20.0)
project(custom-containers VERSION 1.0.0 DESCRIPTION "A collection of data structures for c++")

set (HEADERS
    include/ccnt/bitmask.h
    include/ccnt/circular_array.h
    include/ccnt/doubly_linked_list.h
    include/ccnt/hash_map.h
    include/ccnt/vector.h
)

include(GNUInstallDirs)
add_library(${PROJECT_NAME} INTERFACE)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "${HEADERS}"
)
configure_file(custom-containers.pc.in custom-containers.pc @ONLY)
target_include_directories(${PROJECT_NAME} INTERFACE include)
install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/custom-containers.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
