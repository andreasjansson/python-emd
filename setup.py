from distutils.core import setup, Extension

setup(
    name="emd",
    ext_modules = [Extension("emd", ["pyemd.c", "emd.c"], extra_compile_args = ['-g'])],
    author = 'Andreas Jansson',
    author_email = 'andreas@jansson.me.uk',
    description = ("Python wrapper around Yossi Rubner's Earth Mover's Distance implementation (http://ai.stanford.edu/~rubner/emd/default.htm)"),
    license = 'GPL v3',
    keywords = 'emd distance',
    
    )
