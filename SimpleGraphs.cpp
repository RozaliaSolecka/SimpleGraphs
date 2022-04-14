#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "structmember.h"


using namespace std;
typedef struct {
    PyObject_HEAD
    unsigned short matrix[16];
    unsigned short vertices;


} AdjacencyMatrixObject;

static void
AdjacencyMatrix_dealloc(AdjacencyMatrixObject* self)
{
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject*
AdjacencyMatrix_new(PyTypeObject* type, PyObject* args, PyObject* kwds)
{
    AdjacencyMatrixObject* self;
    self = (AdjacencyMatrixObject*)type->tp_alloc(type, 0);
    return (PyObject*)self;
}
static int
AdjacencyMatrix_init(AdjacencyMatrixObject* self, PyObject* args, PyObject* kwds)
{
    char* text;
    if (!PyArg_ParseTuple(args, "s", &text))
        return NULL;
    int length = (int)text[0] - 63;

    unsigned short number = 0b1000000000000000;
    self->vertices = 0b0000000000000000;
    for (int j = 0; j < length; j++) {
        self->vertices = (self->vertices | number);
        number = (number >> 1);
    }
    for (int j = 0; j < 16; j++) {
        self->matrix[j] = 0;
    }

    int k = 0, i = 1, c;
    for (int v = 1; v < length; v++) {
        for (int u = 0; u < v; u++) {
            if (k == 0) {

                c = int(text[i]) - 63;
                i++;
                k = 6;
            }
            k--;
            unsigned short number = 0b1000000000000000;
            if ((c & (1 << k)) != 0) {
                unsigned short row = self->matrix[u];
                row = (row | (number >> v));
                self->matrix[u] = row;
            }
        }
    }
    return 0;
}

static PyMemberDef AdjacencyMatrix_members[] = {
    {"matrix", T_USHORT, offsetof(AdjacencyMatrixObject, matrix), 0, "adjacency matrix"},
    {"vertices", T_USHORT, offsetof(AdjacencyMatrixObject, vertices), 0, "vertices"},
    {NULL}  /* Sentinel */
};

static PyObject* number_of_vertices(AdjacencyMatrixObject* self)
{
    int counter = 0;
    unsigned short number = 0b1000000000000000;
    for (int i = 0; i < 16; i++) {
        if ((self->vertices & number) == number) {
            counter++;
        }
        number = number >> 1;
    }
    return Py_BuildValue("i", counter);
}

static PyObject* vertices(AdjacencyMatrixObject* self)
{
    PyObject* set = PySet_New(NULL);

    unsigned short number = 0b1000000000000000;
    for (int i = 0; i < 16; i++) {
        if ((self->vertices & number) == number) {
            PyObject* python_int = Py_BuildValue("i", i);
            PySet_Add(set, python_int);
            Py_DECREF(python_int);
        }
        number = number >> 1;
    }

    return set;
}
static PyObject* number_of_edges(AdjacencyMatrixObject* self)
{
    int counter = 0;

    for (int i = 0; i < 16; i++) {
        unsigned short number = 0b1000000000000000;
        for (int j = 0; j < 16; j++) {
            unsigned short row = self->matrix[i];
            if ((row & number) == number) {
                counter++;
            }
            number = number >> 1;
        }
    }
    counter = counter;

    return Py_BuildValue("i", counter);
}
static PyObject* edges(AdjacencyMatrixObject* self)
{
    PyObject* set = PySet_New(NULL);

    for (int i = 0; i < 16; i++) {
        unsigned short number = 0b1000000000000000;
        for (int j = 0; j < 16; j++) {
            unsigned short row = self->matrix[i];
            if ((row & number) == number) {
                if (i <= j)
                {
                    PyObject* python_int = Py_BuildValue("(ii)", i, j);
                    PySet_Add(set, python_int);
                    Py_DECREF(python_int);
                }
                
            }
            number = number >> 1;
        }
    }
    return set;
}
static PyObject* is_edge(AdjacencyMatrixObject* self, PyObject* vertices)
{
    int u, v;
    // PyObject* result = Py_False;
    if (!PyArg_ParseTuple(vertices, "ii", &u, &v))
        return NULL;

    unsigned short number = 0b1000000000000000;
    number = number >> v;

    unsigned short row = self->matrix[u];
    if ((row & number) == number) {
        Py_RETURN_TRUE;
    }

    number = 0b1000000000000000;
    number = number >> u;

    row = self->matrix[v];
    if ((row & number) == number) {
        Py_RETURN_TRUE;
    }
    Py_RETURN_FALSE;
}
static PyObject* vertex_degree(AdjacencyMatrixObject* self, PyObject* vertex)
{
    int v;
    int counter = 0;

    if (!PyArg_ParseTuple(vertex, "i", &v))
        return NULL;
    unsigned short row = self->matrix[v];

    unsigned short number = 0b1000000000000000;
    for (int i = 0; i < 16; i++) {
        if ((row & number) == number) {
            counter++;
        }
        number = number >> 1;
    }

    number = 0b1000000000000000;
    number = number >> v;
    for (int i = 0; i < 16; i++) {
        if ((self->matrix[i] & number) == number) {
            counter++;
        }
    }
    return Py_BuildValue("i", counter);
}
static PyObject* vertex_neighbors(AdjacencyMatrixObject* self, PyObject* vertex)
{
    int v;
    PyObject* set = PySet_New(NULL);

    if (!PyArg_ParseTuple(vertex, "i", &v))
        return NULL;
    unsigned short row = self->matrix[v];

    unsigned short number = 0b1000000000000000;
    for (int i = 0; i < 16; i++) {
        if ((row & number) == number) {
            PyObject* python_int = Py_BuildValue("i", i);
            PySet_Add(set, python_int);
            Py_DECREF(python_int);
        }
        number = number >> 1;
    }

    number = 0b1000000000000000;
    number = number >> v;
    for (int i = 0; i < 16; i++) {
        if ((self->matrix[i] & number) == number) {
            PyObject* python_int = Py_BuildValue("i", i);
            PySet_Add(set, python_int);
            Py_DECREF(python_int);
        }
    }

    return set;
}
static PyObject* delete_vertex(AdjacencyMatrixObject* self, PyObject* vertex)
{
    //PyObject* result = NULL;
    int v;

    if (!PyArg_ParseTuple(vertex, "i", &v))
        Py_RETURN_NONE;

    unsigned short number = 0b1000000000000000;
    number = number >> v;
    number = ~number;
    self->vertices = self->vertices & number;

    self->matrix[v] = 0b0000000000000000;

    for (int i = 0; i < 16; i++) {
        self->matrix[i] = self->matrix[i] & number;
    }
    Py_RETURN_NONE;
}
static PyObject* add_vertex(AdjacencyMatrixObject* self, PyObject* vertex)
{
    //PyObject* result = NULL;
    int v;

    if (!PyArg_ParseTuple(vertex, "i", &v))
        Py_RETURN_NONE;

    unsigned short number = 0b1000000000000000;
    number = (number >> v);
    self->vertices = (self->vertices | number);

    Py_RETURN_NONE;
}

static PyObject* delete_edge(AdjacencyMatrixObject* self, PyObject* vertices)
{
    //PyObject* result = NULL;
    int u, v;

    if (!PyArg_ParseTuple(vertices, "ii", &u, &v))
        Py_RETURN_NONE;

    unsigned short number = 0b1000000000000000;
    number = number >> v;
    number = ~number;
    self->matrix[u] = self->matrix[u] & number;

    number = 0b1000000000000000;
    number = number >> u;
    number = ~number;
    self->matrix[v] = self->matrix[v] & number;

    Py_RETURN_NONE;
}

static PyObject* add_edge(AdjacencyMatrixObject* self, PyObject* vertices)
{
    //PyObject* result = NULL;
    int u, v;
    unsigned short number = 0b1000000000000000;

    if (!PyArg_ParseTuple(vertices, "ii", &u, &v))
        Py_RETURN_NONE;

    number = 0b1000000000000000;
    number = (number >> v);
    unsigned short row = self->matrix[u];
    row = (row | number);
    self->matrix[u] = row;

   
    number = 0b1000000000000000;
    number = (number >> u);
    row = self->matrix[v];
    row = row | number;
    self->matrix[v] = row;
    

    Py_RETURN_NONE;
}
static bool color_component(AdjacencyMatrixObject* self, int* cs, int vertex, int color)
{
    cs[vertex] = color;
    PyObject* edge;

    unsigned short number = 0b1000000000000000;
    for (int v = 0; v < 16; v++) {
        if ((self->vertices & number) == number) {
            edge = Py_BuildValue("(ii)", v, vertex);
            if (is_edge(self, edge) == Py_True) {
                Py_DECREF(edge);
                if (cs[v] == color) {
                    return false;
                }
                if (cs[v] == 0) {
                    if (!color_component(self, cs, v, 3 - color)) {
                        return false;
                    }
                }
            }
            else {
                Py_DECREF(edge);
            }
            
        }
        number = number >> 1;
    }
    return true;
}

static PyObject* is_bipartite(AdjacencyMatrixObject* self)
{

    int cs[16];

    for (int i = 0; i < 16; i++)
    {
        cs[i] = -1;
    }

    unsigned short number = 0b1000000000000000;
    for (int i = 0; i < 16; i++) {
        if ((self->vertices & number) == number) {
            cs[i] = 0;
        }
        number = number >> 1;
    }

    for (int v = 0; v < 16; v++) {
        if (cs[v] == 0) {
            if (color_component(self, cs, v, 1) == false) {
                Py_RETURN_FALSE;
            }
        }
    }

    Py_RETURN_TRUE;
}


static PyMethodDef AdjacencyMatrix_methods[] = {
     {"number_of_vertices", (PyCFunction)number_of_vertices, METH_NOARGS, "Returns the number of vertices."},
     {"vertices", (PyCFunction)vertices, METH_NOARGS, "Returns the vertices."},
     {"number_of_edges", (PyCFunction)number_of_edges, METH_NOARGS, "Returns the number of edges."},
     {"edges", (PyCFunction)edges, METH_NOARGS, "Returns the edges."},
     {"is_edge", (PyCFunction)is_edge, METH_VARARGS, "Returns the vertices are neighbours."},
     {"vertex_degree", (PyCFunction)vertex_degree, METH_VARARGS, "Returns the degree of vertex"},
     {"vertex_neighbors", (PyCFunction)vertex_neighbors, METH_VARARGS, "Returns the neighbors of vertex"},
     {"delete_vertex", (PyCFunction)delete_vertex, METH_VARARGS, "Delete vertex and incidental edges"},
     {"add_vertex", (PyCFunction)add_vertex, METH_VARARGS, "Add vertex"},
     {"delete_edge", (PyCFunction)delete_edge, METH_VARARGS, "Delete edge"},
     {"add_edge", (PyCFunction)add_edge, METH_VARARGS, "Add edge"},
     {"is_bipartite", (PyCFunction)is_bipartite, METH_NOARGS, "If is bipartite"},
     {NULL}
};
static PyTypeObject AdjacencyMatrixType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "adjacencymatrix.AdjacencyMatrix",                          /*tp_name*/
    sizeof(AdjacencyMatrixObject),                    /*tp_basicsize*/
    0,                                          /*tp_itemsize*/
    (destructor)AdjacencyMatrix_dealloc,       /*tp_dealloc*/
    0,                                          /*tp_print*/
    0,                                          /*tp_getattr*/
    0,                                          /*tp_setattr*/
    0,                                          /*tp_compare*/
    0,                                          /*tp_repr*/
    0,                                          /*tp_as_number*/
    0,                                          /*tp_as_sequence*/
    0,                                          /*tp_as_mapping*/
    0,                                          /*tp_hash */
    0,                                          /*tp_call*/
    0,                                          /*tp_str*/
    0,                                          /*tp_getattro*/
    0,                                          /*tp_setattro*/
    0,                                          /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,                         /*tp_flags*/
    0,                                          /*tp_doc*/
    0,                                          /*tp_traverse*/
    0 ,                                         /*tp_clear*/
    0,                                          /*tp_richcompare*/
    0,                                          /*tp_weaklistoffset*/
    0,                                          /*tp_iter*/
    0,                                          /*tp_iternext*/
    AdjacencyMatrix_methods,                                    /*tp_methods*/
    AdjacencyMatrix_members,                                          /*tp_members*/
    0,                                          /*tp_getsets*/
    0,                                          /*tp_base*/
    0,                                          /*tp_dict*/
    0,                                          /*tp_descr_get*/
    0,                                          /*tp_descr_set*/
    0,                                          /*tp_dictoffset*/
    (initproc)AdjacencyMatrix_init,            /*tp_init*/
    0,                                          /*tp_alloc*/
    AdjacencyMatrix_new,                        /*tp_new*/
};

static PyModuleDef adjacencymatrixmodule = {
    PyModuleDef_HEAD_INIT,
    "adjacencymatrix",
    "Example module that creates an extension type.",
    -1,
};

PyMODINIT_FUNC
PyInit_simple_graphs(void)
{
    PyObject* m;
    if (PyType_Ready(&AdjacencyMatrixType) < 0)
        return NULL;

    m = PyModule_Create(&adjacencymatrixmodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&AdjacencyMatrixType);
    if (PyModule_AddObject(m, "AdjacencyMatrix", (PyObject*)&AdjacencyMatrixType) < 0) {
        Py_DECREF(&AdjacencyMatrixType);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}