/*
 * minuet-base-station.cc
 *
 *  Created on: 24 de nov de 2018
 *      Author: everaldo
 */

#include "minuet-base-station.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BaseStation");
NS_OBJECT_ENSURE_REGISTERED (BaseStation);

TypeId BaseStation::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::BaseStation").SetParent<Object>()
						.AddConstructor<BaseStation>()
						.AddAttribute("BaseStationId",
								"The Base Station Id", DoubleValue(),
								MakeUintegerAccessor(&BaseStation::m_baseStationId),
								MakeUintegerChecker<uint32_t>())
						.AddAttribute("PosX",
								"The X Position of Base Station", DoubleValue(),
								MakeDoubleAccessor(&BaseStation::m_pos_x),
								MakeDoubleChecker<double_t>())
						.AddAttribute("PosY",
								"The Y Position of Base Station", DoubleValue(),
								MakeDoubleAccessor(&BaseStation::m_pos_y),
								MakeDoubleChecker<double_t>());
	return tid;
}

TypeId
BaseStation::GetInstanceTypeId(void) const {
	return GetTypeId();
}

BaseStation::BaseStation() {
	NS_LOG_FUNCTION(this);
}

BaseStation::~BaseStation() {
	NS_LOG_FUNCTION(this);
}

uint32_t BaseStation::GetBaseStationId() const {
	return m_baseStationId;
}

void BaseStation::SetBaseStationId(uint32_t baseStationId) {
	m_baseStationId = baseStationId;
}

double_t BaseStation::GetPosX() const {
	return m_pos_x;
}

void BaseStation::SetPosX(double_t posX) {
	m_pos_x = posX;
}

double_t BaseStation::GetPosY() const {
	return m_pos_y;
}

void BaseStation::SetPosY(double_t posY) {
	m_pos_y = posY;
}

/////////////////////////////////////////////////////////////////////////

vector<Ptr<BaseStation>>
BaseStationUtils::BaseStationGenerator() {
	NS_LOG_DEBUG("Class BaseStationUtils: BaseStationGenerator Method");
	vector<Ptr<BaseStation>> baseStationsVector;
	string line;
	ifstream in_file;
	ofstream out_file;
	smatch sm1, sm2;
	string::size_type sz;

	/* initialize random seed: */
	srand (time(NULL));

	regex re1 ("[0-9.]+");

	in_file.open((MinuetConfig::TRACE_BASE_STATIONS_FILE).c_str());
	if(!in_file.good()) {
		cerr << "## ERROR: In BaseStationGenerator method '" << MinuetConfig::TRACE_BASE_STATIONS_FILE << "' Not Found!!" << endl;
		exit(1);
	}

	if(in_file.is_open()){
		while(getline(in_file, line)) {
//				cout << line << endl;

			// Get ID
			regex_search (line,sm1,re1);
			uint32_t baseStationId = stod(sm1[0], &sz);
			line = sm1.suffix().str();

//				cout << "ID: " << baseStationId << endl;

			//Get PosX
			regex_search (line,sm1,re1);
			double_t posX = stod(sm1[0], &sz);
			line = sm1.suffix().str();

//				cout << "PosX: " << posX << endl;

			//Get PosY
			regex_search (line,sm1,re1);
			double_t posY = stod(sm1[0], &sz);
			line = sm1.suffix().str();

//				cout << "PosY: " << posY << endl;

			baseStationsVector.push_back(CreateBaseStation(baseStationId, posX, posY));

		}

		in_file.close();
	}

	return baseStationsVector;

}

Ptr<BaseStation>
BaseStationUtils::CreateBaseStation(uint32_t baseStationId, double_t pos_x, double_t pos_y) {
	NS_LOG_DEBUG("Class BaseStationUtils: CreateBaseStation Method");
	return CreateObjectWithAttributes<BaseStation>("BaseStationId", UintegerValue(baseStationId), "PosX", DoubleValue(pos_x), "PosY", DoubleValue(pos_y));
}

/////////////////////////////////////////////////////////////////////////

NS_OBJECT_ENSURE_REGISTERED (AppBaseStation);

TypeId AppBaseStation::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::AppBaseStation").SetParent<Application>()
			.AddConstructor<AppBaseStation>();

	return tid;
}

AppBaseStation::AppBaseStation() {
	NS_LOG_DEBUG("Class AppBaseStation: AppBaseStation Method");
	m_listenDataSocket = nullptr;
	m_sendControlSocket = nullptr;
	m_sentCounter = 0;
}

AppBaseStation::~AppBaseStation() {
	NS_LOG_DEBUG("Class AppBaseStation: ~AppBaseStation Method");
}

