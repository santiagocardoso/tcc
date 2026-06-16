#include "dca.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DCA");

/*************** ClusteringManagerInterface ************************/
NS_OBJECT_ENSURE_REGISTERED (DCAInterface);

DCAInterface::DCAInterface() {
	NS_LOG_FUNCTION(this);
}

DCAInterface::~DCAInterface() {
	NS_LOG_FUNCTION(this);
}

void DCAInterface::AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer) {
	DCAInterface::m_communicationLayer = comunicationLayer;
	DCAInterface::m_communicationLayer->AttachDCAInterface(this);
}
/*************** END ClusteringManagerInterface ************************/

/*************** VehicleDCA ************************/

VehicleDCA::VehicleDCA(){/* Empty */}

VehicleDCA::VehicleDCA(uint32_t id, uint32_t score, bool isClusterHead, uint32_t clusterHeadId, Vector position, Vector diretion) {
	m_id = id;
	m_score = score;
	m_isClusterHead = isClusterHead;
	m_clusterHeadId = clusterHeadId;
	m_position = position;
	m_direction = diretion;
	m_timeExpire = Simulator::Now().GetSeconds();
}

VehicleDCA::~VehicleDCA(){/* Empty */}

uint32_t VehicleDCA::GetClusterHeadId() const {
	return m_clusterHeadId;
}

void VehicleDCA::SetClusterHeadId(uint32_t clusterHeadId) {
	m_clusterHeadId = clusterHeadId;
}

Vector VehicleDCA::GetDirection() const {
	return m_direction;
}

void VehicleDCA::SetDirection(Vector direction) {
	m_direction = direction;
}

uint32_t VehicleDCA::GetId() const {
	return m_id;
}

void VehicleDCA::SetId(uint32_t id) {
	m_id = id;
}

bool VehicleDCA::IsClusterHead() const {
	return m_isClusterHead;
}

void VehicleDCA::SetIsClusterHead(bool isClusterHead) {
	m_isClusterHead = isClusterHead;
}

Vector VehicleDCA::GetPosition() const {
	return m_position;
}

void VehicleDCA::SetPosition(const Vector position) {
	m_position = position;
}

uint32_t VehicleDCA::GetScore() const {
	return m_score;
}

void VehicleDCA::SetScore(uint32_t score) {
	m_score = score;
}

double_t VehicleDCA::GetTimeExpire() const {
	return m_timeExpire;
}

void VehicleDCA::SetTimeExpire(double_t timeExpire) {
	m_timeExpire = timeExpire;
}

/*************** END VehicleDCA ************************/

NS_OBJECT_ENSURE_REGISTERED(DCA);

TypeId DCA::GetTypeId() {
	static TypeId typeId =
			TypeId("ns3::DCA")
			.AddConstructor<DCA>()
			.SetParent<Object>()
			.AddAttribute("Node",
					"The Node of the DCA", PointerValue(),
					MakePointerAccessor(&DCA::m_node),
					MakePointerChecker<Node>())
			.AddAttribute("CommunicationLayer",
					"The Communication Instance", PointerValue(),
					MakePointerAccessor(&DCA::m_communicationLayer),
					MakePointerChecker<CommunicationLayer>());

	return typeId;
}

DCA::DCA () {
	NS_LOG_FUNCTION(this);
	m_sentCounter = 0;
	m_outOfSim = true;
}

DCA::~DCA() {
	NS_LOG_FUNCTION(this);
}

void DCA::StartClustering () {
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("RUNNING STARTCLUSTERING!");

	m_mobilityModel = m_node->GetObject<MobilityModel>();

	m_outOfSim = false;

	AddCommunicationLayer(m_communicationLayer);

	// Iniciando informações do nó
	m_isClusterHead = true;
	m_clusterHeadId = m_node->GetId();
	m_score = 0;

	PrintInLog("DCA Algorithm Started");

//	m_sendEvent = Simulator::Schedule (Seconds(m_node->GetId() * 0.001), &DCA::SendBeacon, this);
	SendBeacon();

	// Agendamento de manutenção de listas de cluster e eventos agendados
	m_cleanEvent = Simulator::Schedule (Seconds(m_node->GetId() * 0.001), &DCA::CleanUp, this);
	m_maintenanceEvent = Simulator::Schedule (Seconds(m_node->GetId() * 0.001), &DCA::EventsListMaintenance, this);

	m_clusteringStarted = true;
}

