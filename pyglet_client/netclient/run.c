
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


int main() {
    char* args[1];
    args[0] = "test";

    printf("Start\n");
    exec_pycode("import os; os.chdir(os.getcwd());import gameloop");
    //exec_interactive_interpreter(0, args);
    return 0;
}
