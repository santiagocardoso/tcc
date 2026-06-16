#ifndef PCTT_H
#define PCTT_H

#include "ns3/mobility-module.h"
#include "pctt-vehicle.h"
#include "pctt-utils.h"
#include "pctt-header.h"
#include "minuet-communication-layer.h"
#include "minuet-utils.h"

namespace ns3 {

class CommunicationLayer;

class PCTTInterface: public Object {
public:
	PCTTInterface();
	virtual ~PCTTInterface();

	virtual void StartClustering(Ptr<EventDetected> event) = 0;
	virtual void StopClustering(void) = 0;

    virtual bool IsClusterHead() = 0;
	virtual bool IsClusterMember() = 0;
	virtual bool IsIsolated() = 0;
	virtual uint32_t GetClusterId() = 0;
	virtual bool IsStarted() = 0;

	virtual void ReceiveCHMessage(Ptr<Packet> packet, Address addr) = 0;
	virtual void ReceiveJAckMessage(Ptr<Packet> packet, Address addr) = 0;
	virtual void ReceiveCMMessage(Ptr<Packet> packet, Address addr) = 0;

	void AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer);

protected:
	Ptr<CommunicationLayer> m_communicationLayer;
};

class PCTT: public PCTTInterface {
public:
	static TypeId GetTypeId();

	PCTT ();
	virtual ~PCTT ();

	virtual void StartClustering(Ptr<EventDetected> event);
	virtual void StopClustering(void);

    virtual bool IsClusterHead();
	virtual bool IsClusterMember();
	virtual bool IsIsolated();
	virtual uint32_t GetClusterId();
	virtual bool IsStarted();

private:
	virtual void ReceiveCHMessage (Ptr<Packet> packet, Address addr);
	virtual void ReceiveJAckMessage (Ptr<Packet> packet, Address addr);
	virtual void ReceiveCMMessage (Ptr<Packet> packet, Address addr);

	void CHReceivedCMM(CMMHeader cmmHeader, CMMHeader::VehicleInfo cmmHeaderInfo);
	void CHReceivedCHM(CHMHeader chmHeader, CHMHeader::VehicleInfo chmHeaderInfo);

	void CMReceivedCMM(CMMHeader cmmHeader, CMMHeader::VehicleInfo cmmHeaderInfo);
	void CMReceivedCHM(CHMHeader chmHeader, CHMHeader::VehicleInfo chmHeaderInfo);

	void CMReceivedJack(JackMHeader jackmHeader, JackMHeader::VehicleInfo jackmHeaderInfo);
	void NMReceivedJack(JackMHeader jackmHeader, JackMHeader::VehicleInfo jackmHeaderInfo);
	void CHReceivedJack(JackMHeader jackmHeader, JackMHeader::VehicleInfo jackmHeaderInfo);

	void SendCHMessage(bool resetMessage, bool resignValue, Time resignTime);
	void SendJAckCHMessage(uint32_t nonMemberId);
	void SendCMMessage(bool predictionDenial);

	void ForwardCHMessage(CHMHeader chmHeader, CHMHeader::VehicleInfo chmHeaderInfo);
	void ForwardJAckMessage(JackMHeader jackmHeader, JackMHeader::VehicleInfo jackmHeaderInfo);
	void ForwardCMMessage(CMMHeader cmmHeader, CMMHeader::VehicleInfo cmmHeaderInfo);

	void CHProcedure();
	void PeriodicallySendsCHM();
	void CalculatePredictionOBTInML();
	void SendCHMessageBeforeConvergence ();
	void ConvergenceOver();
	void CheckOBTGreater();
	void ChangeCHToCM();

	void CMProcedure();
	void CalculateAndCheckPrediction();
	void TimeoutIntervalOver();
	void ChangeCMToCH();
	void ChangeCHInCM(int32_t chid);

	void NMProcedure();
	void ChangeNMToCH();

	void EventsListMaintenance ();

	bool HasReceivedMsg(int32_t id, int32_t pckt_id);

	void PrintInLog(string message);

	Ptr<Node> m_node;
	Ptr<MobilityModel> m_mobilityModel;

	Vector m_targetPosition;
	Vector m_targetVelocity;
	uint32_t m_eventId;

	Ptr<PCTTClusterHead> m_clusterHead;
	Ptr<PCTTClusterMember> m_clusterMember;
	Ptr<PCTTNonMember> m_nonMember;

	bool m_isClusterHead;
	bool m_isClusterMember;
	bool m_isNonMember;
	bool m_clusteringStarted;

	uint64_t m_sentCounter;

	EventId m_periodicallySendsCHMEvent;
	EventId m_calculatePredictionOBTInMLEvent;
	EventId m_checkAnsweredEvent;
	EventId m_checkOBTGreaterEvent;
	EventId m_calculateAndCheckPredictionEvent;
	EventId m_changeCHToCMEvent;

	EventId m_nmProcedureEvent;
	EventId m_changeNMToCHEvent;

	EventId m_sendCHMessageBeforeConvergenceEvent;
	EventId m_convergenceOverEvent;
	EventId m_changeCMToCHEvent;
	EventId m_changeCHInCMEvent;
	EventId m_timeoutIntervalOverEvent;

	EventId m_maintenanceEvent;
	vector<EventId> m_scheduledEvents;

	vector<vector<int32_t>> m_messagesReceived;

	bool m_outOfSim;
};
}

#endif
