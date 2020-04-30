# Copyright 2020 UPMEM. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

find_path(LIBFTDI_INCLUDE_DIR ftdi.h)

find_library(LIBFTDI_LIBRARIES ftdi)

include(FindPackageHandleStandardArgs)

#Handle standard arguments to find_package like REQUIRED and QUIET
find_package_handle_standard_args(LibFtdi DEFAULT_MSG
    LIBFTDI_LIBRARIES
    LIBFTDI_INCLUDE_DIR
)
