#pragma once

#include <unordered_map>
#include <memory>

#include "../engine/GameManager.hpp"
#include "session.h"

class SessionManager {
public:
	void registerSession(const std::shared_ptr<Session>& session);
	void unregisterSession(size_t id);
	void interpret(size_t id, const std::string& message);
	size_t connected() const;

private:
	std::unordered_map<std::size_t, std::weak_ptr<Session>> sessions_;
	GameManager game_;
};