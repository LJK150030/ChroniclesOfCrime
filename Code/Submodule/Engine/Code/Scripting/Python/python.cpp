#include "Scripting/Python/python.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/EngineCommon.hpp"


PyObject* PyHookLog( PyObject* self, PyObject* args)
{
	UNUSED(self);

	int arg_count = (int)PyTuple_Size(args);
	if (arg_count > 0) {
		PyObject* arg0 = PyTuple_GetItem(args, 0);

		PyObject* str = nullptr;
		if (PyUnicode_CheckExact(arg0)) {
			str = PyUnicode_AsEncodedString(arg0, "utf-8", "~E~");
			
		}
		else {
			PyObject* repr = PyObject_Repr(arg0);
			str = PyUnicode_AsEncodedString(repr, "utf-8", "~E~");
			Py_DecRef(repr);
		}

		char const* bytes = PyBytes_AS_STRING(str);
		DebuggerPrintf(bytes);


		Py_DecRef(str);
	}

	Py_INCREF(Py_None);
	return Py_None;
}


static PyMethodDef MyPyMethods[] = 
{
	// python func name,     C Func to Call,     Type,           Help Message
	{ "log",                 PyHookLog,          METH_VARARGS,   "Logs using the system log." },
	{ nullptr,               nullptr,            0,              nullptr }  // terminator element; 
};


static PyModuleDef MyModule = 
{
	// base,                name,       documentation,    size,          methods,       slots,   tarverse_cb,   clear_cb,   free_cb
	PyModuleDef_HEAD_INIT,  "gh_engine",   nullptr,          -1,            MyPyMethods,   nullptr, nullptr,       nullptr,    nullptr
};


/*
 * import gh_engine
 * gh_engine.log( )
 */


static PyObject* MyModuleInit()
{
	return PyModule_Create(&MyModule);
}


void PythonSystemStartup()
{
	wchar_t const* program = L"TestProgram";

	// presetup
	Py_SetProgramName(program);
	Py_SetPath(L"./python37.zip");

	//Py_Initialize();
	PyImport_AppendInittab("gh_engine", &MyModuleInit);

	Py_Initialize();
	
	PyRun_SimpleString(
		"from time import time,ctime\n"
		"print('Today is', ctime(time()) )"
	);


	// hook up stdout to my log system;
	// "squire" is my engine's module 
	PyRun_SimpleString(
		"import sys\n"
		"import gh_engine\n"
		"class LogOutput:\n"
		"	def write(self, txt):\n"
		"		gh_engine.log(txt)\n"
		"	def flush(self):\n"
		"		pass\n"
		"logOutput = LogOutput()\n"
		"sys.stdout = logOutput\n"
		"sys.stderr = logOutput\n"
	);
}


void PythonSystemShutdown()
{
	Py_FinalizeEx();
}


void PythonTest()
{
	char const* filename = "test";
	char const* func_name = "SomeFunction";

	PyObject* py_name;
	PyObject* py_module;
	PyObject* py_func;

	// convert a locale filesystem string into a python string
	py_name = PyUnicode_DecodeFSDefault(filename);

	// load the file into an module
	py_module = PyImport_Import(py_name);
	if (py_module != nullptr) 
	{
		// successfully loaded
		// all code in the file would be "run" at this point

		// think of the entire file as an "object" or namespace
		// and a function in the file as one of its attributes
		// (any global variable can be accessed in this way)
		py_func = PyObject_GetAttrString(py_module, func_name);
		if (py_func != nullptr) 
		{
			// If you want to make sure it is callable
			if (PyCallable_Check(py_func)) 
			{
				//PyObject* args = Py_BuildValue("(ii)", 2, 32); essentialy what the lower code is doing.

				
				// make a python object to pass in the arguments (should be 
				// as what I need to pass in)
				PyObject* args = PyTuple_New(2);

				PyObject* value = PyLong_FromLong(32);
				PyTuple_SetItem(args, 0, value);  // this "steals" the refernce of value

				value = PyLong_FromLong(20);
				PyTuple_SetItem(args, 1, value);  // set the second argument

				// call the function with this arguments
				PyObject* returned = PyObject_CallObject(py_func, args);
				Py_DECREF(returned);

				// done using it, release the local reference to it
				Py_DECREF(args);    // args will release itself and all references it has
				Py_DECREF(py_func); // done using this function
			}

			Py_DECREF(py_module);
		}
	}

	// remove a reference from the object (potenetially freeing memory)
	Py_DECREF(py_name);

	// error checking
	if (PyErr_Occurred()) {
		PyErr_Print();
	}
}