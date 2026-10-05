# Collect source files
file(GLOB_RECURSE SOURCES 
    "*.cpp"
    "gui/*.cpp"
    "core/*.cpp"
    "core/steg/*.cpp"
    "core/extract/*.cpp"
    "utils/*.cpp"
    "refer/*.cpp"
    "../tests/*.cpp"
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Include directories
target_include_directories(${PROJECT_NAME} 
    PRIVATE 
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/tests
)

# Add compiler and linker flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/MT>
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/DNDEBUG>
    )
endif()

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        fltk
        fltk_images
        JPEG::JPEG
)

# Set Visual Studio specific properties
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif()

    