void DCA::StopClustering () {
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("RUNNING STOPCLUSTERING!");

	//Flag declarando-se fora da simulação
	m_outOfSim = true;

	//Cancelando eventos ativos
	m_sendEvent.Cancel();
	m_cleanEvent.Cancel();
	m_maintenanceEvent.Cancel();

	if (scheduledCheckEvents.size() != 0) {
		for (EventId eventId : scheduledCheckEvents) {
			eventId.Cancel();
		}
		scheduledCheckEvents.clear();
	}

	m_clusteringStarted = false;

	PrintInLog("DCA Algorithm Stopped");
}

void DCA::ReceiveControlMessage (Ptr<Packet> packet, Address addr) {
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("MESSAGE RECEIVED!");

	if (!m_outOfSim) {
		BeaconHeader2 beaconHeader;
		BeaconHeader2::VehicleInfo vehicleInfo;

		packet->RemoveHeader(beaconHeader);
		vehicleInfo = beaconHeader.GetVehicleInfo();

		VehicleDCA vehicle2(vehicleInfo.id, vehicleInfo.score, vehicleInfo.isClusterHead, vehicleInfo.clusterHeadId, vehicleInfo.position, vehicleInfo.direction);

		PrintInLog("Receive Control Message: Beacon");

		HandleBeacon(vehicle2);

	}
}

