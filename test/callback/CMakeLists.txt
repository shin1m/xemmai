add_library(callback MODULE callback.cc library.cc)
target_link_libraries(callback xemmai)
set_property(TARGET callback PROPERTY PREFIX "")
