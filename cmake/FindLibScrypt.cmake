include(FindPackageHandleStandardArgs)

if (LIBSCRYPT_INCLUDE_DIR AND LIBSCRYPT_LIBRARY)
    set(LibScrypt_FIND_QUIETLY TRUE)

else (LIBSCRYPT_INCLUDE_DIR AND LIBSCRYPT_LIBRARY)
    set(LibScrypt_FIND_QUIETLY false)

    find_path (LIBSCRYPT_INCLUDE_DIR
      NAMES libscrypt.h
      PATH_SUFFIXES include
      DOC "LibScrypt include directory")

    find_library (LIBSCRYPT_LIBRARY
      NAMES scrypt
      PATH_SUFFIXES lib
      DOC "LibScrypt release library")

endif (LIBSCRYPT_INCLUDE_DIR AND LIBSCRYPT_LIBRARY)


FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibScrypt DEFAULT_MSG LIBSCRYPT_INCLUDE_DIR LIBSCRYPT_LIBRARY)
mark_as_advanced (LIBSCRYPT_INCLUDE_DIR LIBSCRYPT_LIBRARY)


