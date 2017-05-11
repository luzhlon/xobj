
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
uint32_t _htonl(uint32_t l) { return htonl(l); }
uint64_t _htonll(uint32_t ll) { return htonll(ll); }

uint32_t _htonf(float f) { return htonf(f); }
uint64_t _htond(double f) { return htond(f); }

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
                    buf[0] = 0xD3, WRITE64(n->i, &buf[1]); o.write(buf, 9);
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
        buf[0] = 0xCB, *(uint64_t *)&buf[1] = _htond(n->f),
            o.write(buf, 9);
}

static void toMsgPack(String *s, std::ostream& o) {
    char buf[10];
    //if (s->len() < 0x20)
    //    buf[0] = (0xA0 & s->len()) & 0xFF,
    //    o.write(buf, 1);
    //else
    if (s->len() < 0x100)
        buf[0] = 0xC4, buf[1] = s->len(),
        o.write(buf, 2);
    else if (s->len() < 0x10000)
        buf[0] = 0xC5, WRITE16(s->len(), &buf[1]),
        o.write(buf, 3);
    else if (s->len() < 0x100000000)
        buf[0] = 0xC5, WRITE32(s->len(), &buf[1]),
        o.write(buf, 5);
    o.write(s->c_str(), s->len());
}

namespace xobj {

void toMsgPack(std::ostream& o, Value& v) {
    switch (v.type()) {
    case TV_NIL:
        o << "\xC1";
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

}
