# Copyright (C) 2020 Fabio Sussarellu
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

source_folder = src
build_folder = builds
compiler = tcc
binary_name = amdgpu-fancontrol

$(build_folder)/$(binary_name): $(build_folder)/Parameter.o
	$(compiler) $(build_folder)/Parameter.o $(source_folder)/$(binary_name).c -o $(build_folder)/$(binary_name)

$(build_folder)/Parameter.o: $(build_folder)
	$(compiler) -c $(source_folder)/Parameter.c -o $(build_folder)/Parameter.o

$(build_folder):
	mkdir -p $(build_folder)

clean:
	rm -rf $(build_folder)