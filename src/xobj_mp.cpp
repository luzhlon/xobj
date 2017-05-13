
#include "xobj_mp.h"
#include "xobj_list.h"
#include "xobj_num.h"
#include "xobj_dict.h"
#include "xobj_str.h"

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#ifdef WIN32
#else
#endif

using namespace xobj;

//uint16_t _htons(int16_t s) { return htons(s); }
//uint32_t _htonl(int32_t l) { return htonl(l); }
//uint64_t _htonll(int32_t ll) { return htonll(ll); }

uint16_t _htons(uint16_t s) { return htons(s); }
uint16_t _ntohs(uint16_t s) { return ntohs(s); }
uint32_t _htonl(uint32_t l) { return htonl(l); }
uint32_t _ntohl(uint32_t l) { return ntohl(l); }
uint64_t _htonll(uint64_t ll) { return htonll(ll); }
uint64_t _ntohll(uint64_t ll) { return ntohll(ll); }

#define WRITE16(I, P) *((uint16_t *)(P)) = _htons(I & 0xFFFF)
#define WRITE32(I, P) *((uint32_t *)(P)) = _htonl(I & 0xFFFFFFFF)
#define WRITE64(I, P) *((uint64_t *)(P)) = _htonll(I)

static void toMsgPack(List *l, std::ostream& o) {
    char buf[10];
    if (l->len() < 0x10)
        buf[0] = (0x90 | l->len()), o.write(buf, 1);
    else if (l->len() < 0x10000)
        buf[0] = 0xDC, WRITE16(l->len(), &buf[1]),
        o.write(buf, 3);
    else if (l->len() < 0x100000000)
        buf[0] = 0xDD, WRITE32(l->len(), &buf[1]),
        o.write(buf, 5);
    for (auto i : *l)
        xobj::toMsgPack(o, i);
}

static void toMsgPack(Dict *d, std::ostream& o) {
    char buf[10];
    if (d->len() < 0x10)
        buf[0] = (0x80 | d->len()), o.write(buf, 1);
    else if (d->len() < 0x10000)
        buf[0] = 0xDE, WRITE16(d->len(), &buf[1]),
        o.write(buf, 3);
    else if (d->len() < 0x100000000)
        buf[0] = 0xDF, WRITE32(d->len(), &buf[1]),
        o.write(buf, 5);
    for (auto i : *d)
        xobj::toMsgPack(o, i.key()), xobj::toMsgPack(o, i.value());
}

static void toMsgPack(Number *n, std::ostream& o) {
    char buf[10];
    if (n->isint()) {       // Integer
        if (n->i < 0x10000) {
            if (n->i < 0) {
                if (n->i > -33)
                    buf[0] = n->i & 0xFF, o.write(buf, 1);
                else if (n->i >= (int64_t)(int8_t)0x80)
                    buf[0] = 0xD0, buf[1] = n->i & 0xFF, o.write(buf, 2);
                else if (n->i >= (int64_t)(int16_t)0x8000)
                    buf[0] = 0xD1, WRITE16(n->i, &buf[1]), o.write(buf, 3);
                else if (n->i >= (int64_t)(int32_t)0x80000000)
                    buf[0] = 0xD2, WRITE32(n->i, &buf[1]), o.write(buf, 5);
                else
                    buf[0] = 0xD3, WRITE64(n->i, &buf[1]), o.write(buf, 9);
            } else if (n->i < 0x80)
                buf[0] = n->i & 0xFF, o.write(buf, 1);
            else if (n->i < 0x100)
                buf[0] = 0xCC, buf[1] = n->i & 0xFF,
                o.write(buf, 2);
            else
                buf[0] = 0xCD, WRITE16(n->i, &buf[1]), o.write(buf, 3);
        } else if (n->i < 0x100000000)
            buf[0] = 0xCE, WRITE32(n->i, &buf[1]), o.write(buf, 5);
        else
            buf[0] = 0xCF, WRITE64(n->i, &buf[1]), o.write(buf, 9);
    } else                  // Float Number
        buf[0] = 0xCB,
        *(uint64_t *)&buf[1] = _htonll(*(uint64_t *)(&n->f)), o.write(buf, 9);
}

