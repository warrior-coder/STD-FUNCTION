#include <iostream>

using std::cout;
using std::endl;



/*
реализация function используя только шаблоны
одним из значительных преимуществ шаблонов перед полиморфизмом является их способность сохранять тип
таким образом мы храним любой функтор в массиве байт, не задумываясь о его типе, а при вызове обращаемся к байтам с типом функтора
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
class function<T(Args...)>
{
    /*
    типы указателей на функции для поведения при удалении типов
    char* параметры выводятся из типа функтора
    */
    using invoke_f_t = T(*)(char*, Args...);
    using construct_f_t = void(*)(char*, char*);
    using destroy_f_t = void(*)(char*);

    invoke_f_t invoke_f;
    construct_f_t construct_f;
    destroy_f_t destroy_f;

    /*
    универсальные функции с учетом типов для вызова
    специализация этих функций не соответствует по типу с вышеуказанными типами функций, поэтому будем reinterpret_cast
    */
    template<typename F>
    static T invoke(F* f, Args... args)
    {
        return (*f)(args...);
    }

    template<typename F>
    static void construct(F* f, F* f_src)
    {
        new (f) F(*f_src); // конструируем копию функтора на выделенном участке памяти
    }

    template<typename F>
    static void destroy(F* f)
    {
        f->~F(); // уничтожаем объект на выделенном участке памяти
    }

    /*
    стираем тип любого функтора и храним его в массиве байт (char*)
    поэтому необходимо также хранить размер функтора
    */
    int data_size;
    char* data_ptr;

public:
    /*
    конструктор от любого типа функтора
    приводим типы функторов к массиву байт (char*) с помощью reinterpret_cast
    таким образом стираем их тип
    */
    template<typename F>
    function(F f) :
        invoke_f(reinterpret_cast<invoke_f_t>(function::invoke<F>)),
        construct_f(reinterpret_cast<construct_f_t>(function::construct<F>)),
        destroy_f(reinterpret_cast<destroy_f_t>(function::destroy<F>)),
        data_size(sizeof(F)),
        data_ptr(new char[data_size]) // выделяем динамическую память под функтор
    {
        construct_f(data_ptr, reinterpret_cast<char*>(&f)); // конструируем копию функтора на выделенном участке памяти
    }

    ~function()
    {
        if (data_ptr)
        {
            destroy_f(data_ptr); // уничтожаем объект на выделенном участке памяти
            delete[] data_ptr; // освобождаем выделенную под функтор память
        }
    }

    T operator()(Args... args)
    {
        return invoke_f(data_ptr, args...);
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
    cout << f(x, 3) << endl;

    function<void(int&)> h = inc;
    h(x);
    cout << x << endl;

    function<double()> g = [x, &y]() { return x * y; };
    cout << g() << endl;


    return 0;
}