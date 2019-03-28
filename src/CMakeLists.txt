### example of building library
# add_library(walk_engine walk_engine.cpp)
# target_link_libraries(walk_engine ${LIBRARIES})

### examples of using private variable
# add_executable(exam_sieve_adn_dg exam_sieve_adn.cpp)
# target_link_libraries(exam_sieve_adn_dg dyn_dgraph_mgr)
# target_compile_definitions(exam_sieve_adn_dg PRIVATE DGRAPH=1)


# library
add_library(sieve_streaming sieve_streaming.cpp)
target_link_libraries(sieve_streaming ${LIBRARIES})


# execute
add_executable(test-me test-me.cpp)
target_link_libraries(test-me ${LIBRARIES})

add_executable(exam_obj exam_obj.cpp)
target_link_libraries(exam_obj ${LIBRARIES})

add_executable(exam_stream exam_stream.cpp)
target_link_libraries(exam_stream ${LIBRARIES})

add_executable(exam_sieve_streaming exam_sieve_streaming.cpp)
target_link_libraries(exam_sieve_streaming sieve_streaming ${LIBRARIES})