static void toMsgPack(String *s, std::ostream& o) {
    char buf[10];
    if (s->isbin()) {
        if (s->len() < 0x100)
            buf[0] = 0xC4, buf[1] = s->len(),
            o.write(buf, 2);
        else if (s->len() < 0x10000)
            buf[0] = 0xC5, WRITE16(s->len(), &buf[1]),
            o.write(buf, 3);
        else if (s->len() < 0x100000000)
            buf[0] = 0xC6, WRITE32(s->len(), &buf[1]),
            o.write(buf, 5);
    } else if (s->len() < 0x20)
        buf[0] = (0xA0 | s->len()) & 0xFF, o.write(buf, 1);
    else if (s->len() < 0x100)
        buf[0] = 0xD9, buf[1] = s->len(), o.write(buf, 2);
    else if (s->len() < 0x10000)
        buf[0] = 0xDA, WRITE16(s->len(), &buf[1]), o.write(buf, 3);
    else if (s->len() < 0x100000000)
        buf[0] = 0xDB, WRITE32(s->len(), &buf[1]), o.write(buf, 5);

    o.write(s->c_str(), s->len());
}

namespace xobj {

void toMsgPack(std::ostream& o, Value& v) {
    switch (v.type()) {
    case TV_NIL:
        o << "\xC0";
        break;
    case TV_BOOL:
        o << v._bool()->val() ? "\xC2" : "\xC3";
        break;
    case TV_NUMBER:
        ::toMsgPack(v.num(), o);
        break;
    case TV_STRING:
        ::toMsgPack(v.str(), o);
        break;
    case TV_LIST:
        ::toMsgPack(v.list(), o);
        break;
    case TV_DICT:
        ::toMsgPack(v.dict(), o);
        break;
    default:
        break;
    }
}

static Value readstr(std::istream& i, size_t len) {
    Value str = String::New(i, len);
    return str;
}
static Value readarray(std::istream& i, size_t len) {
    auto l = L();
    for (Value v; len--; l += v)
        if (!fromMsgPack(i, v))
            return Value::Nil;
    return l;
}
static Value readmap(std::istream& i, size_t len) {
    auto d = D();
    Value k, v;
    while (len--) {
        if (!fromMsgPack(i, k))
            return Value::Nil;
        if (!fromMsgPack(i, v))
            return Value::Nil;
        d.set(k, v);
    }
    return d;
}

static bool read(std::istream& i, float &f) {
    if (!i.read((char *)&f, sizeof(f)))
        return false;
    *(uint32_t *)&f = _ntohl(*(uint32_t *)&f);
    return true;
}
static bool read(std::istream& i, double &f) {
    if (!i.read((char *)&f, sizeof(f)))
        return false;
    *(uint64_t *)&f = _ntohll(*(uint64_t *)&f);
    return true;
}
static bool read(std::istream& i, uint8_t &n) {
    if (!i.read((char *)&n, sizeof(n)))
        return false;
    return true;
}
static bool read(std::istream& i, uint16_t &n) {
    if (!i.read((char *)&n, sizeof(n)))
        return false;
    n = _ntohs(n); return true;
}
static bool read(std::istream& i, uint32_t &n) {
    if (!i.read((char *)&n, sizeof(n)))
        return false;
    n = _ntohl(n); return true;
}
static bool read(std::istream& i, uint64_t &n) {
    if (!i.read((char *)&n, sizeof(n)))
        return false;
    n = _ntohll(n); return true;
}
static bool read(std::istream& i, int8_t &n) {
    if (!i.read((char *)&n, sizeof(n)))
        return false;
    return true;
}
static bool read(std::istream& i, int16_t &n) {
    if (!i.read((char *)&n, sizeof(n)))
        return false;
    n = _ntohs(n); return true;
}
static bool read(std::istream& i, int32_t &n) {
    if (!i.read((char *)&n, sizeof(n)))
        return false;
    n = _ntohl(n); return true;
}
static bool read(std::istream& i, int64_t &n) {
    if (!i.read((char *)&n, sizeof(n)))
        return false;
    n = _ntohll(n); return true;
}

bool fromMsgPack(std::istream& i, Value &v) {
    char c;
    if (!i.read(&c, 1))
        return false;
    union {
        float f; double d;
        int8_t b; uint8_t ub;
        int16_t s; uint16_t us;
        int32_t l; uint32_t ul;
        int64_t ll; uint64_t ull;
    } val;
    if (c & 0x80) {
        // Negative fixint
        if ((c & 0xE0) == 0xE0) {
            v = new Number((int64_t)c);
            return true;
        }
        unsigned char n = c;
        if (n < 0xC0) switch (n & 0xF0) {
            case 0x80:                  // fixmap
                v = readmap(i, c & 0x0F);
                return v ? true: false;
            case 0x90:                  // fixarr
                v = readarray(i, c & 0x0F);
                return v ? true: false;
            default:                    // fixstr
                v = readstr(i, c & 0x1F);
                return v ? true: false;
        } else switch (n) {
            case 0xC0:                  // Nil
                v = Value::Nil; return true;
            case 0xC2:                  // False
                v = Value::False; return true;
            case 0xC3:                  // True;
                v = Value::True; return true;
            case 0xC4:                  // bin 8
            case 0xC5:                  // bin 16
            case 0xC6:                  // bin 32
            case 0xC7:                  // ext 8
            case 0xC8:                  // ext 16
            case 0xC9:                  // ext 32
                return false;
            case 0xCA:// float 32
                if (!read(i, val.f)) return false;
                v = new Number(val.f); return true;
            case 0xCB:// float 64
                if (!read(i, val.d)) return false;
                v = new Number(val.d); return true;
            case 0xCC:// uint 8
                if (!read(i, val.ub)) return false;
                v = new Number((int64_t)val.ub); return true;
            case 0xCD:// uint 16
                if (!read(i, val.us)) return false;
                v = new Number((int64_t)val.us); return true;
            case 0xCE:// uint 32
                if (!read(i, val.ul)) return false;
                v = new Number((int64_t)val.ul); return true;
            case 0xCF:// uint 64
                if (!read(i, val.ull)) return false;
                v = new Number((int64_t)val.ull); return true;
            case 0xD0:// int 8
                if (!read(i, val.b)) return false;
                v = new Number((int64_t)val.b); return true;
            case 0xD1:// int 16
                if (!read(i, val.s)) return false;
                v = new Number((int64_t)val.s); return true;
            case 0xD2:// int 32
                if (!read(i, val.l)) return false;
                v = new Number((int64_t)val.l); return true;
            case 0xD3:// int 64
                if (!read(i, val.ll)) return false;
                v = new Number(val.ll); return true;
            case 0xD4:
            case 0xD5:
            case 0xD6:
            case 0xD7:
            case 0xD8:
                return false;
            case 0xD9:// str 8
                if (!read(i, val.ub)) return false;
                v = readstr(i, val.ub);
                return v ? true : false;
            case 0xDA:// str 16
                if (!read(i, val.us)) return false;
                v = readstr(i, val.us);
                return v ? true : false;
            case 0xDB:// str 32
                if (!read(i, val.ul)) return false;
                v = readstr(i, val.ul);
                return v ? true : false;
            case 0xDC:// array 16
                if (!read(i, val.us)) return false;
                v = readarray(i, val.us);
                return v ? true : false;
            case 0xDD:// array 32
                if (!read(i, val.ul)) return false;
                v = readarray(i, val.ul);
                return v ? true : false;
            case 0xDE:// map 16
                if (!read(i, val.us)) return false;
                v = readmap(i, val.ul);
                return v ? true : false;
            case 0xDF:// map 32
                if (!read(i, val.ul)) return false;
                v = readmap(i, val.ul);
                return v ? true : false;
            default:
                return false;
        }
    } else          // Positive fixint
        v = new Number((int64_t)c);
    return true;
}

}
