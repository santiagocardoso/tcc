/*
 * minuet-communication-layer.cc
 *
 *  Created on: 6 de nov de 2018
 *      Author: everaldo
 */

#include "minuet-communication-layer.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CommunicationLayer");
NS_OBJECT_ENSURE_REGISTERED (CommunicationLayer);

TypeId CommunicationLayer::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::CommunicationLayer").SetParent<Object>()
						.AddConstructor<CommunicationLayer>()
						.AddAttribute("Node",
								"The Node of the Socket", PointerValue(),
								MakePointerAccessor(&CommunicationLayer::m_node),
								MakePointerChecker<Node>());
	return tid;
}

CommunicationLayer::CommunicationLayer() {
	NS_LOG_FUNCTION (this);
}

CommunicationLayer::~CommunicationLayer() {
	NS_LOG_FUNCTION (this);

	m_sendControlSocket = nullptr;
	m_listenControlSocket = nullptr;
	m_sendDataSocket = nullptr;
	m_listenDataSocket = nullptr;
}

void CommunicationLayer::StartLayer() {
	NS_LOG_FUNCTION (this);

	if(!m_sendControlSocket) {
		// Configure the Control Send Socket
		m_sendControlSocket = Socket::CreateSocket(m_node, TypeId::LookupByName(MinuetConfig::SEND_CONTROL_PROTOCOL));
		m_sendControlSocket->SetAllowBroadcast(true);
		m_sendControlSocket->ShutdownRecv();
		InetSocketAddress iaddrSendControlSocket = InetSocketAddress (Ipv4Address::GetBroadcast(), MinuetConfig::PORT_CONTROL);
		m_sendControlSocket->Bind();
		m_sendControlSocket->Connect(iaddrSendControlSocket);
		NS_LOG_DEBUG("### SendControlSocket created and configured!");
	} else {
		NS_LOG_INFO("### SendControlSocket already created and configured!");
	}

	if(!m_listenControlSocket) {
		// Configure the Control Listen Socket
		m_listenControlSocket = Socket::CreateSocket(m_node, TypeId::LookupByName(MinuetConfig::LISTEN_CONTROL_PROTOCOL));
		m_listenControlSocket->ShutdownSend();
		InetSocketAddress iaddrListenControlSocket = InetSocketAddress (Ipv4Address::GetAny(), MinuetConfig::PORT_CONTROL);
		m_listenControlSocket->Bind(iaddrListenControlSocket);
		m_listenControlSocket->SetRecvCallback(MakeCallback (&CommunicationLayer::ReceiveControlMessage, this));
		NS_LOG_DEBUG("### ListenControlSocket created and configured!");
	} else {
		NS_LOG_INFO("### ListenControlSocket already created and configured!");
	}

	if(!m_sendDataSocket) {
		// Configure the Data Send Socket
		m_sendDataSocket = Socket::CreateSocket(m_node, TypeId::LookupByName(MinuetConfig::SEND_DATA_PROTOCOL));
		m_sendDataSocket->SetAllowBroadcast(true);
		m_sendDataSocket->ShutdownRecv();
		InetSocketAddress iaddrSendDataSocket = InetSocketAddress (Ipv4Address::GetBroadcast(), MinuetConfig::PORT_DATA);
		m_sendDataSocket->Connect(iaddrSendDataSocket);
		NS_LOG_DEBUG("### SendDataSocket created and configured!");
	} else {
		NS_LOG_INFO("### SendDataSocket already created and configured!");
	}

	if(!m_listenDataSocket) {
		// Configure the Data Listen Socket
		m_listenDataSocket = Socket::CreateSocket(m_node, TypeId::LookupByName(MinuetConfig::LISTEN_DATA_PROTOCOL));
		m_listenDataSocket->ShutdownSend();
		InetSocketAddress iaddrListenDataSocket = InetSocketAddress (Ipv4Address::GetAny(), MinuetConfig::PORT_DATA);
		m_listenDataSocket->Bind(iaddrListenDataSocket);
		m_listenDataSocket->SetRecvCallback(MakeCallback (&CommunicationLayer::ReceiveDataMessage, this));
		NS_LOG_DEBUG("### ListenDataSocket created and configured!");
	} else {
		NS_LOG_INFO("### ListenDataSocket already created and configured!");
	}
}

void CommunicationLayer::StopLayer() {
	NS_LOG_FUNCTION (this);

	if(m_sendControlSocket) {
		m_sendControlSocket->Close();
		m_sendControlSocket = nullptr;
		NS_LOG_DEBUG("### SendControlSocket closed!");
	} else {
		NS_LOG_INFO("### SendControlSocket not created!");
	}

	if(m_listenControlSocket) {
		m_listenControlSocket->Close();
		m_listenControlSocket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
		m_listenControlSocket = nullptr;
		NS_LOG_DEBUG("### ListenControlSocket closed!");
	} else {
		NS_LOG_INFO("### ListenControlSocket not created!");
	}

	if(m_sendDataSocket) {
		m_sendDataSocket->Close();
		m_sendDataSocket = nullptr;
		NS_LOG_DEBUG("### SendDataSocket closed!");
	} else {
		NS_LOG_INFO("### SendDataSocket not created!");
	}

	if(m_listenDataSocket) {
		m_listenDataSocket->Close();
		m_listenDataSocket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket>>());
		m_listenDataSocket = nullptr;
		NS_LOG_DEBUG("### ListenDataSocket closed!");
	} else {
		NS_LOG_INFO("### ListenDataSocket not created!");
	}
}

