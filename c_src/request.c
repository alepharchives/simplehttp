
#line 1 "./c_src/request.rl"

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


#line 163 "./c_src/request.rl"



#line 44 "./c_src/request.c"
static const int http_req_parser_start = 1;
static const int http_req_parser_first_final = 69;
static const int http_req_parser_error = 0;

static const int http_req_parser_en_main = 1;
static const int http_req_parser_en_main_request_header_header_name = 67;
static const int http_req_parser_en_main_request_header_header_value = 20;
static const int http_req_parser_en_main_request_done = 68;


#line 166 "./c_src/request.rl"

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

        
#line 219 "./c_src/request.c"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	switch( (*p) ) {
		case 36: goto tr0;
		case 95: goto tr0;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto tr0;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto tr0;
	} else
		goto tr0;
	goto st0;
st0:
cs = 0;
	goto _out;
tr0:
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 253 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st48;
		case 95: goto st48;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st48;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st48;
	} else
		goto st48;
	goto st0;
tr2:
#line 44 "./c_src/request.rl"
	{
        request->method = buffer_to_string(request->genbuf, p);
        if(request->method == NULL) {p++; cs = 3; goto _out;}
    }
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 280 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto tr4;
		case 32: goto tr4;
		case 35: goto tr5;
		case 42: goto tr6;
		case 43: goto tr7;
		case 47: goto tr8;
		case 63: goto tr9;
	}
	if ( (*p) < 65 ) {
		if ( 45 <= (*p) && (*p) <= 57 )
			goto tr7;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr7;
	} else
		goto tr7;
	goto st0;
tr4:
#line 82 "./c_src/request.rl"
	{
        assert(request->uribuf->pos == NULL && "wont overwrite uri mark");
        request->uribuf->pos = p;
    }
#line 87 "./c_src/request.rl"
	{
        request->uri = buffer_to_string(request->uribuf, p);
        if(request->fragment == NULL) {p++; cs = 4; goto _out;}
    }
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 315 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto tr4;
		case 32: goto tr4;
		case 35: goto tr5;
		case 42: goto tr6;
		case 43: goto tr7;
		case 47: goto tr8;
		case 63: goto tr9;
		case 72: goto tr10;
	}
	if ( (*p) < 65 ) {
		if ( 45 <= (*p) && (*p) <= 57 )
			goto tr7;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr7;
	} else
		goto tr7;
	goto st0;
tr5:
#line 82 "./c_src/request.rl"
	{
        assert(request->uribuf->pos == NULL && "wont overwrite uri mark");
        request->uribuf->pos = p;
    }
	goto st5;
tr51:
#line 54 "./c_src/request.rl"
	{
        request->host = buffer_to_string(request->genbuf, p);
        if(request->host == NULL) {p++; cs = 5; goto _out;}
    }
	goto st5;
tr57:
#line 67 "./c_src/request.rl"
	{
        request->path = buffer_to_string(request->genbuf, p);
        if(request->path == NULL) {p++; cs = 5; goto _out;}
    }
	goto st5;
tr63:
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
#line 72 "./c_src/request.rl"
	{
        request->query = buffer_to_string(request->genbuf, p);
        if(request->query == NULL) {p++; cs = 5; goto _out;}
    }
	goto st5;
tr67:
#line 72 "./c_src/request.rl"
	{
        request->query = buffer_to_string(request->genbuf, p);
        if(request->query == NULL) {p++; cs = 5; goto _out;}
    }
	goto st5;
tr71:
#line 59 "./c_src/request.rl"
	{
        request->port = 0;
    }
	goto st5;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
#line 385 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto tr11;
		case 32: goto tr11;
		case 37: goto tr13;
		case 61: goto tr12;
		case 95: goto tr12;
	}
	if ( (*p) < 44 ) {
		if ( (*p) > 34 ) {
			if ( 36 <= (*p) && (*p) <= 42 )
				goto tr12;
		} else if ( (*p) >= 33 )
			goto tr12;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto tr12;
		} else if ( (*p) >= 64 )
			goto tr12;
	} else
		goto tr12;
	goto st0;
tr11:
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
#line 77 "./c_src/request.rl"
	{
        request->fragment = buffer_to_string(request->genbuf, p);
        if(request->fragment == NULL) {p++; cs = 6; goto _out;}
    }
#line 87 "./c_src/request.rl"
	{
        request->uri = buffer_to_string(request->uribuf, p);
        if(request->fragment == NULL) {p++; cs = 6; goto _out;}
    }
	goto st6;
