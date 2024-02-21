cmake_minimum_required(VERSION 3.10)
project(my_project)
set(CMAKE_CXX_STANDARD 17)



include(CheckCXXCompilerFlag)

if((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))

   # add_compile_options(
   #     -Werror
   #     -Wall
   #     -Wextra
   #     -Wpedantic
   #     -Wcast-align
   #     -Wcast-qual
   #     -Wconversion
   #     -Wctor-dtor-privacy
   #     -Wenum-compare
   #     -Wfloat-equal
   #     -Wnon-virtual-dtor
  #      -Wold-style-cast
  #      -Woverloaded-virtual
  #      -Wredundant-decls
  #      -Wsign-conversion
  #      -Wsign-promo
  #  )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    check_cxx_compiler_flag(-std=c++17 SUPPORTS_CXX17)
    
    if(SUPPORTS_CXX17)
        add_compile_options(-std=c++17)
    else()
        message(FATAL_ERROR "Compiler doesn't support C++17")
    endif()
endif()

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()


# Укажите исходные файлы
set(SOURCES src/main.cpp src/dbscan.cpp)

# Укажите заголовочные файлы
set(HEADERS include/dbscan.h)

# Укажите пути к директориям с данными
set(DATA_DIR ${CMAKE_SOURCE_DIR}/data)
set(FONT_DIR ${CMAKE_SOURCE_DIR}/font)

# Найдите библиотеку SFML
find_package(SFML 2.5 COMPONENTS system window graphics REQUIRED)

# Создайте исполняемый файл
add_executable(sfml_radar_visualizer ${SOURCES} ${HEADERS})

# Добавьте директории с данными и заголовочными файлами в цель сборки
target_include_directories(sfml_radar_visualizer PRIVATE ${DATA_DIR} ${FONT_DIR} include)

# Скопируйте данные в директорию сборки
file(COPY ${DATA_DIR} DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${FONT_DIR} DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(sfml_radar_visualizer sfml-graphics sfml-window sfml-system)

