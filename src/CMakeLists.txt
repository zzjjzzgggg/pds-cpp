### example of building library
# add_library(walk_engine walk_engine.cpp)
# target_link_libraries(walk_engine ${LIBRARIES})

### examples of using private variable
# add_executable(exam_sieve_adn_dg exam_sieve_adn.cpp)
# target_link_libraries(exam_sieve_adn_dg dyn_dgraph_mgr)
# target_compile_definitions(exam_sieve_adn_dg PRIVATE DGRAPH=1)


# library
add_library(mc_sieve_streaming mc_sieve_streaming.cpp)
target_link_libraries(mc_sieve_streaming ${LIBRARIES})

add_library(hist_streaming hist_streaming.cpp)
target_link_libraries(hist_streaming mc_sieve_streaming ${LIBRARIES})

# execute
add_executable(test-me test-me.cpp)
target_link_libraries(test-me ${LIBRARIES})

add_executable(exam_obj exam_obj.cpp)
target_link_libraries(exam_obj ${LIBRARIES})

add_executable(exam_stream exam_stream.cpp)
target_link_libraries(exam_stream ${LIBRARIES})

add_executable(exam_mc_sieve_streaming exam_mc_sieve_streaming.cpp)
target_link_libraries(exam_mc_sieve_streaming mc_sieve_streaming ${LIBRARIES})

add_executable(exam_greedy_insertion_only exam_greedy_insertion_only.cpp)
target_link_libraries(exam_greedy_insertion_only ${LIBRARIES})

add_executable(exam_greedy_mcss exam_greedy_mcss.cpp)
target_link_libraries(exam_greedy_mcss mc_sieve_streaming ${LIBRARIES})

add_executable(exam_mcssopd exam_mcssopd.cpp)
target_link_libraries(exam_mcssopd mc_sieve_streaming ${LIBRARIES})

add_executable(exam_greedy_pds exam_greedy_pds.cpp)
target_link_libraries(exam_greedy_pds ${LIBRARIES})

add_executable(exam_greedy_mcssopd exam_greedy_mcssopd.cpp)
target_link_libraries(exam_greedy_mcssopd mc_sieve_streaming ${LIBRARIES})

add_executable(exam_hist_streaming exam_hist_streaming.cpp)
target_link_libraries(exam_hist_streaming hist_streaming ${LIBRARIES})
