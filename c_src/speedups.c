
#include "speedups.h"
#include "request.h"

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif

PyObject* SimpleHTTPModule = NULL;

static PyMethodDef simplehttp_methods[] = {
    {"c_parse_request", (PyCFunction) parse_request, METH_VARARGS,
        "Parse an HTTP request."},
    {NULL}
};

PyMODINIT_FUNC
initspeedups(void)
{
    PyObject* m;
    const char* moddoc = "Speedups for parsing HTTP messages.";
    m = Py_InitModule3("speedups", simplehttp_methods, moddoc);
    if(m == NULL) return;
    SimpleHTTPModule = m;
}