add_library(container MODULE queue.cc container.cc)
target_link_libraries(container xemmai)
set_property(TARGET container PROPERTY PREFIX "")
