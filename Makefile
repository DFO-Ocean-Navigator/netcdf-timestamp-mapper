
all: src/main.cpp
	make clean
	mkdir -p ./build
	g++-8 -o build/nc-timestamp-mapper src/Database.cpp src/main.cpp -std=c++17 -lstdc++fs -lnetcdf_c++4 -lsqlite3 -I./src/ThirdParty/ -I/opt/tools/boost/boost_1_70_0/include/ -pedantic -Wall -Wextra -O3 -march=native

tests: tests/main.cpp
	mkdir -p ./build
	g++-8 -o build/tests tests/main.cpp -std=c++17 -lstdc++fs -lnetcdf_c++4 -I./src/ThirdParty/ -I./tests/ThirdParty/ -pedantic -Wall -Wextra -O3 -march=native

clean:
	rm -rf ./build
