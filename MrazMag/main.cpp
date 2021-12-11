#include <iostream>
#include "interface.h"
#include <vector>


const int RESTOCK_TIME = 60;
const int RESTOCK_AMOUNT = 100;

typedef std::vector<Client> ClientList;


struct StoreEvent {
	enum Type {
		WorkerSend, WorkerBack, ClientDepart
	};

	Type type;
	int minute;

	struct Worker {
		ResourceType resource;
	} worker;

	struct Client {
		int index;
		int banana;
		int schweppes;
	} client;
};

struct TestStore : ActionHandler {
	Store* impl = nullptr;
	std::vector<StoreEvent> log;

	TestStore()
		: impl(createStore()) {
		impl->setActionHandler(this);
	}

	~TestStore() {
		delete impl;
	}

	TestStore(const TestStore&) = delete;
	TestStore& operator=(const TestStore&) = delete;

	void init(int workerCount, int startBanana, int startSchweppes) {
		impl->init(workerCount, startBanana, startSchweppes);
	}

	void advanceTo(int minute) {
		impl->advanceTo(minute);
	}

	int getBanana() const {
		return impl->getBanana();
	}

	int getSchweppes() const {
		return impl->getSchweppes();
	}

	void addClients(const ClientList& clients) {
		impl->addClients(clients.data(), clients.size());
	}

	void addClients(const Client& single) {
		impl->addClients(&single, 1);
	}

	void onWorkerSend(int minute, ResourceType resource) override {
		if (resource == ResourceType::banana)
			std::cout << "W " << minute << " banana\n";
		else if (resource == ResourceType::schweppes)
			std::cout << "W " << minute << " schweppes\n";
		else
			throw "Unexpected resource type in onWorkerSend.";
	}

	void onWorkerBack(int minute, ResourceType resource) override {
		if (resource == ResourceType::banana)
			std::cout << "D " << minute << " banana\n";
		else if (resource == ResourceType::schweppes)
			std::cout << "D " << minute << " schweppes\n";
		else
			throw "Unexpected resource type in onWorkerBack.";
	}

	void onClientDepart(int index, int minute, int banana, int schweppes) override {
		std::cout << index << " " << minute << " " << banana << " " << schweppes << "\n";
	}
};


int main() {

	TestStore store;
	int W, C, arrival, banana, schweppes, maxWaitTime;
	std::cin >> W >> C;
	std::vector<Client> ClientList;
	for (int i = 0; i < C; i++) {
		std::cin >> arrival >> banana >> schweppes >> maxWaitTime;
		Client client = { arrival, banana, schweppes, maxWaitTime };
		ClientList.push_back(client);
	}


	store.addClients(ClientList);
	store.init(W, 0, 0);
	Client lastClient = ClientList.back();
	int endMinute = lastClient.arriveMinute + 1000;
	for (int i = 0; i < endMinute; ++i) {
		store.advanceTo(i);
	}

	return 0;
}