add_library(ExPackage fun1.cpp fun2.cpp)

target_include_directories(ExPackage
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
	)


