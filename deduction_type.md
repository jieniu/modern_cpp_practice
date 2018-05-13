## 概念 or 忠告
1. The distinction between arguments and parameters is important, because parameters are lvalues, but the arguments with which they are initialized may be rvalues or lvalues
形参（parameter，函数头声明的参数）和实参（argument，或传参）的差异在C++11中变得格外重要，因为引入了左值和右值，形参是左值，但实参既可以是左值，又可以是右值。**参数在传递时，脑海中通常也会模拟一个等式，左边是形参，而右边是实参。**
2. I define a function’s signature to be the part of its declaration that specifies parameter and return types. Function and parameter names are not part of the signature.
约定：函数的签名仅包括返回值、参数列表
3. Sometimes a Standard says that the result of an operation is undefined behavior. That means that runtime behavior is unpredictable
什么是未定义行为：即它的运行时行为是不可预测的
4. 
| Term I Use        | Language Version I Mean |
| --------   | -----|
| C++       | All      | 
| C++98 | C++98 or C++03 |
| C++11 | C++11 and C++14 |
| C++14 | C++14 |   
约定：当你说不同C++的代号时，分别对应着哪个版本
特性：
* C++以性能著称
* C++98缺乏并行性（concurrency）
* C++11支持lambda表达式，C++14支持通用的函数返回推断
* C++11最为广泛的影响即`move`语义
5. as a modern C++ developer, you’d naturally prefer a std::array to a built-in array
作为一个高级C++程序员，更应该选择使用`std::array`

## 类型推断
- 好处：无需重复的拼写显而易见的类型
- 坏处：让代码更难懂，编译器行为并不是那么直观

如果不能理解类型推断操作，在高级C++中高效编程几乎不可能实现，因为类型推断无处不在。

### #1 理解模板类型推断
假设有以下模板定义
```
template<class T>
void f(ParamType param);
```
以及如下调用
```
f(expr);
```
编译器会通过`expr`来推断`T`和`ParamType`的类型，除了`expr`之外，`ParamType`的形式也很重要，`ParamType`的形式分3种情况
1. `ParamType`是一个指针或引用类型，但不是一个universal reference
如果`expr`是一个引用类型，则首先去引用，再根据`ParamType`的类型，来推断出`T`的类型，例如
```
template<class T>
void f(T &param);
// ...
int x = 27;
const int cx = x;
const int &rx = x;
f(x); // T为int，ParamType为int &
f(cx);// T为const int，ParamType为const int &
f(rx);// T为const int，ParamType为const int &
```
可以看到，第2和第3个参数为`T`赋予了`const`属性，这对调用者来说是非常重要的的，可以利用此保证外部引用参数不被修改，即**传`const引用的对象`给参数为`T&`类型的模板是安全的**  
如果将`T &`改为`const T &`，则实参的`const`属性也会去掉
```
template<class T>
void f(const T &param);
// ...
int x = 27;
const int cx = x;
const int &rx = x;
f(x); // T为int，ParamType为const int &
f(cx);// T为int，ParamType为const int &
f(rx);// T为int，ParamType为const int &
```
你也可以通过[下载github上的代码](https://github.com/jieniu/modern_cpp_practice/blob/master/deduction_type.cc#L4:17)来验证运行结果。

如果把引用`&`换成指针`*`，以上规则仍然生效。

1.1 传入数组
C++一般不可以在函数参数中定义数组类型，取而代之的是将数组的第一个元素的地址作为参数，但模板参数的引用形式将数组参数成为可能，例如
```
template<class T>
void f(T &param);
// ...
const char name[] = "J. P. Briggs";
f(name); // T为const char [13]，ParamType为const char (&)[13]
```
在此场景下，编译器可以推断数组的长度，我们可以重新定义模板，以在编译时获得数组长度
```
// 在编译时返回一个数组长度，这里只关注数组长度，所以忽略了数组名
// constexpr关键字使结果在编译期可用
template<class T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept{
    reutrn N;
}

int keyVals[] = { 1, 3, 7, 9, 11, 22, 35 };
std::array<int, arraySize(keyVals)> mappedVals; // 数组长度为7
```

2. `ParamType`是universal reference（参数声明类型为T&&）
a) 当参数为左值时，`T`被推断为左值引用，这也是唯一的`T`被推断为左值引用的情况（其他情况下，引用会被去掉）。  
b) 即便参数为右值引用，当参数为左值时，`ParamType`也仍然为左值引用
c) 在参数为右值时，规则和第1条规则保持一致
```
template<class T>
void f(T &&param);
// ...
int x = 27;
const int cx = x;
const int &rx = x;
f(x); // T为int &，ParamType为int &
f(cx);// T为const int &，ParamType为const int &
f(rx);// T为const int &，ParamType为const int &
f(27); // 因为27为右值T为int，ParamType为int &&
```
3. `ParamType`既不是引用，也不是指针类型——pass-by-value
a) 如果`expr`是一个引用，忽略引用部分
b) 如果`expr`有`const`或`volatile`属性，也忽略这一部分
```
template<class T>
void f(T param);
// ...
int x = 27;
const int cx = x;
const int &rx = x;
f(x); // T为int，ParamType为int
f(cx);// T为int，ParamType为int
f(rx);// T为int，ParamType为int
```
考虑更复杂的情况，如果实参是一个`const`类型的指向常量字符串的指针，那么当其进行`copy-by-value`传参时，会发生什么情况
```
const char * const ptr = "Fun with pointers";
f(ptr); // T为const char *，ParamType为const char *
```
意为字符串的`const`属性被保留，但指针的`const`（`const`在`*`号右边）属性被移除。记住：**只有在copy-by-value条件下，const性质才会被移除**

