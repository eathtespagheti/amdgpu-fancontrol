# Copyright (C) 2021 Fabio Sussarellu
# 
# This file is part of amdgpu-fancontrol.
# 
# amdgpu-fancontrol is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# amdgpu-fancontrol is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with amdgpu-fancontrol.  If not, see <http://www.gnu.org/licenses/>.

ifeq ($(C_COMPILER),)
  $(error C_COMPILER is not set)
endif
build_folder=../../builds
filename=CardsManager

$(build_folder)/$(filename).o: $(build_folder)
	$(C_COMPILER) -c -o $(build_folder)/$(filename).o $(filename).c

$(build_folder):
	mkdir -p $(build_folder)

clean:
	rm $(build_folder)/$(filename).o