void DCA::HandleBeacon (VehicleDCA vehicle2) {
	NS_LOG_FUNCTION(this);

	NS_LOG_INFO("HANDLING BEACON FROM NODE: " << vehicle2.GetId());

	double_t angleDiff = GetAngleDiff(m_mobilityModel->GetVelocity(), vehicle2.GetDirection());
	double_t separation = CalculateDistance(m_mobilityModel->GetPosition(), vehicle2.GetPosition());

	PrintInLog("Handling Beacon From Node #" + std::to_string(vehicle2.GetId()) + " AngleDiff: " + std::to_string(angleDiff) + " Separation: " + std::to_string(separation));

	bool isNeighbor = false;
	bool isCloseNeighbor = false;
	vector<VehicleDCA>::iterator neighbor;
	vector<VehicleDCA>::iterator closeNeighbor;

	for (neighbor = neighbors.begin() ; neighbor < neighbors.end() ; neighbor++) {
		if ((*neighbor).GetId() == vehicle2.GetId()) {
			isNeighbor = true;
			break;
		}
	}

	for (closeNeighbor = closeNeighbors.begin() ; closeNeighbor != closeNeighbors.end() ; closeNeighbor++) {
		if ((*closeNeighbor).GetId() == vehicle2.GetId()) {
			isCloseNeighbor = true;
			break;
		}
	}

	if (angleDiff == 0 ) {
		NS_LOG_INFO("ANGLEDIFF: " << angleDiff);

		vehicle2.SetTimeExpire(vehicle2.GetTimeExpire() + DCAUtils::EXPIRY_TIME_INTERVAL_1);

		NS_LOG_INFO("SETTING NODE "<< vehicle2.GetId() <<" TIMEEXPIRE TO: " << vehicle2.GetTimeExpire());
		PrintInLog("Setting Node #" + std::to_string(vehicle2.GetId()) + " Time Exipire to " + std::to_string(vehicle2.GetTimeExpire()));

		if (!isNeighbor) {
			NS_LOG_INFO("ADDING NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" NEIGHBORS WITH ANGLEDIFF: " << angleDiff);
			PrintInLog("Adding Node #" + std::to_string(vehicle2.GetId()) + " in Neighbors List");
			neighbors.push_back(vehicle2);
		} else {
			NS_LOG_INFO("UPDATE NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" NEIGHBORS");
			PrintInLog("Update Node #" + std::to_string(vehicle2.GetId()) + " in Neighbors List");

			(*neighbor).SetDirection(vehicle2.GetDirection());
			(*neighbor).SetPosition(vehicle2.GetPosition());
			(*neighbor).SetIsClusterHead(vehicle2.IsClusterHead());
			(*neighbor).SetClusterHeadId(vehicle2.GetClusterHeadId());
			(*neighbor).SetScore(vehicle2.GetScore());
			(*neighbor).SetTimeExpire((*neighbor).GetTimeExpire() + DCAUtils::EXPIRY_TIME_INTERVAL_1);
		}

		if (separation < DCAUtils::MAX_SEPARATION_CLOSENEIGHBORS) {
			if(!isCloseNeighbor) {
				NS_LOG_INFO("CLUSTERING NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" CLOSENEIGHBORS WITH SEPARATION: " << separation);
				PrintInLog("Adding Node #" + std::to_string(vehicle2.GetId()) + " in Close Neighbors List");
				closeNeighbors.push_back(vehicle2);
			} else {
				NS_LOG_INFO("UPDATE NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" CLOSENEIGHBORS");
				PrintInLog("Update Node #" + std::to_string(vehicle2.GetId()) + " in Close Neighbors List");

				(*closeNeighbor).SetDirection(vehicle2.GetDirection());
				(*closeNeighbor).SetPosition(vehicle2.GetPosition());
				(*closeNeighbor).SetIsClusterHead(vehicle2.IsClusterHead());
				(*closeNeighbor).SetClusterHeadId(vehicle2.GetClusterHeadId());
				(*closeNeighbor).SetScore(vehicle2.GetScore());
				(*closeNeighbor).SetTimeExpire((*closeNeighbor).GetTimeExpire() + DCAUtils::EXPIRY_TIME_INTERVAL_1);
			}
		}

	} else if((0 < angleDiff) && (angleDiff <= 5)) {
		NS_LOG_INFO("ANGLEDIFF: " << angleDiff);

		vehicle2.SetTimeExpire(vehicle2.GetTimeExpire() + DCAUtils::EXPIRY_TIME_INTERVAL_2);

		NS_LOG_INFO("SETTING NODE "<< vehicle2.GetId() <<" TIMEEXPIRE TO: " << vehicle2.GetTimeExpire());
		PrintInLog("Setting Node #" + std::to_string(vehicle2.GetId()) + " Time Exipire to " + std::to_string(vehicle2.GetTimeExpire()));

		if (!isNeighbor) {
			NS_LOG_INFO("ADDING NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" NEIGHBORS WITH ANGLEDIFF: " << angleDiff);
			PrintInLog("Adding Node #" + std::to_string(vehicle2.GetId()) + " in Neighbors List");
			neighbors.push_back(vehicle2);
		} else {
			NS_LOG_INFO("UPDATE NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" NEIGHBORS");
			PrintInLog("Update Node #" + std::to_string(vehicle2.GetId()) + " in Neighbors List");

			(*neighbor).SetDirection(vehicle2.GetDirection());
			(*neighbor).SetPosition(vehicle2.GetPosition());
			(*neighbor).SetIsClusterHead(vehicle2.IsClusterHead());
			(*neighbor).SetClusterHeadId(vehicle2.GetClusterHeadId());
			(*neighbor).SetScore(vehicle2.GetScore());
			(*neighbor).SetTimeExpire(neighbor->GetTimeExpire() + DCAUtils::EXPIRY_TIME_INTERVAL_2);
		}

		if (separation < DCAUtils::MAX_SEPARATION_CLOSENEIGHBORS) {
			if(!isCloseNeighbor) {
				NS_LOG_INFO("CLUSTERING NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" CLOSENEIGHBORS WITH SEPARATION: " << separation);
				PrintInLog("Adding Node #" + std::to_string(vehicle2.GetId()) + " in Close Neighbors List");
				closeNeighbors.push_back(vehicle2);
			} else {
				NS_LOG_INFO("UPDATE NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" CLOSENEIGHBORS");
				PrintInLog("Update Node #" + std::to_string(vehicle2.GetId()) + " in Close Neighbors List");
				(*closeNeighbor).SetDirection(vehicle2.GetDirection());
				(*closeNeighbor).SetPosition(vehicle2.GetPosition());
				(*closeNeighbor).SetIsClusterHead(vehicle2.IsClusterHead());
				(*closeNeighbor).SetClusterHeadId(vehicle2.GetClusterHeadId());
				(*closeNeighbor).SetScore(vehicle2.GetScore());
				(*closeNeighbor).SetTimeExpire((*closeNeighbor).GetTimeExpire() + DCAUtils::EXPIRY_TIME_INTERVAL_2);
			}
		}

	} else if ((5 < angleDiff) && (angleDiff <= 30)) {
		NS_LOG_INFO("ANGLEDIFF: " << angleDiff);

		if (!isNeighbor) {
			PrintInLog("Scheduling Checking Node #" + std::to_string(vehicle2.GetId()) + " in " + std::to_string(DCAUtils::CHECK_AGAIN));
			EventId checkThresholdEvent = Simulator::Schedule (Seconds(DCAUtils::CHECK_AGAIN), &DCA::CheckIncreaseByThreshold, this, vehicle2, separation, angleDiff);
			scheduledCheckEvents.push_back(checkThresholdEvent);

		}

	} else if ((30 < angleDiff) && (angleDiff <= 45)) {
		NS_LOG_INFO("ANGLEDIFF: " << angleDiff);

		if(isNeighbor) {
			NS_LOG_INFO("UPDATE NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" NEIGHBORS");
			PrintInLog("Update Node #" + std::to_string(vehicle2.GetId()) + " in Neighbors List");

			(*neighbor).SetDirection(vehicle2.GetDirection());
			(*neighbor).SetPosition(vehicle2.GetPosition());
			(*neighbor).SetIsClusterHead(vehicle2.IsClusterHead());
			(*neighbor).SetClusterHeadId(vehicle2.GetClusterHeadId());
			(*neighbor).SetScore(vehicle2.GetScore());
			(*neighbor).SetTimeExpire((*neighbor).GetTimeExpire() + DCAUtils::EXPIRY_TIME_INTERVAL_4);

			NS_LOG_INFO("SETTING NODE "<< vehicle2.GetId() <<" TIMEEXPIRE TO: " << vehicle2.GetTimeExpire());

			PrintInLog("Setting Node #" + std::to_string(vehicle2.GetId()) + " Time Exipire to " + std::to_string((*neighbor).GetTimeExpire()));

			if(isCloseNeighbor) {
				PrintInLog("Try Purge Node #" + std::to_string(vehicle2.GetId()) + " from Close Neighbors List");
				Purge(vehicle2.GetId(), closeNeighbors);
			}
		}
	} else if (45 < angleDiff) {
		NS_LOG_INFO("ANGLEDIFF: " << angleDiff);

		if (isNeighbor) {
			PrintInLog("Scheduling Checking Node #" + std::to_string(vehicle2.GetId()) + " in " + std::to_string(DCAUtils::CHECK_AGAIN));
			EventId checkThresholdEvent = Simulator::Schedule (Seconds(DCAUtils::CHECK_AGAIN), &DCA::CheckIncreaseByThresholdInAngleAbove45, this, vehicle2, separation, angleDiff);
			scheduledCheckEvents.push_back(checkThresholdEvent);
		}
	}

	if(m_isClusterHead && vehicle2.IsClusterHead()) {
		NS_LOG_INFO("BOTH CLUSTERHEADS "<< vehicle2.GetId() <<" AND " << m_node->GetId() << ".");
		PrintInLog("Both ClusterHeads Node #" + std::to_string(vehicle2.GetId()) + " And Node #" + std::to_string(m_node->GetId()));
		ClusterHeadElection();
	}

	if(m_clusterHeadId == vehicle2.GetId() && !vehicle2.IsClusterHead()) {
		NS_LOG_INFO("VEHICLE "<< vehicle2.GetId() <<" IS " << m_node->GetId() << "'S CLUSTERHEAD, BUT IT IS NOT CLUSTERHEAD.");
		PrintInLog("Node #" + std::to_string(vehicle2.GetId()) + " Is #" + std::to_string(m_node->GetId()) + "'S ClusterHead, but it is not ClusterHead");
		ClusterHeadElection();
	}

	if ((m_isClusterHead) && (vehicle2.GetClusterHeadId() == m_node->GetId())) {
		PrintInLog("Node #" + std::to_string(m_node->GetId()) + " Is #" + std::to_string(vehicle2.GetId()) + "'S ClusterHead");
		bool isIn = false;

		for (VehicleDCA clusterMember : clusterMembers) {
			if(clusterMember.GetId() == vehicle2.GetId()){
				isIn = true;
			}
		}

		if(!isIn) {
			PrintInLog("Adding Node #" + std::to_string(vehicle2.GetId()) + " in Cluster Members List");
			clusterMembers.push_back(vehicle2);
		}
	}

	m_score = closeNeighbors.size();
}

void DCA::ClusterHeadElection () {

	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("ELECTING CLUSTERHEAD!");

	if (!m_isClusterHead) {

		uint32_t highestScore = 0;
		bool hasCandidate = false;

		VehicleDCA candidate;

		for(VehicleDCA neighbor : neighbors) {
			if(highestScore < neighbor.GetScore()) {
				highestScore = neighbor.GetScore();
				candidate = neighbor;
				hasCandidate = true;
			} else if(highestScore == neighbor.GetScore()) {
				if(hasCandidate && candidate.GetId() < neighbor.GetId()) {
					candidate = neighbor;
					hasCandidate = true;
				}
			}
		}

		if(hasCandidate) {
			if(candidate.IsClusterHead()) {
				m_isClusterHead = false;
				m_clusterHeadId = candidate.GetId();
				NS_LOG_INFO("VEHICLE " << candidate.GetId() << " IS NOW VEHICLE " << m_node->GetId() << "'S CLUSTERHEAD.");
				PrintInLog("Grouping with Node #" + std::to_string(candidate.GetId()));
			} else {
				if (candidate.GetScore() < m_score) {
					m_isClusterHead = true;
					m_clusterHeadId = m_node->GetId();

					NS_LOG_INFO("VEHICLE " << m_node->GetId() << " IS NOW CLUSTERHEAD.");
				} else if(candidate.GetScore() == m_score) {
					if (candidate.GetId() < m_node->GetId()) {
						m_isClusterHead = true;
						m_clusterHeadId = m_node->GetId();
						NS_LOG_INFO("VEHICLE " << m_node->GetId() << " IS NOW CLUSTERHEAD.");
					}
				}
			}
		}

	} else {

		for (VehicleDCA neighbor : neighbors) {
			if (neighbor.IsClusterHead()) {
				EventId checkVerifyClusterHeadEvent = Simulator::Schedule (Seconds(DCAUtils::CHECK_AGAIN), &DCA::VerifyClusterHead, this, neighbor.GetId());
				scheduledCheckEvents.push_back(checkVerifyClusterHeadEvent);
			} else if (neighbor.GetClusterHeadId() == m_node->GetId() && m_score <= neighbor.GetScore()) {
				EventId checkHigherScoreEvent = Simulator::Schedule(Seconds(DCAUtils::CHECK_AGAIN), &DCA::HasHigherScore, this, neighbor.GetId());
				scheduledCheckEvents.push_back(checkHigherScoreEvent);
			}
		}
	}
}

void DCA::Purge (uint32_t vehicleId, vector<VehicleDCA>& vehicleVector) {
	NS_LOG_FUNCTION(this);

	for(vector<VehicleDCA>::iterator vehicle = vehicleVector.begin() ; vehicle < vehicleVector.end() ; vehicle++) {
		if ((*vehicle).GetId() == vehicleId) {
			vehicle = vehicleVector.erase(vehicle);
			PrintInLog("Purge Node #" + std::to_string(vehicleId) + " from List");
		}
	}
}

void DCA::CleanUp () {
	NS_LOG_FUNCTION(this);

	for(vector<VehicleDCA>::iterator vehicle = neighbors.begin() ; vehicle < neighbors.end() ; vehicle++) {
		if ((*vehicle).GetTimeExpire() <= Simulator::Now().GetSeconds()) {
			NS_LOG_INFO("CLEANING UP VEHICLE: " << (*vehicle).GetId() << " FROM VEHICLE "<< m_node->GetId() <<  " NEIGHBORS");
			vehicle = neighbors.erase(vehicle);
		}
	}

	for(vector<VehicleDCA>::iterator vehicle = closeNeighbors.begin() ; vehicle < closeNeighbors.end() ; vehicle++) {
		if ((*vehicle).GetTimeExpire() <= Simulator::Now().GetSeconds()) {
			NS_LOG_INFO("CLEANING UP VEHICLE: " << (*vehicle).GetId() << " FROM VEHICLE "<< m_node->GetId() <<  " CLOSENEIGHBORS");
			vehicle = closeNeighbors.erase(vehicle);
		}
	}

	for(vector<VehicleDCA>::iterator vehicle = clusterMembers.begin() ; vehicle < clusterMembers.end() ; vehicle++) {
		if ((*vehicle).GetTimeExpire() <= Simulator::Now().GetSeconds()) {
			vehicle = clusterMembers.erase(vehicle);
		}
	}

	m_cleanEvent = Simulator::Schedule (Seconds(DCAUtils::CLEAN_UP_TIMER), &DCA::CleanUp, this);
}

void DCA::VerifyClusterHead (uint32_t vehicleId) {
	NS_LOG_FUNCTION(this);

	for (VehicleDCA neighbor : neighbors) {
		if(neighbor.GetId() == vehicleId) {
			if (neighbor.IsClusterHead()) {
				for (VehicleDCA closeNeighbor : closeNeighbors) {
					if (closeNeighbor.GetId() == vehicleId) {
						if(m_score < closeNeighbor.GetScore()) {
							m_isClusterHead = false;
							m_clusterHeadId = closeNeighbor.GetId();
							PrintInLog("Resigned CH");
							PrintInLog("Grouping with Node #" + std::to_string(closeNeighbor.GetId()));
						} else if(m_score == closeNeighbor.GetScore()) {
							if(m_node->GetId() < closeNeighbor.GetId()) {
								m_isClusterHead = false;
								m_clusterHeadId = closeNeighbor.GetId();
								PrintInLog("Resigned CH");
								PrintInLog("Grouping with Node #" + std::to_string(closeNeighbor.GetId()));
							}
						}
					}
				}
			}
			break;
		}
	}
}

void DCA::HasHigherScore (uint32_t vehicleId) {
	NS_LOG_FUNCTION(this);

	for (VehicleDCA neighbor : neighbors) {
		if(neighbor.GetId() == vehicleId) {
			if (neighbor.GetClusterHeadId() == m_node->GetId() && m_score <= neighbor.GetScore()) {
				if(m_score < neighbor.GetScore()) {
					m_isClusterHead = false;
					m_clusterHeadId = neighbor.GetId();
					PrintInLog("Grouping with Node #" + std::to_string(neighbor.GetId()));
				} else if(m_score == neighbor.GetScore()) {
					if(m_node->GetId() < neighbor.GetId()) {
						m_isClusterHead = false;
						m_clusterHeadId = neighbor.GetId();
						PrintInLog("Grouping with Node #" + std::to_string(neighbor.GetId()));
					}
				}
			}
		}
	}
}

void DCA::CheckIncreaseByThreshold (VehicleDCA vehicle2, double_t separation, double_t angleDiff) {
	NS_LOG_FUNCTION(this);

	double_t separationThreshold = separation + (separation * DCAUtils::THRESHOLD);
	double_t angleDiffThreshold = angleDiff + (angleDiff * DCAUtils::THRESHOLD);

	double_t newSeparation = CalculateDistance(m_mobilityModel->GetPosition(), vehicle2.GetPosition());
	double_t newAngleDiff = GetAngleDiff(m_mobilityModel->GetVelocity(), vehicle2.GetDirection());

	vehicle2.SetTimeExpire(Simulator::Now().GetSeconds() + DCAUtils::EXPIRY_TIME_INTERVAL_3);

	NS_LOG_INFO("SETTING NODE "<< vehicle2.GetId() <<" TIMEEXPIRE TO: " << vehicle2.GetTimeExpire());

	if ((newSeparation <= separationThreshold) && (newAngleDiff <= angleDiffThreshold)) {
		neighbors.push_back(vehicle2);

		if (separation < DCAUtils::MAX_SEPARATION_CLOSENEIGHBORS) {
			NS_LOG_INFO("CLUSTERING NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" CLOSENEIGHBORS WITH SEPARATION: " << separation);
			closeNeighbors.push_back(vehicle2);
		}
	}
}

void DCA::CheckIncreaseByThresholdInAngleAbove45(VehicleDCA vehicle2, double_t separation, double_t angleDiff) {
	NS_LOG_FUNCTION(this);

	double_t separationThreshold = separation + (separation * DCAUtils::THRESHOLD);
	double_t angleDiffThreshold = angleDiff + (angleDiff * DCAUtils::THRESHOLD);

	double_t newSeparation = CalculateDistance(m_mobilityModel->GetPosition(), vehicle2.GetPosition());
	double_t newAngleDiff = GetAngleDiff(m_mobilityModel->GetVelocity(), vehicle2.GetDirection());

	if((newSeparation <= separationThreshold) && (newAngleDiff <= angleDiffThreshold)) {

		for(vector<VehicleDCA>::iterator neighbor = neighbors.begin() ; neighbor < neighbors.end() ; neighbor++) {
			if((*neighbor).GetId() == vehicle2.GetId())
				(*neighbor).SetTimeExpire((*neighbor).GetTimeExpire() + DCAUtils::EXPIRY_TIME_INTERVAL_5);
		}

	} else {

		PrintInLog("Taking Node: " + std::to_string(vehicle2.GetId()) + " out of Cluster");
		Purge(vehicle2.GetId(), neighbors);

	}

	for(auto vehicle : closeNeighbors) {
		if(vehicle2.GetId() == vehicle.GetId())
			Purge(vehicle2.GetId(), closeNeighbors);
	}

	NS_LOG_INFO("SETTING NODE "<< vehicle2.GetId() <<" TIMEEXPIRE TO: " << vehicle2.GetTimeExpire());
}

void DCA::SendBeacon () {
	NS_LOG_FUNCTION(this);
	PrintInLog("Sending Beacon Message");

	BeaconHeader2::VehicleInfo info;
	info.id = m_node->GetId();
	info.score = m_score;
	info.isClusterHead = m_isClusterHead;
	info.clusterHeadId = m_clusterHeadId;
	info.position = m_mobilityModel->GetPosition();
	info.direction = m_mobilityModel->GetVelocity();

	BeaconHeader2 header;
	header.SetSeq(m_sentCounter++);
	header.SetVehicleInfo(info);

	SendMessage(header);

	m_sendEvent = Simulator::Schedule (Seconds(DCAUtils::BEACON_MESSAGE_TIMER), &DCA::SendBeacon, this);

	NS_LOG_INFO("SENDING THIS INFO: ID: " << info.id << " SCORE: " << info.score << " POSITION: " << info.position << " DIRECTION: " << info.direction << " |VEL|: " << info.direction.GetLength());
}

void DCA::SendMessage (BeaconHeader2 header) {
	NS_LOG_FUNCTION(this);
	NS_LOG_DEBUG("NODE: "<< m_node->GetId() <<" SENDING BEACON MESSAGE IN BROADCAST!");

	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(header);

	m_communicationLayer->SendControlMenssage(packet);
}

double_t DCA::GetAngleDiff (Vector v1, Vector v2) {
	NS_LOG_FUNCTION(this);

	double_t diff = 0.0;

	double l1 = v1.GetLength();
	double l2 = v2.GetLength();

	if ((l1 != 0) && (l2 != 0)) {
		diff = acos(((v1.x*v2.x) + (v1.y*v2.y)) / (l1*l2));
	}

	return (diff * 180) / PI;   //Convert from Radius to Degrees
}

bool DCA::IsClusterHead () {
	NS_LOG_FUNCTION(this);

	return m_isClusterHead && (m_clusterHeadId == m_node->GetId());
}

bool DCA::IsClusterMember () {
	NS_LOG_FUNCTION(this);

	return !m_isClusterHead && (m_clusterHeadId != m_node->GetId());
}

bool DCA::IsIsolated () {
	NS_LOG_FUNCTION(this);

	return neighbors.empty();
}

uint32_t DCA::GetClusterId() {
	NS_LOG_FUNCTION(this);
	if(IsClusterHead() || IsClusterMember()) {
		return m_clusterHeadId;
	} else {
		cerr << "## ERROR: In GetClusterId method node is not Cluster Head or Member Cluster" << endl;
		exit(1);
	}
}

bool DCA::IsStarted() {
	return m_clusteringStarted;
}

void DCA::EventsListMaintenance () {
	NS_LOG_FUNCTION(this);

	for(vector<EventId>::iterator eventIdIt =  scheduledCheckEvents.begin() ; eventIdIt < scheduledCheckEvents.end() ; eventIdIt++) {
		if((*eventIdIt).IsExpired()) {
			eventIdIt = scheduledCheckEvents.erase(eventIdIt);
		}
	}

	m_maintenanceEvent = Simulator::Schedule (Seconds(DCAUtils::EVENTS_LIST_MAINTENANCE_TIMER), &DCA::EventsListMaintenance, this);
}

void DCA::PrintInLog(string message) {
	ofstream os;
	os.open (MinuetConfig::LOG_FILE_CLUSTERING_ALGORITHM.c_str(), ofstream::out | ofstream::app);
	os << Simulator::Now().GetNanoSeconds() << "ns - DCA - Node #" << m_node->GetId() << " Pos (" << m_mobilityModel->GetPosition()
			<<  ") GroupId #" << m_clusterHeadId << " : " << message << endl;
	os.close();
	return;
}

}
