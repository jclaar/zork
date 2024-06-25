#pragma once
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/bitset.hpp>
#include <vector>
#include <map>
#include <string>
#include <list>
#include <memory>
#include <bitset>
#include "funcs.h"
#include "defs.h"
#include "strings.h"
#include "cevent.h"

typedef std::initializer_list<const char*> StringList;

enum class ObjectSlots
{
	ksl_odesco,
	ksl_odesc1,
	ksl_osize,
	ksl_ofval,
	ksl_otval,
	ksl_ocapac,
	ksl_oread,
    ksl_oglobal,
    ksl_oactor,
    ksl_ovtype,
    ksl_ostrength,
    ksl_ofmsgs,
    ksl_olint,
    ksl_omatch,
    ksl_obverb,
    ksl_rglobal,
    ksl_rval
};

// Structure for cevents
class olint_t
{
public:
    olint_t(int v, const CEventP& ev, int init_val) :_val(v), _ev(ev)
    {
        _ev->ctick(init_val);
    }

    const CEventP &ev() const { return _ev; }
    int val() const { return _val; }
    void val(int new_val) { _val = new_val; }

private:
    olint_t() {}

    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &ar, const unsigned int version)
    {
        ar & _val;
        ar & _ev;
    }

    int _val = 0;
    CEventP _ev;
};
typedef std::shared_ptr<olint_t> OlintP;

class OP
{
public:
    using melee_func = const tofmsgs*;
    typedef std::variant<int, std::string, melee_func, olint_t, RoomBit> PropVal;
    OP(ObjectSlots os, const PropVal &v) : sl(os), val(v) {}
    explicit OP(ObjectSlots os, int i) : sl(os), val(i) {}
    explicit OP(ObjectSlots os, RoomBit b) : sl(os), val(b) {}
    explicit OP(ObjectSlots os, e_oactor actor) : sl(os), val(static_cast<int>(actor)) {}
    explicit OP(ObjectSlots os, std::string_view v) : sl(os), val(std::string(v)) {}
    explicit OP(ObjectSlots os, const char* p) : sl(os)
    {
        if (p)
            val = p;
    }
    ObjectSlots slot() const { return sl; }
    const PropVal &value() const { return val; }
private:
    ObjectSlots sl;
    PropVal val;
};

typedef Flags<Bits, numbits> OFlags;

class Object
{
public:

    Object() : _melee_func(nullptr) {}

    Object(const StringList &syns, const StringList &adj = {}, const char *desc = "",
        const std::initializer_list<Bits> &bits = {}, rapplic obj_fun = nullptr, const StringList &contents = {},
        const std::initializer_list<OP> &props = {});

	virtual ~Object()
	{

	}

    const std::string &oid() const { return synonyms[0]; }
	const std::vector<std::string> &onames() const { return synonyms; }
    const std::vector<std::string> &oadjs() const { return adjec; }
    const ObjList &ocontents() const { return contents; }
    ObjList &ocontents() { return contents; }
    const tofmsgs *ofmsgs() const;
	const std::string &oread() const;
    const std::string &odesco() const { return _odesco; }
    const std::string &odesc1() const;
    void odesc1(std::string_view s) { _odesc1 = s; }
    std::string_view odesc2() const { return desc; }
    void odesc2(const char *new_desc) { desc = new_desc; }
    int otval() const;
    void otval(int new_value);
    int ofval() const;
    void ofval(int new_val);
    int ocapac() const;
    int osize() const;
    void osize(int new_size);
    int ostrength() const { return _ostrength; }
    void ostrength(int new_strength) { _ostrength = new_strength; }
    int omatch() const;
    void omatch(int new_val);
    const OlintP &olint() const {
        return _olint;
    }
    const AdvP *oactor() const;
    std::optional<Bits> oglobal() const { return _oglobal; }
    const OFlags &oflags() const { return flags; }
    OFlags &oflags() { return flags; }
    const rapplic &oaction() const { return objfn; }
    const RoomP &oroom() const { return _oroom; }
    Object& oroom(const RoomP& r) { _oroom = r; return *this; }
    const ObjectP &ocan() const { return _ocan; }
    Object& ocan(const ObjectP& op) { _ocan = op; return *this; }
    RoomBit ovtype() const;

    const VerbP &obverb() const { return _obverb; }
    void obverb(const VerbP &v) { _obverb = v; }

    void restore(const Object &o);

private:
    friend class boost::serialization::access;
    // For serialization
    template <class archive>
    void save(archive &ar, const unsigned int version) const;
    template <class archive>
    void load(archive &ar, const unsigned int version);

    BOOST_SERIALIZATION_SPLIT_MEMBER();

protected:
    std::vector<std::string> synonyms;
    std::vector<std::string> adjec;
    ObjList contents;
    std::string desc;
    std::string _odesco;
    std::string _odesc1;
    std::string _oread;
    int _osize = 0;
    int _otval = 0;
    int _ofval = 0;
    int _ostrength = 0;
    ObjectP _ocan; // What contains this object.
    RoomP _oroom;  // What room it's in.
    OFlags flags;
    rapplic objfn;
    OlintP _olint;
    VerbP _obverb;
    std::optional<Bits> _oglobal;
    int _omatch = 0;
    int _ocapac = 0;
    e_oactor _oactor = e_oactor::none;
    RoomBit _ovtype = RoomBit::rnumbits;
    OP::melee_func _melee_func;
};

class GObject : public Object
{
public:
    GObject(Bits gbits, const StringList &syns, const StringList &adj = {},
        const char * = "", const std::initializer_list<Bits> &bits = {}, rapplic obj_fun = nullptr,
        const std::initializer_list<const char*> &contents = {},
        const std::initializer_list<OP> &props = { OP(ObjectSlots::ksl_oglobal, OP::PropVal(0)) });

    const std::optional<Bits> &gbits() const { return _gbits; }

private:
    GObject() {}
    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<Object>(*this);
    }

    std::optional<Bits> _gbits;
};


void init_objects();
void init_gobjects();
void init_synonyms();

inline bool empty(const ObjectP &op)
{
    return !op;
}

ObjectP get_obj(std::string_view name, ObjectP init_val = nullptr);
ObjList &global_objects();

typedef std::map<std::string, ObjList, std::less<>> ObjectPobl;
const ObjectPobl &object_pobl();
bool is_obj(const std::string &obj);
const ObjectP &find_obj(std::string_view name);
const ObjectP &sfind_obj(std::string_view name);

inline bool trnn(const ObjectP &op, Bits b)
{
    return op->oflags().test(b);
}

template <typename... Args>
bool trnn(const ObjectP& op, Bits first, Args... args)
{
    if (trnn(op, first))
        return true;
    return trnn(op, args...);
}

template <typename T>
int trz(const ObjectP &op, T b)
{
    return op->oflags()[b] = 0;
}

template <typename T, typename... Args>
int trz(const ObjectP &op, T first, Args... args)
{
    trz(op, first);
    trz(op, args...);
    return 0;
}

template <typename T>
const ObjectP &tro(const ObjectP &op, T b)
{
    op->oflags()[b] = 1;
    return op;
}

template <typename T, typename... Args>
const ObjectP &tro(const ObjectP &op, T first, Args... args)
{
    tro(op, first);
    tro(op, args...);
    return op;
}

inline bool openable(const ObjectP& op)
{
    return trnn(op, Bits::doorbit, Bits::contbit);
}

