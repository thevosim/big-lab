add_subdirectory(hash_tables)
add_subdirectory(tables)
add_subdirectory(trees)

add_library(poly_src INTERFACE)

target_link_libraries(poly_src INTERFACE
        hash_tables
        tables
        trees
)