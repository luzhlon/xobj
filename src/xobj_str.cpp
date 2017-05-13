#include "xobj_str.h"
#include "xobj_dict.h"
#include "xobj_val.h"

// If internal the string
static bool isintern(char *data, size_t len) {
    return len < 32 ? true : false;
}

static xobj::Dict strdict;

namespace xobj {

uint32_t _gethash(char *data, size_t len) {
    static uint32_t seed = 1314;
    uint32_t hash = 0;
    // calculate the hash value of data
    auto p = data;
    for (auto i = len; i--; hash = hash * seed + *p++);
    return hash;
}

// Value operator""_s(const char *str, size_t len) { return Value::V(new String(str)); }

String *String::New(char *data, size_t len) {
    String *str = nullptr;
    if (isintern(data, len))
        str = strdict.get(data, len);
    if (str) return str;
    auto size = offsetof(String, data) + len + 1;
    str = (String *)(new char[size]);
    str->String::String();
    str->_size = len;
    str->_hash = _gethash(data, len);
    memcpy(str->data, data, len);
    str->data[len] = 0;
    if (isintern(data, len))
        strdict.set(str, str);
    return str;
}

String *String::New(std::istream& in, size_t len) {
    String *nstr = nullptr;
    auto size = offsetof(String, data) + len + 1;
    auto str = (String *)(new char[size]);
    if (!in.read(str->data, len)) { delete str; return nullptr; }
    str->data[len] = 0;
    if (isintern(str->data, len))
        nstr = strdict.get(str->data, len);
    if (nstr) {
        delete (char *)str;
        return nstr;
    }
    str->String::String();
    str->_hash = _gethash(str->data, len);
    str->_size = len;
    return str;
}

bool String::operator==(Value &v) const {
    if (!v.isstr()) return false;
    auto str = v.str();
    auto s1 = c_str();
    auto s2 = str->c_str();
    if ((s1 == s2) ||
        (_hash == str->_hash &&
         _size == str->_size &&
         !memcmp(s1, s1, _size)))
        return true;
    return false;
}

bool String::equals(char *str, size_t len) {
    if (len != _size) return false;
    return !memcmp(c_str(), str, len);
}
static void print_hex(std::ostream& o, unsigned char c) {
    char buf[8]; sprintf(buf, "\\x%02x", c); o << buf;
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
    _print_str(o, s.c_str(), s._size);
    o << '"';
    return o;
}

}
