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
	/*// at the moment server broadcasts received message to all connected clients
	// just for presentation purposes
	for(auto session : sessions_) {
		session.second.lock()->write(message);
	}*/
	std::cout << message;
	auto return_data = game_.doWork(message);
	for (auto& msg : return_data) {
		std::cout << msg.first;
		for(auto id : msg.second) {
			sessions_[id].lock()->write(msg.first);
		}
	}
}
