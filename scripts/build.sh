#!/bin/bash

set -e              # Exit immediately if a command exits with a non-zero status.
premake5 gmake      # Generate GNU Makefiles using Premake5
make -j$(nproc) $@  # Build the project using all available CPU cores, and pass
                    # any additional arguments to `make`.
                    