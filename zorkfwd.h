#pragma once
#include <memory>
#include <list>
class Room;
using RoomP = std::shared_ptr<Room>;
using RoomList = std::list<RoomP>;

class hack;
typedef std::shared_ptr<hack> HackP;