tr39:
#line 77 "./c_src/request.rl"
	{
        request->fragment = buffer_to_string(request->genbuf, p);
        if(request->fragment == NULL) {p++; cs = 6; goto _out;}
    }
#line 87 "./c_src/request.rl"
	{
        request->uri = buffer_to_string(request->uribuf, p);
        if(request->fragment == NULL) {p++; cs = 6; goto _out;}
    }
	goto st6;
tr43:
#line 87 "./c_src/request.rl"
	{
        request->uri = buffer_to_string(request->uribuf, p);
        if(request->fragment == NULL) {p++; cs = 6; goto _out;}
    }
	goto st6;
tr50:
#line 54 "./c_src/request.rl"
	{
        request->host = buffer_to_string(request->genbuf, p);
        if(request->host == NULL) {p++; cs = 6; goto _out;}
    }
#line 87 "./c_src/request.rl"
	{
        request->uri = buffer_to_string(request->uribuf, p);
        if(request->fragment == NULL) {p++; cs = 6; goto _out;}
    }
	goto st6;
tr55:
#line 67 "./c_src/request.rl"
	{
        request->path = buffer_to_string(request->genbuf, p);
        if(request->path == NULL) {p++; cs = 6; goto _out;}
    }
#line 87 "./c_src/request.rl"
	{
        request->uri = buffer_to_string(request->uribuf, p);
        if(request->fragment == NULL) {p++; cs = 6; goto _out;}
    }
	goto st6;
tr61:
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
#line 72 "./c_src/request.rl"
	{
        request->query = buffer_to_string(request->genbuf, p);
        if(request->query == NULL) {p++; cs = 6; goto _out;}
    }
#line 87 "./c_src/request.rl"
	{
        request->uri = buffer_to_string(request->uribuf, p);
        if(request->fragment == NULL) {p++; cs = 6; goto _out;}
    }
	goto st6;
tr65:
#line 72 "./c_src/request.rl"
	{
        request->query = buffer_to_string(request->genbuf, p);
        if(request->query == NULL) {p++; cs = 6; goto _out;}
    }
#line 87 "./c_src/request.rl"
	{
        request->uri = buffer_to_string(request->uribuf, p);
        if(request->fragment == NULL) {p++; cs = 6; goto _out;}
    }
	goto st6;
tr70:
#line 59 "./c_src/request.rl"
	{
        request->port = 0;
    }
#line 87 "./c_src/request.rl"
	{
        request->uri = buffer_to_string(request->uribuf, p);
        if(request->fragment == NULL) {p++; cs = 6; goto _out;}
    }
	goto st6;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
#line 512 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto st6;
		case 32: goto st6;
		case 72: goto st7;
	}
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	if ( (*p) == 84 )
		goto st8;
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( (*p) == 84 )
		goto st9;
	goto st0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	if ( (*p) == 80 )
		goto st10;
	goto st0;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	if ( (*p) == 47 )
		goto st11;
	goto st0;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr20;
	goto st0;
tr20:
#line 92 "./c_src/request.rl"
	{
        request->vsn_major = 0;
    }
#line 96 "./c_src/request.rl"
	{
        request->vsn_major = request->vsn_major*10 + ((*p)-'0');
    }
	goto st12;
tr22:
#line 96 "./c_src/request.rl"
	{
        request->vsn_major = request->vsn_major*10 + ((*p)-'0');
    }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 574 "./c_src/request.c"
	if ( (*p) == 46 )
		goto st13;
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr22;
	goto st0;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr23;
	goto st0;
tr23:
#line 100 "./c_src/request.rl"
	{
        request->vsn_minor = 0;
    }
#line 104 "./c_src/request.rl"
	{
        request->vsn_minor = request->vsn_minor*10 + ((*p)-'0');
    }
	goto st14;
tr26:
#line 104 "./c_src/request.rl"
	{
        request->vsn_minor = request->vsn_minor*10 + ((*p)-'0');
    }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
#line 607 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto st15;
		case 13: goto st16;
		case 32: goto st15;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr26;
	goto st0;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	switch( (*p) ) {
		case 9: goto st15;
		case 13: goto st16;
		case 32: goto st15;
	}
	goto st0;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	if ( (*p) == 10 )
		goto st17;
	goto st0;
