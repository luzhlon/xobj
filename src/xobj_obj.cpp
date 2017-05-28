#include "xobj_val.h"
#include "xobj_obj.h"

namespace xobj {

bool Bool::operator==(Value &v) const {
    return v.isbool() && val() == v.b().val();
}

}
