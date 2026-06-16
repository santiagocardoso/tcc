#include "pctt.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PCTT");

/*************** ClusteringManagerInterface ************************/
NS_OBJECT_ENSURE_REGISTERED (PCTTInterface);

PCTTInterface::PCTTInterface() {
	NS_LOG_FUNCTION(this);
}

PCTTInterface::~PCTTInterface() {
	NS_LOG_FUNCTION(this);
}

void PCTTInterface::AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer) {
	PCTTInterface::m_communicationLayer = comunicationLayer;
	PCTTInterface::m_communicationLayer->AttachPCTTInterface(this);
}
/*************** END ClusteringManagerInterface ************************/


/*************** PCTT ***************/
NS_OBJECT_ENSURE_REGISTERED(PCTT);

TypeId PCTT::GetTypeId() {
	static TypeId typeId =
			TypeId("ns3::PCTT")
			.AddConstructor<PCTT>()
			.SetParent<Object>()
			.AddAttribute("Node",
					"The Node of the PCTT", PointerValue(),
					MakePointerAccessor(&PCTT::m_node),
					MakePointerChecker<Node>())
			.AddAttribute("CommunicationLayer",
					"The Communication Instance", PointerValue(),
					MakePointerAccessor(&PCTT::m_communicationLayer),
					MakePointerChecker<CommunicationLayer>());

	return typeId;
}

PCTT::PCTT () {
	NS_LOG_FUNCTION(this);
	m_sentCounter = 0;
}

PCTT::~PCTT() {
	NS_LOG_FUNCTION(this);
}

void PCTT::StartClustering(Ptr<EventDetected> event) {
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("RUNNING STARTCLUSTERING!");

	m_outOfSim = false;

	AddCommunicationLayer(m_communicationLayer);

	m_targetPosition = event->GetMobilityModel()->GetPosition();
	m_targetVelocity = event->GetMobilityModel()->GetVelocity();

	m_eventId = event->GetEventId();

	m_mobilityModel = m_node->GetObject<MobilityModel>();

	m_clusterHead = CreateObjectWithAttributes<PCTTClusterHead>("Id", IntegerValue(m_node->GetId()), "CHID", IntegerValue(m_node->GetId()),
		"TargetPosition", VectorValue(m_targetPosition), "TargetVelocity", VectorValue(m_targetVelocity),
		"MobilityModel", PointerValue(m_mobilityModel));

	m_isClusterHead = true;
	m_isClusterMember = false;
	m_isNonMember = false;

	Time myOBT = m_clusterHead->CalculateOBT(m_targetPosition, m_targetVelocity, m_mobilityModel->GetPosition(), m_mobilityModel->GetVelocity());

	/// REMOVE Only Validation
	double_t distance = CalculateDistance(m_targetPosition, m_mobilityModel->GetPosition());
	if((distance < 10) && (myOBT.GetNanoSeconds() == 0)) {
		cout << "OBT = 0 but in a short distance" << endl;
		exit(EXIT_FAILURE);
	}
	////////////////////

	if(myOBT.Compare(Seconds(0)) == 1) {

		PrintInLog("Node became CH");

		m_convergenceOverEvent = Simulator::Schedule(Seconds(PCTTUtils::CONVERGENCE_TIME), &PCTT::ConvergenceOver, this);
		m_scheduledEvents.push_back(m_convergenceOverEvent);

		SendCHMessageBeforeConvergence();

		m_clusteringStarted = true;

		m_maintenanceEvent = Simulator::Schedule (Seconds(m_node->GetId() * 0.001), &PCTT::EventsListMaintenance, this);

	} else {

		m_clusterHead->Dispose();
		m_clusterHead = nullptr;
		m_sendCHMessageBeforeConvergenceEvent.Cancel();
		m_convergenceOverEvent.Cancel();

		StopClustering();
		m_clusteringStarted = false;

		PrintInLog("Started as NM");
	}
}

void PCTT::StopClustering () {
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("RUNNING STOPCLUSTERING!");

	//Flag declarando-se fora da simulação
	m_outOfSim = true;

	//Cancelando eventos ativos
	m_periodicallySendsCHMEvent.Cancel();
	m_checkAnsweredEvent.Cancel();
	m_checkOBTGreaterEvent.Cancel();
	m_calculateAndCheckPredictionEvent.Cancel();
	m_nmProcedureEvent.Cancel();
	m_convergenceOverEvent.Cancel();
	m_timeoutIntervalOverEvent.Cancel();
	m_maintenanceEvent.Cancel();
	m_changeCHToCMEvent.Cancel();

	if (m_scheduledEvents.size() != 0) {
		for (EventId eventId : m_scheduledEvents) {
			eventId.Cancel();
		}
		m_scheduledEvents.clear();
	}

	m_clusteringStarted = false;

	PrintInLog("PCTT Algorithm Stopped");
}

void PCTT::ReceiveCHMessage (Ptr<Packet> packet, Address addr) {
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("CH MESSAGE RECEIVED!");

	if (!m_outOfSim) {

		CHMHeader chmHeader;
		CHMHeader::VehicleInfo vehicleInfo;

		packet->RemoveHeader(chmHeader);
		vehicleInfo = chmHeader.GetVehicleInfo();

		if (m_isClusterHead) {
			CHReceivedCHM(chmHeader, vehicleInfo);
		} else if (m_isClusterMember) {
			CMReceivedCHM(chmHeader, vehicleInfo);
		}
	}
}

void PCTT::ReceiveJAckMessage (Ptr<Packet> packet, Address addr) {
	NS_LOG_FUNCTION(this);

	if (!m_outOfSim) {

			JackMHeader jackmHeader;
			JackMHeader::VehicleInfo vehicleInfo;

			packet->RemoveHeader(jackmHeader);
			vehicleInfo = jackmHeader.GetVehicleInfo();

			if(m_isClusterMember) {
				CMReceivedJack(jackmHeader, vehicleInfo);
			} else if (m_isNonMember) {
				NMReceivedJack(jackmHeader, vehicleInfo);
			} else if(m_isClusterHead) {
				CHReceivedJack(jackmHeader, vehicleInfo);
			}
		}
}

