#pragma once

#include <unordered_map>
#include <memory>

#include "session.h"

class Session;

class SessionManager {
//private:
public:
	std::unordered_map<int, std::shared_ptr<Session>> sessions_;

public:
	void register_session(std::shared_ptr<Session>& session);
	void unregister_session(int id);
	void acknowledge();
};