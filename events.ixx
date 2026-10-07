module;
#include "cevent.h"

export module ZEvents:Iface;

export class CEventContainer : private std::array<CEventP, std::to_underlying(Event::numevs)>
{
    using Base = std::array<CEventP, std::to_underlying(Event::numevs)>;
public:
    using Base::begin;
    using Base::end;

    CEventContainer();

    const CEventP& operator[](Event event) const { return Base::operator[](std::to_underlying(event)); }
    CEventP& operator[](Event event) {
        return Base::operator[](std::to_underlying(event));
    }
};

export extern CEventContainer ev;

export extern CEventP sphere_clock;
export extern CEventP burnup_int;
