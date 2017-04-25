#include "manager.h"

void SessionManager::registerSession(const std::shared_ptr<Session>& session)
{
	sessions_.insert(std::make_pair(session->getId(), session));
}

void SessionManager::unregisterSession(std::size_t id)
{
	sessions_.erase(id);
}

void SessionManager::interpret(const std::string& message)
{
	// at the moment server broadcasts received message to all connected clients
	// just for presentation purposes
	for(auto session : sessions_) {
		session.second.lock()->write(message);
	}
}