void PCTT::ReceiveCMMessage (Ptr<Packet> packet, Address addr) {
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("CM MESSAGE RECEIVED!");

	if (!m_outOfSim) {

		CMMHeader cmmHeader;
		CMMHeader::VehicleInfo vehicleInfo;

		packet->RemoveHeader(cmmHeader);
		vehicleInfo = cmmHeader.GetVehicleInfo();

		if (m_isClusterHead) {
			CHReceivedCMM(cmmHeader, vehicleInfo);
		} else if (m_isClusterMember) {
			CMReceivedCMM(cmmHeader, vehicleInfo);
		}
	}
}

void PCTT::CHReceivedCMM(CMMHeader cmmHeader, CMMHeader::VehicleInfo cmmHeaderInfo) {
	NS_LOG_FUNCTION(this);

	if (!HasReceivedMsg(cmmHeaderInfo.nodeId, cmmHeaderInfo.packetId)) {

		vector<int32_t> v{cmmHeaderInfo.nodeId, cmmHeaderInfo.packetId};
		m_messagesReceived.push_back(v);

//		PrintInLog("Received CMM from: " + std::to_string(vehicle2.GetId()));

		if (cmmHeaderInfo.predictionDenial) {

			PrintInLog("Received a Prediction Denial");
			m_clusterHead->UpdateNodePosition(cmmHeaderInfo.nodeId, cmmHeaderInfo.position, cmmHeaderInfo.velocity);
			CheckOBTGreater();

		} else {

			PCTTClusterHead::CM cm;
			cm.id = cmmHeaderInfo.nodeId;
			cm.chid = cmmHeaderInfo.chid;
			cm.lastPosition = cmmHeaderInfo.position;
			cm.lastVelocity = cmmHeaderInfo.velocity;
			cm.obt = cmmHeaderInfo.observationTime;

			if (m_clusterHead->IsEqualTargetDirection(m_targetVelocity, m_targetPosition, cmmHeaderInfo.velocity, cmmHeaderInfo.position)) {

				m_clusterHead->AddToMemberList(cm);
				PrintInLog("Adding Or Updating Node #" + std::to_string(cm.id) + " in Members List");
				SendJAckCHMessage(cmmHeaderInfo.nodeId);
				CheckOBTGreater();

			} else {
				if (m_clusterHead->GetMemberList().empty()) {
					m_clusterHead->AddToMemberList(cm);
					PrintInLog("Adding Or Updating Node #" + std::to_string(cm.id) + " in Members List");
					SendJAckCHMessage(cmmHeaderInfo.nodeId);
					CheckOBTGreater();
				}
			}
		}
	}
}

void PCTT::CHReceivedCHM(CHMHeader chmHeader, CHMHeader::VehicleInfo chmHeaderInfo) {
	NS_LOG_FUNCTION(this);

	if (!HasReceivedMsg(chmHeaderInfo.nodeId, chmHeaderInfo.packetId)) {

		vector<int32_t> v{chmHeaderInfo.nodeId, chmHeaderInfo.packetId};
		m_messagesReceived.push_back(v);

		PCTTClusterHead::CM cm;
		cm.id = chmHeaderInfo.nodeId;
		cm.chid = -1;
		cm.lastPosition = chmHeaderInfo.position;
		cm.lastVelocity = chmHeaderInfo.velocity;
		cm.obt = chmHeaderInfo.observationTime;

		if (m_convergenceOverEvent.IsExpired()) {

			//CLUSTERHEAD RECEBEU CHM DE OUTRO CLUSTERHEAD

			if (m_clusterHead->IsEqualTargetDirection(m_targetVelocity, m_targetPosition, chmHeaderInfo.velocity, chmHeaderInfo.position)) {

					m_clusterHead->AddToMemberList(cm);
					PrintInLog("Adding Or Updating Node #" + std::to_string(cm.id) + " in Members List");
					SendJAckCHMessage(chmHeaderInfo.nodeId);
					CheckOBTGreater();

				} else {
					if (m_clusterHead->GetMemberList().empty()) {
						m_clusterHead->AddToMemberList(cm);
						PrintInLog("Adding Or Updating Node #" + std::to_string(cm.id) + " in Members List");
						SendJAckCHMessage(chmHeaderInfo.nodeId);
						CheckOBTGreater();
					}
				}

		} else {
//			PrintInLog("Received CHM from: " + std::to_string(vehicle2.GetId()));

			m_clusterHead->AddToMemberList(cm);
			PrintInLog("Adding Or Updating Node #" + std::to_string(cm.id) + " in Members List");

			if (chmHeaderInfo.hopCount < PCTTUtils::MAX_HOPS) {
				ForwardCHMessage(chmHeader, chmHeaderInfo);
			}
		}
	}
}

void PCTT::CMReceivedCMM(CMMHeader cmmHeader, CMMHeader::VehicleInfo cmmHeaderInfo) {
	NS_LOG_FUNCTION(this);

	if (!HasReceivedMsg(cmmHeaderInfo.nodeId, cmmHeaderInfo.packetId)) {

		vector<int32_t> v{cmmHeaderInfo.nodeId, cmmHeaderInfo.packetId};
		m_messagesReceived.push_back(v);

//		PrintInLog("Received CMM from: " + std::to_string(vehicle2.GetId()));

		if (cmmHeaderInfo.chid == m_clusterMember->GetCHID()) {
			if (cmmHeaderInfo.hopCount < PCTTUtils::MAX_HOPS) {
				ForwardCMMessage(cmmHeader, cmmHeaderInfo);
			}
		}
	}
}

