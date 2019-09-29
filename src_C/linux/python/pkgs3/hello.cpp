#include <Python.h>
#include <iostream>
#include <thread>
#include <mutex>

std::once_flag flag1;

void simple_do_once()
{
    std::thread::id this_id = std::this_thread::get_id();
    std::cout <<"Thread:"<<this_id<< "Simple example: called once\n";
}


void do_once(bool do_flag)
{
    std::thread::id this_id = std::this_thread::get_id();
    std::cout<<"Thread:"<<this_id<<" invoked!\n";
    std::call_once(flag1, simple_do_once);
}

static PyObject* hello(PyObject* self) {
    std::thread t1(do_once, true);
    std::thread t2(do_once, true);
    std::thread t3(do_once, false);
    std::thread t4(do_once, true);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
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
