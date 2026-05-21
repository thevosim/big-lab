add_library(trees INTERFACE)
target_include_directories(trees INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(trees INTERFACE poly_include)