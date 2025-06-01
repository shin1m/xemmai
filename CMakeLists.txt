cmake_minimum_required(VERSION 3.12)
project(xemmai VERSION 202506)
find_package(Iconv REQUIRED)
find_package(Threads REQUIRED)
set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib")
add_subdirectory(src)
add_subdirectory(modules)
enable_testing()
add_subdirectory(test)
install(TARGETS xemmai EXPORT xemmai-targets DESTINATION bin)
install(TARGETS math os threading time DESTINATION lib/xemmai-${xemmai_VERSION})
install(DIRECTORY include/xemmai DESTINATION include/xemmai-${xemmai_VERSION})
install(EXPORT xemmai-targets FILE xemmai-targets.cmake DESTINATION lib/cmake/xemmai-${xemmai_VERSION})
include(CMakePackageConfigHelpers)
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/xemmai-config.cmake"
	INSTALL_DESTINATION "lib/cmake/xemmai-${xemmai_VERSION}"
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
	)
write_basic_package_version_file(
	"${CMAKE_CURRENT_BINARY_DIR}/xemmai-config-version.cmake"
	VERSION "${xemmai_VERSION_MAJOR}.${xemmai_VERSION_MINOR}"
	COMPATIBILITY AnyNewerVersion
	)
install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/xemmai-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/xemmai-config-version.cmake"
	DESTINATION "lib/cmake/xemmai-${xemmai_VERSION}"
	)
