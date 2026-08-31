#pragma once
#include <memory>
#include <list>
#include <vector>
class Room;
using RoomP = std::shared_ptr<Room>;
using RoomList = std::list<RoomP>;

class hack;
typedef std::shared_ptr<hack> HackP;

class Object;
using ObjectP = std::shared_ptr<Object>;
using ObjList = std::list<ObjectP>;
using ObjVector = std::vector<ObjectP>;
class GObject;
typedef std::shared_ptr<GObject> GObjectPtr;
class CEvent;
typedef std::shared_ptr<CEvent> CEventP;
typedef std::list<CEventP> EventList;
class Adv;
typedef std::unique_ptr<Adv> AdvP;
