#!/bin/bash
# Script to check C/C++ code formatting using astyle

astyle --version
          
# Find all C/C++ files
FILES=$(find src include -type f \( -name "*.cpp" -o -name "*.h" \))
          
if [ -z "$FILES" ]; then
    echo "No C/C++ files found to check"
    exit 0
fi
          
# Run astyle and check if any files would be formatted
echo "$FILES" | xargs astyle --options=scripts/.astyle.cfg
          
# Check if any files were modified
if ! git diff --quiet; then
    echo "??  Code style issues found. The following files need formatting:"
    git diff --name-only
    echo ""
    echo "To fix formatting, run:"
    echo "  find src include -type f \( -name '*.cpp' -o -name '*.h' \) | xargs astyle --options=scripts/.astyle.cfg"
    git checkout .
    exit 1
else
    echo "? All files are properly formatted!"
fi
