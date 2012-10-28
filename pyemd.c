#include <Python.h>
#include "emd.h"

float double_dist(feature_t *f1, feature_t *f2)
{
  double d = *f1 - *f2;
  return sqrt(d * d);
}

static PyObject *compute_emd(PyObject *self, PyObject *args, PyObject *keywds)
{
  static char *kwlist[] = {"feature1", "feature2", "weight1", "weight2",
                           NULL};

  PyObject *feature1, *feature2, *weight1, *weight2;
  double *f1, *f2;
  float *w1, *w2;
  int length1, length2;
  signature_t signature1, signature2;
  float distance;
  PyObject *item;
  int i;

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "OOOO", kwlist,
                                  &feature1, &feature2, &weight1, &weight2))
     return NULL;

  if(!PySequence_Check(feature1)) {
    PyErr_SetString(PyExc_TypeError, "feature1 must be a sequence");
    return NULL;
  }

  if(!PySequence_Check(feature2)) {
    PyErr_SetString(PyExc_TypeError, "feature2 must be a sequence");
    return NULL;
  }

  if(!PySequence_Check(weight1)) {
    PyErr_SetString(PyExc_TypeError, "weight1 must be a sequence");
    return NULL;
  }

  if(!PySequence_Check(weight2)) {
    PyErr_SetString(PyExc_TypeError, "weight2 must be a sequence");
    return NULL;
  }

  length1 = PySequence_Size(feature1);
  length2 = PySequence_Size(feature2);

  if(PySequence_Size(weight1) != length1) {
    PyErr_SetString(PyExc_TypeError, "feature1 and weight1 must be the same");
    return NULL;
  }

  if(PySequence_Size(weight2) != length2) {
    PyErr_SetString(PyExc_TypeError, "feature2 and weight2 must be the same");
    return NULL;
  }

  f1 = alloca(length1 * sizeof(double));
  f2 = alloca(length2 * sizeof(double));
  w1 = alloca(length1 * sizeof(double));
  w2 = alloca(length2 * sizeof(double));

  for(i = 0; i < length1; i ++) {
    item = PySequence_GetItem(feature1, i);
    if(!PyFloat_Check(item) && !PyInt_Check(item)) {
      Py_DECREF(item);
      PyErr_SetString(PyExc_TypeError, "f1 should be a sequence of numbers");
      return NULL;
    }
    f1[i] = PyFloat_AsDouble(item);
    Py_DECREF(item);

    item = PySequence_GetItem(weight1, i);
    if(!PyFloat_Check(item) && !PyInt_Check(item)) {
      Py_DECREF(item);
      PyErr_SetString(PyExc_TypeError, "w1 should be a sequence of numbers");
      return NULL;
    }
    w1[i] = PyFloat_AsDouble(item);
    Py_DECREF(item);
  }

  for(i = 0; i < length2; i ++) {
    item = PySequence_GetItem(feature2, i);
    if(!PyFloat_Check(item) && !PyInt_Check(item)) {
      Py_DECREF(item);
      PyErr_SetString(PyExc_TypeError, "f2 should be a sequence of numbers");
      return NULL;
    }
    f2[i] = PyFloat_AsDouble(item);
    Py_DECREF(item);

    item = PySequence_GetItem(weight2, i);
    if(!PyFloat_Check(item) && !PyInt_Check(item)) {
      Py_DECREF(item);
      PyErr_SetString(PyExc_TypeError, "w2 should be a sequence of numbers");
      return NULL;
    }
    w2[i] = PyFloat_AsDouble(item);
    Py_DECREF(item);
  }

  signature1.n = length1;
  signature1.Features = f1;
  signature1.Weights = w1;

  signature2.n = length2;
  signature2.Features = f2;
  signature2.Weights = w2;

  distance = emd(&signature1, &signature2, double_dist, 0, 0);

  return Py_BuildValue("d", distance);
}

static PyMethodDef functions[] = {
    {"emd", (PyCFunction)compute_emd, METH_VARARGS | METH_KEYWORDS,
     "Compute the Earth Mover's Distance."},
    {NULL, NULL, 0, NULL}
};

PyMODINIT_FUNC initemd(void)
{
    Py_InitModule(
        "emd", functions
        );
}
