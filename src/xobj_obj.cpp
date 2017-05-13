#include <stdint.h>
#include "xobj_num.h"
#include "xobj_str.h"
#include "xobj_list.h"
#include "xobj_dict.h"

namespace xobj {

Bool Bool::True(true);
Bool Bool::False(false);
Object  Object::Nil(TV_NIL);

}
