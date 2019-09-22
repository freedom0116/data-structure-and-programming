#include <iostream>
using namespace std;

class C
{
public:
    int a, b, c;
};

int main()
{
    int x;
    C *c1 = new C;
    cout << "sizeof x is " << sizeof(x) << endl;
    cout << "sizeof c1 is " << sizeof(c1) << endl;
}