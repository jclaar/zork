#ifndef OBJSER_H
#define OBJSER_H

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
    std::transform(_ocontents.begin(), _ocontents.end(), std::back_inserter(conts), [](ObjectP o) { return o->oid(); });
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
    std::transform(conts.begin(), conts.end(), std::back_inserter(_ocontents), [](const std::string &s)
    {
        return sfind_obj(s);
    });
}


#endif

