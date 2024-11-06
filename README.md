# RedDigitizer++

A [CAEN](https://www.caen.it/) C++ wrapper for their C API.

CAEN is an italian company that produces digitizer, power supplies, 
amplifiers, and many others types of electronics for particle/nuclear physics.

Their code is great, but it is not beginner friendly. This 
header-only library will help with that. It uses C++17, and it is written for 
readability and memory safety.

This code also contains several examples that can be used to inspire your 
own code writing.

# PyBind11 wrapper
This code is modified to that it can be compiled into a python library using PyBind11.

## How to compile
### Using `setup.py` (Recommended)
1. Make sure `CMake`, `CAENDigitizer`, `PyBind11`  are all installed. `CMake` can be installed using package managers or from https://cmake.org/download/. `CAENDigitizer` can be installed from https://www.caen.it/products/caendigitizer-library/. `PyBind11` can be installed using conda-forge or pip. See https://pybind11.readthedocs.io/en/stable/installing.html.
2. Activate the conda environment if needed.
3. Inside this repository, run `pip install .`.
4. Import the library by running `import red_caen`.

### Using `CMakeLists.txt`
1. Make sure `CMake`, `CAENDigitizer`, `PyBind11`  are all installed. 
2. Run `ccmake .` to generate `Makefile`.
3. Run `make` to compile, generating the `red_caen.so` binary file.
4. Move the binary file to the same folder as the python script, or add the path to the bianry file to `PYTHONPATH` in environment variables. In python, run `import red_caen` to import.