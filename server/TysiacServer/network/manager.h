#pragma once

#include <unordered_map>
#include <memory>

#include "session.h"

class SessionManager {
private:
	std::unordered_map<std::size_t, std::weak_ptr<Session>> sessions_;

public:
	void registerSession(const std::shared_ptr<Session>& session);
	void unregisterSession(std::size_t id);
	void interpret(const std::string& message);
};