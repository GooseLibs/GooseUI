# Windows
if(WIN32)
    ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)

    MESSAGE(STATUS "[GooseUI] -- OS: Windows")
    MESSAGE(STATUS "[GooseUI] -- Display Service: Win32")

    LIST(APPEND GOOSEUI_LIBRARY "dwrite")
    LIST(
        APPEND GOOSEUI_SRC
        "src/platform/win32_window.cpp"
        "src/platform/fonts/win32_font.cpp"
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

        FIND_PACKAGE(X11 REQUIRED COMPONENTS Xext Xcursor)

        LIST(APPEND GOOSEUI_INCLUDE "${X11_INCLUDE_DIR}")
        LIST(APPEND GOOSEUI_LIBRARY "${X11_LIBRARIES}" "${X11_Xcursor_LIB}")
        LIST(APPEND GOOSEUI_BUILD_FLAGS "X11")
        LIST(
            APPEND GOOSEUI_SRC
            "src/platform/x11_window.cpp"
        )
        
        add_compile_definitions(GOOSEUI_XORG_SUPPORT)
    endif()

    if(GOOSEUI_WAYLAND_SUPPORT)
        MESSAGE(STATUS "[GooseUI] -- Display Service: Wayland")

        find_path(WAYLAND_CLIENT_DIR NAMES wayland-client.h PATH_SUFFIXES wayland)
        find_path(WAYLAND_EGL_DIR NAMES wayland-egl.h PATH_SUFFIXES wayland)
        
        find_library(WAYLAND_CLIENT_LIB NAMES wayland-client)
        find_library(WAYLAND_EGL_LIB NAMES wayland-egl)

        find_program(WAYLAND_SCANNER wayland-scanner REQUIRED)
        find_path(WAYLAND_PROTOCOLS_BASE NAMES stable/xdg-shell/xdg-shell.xml 
        PATHS 
            /usr/share/wayland-protocols
            /usr/local/share/wayland-protocols
            /opt/share/wayland-protocols
        )

        if(NOT WAYLAND_CLIENT_DIR AND NOT WAYLAND_CLIENT_LIB)
          MESSAGE(FATAL_ERROR "[GooseUI] -- Faild to find wayland-client")
        endif()

        if(NOT WAYLAND_EGL_DIR AND NOT WAYLAND_EGL_LIB)
          MESSAGE(FATAL_ERROR "[GooseUI] -- Faild to find wayland-egl")
        endif()

        if(NOT WAYLAND_PROTOCOLS_BASE)
          MESSAGE(FATAL_ERROR "[GooseUI] -- Failed to find wayland-protocols")
        endif()

        LIST(APPEND GOOSEUI_INCLUDE ${WAYLAND_CLIENT_DIR} ${WAYLAND_EGL_DIR})
        LIST(APPEND GOOSEUI_LIBRARY ${WAYLAND_CLIENT_LIB} ${WAYLAND_EGL_LIB})
        LIST(APPEND GOOSEUI_BUILD_FLAGS "wayland-client")
        LIST(
          APPEND GOOSEUI_SRC
          "src/platform/wl_window.cpp"
        )

        # Generate Protocols
        function(generate_protocol xml_path basename)
          set(out_h "${CMAKE_CURRENT_BINARY_DIR}/wayland-protocols/${basename}-client-protocol.h")
          set(out_c "${CMAKE_CURRENT_BINARY_DIR}/wayland-protocols/${basename}-protocol.c")

          add_custom_command(
            OUTPUT ${out_h} ${out_c}
            COMMAND ${WAYLAND_SCANNER} client-header ${xml_path} ${out_h}
            COMMAND ${WAYLAND_SCANNER} private-code ${xml_path} ${out_c}
            DEPENDS ${xml_path}
            COMMENT "Generating Wayland protocol: ${basename}"
          )

          set(GOOSEUI_SRC ${GOOSEUI_SRC} ${out_c} PARENT_SCOPE)
        endfunction()

        generate_protocol("${WAYLAND_PROTOCOLS_BASE}/stable/xdg-shell/xdg-shell.xml" "xdg-shell")
        generate_protocol("${WAYLAND_PROTOCOLS_BASE}/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml" "xdg-decoration-unstable-v1")
        generate_protocol("${WAYLAND_PROTOCOLS_BASE}/staging/cursor-shape/cursor-shape-v1.xml" "cursor-shape-v1")
        generate_protocol("${WAYLAND_PROTOCOLS_BASE}/stable/tablet/tablet-v2.xml" "tablet-v2")

        LIST(APPEND GOOSEUI_INCLUDE "${CMAKE_CURRENT_BINARY_DIR}/wayland-protocols")
        add_compile_definitions(GOOSEUI_WAYLAND_SUPPORT)
    endif()
    
endif()