tr38:
#line 134 "./c_src/request.rl"
	{
        if(*p == ' ' || *p == '\t')
        {
            request->continued = 1;
            p--;
            {goto st20;}
        }
        else if(*p == '\r')
        {
            if(!append_header(request, p)) {p++; cs = 17; goto _out;}
            p--;
            {goto st68;}
        }
        else
        {
            if(!append_header(request, p)) {p++; cs = 17; goto _out;}
            p--;
            {goto st67;}
        }
    }
	goto st17;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
#line 660 "./c_src/request.c"
	switch( (*p) ) {
		case 13: goto st18;
		case 33: goto tr29;
		case 124: goto tr29;
		case 126: goto tr29;
	}
	if ( (*p) < 45 ) {
		if ( (*p) > 39 ) {
			if ( 42 <= (*p) && (*p) <= 43 )
				goto tr29;
		} else if ( (*p) >= 35 )
			goto tr29;
	} else if ( (*p) > 46 ) {
		if ( (*p) < 65 ) {
			if ( 48 <= (*p) && (*p) <= 57 )
				goto tr29;
		} else if ( (*p) > 90 ) {
			if ( 94 <= (*p) && (*p) <= 122 )
				goto tr29;
		} else
			goto tr29;
	} else
		goto tr29;
	goto st0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	if ( (*p) == 10 )
		goto tr30;
	goto st0;
tr30:
#line 155 "./c_src/request.rl"
	{
        build_version(request);
        {p++; cs = 69; goto _out;}
    }
	goto st69;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
#line 703 "./c_src/request.c"
	goto st0;
tr29:
#line 108 "./c_src/request.rl"
	{
        assert(request->hdr_name == NULL && "header name already marked");        
        assert(request->genbuf->pos == NULL && "wont overwrite a mark");
        request->genbuf->pos = p;
    }
	goto st19;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
#line 717 "./c_src/request.c"
	switch( (*p) ) {
		case 33: goto st19;
		case 58: goto tr32;
		case 124: goto st19;
		case 126: goto st19;
	}
	if ( (*p) < 45 ) {
		if ( (*p) > 39 ) {
			if ( 42 <= (*p) && (*p) <= 43 )
				goto st19;
		} else if ( (*p) >= 35 )
			goto st19;
	} else if ( (*p) > 46 ) {
		if ( (*p) < 65 ) {
			if ( 48 <= (*p) && (*p) <= 57 )
				goto st19;
		} else if ( (*p) > 90 ) {
			if ( 94 <= (*p) && (*p) <= 122 )
				goto st19;
		} else
			goto st19;
	} else
		goto st19;
	goto st0;
tr32:
#line 114 "./c_src/request.rl"
	{
        request->hdr_name = buffer_to_string(request->genbuf, p);
        if(request->hdr_name == NULL) {p++; cs = 20; goto _out;}
    }
	goto st20;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
#line 753 "./c_src/request.c"
	if ( (*p) == 13 )
		goto tr34;
	goto tr33;
tr33:
#line 119 "./c_src/request.rl"
	{
        assert(request->hdr_name != NULL && "value must have a name");
        assert(request->continued ||
                    (request->genbuf->pos == NULL && "wont overwrite a mark"));
        
        if(request->continued)
        {
            request->continued = 1;
        }
        else
        {
            request->genbuf->pos = p;
        }
    }
	goto st21;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
#line 778 "./c_src/request.c"
	if ( (*p) == 13 )
		goto st22;
	goto st21;
tr34:
#line 119 "./c_src/request.rl"
	{
        assert(request->hdr_name != NULL && "value must have a name");
        assert(request->continued ||
                    (request->genbuf->pos == NULL && "wont overwrite a mark"));
        
        if(request->continued)
        {
            request->continued = 1;
        }
        else
        {
            request->genbuf->pos = p;
        }
    }
	goto st22;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
#line 803 "./c_src/request.c"
	if ( (*p) == 10 )
		goto st23;
	goto st0;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	goto tr38;
tr12:
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st24;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
#line 823 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto tr39;
		case 32: goto tr39;
		case 37: goto st25;
		case 61: goto st24;
		case 95: goto st24;
	}
	if ( (*p) < 44 ) {
		if ( (*p) > 34 ) {
			if ( 36 <= (*p) && (*p) <= 42 )
				goto st24;
		} else if ( (*p) >= 33 )
			goto st24;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st24;
		} else if ( (*p) >= 64 )
			goto st24;
	} else
		goto st24;
	goto st0;
tr13:
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st25;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
#line 857 "./c_src/request.c"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st26;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st26;
	} else
		goto st26;
	goto st0;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st24;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st24;
	} else
		goto st24;
	goto st0;
