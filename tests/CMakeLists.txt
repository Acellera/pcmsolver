add_subdirectory(surface_function)
add_subdirectory(gepol)
add_subdirectory(iefpcm)
add_subdirectory(cpcm)
if(DEVELOPMENT_CODE)
	add_subdirectory(tsless)
	add_subdirectory(wavelet_cavity)
	add_subdirectory(pwc)
	add_subdirectory(pwl)
endif()