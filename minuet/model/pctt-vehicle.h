#ifndef PCTT_VEHICLE_H
#define PCTT_VEHICLE_H

#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/nstime.h"
#include "pctt-utils.h"

using namespace std;

namespace ns3 {

class PCTTClusterHead;
class PCTTClusterMember;
class PCTTNonMember;

class PCTTVehicle : public Object {
public:
	/**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
	static TypeId GetTypeId (void);

	PCTTVehicle();
	virtual ~PCTTVehicle ();

	Time CalculateOBT(Vector targetPosition, Vector targetVelocity, Vector vehiclePosition, Vector vehicleVelocity);

	int32_t GetId();
	void SetID(int32_t cchid);

	int32_t GetCHID();
	void SetCHID(int32_t chid);

	Vector GetTargetPosition();
	void SetTargetPosition(Vector targetPosition);

	Vector GetTargetVelocity();
	void SetTargetVelocity(Vector targetVelocity);

	Ptr<MobilityModel> GetMobilityModel();
	void SetMobilityModel(Ptr<MobilityModel> mobilityModel);

	bool GetTDV(Vector targetPosition, Vector targetVelocity);
	void SetTDV(bool tdv);

	Time GetCCHOBT();

	bool SimpleGetTDV();

	Vector PredictNextPosition(Vector lastPos, Vector currentPos);

	virtual Ptr<PCTTClusterHead> ToClusterHead();
	virtual Ptr<PCTTClusterMember> ToClusterMember();
	virtual Ptr<PCTTNonMember> ToNonMember();

	int32_t m_id;
	int32_t m_chid;

	Vector m_targetPosition;
	Vector m_targetVelocity;

	Ptr<MobilityModel> m_mobilityModel;

	bool m_targetDetectionValue;

private:
	double_t GetAngle (Vector v1);

};

class PCTTClusterMember : public PCTTVehicle {
public:
	/**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
	static TypeId GetTypeId (void);

	PCTTClusterMember();
	virtual ~PCTTClusterMember();

	Vector GetLastPosition();
	void SetLastPosition(Vector pos);

	Vector GetLastVelocity();
	void SetLastVelocity(Vector lastVelocity);

private:
	Vector m_lastPosition;
	Vector m_lastVelocity;

};

class PCTTClusterHead: public PCTTVehicle {
public:
	/**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
	static TypeId GetTypeId (void);

	PCTTClusterHead();
	virtual ~PCTTClusterHead();

	struct CM {
		int32_t id;
		int32_t chid;
		Vector lastPosition;
		Vector lastVelocity;
		Time obt;
		bool answered;
	};

	int32_t GetCCHID();
	void SetCCHID(int32_t cchid);

	vector<PCTTClusterHead::CM>& GetMemberList();
	void SetMemberList(vector<PCTTClusterHead::CM> memberList);

	void SearchCHAndCCHInMemberList();
	bool ExistOBTGreater();

	Ptr<PCTTClusterMember> ToClusterMember();

	void AddToMemberList(PCTTClusterHead::CM cm);

	void UpdateNodePosition(int32_t nodeId, Vector lastPosition, Vector lastVelocity);
	void CheckAnswered();

	double_t GetAngleDiff(Vector v1, Vector v2);
	bool IsEqualTargetDirection(Vector targetVelocity, Vector targetPosition, Vector vehicleVelocity, Vector vehiclePosition);

private:
	int32_t m_cchid;
	vector<CM> m_memberList;

};

class PCTTNonMember : public PCTTVehicle {
public:
	/**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
	static TypeId GetTypeId (void);

	PCTTNonMember();
	virtual ~PCTTNonMember();

	Ptr<PCTTClusterMember> ToClusterMember(int32_t chid);
};

}

#endif