void PCTT::CMReceivedCHM(CHMHeader chmHeader, CHMHeader::VehicleInfo chmHeaderInfo) {
	NS_LOG_FUNCTION(this);

	if (!HasReceivedMsg(chmHeaderInfo.nodeId, chmHeaderInfo.packetId)) {

		vector<int32_t> v{chmHeaderInfo.nodeId, chmHeaderInfo.packetId};
		m_messagesReceived.push_back(v);

//		PrintInLog("Received CHM from: " + std::to_string(vehicle2.GetId()));

		if (m_clusterMember->GetCHID() == chmHeaderInfo.nodeId) {

			m_timeoutIntervalOverEvent.Cancel();
			m_timeoutIntervalOverEvent = Simulator::Schedule(Seconds(PCTTUtils::RESET_TIME_INTERVAL + 0.2), &PCTT::TimeoutIntervalOver, this);
			m_scheduledEvents.push_back(m_timeoutIntervalOverEvent);

			if(m_clusterMember->GetTDV(m_targetPosition, m_targetVelocity)) { // CM can detect target

				if (chmHeaderInfo.reset) {
//					PrintInLog("Sending response to a Reset CHM from: " + std::to_string(vehicle2.GetId()));
					SendCMMessage(false);

				}
			}

			if(chmHeaderInfo.resignValue) {
				if(chmHeaderInfo.candidateClusterHeadId == m_clusterMember->GetId()) {
					// Change to CH in Resign Time
					m_changeCMToCHEvent = Simulator::Schedule(chmHeaderInfo.resignTime, &PCTT::ChangeCMToCH, this);
					m_scheduledEvents.push_back(m_changeCMToCHEvent);
				} else {
					// Change your CHID in Resign Time
					m_changeCHInCMEvent = Simulator::Schedule(chmHeaderInfo.resignTime, &PCTT::ChangeCHInCM, this, chmHeaderInfo.candidateClusterHeadId);
					m_scheduledEvents.push_back(m_changeCHInCMEvent);
				}
			}

			if(chmHeaderInfo.hopCount < PCTTUtils::MAX_HOPS) {
				ForwardCHMessage(chmHeader, chmHeaderInfo);
			}
		}
	}
}

void PCTT::ChangeCMToCH() {
	NS_LOG_FUNCTION(this);

	if(m_isClusterMember) {

		m_clusterHead = m_clusterMember->ToClusterHead();
		m_clusterMember->Dispose ();
		m_clusterMember = nullptr;

		m_isClusterHead = true;
		m_isClusterMember = false;
		m_isNonMember = false;

		PrintInLog("Became CH");

		CHProcedure();
	}
}

void PCTT::ChangeCHInCM(int32_t chid) {
	if(m_isClusterMember) {
		m_clusterMember->SetCHID(chid);
	}
}

void PCTT::CMReceivedJack(JackMHeader jackmHeader, JackMHeader::VehicleInfo jackmHeaderInfo) {
	NS_LOG_FUNCTION(this);

	if (!HasReceivedMsg(jackmHeaderInfo.nodeId, jackmHeaderInfo.packetId)) {

		vector<int32_t> v{jackmHeaderInfo.nodeId, jackmHeaderInfo.packetId};
		m_messagesReceived.push_back(v);

		if (jackmHeaderInfo.hopCount < PCTTUtils::MAX_HOPS) {
			ForwardJAckMessage(jackmHeader, jackmHeaderInfo);
		}
	}

}

void PCTT::NMReceivedJack(JackMHeader jackmHeader, JackMHeader::VehicleInfo jackmHeaderInfo) {
	NS_LOG_FUNCTION(this);

	if(!HasReceivedMsg(jackmHeaderInfo.nodeId, jackmHeaderInfo.packetId)) {

		vector<int32_t> v{jackmHeaderInfo.nodeId, jackmHeaderInfo.packetId};
		m_messagesReceived.push_back(v);

		if (jackmHeaderInfo.nonMemberId == (int32_t) m_node->GetId()) {

//			PrintInLog("Received JAckCHM from: " + std::to_string(m_jackMHeaderVehicleInfo.nodeId));

			m_clusterMember = m_nonMember->ToClusterMember(jackmHeaderInfo.nodeId);
			m_nonMember->Dispose();
			m_nonMember = nullptr;

			m_isClusterHead = false;
			m_isClusterMember = true;
			m_isNonMember = false;

			PrintInLog("Grouping with Node #" + std::to_string(m_clusterMember->GetCHID()));

			m_timeoutIntervalOverEvent = Simulator::Schedule(Seconds(PCTTUtils::RESET_TIME_INTERVAL + 0.2), &PCTT::TimeoutIntervalOver, this);
			m_scheduledEvents.push_back(m_timeoutIntervalOverEvent);

			PrintInLog("Became CM");

			CMProcedure();
		}
	}
}

void PCTT::CHReceivedJack(JackMHeader jackmHeader, JackMHeader::VehicleInfo jackmHeaderInfo) {
	NS_LOG_FUNCTION(this);

	if(!HasReceivedMsg(jackmHeaderInfo.nodeId, jackmHeaderInfo.packetId)) {

		vector<int32_t> v{jackmHeaderInfo.nodeId, jackmHeaderInfo.packetId};
		m_messagesReceived.push_back(v);

		if (jackmHeaderInfo.nonMemberId == (int32_t) m_node->GetId()) {

			if(!m_convergenceOverEvent.IsExpired())
				m_convergenceOverEvent.Cancel();

//			PrintInLog("Received JAckCHM from: " + std::to_string(m_jackMHeaderVehicleInfo.nodeId));

			m_clusterHead->SetCCHID(jackmHeaderInfo.nodeId);
			m_clusterMember = m_clusterHead->ToClusterMember();
			m_clusterHead->Dispose();
			m_clusterHead = nullptr;
			m_sendCHMessageBeforeConvergenceEvent.Cancel();
			m_convergenceOverEvent.Cancel();

			m_isClusterHead = false;
			m_isClusterMember = true;
			m_isNonMember = false;

			PrintInLog("Grouping with Node #" + std::to_string(m_clusterMember->GetCHID()));

			m_timeoutIntervalOverEvent = Simulator::Schedule(Seconds(PCTTUtils::RESET_TIME_INTERVAL + 0.2), &PCTT::TimeoutIntervalOver, this);
			m_scheduledEvents.push_back(m_timeoutIntervalOverEvent);

			PrintInLog("Became CM");

			CMProcedure();
		}
	}
}

