### example of building library
# add_library(walk_engine walk_engine.cpp)
# target_link_libraries(walk_engine ${LIBRARIES})

### examples of using private variable
# add_executable(exam_sieve_adn_dg exam_sieve_adn.cpp)
# target_link_libraries(exam_sieve_adn_dg dyn_dgraph_mgr)
# target_compile_definitions(exam_sieve_adn_dg PRIVATE DGRAPH=1)


####################################################################################
##### library
####################################################################################
add_library(mc_sieve_streaming mc_sieve_streaming.cpp)
target_link_libraries(mc_sieve_streaming ${LIBRARIES})

add_library(hist_approx hist_approx.cpp)
target_link_libraries(hist_approx mc_sieve_streaming ${LIBRARIES})

####################################################################################
##### execute
####################################################################################
set(EXE test-me)
add_executable(${EXE} ${EXE}.cpp)
target_link_libraries(${EXE} ${LIBRARIES})

# set(EXE exam_obj)
# add_executable(${EXE} ${EXE}.cpp)
# target_link_libraries(${EXE} ${LIBRARIES})

set(EXE convert_to_bin)
add_executable(${EXE} ${EXE}.cpp)
target_link_libraries(${EXE} ${LIBRARIES})

set(EXE exam_stream)
add_executable(${EXE} ${EXE}.cpp)
target_link_libraries(${EXE} ${LIBRARIES})

# set(EXE exam_mc_sieve_streaming)
# add_executable(${EXE} ${EXE}.cpp)
# target_link_libraries(${EXE} mc_sieve_streaming ${LIBRARIES})

# set(EXE exam_greedy_insertion_only)
# add_executable(${EXE} ${EXE}.cpp)
# target_link_libraries(${EXE} ${LIBRARIES})

set(EXE exam_greedy_and_mc_sieve_streaming)
add_executable(${EXE} ${EXE}.cpp)
target_link_libraries(${EXE} mc_sieve_streaming ${LIBRARIES})

set(EXE exam_mcssopd)
add_executable(${EXE} ${EXE}.cpp)
target_link_libraries(${EXE} mc_sieve_streaming ${LIBRARIES})

set(EXE exam_greedy_pds)
add_executable(${EXE} ${EXE}.cpp)
target_link_libraries(${EXE} ${LIBRARIES})

# set(EXE exam_greedy_and_mcssopd)
# add_executable(${EXE} ${EXE}.cpp)
# target_link_libraries(${EXE} mc_sieve_streaming ${LIBRARIES})

set(EXE exam_hist_approx)
add_executable(${EXE} ${EXE}.cpp)
target_link_libraries(${EXE} hist_approx ${LIBRARIES})

set(EXE exam_random)
add_executable(${EXE} ${EXE}.cpp)
target_link_libraries(${EXE} ${LIBRARIES})

# set(EXE exam_reservoir_sampling)
# add_executable(${EXE} ${EXE}.cpp)
# target_link_libraries(${EXE} ${LIBRARIES})
