#!/bin/bash

# This file fetches version information from version.cmake file and stores it in shell variables to be used later on

# Path to version.cmake
VERSION_FILE="Systems/Generic/version.cmake"

get_version_value() {
    local var_name="$1"
    # Use sed to extract the value for the variable
    sed -n "s/^set($var_name \\(.*\\))/\\1/p" "$VERSION_FILE" | tr -d '"'
}

# Read the values from the cmake file
MAJOR_VERSION=$(get_version_value "MAJOR_VERSION")
MINOR_VERSION=$(get_version_value "MINOR_VERSION")
PATCH_VERSION=$(get_version_value "PATCH_VERSION")
STAGE=$(get_version_value "STAGE")

REVISION_COUNT="$(git log --oneline | wc -l)"
COMMIT_HASH="$(git rev-parse --short=8 HEAD)"

# Export the variables to be used in the current shell
export MAJOR_VERSION
export MINOR_VERSION
export PATCH_VERSION
export STAGE