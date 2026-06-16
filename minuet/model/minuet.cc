/*
 * minuet.cc
 *
 *  Created on: 6 de nov de 2018
 *      Author: everaldo
 */

#include "minuet.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MINUET");

/*************** MINUETInterface ************************/
	NS_OBJECT_ENSURE_REGISTERED (MINUETInterface);

	MINUETInterface::MINUETInterface() {
		NS_LOG_FUNCTION(this);
	}

	MINUETInterface::~MINUETInterface() {
		NS_LOG_FUNCTION(this);
	}

	void MINUETInterface::AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer) {
		MINUETInterface::m_communicationLayer = comunicationLayer;
		MINUETInterface::m_communicationLayer->AttachMINUETInterface(this);
	}
/*************** END MINUETInterface **********************/

/******************** MINUET ***************************/
	NS_OBJECT_ENSURE_REGISTERED (MINUET);

	TypeId MINUET::GetTypeId(void) {
		static TypeId tid =
				TypeId("ns3::MINUET").SetParent<MINUETInterface>()
				.AddConstructor<MINUET>()
				.AddAttribute("DetectionPhase",
						"The Detection Phase Instance of the Node", PointerValue(),
						MakePointerAccessor(&MINUET::m_detectionLayer),
						MakePointerChecker<DetectionLayer>())
				.AddAttribute("CommunicationLayer",
						"The Communication Layer", PointerValue(),
						MakePointerAccessor(&MINUET::m_communicationLayer),
						MakePointerChecker<CommunicationLayer>())
				.AddAttribute("Node",
						"The Node of the MINUET", PointerValue(),
						MakePointerAccessor(&MINUET::m_node),
						MakePointerChecker<Node>());
		return tid;
	}

	MINUET::MINUET() {
		NS_LOG_FUNCTION (this);
	}

	MINUET::~MINUET() {
		NS_LOG_FUNCTION (this);
	}

	void MINUET::Start(vector<Ptr<EventDetected>> eventsOccurenceVector) {
		NS_LOG_FUNCTION(this);

		bool isNodeEvent = false; // flag to define if m_node is the eventNode

		for (uint32_t i = 0;i < eventsOccurenceVector.size();i++){
			if (!eventsOccurenceVector[i]->IsFixed()){
				isNodeEvent |= (m_node->GetId() == eventsOccurenceVector[i]->GetNodeEventId()) ? true : false;
			}
		}

		m_eventsEventId = vector<EventId>(eventsOccurenceVector.size());

		m_mobilityModel =  m_node->GetObject<MobilityModel>();


		if (!isNodeEvent){
			// Create and Initialize communication

			m_communicationLayer = CreateObjectWithAttributes<CommunicationLayer>("Node", PointerValue(m_node));

			m_communicationLayer->StartLayer();
			AddCommunicationLayer(m_communicationLayer);

			// Create And Initialize Announcement Layer
			//m_announcementLayer = CreateObjectWithAttributes<AnnouncementLayer>("Node", PointerValue(m_node), "CommunicationLayer", PointerValue(m_communicationLayer));
			//m_announcementLayer->StartLayer(); // Start Announcement Layer

			//m_clusteringManager = CreateObjectWithAttributes<ClusteringManager>("Node", PointerValue(m_node), "CommunicationLayer", PointerValue(m_communicationLayer));

			//m_clusteringManager->StartClustering();


			// Create And Initialize Detection Layer
			m_detectionLayer = CreateObjectWithAttributes<DetectionLayer>("Node", PointerValue(m_node), "CommunicationLayer", PointerValue(m_communicationLayer)/*,
																		  "AnnouncementLayer", PointerValue(m_announcementLayer)*/);
			m_detectionLayer->SetEventsOccurenceVector(eventsOccurenceVector);
			m_detectionLayer->StartLayer(); // Start Detection Layer
		}
//		for(uint32_t i = 0 ; i < eventDetectedVector.size() ; i++) {
//			NS_LOG_DEBUG("Scheduling event to " << eventDetectedVector[i]->GetStepTime() << " in Node #" << m_node->GetId());
//			m_eventsEventId[i] = Simulator::Schedule(Seconds (eventDetectedVector[i]->GetStepTime() - Simulator::Now().GetSeconds()), &MINUET::DetectsEvent, this, eventDetectedVector[i]);
//
//			PrintInLog("Scheduling Event #" + boost::uuids::to_string(eventDetectedVector[i]->GetUuidEvent()) + " to " + std::to_string(eventDetectedVector[i]->GetStepTime()));
//
//		}

		PrintInLog("MINUET Started");
	}

	void MINUET::Stop() {
		NS_LOG_FUNCTION(this);
		for(uint32_t i = 0 ; i < m_eventsEventId.size() ; i++) {
			if(!m_eventsEventId[i].IsExpired())
				m_eventsEventId[i].Cancel();
		}

		if((m_detectionLayer != nullptr) && /*(m_announcementLayer != nullptr) && */(m_communicationLayer != nullptr)){
			m_detectionLayer->StopLayer();
			//m_announcementLayer->StopLayer();
			m_communicationLayer->StopLayer();

			m_detectionLayer->Dispose();
			//m_announcementLayer->Dispose();
			m_communicationLayer->Dispose();

			PrintInLog("MINUET Stopped");
		}
	}

	void MINUET::PrintInLog(string message) {
		ofstream os;
		os.open (MinuetConfig::LOG_FILE_MINUET.c_str(), ofstream::out | ofstream::app);
		os << Simulator::Now().GetNanoSeconds() << "ns - MINUET - Node #" << m_node->GetId() << " Pos (" << m_mobilityModel->GetPosition() <<  ") : " << message << endl;
		os.close();
	}
/*************** END MINUET ************************/

} // Namespace ns3
