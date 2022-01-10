# STD FUNCTION

function – полиморфная обертка любого функционального объекта.

```CPP
int x = 1, y = 2;

function<int(int, int)> f = sum;
cout << f(x, 2) << endl; // 3

function<void(int&)> h = inc;
h(y);
cout << y << endl; // 3

function<double()> g = [x, &y]() { return x * y; };
cout << g() << endl; // 3
```