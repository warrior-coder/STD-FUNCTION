#include <iostream>

using std::cout;
using std::endl;



/*
function – полиморфная обертка любого функционального объекта
в основе реализации function лежит стирание типов (type erasure)
это означает, что он стирает детали того, как происходят некоторые операции, и предоставляет единый интерфейс
*/



/*
общая реализация шаблона отсутствует, вся работа происходит в частичной специализации
*/
template<typename F>
class function;

/*
частичная специализация нужна для того чтобы мы могли использовать сигнатуру функции
*/
template<typename T, typename... Args>
class function <T(Args...)>
{
	struct base
	{
		virtual ~base() {} // виртуальный деструктор должен быть объявлен у любого полиморфного типа
		
		virtual T invoke(Args... args) = 0;
	};


	template<typename F>
	struct holder : public base
	{
		F f;

		holder(F f) : base(), f(f) {}

		T invoke(Args... args) override
		{
			return f(args...);
		}
	};

	base* invoker = nullptr;

public:
	template<typename F>
	function(F f)
	{
		invoker = new holder<F>(f);
	}

	/*
	т.к. деструктор base виртуальный, то вызывается деструктор типа на который указывает invoker
	вызовется деструктор holder, вместе с которым уничтожатся поля holder
	*/
	~function()
	{
		delete invoker;
	}

	T operator()(Args... args)
	{
		return invoker->invoke(args...);
	}
};


int sum(int x, int y)
{
	return x + y;
}

void inc(int& x)
{
	x++;
}


int main()
{
	int x = 1, y = 2;

	function<int(int, int)> f = sum;
	cout << f(x, 2) << endl; // 3

	function<void(int&)> h = inc;
	h(y);
	cout << y << endl; // 3

	function<double()> g = [x, &y]() { return x * y; };
	cout << g() << endl; // 3


	return 0;
}