如果给`copy-by-value`的模板传入一个数组，会被解析成什么样子？和`ParamType`为引用类型不一样的是，参数仍然会被推断为指针类型
```
const char name[] = "J. P. Briggs";
f(name); // ParamType被推断为const char *
```

### #2 理解`auto`类型推断
> auto type deduction is template type deduction.
> auto类型推断就是template类型推断

auto类型推断和template类型推断之间有一个映射关系，以下表格第一列和第二列等价

| template类型推断 | auto类型推断|
| ---| --- | 
| `template<class T>`<br>`void func_for_x(T param);`<br><br>`func_for_x(x);` | `auto x = 27;` | 
| `template<class T>`<br>`void func_for_cx(const T param);`<br><br>`func_for_cx(cx);` | `const auto cx = x;`| 
| `template<class T>`<br>`void func_for_rx(const T &param);`<br><br>`func_for_rx(rx)`; | `const auto &rx = x;`| 

**上表可以看出，`auto`关键字对应模板中的类型`T`，而等式左边除变量以外的部分对应着模板参数中的`ParamType`**；`auto`类型推断和模板类型推断一样，也满足3种条件
1. 参数类型为指针或引用，但不是universal reference
```
auto x = 7; // x的类型为int
auto &rx = x; // rx的类型为int &
```
2. 参数类型为universal reference
```
auto x = 7;
const int cx = 7;
auto &&uref1 = x;  // uref1的类型为int &
auto &&uref2 = cx; // uref2的类型为const int &
auto &&uref3 = 27; // uref3的类型为int &&
```
3. 参数类型既为pass-by-value
```
auto x = 7;
```
**等式右边如果是一个数组**，`auto`的行为也和模板类型推断一样
```
const char name[] = "J. P. Briggs";
auto pname = name;  // pname的类型为const char *
auto &rname = name; // rname的类型我const char (&)[13]
```
**等式右边如果是大括号初始化列表**，左边的推断类型为`std::initializer_list`
```
// x3和x4的类型为std::initializer_list<int>
auto x3 = {27};
auto x4{27};
```
这里实际上有两层类型推断，第一层为判定`x3`的类型为`std::initializer_list<T>`，第二层为根据`27`判断`T`的类型为`int`，所以如果大括号中的数据类型不一致，编译器将会报错，例如
```
auto x5 = {1, 2.0}; // cannot deduce actual type for variable 'x5' with type 'auto' from initializer list
auto x6 = {1,2};    // ok
```
**上述推断也是`auto`和`template`类型推断的唯一不同之处**，如果你这样向一个模板类型参数传参，编译器会报错
```
template<class T>
void f(T arg);

f({1,2}); // wrong. candidate template ignored: couldn't infer template argument 'T'
```
因为编译器对待`template`类型推断，不会像`auto`一样，进行两次推断，所以要想顺利编译通过，要对模板函数修改如下：
```
template<class T>
void f(std::initializer_list<T> arg);
```
`auto`在`C++11`中的规则到这里就结束了，但在`C++14`中并没有，**`C++14`允许函数的返回类型或`lambda`表达式参数使用`auto`推断，但这里的规则却是`template`的推断方式**，举两个例子就清楚了
```
// case 1
auto initial() {
  return {1,2}; // error: can't deduce type for {1,2}
}
// case 2
std::vector<int> v;
auto resetV = [&v](const auto& newValue)  { v = newValue; }; 
resetV({1,2,3}); // error: can't deduce type for {1,2,3}
```
很奇怪是不是，这个规则没有特别的原因——**Rule is the rule!**

