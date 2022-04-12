macro(SetupPython)
# -------------------------------- Python --------------------------------

    if(${CMAKE_VERSION} VERSION_LESS "3.12")
        set(Python_ADDITIONAL_VERSIONS ${Python_ADDITIONAL_VERSIONS} "3.10" "3.9" "3.8" "3.7" "3.6")
        find_package(PythonInterp REQUIRED)
        set(Python_ADDITIONAL_VERSIONS ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR})
        if (NOT DEFINED PYTHON_VERSION_STRING)
            find_package(PythonLibs REQUIRED)
        else (NOT DEFINED PYTHON_VERSION_STRING)
            find_package(PythonLibs ${PYTHON_VERSION_STRING} EXACT)
        endif(NOT DEFINED PYTHON_VERSION_STRING)
        if(NOT PYTHONLIBS_FOUND)
            message(FATAL_ERROR "=================================\n"
                                "Python not found, install Python!\n"
                                "=================================\n")
        endif()
        
        # For forward compatibility with new CMake scripts
        set(Python3_EXECUTABLE ${PYTHON_EXECUTABLE})
        set(Python3_LIBRARIES ${PYTHON_LIBRARIES})
        set(Python3_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS})
        set(Python3_LIBRARY_DIRS ${PYTHON_LIBRARY_DIRS})
        set(Python3_VERSION ${PYTHON_VERSION_STRING})
        set(Python3_VERSION_MAJOR ${PYTHON_VERSION_MAJOR})
        set(Python3_VERSION_MINOR ${PYTHON_VERSION_MINOR})
        set(Python3_VERSION_PATCH ${PYTHON_VERSION_PATCH})
        set(Python3_Interpreter_FOUND ${PYTHONINTERP_FOUND})
    else()
        find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

        # For backwards compatibility with old CMake scripts
        set(PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
        set(PYTHON_LIBRARIES ${Python3_LIBRARIES})
        message(status " PYTHON_LIBRARIES : ${PYTHON_LIBRARIES}")
        set(PYTHON_INCLUDE_DIRS ${Python3_INCLUDE_DIRS})
        set(PYTHON_LIBRARY_DIRS ${Python3_LIBRARY_DIRS})
        message(status " Python3_LIBRARY_DIRS : ${Python3_LIBRARY_DIRS}")
        set(PYTHON_VERSION_STRING ${Python3_VERSION})
        set(PYTHON_VERSION_MAJOR ${Python3_VERSION_MAJOR})
        set(PYTHON_VERSION_MINOR ${Python3_VERSION_MINOR})
        set(PYTHON_VERSION_PATCH ${Python3_VERSION_PATCH})
        set(PYTHONINTERP_FOUND ${Python3_Interpreter_FOUND})
    endif()
    
    # prevent python3 lower than 3.6
    if (${PYTHON_VERSION_STRING} VERSION_LESS "3.6")
         message(FATAL_ERROR "To build FreeCAD you need at least Python 3.6\n")
    endif()

endmacro(SetupPython)
