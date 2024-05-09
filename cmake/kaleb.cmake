include(${EXTERN_DIR}/includes/kaleb/shared/cmake/assets.cmake)

string(JSON DEPENDENCIES_LENGTH LENGTH ${PACKAGE_JSON} restoredDependencies)
math(EXPR DEPENDENCIES_LENGTH "${DEPENDENCIES_LENGTH} - 1")

foreach(DEP_IDX RANGE 0 ${DEPENDENCIES_LENGTH} )
    string(JSON DEPENDENCY GET ${PACKAGE_JSON} restoredDependencies ${DEP_IDX})
    string(JSON INTERNAL_DEPENDENCY GET ${DEPENDENCY} dependency)
    string(JSON DEPENDENCY_ID GET ${INTERNAL_DEPENDENCY} id)

    string(COMPARE EQUAL "${DEPENDENCY_ID}" "kaleb" IS_KALEB)

    if (${IS_KALEB})
        string(JSON KALEB_VERSION GET ${DEPENDENCY} version)
    endif()
endforeach()
