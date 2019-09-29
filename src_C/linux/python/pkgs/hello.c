#include <Python.h>

static PyObject* hello(PyObject* self) {
   return Py_BuildValue("s", "Hello, Python extensions!!");
}

static char hello_docs[] =
   "helloworld( ): Any message you want to put here!!\n";
/*
static PyMethodDef helloworld_funcs[] = {
   {"helloworld", (PyCFunction)helloworld, 
      METH_NOARGS, helloworld_docs},
      {NULL}
};

void inithelloworld(void) {
   Py_InitModule3("helloworld", helloworld_funcs,
                  "Extension module example!");
}
*/
static PyMethodDef module_methods[]={
	{"hello",(PyCFunction) hello,
	METH_NOARGS,hello_docs},
	{NULL}
};
static struct PyModuleDef helloworld = 
{
	PyModuleDef_HEAD_INIT,
	"helloworld",
	"Usage: xxxxxxxxxxxx\n",
	-1,
	module_methods
};
PyMODINIT_FUNC PyInit_helloworld(void)
{
	return PyModule_Create(&helloworld);
}
