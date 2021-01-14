#include "stdafx.h"
#include <algorithm>
#include "object.h"
#include "globals.h"
#include "funcs.h"
#include "cevent.h"
#include "util.h"
#include "objfns.h"
#include "strings.h"
#include "zstring.h"
#include "adv.h"
#include "makstr.h"
#include "dung.h"
#include "ZorkException.h"
struct ObjectDefinition {
    StringList syns;
    StringList adj;
    const char *desc;
    std::initializer_list<Bits> bits;
    rapplic obj_fun;
    StringList contents;
    std::initializer_list<OP> props;
};

struct GObjectDefinition {
    Bits gbits;
    StringList syns;
    StringList adj;
    const char *desc;
    std::initializer_list<Bits> bits;
    rapplic obj_fun;
    std::initializer_list<const char *> contents;
    std::initializer_list<OP> props;
};

ObjList &global_objects()
{
    static ObjList gl;
    return gl;
}

namespace
{
    ObjectPobl &Objects()
    {
        static ObjectPobl obj;
        return obj;
    }

    ObjectP make_obj(const StringList &syns, const StringList &adj, const char *desc,
        const std::initializer_list<Bits> &bits, rapplic obj_fun = nullptr, const std::initializer_list<const char*> &contents = {},
        const std::initializer_list<OP> &props = {})
    {
        return std::make_shared<Object>(syns, adj, desc, bits, obj_fun, contents, props);
    }

    GObjectPtr make_gobj(Bits gbits, const StringList &syns, const StringList &adj, const char *desc,
        const std::initializer_list<Bits> &bits, rapplic obj_fun = nullptr, const std::initializer_list<const char *> &contents = {},
        const std::initializer_list<OP> &props = {})
    {
        return std::make_shared<GObject>(gbits, syns, adj, desc, bits, obj_fun, contents, props);
    }

    // List of all global objects.
	typedef std::vector<GObjectPtr> GObjectArray;
	GObjectArray load_gobjects()
	{
#include "gobject.h"
		GObjectArray o;
        o.reserve(std::distance(std::begin(gobjects), std::end(gobjects)));
		std::transform(std::begin(gobjects), std::end(gobjects), std::back_inserter(o), [](const GObjectDefinition &od)
		{
			return make_gobj(od.gbits, od.syns, od.adj, od.desc, od.bits, od.obj_fun, od.contents, od.props);
		});
		return o;
	}

	GObjectArray &get_gobjects()
    {
		static GObjectArray objs = load_gobjects();
        return objs;
    }
    // List of all objects

// Disable optimization on VS2019. Resets on each new version to
// see if linker error is fixed.
#if _MSC_FULL_VER==192227812
#pragma optimize("", off)
#endif
	typedef std::vector<ObjectP> ObjectArray;
	ObjectArray load_objects()
    {
#include "objdefs.h"
		ObjectArray o;
        o.reserve(std::distance(std::begin(objects), std::end(objects)));
        std::transform(std::begin(objects), std::end(objects), std::back_inserter(o), [](const ObjectDefinition &od)
        {
            return make_obj(od.syns, od.adj, od.desc, od.bits, od.obj_fun, od.contents, od.props);
        });

        return o;
    }
#if _MSC_FULL_VER==192227812
#pragma optimize("", on)
#endif

	ObjectArray &get_objects()
    {
        static ObjectArray objs = load_objects();
		return objs;
    }
}

// last_it must be defined after Objects.
ObjectP last_it;

