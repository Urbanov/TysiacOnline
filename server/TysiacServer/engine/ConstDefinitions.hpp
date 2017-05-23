#pragma once
#include <json.hpp>
#include <vector>
#include "Player.hpp"
#include "Controller.hpp"
#include "Room.hpp"

class Room;

using json = nlohmann::json;
using players_it = std::vector<Player>::iterator;
using PRoom = std::unique_ptr<Room>;
using req = std::vector<std::pair<std::string, std::vector<int> > >;
using request_type = std::vector<json>;