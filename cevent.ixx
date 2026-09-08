module;
#include <boost/serialization/access.hpp>

export module ZCevent;
import ZDefs;
import ZObjfns;
import ZRoomfns;
import std;

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

    const std::string& cid() const { return _cid; }

    void restore(const CEvent& src)
    {
        _ctick = src.ctick();
        _cflag = src.cflag();
        _cdeath = src.cdeath();
    }

private:
    CEvent() {}
    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive& ar, const unsigned int version)
    {
        ar& _ctick;
        //ar & _caction;
        ar& _cflag;
        ar& _cid;
        ar& _cdeath;
    }

    int _ctick = 0;
    rapplic _caction = nullptr;
    bool _cflag = false;
    std::string _cid;
    bool _cdeath = false;
};

export using CEventP = std::shared_ptr<CEvent>;

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

class CEventContainer : private std::array<CEventP, std::to_underlying(Event::numevs)>
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

export CEventP sphere_clock;
export using EventList = std::list<CEventP>;


namespace
{
    CEventP mke(int tick, rapplic action, bool flag, const char* id, bool death)
    {
        return std::make_shared<CEvent>(tick, action, flag, id, death);
    }
}

CEventContainer ev;

CEventContainer::CEventContainer() :
    Base({
        mke(0, obj_funcs::brochure(), true, "BROIN", false),
        mke(0, obj_funcs::cyclops(), true, "CYCIN", true),
        mke(0, obj_funcs::slide_cint(), false, "SLDIN", true),
        mke(0, xb_cint(), false, "XBIN", true),
        mke(0, xc_cint(), false, "XCIN", true),
        mke(0, xbh_cint(), false, "XBHIN", true),
        mke(0, room_funcs::forest_room(), false, "FORIN", false),
        mke(0, cure_clock(), false, "CURIN", false),
        mke(0, room_funcs::maint_room(), true, "MNTIN", false),
        mke(0, obj_funcs::lantern(), true, "LNTIN", false),
        mke(0, obj_funcs::match_function(), true, "MATIN", false),
        mke(0, obj_funcs::candles(), true, "CNDIN", false),
        mke(0, obj_funcs::balloon(), true, "BINT", false),
        mke(0, burnup(), true, "BRNIN", true),
        mke(0, obj_funcs::fuse_function(), true, "FUSIN", true),
        mke(0, ledge_mung(), true, "LEDIN", true),
        mke(0, safe_mung(), true, "SAFIN", true),
        mke(0, volgnome(), true, "VLGIN", false),
        mke(0, obj_funcs::gnome_function(), true, "GNOIN", false),
        mke(0, obj_funcs::bucket(), true, "BCKIN", false),
        mke(0, obj_funcs::sphere_function(), true, "SPHIN", false),
        mke(0, scol_clock(), true, "SCLIN", false),
        mke(0, end_game_herald(), false, "EGHER", false),
        mke(0, zgnome_init(), true, "ZGNIN", false),
        mke(0, obj_funcs::zgnome_function(), true, "ZGLIN", false),
        mke(0, follow(), false, "FOLIN", false),
        mke(0, obj_funcs::mrswitch(), false, "MRINT", false),
        mke(0, obj_funcs::mends(), false, "PININ", false),
        mke(0, inquisitor(), false, "INQIN", false),
        mke(0, start_end(), true, "STRTE", false),
        })
{}

CEvent::CEvent(int tick, rapplic action, bool flag, std::string_view id, bool death) :
    _ctick(tick),
    _caction(action),
    _cflag(flag),
    _cid(id),
    _cdeath(death)
{

}

