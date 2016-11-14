cmake_minimum_required (VERSION 3.6)
project("CellParse")
find_package(Threads)
find_package(PkgConfig)
find_package(PostgreSQL)
find_package(Boost 1.60 REQUIRED COMPONENTS system thread)
pkg_check_modules(GTKMM gtkmm-3.0)
include_directories(
	${GTKMM_INCLUDE_DIRS}
	${PostgreSQL_INCLUDE_DIRS}
	${Boost_INCLUDE_DIRS}
	)
link_directories(
	${GTKMM_LIBRARY_DIRS}
	${PostgreSQL_LIBRARY_DIRS}
	${Boost_LIBRARY_DIRS}
	)
add_executable(${PROJECT_NAME}
		main.cpp
		janela_principal.cpp
		estruturas.cpp
		loader_thread.cpp
		banco.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC
		cxx_nullptr
		cxx_lambdas)
target_compile_options(${PROJECT_NAME}
	PUBLIC
	"-ggdb")
target_link_libraries(${PROJECT_NAME} 
	${GTKMM_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT}
	${PostgreSQL_LIBRARIES}
	${Boost_LIBRARIES}
	)