void PCTT::SendCHMessage(bool resetMessage, bool resignValue = false, Time resignTime = Seconds(0)) {
	NS_LOG_FUNCTION(this);

	CHMHeader::VehicleInfo info;
	info.nodeId = m_node->GetId();
	info.position = m_mobilityModel->GetPosition();
	info.velocity = m_mobilityModel->GetVelocity();
	info.currentTime = Simulator::Now();
	info.observationTime = m_clusterHead->CalculateOBT(m_targetPosition, m_targetVelocity, m_mobilityModel->GetPosition(), m_mobilityModel->GetVelocity());
	info.candidateClusterHeadId = m_clusterHead->GetCCHID();
	info.resignTime = resignTime;
	info.resignValue = resignValue;
	info.reset = resetMessage;

	info.hopCount = 1;

	CHMHeader header;
	header.SetSeq(m_sentCounter++);

	info.packetId = header.GetSeq();

	header.SetVehicleInfo(info);

	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(header);

	NS_LOG_DEBUG("NodeId: " << info.nodeId
			   << " PacketId: " << info.packetId
			   << " HopCount: " << info.hopCount
			   << " Position: " << info.position
			   << " Velocity: " << info.velocity
			   << " CurrentTime: " << info.currentTime
			   << " ObservationTime: " << info.observationTime
			   << " CandidateClusterHeadId: " << info.candidateClusterHeadId
			   << " ResignTime: " << info.resignTime.GetNanoSeconds()
			   << " ResignValue: " << info.resignValue
			   << " Reset: " << info.reset);

	PrintInLog("Sending CHM - PacketId: " + std::to_string(info.packetId)
			   + " CandidateClusterHeadId: " + std::to_string(info.candidateClusterHeadId)
			   + " Reset: " + std::to_string(info.reset)
			   + " ResignValue: " + std::to_string(info.resignValue)
			   + " ResignTime: " + std::to_string(info.resignTime.GetNanoSeconds())
			   + " HopCount: " + std::to_string(info.hopCount)
			   + " Position: (" + std::to_string(info.position.x) + ":" + std::to_string(info.position.y) + ":" + std::to_string(info.position.z) + ")"
			   + " Velocity: (" + std::to_string(info.velocity.x) + ":" + std::to_string(info.velocity.y) + ":" + std::to_string(info.velocity.z) + ")"
			   + " CurrentTime: " + std::to_string(info.currentTime.GetNanoSeconds()) + "ns"
			   + " ObservationTime: " + std::to_string(info.observationTime.GetNanoSeconds()) + "ns");

	m_communicationLayer->SendControlMenssage(packet);

	vector<int32_t> v{info.nodeId, info.packetId};
	m_messagesReceived.push_back(v);
}

void PCTT::SendJAckCHMessage (uint32_t nonMemberId) {
	NS_LOG_FUNCTION(this);

	JackMHeader::VehicleInfo info;
	info.nodeId = m_node->GetId();
	info.nonMemberId = nonMemberId;

	info.hopCount = 1;

	JackMHeader header;
	header.SetSeq(m_sentCounter++);

	info.packetId = header.GetSeq();

	header.SetVehicleInfo(info);

	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(header);

	NS_LOG_DEBUG("NodeId: " << info.nodeId
				   << " NonMemberId: " << info.nonMemberId
				   << " PacketId: " << info.packetId
				   << " HopCount: " << info.hopCount);

	PrintInLog("Sending JAckCHM - NonMemberId: " + std::to_string(info.nonMemberId)
				   + " PacketId: " + std::to_string(info.packetId)
				   + " HopCount: " + std::to_string(info.hopCount));

	m_communicationLayer->SendControlMenssage(packet);

	vector<int32_t> v{info.nodeId, info.packetId};
	m_messagesReceived.push_back(v);
}

