
lib_dir := $(CONDA_PREFIX)/lib
include_dir := $(CONDA_PREFIX)/include

compiler_and_flags := $(CXX) -std=c++17 -Wall -Wextra -Wno-unused-variable -march=native -O3 -pedantic -Wshadow -I$(include_dir) -Wl,-rpath=$(lib_dir) -L$(lib_dir)

common := -o build/nc-timestamp-mapper -I./src/ThirdParty/ src/TimestampMapper.cpp src/Utils/ProgressBar.cpp src/DatasetDesc.cpp src/Database.cpp src/FileReaders/NCFileReader.cpp src/CLIOptions.cpp src/main.cpp

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
	$(compiler_and_flags) -o build/tests main.o src/FileReaders/NCFileReader.cpp tests/Test_VariableDesc.cpp tests/Test_CrawlDirectory.cpp tests/Test_TypeTimestamp.cpp tests/Test_DataFileDesc.cpp tests/Test_NCFileReader.cpp tests/Test_HashString.cpp $(libs) -I./src/ThirdParty/ -I./tests/ThirdParty/
	cp -r ./tests/Fixtures ./build

clean:
	rm -rf ./build
