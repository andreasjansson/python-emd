Earth Mover's Distance in Python
================================

This is a Python wrapper around Rubner's Earth Mover's Distance implementation (http://ai.stanford.edu/~rubner/emd/default.htm).

Example Usage
-------------

    >>> import emd
    >>> emd.emd(range(5), range(5), [0, 1, 0, 0, 0], [0, 0, 0, 0, 1])
    3.0

Installation
------------

    sudo python setup.py install

NB: The module has only been tested on Linux and doesn't appear to be working on Windows. I'll try to fix that soon.

References
----------

http://homepages.inf.ed.ac.uk/rbf/CVonline/LOCAL_COPIES/RUBNER/emd.htm
