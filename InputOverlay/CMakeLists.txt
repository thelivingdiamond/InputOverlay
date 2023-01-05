set(MOD_NAME InputOverlay) # CHANGE ME
set(MOD_PREFIX INPUT_OVERLAY) # CHANGE ME

set(SOURCE_FILES
	CMakeLists.txt
	ModMain.cpp
	ModMain.h
	pch.h
	KeyElement.cpp
	KeyElement.h
	InputNameMap.h
		OverlayInputListener.cpp OverlayInputListener.h ImGuiUtils.h ImGuiUtils.cpp MouseVisualizer.cpp MouseVisualizer.h)

add_library(${MOD_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${MOD_NAME}
	CommonMod
)

target_precompile_headers(${MOD_NAME} PRIVATE pch.h)
mod_output_dir(${MOD_NAME} ${MOD_PREFIX})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
