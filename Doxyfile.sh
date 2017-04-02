#!/usr/bin/env bash

# Script for generating file.
TARGET=Doxyfile

command -v doxygen >/dev/null 2>&1 || {
    echo >&2 "Doxygen not available in PATH.";
    exit 1;
}

command -v git >/dev/null 2>&1 && {
    GIT_TAG=$(git describe --first-parent 2> /dev/null)
    PROJECT_NUMBER=$(expr "$GIT_TAG" : 'v\([0-9]\+.[0-9]\+.[0-9]\+\)')
}

echo -n "\
DOXYFILE_ENCODING      = UTF-8
PROJECT_NAME           = pac
PROJECT_NUMBER         = $PROJECT_NUMBER
PROJECT_BRIEF          =
PROJECT_LOGO           =
OUTPUT_DIRECTORY       = target/doc/
OPTIMIZE_OUTPUT_FOR_C  = YES
JAVADOC_AUTOBRIEF      = YES
INPUT                  = src/
INPUT_ENCODING         = UTF-8
FILE_PATTERNS          = *.h *.md
RECURSIVE              = YES
" > $TARGET