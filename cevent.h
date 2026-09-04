#pragma once

#include "defs.h"
#include <memory>
#include <array>
#include <utility>
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

// Handy macros to just refer to events by their names.
#define broin ev[Event::ebroin]
#define cycin ev[Event::ecycin]
#define sldin ev[Event::esldin]
#define xbin ev[Event::exbin]
#define xcin ev[Event::excin]
#define xbhin ev[Event::exbhin]
#define forin ev[Event::eforin]
#define curin ev[Event::ecurin]
#define mntin ev[Event::emntin]
#define lntin ev[Event::elntin]
#define matin ev[Event::ematin]
#define cndin ev[Event::ecndin]
#define bint ev[Event::ebint]
#define brnin ev[Event::ebrnin]
#define fusin ev[Event::efusin]
#define ledin ev[Event::eledin]
#define safin ev[Event::esafin]
#define vlgin ev[Event::evlgin]
#define gnoin ev[Event::egnoin]
#define bckin ev[Event::ebckin]
#define sphin ev[Event::esphin]
#define sclin ev[Event::esclin]
#define egher ev[Event::eegher]
#define zgnin ev[Event::ezgnin]
#define zglin ev[Event::ezglin]
#define folin ev[Event::efolin]
#define mrint ev[Event::emrint]
#define pinin ev[Event::epinin]
#define inqin ev[Event::einqin]
#define strte ev[Event::estrte]
