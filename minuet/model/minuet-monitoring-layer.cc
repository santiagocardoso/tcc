/*
 * minuet-monitoring-layer.cc
 *
 *  Created on: 13 de nov de 2018
 *      Author: everaldo
 */

#include "minuet-monitoring-layer.h"

namespace ns3 {

	NS_LOG_COMPONENT_DEFINE ("MonitoringLayer");

/*************** MonitoringLayerInterface ************************/
	NS_OBJECT_ENSURE_REGISTERED (MonitoringLayerInterface);

	MonitoringLayerInterface::MonitoringLayerInterface() {
		//NS_LOG_FUNCTION(this);
	}

	MonitoringLayerInterface::~MonitoringLayerInterface() {
		//NS_LOG_FUNCTION(this);
	}

	void MonitoringLayerInterface::AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer) {
		MonitoringLayerInterface::m_communicationLayer = comunicationLayer;
		MonitoringLayerInterface::m_communicationLayer->AttachMonitoringLayerInterface(this);
	}
/*************** END MonitoringLayerInterface ************************/

/*************** MonitoringLayer ************************/
	NS_OBJECT_ENSURE_REGISTERED (MonitoringLayer);


	TypeId MonitoringLayer::GetTypeId(void) {
		static TypeId tid =
				TypeId("ns3::MonitoringLayer")
				.SetParent<Object>()
				.AddConstructor<MonitoringLayer>()
				.AddAttribute("Node",
						"The Node of the Node", PointerValue(),
						MakePointerAccessor(&MonitoringLayer::m_node),
						MakePointerChecker<Node>())
				.AddAttribute("CommunicationLayer",
						"The Communication Instance", PointerValue(),
						MakePointerAccessor(&MonitoringLayer::m_communicationLayer),
						MakePointerChecker<CommunicationLayer>())
				.AddAttribute("ClusteringManager",
						"The Clustering Manager Instance", PointerValue(),
						MakePointerAccessor(&MonitoringLayer::m_clusteringManager),
						MakePointerChecker<ClusteringManager>());
		return tid;
	}

	MonitoringLayer::MonitoringLayer() {
		NS_LOG_FUNCTION(this);
		m_isStarted = false;
		m_isMonitoring = false;
		m_sentMonitoringMensCounter = 0;
		m_forwardMonitoringMensCounter = 0;
		m_statusNode = MonitoringEventHeader::StatusNode::RETRANS;
	}

	MonitoringLayer::~MonitoringLayer() {
		NS_LOG_FUNCTION(this);
	}

	void MonitoringLayer::StartLayer() {
		NS_LOG_FUNCTION(this);

		if(!m_isStarted) {
			if(m_communicationLayer && m_node) {
				m_mobilityModel = m_node->GetObject<MobilityModel>();
				AddCommunicationLayer(m_communicationLayer);

				SetupTraceVideoStream(); // Set up video to stream

				m_isStarted = true;
				PrintInLog("MonitoringLayer started");
				NS_LOG_DEBUG("### MonitoringLayer started!");
			} else {
				NS_LOG_WARN("### Impossible to Start MonitoringLayer: CommunicationLayer Or Node is Null");
				exit(EXIT_FAILURE);
			}
		} else {
			NS_LOG_INFO("### MonitoringLayer already started!");
		}
	}

	void MonitoringLayer::StopLayer() {
		NS_LOG_FUNCTION(this);
		PrintInLog("Stopping Layer");
		StopMonitoring();

		m_checkingBaseStationProximityEvent.Cancel();

		PrintInLog("Total Monitoring Messages Sent = " +  std::to_string(m_sentMonitoringMensCounter));
		PrintInLog("Total Monitoring Messages Forward = " +  std::to_string(m_forwardMonitoringMensCounter));
	}

	void MonitoringLayer::SetupTraceVideoStream() {
		uint32_t frameId;
		string frameType;
		uint32_t frameSize;
		uint16_t numOfUdpPackets;
		double_t sendTime;
		double_t lastSendTime = 0.0;

		// Setup set frames video
		m_videoInfoStruct_t *videoInfoStruct;

		//Open file from mp4trace tool of EvalVid.
		ifstream videoTraceFile(MinuetConfig::VIDEO_TRACE_FILE_NAME.c_str(), ios::in);

		if (videoTraceFile.fail()) {
			cerr << "## ERROR: In Monitoring Layer Video Trace File Not Found" << endl;
			exit(1);
		}

		//Store video trace information on the struct
		while (videoTraceFile >> frameId >> frameType >> frameSize >> numOfUdpPackets >> sendTime) {
			videoInfoStruct = new m_videoInfoStruct_t;
			videoInfoStruct->frameType = *(frameType.c_str());
			videoInfoStruct->frameSize = frameSize;
			videoInfoStruct->numOfUdpPackets = numOfUdpPackets;
			videoInfoStruct->packetInterval = Seconds(sendTime - lastSendTime);
			m_videoInfoMap.insert (pair<uint32_t, m_videoInfoStruct_t*>(frameId, videoInfoStruct));
			//NS_LOG_DEBUG(">> MonitoringLayer: " << frameId << "\t" << frameType << "\t" <<
			//frameSize << "\t" << numOfUdpPackets << "\t" << sendTime);
			lastSendTime = sendTime;
		}

		m_numOfFrames = frameId;
		m_videoInfoMapIt = m_videoInfoMap.begin();
	}

	bool MonitoringLayer::IsMonitoring() const {
		return m_isMonitoring;
	}

	void MonitoringLayer::StartMonitoring(Ptr<EventDetected> event) {
		NS_LOG_FUNCTION(this);
		if(!m_isMonitoring)
			TryMonitoring(event);
	}

	void MonitoringLayer::StopMonitoring() {
		//NS_LOG_FUNCTION(this);
		if(m_isStarted) {
			if(m_isMonitoring) {
				PrintInLog("Stopping Monitoring");
				m_monitoringEvent.Cancel();
				m_isMonitoring = false;
				NS_LOG_DEBUG("### Stopped Monitoring!");
			} else {
				NS_LOG_INFO("### Monitoring Layer Is Not Monitoring!");
			}
			//m_checkingBaseStationProximityEvent.Cancel(); //Acho que dá erro porque a camada não encerra de maneira correta, mas comentei para testar
		} else {
			NS_LOG_WARN("### Impossible Stop Monitoring Now: Monitoring Layer no Started!");
		}
	}

	void MonitoringLayer::TryMonitoring(Ptr<EventDetected> event) {
		NS_LOG_FUNCTION(this);

		// PrintInLog("Try Monitoring Event (" + to_string(event->GetEventId()) + ")");

		if(IsInVisionField(event)) {
			AddStatus(MonitoringEventHeader::StatusNode::MONITOR);
			m_isMonitoring = true;

			if((m_videoInfoMapIt == m_videoInfoMap.end())) {
				m_videoInfoMapIt = m_videoInfoMap.begin();
				cerr << "## ERROR: Video limit exceeded'" << endl;
				//exit(EXIT_FAILURE);
			}

			event->SetDetectionTime(Simulator::Now());

			for(int i = 0 ; i < m_videoInfoMapIt->second->numOfUdpPackets - 1 ; i++) {
				Monitoring(event, MinuetConfig::PACKET_PAYLOAD_SIZE);
				PrintTotalFramesSent(m_videoInfoMapIt->first, m_node->GetId(), event->GetEventId());
			}

			Monitoring(event, m_videoInfoMapIt->second->frameSize % MinuetConfig::PACKET_PAYLOAD_SIZE);
			PrintTotalFramesSent(m_videoInfoMapIt->first, m_node->GetId(), event->GetEventId());
			m_videoInfoMapIt++;

			if (m_videoInfoMapIt->second->packetInterval.GetSeconds() == 0)
				m_monitoringEvent = Simulator::ScheduleNow(&MonitoringLayer::TryMonitoring, this, event);
			else
				m_monitoringEvent = Simulator::Schedule (m_videoInfoMapIt->second->packetInterval, &MonitoringLayer::TryMonitoring, this, event);

		} else {
			RemoveStatus(MonitoringEventHeader::StatusNode::MONITOR);
			m_isMonitoring = false;

			FinalizeDump(m_sentMonitoringMensCounter, event->GetEventId());
		}
	}

	void MonitoringLayer::Monitoring(Ptr<EventDetected> eventDetected, uint32_t packetPayloadSize) {
		NS_LOG_FUNCTION(this);

		MonitoringEventHeader monitoringEventHeader;
		monitoringEventHeader.SetSeq(m_sentMonitoringMensCounter);
		monitoringEventHeader.SetStatusNode(m_statusNode);
		monitoringEventHeader.SetRelayId(m_clusteringManager->GetRelayId(m_forwardMonitoringMensCounter));
		monitoringEventHeader.SetNodeId(m_node->GetId());
		monitoringEventHeader.SetMonitorId(m_node->GetId());
		monitoringEventHeader.SetMonitoringTime(eventDetected->GetDetectionTime().GetNanoSeconds());
		monitoringEventHeader.SetEventId(eventDetected->GetEventId());
		monitoringEventHeader.SetFrameId(m_videoInfoMapIt->first);
		monitoringEventHeader.SetFrameTye(m_videoInfoMapIt->second->frameType);

		Ptr<Packet> packet = Create<Packet>(packetPayloadSize);
		packet->AddHeader(monitoringEventHeader);

		m_communicationLayer->SendDataMenssage(packet);

		NS_LOG_FUNCTION("Pacote " << monitoringEventHeader.GetSeq() << "sendo transmitido por " << monitoringEventHeader.GetRelayId() << " pela primeira vez.");

		m_forwardMessages.push_back(vector<uint64_t> {monitoringEventHeader.GetRelayId(), monitoringEventHeader.GetMonitorId(), monitoringEventHeader.GetSeq()});

		//NS_LOG_DEBUG(">> MonitoringLayer: Send packet at " << Simulator::Now() << "\tid: " << m_sentMonitoringMensCounter << "\tudp\t" << packet->GetSize() << std::endl);

		PrintInLog("Sending Monitoring Message:" \
				" Event = " + to_string(eventDetected->GetEventId()) +
				" Seq = " + to_string(monitoringEventHeader.GetSeq()) +
				" FrameId = " + to_string(monitoringEventHeader.GetFrameId()) +
				" FrameType = " + string(1, monitoringEventHeader.GetFrameType()));

		PrintInDump(Simulator::Now(), m_sentMonitoringMensCounter++, packetPayloadSize, monitoringEventHeader.GetEventId(), m_node->GetId());
	}

	bool MonitoringLayer::IsInVisionField(Ptr<EventDetected> event) {
		// Check distance
		double_t distance = sqrt(pow((m_mobilityModel->GetPosition().x - event->GetMobilityModel()->GetPosition().x), 2) + pow((m_mobilityModel->GetPosition().y - event->GetMobilityModel()->GetPosition().y), 2));

		if(distance <= MinuetConfig::MAX_VISION_FIELD_DISTANCE) { // Check if event is near
			if(m_mobilityModel->GetVelocity().GetLength() != 0) {
				/*********** Calculates the angle diff between event and node vectors ************/
				Vector V = m_mobilityModel->GetVelocity();
				Vector Vve = Vector(event->GetMobilityModel()->GetPosition().x-m_mobilityModel->GetPosition().x, event->GetMobilityModel()->GetPosition().y-m_mobilityModel->GetPosition().y, 0);
				double_t angleDiff = acos(((V.x*Vve.x) + (V.y*Vve.y)) / (V.GetLength() * Vve.GetLength())) * 180.0 / PI;
				/*********************************************************************************/

				if((0 <= angleDiff and angleDiff <= MinuetConfig::MAX_VISION_FIELD_ANGLE) or (180 - MinuetConfig::MAX_VISION_FIELD_ANGLE <= angleDiff and angleDiff <= 180)) { // Check angle diff
					return true;
				}
			} else {
				return true;	// Node stationary near the event
			}
		}

		return false;
	}

	void MonitoringLayer::ReceiveDataMessage(Ptr<Packet> packet, Address addr) {
		//NS_LOG_FUNCTION(this);

		MonitoringEventHeader monitoringEventHeader;
		packet->RemoveHeader(monitoringEventHeader);

		PrintInLog("Monitoring Message Received:" \
				" From = " + to_string(monitoringEventHeader.GetNodeId()) +
				" Status = " + to_string(monitoringEventHeader.GetStatusNode()) +
				" RelayId = " + to_string(monitoringEventHeader.GetRelayId()) +
				" MonitorId = " + to_string(monitoringEventHeader.GetMonitorId()) +
				" MonitoringTime = " + to_string(monitoringEventHeader.GetMonitoringTime()) +
				" FrameId = " + to_string(monitoringEventHeader.GetFrameId()) +
				" FrameType = " + string(1, monitoringEventHeader.GetFrameType()) +
				" Seq = " + to_string(monitoringEventHeader.GetSeq()) +
				" EventId = " + to_string(monitoringEventHeader.GetEventId()));

		if(m_clusteringManager->IsStarted()) {
			if(monitoringEventHeader.GetRelayId() == m_node->GetId()) {
				PrintInLog("Monitoring Message Received in Group #" + to_string(m_node->GetId()));
				if((monitoringEventHeader.GetStatusNode() != MonitoringEventHeader::StatusNode::GATEWAY_BS) && (monitoringEventHeader.GetStatusNode() != MonitoringEventHeader::StatusNode::GATEWAY_BS_MONITOR) && (monitoringEventHeader.GetStatusNode() != MonitoringEventHeader::StatusNode::GATEWAY_C_GATEWAY_BS_MONITOR)) {
					if(CheckMonitoringZone(monitoringEventHeader.GetMonitoringTime())) {
						if(!CheckForwardMessage(monitoringEventHeader.GetRelayId(), monitoringEventHeader.GetMonitorId(), monitoringEventHeader.GetSeq())) {
							ForwardMonitoringMessage(monitoringEventHeader.GetSeq(), monitoringEventHeader.GetMonitorId(),
										monitoringEventHeader.GetEventId(), monitoringEventHeader.GetMonitoringTime(), monitoringEventHeader.GetFrameId(), monitoringEventHeader.GetFrameType(), packet->GetSize());

						} else {
							PrintInLog("Unable To Forward Event #" + to_string(monitoringEventHeader.GetEventId()) + " Monitoring Message: Monitoring Message Already Forwarded in Group #" + std::to_string(m_clusteringManager->GetRelayId(m_sentMonitoringMensCounter)));
						}
					} else {
						PrintInLog("Unable To Forward Event #" + to_string(monitoringEventHeader.GetEventId()) + " Monitoring Message: Out of Announcement Zone");
					}
				}
			} else if(m_statusNode == MonitoringEventHeader::StatusNode::GATEWAY_C) {
				if((monitoringEventHeader.GetStatusNode() != MonitoringEventHeader::StatusNode::GATEWAY_BS) && (monitoringEventHeader.GetStatusNode() != MonitoringEventHeader::StatusNode::GATEWAY_BS_MONITOR) && (monitoringEventHeader.GetStatusNode() != MonitoringEventHeader::StatusNode::GATEWAY_C_GATEWAY_BS_MONITOR)) {
					monitoringEventHeader.SetStatusNode(m_statusNode);
					monitoringEventHeader.SetRelayId(m_clusteringManager->GetRelayId(m_sentMonitoringMensCounter));

					monitoringEventHeader.SetSeq(monitoringEventHeader.GetSeq());

					packet->AddHeader(monitoringEventHeader);

					m_communicationLayer->SendDataMenssage(packet);

					m_forwardMonitoringMensCounter++;
				}
			}
		}
	}

	void MonitoringLayer::ReceiveBaseStationAnnouncementMessage(Ptr<Packet> packet, Address addr) {
		//NS_LOG_FUNCTION(this);
		PrintInLog("Announcement Message received from Base Station");
		AddStatus(MonitoringEventHeader::StatusNode::GATEWAY_BS);


		if(!m_checkingBaseStationProximityEvent.IsExpired())
			m_checkingBaseStationProximityEvent.Cancel();

		m_checkingBaseStationProximityEvent = Simulator::Schedule(Time(MinuetConfig::TIME_EXPIRE_GATEWAY), &MonitoringLayer::RemoveStatus, this, MonitoringEventHeader::StatusNode::GATEWAY_BS);
	}

	void MonitoringLayer::AddStatus(MonitoringEventHeader::StatusNode statusNode) {
		//NS_LOG_FUNCTION(this);
		if((statusNode == MonitoringEventHeader::StatusNode::MONITOR) && (m_statusNode != statusNode)) {
			if((m_statusNode != MonitoringEventHeader::StatusNode::GATEWAY_BS_MONITOR) && (m_statusNode != MonitoringEventHeader::StatusNode::GATEWAY_C_MONITOR) && (m_statusNode != MonitoringEventHeader::StatusNode::GATEWAY_C_GATEWAY_BS_MONITOR)) {
				m_statusNode = static_cast<MonitoringEventHeader::StatusNode>(m_statusNode + statusNode);
			}
		}

		if((statusNode == MonitoringEventHeader::StatusNode::GATEWAY_BS) && (m_statusNode != statusNode)) {
			if((m_statusNode != MonitoringEventHeader::StatusNode::GATEWAY_BS_MONITOR) && (m_statusNode != MonitoringEventHeader::StatusNode::GATEWAY_C_GATEWAY_BS) && (m_statusNode != MonitoringEventHeader::StatusNode::GATEWAY_C_GATEWAY_BS_MONITOR)) {
				m_statusNode = static_cast<MonitoringEventHeader::StatusNode>(m_statusNode + statusNode);
				if(m_clusteringManager){
					m_clusteringManager->SetIsGateway(true);
				}
				else{
					NS_LOG_INFO("Clustering Manager encerrado.");
				}
			}
		}

		if((statusNode == MonitoringEventHeader::StatusNode::GATEWAY_C) && (m_statusNode != statusNode)) {
			if((m_statusNode != MonitoringEventHeader::StatusNode::GATEWAY_C_MONITOR) && (m_statusNode != MonitoringEventHeader::StatusNode::GATEWAY_C_GATEWAY_BS) && (m_statusNode != MonitoringEventHeader::StatusNode::GATEWAY_C_GATEWAY_BS_MONITOR)) {
				m_statusNode = static_cast<MonitoringEventHeader::StatusNode>(m_statusNode + statusNode);
			}
		}

//		PrintInLog("Add Status to " + std::to_string(m_statusNode));
	}

	void MonitoringLayer::RemoveStatus(MonitoringEventHeader::StatusNode statusNode) {
		//NS_LOG_FUNCTION(this);
		if((statusNode == MonitoringEventHeader::StatusNode::MONITOR)) {
			if((m_statusNode == statusNode) || (m_statusNode == MonitoringEventHeader::StatusNode::GATEWAY_BS_MONITOR) || (m_statusNode == MonitoringEventHeader::StatusNode::GATEWAY_C_MONITOR) || (m_statusNode == MonitoringEventHeader::StatusNode::GATEWAY_C_GATEWAY_BS_MONITOR)) {
				m_statusNode = static_cast<MonitoringEventHeader::StatusNode>(m_statusNode - statusNode);
			}
		}

		if((statusNode == MonitoringEventHeader::StatusNode::GATEWAY_BS)) {
			if((m_statusNode == statusNode) || (m_statusNode == MonitoringEventHeader::StatusNode::GATEWAY_BS_MONITOR) || (m_statusNode == MonitoringEventHeader::StatusNode::GATEWAY_C_GATEWAY_BS) || (m_statusNode == MonitoringEventHeader::StatusNode::GATEWAY_C_GATEWAY_BS_MONITOR)) {
				m_statusNode = static_cast<MonitoringEventHeader::StatusNode>(m_statusNode - statusNode);
				m_clusteringManager->SetIsGateway(false);
			}
		}

		if((statusNode == MonitoringEventHeader::StatusNode::GATEWAY_C)) {
			if((m_statusNode == statusNode) ||(m_statusNode == MonitoringEventHeader::StatusNode::GATEWAY_C_MONITOR) || (m_statusNode == MonitoringEventHeader::StatusNode::GATEWAY_C_GATEWAY_BS) || (m_statusNode == MonitoringEventHeader::StatusNode::GATEWAY_C_GATEWAY_BS_MONITOR)) {
				m_statusNode = static_cast<MonitoringEventHeader::StatusNode>(m_statusNode - statusNode);
			}
		}

//		PrintInLog("Remove Status to " + std::to_string(m_statusNode));
	}

	void MonitoringLayer::PrintInLog(string message) {
		ofstream os;
		os.open (MinuetConfig::LOG_FILE_MONITORING_LAYER.c_str(), ofstream::out | ofstream::app);

		string groupId = " --";
		os << Simulator::Now().GetNanoSeconds() << "ns - MonitoringLayer - Node #" << m_node->GetId() << " Current Status " << m_statusNode
				<< " Pos (" << m_mobilityModel->GetPosition() <<  ")" << " GroupId #" << groupId << " : " << message << endl;
		os.close();
		return; //removi LOG
	}

	void MonitoringLayer::PrintInDump(Time time, uint64_t pkt_id, uint32_t pkt_size, uint32_t event_id, uint32_t node_id) {
		ofstream os;
		os.setf(std::ios::fixed, std::ios::floatfield);	// floatfield set to fixed
		os.precision(3);

		os.open ((MinuetConfig::SD_FILE_PATH + MinuetConfig::SD_FILE_NAME + to_string(node_id) + "_" + to_string(event_id)).c_str(), ofstream::out | ofstream::app);
		os << time.GetSeconds() << " id " << pkt_id << " udp " << pkt_size << endl;
		os.close();
	}

	void MonitoringLayer::FinalizeDump(uint64_t pkt_id, uint32_t event_id) {
		if(m_videoInfoMapIt != m_videoInfoMap.end()) {
			Time time_aux = Simulator::Now();

			for(int i = 0 ; i < m_videoInfoMapIt->second->numOfUdpPackets - 1 ; i++)
				PrintInDump(time_aux, pkt_id++, MinuetConfig::PACKET_PAYLOAD_SIZE, event_id, m_node->GetId());

			PrintInDump(time_aux, pkt_id++, m_videoInfoMapIt->second->frameSize % MinuetConfig::PACKET_PAYLOAD_SIZE, event_id, m_node->GetId());
			m_videoInfoMapIt++;

			while(m_videoInfoMapIt != m_videoInfoMap.end()){
				time_aux = time_aux + m_videoInfoMapIt->second->packetInterval;

				for(int i = 0 ; i < m_videoInfoMapIt->second->numOfUdpPackets - 1 ; i++)
					PrintInDump(time_aux, pkt_id++, MinuetConfig::PACKET_PAYLOAD_SIZE, event_id, m_node->GetId());

				PrintInDump(time_aux, pkt_id++, m_videoInfoMapIt->second->frameSize % MinuetConfig::PACKET_PAYLOAD_SIZE, event_id, m_node->GetId());
				m_videoInfoMapIt++;
			}
		}
	}

	void MonitoringLayer::PrintTotalFramesSent(uint32_t totalFrames, uint32_t nodeId, uint32_t eventId) {
		ofstream os;
		os.open ((MinuetConfig::SD_FILE_PATH + MinuetConfig::TOTAL_FRAMES_SENT_FILE_NAME + to_string(nodeId) + "_" + to_string(eventId)).c_str(), ofstream::out);
		os << totalFrames << endl;
		os.close();
	}

	void MonitoringLayer::ForwardMonitoringMessage(uint64_t seq, uint32_t monitorId, uint32_t eventId, uint64_t monitoringTime, uint32_t frameId, char32_t frameType, uint32_t payloadSize) {
		NS_LOG_FUNCTION(this);

		MonitoringEventHeader monitoringEventHeader;

		monitoringEventHeader.SetStatusNode(m_statusNode);
		monitoringEventHeader.SetNodeId(m_node->GetId());
		monitoringEventHeader.SetSeq(seq);
		monitoringEventHeader.SetMonitorId(monitorId);
		monitoringEventHeader.SetRelayId(m_clusteringManager->GetRelayId(m_forwardMonitoringMensCounter));
		monitoringEventHeader.SetEventId(eventId);
		monitoringEventHeader.SetMonitoringTime(monitoringTime);
		monitoringEventHeader.SetFrameId(frameId);
		monitoringEventHeader.SetFrameTye(frameType);

		Ptr<Packet> packet = Create<Packet>(payloadSize);
		packet->AddHeader(monitoringEventHeader);

		m_communicationLayer->SendDataMenssage(packet);

		m_forwardMonitoringMensCounter++;

		//NS_LOG_FUNCTION("Pacote " << monitoringEventHeader.GetSeq() << "sendo retransmitido por " << monitoringEventHeader.GetRelayId() << " pela " << m_forwardMonitoringMensCounter << " vez.");

		m_forwardMessages.push_back(vector<uint64_t> {monitorId, seq});

		PrintInLog("Monitoring Message Forwarded in Cluster:" \
					" MonitoringTime = " + to_string(monitoringEventHeader.GetMonitoringTime()) +
					" MonitorId = " + to_string(monitoringEventHeader.GetMonitorId()) +
					" FrameId = " + to_string(monitoringEventHeader.GetFrameId()) +
					" FrameType = " + string(1, monitoringEventHeader.GetFrameType()) +
					" Seq = " + to_string(monitoringEventHeader.GetSeq()));
	}

	bool MonitoringLayer::CheckForwardMessage(uint32_t relayId, uint32_t nodeId, uint64_t seq) {
		//NS_LOG_FUNCTION(this);
		for(auto message : m_forwardMessages) {
			if((message[0] == relayId) && (message[1] == nodeId) && (message[2] == seq))
				return true;
		}
		return false;
	}

	bool MonitoringLayer::CheckMonitoringZone(uint64_t monitoringTime) {
		NS_LOG_FUNCTION(this);
		if((Simulator::Now().GetNanoSeconds() - monitoringTime) <= Time(MinuetConfig::MONITORING_DISSEMINATION_LIMIT_TIME))

			return true;
		else
			return false;
	}

/*************** END MonitoringLayer ************************/

} // namespace ns3

