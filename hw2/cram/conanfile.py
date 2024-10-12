from conan import ConanFile
from conan.tools.cmake import cmake_layout


class ExampleRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def requirements(self):
        self.tool_requires("flex/2.6.4")
        self.tool_requires("bison/3.8.2")

    def layout(self):
        cmake_layout(self)
