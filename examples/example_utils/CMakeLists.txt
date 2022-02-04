add_library(example_utils INTERFACE)

target_sources(example_utils PRIVATE 
               example_utils.h)

target_include_directories(example_utils PUBLIC INTERFACE ./)