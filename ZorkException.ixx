module;

#include <exception>
#include <string>
export module ZorkException;

class ZorkException : public std::exception
{
public:
    ZorkException(const char* msg) : _what(msg) {}

    const char* what() const noexcept override { return _what.c_str(); }

private:
    std::string _what;
};

export inline void error(const char* msg)
{
    throw ZorkException(msg);
}


