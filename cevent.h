#pragma once

#include <any>
#include "defs.h"
#include "object.h"
#include <memory>

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

enum Event
{
    kev_broin,
    kev_cycin,
    kev_sldin,
    kev_xbin,
    kev_xcin,
    kev_xbhin,
    kev_forin,
    kev_curin,
    kev_mntin,
    kev_lntin,
    kev_matin,
    kev_cndin,
    kev_bint,
    kev_brnin,
    kev_fusin,
    kev_ledin,
    kev_safin,
    kev_vlgin,
    kev_gnoin,
    kev_bckin,
    kev_sphin,
    kev_sclin,
    kev_egher,
    kev_zgnin,
    kev_zglin,
    kev_folin,
    kev_mrint,
    kev_pinin,
    kev_inqin,
    kev_strte,
    kev_numevs
};

extern std::array<CEventP, kev_numevs> ev;

extern CEventP sphere_clock;
// Handy macros to just refer to events by their names.
#define broin ev[kev_broin]
#define cycin ev[kev_cycin]
#define sldin ev[kev_sldin]
#define xbin ev[kev_xbin]
#define xcin ev[kev_xcin]
#define xbhin ev[kev_xbhin]
#define forin ev[kev_forin]
#define curin ev[kev_curin]
#define mntin ev[kev_mntin]
#define lntin ev[kev_lntin]
#define matin ev[kev_matin]
#define cndin ev[kev_cndin]
#define bint ev[kev_bint]
#define brnin ev[kev_brnin]
#define fusin ev[kev_fusin]
#define ledin ev[kev_ledin]
#define safin ev[kev_safin]
#define vlgin ev[kev_vlgin]
#define gnoin ev[kev_gnoin]
#define bckin ev[kev_bckin]
#define sphin ev[kev_sphin]
#define sclin ev[kev_sclin]
#define egher ev[kev_egher]
#define zgnin ev[kev_zgnin]
#define zglin ev[kev_zglin]
#define folin ev[kev_folin]
#define mrint ev[kev_mrint]
#define pinin ev[kev_pinin]
#define inqin ev[kev_inqin]
#define strte ev[kev_strte]
