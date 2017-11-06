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
#include <any>
#include "funcs.h"
#include "defs.h"
#include "strings.h"

enum ObjectSlots
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
    olint_t(int v, bool enable, CEventP ev, int init_val);

    CEventP ev() const { return _ev; }
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

    int _val;
    CEventP _ev;
};
typedef std::shared_ptr<olint_t> OlintP;

class OP
{
public:
    typedef const std::vector<std::vector<std::string>> &(*melee_func)();
    typedef std::variant<int, std::string, melee_func, olint_t> PropVal;
    enum { kPV_int, kPV_string, kPV_meleefunc, kPV_olint };
    OP(ObjectSlots os, const PropVal &v) : sl(os), val(v) {}
    ObjectSlots slot() const { return sl; }
    const PropVal &value() { return val; }
private:
    ObjectSlots sl;
    PropVal val;
};
//typedef std::tuple<ObjectSlots, std::any> OP;

class Object
{
public:

    Object() {}
	//Object(const vector<string> &syns, const vector<string> &adj, const string &description,
	//	const list<Bits> &bits);

    Object(const std::initializer_list<std::string> &syns, const std::initializer_list<std::string> &adj = {}, const std::string &desc = "",
        const std::initializer_list<Bits> &bits = {}, rapplic obj_fun = nullptr, const std::initializer_list<ObjectP> &contents = {},
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
    void odesc1(const std::string &s) { _odesc1 = s; }
    const std::string &odesc2() const { return desc; }
    void odesc2(const std::string &new_desc) { desc = new_desc; }
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
    OlintP olint() const {
        return _olint;
    }
    AdvP oactor() const;
    const std::string &oglobal() const { return _oglobal; }
    const std::bitset<numbits> &oflags() const { return flags; }
    std::bitset<numbits> &oflags() { return flags; }
    rapplic oaction() const { return objfn; }
    RoomP oroom() const { return _oroom; }
    void oroom(RoomP r) { _oroom = r; }
    ObjectP ocan() const { return _ocan; }
    void ocan(ObjectP op) { _ocan = op; }
    Bits ovtype() const;

    VerbP obverb() const { return _obverb; }
    void obverb(VerbP v) { _obverb = v; }

	bool call_fn()
	{
		bool rv = objfn != nullptr;
		if (rv)
			rv = (*objfn)();
		return rv;
	}

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
    std::list<ObjectP> contents;
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
    std::bitset<numbits> flags;
    rapplic objfn = nullptr;
    std::map<ObjectSlots, OP::PropVal> prop_map;
    OlintP _olint;
    VerbP _obverb;
    std::string _oglobal;
    int _omatch = 0;
    int _ocapac = 0;
};

class GObject : public Object
{
public:
    GObject(const std::string &name, const std::initializer_list<std::string> &syns, const std::initializer_list<std::string> &adj = {},
        const std::string & = "", const std::initializer_list<Bits> &bits = {}, rapplic obj_fun = nullptr,
        const std::initializer_list<ObjectP> &contents = {},
        const std::initializer_list<OP> &props = { OP(ksl_oglobal, 0) });

    const std::string &name() const { return _name; }

private:
    GObject() {}
    friend class boost::serialization::access;
    template <class archive>
    void serialize(archive &ar, const unsigned int version)
    {
        ar & boost::serialization::base_object<Object>(*this);
    }

    std::string _name;
};


void init_objects();
void init_gobjects();
void init_synonyms();
bool memq(ObjectP op, const ObjList &ol);
bool memq(ObjectP op, Iterator<ObjVector> ol);
bool memq(ObjectP op, Iterator<ObjList> ol);

inline int length(const ObjList &ol) { return (int)ol.size(); }

inline bool empty(ObjectP op)
{
    return !op;
}

size_t obj_count();
ObjectP get_obj(const std::string &name, ObjectP init_val = nullptr);
ObjList &global_objects();

typedef std::map<std::string, ObjList> ObjectPobl;
const ObjectPobl &object_pobl();
ObjectP find_obj(const std::string &name, bool correctness = true);
ObjectP sfind_obj(const std::string &name);
