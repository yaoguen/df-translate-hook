#include "pch.h"
#include "Python.h"
#include "changetext.h"
#include <cstring>

#define ERROR_MESSAGE(message) MessageBox(0,message,"ChangeText",MB_ICONERROR)

PyObject * pModule = nullptr,
         * pfuncChangeText = nullptr,
         * pArgs = nullptr;

static int initialized = 0;

EXPORT int Init() {
    Py_Initialize();
    PyRun_SimpleString("import sys\nsys.stderr = open('changetext.err', 'a', 1, encoding='utf-8')\nsys.stdout = open('changetext.log', 'a', 1, encoding='utf-8')");
    pModule = PyImport_ImportModule("changetext");
    if(pModule) {
        pfuncChangeText = PyObject_GetAttrString(pModule, "ChangeText");
        if(!(pfuncChangeText && PyCallable_Check(pfuncChangeText))) {
            Py_XDECREF(pfuncChangeText);
            pfuncChangeText = nullptr;
            ERROR_MESSAGE("Error: Probably changetext.py module doesn't contain ChangeText function.\n");
        }
        else pArgs = PyTuple_New(1);
    }
    else {
        PyErr_PrintEx(1);
        ERROR_MESSAGE("Error: Failed to import changetext.py module.\nSee changetext.err for details.\n");
    }

    initialized = 1; // At least tried to initialize
    return pfuncChangeText!= nullptr;
}

size_t my_strlen(uint16_t * s) {
    size_t i;
    for(i=0; s[i]; i++);
    return i*sizeof(uint16_t);
}

size_t my_strlen16(uint16_t * s) {
    size_t i;
    for(i=0; s[i]; i++);
    return i;
}

/* Функция, получающая исходный текст и передающая его на обработку скрипту*/
EXPORT uint16_t * ChangeText(uint16_t * src) {
    static PyObject * pValue = nullptr;

    if(!initialized) Init();

    if(pfuncChangeText && pArgs) {
        PyObject* bytesUtf16 = PyBytes_FromStringAndSize((char*)src, my_strlen(src));
        PyTuple_SetItem(pArgs, 0, bytesUtf16);
        Py_XDECREF(pValue);
        pValue = PyObject_CallObject(pfuncChangeText, pArgs);
        if(pValue == Py_None)
        {
            return src;
        }

        if(pValue)
            return (uint16_t*)PyBytes_AS_STRING(pValue);
    	
        PyErr_PrintEx(1);
        Py_XDECREF(pValue);
        return nullptr;
    }
	
    return nullptr;
}
