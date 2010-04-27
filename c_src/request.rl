
#include "request.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    PyObject*   method;
    PyObject*   uri;
    PyObject*   scheme;
    PyObject*   host;
    int         port;
    PyObject*   path;
    PyObject*   query;
    PyObject*   fragment;
    int         vsn_major;
    int         vsn_minor;
    PyObject*   version;
    
    PyObject*   headers;
    PyObject*   hdr_name;
    int         continued;

    PyObject*   reader;
    PyObject*   data;
    char*       buffer;
    size_t      buflen;
    size_t      nread;
    int         cs;
    
    buffer_t*   genbuf;
    buffer_t*   uribuf;
} request_t;

%%{
    machine http_req_parser;

    action mark {
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = fpc;
    }

    action write_method {
        request->method = buffer_to_string(request->genbuf, fpc);
        if(request->method == NULL) fbreak;
    }

    action write_scheme {
        request->scheme = buffer_to_string(request->genbuf, fpc);
        if(request->scheme == NULL) fbreak;
    }

    action write_host {
        request->host = buffer_to_string(request->genbuf, fpc);
        if(request->host == NULL) fbreak;
    }

    action mark_port {
        request->port = 0;
    }
    
    action write_port {
        request->port = request->port*10 + ((*fpc)-'0');
    }
    
    action write_path {
        request->path = buffer_to_string(request->genbuf, fpc);
        if(request->path == NULL) fbreak;
    }

    action write_query {
        request->query = buffer_to_string(request->genbuf, fpc);
        if(request->query == NULL) fbreak;
    }

    action write_fragment {
        request->fragment = buffer_to_string(request->genbuf, fpc);
        if(request->fragment == NULL) fbreak;
    }

    action mark_uri {
        assert(request->uribuf->pos == NULL && "wont overwrite uri mark");
        request->uribuf->pos = fpc;
    }

    action write_uri {
        request->uri = buffer_to_string(request->uribuf, fpc);
        if(request->fragment == NULL) fbreak;
    }

    action start_major {
        request->vsn_major = 0;
    }
    
    action write_major {
        request->vsn_major = request->vsn_major*10 + ((*fpc)-'0');
    }
    
    action start_minor {
        request->vsn_minor = 0;
    }
    
    action write_minor {
        request->vsn_minor = request->vsn_minor*10 + ((*fpc)-'0');
    }

    action mark_name {
        assert(request->hdr_name == NULL && "header name already marked");        
        assert(request->genbuf->pos == NULL && "wont overwrite a mark");
        request->genbuf->pos = fpc;
    }
    
    action write_name {
        request->hdr_name = buffer_to_string(request->genbuf, fpc);
        if(request->hdr_name == NULL) fbreak;
    }
    
    action mark_value {
        assert(request->hdr_name != NULL && "value must have a name");
        assert(request->continued ||
                    (request->genbuf->pos == NULL && "wont overwrite a mark"));
        
        if(request->continued)
        {
            request->continued = 1;
        }
        else
        {
            request->genbuf->pos = fpc;
        }
    }
    
    action maybe_write_value {
        if(*fpc == ' ' || *fpc == '\t')
        {
            request->continued = 1;
            fhold;
            fgoto header_value;
        }
        else if(*fpc == '\r')
        {
            if(!append_header(request, fpc)) fbreak;
            fhold;
            fgoto done;
        }
        else
        {
            if(!append_header(request, fpc)) fbreak;
            fhold;
            fgoto header_name;
        }
    }

    action done {
        build_version(request);
        fbreak;
    }

    include http "http.rl";
    
    main := request;
}%%

%% write data;