Object::Object(const std::initializer_list<const char *> &syns, const std::initializer_list<const char *> &adj, const char *description,
    const std::initializer_list<Bits> &bits, rapplic objfun, const StringList &cntnts, 
    const std::initializer_list<OP> &props) :
    synonyms(syns.begin(), syns.end()),
    adjec(adj.begin(), adj.end()),
    desc(description),
    objfn(objfun)
{
	std::transform(cntnts.begin(), cntnts.end(), std::back_inserter(contents),
		[](const char *obj)
	{
		return get_obj(obj);
	});
    // All objects must at least have an id in syns[0].
    _ASSERT(syns.size() > 0);
    for (Bits b : bits)
    {
        // The Bits enum corresponds to a bit in flags.
        flags.set(b);
    }

    // Add all adjectives to the main word list.
    for (const std::string &adj : adjec)
    {
        add_zork(kAdj, adj);
    }

    for (const OP &obj_prop : props)
    {
        switch (obj_prop.slot())
        {
        case ksl_oread:
        {
            _oread = std::get<std::string>(obj_prop.value());
            break;
        }
        case ksl_odesco:
        {
            _odesco = std::get<std::string>(obj_prop.value());
            break;
        }
        case ksl_odesc1:
        {
            _odesc1 = std::get<std::string>(obj_prop.value());
            break;
        }
        case ksl_otval:
        {
            _otval = std::get<int>(obj_prop.value());
            break;
        }
        case ksl_ofval:
        {
            _ofval = std::get<int>(obj_prop.value());
            break;
        }
        case ksl_olint:
        {
            _olint = std::make_shared<olint_t>(std::get<olint_t>(obj_prop.value()));
            clock_disable(clock_int(_olint->ev(), _olint->ev()->ctick()));
            break;
        }
        case ksl_ostrength:
        {
            _ostrength = std::get<int>(obj_prop.value());
            break;
        }
        case ksl_osize:
        {
            _osize = std::get<int>(obj_prop.value());
            break;
        }
        case ksl_omatch:
        {
            _omatch = std::get<int>(obj_prop.value());
            break;
        }
        case ksl_ocapac:
        {
            _ocapac = std::get<int>(obj_prop.value());
            break;
        }
        case ksl_oglobal:
            _oglobal = (Bits) std::get<int>(obj_prop.value());
            break;
        case ksl_oactor:
            _oactor = (e_oactor)std::get<int>(obj_prop.value());
            break;
        case ksl_ovtype:
            _ovtype = (Bits)std::get<int>(obj_prop.value());
            break;
        case ksl_ofmsgs:
            _melee_func = std::get<OP::melee_func>(obj_prop.value());
            break;
        case ksl_obverb:
            // Never used? Just ignore.
            break;
        default:
        {
            _ASSERT(0);
        }
        }
    }
}

const AdvP *Object::oactor() const
{
    const AdvP *actor = nullptr;
    if (_oactor != oa_none)
    {
        actor = &actors()[_oactor];
    }
    return actor;
}

Bits Object::ovtype() const
{
    return _ovtype;
}

int Object::ocapac() const
{
    return _ocapac;
}

int Object::osize() const
{
    return _osize;
}

void Object::osize(int new_size)
{
    _osize = new_size;
}

int Object::omatch() const
{
    return _omatch;
}

void Object::omatch(int new_matches)
{
    _omatch = new_matches;
}

void Object::otval(int new_value)
{
    _otval = new_value;
}

int Object::otval() const
{
    return _otval;
}

void Object::ofval(int new_val)
{
    _ofval = new_val;
}

int Object::ofval() const
{
    return _ofval;
}

void Object::restore(const Object &o)
{
    _ocan = o._ocan;
    flags = o.flags;
    _oroom = o._oroom;
    _ofval = o._ofval;
    _osize = o._osize;
    _ocapac = o._ocapac;
    _omatch = o._omatch;
    _ostrength = o._ostrength;
    contents = o.contents;
    desc = o.desc;
    _odesc1 = o._odesc1;
}


const tofmsgs *Object::ofmsgs() const
{
    const tofmsgs *ofmsg = nullptr;
    if (_melee_func)
    {
        ofmsg = &_melee_func();
    }
    return ofmsg;
}

const std::string &Object::odesc1() const
{
    return _odesc1;
}

const std::string &Object::oread() const
{
    return _oread;
}

std::map<std::string, int64_t> gobject_map;

GObject::GObject(Bits g_bits, const StringList &syns, const StringList &adj,
    const char *desc, const std::initializer_list<Bits> &_bits, rapplic obj_fun,
const std::initializer_list<const char *> &contents, const std::initializer_list<OP> &props) :
Object(syns, adj, desc, _bits, obj_fun, contents, props)
{
    if (g_bits != numbits)
    {
        _gbits = g_bits;
        _oglobal = g_bits;
    }
    flags.set(::oglobal);
}

