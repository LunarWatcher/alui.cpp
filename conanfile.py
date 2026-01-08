from conan import ConanFile
import os
from conan.tools.files import copy
from pathlib import Path
import shutil

class AluiTests(ConanFile):
    """
    Used for fetching dependencies for use in tests
    """
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("freetype/2.13.3")
        self.requires("libpng/1.6.53")
        self.requires("zlib/1.3.1")
        self.requires("doxygen/1.16.0")