void PCTT::SendCMMessage(bool predictionDenial) {
	NS_LOG_FUNCTION(this);

	CMMHeader::VehicleInfo info;

	if (m_isNonMember) {
		info.nodeId = m_node->GetId();
		info.chid = m_nonMember->GetCHID();
		info.targetDetectionValue = true;
		info.currentTime = Simulator::Now();
		info.position =  m_mobilityModel->GetPosition();
		info.velocity = m_mobilityModel->GetVelocity();
		info.observationTime = m_nonMember->CalculateOBT(m_targetPosition, m_targetVelocity, m_mobilityModel->GetPosition(), m_mobilityModel->GetVelocity());
		info.predictionDenial = predictionDenial;
	} else {
		info.nodeId = m_node->GetId();
		info.chid = m_clusterMember->GetCHID();
		info.targetDetectionValue = m_clusterMember->GetTDV(m_targetPosition, m_targetVelocity);
		info.currentTime = Simulator::Now();
		info.position = m_clusterMember->GetLastPosition();
		info.velocity = m_clusterMember->GetLastVelocity();
		info.observationTime =  m_clusterMember->CalculateOBT(m_targetPosition, m_targetVelocity, m_clusterMember->GetLastPosition(), m_clusterMember->GetLastVelocity());
		info.predictionDenial = predictionDenial;
	}

	info.hopCount = 1;

	CMMHeader header;
	header.SetSeq(m_sentCounter++);

	info.packetId = header.GetSeq();

	header.SetVehicleInfo(info);

	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(header);

	NS_LOG_DEBUG("NodeId: " << info.nodeId
			   << " CHID: " << info.chid
			   << " PacketId: " << info.packetId
			   << " HopCount: " << info.hopCount
			   << " Position: " << info.position
			   << " Velocity: " << info.velocity
			   << " CurrentTime: " << info.currentTime
			   << " ObservationTime: " << info.observationTime
			   << " TDV: " << info.targetDetectionValue
			   << " PredictionDenial: " << info.predictionDenial);

	PrintInLog("Sending CMM - CHID: " + std::to_string(info.chid)
			   + " PacketId: " + std::to_string(info.packetId)
			   + " TDV: " + std::to_string(info.targetDetectionValue)
			   + " HopCount: " + std::to_string(info.hopCount)
			   + " Position: (" + std::to_string(info.position.x) + ":" + std::to_string(info.position.y) + ":" + std::to_string(info.position.z) + ")"
			   + " Velocity: (" + std::to_string(info.velocity.x) + ":" + std::to_string(info.velocity.y) + ":" + std::to_string(info.velocity.z) + ")"
			   + " CurrentTime: " + std::to_string(info.currentTime.GetNanoSeconds()) + "ns"
			   + " ObservationTime: " + std::to_string(info.observationTime.GetNanoSeconds()) + "ns"
			   + " PredictionDenial: " + std::to_string(info.predictionDenial));

	m_communicationLayer->SendControlMenssage(packet);

	vector<int32_t> v{info.nodeId, info.packetId};
	m_messagesReceived.push_back(v);
}

void PCTT::CHProcedure() {
	NS_LOG_FUNCTION(this);

	PeriodicallySendsCHM();

	m_calculatePredictionOBTInMLEvent = Simulator::Schedule(Seconds(PCTTUtils::PREDICTION_TIME_INTERVAL), &PCTT::CalculatePredictionOBTInML, this);
	m_scheduledEvents.push_back(m_calculatePredictionOBTInMLEvent);
}

void PCTT::PeriodicallySendsCHM() {
	NS_LOG_FUNCTION(this);

	if(m_isClusterHead) {

		if (m_clusterHead->GetTDV(m_targetPosition, m_targetVelocity)) {
			SendCHMessage(true);

			m_checkAnsweredEvent = Simulator::Schedule(Seconds(PCTTUtils::CHECK_ANSWERED_TIME), &PCTTClusterHead::CheckAnswered, m_clusterHead);
			m_scheduledEvents.push_back(m_checkAnsweredEvent);

			m_checkOBTGreaterEvent = Simulator::Schedule(Seconds(PCTTUtils::CHECK_ANSWERED_TIME), &PCTT::CheckOBTGreater, this);
			m_scheduledEvents.push_back(m_checkOBTGreaterEvent);

			m_periodicallySendsCHMEvent = Simulator::Schedule(Seconds(PCTTUtils::RESET_TIME_INTERVAL), &PCTT::PeriodicallySendsCHM, this);
			m_scheduledEvents.push_back(m_periodicallySendsCHMEvent);

		} else { // Node does not visualize the target

			if(!m_clusterHead->GetMemberList().empty()) {  // But has members

				m_clusterHead->SearchCHAndCCHInMemberList();

				if(m_clusterHead->GetCHID() != (int32_t) m_node->GetId()) {  // Change CH to CM
					SendCHMessage(false, true, Seconds(0));

					PrintInLog("Resigned! The CH now is: " + std::to_string(m_clusterHead->GetCHID()));

					m_clusterMember = m_clusterHead->ToClusterMember();
					m_clusterHead->Dispose();
					m_clusterHead = nullptr;
					m_sendCHMessageBeforeConvergenceEvent.Cancel();
					m_convergenceOverEvent.Cancel();

					m_isClusterHead = false;
					m_isClusterMember = true;
					m_isNonMember = false;

					PrintInLog("Grouping with Node #" + std::to_string(m_clusterMember->GetCHID()));

					PrintInLog("Became CM");
					CMProcedure();
				} else {
					m_periodicallySendsCHMEvent = Simulator::Schedule(Seconds(PCTTUtils::RESET_TIME_INTERVAL), &PCTT::PeriodicallySendsCHM, this);
					m_scheduledEvents.push_back(m_periodicallySendsCHMEvent);
				}
			} else { // Change CH to NM
				m_nonMember = m_clusterHead->ToNonMember();
				m_clusterHead->Dispose();
				m_clusterHead = nullptr;
				m_sendCHMessageBeforeConvergenceEvent.Cancel();
				m_convergenceOverEvent.Cancel();

				m_isClusterHead = false;
				m_isClusterMember = false;
				m_isNonMember = true;

				NMProcedure();
			}
		}
	}
}

void PCTT::CalculatePredictionOBTInML() {
	NS_LOG_FUNCTION(this);

	if(m_isClusterHead) {
		for(PCTTClusterHead::CM cm : m_clusterHead->GetMemberList()) {
			Vector nextPosition;

			nextPosition.x = cm.lastPosition.x + cm.lastVelocity.x * PCTTUtils::PREDICTION_TIME_INTERVAL;
			nextPosition.y = cm.lastPosition.y + cm.lastVelocity.y * PCTTUtils::PREDICTION_TIME_INTERVAL;

			cm.obt = m_clusterHead->CalculateOBT(m_targetPosition, m_targetVelocity, nextPosition, cm.lastVelocity);

			/// REMOVE Only Validation
			double_t distance = CalculateDistance(m_targetPosition, nextPosition);
			if((distance < 10) && (cm.obt.GetNanoSeconds() == 0)) {
				cout << "OBT = 0 but in a short distance" << endl;
				exit(EXIT_FAILURE);
			}
			////////////////////

			cm.lastPosition = nextPosition;
		}

		CheckOBTGreater();

		m_calculatePredictionOBTInMLEvent = Simulator::Schedule(Seconds(PCTTUtils::PREDICTION_TIME_INTERVAL), &PCTT::CalculatePredictionOBTInML, this);
		m_scheduledEvents.push_back(m_calculatePredictionOBTInMLEvent);
	}
}

