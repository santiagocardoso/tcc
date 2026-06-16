/*
 * minuet-announcement-layer.cc
 *
 *  Created on: 5 de nov de 2018
 *      Author: everaldo
 */

#include "minuet-announcement-layer.h"

namespace ns3 {

	NS_LOG_COMPONENT_DEFINE ("AnnouncementLayer");

/*************** AnnouncementLayerInterface ************************/
	NS_OBJECT_ENSURE_REGISTERED (AnnouncementLayerInterface);

	AnnouncementLayerInterface::AnnouncementLayerInterface() {
		NS_LOG_FUNCTION(this);
	}

	AnnouncementLayerInterface::~AnnouncementLayerInterface() {
		NS_LOG_FUNCTION(this);
	}

	void AnnouncementLayerInterface::AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer) {
		AnnouncementLayerInterface::m_communicationLayer = comunicationLayer;
		AnnouncementLayerInterface::m_communicationLayer->AttachAnnouncementLayerInterface(this);
	}
/*************** END AnnouncementLayerInterface **********************/

/*************** AnnouncementLayerInterface ************************/
	NS_OBJECT_ENSURE_REGISTERED (AnnouncementLayer);

	TypeId AnnouncementLayer::GetTypeId(void) {
		static TypeId tid =
				TypeId("ns3::AnnouncementLayer")
				.SetParent<AnnouncementLayerInterface>()
				.AddConstructor<AnnouncementLayer>()
				.AddAttribute("Node",
						"The Node of the Node", PointerValue(),
						MakePointerAccessor(&AnnouncementLayer::m_node),
						MakePointerChecker<Node>())
				.AddAttribute("CommunicationLayer",
						"The Communication Instance", PointerValue(),
						MakePointerAccessor(&AnnouncementLayer::m_communicationLayer),
						MakePointerChecker<CommunicationLayer>());

		return tid;
	}

	AnnouncementLayer::AnnouncementLayer(){
		NS_LOG_FUNCTION(this);
		m_communicationLayer = nullptr;
		m_node = nullptr;
		m_isStarted = false;
		m_sentCounter = 0;
	}

	AnnouncementLayer::~AnnouncementLayer(){
		NS_LOG_FUNCTION(this);
	}

	void AnnouncementLayer::StartLayer() {
		NS_LOG_FUNCTION(this);
		if(!m_isStarted) {
			if(m_communicationLayer && m_node) {

				m_mobilityModel = m_node->GetObject<MobilityModel>();

				PrintInLog("Starting Layer");

				m_clusteringManager = CreateObjectWithAttributes<ClusteringManager>("Node", PointerValue(m_node), "CommunicationLayer", PointerValue(m_communicationLayer));

				m_monitoringLayer = CreateObjectWithAttributes<MonitoringLayer>("Node", PointerValue(m_node), "CommunicationLayer", PointerValue(m_communicationLayer),
																			"ClusteringManager", PointerValue(m_clusteringManager));
				m_monitoringLayer->StartLayer();

				m_clusteringManager->StartClustering();

				AddCommunicationLayer(m_communicationLayer);
				m_isStarted = true;

				NS_LOG_DEBUG("### AnnouncementLayer started!");
			} else {
				NS_LOG_WARN("### Impossible to Start AnnouncementLayer: CommunicationLayer Or Node is Null");
				exit(EXIT_FAILURE);
			}
		} else {
			NS_LOG_INFO("### AnnouncementLayer already started!");
		}
	}

	void AnnouncementLayer::StopLayer() {
		NS_LOG_FUNCTION(this);
		PrintInLog("Stopping Layer");
		m_announcementFrequency.Cancel();
		m_monitoringLayer->StopLayer();
		m_clusteringManager->StopClustering();
		m_clusteringManager->Dispose();
		m_monitoringLayer->Dispose();
	}

	void AnnouncementLayer::receiveControlMessage(Ptr<Packet> packet, Address addr) {
		//NS_LOG_FUNCTION(this);
		Ptr<EventDetected> eventDetected;

		// Test if received Announcement Event Message
		AnnouncementEventHeader announcementEventHeader;
		packet->RemoveHeader(announcementEventHeader);
		AnnouncementEventHeader::AnnouncementEventDetectedInfo eventInfo = announcementEventHeader.GetEventDetectedInfo();

		eventDetected = CreateObject<EventDetected>();
		eventDetected->SetEventId(eventInfo.eventId);
		eventDetected->SetFixed(eventInfo.fixed);
		eventDetected->SetDetectionTime(Time(eventInfo.detectionTime));
		eventDetected->SetOccurenceTime(Time(eventInfo.occurrenceTime));
		eventDetected->SetDuration(Time(eventInfo.duration));

		Ptr<ConstantVelocityMobilityModel> mobilityModel =  CreateObject<ConstantVelocityMobilityModel>();
		mobilityModel->SetAttribute("Position", VectorValue(eventInfo.position));
		mobilityModel->SetVelocity(eventInfo.velocity);
		eventDetected->SetMobilityModel(mobilityModel);

		eventDetected->SetLastUpdated(Time(eventInfo.lastUpdated));

		PrintInLog("Announcement Message Received: Event #" + to_string(eventInfo.eventId) + " DetectorNodeId: #" + to_string(announcementEventHeader.GetDetectorNodeId()) + " Seq: " + to_string(announcementEventHeader.GetSeq()));

		if(CheckLifeTimeEvent(eventDetected)) {
			if(CheckAnnouncementZone(eventDetected)) {
				if(!CheckForwardMessage(announcementEventHeader.GetDetectorNodeId(), announcementEventHeader.GetSeq())) {
					ForwardAnnounceMessage(announcementEventHeader.GetDetectorNodeId(), announcementEventHeader.GetSeq(), eventDetected); // Forwarded Event Announcement Message
				} else {
					PrintInLog("Unable To Forward Event #" + to_string(eventDetected->GetEventId()) + " Detection Message: Message Already Forwarded");
				}
			} else {
				PrintInLog("Unable To Forward Event #" + to_string(eventDetected->GetEventId()) + " Detection Message: Out of Announcement Zone");
			}
		} else {
			PrintInLog("Unable To Forward Event #" + to_string(eventDetected->GetEventId()) + " Detection Message: Out of Lifetime Event");
		}

	}

	void AnnouncementLayer::ForwardAnnounceMessage(uint32_t detectorNodeId, uint64_t seq, Ptr<EventDetected> event) {
		//NS_LOG_FUNCTION(this);

		AnnouncementEventHeader::AnnouncementEventDetectedInfo eventInfo;
		eventInfo.eventId = event->GetEventId();
		eventInfo.fixed = event->IsFixed();
		eventInfo.detectionTime = event->GetDetectionTime().GetNanoSeconds();
		eventInfo.occurrenceTime = event->GetOccurenceTime().GetNanoSeconds();
		eventInfo.duration = event->GetDuration().GetNanoSeconds();
		eventInfo.position = event->GetMobilityModel()->GetPosition();
		eventInfo.velocity = event->GetMobilityModel()->GetVelocity();
		eventInfo.lastUpdated = event->GetLastUpdated().GetNanoSeconds();

		AnnouncementEventHeader announcementEventHeader;
		announcementEventHeader.SetSeq(seq);
		announcementEventHeader.SetDetectorNodeId(detectorNodeId);
		announcementEventHeader.SetEventDetectedInfo(eventInfo);

		Ptr<Packet> packet = Create<Packet>();
		packet->AddHeader(announcementEventHeader);

		m_communicationLayer->SendControlMenssage(packet); // Send Event Detection Message

		m_forwardMessages.push_back(vector<uint64_t> {detectorNodeId, seq});

		PrintInLog("Announcement Message Forwarded: Event #" + to_string(eventInfo.eventId) + " DetectorNodeId: #" + to_string(announcementEventHeader.GetDetectorNodeId()) + " Seq: " + to_string(announcementEventHeader.GetSeq()) + " Distance: " + to_string(CalculateDistance(m_mobilityModel->GetPosition(), event->GetMobilityModel()->GetPosition())));

		//m_clusteringManager->StartClustering();
	}

	void AnnouncementLayer::TryAnnounceEvent(Ptr<EventDetected> event) {
		NS_LOG_FUNCTION(this);

		if(m_eventsDataBase.empty()) {
			m_eventsDataBase.push_back(event);
			AnnounceEvent(event->GetEventId());
		} else {
			bool eventExist = false;

			for(vector<Ptr<EventDetected>>::iterator it = m_eventsDataBase.begin() ; it != m_eventsDataBase.end() ; it++) {
				if((*it)->GetEventId() == event->GetEventId()) {
					eventExist = true;
					m_eventsDataBase.erase(it);
					m_eventsDataBase.push_back(event);
					break;
				}
			}

			if(!eventExist) {
				m_eventsDataBase.push_back(event);
				AnnounceEvent(event->GetEventId());
			}

		}
	}

	void AnnouncementLayer::AnnounceEvent(uint32_t eventId) {
		NS_LOG_FUNCTION(this);
		bool announce = false;
		Ptr<EventDetected> event = nullptr;

		for(vector<Ptr<EventDetected>>::iterator it = m_eventsDataBase.begin() ; it != m_eventsDataBase.end() ; it++) {
			if((*it)->GetEventId() == eventId) {
				if((Simulator::Now() - (*it)->GetLastUpdated()) <= Time(MinuetConfig::ANNOUNCEMENT_TIME_LIMIT)) {
					announce = true;
					event = (*it);
				} else {
					m_eventsDataBase.erase(it);
				}

				break;
			}
		}

		if(announce) {
			AnnouncementEventHeader::AnnouncementEventDetectedInfo eventInfo;
			eventInfo.eventId = event->GetEventId();
			eventInfo.fixed = event->IsFixed();
			eventInfo.detectionTime = event->GetDetectionTime().GetNanoSeconds();
			eventInfo.occurrenceTime = event->GetOccurenceTime().GetNanoSeconds();
			eventInfo.duration = event->GetDuration().GetNanoSeconds();
			eventInfo.position = event->GetMobilityModel()->GetPosition();
			eventInfo.velocity = event->GetMobilityModel()->GetVelocity();
			eventInfo.lastUpdated = event->GetLastUpdated().GetNanoSeconds();

			AnnouncementEventHeader announcementEventHeader;
			announcementEventHeader.SetSeq(m_sentCounter++);
			announcementEventHeader.SetDetectorNodeId(m_node->GetId());
			announcementEventHeader.SetEventDetectedInfo(eventInfo);

			Ptr<Packet> packet = Create<Packet>();
			packet->AddHeader(announcementEventHeader);

			m_communicationLayer->SendControlMenssage(packet); // Send Event Detection Message

			m_forwardMessages.push_back(vector<uint64_t> {announcementEventHeader.GetDetectorNodeId(), announcementEventHeader.GetSeq()});

			PrintInLog("Announcement Message Sent: Event #" + to_string(event->GetEventId()) + " DetectorNodeId: #" + to_string(announcementEventHeader.GetDetectorNodeId()) + " Seq: " + to_string(announcementEventHeader.GetSeq()) + " Distance: " + to_string(CalculateDistance(m_mobilityModel->GetPosition(), eventInfo.position)));

			//m_clusteringManager->StartClustering();
			m_monitoringLayer->StartMonitoring(event);

			m_announcementFrequency = Simulator::Schedule(Time(MinuetConfig::ANNOUNCEMENT_INTERVAL), &AnnouncementLayer::AnnounceEvent, this, event->GetEventId());

		}
	}

	bool AnnouncementLayer::CheckAnnouncementZone(Ptr<EventDetected> eventDetected) {
		if((Simulator::Now() - eventDetected->GetDetectionTime()) <= Time(MinuetConfig::ANNOUNCEMENT_DISSEMINATION_LIMIT_TIME))
			return true;
		else
			return false;
	}

	bool AnnouncementLayer::CheckLifeTimeEvent(Ptr<EventDetected> eventDetected) {
		if((eventDetected->GetOccurenceTime() <= Simulator::Now()) && (Simulator::Now() <= (eventDetected->GetOccurenceTime() + eventDetected->GetDuration())))
			return true;
		else
			return false;
	}

	bool AnnouncementLayer::CheckForwardMessage(uint32_t senderId, uint64_t seq) {
		for(auto message : m_forwardMessages) {
			if((message[0] == senderId) && (message[1] == seq))
				return true;
		}
		return false;
	}

	void AnnouncementLayer::PrintInLog(string message) {
		ofstream os;
		os.open (MinuetConfig::LOG_FILE_ANNOUNCEMENT_LAYER.c_str(), ofstream::out | ofstream::app);
		os << Simulator::Now().GetNanoSeconds() << "ns - AnnouncementLayer - Node #" << m_node->GetId() << " Pos (" << m_mobilityModel->GetPosition() <<  ") : " << message << endl;
		os.close();
	}

/*************** END AnnouncementLayerInterface ************************/

} // namespace ns3
