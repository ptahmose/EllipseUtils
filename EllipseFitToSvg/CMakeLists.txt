


add_executable(EllipseFitToSvg EllipseFitToSvg.cpp options.cpp ReadPoints.cpp stdafx.cpp WriteSvg.cpp Utilities.cpp optionparser.h options.h ReadPoints.h stdafx.h targetver.h WriteSvg.h Utilities.h)

set_property(TARGET EllipseFitToSvg PROPERTY CXX_STANDARD 11)
