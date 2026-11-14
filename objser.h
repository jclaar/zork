#pragma once
// Serialization functions for Object are defined here so
// that room.h can be included properly.
// For serialization
template <class archive>
void Object::save(archive &ar, const unsigned int version) const
{
    ar & oid();
    ar & (_ocan ? _ocan->oid() : std::string());
    ar & _oflags;
    ar & (_oroom ? _oroom->rid() : std::string());
    ar & _ofval;
    ar & _osize;
    ar & _ocapac;
    ar & _omatch;
    ar & _ostrength;
    std::list<std::string> conts;
	std::ranges::transform(_ocontents, std::back_inserter(conts), &Object::oid, [](const ObjectP& o) { return *o; });
    ar & conts;
    ar & _odesc1;
    ar & _odesc2;
}

template <class archive>
void Object::load(archive &ar, const unsigned int version)
{
    std::string temp;
    ar & temp;
    _ASSERT(synonyms.empty());
    synonyms.push_back(temp);
    ar & temp;
    if (!temp.empty())
    {
        _ocan = sfind_obj(temp);
    }
    ar & _oflags;
    ar & temp;
    if (!temp.empty())
        _oroom = sfind_room(temp);
    ar & _ofval;
    ar & _osize;
    ar & _ocapac;
    ar & _omatch;
    ar & _ostrength;
    std::list<std::string> conts;
    ar & conts;
    ar & _odesc1;
    ar & _odesc2;
    _ocontents.clear();
    std::ranges::transform(conts, std::back_inserter(_ocontents), sfind_obj);
}

