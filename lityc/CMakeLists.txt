set(
	sources
	CommandLineInterface.cpp CommandLineInterface.h
	main.cpp
)

add_executable(lityc ${sources})
target_link_libraries(lityc PRIVATE lity ${Boost_PROGRAM_OPTIONS_LIBRARIES})

include(GNUInstallDirs)
install(TARGETS lityc DESTINATION "${CMAKE_INSTALL_BINDIR}")

if(LITYC_LINK_STATIC AND UNIX AND NOT APPLE)
	# Produce lityc as statically linked binary (includes C/C++ standard libraries)
	# This is not supported on macOS, see
	# https://developer.apple.com/library/content/qa/qa1118/_index.html.
	set_target_properties(
		lityc PROPERTIES
		LINK_FLAGS -static
		LINK_SEARCH_START_STATIC ON
		LINK_SEARCH_END_STATIC ON
	)
endif()
