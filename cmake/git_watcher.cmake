# git_watcher.cmake
# https://raw.githubusercontent.com/andrew-hardin/cmake-git-version-tracking/master/git_watcher.cmake
#
# Released under the MIT License.
# https://raw.githubusercontent.com/andrew-hardin/cmake-git-version-tracking/master/LICENSE

macro(PATH_TO_ABSOLUTE var_name)
    get_filename_component(${var_name} "${${var_name}}" ABSOLUTE)
endmacro()

macro(CHECK_REQUIRED_VARIABLE var_name)
    if(NOT DEFINED ${var_name})
        message(FATAL_ERROR "The \"${var_name}\" variable must be defined.")
    endif()
    PATH_TO_ABSOLUTE(${var_name})
endmacro()

macro(CHECK_OPTIONAL_VARIABLE var_name default_value)
    if(NOT DEFINED ${var_name})
        set(${var_name} ${default_value})
    endif()
    PATH_TO_ABSOLUTE(${var_name})
endmacro()

CHECK_REQUIRED_VARIABLE(PRE_CONFIGURE_FILE)
CHECK_REQUIRED_VARIABLE(POST_CONFIGURE_FILE)
CHECK_OPTIONAL_VARIABLE(GIT_STATE_FILE "${CMAKE_BINARY_DIR}/git-state-hash")
CHECK_OPTIONAL_VARIABLE(GIT_WORKING_DIR "${CMAKE_SOURCE_DIR}")

if(NOT DEFINED GIT_EXECUTABLE)
    find_package(Git QUIET REQUIRED)
endif()
CHECK_REQUIRED_VARIABLE(GIT_EXECUTABLE)

set(_state_variable_names
    GIT_RETRIEVED_STATE
    GIT_HEAD_SHA1
    GIT_IS_DIRTY
    GIT_COMMIT_DATE_ISO8601
    GIT_DESCRIBE
)

macro(RunGitCommand)
    execute_process(COMMAND
        "${GIT_EXECUTABLE}" ${ARGV}
        WORKING_DIRECTORY "${_working_dir}"
        RESULT_VARIABLE exit_code
        OUTPUT_VARIABLE output
        ERROR_QUIET
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT exit_code EQUAL 0)
        set(ENV{GIT_RETRIEVED_STATE} "false")
    endif()
endmacro()

function(GetGitState _working_dir)

    set(ENV{GIT_RETRIEVED_STATE} "true")

    RunGitCommand(status --porcelain)
    if(NOT exit_code EQUAL 0)
        set(ENV{GIT_IS_DIRTY} "false")
    else()
        if(NOT "${output}" STREQUAL "")
            set(ENV{GIT_IS_DIRTY} "true")
        else()
            set(ENV{GIT_IS_DIRTY} "false")
        endif()
    endif()

    set(object HEAD)
    RunGitCommand(show -s "--format=%H" ${object})
    if(exit_code EQUAL 0)
        set(ENV{GIT_HEAD_SHA1} ${output})
    endif()

    RunGitCommand(show -s "--format=%ci" ${object})
    if(exit_code EQUAL 0)
        set(ENV{GIT_COMMIT_DATE_ISO8601} "${output}")
    endif()

    RunGitCommand(describe --always ${object})
    if(NOT exit_code EQUAL 0)
        set(ENV{GIT_DESCRIBE} "unknown")
    else()
        set(ENV{GIT_DESCRIBE} "${output}")
    endif()

endfunction()

function(GitStateChangedAction)
    foreach(var_name ${_state_variable_names})
        set(${var_name} $ENV{${var_name}})
    endforeach()
    configure_file("${PRE_CONFIGURE_FILE}" "${POST_CONFIGURE_FILE}" @ONLY)
endfunction()

function(HashGitState _state)
    set(ans "")
    foreach(var_name ${_state_variable_names})
        string(SHA256 ans "${ans}$ENV{${var_name}}")
    endforeach()
    set(${_state} ${ans} PARENT_SCOPE)
endfunction()

function(CheckGit _working_dir _state_changed)

    GetGitState("${_working_dir}")

    HashGitState(state)

    file(SHA256 ${PRE_CONFIGURE_FILE} preconfig_hash)
    string(SHA256 state "${preconfig_hash}${state}")

    if(EXISTS "${GIT_STATE_FILE}")
        file(READ "${GIT_STATE_FILE}" OLD_HEAD_CONTENTS)
        if(OLD_HEAD_CONTENTS STREQUAL "${state}")
            set(${_state_changed} "false" PARENT_SCOPE)
            return()
        endif()
    endif()

    file(WRITE "${GIT_STATE_FILE}" "${state}")
    set(${_state_changed} "true" PARENT_SCOPE)
endfunction()

function(SetupGitMonitoring)
    add_custom_target(check_git
        ALL
        DEPENDS ${PRE_CONFIGURE_FILE}
        BYPRODUCTS
            ${POST_CONFIGURE_FILE}
            ${GIT_STATE_FILE}
        COMMENT "Checking the git repository for changes..."
        COMMAND
            ${CMAKE_COMMAND}
            -D_BUILD_TIME_CHECK_GIT=TRUE
            -DGIT_WORKING_DIR=${GIT_WORKING_DIR}
            -DGIT_EXECUTABLE=${GIT_EXECUTABLE}
            -DGIT_STATE_FILE=${GIT_STATE_FILE}
            -DPRE_CONFIGURE_FILE=${PRE_CONFIGURE_FILE}
            -DPOST_CONFIGURE_FILE=${POST_CONFIGURE_FILE}
            -P "${CMAKE_CURRENT_LIST_FILE}")
endfunction()

function(Main)
    if(_BUILD_TIME_CHECK_GIT)
        CheckGit("${GIT_WORKING_DIR}" changed)
        if(changed OR NOT EXISTS "${POST_CONFIGURE_FILE}")
            GitStateChangedAction()
        endif()
    else()
        SetupGitMonitoring()
    endif()
endfunction()

Main()
