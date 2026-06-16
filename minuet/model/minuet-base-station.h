/*
 * minuet-base-station.h
 *
 *  Created on: 24 de nov de 2018
 *      Author: everaldo
 */

#ifndef MINUET_BASE_STATION_H_
#define MINUET_BASE_STATION_H_

#include <regex>
#include <ctime>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "minuet-utils.h"
#include "minuet-header.h"

using namespace std;

namespace ns3 {

class BaseStation : public Object {
public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId(void) const;

	BaseStation();
	~BaseStation();

	uint32_t GetBaseStationId() const;
	void SetBaseStationId(uint32_t baseStationId);

	double_t GetPosX() const;
	void SetPosX(double_t posX);

	double_t GetPosY() const;
	void SetPosY(double_t posY);

private:
	uint32_t m_baseStationId;
	double_t m_pos_x;
	double_t m_pos_y;

};

class BaseStationUtils {
public:
	static vector<Ptr<BaseStation>> BaseStationGenerator();

private:
	static Ptr<BaseStation> CreateBaseStation(uint32_t baseStationId, double_t pos_x, double_t pos_y);
};

class AppBaseStation : public Application {

public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	AppBaseStation();
	virtual ~AppBaseStation();

private:
	/// inherited from Application base class.
	virtual void StartApplication (void);    // Called at time specified by Start
	virtual void StopApplication (void);     // Called at time specified by Stop

	void ReceiveDataMessage(Ptr<Socket> socket);
	void SendAnnouncementBaseStationMessage();

	void PrintInLog(string message);
	void PrintInDump(uint64_t pkt_id, uint32_t pkt_size, uint32_t event_id, uint32_t node_id);
	bool MessageIsRedundant(uint64_t seq, uint32_t eventId, uint32_t monitorId);

	Ptr<Socket> m_listenDataSocket;
	Ptr<Socket> m_sendControlSocket;

	uint64_t m_sentCounter;

	vector<vector<uint64_t>> m_deliveredMessages;
};

} // namespace ns3

#endif /* MINUET_BASE_STATION_H_ */
