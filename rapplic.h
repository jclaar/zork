#pragma once
#include <optional>
#include <variant>
#include <functional>
#include "zorkfwd.h"
// Hacky method of allowing an additional argument to be passed to 
// apply_random. This is only used in a couple of cases.
enum class ApplyRandomArg
{
    read_out,
    read_in
};
// Defines a functor for an rapplic. The ApplyRandomArg argument is optional.
typedef std::optional<ApplyRandomArg> Rarg;
#define RAPPLIC(x) struct x { \
    bool operator()() const; \
    bool operator()(Rarg ra) const { return (*this)(); } \
    }
#define RAPPLIC_RARG(x) struct x { \
    bool operator()(Rarg rarg = Rarg()) const; \
}
#define RAPPLIC_DEF(x, type, def) struct x { \
    bool operator()(type v = def) const;\
    bool operator()(Rarg arg, type v = def) const { return (*this)(v); } \
    }

#define ERAPPLIC(x) export RAPPLIC(x)
#define ERAPPLIC_DEF(x,y,z) export RAPPLIC_DEF(x,y,z)

#define HACKFN(x) struct x { \
    bool operator()(const HackP &dem) const; \
}

// Values that can be returned from an exit function.
using ExitFuncVal = std::variant<std::monostate, bool, RoomP>;

using rapplic = std::function<bool(Rarg)>;
using ex_rapplic = std::function<ExitFuncVal()>;
using hackfn = std::function<bool(const HackP&)>;
#define EX_RAPPLIC(x) struct x { ExitFuncVal operator()() const; }
