#include <python.h>
#include "xobj_val.h"
#include "xobj_obj.h"

namespace xobj {

const char *_py_encode = "gbk";

PyObject *Value::toPyObj() {
    switch (type()) {
    case TV_NIL:
        return _PyObject_New(&_PyNone_Type);
    case TV_NUMBER:
        return isint()? PyLong_FromLong(_num()->i): PyFloat_FromDouble(_num()->f);
    case TV_BOOL:
        return b()->val() ? Py_True : Py_False;
    case TV_STRING:
        return PyUnicode_Decode(_str()->c_str(), _str()->len(), _py_encode, nullptr);
    case TV_LIST:
        return _list()->toPyObj();
    case TV_DICT:
        return _dict()->toPyObj();
    default:
        return nullptr;
    }
}

PyObject *List::toPyObj() {
    PyObject *list = PyList_New(size);
    for (int i = 0; i < size; ++i)
        PyList_SetItem(list, i, vals[i].toPyObj());
    return list;
}

PyObject *Dict::toPyObj() {
    PyObject *dict = PyDict_New();
    for (int i = 0; i < maxsize; ++i) {
        auto p = &_items[i];
        if (!p->empty())
            PyDict_SetItem(dict, p->key().toPyObj(), p->value().toPyObj());
    }
    return dict;
}

}
