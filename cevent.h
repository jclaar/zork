#pragma once

#include "defs.h"
#include <memory>
#include <boost/serialization/access.hpp>

// CEVENT structure
class CEvent
{
public:
    CEvent(int tick, rapplic action, bool flag, std::string_view id, bool death);

    rapplic caction() const { return _caction; }
    int ctick() const { return _ctick; }
    void ctick(int tick) { _ctick = tick; }
    bool cflag() const { return _cflag; }
    void cflag(bool flag) { _cflag = flag; }
    bool cdeath() const { return _cdeath; }

    const std::string &cid() const { return _cid; }

    void restore(const CEvent &src)
    {
        _ctick = src.ctick();
        _cflag = src.cflag();
        _cdeath = src.cdeath();
    }

private:
    CEvent() {}
    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &ar, const unsigned int version)
    {
        ar & _ctick;
        //ar & _caction;
        ar & _cflag;
        ar & _cid;
        ar & _cdeath;
    }

    int _ctick = 0;
    rapplic _caction = nullptr;
    bool _cflag = false;
    std::string _cid;
    bool _cdeath = false;
};

enum class Event
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

class CEventContainer : private std::array<CEventP, static_cast<size_t>(Event::numevs)>
{
    using Base = std::array<CEventP, static_cast<size_t>(Event::numevs)>;
public:
    using Base::begin;
    using Base::end;

    CEventContainer();

    const CEventP& operator[](Event event) const { return Base::operator[](static_cast<size_t>(event)); }
    CEventP& operator[](Event event) { 
        return Base::operator[](static_cast<size_t>(event)); 
    }
};

extern CEventContainer ev;

extern CEventP sphere_clock;
// Handy macros to just refer to events by their names.
#define broin ev[Event::broin]
#define cycin ev[Event::cycin]
#define sldin ev[Event::sldin]
#define xbin ev[Event::xbin]
#define xcin ev[Event::xcin]
#define xbhin ev[Event::xbhin]
#define forin ev[Event::forin]
#define curin ev[Event::curin]
#define mntin ev[Event::mntin]
#define lntin ev[Event::lntin]
#define matin ev[Event::matin]
#define cndin ev[Event::cndin]
#define bint ev[Event::bint]
#define brnin ev[Event::brnin]
#define fusin ev[Event::fusin]
#define ledin ev[Event::ledin]
#define safin ev[Event::safin]
#define vlgin ev[Event::vlgin]
#define gnoin ev[Event::gnoin]
#define bckin ev[Event::bckin]
#define sphin ev[Event::sphin]
#define sclin ev[Event::sclin]
#define egher ev[Event::egher]
#define zgnin ev[Event::zgnin]
#define zglin ev[Event::zglin]
#define folin ev[Event::folin]
#define mrint ev[Event::mrint]
#define pinin ev[Event::pinin]
#define inqin ev[Event::inqin]
#define strte ev[Event::strte]
