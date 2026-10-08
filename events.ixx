module;
//#include "cevent.h"
#include <array>
#include "defs.h"

export module ZEvents:Iface;

export enum class Event
{
    broin,
    cycin,
    sldin,
    xbin,
    xcin,
    xbhin,
    forin,
    curin,
    mntin,
    lntin,
    matin,
    cndin,
    bint,
    brnin,
    fusin,
    ledin,
    safin,
    vlgin,
    gnoin,
    bckin,
    sphin,
    sclin,
    egher,
    zgnin,
    zglin,
    folin,
    mrint,
    pinin,
    inqin,
    strte,
    numevs
};

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
