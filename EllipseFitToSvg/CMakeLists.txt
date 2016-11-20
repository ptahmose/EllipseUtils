
use_cxx11() 

add_executable(EllipseFitToSvg EllipseFitToSvg.cpp options.cpp ReadPoints.cpp stdafx.cpp WriteSvg.cpp Utilities.cpp optionparser.h options.h ReadPoints.h stdafx.h targetver.h WriteSvg.h Utilities.h)

set_target_properties(EllipseFitToSvg 
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Build/CMake/")

