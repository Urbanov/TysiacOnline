set(tests_sources
	test_main.cpp
	engine_tests.cpp
	server_tests.cpp
)


set(TEST_PROJECT_NAME "${PROJECT_NAME}_test")
add_executable(${TEST_PROJECT_NAME} EXCLUDE_FROM_ALL ${tests_sources})
target_link_libraries(${TEST_PROJECT_NAME} ${Boost_LIBRARIES} engine network)