#ifndef TYPEINDEX_H
#define TYPEINDEX_H

typedef int typeindex;

template<typename T>
struct TypeInfo
{
    static typeindex Index()
    {
        static typeindex id = _NewId();
        return id;
    }
private:
};

template<typename T>
typeindex type_index()
{
    return TypeInfo<T>::Index();
}

template<typename T>
const char* type_name()
{
    return "unknown_type";
}
#define DEF_TYPE_NAME(NAME) \
template<> \
const char* type_name<NAME>() \
{ \
    return #NAME; \
}

template<typename T>
typeindex GetTypeIndex(T value)
{
    return TypeInfo<T>::Index();
}

inline typeindex _NewId()
{
    static typeindex id;
    return ++id;
}

#endif
