find_package(Doxygen)

if(DOXYGEN_FOUND)
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/html/index.html
			${CMAKE_BINARY_DIR}/latex/Makefile
		COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile
     		#DEPENDS  ${headers}
	)
      	add_custom_target(doc
	    	DEPENDS ${CMAKE_BINARY_DIR}/html/index.html
	    	COMMENT "Building doxygen documentation"
	)
      	add_custom_target(html
	    	DEPENDS ${CMAKE_BINARY_DIR}/html/index.html
	    	COMMENT "Building doxygen documentation"
	)
	add_custom_command(
		OUTPUT ${CMAKE_BINARY_DIR}/Mastermind.pdf
		DEPENDS ${CMAKE_BINARY_DIR}/latex/Makefile
		COMMAND make
		COMMAND cmake -E copy refman.pdf ${CMAKE_BINARY_DIR}/Mastermind.pdf
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/latex
     		#DEPENDS  ${headers}
	)
      	add_custom_target(pdf
		DEPENDS ${CMAKE_BINARY_DIR}/Mastermind.pdf
	    	COMMENT "Building doxygen documentation"
	)
      	install(DIRECTORY ${CMAKE_BINARY_DIR}/html
      		DESTINATION share/doc
	)
endif()
