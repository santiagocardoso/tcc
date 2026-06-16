/*
 * minuet-communication-layer.h
 *
 *  Created on: 6 de nov de 2018
 *      Author: everaldo
 */

#ifndef MINUET_COMMUNICATION_LAYER_H_
#define MINUET_COMMUNICATION_LAYER_H_

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "minuet-utils.h"
#include "minuet.h"
#include "minuet-detection-layer.h"
#include "minuet-announcement-layer.h"
#include "minuet-monitoring-layer.h"
#include "dca.h"
#include "pctt.h"
#include "social.h"

namespace ns3 {

class MINUETInterface;
class DetectionLayerInterface;
class AnnouncementLayerInterface;
class MonitoringLayerInterface;
class DCAInterface;
class PCTTInterface;
class SOCIALInterface;

class CommunicationLayer : public Object, Singleton<CommunicationLayer> {
public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	CommunicationLayer();
	virtual ~CommunicationLayer();

	void StartLayer();
	void StopLayer();
	void SendControlMenssage(Ptr<Packet> packet);
	void SendDataMenssage(Ptr<Packet> packet);

	void AttachMINUETInterface(Ptr<MINUETInterface> minuetInterface);
	void AttachDetectionLayerInterface(Ptr<DetectionLayerInterface> detectionLayerInterface);
	void AttachAnnouncementLayerInterface(Ptr<AnnouncementLayerInterface> announcementLayerInterface);
	void AttachMonitoringLayerInterface(Ptr<MonitoringLayerInterface> monitoringLayerInterface);
	void AttachDCAInterface(Ptr<DCAInterface> dcaInterface);
	void AttachPCTTInterface(Ptr<PCTTInterface> pcttInterface);
	void AttachSOCIALInterface(Ptr<SOCIALInterface> socialInterface);

private:

	void ReceiveControlMessage(Ptr<Socket> socket);
	void ReceiveDataMessage(Ptr<Socket> socket);

	Ptr<Node> m_node;

	Ptr<Socket> m_sendControlSocket;
	Ptr<Socket> m_listenControlSocket;

	Ptr<Socket> m_sendDataSocket;
	Ptr<Socket> m_listenDataSocket;

	Ptr<MINUETInterface> m_minuetInterface;
	Ptr<DetectionLayerInterface> m_detectionLayerInterface;
	Ptr<AnnouncementLayerInterface> m_announcementLayerInterface;
	Ptr<MonitoringLayerInterface> m_monitoringLayerInterface;
	Ptr<DCAInterface> m_dcaInterface;
	Ptr<PCTTInterface> m_pcttInterface;
	Ptr<SOCIALInterface> m_socialInterface;
};

} // namespace ns3

#endif /* MINUET_COMMUNICATION_H_ */