void CommunicationLayer::SendControlMenssage(Ptr<Packet> packet) {
	//NS_LOG_FUNCTION(this);
	Ptr<Ipv4> ipv4;
	Ipv4InterfaceAddress iaddr;

	if(m_sendControlSocket) {
		m_sendControlSocket->Send(packet);
		ipv4 = m_node->GetObject<Ipv4> ();
		iaddr = ipv4->GetAddress(1,0);

		//NS_LOG_DEBUG("### SendControlSocket in Node #" << m_node->GetId() << " IP (" << iaddr.GetLocal() << ") Sending Packet");
	} else {
		//NS_LOG_INFO("### Impossible send control message!! ListenControlSocket not created!");
	}
}

void CommunicationLayer::SendDataMenssage(Ptr<Packet> packet) {
	//NS_LOG_FUNCTION(this);
	Ptr<Ipv4> ipv4;
	Ipv4InterfaceAddress iaddr;

	if(m_sendDataSocket) {
		m_sendDataSocket->Send(packet);
		ipv4 = m_node->GetObject<Ipv4> ();
		iaddr = ipv4->GetAddress(1,0);

		//NS_LOG_DEBUG("### SendDataSocket in Node #" << m_node->GetId() << " IP (" << iaddr.GetLocal() << ") Sending Packet");
	} else {
		//NS_LOG_INFO("### Impossible send data message!! ListenDataSocket not created!");
	}
}

void CommunicationLayer::ReceiveControlMessage(Ptr<Socket> socket) {
	//NS_LOG_FUNCTION (this << socket);
	Address addr;
	Ptr<Packet> packet;

	while(packet = socket->RecvFrom(addr)) {
		//NS_LOG_INFO("Node #" << m_node->GetId() << " Received Control Message From " << InetSocketAddress::ConvertFrom (addr).GetIpv4 ());
		if(packet->GetSize() == 0) //EOF
			break;

		PacketMetadata::ItemIterator metadataIterator = packet->BeginItem();
		PacketMetadata::Item item;

		while (metadataIterator.HasNext()){
			item = metadataIterator.Next();

			// Check type id of the header
			if(item.tid.GetName() == "ns3::AnnouncementEventHeader") {
				m_announcementLayerInterface->receiveControlMessage(packet, addr);  // Message regarding the Announcement Layer
			} else if((item.tid.GetName() == "ns3::BeaconHeader") && m_dcaInterface) {
				m_dcaInterface->ReceiveControlMessage(packet, addr);
			} else if((item.tid.GetName() == "ns3::CHMHeader") && m_pcttInterface) {
				m_pcttInterface->ReceiveCHMessage(packet, addr);
			} else if((item.tid.GetName() == "ns3::JackMHeader") && m_pcttInterface) {
				m_pcttInterface->ReceiveJAckMessage(packet, addr);
			} else if((item.tid.GetName() == "ns3::CMMHeader") && m_pcttInterface) {
				m_pcttInterface->ReceiveCMMessage(packet, addr);
			} else if((item.tid.GetName() == "ns3::BeaconHeader") && m_socialInterface) {
				m_socialInterface->ReceiveControlMessage(packet, addr);
			} else if((item.tid.GetName() == "ns3::AnnouncementBaseStationHeader") && m_monitoringLayerInterface) {
				m_monitoringLayerInterface->ReceiveBaseStationAnnouncementMessage(packet, addr);
			}
		}
	}
}

void CommunicationLayer::ReceiveDataMessage(Ptr<Socket> socket) {
	//NS_LOG_FUNCTION (this << socket);
	Address addr;
	Ptr<Packet> packet;

	while(packet = socket->RecvFrom(addr)) {
		//NS_LOG_INFO("Node #" << m_node->GetId() << " Received one Data Message From " << InetSocketAddress::ConvertFrom (addr).GetIpv4 ());
		if (packet->GetSize() == 0) // EOF
			break;

		PacketMetadata::ItemIterator metadataIterator = packet->BeginItem();
		PacketMetadata::Item item;

		item = metadataIterator.Next(); // Get Header

		// Check type id of the header
		if((item.tid.GetName() == "ns3::MonitoringEventHeader") && m_monitoringLayerInterface) {
			m_monitoringLayerInterface->ReceiveDataMessage(packet, addr);
		}

//		while (metadataIterator.HasNext()){
//			item = metadataIterator.Next();
//
//			// Check type id of the header
//			if((item.tid.GetName() == "ns3::MonitoringEventHeader") && m_monitoringLayerInterface) {
//				m_monitoringLayerInterface->ReceiveDataMessage(packet, addr);
//			}
//		}
	}
}

void CommunicationLayer::AttachMINUETInterface(Ptr<MINUETInterface> minuetInterface) {
	m_minuetInterface = minuetInterface;
}

void CommunicationLayer::AttachDetectionLayerInterface(Ptr<DetectionLayerInterface> detectionLayerInterface) {
	m_detectionLayerInterface = detectionLayerInterface;
}

void CommunicationLayer::AttachAnnouncementLayerInterface(Ptr<AnnouncementLayerInterface> announcementLayerInterface) {
	m_announcementLayerInterface = announcementLayerInterface;
}

void CommunicationLayer::AttachMonitoringLayerInterface(Ptr<MonitoringLayerInterface> monitoringLayerInterface) {
	m_monitoringLayerInterface = monitoringLayerInterface;
}

void CommunicationLayer::AttachDCAInterface(Ptr<DCAInterface> dcaInterface) {
	m_dcaInterface = dcaInterface;
}

void CommunicationLayer::AttachPCTTInterface(Ptr<PCTTInterface> pcttInterface) {
	m_pcttInterface = pcttInterface;
}

void CommunicationLayer::AttachSOCIALInterface(Ptr<SOCIALInterface> socialInterface) {
	m_socialInterface = socialInterface;
}
} // namespace ns3
