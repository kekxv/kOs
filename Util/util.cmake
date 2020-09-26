
FILE(GLOB srcSourceHeader "${CMAKE_CURRENT_LIST_DIR}/*")
list(FILTER srcSourceHeader EXCLUDE REGEX ".*\\.c")
list(FILTER srcSourceHeader EXCLUDE REGEX ".*\\.cpp")
list(FILTER srcSourceHeader EXCLUDE REGEX ".*\\.h")
list(FILTER srcSourceHeader EXCLUDE REGEX ".*\\.cmake")
include_directories(${CMAKE_CURRENT_LIST_DIR} ${srcSourceHeader})