void PCTT::CMProcedure () {
	NS_LOG_FUNCTION(this);

	m_timeoutIntervalOverEvent = Simulator::Schedule(Seconds(PCTTUtils::RESET_TIME_INTERVAL + 0.2), &PCTT::TimeoutIntervalOver, this);
	m_scheduledEvents.push_back(m_timeoutIntervalOverEvent);

	m_calculateAndCheckPredictionEvent = Simulator::Schedule(Seconds(PCTTUtils::PREDICTION_TIME_INTERVAL), &PCTT::CalculateAndCheckPrediction, this);
	m_scheduledEvents.push_back(m_calculateAndCheckPredictionEvent);
}

void PCTT::CalculateAndCheckPrediction() {
	NS_LOG_FUNCTION(this);

	if(m_isClusterMember) {
		Vector nextPosition;

		// S = So + Vo * t
		nextPosition.x = m_clusterMember->GetLastPosition().x + m_clusterMember->GetLastVelocity().x * PCTTUtils::PREDICTION_TIME_INTERVAL;
		nextPosition.y = m_clusterMember->GetLastPosition().y + m_clusterMember->GetLastVelocity().y * PCTTUtils::PREDICTION_TIME_INTERVAL;

		double_t variationRateInX = abs( 100.0 - ((nextPosition.x * 100.0) / m_mobilityModel->GetPosition().x));
		double_t variationRateInY = abs( 100.0 - ((nextPosition.y * 100.0) / m_mobilityModel->GetPosition().y));

		NS_LOG_DEBUG("Prediction Position: " << nextPosition);
		NS_LOG_DEBUG("Real Position: " << m_mobilityModel->GetPosition());
		NS_LOG_DEBUG("VariationRateInX: " << variationRateInX << " VariationRateInY: " << variationRateInY);

		if((PCTTUtils::MAX_VARIATION_RATE_IN_X < variationRateInX ) || (PCTTUtils::MAX_VARIATION_RATE_IN_Y < variationRateInY)) {
			PrintInLog("Prediction Denied");

			m_clusterMember->SetLastPosition(m_mobilityModel->GetPosition());
			m_clusterMember->SetLastVelocity(m_mobilityModel->GetVelocity());

			SendCMMessage(true);
		} else {
			m_clusterMember->SetLastPosition(nextPosition);
		}

		m_calculateAndCheckPredictionEvent = Simulator::Schedule(Seconds(PCTTUtils::PREDICTION_TIME_INTERVAL), &PCTT::CalculateAndCheckPrediction, this);
		m_scheduledEvents.push_back(m_calculateAndCheckPredictionEvent);

	}
}

void PCTT::NMProcedure () {
	NS_LOG_FUNCTION(this);

	if(m_isNonMember) {

		if (m_nonMember->GetTDV(m_targetPosition, m_targetVelocity)) {
			PrintInLog("Now seeing the target.");
			SendCMMessage(false);

			m_changeNMToCHEvent = Simulator::Schedule (Seconds(PCTTUtils::CHANGE_NM_TO_CH_TIME), &PCTT::ChangeNMToCH, this);
			m_scheduledEvents.push_back(m_changeNMToCHEvent);

		} else {

			m_nmProcedureEvent = Simulator::Schedule (Time(MinuetConfig::CHECK_OCCURRENCE_EVENT_INTERVAL), &PCTT::NMProcedure, this);
			m_scheduledEvents.push_back(m_nmProcedureEvent);

		}
	}
}

void PCTT::ChangeNMToCH() {
	NS_LOG_FUNCTION(this);

	if(m_isNonMember) {
		PrintInLog("Became CH");

		m_clusterHead = m_nonMember->ToClusterHead();
		m_nonMember->Dispose();
		m_nonMember = nullptr;

		m_isClusterHead = true;
		m_isClusterMember = false;
		m_isNonMember = false;

		m_convergenceOverEvent = Simulator::Schedule(Seconds(PCTTUtils::CONVERGENCE_TIME), &PCTT::ConvergenceOver, this);
		m_scheduledEvents.push_back(m_convergenceOverEvent);

		SendCHMessageBeforeConvergence();
	}
}

void PCTT::ForwardCHMessage(CHMHeader chmHeader, CHMHeader::VehicleInfo chmHeaderInfo) {
	NS_LOG_FUNCTION(this);

	++chmHeaderInfo.hopCount;

	CHMHeader header;
	header.SetSeq(m_sentCounter++);

	header.SetVehicleInfo(chmHeaderInfo);

	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(header);

	PrintInLog("Forwarding CHM - From: " + std::to_string(chmHeaderInfo.nodeId)
				   + " PacketId: " + std::to_string(chmHeaderInfo.packetId)
				   + " CandidateClusterHeadId: " + std::to_string(chmHeaderInfo.candidateClusterHeadId)
				   + " Reset: " + std::to_string(chmHeaderInfo.reset)
				   + " ResignValue: " + std::to_string(chmHeaderInfo.resignValue)
				   + " ResignTime: " + std::to_string(chmHeaderInfo.resignTime.GetNanoSeconds()) + "ns"
				   + " HopCount: " + std::to_string(chmHeaderInfo.hopCount)
				   + " Position: (" + std::to_string(chmHeaderInfo.position.x) + ":" + std::to_string(chmHeaderInfo.position.y) + ":" + std::to_string(chmHeaderInfo.position.z) + ")"
				   + " Velocity: (" + std::to_string(chmHeaderInfo.velocity.x) + ":" + std::to_string(chmHeaderInfo.velocity.y) + ":" + std::to_string(chmHeaderInfo.velocity.z) + ")"
				   + " CurrentTime: " + std::to_string(chmHeaderInfo.currentTime.GetNanoSeconds()) + "ns"
				   + " ObservationTime: " + std::to_string(chmHeaderInfo.observationTime.GetNanoSeconds()));

	m_communicationLayer->SendControlMenssage(packet);
}