tr6:
#line 82 "./c_src/request.rl"
	{
        assert(request->uribuf->pos == NULL && "wont overwrite uri mark");
        request->uribuf->pos = p;
    }
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
#line 891 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto tr43;
		case 32: goto tr43;
	}
	goto st0;
tr7:
#line 82 "./c_src/request.rl"
	{
        assert(request->uribuf->pos == NULL && "wont overwrite uri mark");
        request->uribuf->pos = p;
    }
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st28;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
#line 913 "./c_src/request.c"
	switch( (*p) ) {
		case 43: goto st28;
		case 58: goto tr45;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st28;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st28;
		} else if ( (*p) >= 65 )
			goto st28;
	} else
		goto st28;
	goto st0;
tr45:
#line 49 "./c_src/request.rl"
	{
        request->scheme = buffer_to_string(request->genbuf, p);
        if(request->scheme == NULL) {p++; cs = 29; goto _out;}
    }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
#line 941 "./c_src/request.c"
	if ( (*p) == 47 )
		goto st30;
	goto st0;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	if ( (*p) == 47 )
		goto st31;
	goto st0;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
	switch( (*p) ) {
		case 33: goto tr48;
		case 37: goto tr49;
		case 59: goto tr48;
		case 61: goto tr48;
		case 63: goto tr48;
		case 95: goto tr48;
	}
	if ( (*p) < 48 ) {
		if ( (*p) > 42 ) {
			if ( 44 <= (*p) && (*p) <= 46 )
				goto tr48;
		} else if ( (*p) >= 36 )
			goto tr48;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto tr48;
		} else if ( (*p) >= 65 )
			goto tr48;
	} else
		goto tr48;
	goto st0;
tr48:
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st32;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
#line 990 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto tr50;
		case 32: goto tr50;
		case 35: goto tr51;
		case 47: goto tr52;
		case 58: goto tr53;
		case 63: goto tr54;
	}
	goto st0;
tr76:
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st33;
tr8:
#line 82 "./c_src/request.rl"
	{
        assert(request->uribuf->pos == NULL && "wont overwrite uri mark");
        request->uribuf->pos = p;
    }
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st33;
tr52:
#line 54 "./c_src/request.rl"
	{
        request->host = buffer_to_string(request->genbuf, p);
        if(request->host == NULL) {p++; cs = 33; goto _out;}
    }
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st33;
tr72:
#line 59 "./c_src/request.rl"
	{
        request->port = 0;
    }
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st33;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
#line 1046 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto tr55;
		case 32: goto tr55;
		case 35: goto tr57;
		case 37: goto st34;
		case 61: goto st33;
		case 63: goto tr59;
		case 95: goto st33;
	}
	if ( (*p) < 44 ) {
		if ( 33 <= (*p) && (*p) <= 42 )
			goto st33;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st33;
		} else if ( (*p) >= 64 )
			goto st33;
	} else
		goto st33;
	goto st0;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st35;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st35;
	} else
		goto st35;
	goto st0;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st33;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st33;
	} else
		goto st33;
	goto st0;
tr9:
#line 82 "./c_src/request.rl"
	{
        assert(request->uribuf->pos == NULL && "wont overwrite uri mark");
        request->uribuf->pos = p;
    }
	goto st36;
tr54:
#line 54 "./c_src/request.rl"
	{
        request->host = buffer_to_string(request->genbuf, p);
        if(request->host == NULL) {p++; cs = 36; goto _out;}
    }
	goto st36;
tr59:
#line 67 "./c_src/request.rl"
	{
        request->path = buffer_to_string(request->genbuf, p);
        if(request->path == NULL) {p++; cs = 36; goto _out;}
    }
	goto st36;
tr74:
#line 59 "./c_src/request.rl"
	{
        request->port = 0;
    }
	goto st36;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
#line 1125 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto tr61;
		case 32: goto tr61;
		case 35: goto tr63;
		case 37: goto tr64;
		case 61: goto tr62;
		case 95: goto tr62;
	}
	if ( (*p) < 44 ) {
		if ( 33 <= (*p) && (*p) <= 42 )
			goto tr62;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto tr62;
		} else if ( (*p) >= 64 )
			goto tr62;
	} else
		goto tr62;
	goto st0;
tr62:
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st37;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
#line 1157 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto tr65;
		case 32: goto tr65;
		case 35: goto tr67;
		case 37: goto st38;
		case 61: goto st37;
		case 95: goto st37;
	}
	if ( (*p) < 44 ) {
		if ( 33 <= (*p) && (*p) <= 42 )
			goto st37;
	} else if ( (*p) > 59 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st37;
		} else if ( (*p) >= 64 )
			goto st37;
	} else
		goto st37;
	goto st0;
