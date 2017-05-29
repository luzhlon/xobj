#include <iostream>
#include <stdarg.h>
#include "xobj_val.h"
#include "xobj_str.h"
#include "xobj_num.h"
#include "xobj_dict.h"
#include "xobj_list.h"

namespace xobj {

Value Value::Nil;
Value Value::True(true);
Value Value::False(false);

Bool Bool::True(true);
Bool Bool::False(false);

Value::Value(bool b) { refer(b ? &Bool::True : &Bool::False); }
Value::Value(int64_t i) { refer(new Int(i)); }
Value::Value(double n) { refer(new Float(n)); }
Value::Value(char *str, size_t len) { refer(String::New(str, len)); }

int Value::len() {
    switch (type()) {
    case TV_STRING:
        return s().len();
    case TV_LIST:
        return l().len();
    case TV_DICT:
        return d().len();
    default:
        return 0;
    }
}

void Value::clear() {
    switch (type()) {
    case TV_LIST:
        return l().clear();
    case TV_DICT:
        return d().clear();
    }
}

Value& Value::operator+=(Value &v) {
    switch (type()) {
    case TV_LIST:
        l().append(v);
        break;
    }
    return *this;
}

Value Value::get(Value &v) const {
    switch (type()) {
    case TV_LIST:
        return v.isint() ? l().get(v.i().val()) : Nil;
    case TV_DICT:
        return d().get(v);
    default:
        return Nil;
    }
}

Value& Value::set(Value &k, Value &v) {
    switch (type()) {
    case TV_LIST:
        if (k.isint()) l().set(k.i().val(), v);
        break;
    case TV_DICT:
        d().set(k, v);
        break;
    }
    return *this;
}

void *Value::p() const { return isint() ? (void *)i().val() : nullptr; }

int Value::index(Value &v) {
    switch (type()) {
    case TV_LIST:
        return l().index(v);
    }
    return -1;
}
int Value::count(Value &v) {
    switch (type()) {
    case TV_LIST:
        return l().count(v);
    }
    return -1;
}

Value P(void *p) { return V((int64_t)p); }

}