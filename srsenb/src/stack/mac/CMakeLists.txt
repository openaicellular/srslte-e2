#
# Copyright 2013-2020 Software Radio Systems Limited
#
# This file is part of srsLTE
#
# srsLTE is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsLTE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

if(ENABLE_SLICER)
  set(SOURCES mac.cc ue.cc scheduler.cc scheduler_carrier.cc scheduler_grid.cc scheduler_harq.cc scheduler_ue.cc slicer.cc scheduler_metric_sliced.cc)
else()
  set(SOURCES mac.cc ue.cc scheduler.cc scheduler_carrier.cc scheduler_grid.cc scheduler_harq.cc scheduler_metric.cc scheduler_ue.cc)
endif()
add_library(srsenb_mac STATIC ${SOURCES})

if(ENABLE_5GNR)
    set(SOURCES mac_nr.cc)
    add_library(srsgnb_mac STATIC ${SOURCES})
endif()
