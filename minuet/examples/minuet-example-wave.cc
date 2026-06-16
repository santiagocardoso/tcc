/*
 * minuet-example-wave.cc
 *
 *  Created on: 4 de nov de 2018
 *      Author: everaldo
 */

#include <string>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/wave-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("MINUETExampleWave");

//static bool
//Receive (Ptr<NetDevice> dev, Ptr<const Packet> pkt, uint16_t mode, const Address &sender)
//{
//  SeqTsHeader seqTs;
//  pkt->PeekHeader (seqTs);
//  std::cout << "receive a packet: " << std::endl
//            << "  sequence = " << seqTs.GetSeq () << "," << std::endl
//            << "  sendTime = " << seqTs.GetTs ().GetSeconds () << "s," << std::endl
//            << "  recvTime = " << Now ().GetSeconds () << "s," << std::endl
//            << "  protocol = 0x" << std::hex << mode << std::dec  << std::endl;
//  return true;
//}

//static bool
//ReceiveVsa (Ptr<const Packet> pkt,const Address & address, uint32_t, uint32_t) {
//  std::cout << "receive a VSA management frame: recvTime = " << Now ().GetSeconds () << "s." << std::endl;
//  return true;
//}

//int main (int argc, char *argv[]) {
//
//	NodeContainer nodes = NodeContainer ();
//	nodes.Create (2);
//
//	NetDeviceContainer devices; ///< the devices
//
//	MobilityHelper mobility;
//	Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator> ();
//	positionAlloc->Add (Vector (0.0, 0.0, 0.0));
//	positionAlloc->Add (Vector (5.0, 0.0, 0.0));
//	mobility.SetPositionAllocator (positionAlloc);
//	mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
//	mobility.Install (nodes);
//
//	YansWifiChannelHelper waveChannel = YansWifiChannelHelper::Default ();
//	YansWavePhyHelper wavePhy =  YansWavePhyHelper::Default ();
//	wavePhy.SetChannel (waveChannel.Create ());
//	wavePhy.SetPcapDataLinkType (WifiPhyHelper::DLT_IEEE802_11);
//	QosWaveMacHelper waveMac = QosWaveMacHelper::Default ();
//	WaveHelper waveHelper = WaveHelper::Default ();
//	devices = waveHelper.Install (wavePhy, waveMac, nodes);
//
//	for (uint32_t i = 0; i != devices.GetN (); ++i) {
//	  Ptr<WaveNetDevice> device = DynamicCast<WaveNetDevice> (devices.Get (i));
//	  device->SetReceiveCallback (MakeCallback (&Receive));
//	  device->SetWaveVsaCallback (MakeCallback  (&ReceiveVsa));
//	}
//
//	return EXIT_SUCCESS;
//}

void ReceivePacket (Ptr<Socket> socket) {
	Address addr;
	Ptr<Packet> packet = socket->RecvFrom(addr);
	NS_LOG_INFO("Node #" << socket->GetNode()->GetId() << " Received one packet from " << InetSocketAddress::ConvertFrom (addr).GetIpv4 ());
}

static void SendPacket (Ptr<Socket> socket, uint32_t pktCount, Time pktInterval) {
	Address addr;

	Ptr<Ipv4> ipv4 = socket->GetNode()->GetObject<Ipv4> ();
	Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1,0);

	NS_LOG_INFO("Node #" << socket->GetNode()->GetId() << " IP (" << iaddr.GetLocal() << ") Sending Packet");
	if (pktCount > 0) {
		socket->Send (Create<Packet> (0));
		Simulator::Schedule (pktInterval, &SendPacket, socket, pktCount - 1, pktInterval);
	} else {
		socket->Close ();
	}
}

