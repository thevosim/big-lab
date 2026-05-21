add_library(hash_tables INTERFACE)
target_include_directories(hash_tables INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
target_link_libraries(hash_tables INTERFACE poly_include)