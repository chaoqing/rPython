#include <Python.h>
#include <string.h>

#ifndef _WIN32
#include <dlfcn.h>
#endif

#include <bytesobject.h>		// Port python 3

#define xstr(a) str(a)
#define str(a) #a

#if PY_MAJOR_VERSION >= 3
#define PY3K
#endif

void py_init(){

#ifndef _WIN32
    //dlopen( "libpython2.7.so.1.0", RTLD_NOW | RTLD_GLOBAL );
    dlopen( xstr(PYTHONLIBFILE), RTLD_NOW | RTLD_GLOBAL );		// Passed as a macro at compile time
#endif

    Py_Initialize();
    char startup[]=""
        "import json\n"
        "def object2json(obj):\n"
        "    obj_type = str(type(obj))\n"
        ""
        "    if obj_type in [\"<type 'numpy.ndarray'>\", \"<type 'list'>\", \"<type 'set'>\"]:\n"
        "        return '['+(','.join([object2json(x) for x in obj]))+']'\n"
        ""
        "    elif obj_type == \"<type 'datetime.datetime'>\":\n"
        "        return json.dumps(str(obj))\n"
        ""
        "    elif obj_type == \"<class 'pandas.core.series.Series'>\":\n"
        "        return object2json(obj.values)\n"
        ""
        "    elif obj_type in [\"<class 'pandas.core.frame.DataFrame'>\",\"<type 'dict'>\"]:\n"
        "        return '{'+(','.join([object2json(col)+':'+object2json(obj[col]) for col in obj]))+'}'\n"
        ""
        "    elif obj_type == \"<type 'tuple'>\":\n"
        "        if len(set(map(type, obj))) == 1:\n"
        "            return object2json(list(obj))\n"
        "        else:\n"
        "            return object2json(dict(zip(map(str,range(len(obj))), obj)))\n"
        ""
        "    else:\n"
        "        try:\n"
        "            j = json.dumps(obj)\n"
        "        except:\n"
        "            j = str(obj)\n"
        "        return j\n"
        "";
    PyRun_SimpleString(startup);
}

void py_close(){
    Py_Finalize();
}


void py_exec_code(const char** code, int* exit_status )
{
    /**exit_code = PyRun_SimpleString(*code); */
     *exit_status = PyRun_SimpleString(*code); 
}


/*void exec_pycode(const char** code, int* exit_code, char** resultado ) */
void py_get_var( const char** var_name, int* found, char** resultado )
{

    *found = 0;

    PyObject * module     = PyImport_AddModule("__main__");
    PyObject * dictionary = PyModule_GetDict(module);
    PyObject * result     = PyDict_GetItemString(dictionary, *var_name );

    if( result == NULL ){
        *found = 1;
        *resultado = "";
        return;
    }

#ifdef PY3K
     *resultado = PyBytes_AS_STRING( PyUnicode_AsUTF8String(result) );
#else
    *resultado = PyString_AS_STRING(result);
#endif
    // *resultado = PyUnicode_AsUTF8(result);				// Python 3?
}