int main (int argc, char *argv[]) {

	CommandLine cmd;
	cmd.Parse (argc, argv);

	LogLevel logLevel = (LogLevel) (LOG_PREFIX_ALL | LOG_LEVEL_FUNCTION);
	LogComponentEnable("MINUETExampleWave", logLevel);

	uint32_t totalNodes = 3;
	double_t transmInterval = 1.0;	// Transmission Interval
	uint32_t totalPacket = 20;		// Total packet transmitted

	// Create nodes
	NodeContainer nodes;
	nodes.Create (totalNodes);
	NS_LOG_INFO ("Nodes Created");

	/////////// Configure Mobility  ////////////////////
	MobilityHelper mobility;
	mobility.SetPositionAllocator ("ns3::GridPositionAllocator",
			"MinX", DoubleValue (0.0),
			"MinY", DoubleValue (0.0),
			"DeltaX", DoubleValue (50.0),
			"DeltaY", DoubleValue (100.0),
			"GridWidth", UintegerValue (30),
			"LayoutType", StringValue ("RowFirst"));

	mobility.SetMobilityModel ("ns3::RandomWalk2dMobilityModel",
			"Bounds", RectangleValue (Rectangle (-500, 500, -500, 500)));
	mobility.Install (nodes);
	/////////////////////////////////////////////////


	/***************** Configure Channel *********************/
	/////////////////////// Wifi Channel //////////////////////////
	string phyMode ("OfdmRate6MbpsBW10MHz");
	YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
	Ptr<YansWifiChannel> channel = wifiChannel.Create ();

	YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
	wifiPhy.SetChannel (channel);
	wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);
	wifiPhy.Set ("TxPowerStart", DoubleValue(32));
	wifiPhy.Set ("TxPowerEnd", DoubleValue(32));
	wifiPhy.Set ("TxGain", DoubleValue(12));
	wifiPhy.Set ("RxGain", DoubleValue(12));
	wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue(-61.8));
	wifiPhy.Set ("CcaMode1Threshold", DoubleValue(-64.8));

	NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default ();
	Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();

	wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
			"DataMode",StringValue (phyMode),
			"ControlMode",StringValue (phyMode));
	NetDeviceContainer wifiDevices1 = wifi80211p.Install (wifiPhy, wifi80211pMac, nodes);
	/////////////////////// Wifi Channel END //////////////////////////

	/////////////////////// Wired Channel Unsing CSMA //////////////////////////
//	uint32_t MTUChannel = 1400;
//	DataRate dataRateChannel = DataRate (5000000);
//	TimeValue delayChannel = TimeValue(MilliSeconds (2));
//	CsmaHelper csma;
//	csma.SetChannelAttribute ("DataRate", DataRateValue (dataRateChannel));
//	csma.SetChannelAttribute ("Delay", delayChannel);
//	csma.SetDeviceAttribute ("Mtu", UintegerValue (MTUChannel));
//	NetDeviceContainer wifiDevices1 = csma.Install (nodes);
	/////////////////////// Wired Channel Unsing CSMA END //////////////////////

	/*****************************************************************************/

	InternetStackHelper internet;
	internet.Install (nodes);
	NS_LOG_INFO ("Internet Stack Configured");

	// Assign IP Address
	Ipv4AddressHelper ipv4;
	ipv4.SetBase ("10.1.1.0", "255.255.25.0");
	Ipv4InterfaceContainer i = ipv4.Assign (wifiDevices1);
	NS_LOG_INFO ("IPs Configured");

	uint16_t port = 4477;
	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");

	//////////////  CREATE SOCKET IN NODE 0  ////////////////////////
	Ptr<Socket> socketSend0 = Socket::CreateSocket (nodes.Get (0), tid);
	InetSocketAddress remote = InetSocketAddress (Ipv4Address::GetBroadcast(), port);
	socketSend0->SetAllowBroadcast(true);
	socketSend0->ShutdownRecv();
	socketSend0->Bind();
	socketSend0->Connect (remote);

	//Schedule SendPacket
	Simulator::ScheduleWithContext (socketSend0->GetNode ()->GetId(), Seconds(1.0), &SendPacket, socketSend0, totalPacket, Seconds(transmInterval));
	/////////////////////////////////////////////////////////////////

	//////////////  CREATE SOCKET IN NODE 1  ////////////////////////
	Ptr<Socket> socketListen1 = Socket::CreateSocket (nodes.Get(1), tid);
	InetSocketAddress iaddrList1 = InetSocketAddress (Ipv4Address::GetAny (), port);
	socketListen1->ShutdownSend();
	socketListen1->Bind (iaddrList1);
	socketListen1->Listen();
	socketListen1->SetRecvCallback (MakeCallback (&ReceivePacket));
	///////////////////////////////////////////////////////////////

	//////////////   CREATE SOCKETS IN NODE 2   ////////////////////////
	Ptr<Socket> socketListen2 = Socket::CreateSocket(nodes.Get(2), tid);
	InetSocketAddress iaddr2 = InetSocketAddress(Ipv4Address::GetAny(), port);
	socketListen2->ShutdownSend();
	socketListen2->Bind(iaddr2);
	socketListen2->Listen();
	socketListen2->SetRecvCallback (MakeCallback (&ReceivePacket));
	////////////////////////////////////////////////////////////////

	// RUN Simulation
	Simulator::Run ();
	Simulator::Destroy ();

	return EXIT_SUCCESS;
}
