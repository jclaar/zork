#ifndef OBJSER_H
#define OBJSER_H

#include "room.h"

// Serialization functions for Object are defined here so
// that room.h can be included properly.
// For serialization
template <class archive>
void Object::save(archive &ar, const unsigned int version) const
{
    ar & oid();
    ar & (_ocan ? _ocan->oid() : std::string());
    ar & flags;
    ar & (_oroom ? _oroom->rid() : std::string());
    ar & _ofval;
    ar & _osize;
    ar & _ocapac;
    ar & _omatch;
    ar & _ostrength;
    std::list<std::string> conts;
    std::transform(contents.begin(), contents.end(), std::back_inserter(conts), [](ObjectP o) { return o->oid(); });
    ar & conts;
    ar & _odesc1;
    ar & desc;
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
    ar & flags;
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
    ar & desc;
    contents.clear();
    std::transform(conts.begin(), conts.end(), std::back_inserter(contents), [](const std::string &s)
    {
        return sfind_obj(s);
    });
}


#endif

