[![Codacy Badge](https://api.codacy.com/project/badge/Grade/083f17b426a543abace3a1fb8faf5e8a)](https://app.codacy.com/app/htmlboss/netcdf-timestamp-mapper?utm_source=github.com&utm_medium=referral&utm_content=DFO-Ocean-Navigator/netcdf-timestamp-mapper&utm_campaign=Badge_Grade_Dashboard)
[![CodeFactor](https://www.codefactor.io/repository/github/dfo-ocean-navigator/netcdf-timestamp-mapper/badge)](https://www.codefactor.io/repository/github/dfo-ocean-navigator/netcdf-timestamp-mapper)

## Dependencies

* Ubuntu 16.04 or newer.
* Boost 1.70 or newer.
* GCC 9 (for C++20 support) or newer.
* Headers and libraries in `install_deps.sh`

## Notes
* The `Makefile` looks for Boost in `opt/tools/boost/boost_1_70_0/include/`.

## Building

* In the root project directory: `make` to build the program, `make tests` to build the tests, and `make clean` to...clean.
