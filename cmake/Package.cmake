# #######################################################################################################################
# # Automatic deployment
# #######################################################################################################################

get_filename_component(BUILD_FOLDER "${CMAKE_BINARY_DIR}" NAME)
set(BUILD_NAME "${BUILD_FOLDER}")

if(GTS_BUILD_DISTRIBUTION)

    message(STATUS "Automatic deployment build name: ${BUILD_NAME}")

    set(DEPLOY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/distribution/Package-${BUILD_NAME}")

    # Create the target deployment folder.
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}"
      COMMENT "Creating deployment directory for ${BUILD_NAME}"
    )

    # Copy the main DLL.
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
              $<TARGET_FILE:${PROJECT_NAME}>
              "${DEPLOY_DIR}/"
      COMMENT "Copying ${PROJECT_NAME} DLL to deployment directory"
    )

    # Copy the PDB if available
    if(CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo|Release")
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_PDB_FILE:${PROJECT_NAME}>
                "${DEPLOY_DIR}/"
        COMMENT "Copying ${PROJECT_NAME} PDB to deployment directory"
      )
    endif()

    # Create the GTSPlugin subfolder
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${DEPLOY_DIR}/GTSPlugin"
      COMMENT "Creating GTSPlugin subdirectory"
    )

    # Copy Fonts directory
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/distribution/Fonts")
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/distribution/Fonts"
                "${DEPLOY_DIR}/GTSPlugin/Fonts"
        COMMENT "Copying Fonts directory to deployment"
      )
    endif()

    # Copy Icons directory
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/distribution/Icons")
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/distribution/Icons"
                "${DEPLOY_DIR}/GTSPlugin/Icons"
        COMMENT "Copying Icons directory to deployment"
      )
    endif()

    # Copy Config directory
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/distribution/Config")
      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/distribution/Config"
                "${DEPLOY_DIR}/GTSPlugin"
        COMMENT "Copying Config directory to deployment"
      )
    endif()

    # Clean output DLLs and PDBs
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/distribution")
      file(GLOB_RECURSE OUTPUT_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/distribution/Plugin-*/*.dll")
      file(GLOB_RECURSE OUTPUT_PDBS "${CMAKE_CURRENT_SOURCE_DIR}/distribution/Plugin-*/*.pdb")

      if(OUTPUT_DLLS)
        set_property(TARGET ${PROJECT_NAME}
          APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${OUTPUT_DLLS}")
      endif()

      if(OUTPUT_PDBS)
        set_property(TARGET ${PROJECT_NAME}
          APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${OUTPUT_PDBS}")
      endif()
    endif()
endif()

# Copy to GTSPLUGIN_COPY_DIR if defined
if(GTS_DEPLOY_TO_FOLDER)
    if(DEFINED ENV{GTSPLUGIN_COPY_DIR})
      message(STATUS "GTSPLUGIN_COPY_DIR is set to: $ENV{GTSPLUGIN_COPY_DIR}")

      add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                $<TARGET_FILE:${PROJECT_NAME}>
                "$ENV{GTSPLUGIN_COPY_DIR}/"
        COMMENT "Copying ${PROJECT_NAME} DLL to GTSPLUGIN_COPY_DIR"
      )

      if(CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo|Release")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E copy
                  $<TARGET_PDB_FILE:${PROJECT_NAME}>
                  "$ENV{GTSPLUGIN_COPY_DIR}/"
          COMMENT "Copying ${PROJECT_NAME} PDB to GTSPLUGIN_COPY_DIR"
        )
      endif()
    endif()
 endif()