# Windows
if(WIN32)
    ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)

    MESSAGE(STATUS "[GooseUI] -- OS: Windows")
    MESSAGE(STATUS "[GooseUI] -- Display Service: Win32")

    LIST(APPEND GOOSEUI_LIBRARY "dwrite")
    LIST(
        APPEND GOOSEUI_SRC
        #"src/platform/win32_window.cpp"
        #"src/platform/win32_font.cpp"
    )
endif()

# MacOS
if(APPLE)
    MESSAGE(STATUS "[GooseUI] -- OS: MacOS")
    MESSAGE(FATAL_ERROR "[GooseUI] -- honk (Unsupported)")
endif()

# Unix Or Other
if(UNIX AND NOT APPLE)
    MESSAGE(STATUS "[GooseUI] -- OS: UNIX or Other")

    # Freetype
    FIND_PACKAGE(Freetype REQUIRED)
    LIST(APPEND GOOSEUI_LIBRARY Freetype::Freetype)
    LIST(
        APPEND GOOSEUI_SRC
        "src/platform/fonts/freetype_font.cpp"
    )

    # EGL
    FIND_PACKAGE(OpenGL REQUIRED COMPONENTS EGL)
    LIST(APPEND GOOSEUI_LIBRARY "OpenGL::EGL")

    if(GOOSEUI_XORG_SUPPORT)
        MESSAGE(STATUS "[GooseUI] -- Display Service: X11")

        FIND_PACKAGE(X11 REQUIRED)

        LIST(APPEND GOOSEUI_INCLUDE "${X11_INCLUDE_DIR}")
        LIST(APPEND GOOSEUI_LIBRARY "${X11_LIBRARIES}")
        LIST(APPEND GOOSEUI_BUILD_FLAGS "X11")
        LIST(
            APPEND GOOSEUI_SRC
            "src/platform/x11_window.cpp"
            "src/platform/x11_static.cpp"
        )
        
        add_compile_definitions(GOOSEUI_XORG_SUPPORT)
    endif()

    if(GOOSEUI_WAYLAND_SUPPORT)
        MESSAGE(STATUS "[GooseUI] -- Display Service: Wayland")

        find_path(WAYLAND_CLIENT_DIR NAMES wayland-client.h PATH_SUFFIXES wayland)
        find_path(WAYLAND_EGL_DIR NAMES wayland-egl.h PATH_SUFFIXES wayland)
        
        find_library(WAYLAND_CLIENT_LIB NAMES wayland-client)
        find_library(WAYLAND_EGL_LIB NAMES wayland-egl)

        if(NOT WAYLAND_CLIENT_DIR AND NOT WAYLAND_CLIENT_LIB)
          MESSAGE(FATAL_ERROR "[GooseUI] -- Faild to find wayland-client")
        endif()

        if(NOT WAYLAND_EGL_DIR AND NOT WAYLAND_EGL_LIB)
          MESSAGE(FATAL_ERROR "[GooseUI] -- Faild to find wayland-egl")
        endif()

        LIST(APPEND GOOSEUI_INCLUDE ${WAYLAND_CLIENT_DIR} ${WAYLAND_EGL_DIR})
        LIST(APPEND GOOSEUI_LIBRARY ${WAYLAND_CLIENT_LIB} ${WAYLAND_EGL_LIB})
        LIST(APPEND GOOSEUI_BUILD_FLAGS "wayland-client")
        LIST(
            APPEND GOOSEUI_SRC
            #"src/platform/wl_window.cpp"
            
            #"src/modules/wayland-protocals/xdg-shell.c"
        )
        
        add_compile_definitions(GOOSEUI_WAYLAND_SUPPORT)
    endif()
    
endif()