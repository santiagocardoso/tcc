/*
 * minuet.h
 *
 *  Created on: 6 de nov de 2018
 *      Author: everaldo
 */

#ifndef NS_3_28_SRC_MINUET_MODEL_MINUET_H_
#define NS_3_28_SRC_MINUET_MODEL_MINUET_H_

#include <string>
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "minuet-event.h"
#include "minuet-communication-layer.h"
#include "minuet-detection-layer.h"
#include "minuet-announcement-layer.h"

namespace ns3 {

class CommunicationLayer;
class DetectionLayer;
class AnnouncementLayer;

class MINUETInterface : public Object {
public:
	MINUETInterface();
	virtual ~MINUETInterface();

	void AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer);

protected:
	Ptr<CommunicationLayer> m_communicationLayer;
};

class MINUET : public MINUETInterface {
public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	MINUET();
	virtual ~MINUET();

	void Start(vector<Ptr<EventDetected>> eventsOccurenceVector);
	void Stop();

private:
	void DetectsEvent(Ptr<EventDetected> event);

	void PrintInLog(string message);

	Ptr<Node> m_node;
	Ptr<MobilityModel> m_mobilityModel;
	Ptr<DetectionLayer> m_detectionLayer;
	//Ptr<AnnouncementLayer> m_announcementLayer;
	//Ptr<ClusteringManager> m_clusteringManager;

	vector<EventId> m_eventsEventId;
};

}

#endif /* NS_3_28_SRC_MINUET_MODEL_MINUET_H_ */
