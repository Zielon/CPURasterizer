add_executable(CPURasterizer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Texture.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Clipper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/ColorBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/DepthBuffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Rasterizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/SutherlandHodgman.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Antialising/FXAA.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Samplers/Sampler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HDR/HDRLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImGui/imgui_impl_glfw.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ImGui/imgui_impl_opengl3.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Loader/Loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewer/Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewer/Menu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Viewer/Window.cpp)
  set_target_properties(CPURasterizer PROPERTIES
      CXX_VISIBILITY_PRESET   "hidden")
  target_compile_features(CPURasterizer
      PRIVATE
        cxx_std_17)
  target_compile_definitions(CPURasterizer
      PRIVATE
        IMGUI_IMPL_OPENGL_LOADER_GLAD)
  target_include_directories(CPURasterizer
      PRIVATE
        ${STB_INCLUDE_DIRS})
  target_link_libraries(CPURasterizer
      PRIVATE
        glad::glad glfw imgui::imgui tinyobjloader::tinyobjloader
        OpenMP::OpenMP_CXX
        $<$<CXX_COMPILER_ID:GNU>:tbb>)