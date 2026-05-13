#!/bin/bash

# Check if VCPKG_ROOT environment variable is set
if [ -z "$VCPKG_ROOT" ]; then
    echo "VCPKG_ROOT environment variable not detected. Please set VCPKG_ROOT first."
    exit 1
fi

# Check if vcpkg executable exists in VCPKG_ROOT directory
if [ ! -x "$VCPKG_ROOT/vcpkg" ]; then
    echo "vcpkg not found in $VCPKG_ROOT. Please check your VCPKG_ROOT setting."
    exit 1
fi

# Check if package list file exists
if [ ! -f "VcpkgPackageList.txt" ]; then
    echo "VcpkgPackageList.txt not found in current directory."
    exit 1
fi

# Read packages from VcpkgPackageList.txt and install them
PACKAGES=""
while IFS= read -r line || [ -n "$line" ]; do
    PKG=$(echo "$line" | xargs) # Trim whitespace
    if [ -n "$PKG" ]; then
        PACKAGES="$PACKAGES $PKG"
    fi
done < "VcpkgPackageList.txt"

if [ -z "$PACKAGES" ]; then
    echo "No packages specified in VcpkgPackageList.txt."
    exit 1
fi

"$VCPKG_ROOT/vcpkg" install $PACKAGES

# Check installation result
if [ $? -eq 0 ]; then
    echo "Packages installed successfully."
else
    echo "Package installation failed. Please check the error message."
    exit 1
fi