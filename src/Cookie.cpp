#include "Cookie.hpp"
#include "Macros.hpp"

std::string		Cookie::setRandCookie()
{
    std::string cookie;
    size_t      cookie_len = 32;
    std::string rand_arr = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFIHIJKLMNOPQRSTUVWXYZ";
    size_t      len = rand_arr.length();

    std::srand(std::time(NULL));
    while (cookie_len-- > 0)
        cookie += rand_arr[rand() % len];
    return (cookie);
}