void
AppBaseStation::StartApplication (void) {
	NS_LOG_DEBUG("Class AppBaseStation: StartApplication Method");

	if(!m_sendControlSocket) {
		m_sendControlSocket = Socket::CreateSocket(GetNode(), TypeId::LookupByName(MinuetConfig::LISTEN_DATA_PROTOCOL_BASE_STATION));
		m_sendControlSocket->SetAllowBroadcast(true);
		m_sendControlSocket->ShutdownRecv();
		InetSocketAddress iaddrSendControlSocket = InetSocketAddress (Ipv4Address::GetBroadcast(), MinuetConfig::PORT_CONTROL);
		m_sendControlSocket->Bind();
		m_sendControlSocket->Connect(iaddrSendControlSocket);
		NS_LOG_DEBUG("### SendControlSocket created and configured!");
	} else {
		NS_LOG_INFO("### SendControlSocket already created and configured!");
	}

	if(!m_listenDataSocket) {
		// Configure the Data Listen Socket
		m_listenDataSocket = Socket::CreateSocket(GetNode(), TypeId::LookupByName(MinuetConfig::LISTEN_DATA_PROTOCOL_BASE_STATION));
		m_listenDataSocket->ShutdownSend();
		InetSocketAddress iaddrListenDataSocket = InetSocketAddress (Ipv4Address::GetAny(), MinuetConfig::PORT_DATA);
		m_listenDataSocket->Bind(iaddrListenDataSocket);
		m_listenDataSocket->SetRecvCallback(MakeCallback (&AppBaseStation::ReceiveDataMessage, this));
		NS_LOG_DEBUG("### Base Station Socket created and configured!");
	} else {
		NS_LOG_INFO("### Base Station Socket already created and configured!");
	}

	Simulator::Schedule(Seconds(GetNode()->GetId() * 0.001), &AppBaseStation::SendAnnouncementBaseStationMessage, this);
}

void
AppBaseStation::StopApplication (void) {
	NS_LOG_DEBUG("Class AppBaseStation: StopApplication Method");

}

void AppBaseStation::ReceiveDataMessage(Ptr<Socket> socket) {
	NS_LOG_DEBUG("Class AppBaseStation: ReceiveDataMessage Method");
	Address addr;
	Ptr<Packet> packet = socket->RecvFrom(addr);

	MonitoringEventHeader monitoringEventHeader;
	packet->RemoveHeader(monitoringEventHeader);

	NS_LOG_INFO("Base Station #" << GetNode()->GetId() << " Received one Data Message From " << InetSocketAddress::ConvertFrom (addr).GetIpv4 ());

	PrintInLog("Monitoring Message Received:" \
				" From = " + to_string(monitoringEventHeader.GetNodeId()) +
				" Status = " + to_string(monitoringEventHeader.GetStatusNode()) +
				" RelayId = " + to_string(monitoringEventHeader.GetRelayId()) +
				" MonitorId = " + to_string(monitoringEventHeader.GetMonitorId()) +
				" FrameId = " + to_string(monitoringEventHeader.GetFrameId()) +
				" FrameType = " + string(1, monitoringEventHeader.GetFrameType()) +
				" Seq = " + to_string(monitoringEventHeader.GetSeq()) +
				" EventId = " + to_string(monitoringEventHeader.GetEventId()));

	if(!MessageIsRedundant(monitoringEventHeader.GetSeq(), monitoringEventHeader.GetEventId(), monitoringEventHeader.GetMonitorId())) {
		PrintInDump(monitoringEventHeader.GetSeq(), packet->GetSize(), monitoringEventHeader.GetEventId(), monitoringEventHeader.GetMonitorId());
		m_deliveredMessages.push_back(vector<uint64_t> {monitoringEventHeader.GetSeq(), monitoringEventHeader.GetEventId(), monitoringEventHeader.GetMonitorId()});
	}
}

void AppBaseStation::SendAnnouncementBaseStationMessage() {
	NS_LOG_DEBUG("Class AppBaseStation: SendAnnouncementBaseStationMessage Method");

	AnnouncementBaseStationHeader::AnnouncementBaseStationInfo announcementBaseStationInfo;
	announcementBaseStationInfo.payload = 0;

	AnnouncementBaseStationHeader announcementBaseStationHeader;
	announcementBaseStationHeader.SetSeq(m_sentCounter++);
	announcementBaseStationHeader.SetAnnouncementBaseStationInfo(announcementBaseStationInfo);

	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(announcementBaseStationHeader);

	m_sendControlSocket->Send(packet);

	Ptr<Ipv4> ipv4 = GetNode()->GetObject<Ipv4> ();
	Ipv4InterfaceAddress iaddr = ipv4->GetAddress(1,0);
	NS_LOG_DEBUG("### Base Station #" << GetNode()->GetId() << " IP (" << iaddr.GetLocal() << ") Sending Announcement Message");

	Simulator::Schedule(Time(MinuetConfig::ANNOUNCEMENT_BASE_STATION_INTERVAL), &AppBaseStation::SendAnnouncementBaseStationMessage, this);
}

void AppBaseStation::PrintInLog(string message) {
	ofstream os;
	os.open (MinuetConfig::LOG_FILE_BASE_STATIONS.c_str(), ofstream::out | ofstream::app);
	os << Simulator::Now().GetNanoSeconds() << "ns - BASE STATION - Node #" << m_node->GetId() << ": " << message << endl;
	os.close();
}

void AppBaseStation::PrintInDump(uint64_t pkt_id, uint32_t pkt_size, uint32_t event_id, uint32_t node_id) {
		ofstream os;
		os.setf(std::ios::fixed, std::ios::floatfield);	// floatfield set to fixed
		os.precision(3);

		os.open ((MinuetConfig::RD_FILE_PATH + MinuetConfig::RD_FILE_NAME + to_string(node_id) + "_" + to_string(event_id)).c_str(), ofstream::out | ofstream::app);
		os << Simulator::Now().GetSeconds() << " id " << pkt_id << " udp " << pkt_size << endl;
		os.close();
}

bool AppBaseStation::MessageIsRedundant(uint64_t seq, uint32_t eventId, uint32_t monitorId) {
	NS_LOG_FUNCTION(this);
	for(auto message : m_deliveredMessages) {
		if((message[0] == seq) && (message[1] == eventId) && (message[2] == monitorId))
			return true;
	}
	return false;
}

} // namespace ns3