void PCTT::ForwardJAckMessage(JackMHeader jackmHeader, JackMHeader::VehicleInfo jackmHeaderInfo) {
	NS_LOG_FUNCTION(this);

	++jackmHeaderInfo.hopCount;

	JackMHeader header;
	header.SetSeq(m_sentCounter++);

	header.SetVehicleInfo(jackmHeaderInfo);

	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(header);

	PrintInLog("Forwarding JAckCHM - From: " + std::to_string(jackmHeaderInfo.nodeId)
			+ " NonMemberId: " + std::to_string(jackmHeaderInfo.nonMemberId)
			+ " PacketId: " + std::to_string(jackmHeaderInfo.packetId)
			+ " HopCount: " + std::to_string(jackmHeaderInfo.hopCount));

	m_communicationLayer->SendControlMenssage(packet);
}

void PCTT::ForwardCMMessage(CMMHeader cmmHeader, CMMHeader::VehicleInfo cmmHeaderInfo) {
	NS_LOG_FUNCTION(this);

	cmmHeaderInfo.hopCount++;

	CMMHeader header;
	header.SetSeq(m_sentCounter++);

	header.SetVehicleInfo(cmmHeaderInfo);

	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(header);

	PrintInLog("Forwarding CMM - From: " + std::to_string(cmmHeaderInfo.nodeId)
			+ " CHID: " + std::to_string(cmmHeaderInfo.chid)
			+ " PacketId: " + std::to_string(cmmHeaderInfo.packetId)
			+ " TDV: " + std::to_string(cmmHeaderInfo.targetDetectionValue)
			+ " HopCount: " + std::to_string(cmmHeaderInfo.hopCount)
			+ " Position: (" + std::to_string(cmmHeaderInfo.position.x) + ":" + std::to_string(cmmHeaderInfo.position.y) + ":" + std::to_string(cmmHeaderInfo.position.z) + ")"
			+ " Velocity: (" + std::to_string(cmmHeaderInfo.velocity.x) + ":" + std::to_string(cmmHeaderInfo.velocity.y) + ":" + std::to_string(cmmHeaderInfo.velocity.z) + ")"
			+ " CurrentTime: " + std::to_string(cmmHeaderInfo.currentTime.GetNanoSeconds()) + "ns"
			+ " ObservationTime: " + std::to_string(cmmHeaderInfo.observationTime.GetNanoSeconds()) + "ns"
			+ " PredictionDenial: " + std::to_string(cmmHeaderInfo.predictionDenial));

	m_communicationLayer->SendControlMenssage(packet);
}

void PCTT::SendCHMessageBeforeConvergence () {
	NS_LOG_FUNCTION(this);

	if (!m_convergenceOverEvent.IsExpired()) {
		SendCHMessage(false);
		m_sendCHMessageBeforeConvergenceEvent = Simulator::Schedule (Seconds(PCTTUtils::SEND_CH_MESSAGE_BEFORE_CONVERGENCE_TIME_INTERVAL), &PCTT::SendCHMessageBeforeConvergence, this);
	}
}

void PCTT::ConvergenceOver() {
	NS_LOG_FUNCTION(this);
	PrintInLog("Cluster Converging");

	m_sendCHMessageBeforeConvergenceEvent.Cancel();

	m_clusterHead->SearchCHAndCCHInMemberList();

	if (m_clusterHead->GetCHID() != (int32_t) m_node->GetId()) {

		m_clusterMember = m_clusterHead->ToClusterMember();
		m_clusterHead->Dispose();
		m_clusterHead = nullptr;
		m_sendCHMessageBeforeConvergenceEvent.Cancel();
		m_convergenceOverEvent.Cancel();

		PrintInLog("Became CM");

		m_isClusterHead = false;
		m_isClusterMember = true;
		m_isNonMember = false;

		PrintInLog("Grouping with Node #" + std::to_string(m_clusterMember->GetCHID()));

		CMProcedure();

	} else {
//		PrintInLog("Sending Reset Message");
//		m_clusterHead->GetMemberList().clear(); // Remove all CM of the member list
		CHProcedure();
	}
}

void PCTT::TimeoutIntervalOver() {
	NS_LOG_FUNCTION(this);

	if(m_isClusterMember) {
		PrintInLog("Out of Cluster");

		m_nonMember = m_clusterMember->ToNonMember();

		m_clusterMember->Dispose();
		m_clusterMember = nullptr;

		m_isClusterHead = false;
		m_isClusterMember = false;
		m_isNonMember = true;

		NMProcedure();
	} else {
		NS_LOG_INFO("Node is not Cluster Member");
	}
}

