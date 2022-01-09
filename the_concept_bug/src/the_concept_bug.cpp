#include <concepts>
#include <iostream>
#include <string>

// This file shows a current compiler bug that occurs in two of the major C++20 compilers.

// in_body_t
template <typename T>
class in_body_t
{
public:
    static std::string Type_Name()
    {
        return "(none)";
    }
};

template <std::same_as<short> T>
class in_body_t<T>
{
public:
    static std::string Type_Name()
    {
        return "short";
    }
};

template <std::same_as<long> T>
class in_body_t<T>
{
public:
    static std::string Type_Name()
    {
        return "long";
    }
};

template <std::same_as<long long> T>
class in_body_t<T>
{
public:
    static std::string Type_Name()
    {
        return "long long";
    }
};

// out_of_body_t
template <typename T>
class out_of_body_t
{
public:
    static std::string Type_Name();
};
template <typename T>
std::string out_of_body_t<T>::Type_Name()
{
    return "(none)";
}

#define A 1
#define B 0
#define C 0
// here's the bug:
//      In x86-64 msvc with the /std:c++20 flag, you can only define one of these at a time.
//      In x86-64 clang 13.0.0 with the -std=c++20, you cannot define any of them at all.
//      In x86-64 gcc 11.2 with the -std=c++20 flag, you can define all of them.
//      (tested using https://godbolt.org/)
#if A
template <std::same_as<short> T>
class out_of_body_t<T>
{
public:
    static std::string Type_Name();
};
template <std::same_as<short> T>
std::string out_of_body_t<T>::Type_Name()
{
    return "short";
}
#endif
#if B
template <std::same_as<long> T>
class out_of_body_t<T>
{
public:
    static std::string Type_Name();
};
template <std::same_as<long> T>
std::string out_of_body_t<T>::Type_Name()
{
    return "long";
}
#endif
#if C
template <std::same_as<long long> T>
class out_of_body_t<T>
{
public:
    static std::string Type_Name();
};
template <std::same_as<long long> T>
std::string out_of_body_t<T>::Type_Name()
{
    return "long long";
}
#endif

// work_around_t
// create any combination of different types and templates for each specialization
template <typename T>
class work_around_integral_t
{
public:
    static std::string Type_Name();
};
template <typename T>
std::string work_around_integral_t<T>::Type_Name()
{
    return "(none)";
}

class work_around_short_t
{
public:
    static std::string Type_Name();
};
std::string work_around_short_t::Type_Name()
{
    return "short";
}

template <std::same_as<long> T>
class work_around_long_t
{
public:
    static std::string Type_Name();
};
template <std::same_as<long> T>
std::string work_around_long_t<T>::Type_Name()
{
    return "long";
}

template <std::same_as<long long> T>
class work_around_long_long_t
{
public:
    static std::string Type_Name();
};
template <std::same_as<long long> T>
std::string work_around_long_long_t<T>::Type_Name()
{
    return "long long";
}

// create a single template that resolves to each specialization
template <typename T>
class work_around_specialization_t
{
public:
    using type = work_around_integral_t<T>;
};

template <std::same_as<short> T>
class work_around_specialization_t<T>
{
public:
    using type = work_around_short_t;
};

template <std::same_as<long> T>
class work_around_specialization_t<T>
{
public:
    using type = work_around_long_t<T>;
};

template <std::same_as<long long> T>
class work_around_specialization_t<T>
{
public:
    using type = work_around_long_long_t<T>;
};

// define your actual type with an alias
template <typename T>
using work_around_t = typename work_around_specialization_t<T>::type;

int main()
{
    std::cout << in_body_t<short>::Type_Name() << std::endl;
    std::cout << in_body_t<long>::Type_Name() << std::endl;
    std::cout << in_body_t<long long>::Type_Name() << std::endl;
    std::cout << std::endl;

    std::cout << out_of_body_t<short>::Type_Name() << std::endl;
    std::cout << out_of_body_t<long>::Type_Name() << std::endl;
    std::cout << out_of_body_t<long long>::Type_Name() << std::endl;
    std::cout << std::endl;

    std::cout << work_around_t<short>::Type_Name() << std::endl;
    std::cout << work_around_t<long>::Type_Name() << std::endl;
    std::cout << work_around_t<long long>::Type_Name() << std::endl;
    std::cout << std::endl;

    return 0;
}
