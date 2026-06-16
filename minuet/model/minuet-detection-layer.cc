/*
 * minuet-detection-layer.cc
 *
 *  Created on: 18 de out de 2018
 *      Author: everaldo
 */

#include "minuet-detection-layer.h"

namespace ns3 {

	NS_LOG_COMPONENT_DEFINE ("DetectionLayer");


/*************** DetectionLayerInterface ************************/
	NS_OBJECT_ENSURE_REGISTERED (DetectionLayerInterface);

	DetectionLayerInterface::DetectionLayerInterface() {
		NS_LOG_FUNCTION(this);
	}

	DetectionLayerInterface::~DetectionLayerInterface() {
		NS_LOG_FUNCTION(this);
	}

	void DetectionLayerInterface::AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer) {
		DetectionLayerInterface::m_communicationLayer = comunicationLayer;
		DetectionLayerInterface::m_communicationLayer->AttachDetectionLayerInterface(this);
	}

/*************** AND DetectionLayerInterface **********************/

/******************** DetectionLayer ***************************/
	NS_OBJECT_ENSURE_REGISTERED (DetectionLayer);

	TypeId DetectionLayer::GetTypeId(void) {
		static TypeId tid =
				TypeId("ns3::DetectionLayer")
						.SetParent<Object>()
						.AddConstructor<DetectionLayer>()
						.AddAttribute("Node",
								"The Node of the Node", PointerValue(),
								MakePointerAccessor(&DetectionLayer::m_node),
								MakePointerChecker<Node>())
						.AddAttribute("CommunicationLayer",
								"The Communication Instance", PointerValue(),
								MakePointerAccessor(&DetectionLayer::m_communicationLayer),
								MakePointerChecker<CommunicationLayer>())
						/*.AddAttribute("ClusteringManager",
								"The Announcement Layer Instance", PointerValue(),
								MakePointerAccessor(&DetectionLayer::m_clusteringManager),
								MakePointerChecker<AnnouncementLayer>())*/;
		return tid;
	}

	DetectionLayer::DetectionLayer() {
		//NS_LOG_FUNCTION(this);
		m_isStarted = false;
	}

	DetectionLayer::~DetectionLayer() {
		NS_LOG_FUNCTION(this);
	}

	void DetectionLayer::StartLayer() {
		//NS_LOG_FUNCTION(this);
		if(!m_isStarted) {
			if(/*m_announcementLayer &&*/ m_communicationLayer && m_node) {
				m_mobilityModel = m_node->GetObject<MobilityModel>();
				AddCommunicationLayer(m_communicationLayer);
				CheckEventOccurence();

				m_clusteringManager = CreateObjectWithAttributes<ClusteringManager>("Node", PointerValue(m_node), "CommunicationLayer", PointerValue(m_communicationLayer));

				m_monitoringLayer = CreateObjectWithAttributes<MonitoringLayer>("Node", PointerValue(m_node), "CommunicationLayer", PointerValue(m_communicationLayer),
																			"ClusteringManager", PointerValue(m_clusteringManager));
				m_monitoringLayer->StartLayer();

				m_clusteringManager->StartClustering();

				m_isStarted = true;
				NS_LOG_DEBUG("### DetectionLayer started!");
			} else {
				//NS_LOG_WARN("### Impossible to Start DetectionLayer: AnnouncementLayer Or CommunicationLayer Or Node is Null");
				NS_LOG_WARN("### Impossible to Start DetectionLayer: CommunicationLayer Or Node is Null");
				exit(EXIT_FAILURE);
			}
		} else {
			//NS_LOG_INFO("### DetectionLayer already started!");
		}
	}

	void DetectionLayer::StopLayer() {
		//NS_LOG_FUNCTION(this);
		m_checkOccurrenceEventId.Cancel();
		m_monitoringLayer->StopLayer();
		m_clusteringManager->StopClustering();
		m_clusteringManager->Dispose();
		m_monitoringLayer->Dispose();
	}

	void DetectionLayer::SetEventsOccurenceVector(vector<Ptr<EventDetected>> eventsOccurenceVector) {
		m_eventsOccurenceVector = eventsOccurenceVector;
	}

	void DetectionLayer::DetectsEvent(Ptr<EventDetected> event) {
		//NS_LOG_FUNCTION(this);
		NS_LOG_INFO("Node #" << m_node->GetId() << " in PosX: " << m_mobilityModel->GetPosition().x << " PosY: " << m_mobilityModel->GetPosition().y << " Detected Event");

		event->SetDetectionTime(Simulator::Now());
		event->SetLastUpdated(Simulator::Now());

		bool alreadyDetected = (find(m_eventsDetectedIdVector.begin(), m_eventsDetectedIdVector.end(), event->GetEventId()) != m_eventsDetectedIdVector.end());

		double_t distance = sqrt(pow((m_mobilityModel->GetPosition().x - event->GetMobilityModel()->GetPosition().x), 2) + pow((m_mobilityModel->GetPosition().y - event->GetMobilityModel()->GetPosition().y), 2));
		if(!alreadyDetected) {
			PrintInLog("Event (" + std::to_string(event->GetEventId()) + ") Detected - Distance " + std::to_string(distance));
			m_eventsDetectedIdVector.push_back(event->GetEventId());
		} else
			PrintInLog("Vehicle continues detecting Event (" + std::to_string(event->GetEventId()) + ") Detected - Distance " + std::to_string(distance));

		//m_announcementLayer->TryAnnounceEvent(event);
		m_monitoringLayer->StartMonitoring(event);
	}

	void DetectionLayer::receiveControlMessage(Ptr<Packet> packet, Address addr) {
		//NS_LOG_FUNCTION(this);
		/* Empty */
	}

	void DetectionLayer::CheckEventOccurence() {
		//NS_LOG_FUNCTION(this);

//		///***** Create Velocities Log ****///
//		PrintInVelocitiesLog();
//		m_checkOccurrenceEventId = Simulator::Schedule(Time(MinuetConfig::CHECK_OCCURRENCE_EVENT_INTERVAL), &DetectionLayer::CheckEventOccurence, this);
//		//**********************************//


		if(!m_eventsOccurenceVector.empty()) {
			for(auto event : m_eventsOccurenceVector) {
				if((event->GetOccurenceTime() <= Simulator::Now()) && (Simulator::Now() <= (event->GetOccurenceTime() + event->GetDuration()))) {

					if(IsInFieldVision(event)) {

//						
//						PrintInLog("Event Detected");
//				

						DetectsEvent(event);
					} else {
						for(vector<uint32_t>::iterator it = m_eventsDetectedIdVector.begin() ; it != m_eventsDetectedIdVector.end() ; it++) {
							if((*it) == event->GetEventId()) {
								m_eventsDetectedIdVector.erase(it);
								break;
							}
						}
					}

				} else {
					for(vector<uint32_t>::iterator it = m_eventsDetectedIdVector.begin() ; it != m_eventsDetectedIdVector.end() ; it++) {
						if((*it) == event->GetEventId()) {
							m_eventsDetectedIdVector.erase(it);
							break;
						}
					}
				}
			}

			m_checkOccurrenceEventId = Simulator::Schedule(Time(MinuetConfig::CHECK_OCCURRENCE_EVENT_INTERVAL), &DetectionLayer::CheckEventOccurence, this);
		} else {
			cerr << "### Error: No event define to simulation" << endl;
			exit(EXIT_FAILURE);
		}
	}

	bool DetectionLayer::IsInFieldVision(Ptr<EventDetected> event) {
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

	void DetectionLayer::PrintInLog(string message) {
		ofstream os;
		os.open (MinuetConfig::LOG_FILE_DETECTION_LAYER.c_str(), ofstream::out | ofstream::app);
		os << Simulator::Now().GetNanoSeconds() << "ns - DetectionLayer - Node #" << m_node->GetId() << " Pos (" << m_mobilityModel->GetPosition() <<  ") : " << message << endl;
		os.close();
		return; //removi LOG
	}

	void DetectionLayer::PrintInVelocitiesLog() {
		//ofstream os;
		//os.open (MinuetConfig::LOG_FILE_VELOCITIES.c_str(), ofstream::out | ofstream::app);
		//os << Simulator::Now().GetNanoSeconds() << "ns - DetectionLayer - Node #" << m_node->GetId()
		//		<< " Pos (" << m_mobilityModel->GetPosition() << ")"
		//		<< " Vel (" << m_mobilityModel->GetVelocity() << ")"
		//		<< " |Vel| = " << m_mobilityModel->GetVelocity().GetLength() << " m/s" << endl;
		//os.close();
		return; //removi LOG
	}

/******************** END DetectionLayer ***************************/

} // namespace ns3
