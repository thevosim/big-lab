add_library(tables INTERFACE)
target_include_directories(tables INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(tables INTERFACE poly_include)