#include "manager.h"

void SessionManager::registerSession(const std::shared_ptr<Session>& session)
{
	sessions_.insert(std::make_pair(session->getId(), session));
}

void SessionManager::unregisterSession(size_t id)
{
	sessions_.erase(id);
}

void SessionManager::interpret(size_t id, const std::string& message)
{
	auto return_data = game_.doWork(id, message);
	for (auto& msg : return_data) {
		for(auto id : msg.second) {
			sessions_.at(id).lock()->write(msg.first);
		}
	}
}
