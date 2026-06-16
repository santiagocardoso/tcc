/*
 * minuet-header.cc
 *
 *  Created on: 18 de out de 2018
 *      Author: everaldo
 */

#include "minuet-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MINUETHeader");


/*****************  MINUETHeader  **********************/
NS_OBJECT_ENSURE_REGISTERED(MINUETHeader);

TypeId MINUETHeader::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::MINUETHeader").SetParent<Header>()
			.AddConstructor<MINUETHeader>();

	return tid;
}

MINUETHeader::MINUETHeader(){
	NS_LOG_DEBUG("Class: MINUETHeader " << this);
}
MINUETHeader::~MINUETHeader(){
	NS_LOG_DEBUG("Class: MINUETHeader " << this);
}

TypeId MINUETHeader::GetInstanceTypeId(void) const {
	NS_LOG_DEBUG("Class: MINUETHeader " << this);
	return GetTypeId ();
}

void MINUETHeader::Print(std::ostream &os) const {
	NS_LOG_DEBUG("Class: MINUETHeader " << this);
}

uint32_t MINUETHeader::GetSerializedSize(void) const {
	NS_LOG_DEBUG("Class: MINUETHeader " << this);
	return 0; // MINUETHeader should never serialize objects
}

void MINUETHeader::Serialize(Buffer::Iterator start) const {
	NS_LOG_DEBUG("Class: MINUETHeader " << this);
}

uint32_t MINUETHeader::Deserialize(Buffer::Iterator start) {
	NS_LOG_DEBUG("Class: MINUETHeader " << this);
	return 0; // MINUETHeader should never deserialize objects
}
/*****************  MINUETHeader END **********************/

/*****************  AnnouncementEventHeader  **********************/
NS_OBJECT_ENSURE_REGISTERED(AnnouncementEventHeader);

TypeId AnnouncementEventHeader::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::AnnouncementEventHeader").SetParent<MINUETHeader>()
			.AddConstructor<AnnouncementEventHeader>();

	return tid;
}

AnnouncementEventHeader::AnnouncementEventHeader(){
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);
	m_seq = 0;
}
AnnouncementEventHeader::~AnnouncementEventHeader(){
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);
}

AnnouncementEventHeader::AnnouncementEventDetectedInfo AnnouncementEventHeader::GetEventDetectedInfo() const {
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);
	return m_eventDetectedInfo;
}

void AnnouncementEventHeader::SetEventDetectedInfo(AnnouncementEventDetectedInfo eventDetectedInfo) {
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);
	m_eventDetectedInfo = eventDetectedInfo;
}

uint64_t AnnouncementEventHeader::GetSeq() const {
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);
	return m_seq;
}

void AnnouncementEventHeader::SetSeq(uint64_t seq) {
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);
	m_seq = seq;
}

uint32_t AnnouncementEventHeader::GetDetectorNodeId() const {
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);
	return m_detectorNodeId;
}

void AnnouncementEventHeader::SetDetectorNodeId(uint32_t detectorNodeId) {
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);
	m_detectorNodeId = detectorNodeId;
}

TypeId AnnouncementEventHeader::GetInstanceTypeId(void) const {
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);
	return GetTypeId ();
}

void AnnouncementEventHeader::Print(std::ostream &os) const {
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);
	os << "(seq = " << m_seq
			<< "EVENT ID = " << m_eventDetectedInfo.eventId
			<< "IS FIXED = " << m_eventDetectedInfo.fixed
			<< "STEP TIME = " << m_eventDetectedInfo.detectionTime
			<< "DURATION = " << m_eventDetectedInfo.duration
			<< "POSITION = " << m_eventDetectedInfo.position
			<< "VELOCITY = " << m_eventDetectedInfo.velocity << ")";
}

uint32_t AnnouncementEventHeader::GetSerializedSize(void) const {
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);
	return sizeof(uint64_t) + sizeof(uint32_t) + sizeof(AnnouncementEventDetectedInfo);
}

void AnnouncementEventHeader::Serialize(Buffer::Iterator start) const {
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);
	Buffer::Iterator i = start;
	i.WriteHtonU64(m_seq);
	i.WriteHtonU32(m_detectorNodeId);

	unsigned char temp2[sizeof(AnnouncementEventDetectedInfo)];
	memcpy( temp2, &m_eventDetectedInfo, sizeof(AnnouncementEventDetectedInfo));
	i.Write(temp2, sizeof(AnnouncementEventDetectedInfo));
}

