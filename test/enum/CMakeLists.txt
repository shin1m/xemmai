add_library(enum MODULE enum.cc)
target_link_libraries(enum xemmai)
set_property(TARGET enum PROPERTY PREFIX "")
