### example of building library
# add_library(walk_engine walk_engine.cpp)
# target_link_libraries(walk_engine ${LIBRARIES})

### examples of using private variable
# add_executable(exam_sieve_adn_dg exam_sieve_adn.cpp)
# target_link_libraries(exam_sieve_adn_dg dyn_dgraph_mgr)
# target_compile_definitions(exam_sieve_adn_dg PRIVATE DGRAPH=1)



# test
add_executable(test-me test-me.cpp)
target_link_libraries(test-me ${LIBRARIES})
