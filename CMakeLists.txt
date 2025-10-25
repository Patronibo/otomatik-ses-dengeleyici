cmake_minimum_required(VERSION 3.15)
project(SesDengeleyici VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Release build optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Strip debug symbols
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /DNDEBUG")
    # Function-level linking for smaller binary
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")
endif()

# Windows özel ayarlar
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

# Kaynak dosyalar
set(SOURCES
    src/main.cpp
    src/AudioCapture.cpp
    src/AudioCompressor.cpp
    src/TrayIcon.cpp
)

set(HEADERS
    include/AudioCapture.h
    include/AudioCompressor.h
    include/TrayIcon.h
    include/resource.h
)

# İkon ve resource dosyaları (opsiyonel)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/resources/app.rc)
    set(RESOURCES resources/app.rc)
endif()

# Executable oluştur
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${RESOURCES})

# Include dizinleri
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Windows kütüphanelerini link et
if(WIN32)
    target_link_libraries(${PROJECT_NAME} 
        winmm
        ole32
        shell32
        user32
        advapi32
        comctl32
    )
endif()

# Çıktı dizini ayarla
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

