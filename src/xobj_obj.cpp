#include "xobj_val.h"
#include "xobj_obj.h"
#include "xobj_num.h"

namespace xobj {

Bool Bool::True(true);
Bool Bool::False(false);

Value Value::Nil;
Value Value::True(true);
Value Value::False(false);

Value::Value(bool b) {
    refer(b ? &Bool::True : &Bool::False);
}

bool Bool::operator==(const Value &v) const {
    return v.isbool() && val() == v.b().val();
}

Value::Value(int64_t i) { refer(new Int(i)); }
Value::Value(double n) { refer(new Float(n)); }

Value P(void *p) { return V((int64_t)p); }

}
