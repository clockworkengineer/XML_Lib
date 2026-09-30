from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
import os

class XmlLibConan(ConanFile):
    name = "xml-lib"
    version = "1.4.0"
    license = "MIT"
    author = "Rob Thomas"
    url = "https://github.com/clockworkengineer/XML_Lib"
    description = "High-performance, standards-compliant, modular C++20 XML library with zero-copy parsing, XPath 1.0, W3C XML Schema (XSD), DTD, and streaming pull/push APIs."
    topics = ("xml", "parser", "xpath", "xsd", "dtd", "streaming", "c++20")
    settings = "os", "compiler", "build_type", "arch"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "with_xpath": [True, False],
        "with_xsd": [True, False],
        "with_dtd": [True, False],
        "with_stringify": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "with_xpath": True,
        "with_xsd": True,
        "with_dtd": True,
        "with_stringify": True,
    }

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def layout(self):
        cmake_layout(self)

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["XML_LIB_ENABLE_XPATH"] = self.options.with_xpath
        tc.variables["XML_LIB_ENABLE_XSD"] = self.options.with_xsd
        tc.variables["XML_LIB_ENABLE_DTD"] = self.options.with_dtd
        tc.variables["XML_LIB_ENABLE_STRINGIFY"] = self.options.with_stringify
        tc.variables["BUILD_TESTING"] = False
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["XML_Lib"]
        self.cpp_info.set_property("cmake_file_name", "XML_Lib")
        self.cpp_info.set_property("cmake_target_name", "XML_Lib::XML_Lib")
