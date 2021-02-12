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
dependecies = $(build_folder)/Parameter.o $(build_folder)/ParametersManagement.o $(build_folder)/TemperatureSpeedMap.o $(build_folder)/Logging.o $(build_folder)/CommonVariables.o $(build_folder)/ForksManager.o $(build_folder)/CardsManager.o

$(build_folder)/$(binary_name): $(dependecies)
	$(compiler) -o $(build_folder)/$(binary_name) $(dependecies) $(source_folder)/$(binary_name).c

$(build_folder)/CardsManager.o:
	$(MAKE) -C $(source_folder)/CardsManager.h

$(build_folder)/ForksManager.o:
	$(MAKE) -C $(source_folder)/ForksManager.h

$(build_folder)/CommonVariables.o:
	$(MAKE) -C $(source_folder)/CommonVariables.h

$(build_folder)/Logging.o:
	$(MAKE) -C $(source_folder)/Logging.h

$(build_folder)/TemperatureSpeedMap.o:
	$(MAKE) -C $(source_folder)/TemperatureSpeedMap.h

$(build_folder)/ParametersManagement.o:
	$(MAKE) -C $(source_folder)/ParametersManagement.h

$(build_folder)/Parameter.o:
	$(MAKE) -C $(source_folder)/Parameter.h

clean:
	rm $(build_folder)/$(binary_name)