uint32_t AnnouncementEventHeader::Deserialize(Buffer::Iterator start) {
	NS_LOG_DEBUG("Class: AnnouncementEventHeader " << this);

	Buffer::Iterator i = start;
	m_seq = i.ReadNtohU64();
	m_detectorNodeId = i.ReadNtohU32();

	unsigned char temp2[sizeof(AnnouncementEventDetectedInfo)];
	i.Read(temp2, sizeof(AnnouncementEventDetectedInfo));
	memcpy(&m_eventDetectedInfo, &temp2, sizeof(AnnouncementEventDetectedInfo) );

	return GetSerializedSize();
}

/*****************  AnnouncementEventHeader END  **********************/

/*****************  MonitoringEventHeader  **********************/
NS_OBJECT_ENSURE_REGISTERED(MonitoringEventHeader);

TypeId MonitoringEventHeader::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::MonitoringEventHeader").SetParent<MINUETHeader>()
			.AddConstructor<MonitoringEventHeader>();

	return tid;
}

MonitoringEventHeader::MonitoringEventHeader(){
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	m_seq = 0;
}
MonitoringEventHeader::~MonitoringEventHeader(){
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
}

uint64_t MonitoringEventHeader::GetSeq() const {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	return m_seq;
}
void MonitoringEventHeader::SetSeq(uint64_t seq) {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	m_seq = seq;
}

uint32_t MonitoringEventHeader::GetMonitorId() const {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	return m_monitorId;
}
void MonitoringEventHeader::SetMonitorId(uint32_t monitorId) {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	m_monitorId = monitorId;
}

uint32_t MonitoringEventHeader::GetRelayId() const {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	return m_relayId;
}
void MonitoringEventHeader::SetRelayId(uint32_t relayId) {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	m_relayId = relayId;
}

uint32_t MonitoringEventHeader::GetNodeId() const {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	return m_nodeId;
}
void MonitoringEventHeader::SetNodeId(uint32_t nodeId) {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	m_nodeId = nodeId;
}

MonitoringEventHeader::StatusNode MonitoringEventHeader::GetStatusNode() const {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	return m_statusNode;
}
void MonitoringEventHeader::SetStatusNode(StatusNode statusNode) {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	m_statusNode = statusNode;
}

uint32_t MonitoringEventHeader::GetEventId() const {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	return m_eventId;
}
void MonitoringEventHeader::SetEventId(uint32_t eventId) {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	m_eventId = eventId;
}

uint64_t MonitoringEventHeader::GetMonitoringTime() const {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	return m_monitoringTime;
}
void MonitoringEventHeader::SetMonitoringTime(uint64_t monitoringTime) {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	m_monitoringTime = monitoringTime;
}

uint32_t MonitoringEventHeader::GetFrameId() const {
	return m_frameId;
}
void MonitoringEventHeader::SetFrameId(uint32_t frameId) {
	m_frameId = frameId;
}

char32_t MonitoringEventHeader::GetFrameType() const {
	return m_frameType;
}
void MonitoringEventHeader::SetFrameTye(char32_t frameType) {
	m_frameType = frameType;
}

TypeId MonitoringEventHeader::GetInstanceTypeId(void) const {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	return GetTypeId ();
}

void MonitoringEventHeader::Print(std::ostream &os) const {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	os << "(SEQ = " << m_seq << endl
			<< "MONITOR ID = " << m_monitorId << endl
			<< "RELAY ID = " << m_relayId << endl
			<< "NODE ID = " << m_nodeId << endl
			<< "STATUS NODE = " << m_statusNode << endl
			<< "EVENT ID = " << m_eventId << endl
			<< "MONITORING _TIME = " << m_monitoringTime << endl
			<< "FRAME_ID = " << m_frameId << endl
			<< "FRAME_TYPE" << m_frameType << ")";
}

uint32_t MonitoringEventHeader::GetSerializedSize(void) const {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	return sizeof(uint64_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint32_t) + sizeof(char32_t) + sizeof(StatusNode);
}

void MonitoringEventHeader::Serialize(Buffer::Iterator start) const {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);
	Buffer::Iterator i = start;
	i.WriteHtonU64(m_seq);
	i.WriteHtonU32(m_monitorId);
	i.WriteHtonU32(m_relayId);
	i.WriteHtonU32(m_nodeId);
	i.WriteHtonU32(m_eventId);
	i.WriteHtonU64(m_monitoringTime);
	i.WriteHtonU32(m_frameId);
	i.WriteHtonU32(m_frameType);

	unsigned char temp[sizeof(StatusNode)];
	memcpy( temp, &m_statusNode, sizeof(StatusNode));
	i.Write(temp, sizeof(StatusNode));
}

