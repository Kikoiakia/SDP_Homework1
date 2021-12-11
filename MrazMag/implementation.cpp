#include "interface.h"
#include <vector>

const int RESTOCK_TIME = 60;
const int RESTOCK_AMOUNT = 100;

/// This is sample empty implementation you can place your solution here or delete this and include tests to your solution
struct Worker {
	int minuteSend;
	ResourceType type;
	int minuteBack;
};
struct MyStore : Store
{
	ActionHandler* actionHandler = nullptr;
	std::vector<Client> clients;
	std::vector<Worker> workers;
	std::vector<int> indexes;
	int incomingBananas = 0;
	int incomingSchweppes = 0;
	int clientsCount = 0;
	int workerCount = 0;
	int bananas = 0;
	int schweppes = 0;
	int time = 0;

	void setActionHandler(ActionHandler* handler) override
	{
		actionHandler = handler;
	}

	void init(int workerCount = 0, int startBanana = 0, int startSchweppes = 0) override
	{
		this->workerCount = workerCount;
		bananas = startBanana;
		schweppes = startSchweppes;
	}

	void addClients(const Client* clients, int count) override
	{
		for (int i = 0; i < count; i++) {
			this->clients.push_back(*(clients+i));
		}
		clientsCount += count;
	}

	void advanceTo(int minute) override
	{
		for (int i = 0; i < clientsCount; i++)
		{
			bool away = false;
			for (int j = 0; j < indexes.size(); j++) {
				if (i == indexes[j]) {
					away = true;
					break;
				}
			}
			if (away) continue;
			if (clients[i].arriveMinute <= minute && clients[i].arriveMinute >= time)
			{

				bool hasBananas = false;
				bool hasSchweppes = false;
				if (clients[i].banana > bananas + incomingBananas && workerCount > 0)
				{
					bool workerSend = false;
					for (int j = 0; j < workers.size(); j++) {
						if (workers[j].type == ResourceType::banana) {
							workerSend = true;
							break;
						}
					}
					int stockWanted = clients[i].banana;
					while (stockWanted - bananas > 0 && workerCount > 0 && !workerSend) {
						actionHandler->onWorkerSend(minute, ResourceType::banana);
						Worker worker = { clients[i].arriveMinute, ResourceType::banana, clients[i].arriveMinute + RESTOCK_TIME };
						workers.push_back(worker);
						workerCount--;
						stockWanted -= RESTOCK_AMOUNT;
						incomingBananas += RESTOCK_AMOUNT;
					}
				}
				else if(clients[i].banana <= bananas)
					hasBananas = true;

				if (clients[i].schweppes > schweppes + incomingSchweppes && workerCount > 0)
				{
					bool workerSend = false;
					for (int j = 0; j < workers.size(); j++) {
						if (workers[j].type == ResourceType::schweppes) {
							workerSend = true;
							break;
						}
					}
					int stockWanted = clients[i].schweppes;
					while (stockWanted - schweppes > 0 && workerCount > 0 && !workerSend) {
						actionHandler->onWorkerSend(minute, ResourceType::schweppes);
						Worker worker = { clients[i].arriveMinute, ResourceType::schweppes, clients[i].arriveMinute + RESTOCK_TIME };
						workers.push_back(worker);
						workerCount--;
						stockWanted -= RESTOCK_AMOUNT;
						incomingSchweppes += RESTOCK_AMOUNT;
					}
				}
				else if(clients[i].schweppes <= schweppes)
					hasSchweppes = true;

				if(hasBananas && hasSchweppes)
				{
					actionHandler->onClientDepart(i, minute, clients[i].banana, clients[i].schweppes);
					bananas -= clients[i].banana;
					schweppes -= clients[i].schweppes;
					indexes.push_back(i);
				}
			}
		}

		bool recharged = false;
		for (int i = 0; i < workers.size(); i++) {
			if (workers[i].minuteBack <= minute && workers[i].minuteBack >= time) {
				actionHandler->onWorkerBack(workers[i].minuteBack, workers[i].type);
				if (workers[i].type == ResourceType::banana)
					bananas += RESTOCK_AMOUNT;
				else
					schweppes += RESTOCK_AMOUNT;
				workers.erase(workers.begin()+i);
				workerCount++;
				recharged = true;
				i--;
			}
		}
		if (recharged) {
			for (int i = 0; i < clientsCount; i++) {

				bool away = false;
				for (int j = 0; j < indexes.size(); j++) {
					if (i == indexes[j]) {
						away = true;
						break;
					}
				}
				if (away) continue;
				if (clients[i].arriveMinute <= minute){
					if (clients[i].banana <= bananas && clients[i].schweppes <= schweppes) {
						actionHandler->onClientDepart(i, minute, clients[i].banana, clients[i].schweppes);
						bananas -= clients[i].banana;
						schweppes -= clients[i].schweppes;
						indexes.push_back(i);
					}
				}
			}
		}
		for(int i = 0; i < clientsCount; i++){

			bool away = false;
			for (int j = 0; j < indexes.size(); j++) {
				if (i == indexes[j])
				away = true;
			}
			if (away) continue;
			if (clients[i].arriveMinute + clients[i].maxWaitTime <= minute && clients[i].arriveMinute + clients[i].maxWaitTime >= time)
			{
				if (clients[i].banana > bananas)
				{
					if (clients[i].schweppes > schweppes)
					{
						actionHandler->onClientDepart(i, minute, bananas, schweppes);
						bananas = 0;
						schweppes = 0;
						indexes.push_back(i);
					}
					else
					{
						actionHandler->onClientDepart(i, minute, bananas, clients[i].schweppes);
						bananas -= 0;
						schweppes -= clients[i].schweppes;
						indexes.push_back(i);
					}
				}
				else
				{
					if (clients[i].schweppes > schweppes)
					{
						actionHandler->onClientDepart(i, minute, clients[i].banana, schweppes);
						bananas -= clients[i].banana;
						schweppes = 0;
						indexes.push_back(i);
					}
					else
					{
						actionHandler->onClientDepart(i, minute, clients[i].banana, clients[i].schweppes);
						bananas -= clients[i].banana;
						schweppes -= clients[i].schweppes;
						indexes.push_back(i);
					}
				}
				
			}
		}
		
		time = minute;
	}

	virtual int getBanana() const
	{
		return bananas;
	}

	virtual int getSchweppes() const
	{
		return schweppes;
	}
};

Store* createStore()
{
	return new MyStore();
}