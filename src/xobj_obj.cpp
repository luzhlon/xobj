#include <stdint.h>
#include "xobj_num.h"
#include "xobj_str.h"
#include "xobj_list.h"
#include "xobj_dict.h"

namespace xobj {

Bool Bool::True(true);
Bool Bool::False(false);
Object  Object::Nil(TV_NIL);

void Object::refDec() {
    _refs -= 1;
    if (_refs) return;
    switch (_type) {
    case TV_NUMBER:
        delete (Number *)this;
        break;
    case TV_STRING:
        String::Delete((String *)this);
        break;
    case TV_LIST:
        delete (List *)this;
        break;
    case TV_DICT:
        delete (Dict *)this;
        break;
    }
}

}
