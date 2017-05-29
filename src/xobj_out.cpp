
#include "xobj_out.h"
#include "xobj_num.h"
#include "xobj_str.h"
#include "xobj_list.h"
#include "xobj_dict.h"

namespace xobj {

static void print_hex(ostream& o, unsigned char c) {
    char buf[8];
    sprintf(buf, "\\x%02x", c); o << buf;
}
static void print_char(ostream& o, unsigned char c) {
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

void _print_bin(ostream& o, const char *data, size_t len) {
    const char *end = data + len;
    for (const char *p = data; p < end; p++)
        if (((unsigned char)*p) > 0x7F)
            print_hex(o, *p);
        else
            print_char(o, *p);
}

void _print_str(ostream& o, const char *data, size_t len) {
    const char *end = data + len;
    for (const char * p = data; p < end; p++)
        print_char(o, *p);
}

ostream& operator<<(ostream& o, const String& s) {
    o << '"';
    _print_str(o, s.c_str(), s.len());
    o << '"';
    return o;
}

ostream& operator<<(ostream& o, const List& l) {
    o << '[';
    auto len = l.len();
    auto p = l.valist();
    if (len) {
        int i = 0;
        o << p[i++];
        while (i < len)
            o << ',' << p[i++];
    }
    o << ']';
    return o;
}

ostream& operator<<(ostream& o, const Dict& d) {
    o << '{';
    auto it = d.begin();
    auto ed = d.end();
    if (it != ed)
    while (1) {
        o << (*it).key() << ':' << (*it).value(), ++it;
        if (it != ed) o << ','; else break;
    }
    o << '}';
    return o;
}

ostream& operator<<(ostream& o, const Value& v) {
    switch (v.type()) {
    case TV_NIL:
        return o << "nil";
    case TV_INT:
        return o << v.i().val();
    case TV_FLOAT:
        return o << v.f().val();
    case TV_STRING:
        return o << v.s();
    case TV_BOOL:
        return o << (v.b().val() ? "true": "false");
    case TV_LIST:
        return o << v.l();
    case TV_DICT:
        return o << v.d();
    }
    return o;
}

}