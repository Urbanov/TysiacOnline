#include "manager.h"


/**
 * @brief adds a session to list of valid sessions
 *  
 * @param session session to be registered
 */
void SessionManager::registerSession(const std::shared_ptr<Session>& session)
{
	sessions_.insert(std::make_pair(session->getId(), session));
}


/**
 * @brief removes session from list of valid sessions
 * 
 * @param id id of session to be removed
 */
void SessionManager::unregisterSession(size_t id)
{
	if (!sessions_.erase(id)) {
		throw std::runtime_error("SessionManager: trying to unregister non-existing session");
	}
}


/**
 * @brief calls game engine to interpret the message and sends a response to clients
 * 
 * @param id id of client who sent the message
 * @param message actual message
 */
void SessionManager::interpret(size_t id, const std::string& message)
{
	auto return_data = game_.doWork(id, message);
	for (auto& msg : return_data) {
		for (auto user_id : msg.second) {
			if (auto session = sessions_.at(user_id).lock()) {
				session->write(msg.first);
			} 
			else {
				throw std::runtime_error("SessionManager: trying to send data to non-existing session");
			}
		}
	}
}


/**
 * @brief returns the number of connected clients
 * 
 * @return number of valid sessions
 */
size_t SessionManager::connected() const
{
	return sessions_.size();
}
