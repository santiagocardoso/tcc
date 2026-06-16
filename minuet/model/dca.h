#ifndef DCA_H
#define DCA_H

#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "dca-header.h"
#include "dca-utils.h"
#include "minuet-communication-layer.h"

namespace ns3 {

class CommunicationLayer;

class DCAInterface : public Object {
public:
	DCAInterface();
	virtual ~DCAInterface();

	virtual void StartClustering() = 0;
	virtual void StopClustering(void) = 0;

	virtual bool IsClusterHead() = 0;
	virtual bool IsClusterMember() = 0;
	virtual bool IsIsolated() = 0;
	virtual uint32_t GetClusterId() = 0;
	virtual bool IsStarted() = 0;

	virtual void ReceiveControlMessage(Ptr<Packet> packet, Address addr) = 0;

	void AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer);

protected:
	Ptr<CommunicationLayer> m_communicationLayer;
};

class VehicleDCA {
public:
	VehicleDCA();
	VehicleDCA(uint32_t id, uint32_t m_score, bool isClusterHead, uint32_t clusterHeadId, Vector position, Vector diretion);
	~VehicleDCA();

	uint32_t GetClusterHeadId() const;
	void SetClusterHeadId(uint32_t clusterHeadId);

	Vector GetDirection() const;
	void SetDirection(Vector direction);

	uint32_t GetId() const;
	void SetId(uint32_t id);

	bool IsClusterHead() const;
	void SetIsClusterHead(bool isClusterHead);

	Vector GetPosition() const;
	void SetPosition(const Vector position);

	uint32_t GetScore() const;
	void SetScore(uint32_t score);

	double_t GetTimeExpire() const;
	void SetTimeExpire(double_t timeExpire);

private:
	uint32_t m_id;
	uint32_t m_score;
	double_t m_timeExpire;
	bool m_isClusterHead;
	uint32_t m_clusterHeadId;
	Vector m_position;
	Vector m_direction;

};

class DCA : public DCAInterface {

public:
	static TypeId GetTypeId();

	DCA();
	virtual ~DCA();

    virtual void StartClustering(void);
    virtual void StopClustering(void);

    virtual bool IsClusterHead();
	virtual bool IsClusterMember();
	virtual bool IsIsolated();
	virtual uint32_t GetClusterId();
	virtual bool IsStarted();

private:
    virtual void ReceiveControlMessage(Ptr<Packet> packet, Address addr);

	void HandleBeacon (VehicleDCA vehicle2);
	void ClusterHeadElection ();
	void Purge (uint32_t vehicleId, vector<VehicleDCA>& vehicleVector);
	void CleanUp ();

	void VerifyClusterHead (uint32_t vehicleId);
	void HasHigherScore (uint32_t score1);
	void CheckIncreaseByThreshold (VehicleDCA vehicle2, double_t separation, double_t angleDiff);
	void CheckIncreaseByThresholdInAngleAbove45 (VehicleDCA vehicle2, double_t separation, double_t angleDiff);

	void SendBeacon ();
	void SendMessage (BeaconHeader2 header);

	double_t GetAngleDiff (Vector v1, Vector v2);

	void EventsListMaintenance ();

	void PrintInLog(string message);

	///// Attributes Node  //////
	Ptr<Node> m_node;
	uint32_t m_score;
	bool m_isClusterHead;
	uint32_t m_clusterHeadId;
	Ptr<MobilityModel> m_mobilityModel;
	////////////////////////////

	vector<VehicleDCA> neighbors;
	vector<VehicleDCA> closeNeighbors;
	vector<VehicleDCA> clusterMembers;

	bool m_clusteringStarted;

	uint64_t m_sentCounter;

	EventId m_sendEvent;
	EventId m_cleanEvent;
	EventId m_maintenanceEvent;

	vector<EventId> scheduledCheckEvents;

	bool m_outOfSim;

};

}


#endif
