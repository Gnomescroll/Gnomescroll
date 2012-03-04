
#include <Python.h>

#include <stdio.h>

#include <SDL.h>


void exec_pycode(const char* code)
{
  //Py_Initialize();
  PyRun_SimpleString(code);
  //Py_Finalize();
}

void exec_interactive_interpreter(int argc, char** argv)
{
  Py_Initialize();
  Py_Main(argc, argv);
  Py_Finalize();
}

void exec_file(int argc, char** argv) {
    Py_Initialize();
    PySys_SetArgv(argc, argv);
    const char* pyversion = Py_GetVersion();
    printf("Python: %s\n", pyversion);
    // Get a reference to the main module.
    //PyObject* main_module = PyImport_AddModule("__main__");

    // Get the main module's dictionary
    // and make a copy of it.
    //PyObject* main_dict = PyModule_GetDict(main_module);
    //PyObject* main_dict_copy = PyDict_Copy(main_dict);

    // Execute two different files of
    // Python code in separate environments
    //exec_pycode("import os; import sys; print os.getcwd(); sys.path.insert(0, os.getcwd());");
    #if PRODUCTION
    char pyfile[] = "gameloop.pyc";
    #else
    char pyfile[] = "gameloop.py";
    #endif
    
    FILE* file_1 = fopen(pyfile, "r");
    //PyRun_File(file_1, "gameloop.py", Py_file_input, main_dict, main_dict);
    if (PyRun_SimpleFile(file_1, pyfile)) {
        fprintf(stderr, "Error executing gameloop.py.");
        fflush(stderr);
    }

    fclose(file_1);

    /*
    FILE* file_2 = fopen("file2.py", "r");
    PyRun_File(file_2, "file2.py",
               Py_file_input,
               main_dict_copy, main_dict_copy);
    */
    Py_Finalize();
}

int main(int argc, char** argv) {
    //char* args[1];
    //args[0] = "test";

    printf("Start\n");
    //exec_pycode("import os; import sys; print os.getcwd(); sys.path.append(os.getcwd());import args_client;args_client.main()");
    exec_file(argc, argv);
    //runniPyImport_Import("gameloop")
    //exec_pycode("import gameloop");
    //exec_interactive_interpreter(0, args);

    SDL_Quit();

    return 0;
}
