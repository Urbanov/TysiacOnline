set(network_sources 
	manager.cpp
	server.cpp
	session.cpp
)

add_library(network ${network_sources})
target_link_libraries(network engine)