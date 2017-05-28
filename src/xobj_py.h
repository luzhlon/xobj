#ifndef __XOBJ_PY_H__
#define __XOBJ_PY_H__

#include "xobj.h"

#include <Python.h>

namespace xobj {
    PyObject *topyobj();
    Value frompyobj(PyObject *);
}

#endif /* __XOBJ_PY_H__ */