void init_objects()
{
    // Just add all objects to the map, based on the first item in the object names.
    auto &objs = get_objects();
	auto cur = objs.begin();
    while (cur != objs.end())
    {
        auto p = get_obj((*cur)->oid(), *cur);
        // Empty objects may have been added already, so we need to initialize them
        // if so.
        if (p.get() != cur->get())
        {
            // The container may have been set already when the containing object
            // was created, so restore it if necessary.
            ObjectP old_ocan = p->ocan();
            *(p.get()) = *cur->get();
            p->ocan(old_ocan);
        }
        inc_score_max(p->ofval() + p->otval());

        // Ensure everything in this object has its ocan pointer set.
        for (const ObjectP &o : p->ocontents())
        {
            o->ocan(p);
        }

		++cur;
    }

}

void init_synonyms()
{
    auto &objs = get_objects();
    // At this point, all global and regular objects have been added. This assumes that
    // all names are unique. 
#if _DEBUG
    const ObjectPobl &ob = Objects();
    for (auto iter : ob)
    {
        _ASSERT(iter.second.size() == 1);
    }
#endif

    // Store all synonyms in the map. Do this after all "real" objects have been created.
	auto cur = objs.begin();
    while (cur != objs.end())
    {
        auto p = get_obj((*cur)->oid());
        _ASSERT(p);
        if (p->onames().size() > 1)
        {
            for (auto iter = p->onames().begin() + 1; iter != p->onames().end(); ++iter)
            {
                Objects()[*iter].push_back(p);
            }
        }
		++cur;
    }

    // Do the same for the global objects.
    auto &gobjs = get_gobjects();
	auto gobj_iter = gobjs.begin();
	while (gobj_iter != gobjs.end())
    {
        auto cur = *gobj_iter;
        auto p = get_obj(cur->oid());
        _ASSERT(p);
        if (p->onames().size() > 1)
        {
            for (auto iter = p->onames().begin() + 1; iter != p->onames().end(); ++iter)
            {
                Objects()[*iter].push_back(p);
            }
        }
		++gobj_iter;
    }
}

void init_gobjects()
{
    auto &objs = get_gobjects();
	auto objs_iter = objs.begin();
    ObjectPobl &obj_pobl = Objects();
    while (objs_iter != objs.end())
    {
        auto cur = *objs_iter;
        // Make sure this object is not already in the global list.
        auto &gl = global_objects();
        if (std::find_if(gl.begin(), gl.end(), [cur](ObjectP p)
        {
            return p->oid() == cur->oid();
        }) != gl.end())
        {
            error("Duplicate gobject added.");
        }
        gl.push_back(cur);

        // Make sure that this is the correct object in the object list.
        auto p = get_obj(cur->oid(), cur);
        if (p.get() != cur.get())
        {
            _ASSERT(obj_pobl[p->oid()].size() == 1);
            *(p.get()) = *cur;
        }
        
        if (obj_pobl[p->oid()].empty())
        {
            // Insert into the object list.
            obj_pobl[p->oid()].push_front(p);
        }
		++objs_iter;
    }

    ObjList::iterator it = std::find_if(global_objects().begin(), global_objects().end(), [](ObjectP p) { return p->oid() == "IT"; });
    _ASSERT(it != global_objects().end());
    it_object = std::dynamic_pointer_cast<GObject>(*it);
}


ObjectP get_obj(std::string_view sname, ObjectP init_val)
{
    ObjectP op;
    std::string name(sname);
    auto o = Objects().find(name);
    if (o == Objects().end())
    {
        // Not in the list. Add either that passed value or an empty one.
        if (!init_val)
        {
            init_val = make_obj({ name.c_str() }, {}, "", {});
        }
        Objects()[name].push_front(init_val);
        op = init_val;
    }
    else
    {
        op = o->second.front();
    }
    return op;
}


olint_t::olint_t(int v, bool enable, const CEventP &ev, int init_val) : _val(v), _ev(ev)
{
    _ev->ctick(init_val);
}

const ObjectPobl &object_pobl()
{
    return Objects();
}

bool is_obj(const std::string &name)
{
    return Objects().find(name) != Objects().end();
}

const ObjectP &find_obj(std::string_view name)
{
    _ASSERT(name.size() <= 5); // To catch typos
    auto iter = Objects().find(name);
    _ASSERT(iter != Objects().end());
    return iter->second.front();
}

const ObjectP &sfind_obj(std::string_view name)
{
    return find_obj(name);
}


