
lib_dir := $(CONDA_PREFIX)/lib
include_dir := $(CONDA_PREFIX)/include

compiler_and_flags := g++-9 -std=c++2a -Wall -Wextra -march=native -O3 -pedantic -Wshadow 

common := -o build/nc-timestamp-mapper -I./src/ThirdParty/ -I$(include_dir) -Wl,-rpath=$(lib_dir) -L$(lib_dir) src/TimestampMapper.cpp src/ProgressBar.cpp src/DatasetDesc.cpp src/Database.cpp src/FileReaders/NCFileReader.cpp src/CLIOptions.cpp src/main.cpp  

libs := -lstdc++fs -lnetcdf_c++4 -lsqlite3

create_output_dir := mkdir -p ./build

all: src/main.cpp
	make clean
	$(create_output_dir)
	$(compiler_and_flags) $(common) $(libs)

debug: src/main.cpp
	make clean
	$(create_output_dir)
	$(compiler_and_flags) $(common) $(libs) -D_DEBUG

.PHONY test: tests/main.cpp
	make clean
	$(create_output_dir)
	$(compiler_and_flags) -I./tests/ThirdParty/ -c tests/main.cpp
	$(compiler_and_flags) -o build/tests main.o tests/Test_VariableDesc.cpp tests/Test_DataFileDesc.cpp tests/Test_HashString.cpp $(libs) -I./src/ThirdParty/ -I./tests/ThirdParty/

clean:
	rm -rf ./build
