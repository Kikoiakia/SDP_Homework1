#include "interface.h"
#include <vector>

const int RESTOCK_TIME = 60;
const int RESTOCK_AMOUNT = 100;

/// This is sample empty implementation you can place your solution here or delete this and include tests to your solution
struct Worker {
	int minuteSend;
	ResourceType type;
};
struct MyStore : Store
{
private:
	ActionHandler* actionHandler = nullptr;
	std::vector<Client> clients;
	std::vector<Worker> workers;
	std::vector<int> indexes;
	int stockBananas = 0;
	int stockSchweppes = 0;
	int localTime = 0;
	int workerBananas = 0;
	int workerSchweppes = 0;
	int clientsCount = 0;
	int availableWorkers = 0;
	int bananaWorkers = 0;
	

public:
	void setActionHandler(ActionHandler* handler) override
	{
		actionHandler = handler;
	}

	void init(int workerCount = 0, int startBanana = 0, int startSchweppes = 0) override
	{
		this->availableWorkers = workerCount;
		stockBananas = startBanana;
		stockSchweppes = startSchweppes;
	}

	void addClients(const Client* clients, int count) override
	{
		for (int i = 0; i < count; i++) {
			this->clients.push_back(clients[i]);
		}
		clientsCount += count;
	}

	void advanceTo(int minute) override
	{
		CheckInventory(minute);

		RechargeStock(minute);

		CheckDeparture(minute);

		localTime = minute;
	}

	virtual int getBanana() const
	{
		return stockBananas;
	}

	virtual int getSchweppes() const
	{
		return stockSchweppes;
	}



	private:

		void CheckInventory(int minute)
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
				if (clients[i].arriveMinute <= minute && clients[i].arriveMinute >= localTime)
				{

					bool hasBananas = false;
					bool hasSchweppes = false;
					if (clients[i].banana <= stockBananas) hasBananas = true;
					if (clients[i].schweppes <= stockSchweppes) hasSchweppes = true;

					if (hasBananas && hasSchweppes)
					{
						actionHandler->onClientDepart(i, minute, clients[i].banana, clients[i].schweppes);
						stockBananas -= clients[i].banana;
						stockSchweppes -= clients[i].schweppes;
						indexes.push_back(i);
					}
					else if (clients[i].banana > stockBananas + workerBananas && clients[i].schweppes > stockSchweppes + workerSchweppes) {
						int wantedBanana = clients[i].banana - workerBananas;
						int wantedSchweppes = clients[i].schweppes - workerSchweppes;
						while (wantedSchweppes >= 0 && wantedBanana >= 0) {
							if (wantedBanana >= wantedSchweppes && availableWorkers > 0) {
								actionHandler->onWorkerSend(minute, ResourceType::banana);
								Worker worker = { clients[i].arriveMinute, ResourceType::banana };
								workers.push_back(worker);
								availableWorkers--;
								wantedBanana -= RESTOCK_AMOUNT;
								workerBananas += RESTOCK_AMOUNT;
							}
							else if(wantedBanana < wantedSchweppes && availableWorkers > 0){
								actionHandler->onWorkerSend(minute, ResourceType::schweppes);
								Worker worker = { clients[i].arriveMinute, ResourceType::schweppes };
								workers.push_back(worker);
								availableWorkers--;
								wantedSchweppes -= RESTOCK_AMOUNT;
								workerSchweppes += RESTOCK_AMOUNT;
							}
							else {
								break;
							}
						}
					}
					else if (clients[i].banana > stockBananas + workerBananas) {
						int wantedBanana = clients[i].banana - workerBananas;
						while (wantedBanana > 0 && availableWorkers > 0) {
							actionHandler->onWorkerSend(minute, ResourceType::banana);
							Worker worker = { clients[i].arriveMinute, ResourceType::banana };
							workers.push_back(worker);
							availableWorkers--;
							wantedBanana -= RESTOCK_AMOUNT;
							workerBananas += RESTOCK_AMOUNT;
						}
					}
					else {
						int wantedSchweppes = clients[i].schweppes - workerSchweppes;
						while (wantedSchweppes > 0 && availableWorkers > 0) {
							actionHandler->onWorkerSend(minute, ResourceType::schweppes);
							Worker worker = { clients[i].arriveMinute, ResourceType::schweppes };
							workers.push_back(worker);
							availableWorkers--;
							wantedSchweppes -= RESTOCK_AMOUNT;
							workerSchweppes += RESTOCK_AMOUNT;
						}
					}
				}
			}
		}

		void RechargeStock(int minute)
		{
			bool recharged = false;
			for (int i = 0; i < workers.size(); i++) {
				if (workers[i].minuteSend + RESTOCK_TIME <= minute && workers[i].minuteSend + RESTOCK_TIME >= localTime) {
					actionHandler->onWorkerBack(workers[i].minuteSend + RESTOCK_TIME, workers[i].type);
					if (workers[i].type == ResourceType::banana)
						stockBananas += RESTOCK_AMOUNT;
					else
						stockSchweppes += RESTOCK_AMOUNT;
					workers.erase(workers.begin() + i);
					availableWorkers++;
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
					if (clients[i].arriveMinute <= minute) {
						if (clients[i].banana <= stockBananas && clients[i].schweppes <= stockSchweppes) {
							actionHandler->onClientDepart(i, minute, clients[i].banana, clients[i].schweppes);
							stockBananas -= clients[i].banana;
							stockSchweppes -= clients[i].schweppes;
							indexes.push_back(i);
						}
					}
				}
			}
		}


		void CheckDeparture(int minute)
		{
			for (int i = 0; i < clientsCount; i++) {

				bool away = false;
				for (int j = 0; j < indexes.size(); j++) {
					if (i == indexes[j])
						away = true;
				}
				if (away) continue;
				if (clients[i].arriveMinute + clients[i].maxWaitTime <= minute && clients[i].arriveMinute + clients[i].maxWaitTime >= localTime)
				{
					if (clients[i].banana > stockBananas)
					{
						if (clients[i].schweppes > stockSchweppes)
						{
							actionHandler->onClientDepart(i, minute, stockBananas, stockSchweppes);
							stockBananas = 0;
							stockSchweppes = 0;
							indexes.push_back(i);
						}
						else
						{
							actionHandler->onClientDepart(i, minute, stockBananas, clients[i].schweppes);
							stockBananas = 0;
							stockSchweppes -= clients[i].schweppes;
							indexes.push_back(i);
						}
					}
					else
					{
						if (clients[i].schweppes > stockSchweppes)
						{
							actionHandler->onClientDepart(i, minute, clients[i].banana, stockSchweppes);
							stockBananas -= clients[i].banana;
							stockSchweppes = 0;
							indexes.push_back(i);
						}
						else
						{
							actionHandler->onClientDepart(i, minute, clients[i].banana, clients[i].schweppes);
							stockBananas -= clients[i].banana;
							stockSchweppes -= clients[i].schweppes;
							indexes.push_back(i);
						}
					}

				}
			}
		}
};

Store* createStore()
{
	return new MyStore();
}