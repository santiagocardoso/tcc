#ifndef SOCIAL_H
#define SOCIAL_H

#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "social-header.h"
#include "social-utils.h"
#include "minuet-communication-layer.h"

namespace ns3 {

class CommunicationLayer;

class SOCIALInterface : public Object {
public:
	SOCIALInterface();
	virtual ~SOCIALInterface();

	virtual void StartClustering() = 0;
	virtual void StopClustering(void) = 0;

	virtual bool IsGateway() = 0;
	virtual bool IsRelay() = 0;
	virtual bool IsClusterMember() = 0;
	virtual bool IsIsolated() = 0;
	virtual uint16_t GetRelayId(uint16_t op) = 0;
	virtual bool IsStarted() = 0;

	virtual void ReceiveControlMessage(Ptr<Packet> packet, Address addr) = 0;

	void AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer);

protected:
	Ptr<CommunicationLayer> m_communicationLayer;
};

class VehicleSOCIAL {
public:
	VehicleSOCIAL();
	VehicleSOCIAL(uint16_t id, uint32_t m_score, bool isRelay, uint16_t relayId, Vector position, bool isPerfilSocial, bool isGateway, uint32_t m_eigenScore, float closenessScore, vector<vector<uint16_t>> neighborsList, uint16_t atividadeSocial); //Todas
	//VehicleSOCIAL(uint16_t id, uint32_t m_score, bool isRelay, uint16_t relayId, Vector position, bool isPerfilSocial, bool isGateway, uint32_t m_eigenScore, float closenessScore, vector<vector<uint16_t>> neighborsList); //STR
	//VehicleSOCIAL(uint16_t id, uint32_t m_atividadeSocial, bool isRelay, uint16_t relayId, Vector position, bool isPerfilSocial, bool isGateway); //Atividade Social
	//VehicleSOCIAL(uint16_t id, uint32_t m_amizade, bool isRelay, uint16_t relayId, Vector position, bool isPerfilSocial, bool isGateway); //Amizade

	~VehicleSOCIAL();

	uint16_t GetRelayId() const;
	void SetRelayId(uint16_t relayId);

	Vector GetDirection() const;
	void SetDirection(Vector direction);

	uint16_t GetId() const;
	void SetId(uint16_t id);

	bool IsRelay() const;
	void SetIsRelay(bool isRelay);

	bool IsGateway() const;
	void SetIsGateway(bool isGateway);

	Vector GetPosition() const;
	void SetPosition(const Vector position);

	uint32_t GetScore() const;
	void SetScore(uint32_t score);

	uint32_t GetEigenScore() const;
	void SetEigenScore(uint32_t eigenScore);

	float GetClosenessScore() const;
	void SetClosenessScore(float closenessScore);

	uint16_t GetAtividadeSocial() const;
	void SetAtividadeSocial(uint16_t atividadeSocial);

	vector<vector<uint16_t>> GetNeighborsList() const;

	bool IsPerfilSocial() const;
	void SetIsPerfilSocial(bool isPerfilSocial);

	double_t GetTimeExpire() const;
	void SetTimeExpire(double_t timeExpire);


private:
	uint16_t m_id;
	double_t m_timeExpire;
	bool m_isRelay;
	uint16_t m_relayId;
	Vector m_position;
	Vector m_direction;
	bool m_isPerfilSocial;
	bool m_isGateway;
	std::vector<std::vector<uint16_t>> m_neighborsList;

	uint16_t m_atividadeSocial;

	uint32_t m_score;
	uint32_t m_eigenScore;
	float m_closenessScore;

	//uint16_t m_idadeVeiculo;
};

class SOCIAL : public SOCIALInterface {

public:
	static TypeId GetTypeId();

	SOCIAL();
	virtual ~SOCIAL();

    virtual void StartClustering(void);
    virtual void StopClustering(void);

    virtual bool IsRelay();
    virtual bool IsGateway();
    virtual void SetIsGateway(bool isGateway);
    virtual bool IsPerfilSocial();
	virtual bool IsClusterMember();
	virtual bool IsIsolated();
	virtual uint16_t GetRelayId(uint16_t op);
	virtual bool IsStarted();

private:
    virtual void ReceiveControlMessage(Ptr<Packet> packet, Address addr);

	void HandleBeacon (VehicleSOCIAL vehicle2);
	uint16_t RelayElection();
	uint16_t RelayElectionSTR();
	uint16_t RelayElectionAtivSocial();
	//uint16_t RelayElectionAmizade();
	void RelayElectionAmizade();  //APAGAR ESSA LINHA E DESCOMENTAR A ANTERIOR
	void Purge (uint32_t vehicleId, vector<VehicleSOCIAL>& vehicleVector);
	void CleanUp ();

	void VerifyRelay (uint32_t vehicleId);
	void HasHigherScore (uint32_t score1);
	void CheckIncreaseByThreshold (VehicleSOCIAL vehicle2, double_t separation, double_t angleDiff);
	void CheckIncreaseByThresholdInAngleAbove45 (VehicleSOCIAL vehicle2, double_t separation, double_t angleDiff);

	void SendBeacon ();
	void SendMessage (BeaconHeader header);

	double_t GetAngleDiff (Vector v1, Vector v2);

	void EventsListMaintenance ();

	void PrintInLog(string message);
	void PrintInGetEdgesOnAnInstant(string message);

	///// Attributes Node  //////
	Ptr<Node> m_node;
	bool m_isRelay;
	uint16_t m_relayId;
	bool m_isPerfilSocial;
	bool m_isGateway;
	Ptr<MobilityModel> m_mobilityModel;

	// static std::map<std::string, std::vector<int>> m_randomData;
	static std::map<std::string, std::map<int, int>> m_randomData;
    static bool m_dataLoaded; // mapa random.txt

	uint32_t m_eigenScore;
	float m_closenessScore;
	uint32_t m_score;


	////////////////////////////

	////// Metrics Weights ///////
	float wBC;
	float wCC;
	float wDC;
	float wEC;
	//////////////////////////////

	//vector<VehicleSOCIAL> neighbors;
	vector<VehicleSOCIAL> closeNeighbors;
	//vector<VehicleSOCIAL> clusterMembers;
	vector<vector<uint16_t>> m_neighborsList;
	vector<BeaconHeader> packetList;

	vector<vector<uint16_t>> novosVizinhos;
	uint16_t m_atividadeSocial;

	bool m_clusteringStarted;

	uint64_t m_sentCounter;

	EventId m_sendEvent;
	EventId m_cleanEvent;
	EventId m_maintenanceEvent;
	//EventId m_neighborMaintenanceEvent;

	vector<EventId> scheduledCheckEvents;

	bool m_outOfSim;
};

}


#endif
