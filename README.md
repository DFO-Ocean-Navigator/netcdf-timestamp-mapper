[![CodeFactor](https://www.codefactor.io/repository/github/dfo-ocean-navigator/netcdf-timestamp-mapper/badge)](https://www.codefactor.io/repository/github/dfo-ocean-navigator/netcdf-timestamp-mapper)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/dab035a5fdf5491daca4bbc8980b1615)](https://www.codacy.com/app/htmlboss/netcdf-timestamp-mapper?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=DFO-Ocean-Navigator/netcdf-timestamp-mapper&amp;utm_campaign=Badge_Grade)

## Dependencies

* Ubuntu 16.04 or newer.
* GCC 7.4 or newer.
* Miniconda3 x64 (latest version) (`conda env create --file environment.yml`).

## Documentation
* Tool docs: [https://dfo-ocean-navigator.github.io/netcdf-timestamp-mapper/](https://dfo-ocean-navigator.github.io/netcdf-timestamp-mapper/)
* Code Doxygen: [https://dfo-ocean-navigator.github.io/netcdf-timestamp-mapper/code-docs/index.html](https://dfo-ocean-navigator.github.io/netcdf-timestamp-mapper/code-docs/index.html)
* Historical table schema: [https://dbdiagram.io/d/5dcc0404edf08a25543de03f](https://dbdiagram.io/d/5dcc0404edf08a25543de03f)
![historical table schema](https://raw.githubusercontent.com/DFO-Ocean-Navigator/netcdf-timestamp-mapper/master/docs/img/Historical-Tables.png "Historical Table Schema")

## Building
* Follow Dependencies above.
* `conda activate index-tool`.
* Clone this repo and move into the directory.
* `git submodule update --init --recursive`
* `make` to build the program, `make test` to build the tests, and `make clean` to...clean.
