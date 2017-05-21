#include "manager.h"

void SessionManager::registerSession(const std::shared_ptr<Session>& session)
{
	sessions_.insert(std::make_pair(session->getId(), session));
}

void SessionManager::unregisterSession(size_t id)
{
	if (!sessions_.erase(id)) {
		throw std::runtime_error("SessionManager: trying to unregister non-existing session");
	}
}

void SessionManager::interpret(size_t id, const std::string& message)
{
	auto return_data = game_.doWork(id, message);
	for (auto& msg : return_data) {
		for(auto id : msg.second) {
			if (auto session = sessions_.at(id).lock()) {
				session->write(msg.first);
			} else {
				throw std::runtime_error("SessionManager: trying to send data to non-existing session");
			}
		}
	}
}

size_t SessionManager::connected() const
{
	return sessions_.size();
}