# setup.py
from setuptools import setup, Extension
import pybind11
from pybind11.setup_helpers import Pybind11Extension
import os

version = "0.0.8"

# skip module import if compiling for ReadTheDocs
if os.environ.get("READTHEDOCS"):
    ext_mods = []
else:
    ext_mods = [
        Pybind11Extension(
            "red_caen", 
            ["red_digitizer_helper.cpp"], 
            define_macros=[("VERSION", f"\"{version}\"")], 
            libraries=["CAENDigitizer"],
            cxx_std=20
        ),
    ]

setup(
    name="red_caen",
    version=version,
    description="Python library for wrapper of CAEN driver",
    ext_modules=ext_mods,
    install_requires=["pybind11"],
)

