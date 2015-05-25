#
# Copyright (c) 2013, BlurryRoots aka Sven Freiberg
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


#
# basic setup
#

PROJECT_NAME = string-library

C_COMPILER = clang
C_COMPILER_FLAGS = -Wall -std=c11

CPP_COMPILER = clang++
CPP_COMPILER_FLAGS = -Wall -std=c++11

SOURCE = src
TEST = test
BUILD = bin
OBJECTS = obj

TEST_SUIT = Catch/include

INCLUDES = -I$(TEST_SUIT) -I$(SOURCE)
LIB_PATH = -L$(BUILD)
LIBS = -l$(PROJECT_NAME)

ERROR_OUTPUT = 2> error.log

C_COMPILE_COMMAND = $(C_COMPILER) $(C_COMPILER_FLAGS)

CPP_COMPILE_COMMAND = $(CPP_COMPILER) $(CPP_COMPILER_FLAGS)

#
# cleaning
#

default: all

clean-build:
	rm -rf $(BUILD)

clean-objects:
	rm -rf $(OBJECTS)

clean-test:
	rm -rf $(BUILD)/test
	rm -rf $(OBJECTS)/test

prepare-build:
	mkdir -p $(BUILD)

prepare-objects:
	mkdir -p $(OBJECTS)

prepare-test:
	mkdir -p $(BUILD)/test
	mkdir -p $(OBJECTS)/test

clean: clean-build clean-objects

prepare: prepare-build prepare-objects

build-static-library:
	$(C_COMPILE_COMMAND) -c $(SOURCE)/$(PROJECT_NAME).c -o $(OBJECTS)/$(PROJECT_NAME).o $(ERROR_OUTPUT)
	ar rvs $(BUILD)/lib$(PROJECT_NAME).a $(OBJECTS)/$(PROJECT_NAME).o

build-test: clean-test prepare-test
	$(CPP_COMPILE_COMMAND) $(INCLUDES) -c $(TEST)/$(PROJECT_NAME).c -o $(OBJECTS)/test/$(PROJECT_NAME).o $(ERROR_OUTPUT)
	$(CPP_COMPILER) $(LIB_PATH) $(OBJECTS)/test/$(PROJECT_NAME).o $(LIBS) -o $(BUILD)/test/$(PROJECT_NAME) $(ERROR_OUTPUT)

run-test: build-test
	bin/test/./$(PROJECT_NAME)

static-library: clean prepare build-static-library

static-library-with-test: static-library run-test

all: static-library-with-test
