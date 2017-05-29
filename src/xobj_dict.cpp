#include <stdint.h>
#include "xobj_dict.h"
#include "xobj_str.h"

using namespace xobj;

namespace xobj {

static inline
bool isvalid(Dict::Node *node) { return node && !node->empty(); }

String *Dict::get(char *str, size_t len, hash_t hash) {
    auto node = getnode(hash);
    if (!isvalid(node)) return nullptr;
    auto p = node;
    do {
        auto k = p->key();
        if (k.isstr() && k.s().equals(str, len))
            return &k.s();
        p = p->next();
    } while (p != node);
    return nullptr;
}

void Dict::set(Value &k, Value &v) {
    if (k.isnil()) return;
    auto node = getnode(k);
    if (!node)
        node = newkey(k);
    node->value() = v;
}

Dict::Node *Dict::newkey(Value &k) {
    auto node = getnode(k.gethash(), true);
    if (node->empty())
        node->key() = k;
    else {
        auto idle = idlenode();
        if (!idle) {
            rehash();
            return newkey(k);
        }
        if (really(node))
            idle->key() = k,        // idle is the new node
            node->linkin(idle),     // link idle into node
            node = idle;
        else                        // node is occupied, but it shouldn't be here
            node->moveto(idle),
            node->init(k);
    }
    _size++;
    return node;
}

void Dict::rehash() {
    auto oitems = _items;                // old items
    auto ocap = _capacity;                // old capacity
    alloc(ocap ? 2 * ocap : 1);
    _size = 0;

    auto end = oitems + ocap;
    for (auto p = oitems; p < end; p++) {
        if (!p->empty())
            if (!p->value().isnil())
                set(p->key(), p->value());
    }
    delete[] oitems;
}

/* Tuple *Dict::items() { */
/*     int i = 0; */
/*     auto tp = Tuple::New(size); */
/*     auto end = _items + maxsize; */
/*     for (auto p = _items; p < end; p++) { */
/*         auto t = Value::tuple(2); */
/*         if (!p->empty()) { */
/*             t.set(0, p->key()); */
/*             t.set(1, p->value()); */
/*             tp->set(i++, t); */
/*         } */
/*     } */
/*     return tp; */
/* } */

/* Tuple *Dict::keys() { */
/*     int i = 0; */
/*     auto tp = Tuple::New(size); */
/*     auto end = _items + maxsize; */
/*     for (auto p = _items; p < end; p++) */
/*         if (!p->empty()) */
/*             tp->set(i++, p->key()); */
/*     return tp; */
/* } */
/* Tuple *Dict::values() { */
/*     int i = 0; */
/*     auto tp = Tuple::New(size); */
/*     auto end = _items + maxsize; */
/*     for (auto p = _items; p < end; p++) */
/*         if (!p->empty()) */
/*             tp->set(i++, p->value()); */
/*     return tp; */
/* } */

Dict::Node *Dict::idlenode() {
    while (_free)
        if (_items[--_free].empty())
            return &_items[_free];
    return nullptr;
}

}
