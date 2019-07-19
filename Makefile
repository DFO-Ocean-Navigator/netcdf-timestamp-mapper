
common := g++-9 -o build/nc-timestamp-mapper src/TimestampMapper.cpp src/DatasetDesc.cpp src/Database.cpp src/main.cpp -std=c++2a -lstdc++fs -lnetcdf_c++4 -lsqlite3 -I./src/ThirdParty/ -I/opt/tools/boost/boost_1_70_0/include/ -pedantic-errors -Wall -Wextra -O3 -march=native

create_output_dir := mkdir -p ./build

all: src/main.cpp
	make clean
	$(create_output_dir)
	$(common)

debug: src/main.cpp
	make clean
	$(create_output_dir)
	$(common) -D_DEBUG

tests: tests/main.cpp
	$(create_output_dir)
	g++-9 -o build/tests tests/main.cpp -std=c++17 -lstdc++fs -lnetcdf_c++4 -I./src/ThirdParty/ -I./tests/ThirdParty/ -pedantic-errors -Wall -Wextra -O3 -march=native

clean:
	rm -rf ./build
