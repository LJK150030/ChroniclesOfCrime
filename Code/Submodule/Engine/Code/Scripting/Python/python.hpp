#pragma once
#include "ThirdParty\Python\Python.h"

void PythonSystemStartup();
void PythonSystemShutdown();
PyObject* PyHookLog(PyObject* self, PyObject* args);