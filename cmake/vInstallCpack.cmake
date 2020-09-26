#vInstallCpack.cmake
###cpack
string(TIMESTAMP vTimeStamp "%Y%m%d%H%M")

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${vTimeStamp}")
include(CPack)