#include <iostream>
#include <boost/type_index.hpp>

using namespace std;
using boost::typeindex::type_id_with_cvr;

template<class T>
void f(std::initializer_list<T> a) {
	return;
}

template<typename T>
void f(T &param)
{
	using std::cout;
	using boost::typeindex::type_id_with_cvr;
	cout << "T = " << type_id_with_cvr<T>().pretty_name() << endl;
	cout << "param = " << type_id_with_cvr<decltype(param)>().pretty_name() << endl;
}

template<typename T>
void cf(const T &param)
{
	using std::cout;
	using boost::typeindex::type_id_with_cvr;
	cout << "T = " << type_id_with_cvr<T>().pretty_name() << endl;
	cout << "param = " << type_id_with_cvr<decltype(param)>().pretty_name() << endl;
}

int main() {
    int x = 27;
    const int cx = x;
    const int &rx = x;
    f(x);
    f(cx);
    f(rx);
    cf(x);
    cf(cx);
    cf(rx);

    return 0;
}
