# CMakeList.txt: DB 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)
project ("SimpleDB")

set(EXEC db)

# 将源代码添加到此项目的可执行文件。
aux_source_directory(./src SRC_LIST)
message(STATUS "The src=${SRC_LIST}.")
add_executable (${EXEC} ${SRC_LIST})


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${EXEC} PROPERTY C_STANDARD)
endif()

enable_testing()
add_test(
  NAME python_test
  COMMAND pytest -v ${CMAKE_CURRENT_SOURCE_DIR}/test/pytest/case.py --exec $<TARGET_FILE:${EXEC}>
  )

# TODO: 如有需要，请添加测试并安装目标。
