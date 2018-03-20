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
#include "objdefs.h"
#include "gobject.h"

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
        const std::initializer_list<Bits> &bits, rapplic obj_fun = nullptr, const std::initializer_list<ObjectP> &contents = {},
        const std::initializer_list<OP> &props = {})
    {
        return std::make_shared<Object>(syns, adj, desc, bits, obj_fun, contents, props);
    }

    GObjectPtr make_gobj(const std::string &name, const StringList &syns, const StringList &adj, const char *desc,
        const std::initializer_list<Bits> &bits, rapplic obj_fun = nullptr, const std::initializer_list<ObjectP> &contents = {},
        const std::initializer_list<OP> &props = {})
    {
        return std::make_shared<GObject>(name, syns, adj, desc, bits, obj_fun, contents, props);
    }

    // List of all global objects.
	typedef std::array<GObjectPtr, sizeof(gobjects) / sizeof(gobjects[0])> GObjectArray;
	GObjectArray load_gobjects()
	{
		GObjectArray o;
		std::transform(std::begin(gobjects), std::end(gobjects), o.begin(), [](const GObjectDefinition &od)
		{
			return make_gobj(od.name, od.syns, od.adj, od.desc, od.bits, od.obj_fun, od.contents, od.props);
		});
		return o;
	}

	GObjectArray &get_gobjects()
    {
		static GObjectArray objs = load_gobjects();
        return objs;
    }
    // List of all objects

	typedef std::array<ObjectP, sizeof(objects) / sizeof(objects[0])> ObjectArray;
	ObjectArray load_objects()
    {
		ObjectArray o;
        std::transform(std::begin(objects), std::end(objects), o.begin(), [](const ObjectDefinition &od)
        {
            return make_obj(od.syns, od.adj, od.desc, od.bits, od.obj_fun, od.contents, od.props);
        });

        return o;
    }

	ObjectArray &get_objects()
    {
        static ObjectArray objs = load_objects();
		return objs;
    }
}

// last_it must be defined after Objects.
ObjectP last_it(get_obj("#####"));

Object::Object(const std::initializer_list<const char *> &syns, const std::initializer_list<const char *> &adj, const char *description,
    const std::initializer_list<Bits> &bits, rapplic objfun, const std::initializer_list<ObjectP> &cntnts, 
    const std::initializer_list<OP> &props) :
    synonyms(syns.begin(), syns.end()),
    adjec(adj.begin(), adj.end()),
    desc(description),
    objfn(objfun),
    contents(cntnts)
{
    // All objects must at least have an id in syns[0].
    _ASSERT(syns.size() > 0);
    for (Bits b : bits)
    {
        // The Bits enum corresponds to a bit in flags.
        flags[b] = true;
    }

    // Add all adjectives to the main word list.
    for (const std::string &adj : adjec)
    {
        add_zork(kAdj, { adj });
    }

    for (auto obj_prop : props)
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
        default:
        {
            prop_map[obj_prop.slot()] = obj_prop.value();
        }
        }
    }
}

AdvP Object::oactor() const
{
    AdvP actor;
    e_oactor e = oa_none;
    auto i = prop_map.find(ksl_oactor);
    if (i != prop_map.end())
    {
        int v = std::get<int>(i->second);
        _ASSERT(v >= 0 && v <= oa_none);
        e = (e_oactor)v;
        if (e != oa_none)
        {
            actor = actors()[e];
        }
    }
    return actor;
}

Bits Object::ovtype() const
{
    Bits b = numbits;
    auto i = prop_map.find(ksl_ovtype);
    if (i != prop_map.end())
    {
        int v = std::get<int>(i->second);
        _ASSERT(v >= 0 && v < numbits);
        b = (Bits)v;
    }
    return b;
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
}


const tofmsgs *Object::ofmsgs() const
{
    typedef const tofmsgs &(*ofmsgf)();
    const tofmsgs *ofmsg = nullptr;
    auto i = prop_map.find(ksl_ofmsgs);
    if (i != prop_map.end())
    {
        typedef const tofmsgs &(*ofmsgf)();
        ofmsgf fn = std::get<OP::melee_func>(i->second);
        ofmsg = &fn();
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

GObject::GObject(const std::string &name, const StringList &syns, const StringList &adj,
    const char *desc, const std::initializer_list<Bits> &_bits, rapplic obj_fun,
const std::initializer_list<ObjectP> &contents, const std::initializer_list<OP> &props) :
Object(syns, adj, desc, _bits, obj_fun, contents, props)
{
    int64_t bits;
    if (!name.empty())
    {
        _name = name;
        std::string sname = name;
        // Has this name been assigned?
        auto iter = gobject_map.find(sname);
        if (iter != gobject_map.end())
        {
            bits = iter->second;
        }
        else
        {
            glohi = bits = glohi * 2;
            gobject_map[sname] = bits;
        }
        _oglobal = name;
    }
    else
    {
        glohi = bits = glohi * 2;
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
        for (ObjectP o : p->ocontents())
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


size_t obj_count()
{
    return Objects().size();
}

ObjectP get_obj(const char *name, ObjectP init_val)
{
    return get_obj(std::string(name), init_val);
}

ObjectP get_obj(const std::string &name, ObjectP init_val)
{
    ObjectP op;
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


olint_t::olint_t(int v, bool enable, CEventP ev, int init_val) : _val(v)
{
    _ev = ev;
    _ev->ctick(init_val);
}

const ObjectPobl &object_pobl()
{
    return Objects();
}

ObjectP find_obj(const std::string &name, bool check_correctness)
{
    _ASSERT(name.size() <= 5); // To catch typos
    if (check_correctness)
    {
        _ASSERT(Objects().find(name) != Objects().end());
    }
    auto iter = Objects().find(name);
    if (check_correctness)
    {
        _ASSERT(iter != Objects().end());
        _ASSERT(iter->second.size() == 1 || iter->second.front()->onames()[0] == name);
    }
    return iter == Objects().end() ? ObjectP() : iter->second.front();
}

ObjectP sfind_obj(const std::string &name)
{
    return find_obj(name);
}

ObjectP sfind_obj(const char *name)
{
    return sfind_obj(std::string(name));
}

