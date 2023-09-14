cmake_minimum_required(VERSION 3.12)

project(RTSP_Camera_Viewer)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Create a list of your source files
set(SRC_FILES
    rtsp_viewer.cpp
    rtsp_buffer_viewer.cpp
    # Add other .cpp files here
)

# Create an executable for each source file
foreach(SRC_FILE ${SRC_FILES})
    # Extract the filename without extension
    get_filename_component(EXEC_NAME ${SRC_FILE} NAME_WE)

    # Add the executable
    add_executable(${EXEC_NAME} ${SRC_FILE})

    # Link the OpenCV libraries to the executable
    target_link_libraries(${EXEC_NAME} ${OpenCV_LIBS})
endforeach()

