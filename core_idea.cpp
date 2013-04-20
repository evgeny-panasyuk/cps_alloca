#include <iostream>
#include <vector>

using namespace std;

void use(void *p)
{
    static auto volatile t=p;
};

template<typename T,unsigned N,typename F>
auto cps_alloca_static(F &&f) -> decltype(f(nullptr,nullptr))
{
    T data[N];
    return f(&data[0],&data[0]+N);
}

template<typename T,typename F>
auto cps_alloca_dynamic(unsigned n,F &&f) -> decltype(f(nullptr,nullptr))
{
    vector<T> data(n);
    return f(&data[0],&data[0]+n);
}

template<typename T,typename F>
auto cps_alloca(unsigned n,F &&f) -> decltype(f(nullptr,nullptr))
{
    switch(n)
    {
        case 1: return cps_alloca_static<T,1>(f);
        case 2: return cps_alloca_static<T,2>(f);
        case 3: return cps_alloca_static<T,3>(f);
        case 4: return cps_alloca_static<T,4>(f);
        case 0: return f(nullptr,nullptr);
        default: return cps_alloca_dynamic<T>(n,f);
    }; // mpl::for_each / array / index pack / recursive bsearch / etc variacion
}

struct Payload
{
    char d[1 << 14];
};

void test(unsigned n)
{
    volatile char begin;
    cps_alloca<Payload>(n,[&](Payload *first,Payload *last)
    {
        volatile char end;
        const auto overhead = &begin-&end - int(n*sizeof(Payload));
        cout << "n=" << n << "\toverhead=" << overhead << endl;
        use(first); use(last);
    });
}

int main()
{
    for(auto i=1u;i!=6u;++i)
        test(i);
}
