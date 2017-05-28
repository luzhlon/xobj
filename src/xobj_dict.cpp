#include <stdint.h>
#include "xobj_dict.h"
#include "xobj_str.h"

using namespace xobj;

namespace xobj {

static inline
bool isvalid(Dict::Node *node) { return node && !node->empty(); }

String *Dict::get(char *str, size_t len, hash_t hash) {
    auto node = HashNode(hash);
    if (!isvalid(node)) return nullptr;
    auto p = node;
    do {
        auto k = p->key();
        if (k.isstr() && k.s().equals(str, len))
            return &k.s();
        p = p->Next();
    } while (p != node);
    return nullptr;
}

void Dict::set(Value &k, Value &v) {
    if (k.isnil()) return;
    auto node = NodeByKey(k);
    if (!node)
        node = NewKeyNode(k);
    node->value() = v;
}

Dict::Node *Dict::NewKeyNode(Value &k) {
    checkitems();
    auto node = HashNode(k);
    if (node->empty())
        node->key() = k;
    else {
        auto idle = GetIdleNode();
        if (!idle) {
            rehash();
            return NewKeyNode(k);
        }
        if (Really(node)) {
            // 需要将新节点链入
            idle->set(k);
            Link(node, idle);
            node = idle;
        } else {
            // 该节点已被占用，但不该在这里，需要移动该节点
            MoveNode(node, idle);
            node->init(k);
        }
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

Dict::Node *Dict::GetIdleNode() {
    while (_free)
        if (_items[--_free].empty())
            return &_items[_free];
    return nullptr;
}

Dict::Node *Dict::NodeByKey(Value &k) {
    auto node = HashNode(k);
    return isvalid(node) && Really(node) ?
                FindNode(node, k) : nullptr;
}

void Dict::Link(Node *o, Node *n) {
    n->Prev(o);
    n->Next(o->Next());
    o->Next()->Prev(n);
    o->Next(n);
}

void Dict::MoveNode(Node *src, Node *dst) {
    auto nx = src->Next();
    auto pr = src->Prev();
    *dst = *src;
    dst->Next(nx);
    dst->Prev(pr);
    nx->Prev(dst);
    pr->Next(dst);
}

Dict::Node *Dict::FindNode(Node *n, Value &k) {
    auto p = n;
    do {
        if (p->key() == k)
            return p;
        p = p->Next();
    } while (p != n);
    return nullptr;
}

std::ostream& operator<<(std::ostream& o, const Dict& d) {
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

}