int
append_header(request_t* request, const char* ptr)
{
    PyObject* tuple = NULL;
    PyObject* val = NULL;

    assert(request->hdr_name != NULL && "invalid internal state: no hdr_name");
    assert(request->headers != NULL && "invalid internal state: no headers");

    tuple = PyTuple_New(2);
    if(tuple == NULL)
    {
        PyErr_NoMemory();
        goto error;
    }
    
    val = buffer_to_trimmed_string(request->genbuf, ptr);
    if(val == NULL) goto error;

    PyTuple_SET_ITEM(tuple, 0, request->hdr_name);
    PyTuple_SET_ITEM(tuple, 1, val);
    
    request->hdr_name = NULL;
    val = NULL;
    
    if(PyList_Append(request->headers, tuple) != 0) goto error;
    
    return 1;

error:
    Py_XDECREF(tuple);
    Py_XDECREF(val);
    return 0;
}

int
build_version(request_t* request)
{
    PyObject* major = NULL;
    PyObject* minor = NULL;
    PyObject* tuple = NULL;
    
    major = PyInt_FromLong(request->vsn_major);
    if(major == NULL) goto error;
    
    minor = PyInt_FromLong(request->vsn_minor);
    if(minor == NULL) goto error;
    
    tuple = PyTuple_New(2);
    if(tuple == NULL) goto error;
    
    PyTuple_SET_ITEM(tuple, 0, major);
    PyTuple_SET_ITEM(tuple, 1, minor);
    
    // SET_ITEM steals
    major = NULL;
    minor = NULL;
    
    request->version = tuple;
    
    return 1;
    
error:
    Py_XDECREF(major);
    Py_XDECREF(minor);
    Py_XDECREF(tuple);
    return 0;
}

int
fill_buffer(request_t* request, char** buffer, size_t* length)
{
    PyObject* args = NULL;
    Py_ssize_t tmplen = 0;
    int ret = -1;
    
    if(request->buffer != NULL)
    {
        *buffer = request->buffer;
        *length = request->buflen;
        request->buffer = NULL;
        request->buflen = 0;
        ret = 1;
    }
    else
    {
        Py_XDECREF(request->data);
        request->data = NULL;
        
        args = Py_BuildValue("()");
        request->data = PyObject_CallObject(request->reader, args);

        if(request->data == NULL) goto done;
        
        if(PyString_AsStringAndSize(request->data, buffer, &tmplen) < 0)
        {
            *buffer = NULL;
            *length = 0;
            goto done;
        }

        *length = (size_t) tmplen;
        if(tmplen == 0)
        {
            ret = 0;
        }
        else
        {
            ret = 1;
        }        
    }

done:
    Py_XDECREF(args);
    return ret;
}    

void
save_chunk(request_t* req, char* buffer, size_t length)
{
    req->buffer = buffer;
    req->buflen = length;
}

int
do_parse(request_t* request)
{
    int started = 0;
    int status;
    char* buffer;
    size_t length;
    char* p;
    char* pe;
    int cs;
    int gensaved = 0;
    int urisaved = 0;
    
    do
    {
        // Get a chunk of data to parse.
        status = fill_buffer(request, &buffer, &length);
        if(status < 0 || (status == 0 && started)) return -1;
        if(status == 0) return 0;
        started = 1;

        // If we saved some state in the buffers we need
        // to reinitialize them before resuming parsing.

        if(gensaved) reinit_buffer(request->genbuf, buffer);
        gensaved = 0;
        
        if(urisaved) reinit_buffer(request->uribuf, buffer);
        urisaved = 0;

        // Setup and run the main parse loop saving the
        // parse state afterwards.

        cs = request->cs;
        p = buffer;
        pe = buffer + length;

        %% write exec;

        if(PyErr_Occurred())
        {
            return -1;
        }

        request->cs = cs;
        request->nread += p - buffer;
        
        if(p < pe) save_chunk(request, p, pe-p);
        
        // Parsing stopped in the middle of a buffer
        // state. Save the current data and prepare
        // for reinitialization.
        
        gensaved = request->genbuf->pos ? 1 : 0;
        if(gensaved) save_buffer(request->genbuf, p);
        
        urisaved = request->uribuf->pos ? 1 : 0;
        if(urisaved) save_buffer(request->uribuf, p);

    } while(cs != http_req_parser_error && cs < http_req_parser_first_final);

    if(cs == http_req_parser_error)
    {
        if(!PyErr_Occurred())
        {
            // MAKE MOAR BUTTAH
            PyErr_SetString(PyExc_ValueError, "Failed to parse data stream.");
            return -1;
        }
    }

    return 1;
}

