project(p3vis)

file (GLOB SRCS "*.cpp")
file (GLOB HEADERS "*.h")

source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SRCS})

set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type.")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /DEBUG")
string(REPLACE "/DNDEBUG" "" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

if (DEFINED LINK_ALL_STATIC)
    set(P3BUILT built_x64_static)
else()
    set(P3BUILT built_x64)
endif ()

set(PANDA3D_DIR ..\\..\\..\\cio\\cio-panda3d\\${P3BUILT} CACHE PATH "Panda3D directory")
set(BSP_COMMON_INCLUDE ..\\common CACHE PATH "Common directory")
set(BSP_COMMON_LIB ..\\common\\Release\\bsp_common.lib CACHE FILEPATH "Common lib file")

add_definitions(-DBUILDING_P3VIS -DWITHIN_PANDA -DNOMINMAX -DSTDC_HEADERS -DHLVIS)
if (DEFINED LINK_ALL_STATIC)
    add_definitions(-DLINK_ALL_STATIC)
endif ()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    # 64 bits
    add_definitions(-DVERSION_64BIT)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
    # 32 bits
    add_definitions(-DVERSION_32BIT)
endif()

include_directories(./ ${PANDA3D_DIR}/include ${BSP_COMMON_INCLUDE})
link_directories(${PANDA3D_DIR}/lib)

add_executable(p3vis ${SRCS} ${HEADERS})

STRING(REGEX REPLACE "/" "\\\\" WIN_PANDA3D_DIR ${PANDA3D_DIR})

add_custom_command(TARGET p3vis POST_BUILD COMMAND copy /Y ${CMAKE_BUILD_TYPE}\\p3vis.exe ${WIN_PANDA3D_DIR}\\bin\\p3vis.exe)
add_custom_command(TARGET p3vis POST_BUILD COMMAND copy /Y ${CMAKE_BUILD_TYPE}\\p3vis.pdb ${WIN_PANDA3D_DIR}\\bin\\p3vis.pdb)

target_link_libraries(p3vis
					  libpanda.lib
					  libpandaexpress.lib
					  libp3dtool.lib
					  libp3dtoolconfig.lib
                      ${BSP_COMMON_LIB})
