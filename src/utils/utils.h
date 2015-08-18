#ifndef UTILS_H
#define UTILS_H

namespace kronos
{

constexpr unsigned int cstr2int(const char* str, int h = 0)
{
    return !str[h] ? 5381 : (cstr2int(str, h+1)*33) ^ str[h];
}

}

#endif // UTILS_H