void PCTT::CheckOBTGreater() {
	NS_LOG_FUNCTION(this);

	if(m_isClusterHead) {
		if(m_clusterHead->GetTDV(m_targetPosition, m_targetVelocity)) {
			if(!m_clusterHead->GetMemberList().empty()) {
				PCTTClusterHead::CM ch = *m_clusterHead->GetMemberList().begin();
				PCTTClusterHead::CM cch = ch;

				for(PCTTClusterHead::CM cm : m_clusterHead->GetMemberList()) {
					if (cm.obt.Compare(ch.obt) == 1) {
						cch = ch;
						ch = cm;
					}
				}

				Time myOBT = m_clusterHead->CalculateOBT(m_targetPosition, m_targetVelocity, m_mobilityModel->GetPosition(), m_mobilityModel->GetVelocity());

				/// REMOVE Only Validation
				double_t distance = CalculateDistance(m_targetPosition, m_mobilityModel->GetPosition());
				if((distance < 10) && (myOBT.GetNanoSeconds() == 0)) {
					cout << "OBT = 0 but in a short distance" << endl;
					exit(EXIT_FAILURE);
				}
				////////////////////

				if(ch.obt.Compare(myOBT) == 1) {
					m_clusterHead->SetCCHID(ch.id);
					SendCHMessage(false, true, m_clusterHead->CalculateOBT(m_targetPosition, m_targetVelocity, m_mobilityModel->GetPosition(), m_mobilityModel->GetVelocity()));

					m_changeCHToCMEvent = Simulator::Schedule(m_clusterHead->CalculateOBT(m_targetPosition, m_targetVelocity, m_mobilityModel->GetPosition(), m_mobilityModel->GetVelocity()), &PCTT::ChangeCHToCM, this);
					m_scheduledEvents.push_back(m_changeCHToCMEvent);
				}
			}
		} else { // Node does not visualize the target

			if(!m_clusterHead->GetMemberList().empty()) {  // But has members

				m_clusterHead->SearchCHAndCCHInMemberList();

				if(m_clusterHead->GetCHID() != (int32_t) m_node->GetId()) {  // Change CH to CM
					SendCHMessage(false, true, Seconds(0));

					PrintInLog("Resigned! The CH now is: " + std::to_string(m_clusterHead->GetCHID()));

					m_clusterMember = m_clusterHead->ToClusterMember();
					m_clusterHead->Dispose();
					m_clusterHead = nullptr;
					m_sendCHMessageBeforeConvergenceEvent.Cancel();
					m_convergenceOverEvent.Cancel();

					m_isClusterHead = false;
					m_isClusterMember = true;
					m_isNonMember = false;

					PrintInLog("Grouping with Node #" + std::to_string(m_clusterMember->GetCHID()));

					PrintInLog("Became CM");
					CMProcedure();
				}

			} else { // Change CH to NM
				m_nonMember = m_clusterHead->ToNonMember();
				m_clusterHead->Dispose();
				m_clusterHead = nullptr;
				m_sendCHMessageBeforeConvergenceEvent.Cancel();
				m_convergenceOverEvent.Cancel();

				m_isClusterHead = false;
				m_isClusterMember = false;
				m_isNonMember = true;

				NMProcedure();
			}
		}
	}
}

void PCTT::ChangeCHToCM() {
	NS_LOG_FUNCTION(this);

	if(m_isClusterHead) {
		m_clusterMember = m_clusterHead->ToClusterMember();
		m_clusterHead->Dispose();
		m_clusterHead = nullptr;
		m_sendCHMessageBeforeConvergenceEvent.Cancel();
		m_convergenceOverEvent.Cancel();

		m_isClusterHead = false;
		m_isClusterMember = true;
		m_isNonMember = false;

		PrintInLog("Grouping with Node #" + std::to_string(m_clusterMember->GetCHID()));

		PrintInLog("Became CM");
		CMProcedure();
	}
}

void PCTT::EventsListMaintenance () {
	NS_LOG_FUNCTION(this);

	for(vector<EventId>::iterator eventIdIt = m_scheduledEvents.begin() ; eventIdIt < m_scheduledEvents.end() ; eventIdIt++) {
		if((*eventIdIt).IsExpired()) {
			eventIdIt = m_scheduledEvents.erase(eventIdIt);
		}
	}

	m_maintenanceEvent = Simulator::Schedule (Seconds(PCTTUtils::EVENTS_LIST_MAINTENANCE_TIMER), &PCTT::EventsListMaintenance, this);
}

bool PCTT::IsClusterHead () {
	NS_LOG_FUNCTION(this);

	return m_isClusterHead;
}

bool PCTT::IsClusterMember () {
	NS_LOG_FUNCTION(this);

	return m_isClusterMember;
}

bool PCTT::IsIsolated () {
	NS_LOG_FUNCTION(this);

	return m_isNonMember || (m_isClusterHead && !(m_clusterHead->GetMemberList().empty()));
}

uint32_t PCTT::GetClusterId() {
	NS_LOG_FUNCTION(this);

	if(IsClusterHead()) {
		return m_clusterHead->GetId();
	} else if (IsClusterMember()) {
		return m_clusterMember->GetCHID();
	} else if (m_isNonMember) {
		return m_nonMember->GetId();
	}	else {
		cerr << "## ERROR: In GetClusterId method node is not Cluster Head or Member Cluster" << endl;
		exit(1);
	}
}

bool PCTT::IsStarted() {
	return m_clusteringStarted;
}

bool PCTT::HasReceivedMsg (int32_t id, int32_t pckt_id) {
	bool hasReceived = false;

	if (!m_messagesReceived.empty()) {
		for (vector<int32_t> i : m_messagesReceived) {
			if (i[0] == id) {
				if (i[1] == pckt_id) {
					hasReceived = true;
				}
			}
		}
	}

	return hasReceived;
}

void PCTT::PrintInLog(string message) {
	int32_t chid = -1;

	if(m_clusterHead) {
		chid = m_clusterHead->GetCHID();
	} else if(m_clusterMember) {
		chid = m_clusterMember->GetCHID();
	} else if(m_nonMember) {
		chid = m_nonMember->GetCHID();
	}

	ofstream os;
	os.open (MinuetConfig::LOG_FILE_CLUSTERING_ALGORITHM.c_str(), ofstream::out | ofstream::app);
	os << Simulator::Now().GetNanoSeconds() << "ns - PCTT - Node #" << m_node->GetId() << " Pos (" << m_mobilityModel->GetPosition() <<  ") ClusterId #" << chid << " : " << message << endl;
	os.close();
}

}
