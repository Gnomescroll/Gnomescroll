
#include <Python.h>

#include <stdio.h>



void exec_pycode(const char* code)
{
  Py_Initialize();
  PyRun_SimpleString(code);
  Py_Finalize();
}

void exec_interactive_interpreter(int argc, char** argv)
{
  Py_Initialize();
  Py_Main(argc, argv);
  Py_Finalize();
}

void exec_file() {
  Py_Initialize();

// Get a reference to the main module.
PyObject* main_module = PyImport_AddModule("__main__");

// Get the main module's dictionary
// and make a copy of it.
PyObject* main_dict = PyModule_GetDict(main_module);
//PyObject* main_dict_copy = PyDict_Copy(main_dict);

// Execute two different files of
// Python code in separate environments
FILE* file_1 = fopen("gameloop.py", "r");
PyRun_File(file_1, "gameloop.py", Py_file_input, main_dict, main_dict);

/*
FILE* file_2 = fopen("file2.py", "r");
PyRun_File(file_2, "file2.py",
           Py_file_input,
           main_dict_copy, main_dict_copy);
*/
    Py_Finalize();
}

int main() {
    char* args[1];
    args[0] = "test";

    printf("Start\n");
    exec_pycode("import os; import sys; print os.getcwd(); sys.path.append(os.getcwd());import gameloop;");
    //exec_file();
    //runniPyImport_Import("gameloop")
    //exec_pycode("import gameloop");
    //exec_interactive_interpreter(0, args);
    return 0;
}
