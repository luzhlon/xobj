#include <ctype.h>
#include "xobj_str.h"
#include "xobj_dict.h"
#include "xobj_val.h"

static xobj::Dict strdict;

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

// Value operator""_s(const char *str, size_t len) { return Value::V(new String(str)); }

String *String::New(char *data, size_t len) {
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

String *String::New(std::istream& in, size_t n) {
    auto size = offsetof(VarStr, data) + n + 1;
    auto str = (VarStr *)(new char[size]);
    // Read string from stream failure
    if (!in.read(str->data, n))
        { delete str; return nullptr; }
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

bool String::operator==(Value &v) const {
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
static void print_hex(std::ostream& o, unsigned char c) {
    char buf[8];
    sprintf(buf, "\\x%02x", c); o << buf;
}
static void print_char(std::ostream& o, unsigned char c) {
    switch (c) {
    case '\a':
        o << "\\a";
        break;
    case '\b':
        o << "\\b";
        break;
    case '\f':
        o << "\\f";
        break;
    case '\n':
        o << "\\n";
        break;
    case '\r':
        o << "\\r";
        break;
    case '\t':
        o << "\\t";
        break;
    case '\v':
        o << "\\v";
        break;
    case '\\':
        o << "\\";
        break;
    case '\"':
        o << "\\\"";
        break;
    default:
        if (c < ' ')
            print_hex(o, c);
        else
            o << c;
        break;
    }
}

void _print_bin(std::ostream& o, const char *data, size_t len) {
    const char *end = data + len;
    for (const char *p = data; p < end; p++)
        if (((unsigned char)*p) > 0x7F)
            print_hex(o, *p);
        else
            print_char(o, *p);
}

void _print_str(std::ostream& o, const char *data, size_t len) {
    const char *end = data + len;
    for (const char * p = data; p < end; p++)
        print_char(o, *p);
}

std::ostream& operator<<(std::ostream& o, const String& s) {
    o << '"';
    _print_str(o, s.c_str(), s.len());
    o << '"';
    return o;
}

}
