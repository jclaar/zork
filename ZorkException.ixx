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

export [[noreturn]] inline void error(const char* msg)
{
    throw ZorkException(msg);
}

// This exception is thrown when the user has quit or restart. 
// This attempts to mimic the behavior of the QUIT MDL function,
// which is just an immediate exit of the running application.
// (Probably exit() would do the same thing, but I hate exit(). :-) )
export class ExitException : public std::exception {
public:
    ExitException(bool restart_flag) : restart(restart_flag) {}

    bool restart_flag() const { return restart; };

private:
    bool restart;
};
