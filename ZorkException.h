#pragma once
class ZorkException : public std::exception
{
public:
    ZorkException(const char *msg) : _what(msg) {}

    const char *what() const noexcept override { return _what.c_str(); }

private:
    std::string _what;
};

inline void error(const char *msg)
{
    throw ZorkException(msg);
}


