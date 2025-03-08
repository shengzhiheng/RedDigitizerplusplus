# setup.py
from setuptools import setup, Extension
import pybind11
from pybind11.setup_helpers import Pybind11Extension
import os

# skip module import if compiling for ReadTheDocs
if os.environ.get("READTHEDOCS"):
    ext_mods = []
else:
    ext_mods = [
        Pybind11Extension(
            "red_caen", 
            ["red_digitizer_helper.cpp"], 
            libraries=["CAENDigitizer"],
            cxx_std=20
        ),
    ]

setup(
    name="red_caen",
    version="0.0.7",
    description="Python library for wrapper of CAEN driver",
    ext_modules=ext_mods,
    install_requires=["pybind11"],
)
