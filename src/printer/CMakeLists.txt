add_library (printer STATIC
			 "printer.cpp"
			 "printer.h")

target_link_libraries (printer PRIVATE SDL2)