tr64:
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st38;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
#line 1189 "./c_src/request.c"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st39;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st39;
	} else
		goto st39;
	goto st0;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st37;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st37;
	} else
		goto st37;
	goto st0;
tr53:
#line 54 "./c_src/request.rl"
	{
        request->host = buffer_to_string(request->genbuf, p);
        if(request->host == NULL) {p++; cs = 40; goto _out;}
    }
	goto st40;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
#line 1223 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto tr70;
		case 32: goto tr70;
		case 35: goto tr71;
		case 47: goto tr72;
		case 63: goto tr74;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr73;
	goto st0;
tr73:
#line 59 "./c_src/request.rl"
	{
        request->port = 0;
    }
#line 63 "./c_src/request.rl"
	{
        request->port = request->port*10 + ((*p)-'0');
    }
	goto st41;
tr77:
#line 63 "./c_src/request.rl"
	{
        request->port = request->port*10 + ((*p)-'0');
    }
	goto st41;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
#line 1254 "./c_src/request.c"
	switch( (*p) ) {
		case 9: goto tr43;
		case 32: goto tr43;
		case 35: goto st5;
		case 47: goto tr76;
		case 63: goto st36;
	}
	if ( 48 <= (*p) && (*p) <= 57 )
		goto tr77;
	goto st0;
tr49:
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st42;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
#line 1276 "./c_src/request.c"
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st43;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st43;
	} else
		goto st43;
	goto st0;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto st32;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto st32;
	} else
		goto st32;
	goto st0;
tr10:
#line 82 "./c_src/request.rl"
	{
        assert(request->uribuf->pos == NULL && "wont overwrite uri mark");
        request->uribuf->pos = p;
    }
#line 39 "./c_src/request.rl"
	{
        assert(request->genbuf->pos == NULL && "won't overwrite a mark.");
        request->genbuf->pos = p;
    }
	goto st44;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
