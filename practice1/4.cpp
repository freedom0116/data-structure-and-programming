#include <iostream>
using namespace std;

int main()
{
    int a = 10;
    int &r = a;
    cout << "&a = " << &a << endl;
    cout << "&r = " << &r << endl;
    int b = 20;
    r = b;
    cout << "&b = " << &b << endl;
    cout << "&r = " << &r << endl;
    cout << "a = " << a << endl;
    cout << "b = " << b << endl;
    cout << "r = " << r << endl;
}