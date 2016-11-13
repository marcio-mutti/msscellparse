cmake_minimum_required (VERSION 3.6)
project("CellParse")
find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)
link_directories(
	    ${GTKMM_LIBRARY_DIRS}  )
include_directories(
	        ${GTKMM_INCLUDE_DIRS}  )
add_executable(${PROJECT_NAME}
		main.cpp
		janela_principal.cpp)




target_compile_features(${PROJECT_NAME} PUBLIC
		cxx_nullptr
		cxx_lambdas)
target_link_libraries(${PROJECT_NAME} 
		    ${GTKMM_LIBRARIES}  )
