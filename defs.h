#pragma once

#ifndef _ASSERT
#define _ASSERT(x) assert(x)
#endif

// Handy macro to define a property using the deducing this support in C++23.
#define PROP(p) auto && p(this auto && self) noexcept \
 { \
    return std::forward<decltype(self)>(self)._##p; \
 }

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
#define EX_RAPPLIC(x) struct x { ExitFuncVal operator()() const; }

#define HACKFN(x) struct x { \
    bool operator()(const HackP &dem) const; \
}

// Macros to export functions.
#define ERAPPLIC(x) export RAPPLIC(x)
#define ERAPPLIC_RARG(x) export RAPPLIC_RARG(x)
#define ERAPPLIC_DEF(x, type, def) export RAPPLIC_DEF(x, type, def)



#if 0
class Object;
using ObjectP = std::shared_ptr<Object>;
using ObjList = std::list<ObjectP>;
using ObjVector = std::vector<ObjectP>;
class GObject;
typedef std::shared_ptr<GObject> GObjectPtr;
class Room;
using RoomP = std::shared_ptr<Room>;
using RoomList = std::list<RoomP>;
class CEvent;
typedef std::shared_ptr<CEvent> CEventP;
typedef std::list<CEventP> EventList;
class Adv;
typedef std::unique_ptr<Adv> AdvP;

class hack;
typedef std::shared_ptr<hack> HackP;
#endif





// oflags, rflags testers and setter

template <typename T>
int length(const T& c)
{
    return (int) c.size();
}




