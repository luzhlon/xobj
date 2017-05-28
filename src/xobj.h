#ifndef __XOBJ_H__
#define __XOBJ_H__

#include <stdint.h>

namespace xobj {

struct Object;
struct Int;
struct Float;
struct Bool;
struct String;
struct List;
struct Tuple;
struct Dict;

enum type_t {
    TV_NIL = 0, TV_BOOL,
    TV_INT, TV_STRING,
    TV_FLOAT, TV_TUPLE,
    TV_LIST, TV_DICT
};

typedef uint32_t hash_t;

}

#endif /* __XOBJ_H__ */
