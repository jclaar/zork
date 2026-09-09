export module Zork:AdvI;
import std;
export import :Adv;
import :Room;

template <class archive>
void Adv::save(archive& ar, const unsigned int version) const
{
    ar& (_aroom ? _aroom->rid() : std::string());
    ar& _ascore;
    ar& (_avehicle ? _avehicle->oid() : std::string());
    ar& _aobj->oid();
    ar& _astrength;
    ar& bits;
    std::list<std::string> obj_list;
    for (auto o : _aobjs)
    {
        obj_list.push_back(o->oid());
    }
    ar& obj_list;
}

template <class archive>
void Adv::load(archive& ar, const unsigned int version)
{
    std::string temp;
    ar& temp;
    if (!temp.empty())
        _aroom = sfind_room(temp);
    ar& _ascore;
    ar& temp;
    if (!temp.empty())
        _avehicle = sfind_obj(temp);
    ar& temp;
    _aobj = sfind_obj(temp);
    ar& _astrength;
    ar& bits;
    std::list<std::string> obj_list;
    ar& obj_list;
    for (auto s : obj_list)
    {
        _aobjs.push_back(sfind_obj(s));
    }
}
