#include <Python.h>
#include "emd.h"

// define PyInt_* macros for Python 3.x
#ifndef PyInt_Check
#define PyInt_Check             PyLong_Check
#define PyInt_FromLong          PyLong_FromLong
#define PyInt_AsLong            PyLong_AsLong
#define PyInt_Type              PyLong_Type
#endif


float double_dist(feature_t *f1, feature_t *f2)
{
  double d = *f1 - *f2;
  return sqrt(d * d);
}

static PyObject *compute_emd(PyObject *self, PyObject *args, PyObject *keywds)
{
  static char *kwlist[] = {"feature1", "feature2", "weight1", "weight2",
                           "return_flows",
                           NULL};

  PyObject *feature1, *feature2, *weight1, *weight2;
  double *f1, *f2;
  float *w1, *w2;
  int length1, length2;
  signature_t signature1, signature2;
  flow_t *flow;
  float distance;
  PyObject *item;
  int i, flow_size;
  unsigned short int return_flows = 0;

  if(!PyArg_ParseTupleAndKeywords(args, keywds, "OOOO|H", kwlist,
                                  &feature1, &feature2, &weight1, &weight2, &return_flows))
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

  if (return_flows) {
    flow = alloca((length1 + length2 - 1) * sizeof(flow_t));
  } else {
    flow = NULL;
  }

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

  distance = emd(&signature1, &signature2, double_dist, flow, &flow_size);

  if (return_flows) {
    PyObject *returnTuple = PyTuple_New(2);
    PyTuple_SetItem(returnTuple, 0, Py_BuildValue("d", distance));

    PyObject *flowList = PyList_New((Py_ssize_t) flow_size);
    for(i = 0; i < flow_size; i++) {
      PyObject *tuple = PyTuple_New(3);
      PyTuple_SetItem(tuple, 0, Py_BuildValue("I", flow[i].from));
      PyTuple_SetItem(tuple, 1, Py_BuildValue("I", flow[i].to));
      PyTuple_SetItem(tuple, 2, Py_BuildValue("d", flow[i].amount));
      PyList_SetItem(flowList, i, tuple);
    }
    PyTuple_SetItem(returnTuple, 1, flowList);
    return returnTuple;

  }
  return Py_BuildValue("d", distance);
}

static PyMethodDef functions[] = {
    {"emd", (PyCFunction)compute_emd, METH_VARARGS | METH_KEYWORDS,
     "Compute the Earth Mover's Distance."},
    {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION >= 3
//-----------------------------------------------------------------------------
//   Declaration of module definition for Python 3.x.
//-----------------------------------------------------------------------------
static struct PyModuleDef g_ModuleDef = {
    PyModuleDef_HEAD_INIT,
    "emd",
    NULL,
    -1,
    functions,                             // methods
    NULL,                                  // m_reload
    NULL,                                  // traverse
    NULL,                                  // clear
    NULL                                   // free
};
#endif



#if PY_MAJOR_VERSION >= 3
PyObject * PyInit_emd(void)
{
  return PyModule_Create(&g_ModuleDef);
#else
PyMODINIT_FUNC initemd(void)
{
    Py_InitModule(
        "emd", functions
        );
#endif
}
