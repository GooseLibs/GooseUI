CMAKE_MINIMUM_REQUIRED(VERSION 4.2.0)
PROJECT(GooseUI-tests VERSION 0.0.1)

link_libraries(GooseUI OpenGL::GL)

add_executable(WindowTest windowTest.cpp)
#add_executable(FontTest fontTest.cpp)
#add_executable(WidgetTest widgetTest.cpp)