### #3 理解`decltype`
**decltype** - 输入一个名字或表达式，`decltype`会告诉你该输入的类型。**在C++11中，`decltype`常被用在模板函数的返回类型中，这个返回类型通常由模板的参数决定**，例如，我们想让函数具备和`operator[]`一样，返回容器内具体元素的引用，可以这样声明模板：
```
template<typename Container, typename Index>
auto authAndAccess(Container &c, Index i) 
  -> decltype(c[i])
{
  return c[i];
}
```
以上代码，`auto`不会起到任何作用，函数的返回类型被C++11的尾部返回类型（trailing return type）取代，这种方式的好处是，**声明的返回类型可以依赖于函数的参数**，如果像传统方式一样，把它们写在函数名之前，会报「c和i没有被声明」的错。

**但值得注意的是，C++14不需要这样，仅靠`auto`即可实现返回类型推断**，但根据`#1`和`#2`的规则，返回的值会失去引用属性，则下面的用法会报错，因为右值不可以被赋值
```
std::deque<int> d;
...
authAndAccess(d, 5) = 10;
```

要让`authAndAccess`函数和`[]operator()`的返回类型一致，就需要借助`decltype`，模板声明要修改如下
```
template<typename Container, typename Index>
decltype(auto)
authAndAccess(Container &c, Index i);
```

除此之外，`decltype`还被用作等式赋值操作中，例如
```
const int &i = 10;
auto x = i; // x 为int类型
decltype(auto) dx = i; // dx为const int &类型
```

但如果还想让`authAndAccess`函数接受右值，满足对临时容器元素的复制需求，如
```
std::deque<std::string> makeStringDeque(); // 工厂函数
auto s = authAndAccess(makeStringDeque(), 5);
```
又该如何修改`authAndAccess`呢？这里就要`universal reference`派上用场了。
```
template<typename Container, typename Index>
decltype(auto) authAndAccess(Container &&c, Index i)
{
  return std::forward<Container>(c)[i];
}
```

而如果在C++11的环境下，以上代码需要手动声明返回类型
```
template<typename Container, typename Index>
auto
authAndAccess(Container &&c, Index i)
-> decltype(std::forward<Container>(c)[i])
...
```

在使用`decltype(auto)`时，还有一点需要注意，当需要推断的参数是一个表达式，而不仅仅是一个名字时，`decltype`会将其推断为具体类型的引用（T &），尤其值得注意的是，变量`x`和`(x)`具有不同的性质，前者是一个变量名，而后者被解释为表达式，所以假设`x`是`int`类型的情况下，`decltype(x)`被推断为`int`，而`decltype((x))`被推断为`int &`，例如
```
int ix = 10;
decltype(auto) dix = ix;
dix = 11;
cout << "ix:" << ix << endl; // ix: 10
decltype(auto) deix = (ix);
deix = 11;
cout << "ix:" << ix << endl; // ix: 11
```
于是当`decltype(auto)`作为函数的返回类型时，`return`语句需要小心对待，尤其是`return`一个局部变量时，千万不能将其用圆括号括起来，否则你会返回一个临时对象的引用，你的程序的行为就是未定义的。

### #4 了解如何查看推断类型
比较靠谱的获取对象类型的方法是使用Boost.TypeIndex库（<boost/type_index.hpp>），使用其中的`boost::typeindex::type_id_with_cvr<T>()`模板，该模板接受的参数即为你想要诊断的变量或表达式的类型，`cvr`代表`const`、`volatile`以及`reference`，表示这三种类型不会被省略。
```
template<typename T>
void f(const T &param)
{
  using std::cout;
  using boost::typeindex::type_id_with_cvr;
  cout << "T = " << type_id_with_cvr<T>().pretty_name() << endl;
  cout << "param = " << type_id_with_cvr<decltype(param)>().pretty_name() << endl;
}
```

本文是对《effective Modern C++》一书的第一章节的笔记，想深入学习该内容的同学还请以原书为准。

参考：
《effective modern C++》by Scott Meyes
