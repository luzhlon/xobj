#include <ctype.h>
#include <istream>

#include "xobj_str.h"
#include "xobj_dict.h"
#include "xobj_val.h"

static auto _strdict = xobj::Dict::New();
static auto& strdict = _strdict.d();

static uint32_t _seed = 1314;

namespace xobj {

_StrAttr _getstrattr(char *data, size_t len) {
    _StrAttr t;
    // calculate the hash value of data
    auto p = data; auto e = p + len;
    if (len <= 32)
        while (p < e && (isalnum(*p) || *p == '_'))
            (t.hash *= _seed) += *p++;
    if (p < e) {
        t.iskw = false;
        while (p < e)
            (t.hash *= _seed) += *p++;
    }
    return t;
}

uint32_t _gethash(char *data, size_t len) { return _getstrattr(data, len).hash; }

Value::Value(char *str, size_t len) {
    String::New(str, len).moveto(*this);
}
/* TODO: Covert to return-type:Value */
Value String::New(char *data, size_t len) {
    String *str = nullptr;
    auto attr = _getstrattr(data, len);
    if (attr.iskw) {
        str = strdict.get(data, len, attr.hash);
        if (str) return str; // Already exists
    }
    // Allocate new String
    auto size = offsetof(VarStr, data) + len + 1;
    auto vstr = (VarStr *)(new char[size]);
    vstr->VarStr::VarStr(data, len, attr);
    if (attr.iskw)          // Store into internal
        strdict.set(vstr, vstr);
    return vstr;
}

using namespace std;

Value String::New(istream& in, size_t n) {
    auto size = offsetof(VarStr, data) + n + 1;
    auto str = (VarStr *)(new char[size]);
    // Read string from stream failure
    if (!in.read(str->data, n))
        { delete str; return Value::Nil; }
    str->data[n] = 0, str->len(n);
    auto attr = _getstrattr(str->data, n);
    if (attr.iskw) {
        auto nstr = strdict.get(str->data, n, attr.hash);
        if (nstr)           // Already exists
            { delete str; return  nstr; }
    }
    str->VarStr::VarStr(attr);
    if (attr.iskw)          // Store into internal
        strdict.set(str, str);
    return str;
}

Value TS(char *str, size_t n) {
    return Value::V(new TempStr(str, n));
}
Value TS(char *str) {
    return Value::V(new TempStr(str, strlen(str)));
}
Value CS(const char *str) {
    return Value::V(new ConstStr(str));
}

ConstStr::ConstStr(const char *str) : _str(str) {
    auto n = strlen(str); len(n);
    _attr = _getstrattr((char *)str, n);
    isbin(_attr.iskw);
    if (_attr.iskw)
        strdict.set(this, this);
}

bool String::operator==(const Value &v) const {
    if (!v.isstr()) return false;
    auto& str = v.s();
    if (&str == this) return true;
    auto s1 = c_str(), s2 = str.c_str();
    if ((s1 == s2) ||
        (hash() == str.hash() &&
         len() == str.len() &&
         !memcmp(s1, s1, len())))
        return true;
    return false;
}

bool String::equals(char *str, size_t n) {
    return n == len() ?
        !memcmp(c_str(), str, n) : false;
}

}
