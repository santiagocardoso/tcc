/*
 * minuet-detection-layer.h
 *
 *  Created on: 18 de out de 2018
 *      Author: everaldo
 */

#ifndef MINUET_DETECTION_LAYER_H_
#define MINUET_DETECTION_LAYER_H_

#include <math.h>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "minuet-event.h"
#include "minuet-utils.h"
#include "minuet-announcement-layer.h"
#include "minuet-communication-layer.h"

using namespace std;

namespace ns3 {

class AnnouncementLayer;
class CommunicationLayer;

class DetectionLayerInterface : public Object {
public:
	DetectionLayerInterface();
	virtual ~DetectionLayerInterface();
	virtual void receiveControlMessage(Ptr<Packet> packet, Address addr) = 0;

	void AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer);

protected:
	Ptr<CommunicationLayer> m_communicationLayer;
};

class DetectionLayer : public DetectionLayerInterface {
public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	DetectionLayer();
	~DetectionLayer();
	void StartLayer();
	void StopLayer();

	void SetEventsOccurenceVector(vector<Ptr<EventDetected>> eventsOccurenceVector);

	void DetectsEvent(Ptr<EventDetected> event);

private:
	virtual void receiveControlMessage(Ptr<Packet> packet, Address addr);
	void PrintInLog(string message);
	void PrintInVelocitiesLog();

	void CheckEventOccurence();
	bool IsInFieldVision(Ptr<EventDetected> event);

	bool m_isStarted;
	Ptr<Node> m_node;
	Ptr<MobilityModel> m_mobilityModel;
	//Ptr<AnnouncementLayer> m_announcementLayer;
	Ptr<MonitoringLayer> m_monitoringLayer;
	Ptr<ClusteringManager> m_clusteringManager;

	vector<uint32_t> m_eventsDetectedIdVector;

	vector<Ptr<EventDetected>> m_eventsOccurenceVector;
	EventId m_checkOccurrenceEventId;
};

} // namespace ns3

#endif /* MINUET_DETECTION_LAYER_H_ */