request_t*
create_request(PyObject* reader)
{
    int cs = 0;

    if(reader == NULL)
    {
        PyErr_SetString(PyExc_ValueError, "Reader must not be NULL.");
        return NULL;
    }

    request_t* ret = (request_t*) malloc(sizeof(request_t));
    if(ret == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    memset(ret, '\0', sizeof(request_t));
    ret->port = 80;
    ret->reader = reader;
    
    ret->headers = PyList_New(0);
    if(ret->headers == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }
    
    ret->genbuf = init_buffer(64*1024);
    if(ret->genbuf == NULL)
    {
        free(ret);
        PyErr_NoMemory();
        return NULL;
    }
    
    ret->uribuf = init_buffer(64*1024);
    if(ret->uribuf == NULL)
    {
        free_buffer(ret->genbuf);
        free(ret);
        PyErr_NoMemory();
        return NULL;
    }

    %% write init;
    
    ret->cs = cs;
    
    return ret;
}

void
destroy_request(request_t* req)
{
    if(req == NULL) return;
   
    Py_XDECREF(req->method);
    Py_XDECREF(req->uri);
    Py_XDECREF(req->scheme);
    Py_XDECREF(req->host);
    Py_XDECREF(req->path);
    Py_XDECREF(req->query);
    Py_XDECREF(req->fragment);
    Py_XDECREF(req->version);
    Py_XDECREF(req->hdr_name);
    Py_XDECREF(req->headers);
    Py_XDECREF(req->reader);
    Py_XDECREF(req->data);

    free_buffer(req->genbuf);
    free_buffer(req->uribuf);
    
    free(req);
};

int
move_object(PyObject* self, char* name, PyObject** value)
{
    if(*value == NULL)
    {
        if(PyObject_SetAttrString(self, name, Py_None) != 0)
        {
            return -1;
        }
        return 0;
    }
    else if(PyObject_SetAttrString(self, name, *value) != 0)
    {
        return -1;
    }
    Py_DECREF(*value);
    *value = NULL;
    return 0;
}

PyObject*
parse_request(PyObject* ignored, PyObject* args)
{
    PyObject* self;
    PyObject* source;
    PyObject* port = NULL;
    PyObject* ret = NULL;
    int status;

    if(!PyArg_ParseTuple(args, "OO", &self, &source)) return NULL;

    request_t* req = create_request(source);
    if(req == NULL) goto done;

    status = do_parse(req);
    if(status < 0) goto done;
    if(status == 0)
    {
        destroy_request(req);
        Py_RETURN_NONE;
    }
    
    // Move objects to self

    if(move_object(self, "method", &(req->method)) != 0) goto done;
    if(move_object(self, "uri", &(req->uri)) != 0) goto done;
    if(move_object(self, "scheme", &(req->scheme)) != 0) goto done;
    if(move_object(self, "host", &(req->host)) != 0) goto done;
    
    port = PyInt_FromLong(req->port);
    if(port == NULL) goto done;
    if(move_object(self, "port", &port) != 0) goto done;

    if(move_object(self, "path", &(req->path)) != 0) goto done;
    if(move_object(self, "query", &(req->query)) != 0) goto done;
    if(move_object(self, "fragment", &(req->fragment)) != 0) goto done;
    if(move_object(self, "version", &(req->version)) != 0) goto done;
    if(move_object(self, "headers", &(req->headers)) != 0) goto done;

    if(req->buflen > 0)
    {
        ret = PyString_FromStringAndSize(req->buffer, req->buflen);
    }
    else
    {
        ret = PyString_FromString("");
    }

done:
    Py_XDECREF(port);
    if(req != NULL) destroy_request(req);
    return ret;
}

