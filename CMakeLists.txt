#  CMAKE Main File for A PET Image Reconstruction library (APIRL)
#  Copyright (c) 2009

PROJECT(APIRL)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Type of build (Debug or Release). CMAKE_BUILD_TYPE is a cache variable, therefore is better to set it in configuration time.
#set(CMAKE_BUILD_TYPE	Debug)

# If debug, define a preprocessor variable to be able to use it for conditional compiling.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-D__DEBUG__)
endif()

# Disable secure Warnings for stdc functions.
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

###################### OPENMP CONFIGURATION #############################
INCLUDE(FindOpenMP)
IF(OPENMP_FOUND)
    OPTION(USE_OPENMP    "Manages the frame grabbing on windows" ON)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
	message(STATUS "Flags: ${OpenMP_C_FLAGS} ${OpenMP_CXX_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
     SET(ENV{OMP_STACKSIZE} 512M)
ELSE(OPENMP_FOUND)
    SET(USE_OPENMP OFF)
ENDIF(OPENMP_FOUND)
#######################################################################

################## CHANG STACK SIZE TO AVOID PROBLEM WITH OPENMP #########
SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --main-stacksize=8500000")
#########################################################

# The install path depends on the building type, for debug builds it is installed in a debug folder
# located in the build path. For the release build, the defatult install path for the OS is used.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	SET(APIRL_INSTALL_PATH "${CMAKE_BINARY_DIR}/debug")
else()
	SET(APIRL_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}/APIRL")
endif()	
MESSAGE(STATUS "INSTALL PATH: ${APIRL_INSTALL_PATH}")

################### BUILD LIBRARIES FOR GPU USE ########################
# Only for Cuda compatible devices:
################## CHECK IF CUDA AVAILABLE ############################
find_package(CUDA)
if (CUDA_FOUND)
	SET(ENABLE_GPU ON)
	add_definitions(-D__USE_CUDA__)
	# To see all the outputs from nvcc
	SET(CUDA_VERBOSE_BUILD ON)
	SET(CUDA_BUILD_CUBIN ON)
	# Build dynamic libraries:
	SET(CUDA_LIBRARIES_BUILD	SHARED)
	# Set minimum cuda compute capabilities for the compiler:
	SET(CUDA_COMPUTE_CAPABILITY 3.5)
	# Additional Cuda flags that can have impact on performance:
	#SET(CUDA_USE_FAST_MATH ON)
	SET(CUDA_USE_CACHE_ONLY_L2 ON)
	#SET(CUDA_MAX_REG_COUNT 32)
	#set(CUDA_NVCC_FLAGS "${CUDA_NVCC_FLAGS} -rdc=true -lineinfo")
	MESSAGE(STATUS "NVCC FLAGS: ${CUDA_NVCC_FLAGS}")
else (CUDA_FOUND)
	SET(ENABLE_GPU OFF)	
endif (CUDA_FOUND)
#######################################################################


##################### ADD SUBDIRECTORIES TO THE CONFIGURATION #################################
# Variables with the headers directory for each subfolder.
SET(cmd_Headers_Dir ${CMAKE_SOURCE_DIR}/source/cmd/inc)
SET(recon_Headers_Dir ${CMAKE_SOURCE_DIR}/source/recon/inc)
SET(utils_Headers_Dir ${CMAKE_SOURCE_DIR}/source/utils/inc)
SET(data_Headers_Dir ${CMAKE_SOURCE_DIR}/source/data/inc)
SET(reconGPU_Headers_Dir ${CMAKE_SOURCE_DIR}/source/reconGPU/inc)
add_subdirectory(source/recon)
add_subdirectory(source/data)
add_subdirectory(source/utils)
add_subdirectory(source/cmd)
if(ENABLE_GPU)
	add_subdirectory(source/reconGPU)
	add_subdirectory(source/cmdGPU)
endif(ENABLE_GPU)
################################################################################################

# USED TO HAD PROBLEM TO COMPILE DEVICE CODE IN 64 BIT ON WINDOWS:
if(CMAKE_HOST_WIN32 OR CMAKE_HOST_WIN64)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	  set(CUDA_64_BIT_DEVICE_CODE OFF)
	else(CMAKE_SIZEOF_VOID_P EQUAL 4)
	  set(CUDA_64_BIT_DEVICE_CODE OFF)
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
else()
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	  set(CUDA_64_BIT_DEVICE_CODE ON)
	else(CMAKE_SIZEOF_VOID_P EQUAL 4)
	  set(CUDA_64_BIT_DEVICE_CODE OFF)
	endif(CMAKE_SIZEOF_VOID_P EQUAL 8)
endif()


# Check if Doxygen is available
find_package(Doxygen)
if (DOXYGEN_FOUND)
	OPTION(INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF)
	OPTION(USE_DOT "Set to ON to perform diagram generation with graphviz" OFF)
	OPTION(USE_LATEX "Set to ON to build latex documentation" OFF)
	OPTION(USE_CHM "Set to ON to build CHM Windows documentation" OFF)
	SET(INSTALL_DOC ON)
	IF (INSTALL_DOC)
		# Create doc directory:
		file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/doc")
		#install(DIRECTORY DESTINATION "${CMAKE_BINARY_DIR}/doc")
		INCLUDE("${CMAKE_SOURCE_DIR}/source/doxygen/generateDOC.cmake" )
		GENERATE_DOCUMENTATION(${CMAKE_SOURCE_DIR}/source/${PROJECT_NAME}.dox.in)
	ENDIF()
else (DOXYGEN_FOUND)
	MESSAGE(WARNING "DOXYGEN WAS NOT FOUND, THE SOURCES DOCUMENTATION WILL NOT BE GENERATED.")
endif (DOXYGEN_FOUND)

# INSTALL SAMPLE FILES
FILE(GLOB Sample_Files ABSOLUTE ${CMAKE_SOURCE_DIR}/samples/*.txt | ${CMAKE_SOURCE_DIR}/samples/*.par)

Install(FILES ${Sample_Files} DESTINATION "${APIRL_INSTALL_PATH}/Samples")




