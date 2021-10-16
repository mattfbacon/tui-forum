file(GLOB_RECURSE taopq_sources ${CMAKE_CURRENT_LIST_DIR}/taopq/src/lib/pq/*.cpp)

add_library(taopq STATIC ${taopq_sources})
target_include_directories(taopq PRIVATE ${CMAKE_CURRENT_LIST_DIR}/taopq/include)
target_include_directories(taopq SYSTEM INTERFACE ${CMAKE_CURRENT_LIST_DIR}/taopq/include)
target_link_libraries(taopq PUBLIC pq)
