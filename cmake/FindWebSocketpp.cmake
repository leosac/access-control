# - Try to find libwebsocketpp library
# Once done this will define
#
#  WEBSOCKETPP_FOUND - system has libwebsocketpp
#  WEBSOCKETPP_INCLUDE_DIR - the libwebsocketpp include directory
#  WEBSOCKETPP_LIBRARY - Link these to use libwebsocketpp

FIND_PATH(WEBSOCKETPP_INCLUDE_DIR
  NAMES websocketpp/websocketpp.hpp
  PATH_SUFFIXES include
  DOC "libwebsocketpp include directory")

FIND_LIBRARY(WEBSOCKETPP_LIBRARY
  NAMES websocketpp
  PATH_SUFFIXES bin lib
  DOC "libwebsocketpp library")

# handle the QUIETLY and REQUIRED arguments and set WEBSOCKETPP_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(WebSocketpp DEFAULT_MSG WEBSOCKETPP_LIBRARY WEBSOCKETPP_INCLUDE_DIR)

MARK_AS_ADVANCED(WEBSOCKETPP_INCLUDE_DIR WEBSOCKETPP_LIBRARY)
