[![CodeFactor](https://www.codefactor.io/repository/github/dfo-ocean-navigator/netcdf-timestamp-mapper/badge)](https://www.codefactor.io/repository/github/dfo-ocean-navigator/netcdf-timestamp-mapper)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/dab035a5fdf5491daca4bbc8980b1615)](https://www.codacy.com/app/htmlboss/netcdf-timestamp-mapper?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=DFO-Ocean-Navigator/netcdf-timestamp-mapper&amp;utm_campaign=Badge_Grade)

## Dependencies

* Ubuntu 16.04 or newer.
* Boost 1.70 or newer.
* GCC 9 (for C++20 support) or newer.
* Headers and libraries in `install_deps.sh`

## Notes
* The `Makefile` looks for Boost in `opt/tools/boost/boost_1_70_0/include/`.

## Building

* In the root project directory: `make` to build the program, `make tests` to build the tests, and `make clean` to...clean.
