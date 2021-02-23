add_executable(xemmai
	portable.cc
	type.cc
	object.cc
	class.cc
	structure.cc
	module.cc
	fiber.cc
	thread.cc
	tuple.cc
	symbol.cc
	ast.cc
	code.cc
	lambda.cc
	native.cc
	throwable.cc
	null.cc
	boolean.cc
	integer.cc
	float.cc
	string.cc
	array.cc
	dictionary.cc
	bytes.cc
	lexer.cc
	parser.cc
	global.cc
	io.cc
	io__file.cc
	io__reader.cc
	io__writer.cc
	io__path.cc
	engine.cc
	main.cc
	)
target_compile_features(xemmai PUBLIC cxx_std_17)
target_include_directories(xemmai PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_compile_definitions(xemmai PRIVATE "XEMMAI_MODULE_PATH=../lib/xemmai-${xemmai_VERSION}")
target_link_libraries(xemmai PRIVATE dl pthread)
set_property(TARGET xemmai PROPERTY ENABLE_EXPORTS 1)