#line 1315 "./c_src/request.c"
	switch( (*p) ) {
		case 43: goto st28;
		case 58: goto tr45;
		case 84: goto st45;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st28;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st28;
		} else if ( (*p) >= 65 )
			goto st28;
	} else
		goto st28;
	goto st0;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
	switch( (*p) ) {
		case 43: goto st28;
		case 58: goto tr45;
		case 84: goto st46;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st28;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st28;
		} else if ( (*p) >= 65 )
			goto st28;
	} else
		goto st28;
	goto st0;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
	switch( (*p) ) {
		case 43: goto st28;
		case 58: goto tr45;
		case 80: goto st47;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st28;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 90 ) {
			if ( 97 <= (*p) && (*p) <= 122 )
				goto st28;
		} else if ( (*p) >= 65 )
			goto st28;
	} else
		goto st28;
	goto st0;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
	switch( (*p) ) {
		case 43: goto st28;
		case 47: goto st11;
		case 58: goto tr45;
	}
	if ( (*p) < 65 ) {
		if ( 45 <= (*p) && (*p) <= 57 )
			goto st28;
	} else if ( (*p) > 90 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st28;
	} else
		goto st28;
	goto st0;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st49;
		case 95: goto st49;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st49;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st49;
	} else
		goto st49;
	goto st0;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st50;
		case 95: goto st50;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st50;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st50;
	} else
		goto st50;
	goto st0;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st51;
		case 95: goto st51;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st51;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st51;
	} else
		goto st51;
	goto st0;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st52;
		case 95: goto st52;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st52;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st52;
	} else
		goto st52;
	goto st0;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st53;
		case 95: goto st53;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st53;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st53;
	} else
		goto st53;
	goto st0;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st54;
		case 95: goto st54;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st54;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st54;
	} else
		goto st54;
	goto st0;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st55;
		case 95: goto st55;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st55;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st55;
	} else
		goto st55;
	goto st0;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st56;
		case 95: goto st56;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st56;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st56;
	} else
		goto st56;
	goto st0;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st57;
		case 95: goto st57;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st57;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st57;
	} else
		goto st57;
	goto st0;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st58;
		case 95: goto st58;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st58;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st58;
	} else
		goto st58;
	goto st0;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st59;
		case 95: goto st59;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st59;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st59;
	} else
		goto st59;
	goto st0;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st60;
		case 95: goto st60;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st60;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st60;
	} else
		goto st60;
	goto st0;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st61;
		case 95: goto st61;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st61;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st61;
	} else
		goto st61;
	goto st0;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st62;
		case 95: goto st62;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st62;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st62;
	} else
		goto st62;
	goto st0;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st63;
		case 95: goto st63;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st63;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st63;
	} else
		goto st63;
	goto st0;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st64;
		case 95: goto st64;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st64;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st64;
	} else
		goto st64;
	goto st0;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st65;
		case 95: goto st65;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st65;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st65;
	} else
		goto st65;
	goto st0;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
		case 36: goto st66;
		case 95: goto st66;
	}
	if ( (*p) < 48 ) {
		if ( 45 <= (*p) && (*p) <= 46 )
			goto st66;
	} else if ( (*p) > 57 ) {
		if ( 65 <= (*p) && (*p) <= 90 )
			goto st66;
	} else
		goto st66;
	goto st0;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
	switch( (*p) ) {
		case 9: goto tr2;
		case 32: goto tr2;
	}
	goto st0;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
	switch( (*p) ) {
		case 33: goto tr29;
		case 124: goto tr29;
		case 126: goto tr29;
	}
	if ( (*p) < 45 ) {
		if ( (*p) > 39 ) {
			if ( 42 <= (*p) && (*p) <= 43 )
				goto tr29;
		} else if ( (*p) >= 35 )
			goto tr29;
	} else if ( (*p) > 46 ) {
		if ( (*p) < 65 ) {
			if ( 48 <= (*p) && (*p) <= 57 )
				goto tr29;
		} else if ( (*p) > 90 ) {
			if ( 94 <= (*p) && (*p) <= 122 )
				goto tr29;
		} else
			goto tr29;
	} else
		goto tr29;
	goto st0;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	if ( (*p) == 13 )
		goto st18;
	goto st0;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof69: cs = 69; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
	_test_eof29: cs = 29; goto _test_eof; 
	_test_eof30: cs = 30; goto _test_eof; 
	_test_eof31: cs = 31; goto _test_eof; 
	_test_eof32: cs = 32; goto _test_eof; 
	_test_eof33: cs = 33; goto _test_eof; 
	_test_eof34: cs = 34; goto _test_eof; 
	_test_eof35: cs = 35; goto _test_eof; 
	_test_eof36: cs = 36; goto _test_eof; 
	_test_eof37: cs = 37; goto _test_eof; 
	_test_eof38: cs = 38; goto _test_eof; 
	_test_eof39: cs = 39; goto _test_eof; 
	_test_eof40: cs = 40; goto _test_eof; 
	_test_eof41: cs = 41; goto _test_eof; 
	_test_eof42: cs = 42; goto _test_eof; 
	_test_eof43: cs = 43; goto _test_eof; 
	_test_eof44: cs = 44; goto _test_eof; 
	_test_eof45: cs = 45; goto _test_eof; 
	_test_eof46: cs = 46; goto _test_eof; 
	_test_eof47: cs = 47; goto _test_eof; 
	_test_eof48: cs = 48; goto _test_eof; 
	_test_eof49: cs = 49; goto _test_eof; 
	_test_eof50: cs = 50; goto _test_eof; 
	_test_eof51: cs = 51; goto _test_eof; 
	_test_eof52: cs = 52; goto _test_eof; 
	_test_eof53: cs = 53; goto _test_eof; 
	_test_eof54: cs = 54; goto _test_eof; 
	_test_eof55: cs = 55; goto _test_eof; 
	_test_eof56: cs = 56; goto _test_eof; 
	_test_eof57: cs = 57; goto _test_eof; 
	_test_eof58: cs = 58; goto _test_eof; 
	_test_eof59: cs = 59; goto _test_eof; 
	_test_eof60: cs = 60; goto _test_eof; 
	_test_eof61: cs = 61; goto _test_eof; 
	_test_eof62: cs = 62; goto _test_eof; 
	_test_eof63: cs = 63; goto _test_eof; 
	_test_eof64: cs = 64; goto _test_eof; 
	_test_eof65: cs = 65; goto _test_eof; 
	_test_eof66: cs = 66; goto _test_eof; 
	_test_eof67: cs = 67; goto _test_eof; 
	_test_eof68: cs = 68; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}

#line 329 "./c_src/request.rl"

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

    
#line 1936 "./c_src/request.c"
	{
	cs = http_req_parser_start;
	}

#line 412 "./c_src/request.rl"
    
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

