#include <iostream>
#include <sstream>
#include <string>

#include "xobj_val.h"
#include "xobj_list.h"
#include "xobj_dict.h"
#include "xobj_mp.h"
#include "xobj_str.h"

#include "tstream.h"

using namespace xobj;
using namespace std;

class Test {
public:
    // test msgpack converting
    void test_msgpack() {
        tstream ts;
        if (!ts.connect("127.0.0.1", 5000)) {
            cout << "Connect failure\n"; return;
        }
        Value data;
        if (!mp_unpack(data, ts)) {
            cout << "Receive data failure\n"; return;
        }
        cout << data << endl;
        if (!mp_pack(data, ts))
            cout << "Send data failure\n";
    }
    // test string internal
    void test_internal() {
        auto s1 = CS("abc");
        Value s2("abc");
        cout << "Test string internal:\n\t"
            << &s1.o() << ' ' << &s2.o() << ' ' << (&s1.s() == &s2.s()) << endl;
    }
    // test list
    void test_list() {
        auto l = L("1", "2", "3", 4, 5, 6, true);
        l += 7;
        cout << "Test List: " << l << endl;
        cout << "Test List Iterator: " << endl;
        //for (auto i = l.list()->begin(); i != l.list()->end(); i++)
        for (auto i : l.l())
            cout << i << endl;
        cout << endl;
    }
    // test dictionary
    void test_dict() {
        auto d = D("a", 1,
            "b", 2,
            "c", 3,
            "d", 4);
        cout << "Test Dictionary: " << d << endl;
        cout << "Dictionary Iterator:" << endl;
        for (auto i : d.d())
            cout << i.key() << ':' << i.value() << endl;
        cout << endl;
        d.clear();
        d.set("ea", 5);
        d.set(1, "1");
        cout << "Clear Dictionary: " << d << endl;
        cout << "d.ea = " << d.get("ea") << endl;
        cout << "d[1] = " << d[1] << endl;
        cout << "d[2] = " << d[2] << endl;
    }

    void test_pointer() {
        int n = 1;
        auto p = P(&n);
        auto l = (int *)p.p();
        cout << "Test pointer:" << *l << endl;
    }
};

int main(int argc, char **argv) {
    auto test = new Test;
    test->test_pointer();
    test->test_internal();
    test->test_list();
    test->test_dict();
    test->test_msgpack();
    getchar();
    return 0;
}
