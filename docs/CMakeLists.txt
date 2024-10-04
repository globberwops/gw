find_package(Doxygen OPTIONAL_COMPONENTS dot)

if(NOT Doxygen_FOUND)
  message(WARNING "Doxygen not found, documentation will not be generated")
  return()
endif()

include(FetchContent)
FetchContent_Declare(
  doxygen_awesome_css
  GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
  GIT_TAG v2.3.4)
FetchContent_MakeAvailable(doxygen_awesome_css)

set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_DISABLE_INDEX NO)
set(DOXYGEN_DOT_IMAGE_FORMAT svg)
set(DOXYGEN_DOT_TRANSPARENT YES)
set(DOXYGEN_EXAMPLE_PATH ${PROJECT_SOURCE_DIR}/examples)
set(DOXYGEN_EXCLUDE_SYMBOLS = "detail")
set(DOXYGEN_FULL_SIDEBAR NO)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_HTML_EXTRA_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/header.html
    ${doxygen_awesome_css_SOURCE_DIR}/doxygen-awesome-darkmode-toggle.js
    ${doxygen_awesome_css_SOURCE_DIR}/doxygen-awesome-fragment-copy-button.js
    ${doxygen_awesome_css_SOURCE_DIR}/doxygen-awesome-interactive-toc.js
    ${doxygen_awesome_css_SOURCE_DIR}/doxygen-awesome-paragraph-link.js)
set(DOXYGEN_HTML_EXTRA_STYLESHEET
    ${doxygen_awesome_css_SOURCE_DIR}/doxygen-awesome.css
    ${doxygen_awesome_css_SOURCE_DIR}/doxygen-awesome-sidebar-only.css
    ${doxygen_awesome_css_SOURCE_DIR}/doxygen-awesome-sidebar-only-darkmode-toggle.css)
set(DOXYGEN_HTML_HEADER header.html)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ${PROJECT_SOURCE_DIR}/README.md)

doxygen_add_docs(gw_docs ${PROJECT_SOURCE_DIR}/README.md ${PROJECT_SOURCE_DIR}/include COMMENT "Build documentation")
