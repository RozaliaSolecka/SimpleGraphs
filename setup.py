from distutils.core import setup, Extension

if __name__ == '__main__':

    setup(name="simple_graphs", version="1.0", ext_modules=[Extension("simple_graphs", ["SimpleGraphs.cpp"])])