uint32_t MonitoringEventHeader::Deserialize(Buffer::Iterator start) {
	NS_LOG_DEBUG("Class: MonitoringEventHeader " << this);

	Buffer::Iterator i = start;
	m_seq = i.ReadNtohU64();
	m_monitorId = i.ReadNtohU32();
	m_relayId = i.ReadNtohU32();
	m_nodeId = i.ReadNtohU32();
	m_eventId = i.ReadNtohU32();
	m_monitoringTime = i.ReadNtohU64();
	m_frameId = i.ReadNtohU32();
	m_frameType = i.ReadNtohU32();

	unsigned char temp[sizeof(StatusNode)];
	i.Read(temp, sizeof(StatusNode));
	memcpy(&m_statusNode, &temp, sizeof(StatusNode));

	return GetSerializedSize();
}
/*****************  MonitoringEventHeader END  **********************/

/*****************  AnnouncementBaseStationHeader  **********************/
NS_OBJECT_ENSURE_REGISTERED(AnnouncementBaseStationHeader);

TypeId AnnouncementBaseStationHeader::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::AnnouncementBaseStationHeader").SetParent<MINUETHeader>()
			.AddConstructor<AnnouncementBaseStationHeader>();

	return tid;
}

AnnouncementBaseStationHeader::AnnouncementBaseStationHeader(){
	NS_LOG_DEBUG("Class: AnnouncementBaseStationHeader " << this);
	m_seq = 0;
}
AnnouncementBaseStationHeader::~AnnouncementBaseStationHeader(){
	NS_LOG_DEBUG("Class: AnnouncementBaseStationHeader " << this);
}

AnnouncementBaseStationHeader::AnnouncementBaseStationInfo AnnouncementBaseStationHeader::GetAnnouncementBaseStationInfo() const {
	NS_LOG_DEBUG("Class: AnnouncementBaseStationHeader " << this);
	return m_announcementBaseStationInfo;
}

void AnnouncementBaseStationHeader::SetAnnouncementBaseStationInfo(AnnouncementBaseStationInfo announcementBaseStationInfo) {
	NS_LOG_DEBUG("Class: AnnouncementBaseStationHeader " << this);
	m_announcementBaseStationInfo = announcementBaseStationInfo;
}

uint64_t AnnouncementBaseStationHeader::GetSeq() const {
	NS_LOG_DEBUG("Class: AnnouncementBaseStationHeader " << this);
	return m_seq;
}

void AnnouncementBaseStationHeader::SetSeq(uint64_t seq) {
	NS_LOG_DEBUG("Class: AnnouncementBaseStationHeader " << this);
	m_seq = seq;
}

TypeId AnnouncementBaseStationHeader::GetInstanceTypeId(void) const {
	NS_LOG_DEBUG("Class: AnnouncementBaseStationHeader " << this);
	return GetTypeId ();
}

void AnnouncementBaseStationHeader::Print(std::ostream &os) const {
	NS_LOG_DEBUG("Class: AnnouncementBaseStationHeader " << this);
	os << "(seq = " << m_seq
			<< "PAYLOAD = " << m_announcementBaseStationInfo.payload <<")";
}

uint32_t AnnouncementBaseStationHeader::GetSerializedSize(void) const {
	NS_LOG_DEBUG("Class: AnnouncementBaseStationHeader " << this);
	return sizeof(uint64_t) + sizeof(AnnouncementBaseStationInfo);
}

void AnnouncementBaseStationHeader::Serialize(Buffer::Iterator start) const {
	NS_LOG_DEBUG("Class: AnnouncementBaseStationHeader " << this);
	Buffer::Iterator i = start;
	i.WriteHtonU64(m_seq);

	unsigned char temp[sizeof(AnnouncementBaseStationInfo)];
	memcpy( temp, &m_announcementBaseStationInfo, sizeof(AnnouncementBaseStationInfo));
	i.Write(temp, sizeof(AnnouncementBaseStationInfo));
}

uint32_t AnnouncementBaseStationHeader::Deserialize(Buffer::Iterator start) {
	NS_LOG_DEBUG("Class: AnnouncementBaseStationHeader " << this);

	Buffer::Iterator i = start;
	m_seq = i.ReadNtohU64();

	unsigned char temp[sizeof(AnnouncementBaseStationInfo)];
	i.Read(temp, sizeof(AnnouncementBaseStationInfo));
	memcpy(&m_announcementBaseStationInfo, &temp, sizeof(AnnouncementBaseStationInfo) );

	return GetSerializedSize();
}
/*****************  AnnouncementEventHeader END  **********************/

} // namespace ns3
