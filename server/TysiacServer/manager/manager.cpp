#include "manager.h"
#include <iostream>

void SessionManager::register_session(std::shared_ptr<Session>& session)
{
	static int id = 0;
	sessions_.insert(std::make_pair(id++, session));
	//sessions_.emplace(id++, (new Session(manager, socket)));
	std::cout << id << " reg\n";
}

void SessionManager::unregister_session(int id)
{
	sessions_.erase(id);
	std::cout << id << " unreg; size = " << sessions_.size();
}

void SessionManager::acknowledge()
{
	std::cout << "ack\n";
}
