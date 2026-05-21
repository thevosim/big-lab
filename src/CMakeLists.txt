add_subdirectory(hash_tables)
add_subdirectory(tables)
add_subdirectory(trees)

add_library(containers INTERFACE)

target_link_libraries(containers INTERFACE
        hash_tables
        tables
        trees
)