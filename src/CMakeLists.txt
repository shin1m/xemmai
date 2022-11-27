add_executable(xemmai
	portable.cc
	type.cc
	sharable.cc
	class.cc
	module.cc
	symbol.cc
	native.cc
	fiber.cc
	thread.cc
	ast.cc
	code.cc
	lambda.cc
	throwable.cc
	null.cc
	boolean.cc
	integer.cc
	float.cc
	string.cc
	tuple.cc
	list.cc
	map.cc
	bytes.cc
	lexer.cc
	parser.cc
	global.cc
	io.cc
	io__file.cc
	io__reader.cc
	io__writer.cc
	engine.cc
	main.cc
	)
target_compile_features(xemmai PUBLIC cxx_std_17)
target_include_directories(xemmai PUBLIC
	"$<BUILD_INTERFACE:${xemmai_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:include/xemmai-${xemmai_VERSION}>"
	)
target_compile_definitions(xemmai PRIVATE
	"XEMMAI_MODULE_PATH=../lib/xemmai-${xemmai_VERSION}"
	"$<$<PLATFORM_ID:Windows>:XEMMAI__PORTABLE__EXPORT=__declspec(dllexport)>"
	PUBLIC "$<$<CXX_COMPILER_ID:MSVC>:_ITERATOR_DEBUG_LEVEL=0>"
	)
target_link_libraries(xemmai PRIVATE $<$<NOT:$<PLATFORM_ID:Windows>>:dl> Iconv::Iconv Threads::Threads)
target_link_options(xemmai PRIVATE
	$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Debug>>:/STACK:0x01000000>
	$<$<AND:$<CXX_COMPILER_ID:MSVC>,$<CONFIG:Release>>:/STACK:0x800000>
	)
set_property(TARGET xemmai PROPERTY ENABLE_EXPORTS 1)
