#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Python.h>

// in case you want to create your own exception object from c
static PyObject *SpamError;

// self points to module object or class instance
static PyObject * spam_system(PyObject *self, PyObject *args) // or third param PyObject *kwargs with METH_KEYWORDS def
{
  const char *command;
  int sts;
  
  if(!PyArg_ParseTuple(args, "s", &command))  // or PyArg_ParseTupleAndKeywords()
    return NULL;

  sts = system(command);
  printf("cmd: %s\n", command);
  if (sts < 0) {
    PyErr_SetString(SpamError, "System command failed");
    return NULL;
  }
  return Py_BuildValue("i", sts);
  // return PyLong_FromLong(sts);

  // if there isn't anything to return, you still need to return to python, None
  // Py_RETRUN_NONE;
  // same as
  // Py_INCREF(Py_None);
  // return Py_None;
}

static PyMethodDef SpamMethods[] = {
  {"system", spam_system, METH_VARARGS,
   "Execute a shell command."},
  {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC // void return type, and extern-ed for c++
initspam(void) // has to be named init<module>
{
  PyObject *m;
  m = Py_InitModule("spam", SpamMethods);
  if (m == NULL)  
    return; 

  SpamError = PyErr_NewException("spam.error", NULL, NULL);
  Py_INCREF(SpamError);
  PyModule_AddObject(m, "error", SpamError);
}

