#include <iostream>
#include <stdarg.h>
#include "xobj_val.h"
#include "xobj_str.h"
#include "xobj_num.h"
#include "xobj_dict.h"
#include "xobj_list.h"

namespace xobj {

Value Value::Nil;
Value True(true);
Value False(false);

void Value::_inc_ref() { if (_obj) _obj->refInc(); }
void Value::_dec_ref() { if (_obj) _obj->refDec(); }

VALUE_TYPE Value::type() const {
    return _obj ? (VALUE_TYPE)_obj->type(): TV_NIL;
}
bool Value::isint() const { return isnum() && num()->isint(); }
bool Value::isfloat() const { return isnum() && !num()->isint(); }

Value::Value(bool b) { setobj(b ? &Bool::True : &Bool::False); }
Value::Value(int64_t i) { setobj(new Number(i)); }
Value::Value(double n) { setobj(new Number(n)); }
Value::Value(char *str, size_t len) { setobj(String::New(str, len)); }

int Value::len() {
    switch (type()) {
    case TV_STRING:
        return str()->len();
    case TV_LIST:
        return list()->len();
    case TV_DICT:
        return dict()->len();
    default:
        return 0;
    }
}

void Value::clear() {
    switch (type()) {
    case TV_LIST:
        return list()->clear();
    case TV_DICT:
        return dict()->clear();
    }
}

Value::operator bool() {
    if (!_obj) return false;
    switch (type()) {
    case TV_BOOL:
        return _bool()->val();
    case TV_NUMBER:
        return (bool)num()->i;
    default:
        return true;
    }
}

bool Value::operator==(Value &v) const {
    return isnil() ? v.isnil(): *_obj == v;
}
uint32_t Value::_hash() const {
    return _obj ? _obj->hash(): 0;
}

Value& Value::operator+=(Value &v) {
    switch (type()) {
    case TV_LIST:
        list()->append(v);
        break;
    }
    return *this;
}

Value Value::get(Value &v) const {
    switch (type()) {
    case TV_LIST:
        return v.isint() ? list()->get(v.num()->i) : Nil;
    case TV_DICT:
        return dict()->get(v);
    default:
        return Nil;
    }
}

Value& Value::set(Value &k, Value &v) {
    switch (type()) {
    case TV_LIST:
        if (k.isint()) list()->set(k.num()->i, v);
        break;
    case TV_DICT:
        dict()->set(k, v);
        break;
    }
    return *this;
}

int Value::index(Value &v) {
    switch (type()) {
    case TV_LIST:
        return list()->index(v);
    }
    return -1;
}
int Value::count(Value &v) {
    switch (type()) {
    case TV_LIST:
        return list()->count(v);
    }
    return -1;
}

std::ostream& operator<<(std::ostream& o, const Value& v) {
    switch (v.type()) {
    case TV_NIL:
        o << "nil";
        break;
    case TV_NUMBER:
        o << *v.num();
        break;
    case TV_STRING:
        o << *v.str();
        break;
    case TV_BOOL:
        o << (v._bool()->val() ? "true": "false");
        break;
    case TV_LIST:
        o << *v.list();
        break;
    case TV_DICT:
        o << *v.dict();
        break;
    }
    return o;
}

}

#ifdef XOBJ_USE_PY
#include "xobj_py.cpp"
#endif
