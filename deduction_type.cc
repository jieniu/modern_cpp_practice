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
	cout << "[Case 1] (T &param) T = " << type_id_with_cvr<T>().pretty_name() << endl;
	cout << "[Case 1] (T &param) param = " << type_id_with_cvr<decltype(param)>().pretty_name() << endl;
}

template<typename T>
void cf(const T &param)
{
	cout << "[Case 1] (const T &param) T = " << type_id_with_cvr<T>().pretty_name() << endl;
	cout << "[Case 1] (const T &param) param = " << type_id_with_cvr<decltype(param)>().pretty_name() << endl;
}

// 在编译时返回一个数组长度，这里只关注数组长度，所以忽略了数组名
// constexpr关键字使结果在编译期可用
template<class T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept {
    return N;
}

template<class T>
void rf(T &&param) 
{
	cout << "[Case 2] (T &&param) T = " << type_id_with_cvr<T>().pretty_name() << endl;
	cout << "[Case 2] (T &&param) param = " << type_id_with_cvr<decltype(param)>().pretty_name() << endl;
}

template<class T>
void cbvf(T param) 
{
	cout << "[Case 3] (T param) T = " << type_id_with_cvr<T>().pretty_name() << endl;
	cout << "[Case 3] (T param) param = " << type_id_with_cvr<decltype(param)>().pretty_name() << endl;
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

    int kv[] = {1,3,7,9,11,22,35};
    std::array<int, arraySize(kv)> arrays;
    cout << "[Case 1] (T (&)[N]) array len = " << arrays.size() << endl;

    rf(x);
    rf(cx);
    rf(rx);
    rf(27);

    cbvf(x);
    cbvf(cx);
    cbvf(rx);
    const char *const cptr = "Fun with pointers";
    cbvf(cptr);

    return 0;
}
