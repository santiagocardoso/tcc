#include "pctt-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("PCTTHeader");

/*****************  PCTTHeader  **********************/
NS_OBJECT_ENSURE_REGISTERED(PCTTHeader);

TypeId PCTTHeader::GetTypeId() {
	static TypeId typeId = TypeId("ns3::PCTTHeader")
							.SetParent<Header>()
							.AddConstructor<PCTTHeader>();
	return typeId;
}

PCTTHeader::PCTTHeader() {
	NS_LOG_DEBUG("Class PCTTHeader PCTTHeader Method");
}

PCTTHeader::~PCTTHeader() {
	NS_LOG_DEBUG("Class PCTTHeader: ~PCTTHeader Method");
}

TypeId PCTTHeader::GetInstanceTypeId(void) const {
	NS_LOG_DEBUG("Class PCTTHeader " << this);
	return GetTypeId();
}

void PCTTHeader::Print(std::ostream &os) const {
	NS_LOG_DEBUG("Class PCTTHeader " << this);
}

uint32_t PCTTHeader::GetSerializedSize(void) const {
	NS_LOG_DEBUG("Class PCTTHeader " << this);
	return 0;
}

void PCTTHeader::Serialize(Buffer::Iterator start) const {
	NS_LOG_DEBUG("Class PCTTHeader " << this);
}

uint32_t PCTTHeader::Deserialize(Buffer::Iterator start) {
	NS_LOG_DEBUG("Class PCTTHeader " << this);
	return 0;
}
/*****************  PCTTHeader END **********************/

/*****************  CHMHeader  **********************/
NS_OBJECT_ENSURE_REGISTERED(CHMHeader);

TypeId CHMHeader::GetTypeId() {
	static TypeId typeId = TypeId("ns3::CHMHeader")
							.SetParent<PCTTHeader>()
							.AddConstructor<CHMHeader>();
	return typeId;
}

CHMHeader::CHMHeader() {
	NS_LOG_DEBUG("Class CHMHeader " << this);
	m_seq = 0;
}

CHMHeader::~CHMHeader() {
	NS_LOG_DEBUG("Class CHMHeader " << this);
}

uint64_t CHMHeader::GetSeq() const {
	return m_seq;
}
void CHMHeader::SetSeq(uint64_t seq) {
	m_seq = seq;
}

void CHMHeader::SetVehicleInfo(VehicleInfo info) {
	NS_LOG_DEBUG("Class CHMHeader " << this);
	m_vehicleInfo = info;
}

CHMHeader::VehicleInfo CHMHeader::GetVehicleInfo() {
	NS_LOG_DEBUG("Class CHMHeader " << this);
	return m_vehicleInfo;
}

TypeId CHMHeader::GetInstanceTypeId(void) const {
	NS_LOG_DEBUG("Class: CHMHeader " << this);
	return GetTypeId ();
}

void CHMHeader::Print(std::ostream &os) const {
	NS_LOG_DEBUG("Class: CHMHeader " << this);
	os << "(seq = " << m_seq
			<< "NODE_ID = " << m_vehicleInfo.nodeId
			<< "PACKET_ID = " << m_vehicleInfo.packetId
			<< "POSITION = " << m_vehicleInfo.position
			<< "VELOCITY = " << m_vehicleInfo.velocity
			<< "CURRENT_TIME = " << m_vehicleInfo.currentTime
			<< "RESIGN_TIME = " << m_vehicleInfo.resignTime
			<< "OBSERVATION_TIME = " << m_vehicleInfo.observationTime << ")";
}

uint32_t CHMHeader::GetSerializedSize(void) const {
	NS_LOG_DEBUG("Class: CHMHeader " << this);
	return sizeof(uint64_t) + sizeof(VehicleInfo);
}

void CHMHeader::Serialize(Buffer::Iterator start) const {
	NS_LOG_DEBUG("Class: CHMHeader " << this);
	Buffer::Iterator i = start;
	i.WriteHtonU64(m_seq);

	// Write mobility structure
	unsigned char temp[sizeof(VehicleInfo)];
	memcpy( temp, &m_vehicleInfo, sizeof(VehicleInfo));
	i.Write(temp, sizeof(VehicleInfo));
}

uint32_t CHMHeader::Deserialize(Buffer::Iterator start) {
	NS_LOG_DEBUG("Class: CHMHeader " << this);

	Buffer::Iterator i = start;
	m_seq = i.ReadNtohU64();

	unsigned char temp[sizeof(VehicleInfo)];
	i.Read(temp, sizeof(VehicleInfo));
	memcpy(&m_vehicleInfo, &temp, sizeof(VehicleInfo) );

	return GetSerializedSize();
}
/*****************  CHMHeader END  **********************/

/*****************  CMMHeader  **********************/
NS_OBJECT_ENSURE_REGISTERED(CMMHeader);

TypeId CMMHeader::GetTypeId() {
	static TypeId typeId = TypeId("ns3::CMMHeader")
							.SetParent<PCTTHeader>()
							.AddConstructor<CMMHeader>();
	return typeId;
}

CMMHeader::CMMHeader() {
	NS_LOG_DEBUG("Class CMMHeader " << this);
	m_seq = 0;
}

CMMHeader::~CMMHeader() {
	NS_LOG_DEBUG("Class CMMHeader " << this);
}

uint64_t CMMHeader::GetSeq() const {
	return m_seq;
}
void CMMHeader::SetSeq(uint64_t seq) {
	m_seq = seq;
}

void CMMHeader::SetVehicleInfo(VehicleInfo info) {
	NS_LOG_DEBUG("Class CMMHeader " << this);
	m_vehicleInfo = info;
}

CMMHeader::VehicleInfo CMMHeader::GetVehicleInfo() {
	NS_LOG_DEBUG("Class CMMHeader " << this);
	return m_vehicleInfo;
}

TypeId CMMHeader::GetInstanceTypeId(void) const {
	NS_LOG_DEBUG("Class: CMMHeader " << this);
	return GetTypeId ();
}

void CMMHeader::Print(std::ostream &os) const {
	NS_LOG_DEBUG("Class: CMMHeader " << this);
	os << "(seq = " << m_seq
			<< "NODE_ID = " << m_vehicleInfo.nodeId
			<< "PACKET_ID = " << m_vehicleInfo.packetId
			<< "TARGET_DETECTION_VALUE = " << m_vehicleInfo.targetDetectionValue
			<< "CURRENT_TIME = " << m_vehicleInfo.currentTime
			<< "POSITION = " << m_vehicleInfo.position
			<< "OBSERVATION_TIME = " << m_vehicleInfo.observationTime
			<< "PREDICTION_DENIAL = " << m_vehicleInfo.predictionDenial << ")";
}

uint32_t CMMHeader::GetSerializedSize(void) const {
	NS_LOG_DEBUG("Class: CMMHeader " << this);
	return sizeof(uint64_t) + sizeof(VehicleInfo);
}

void CMMHeader::Serialize(Buffer::Iterator start) const {
	NS_LOG_DEBUG("Class: CMMHeader " << this);
	Buffer::Iterator i = start;
	i.WriteHtonU64(m_seq);

	// Write mobility structure
	unsigned char temp[sizeof(VehicleInfo)];
	memcpy( temp, &m_vehicleInfo, sizeof(VehicleInfo));
	i.Write(temp, sizeof(VehicleInfo));
}

uint32_t CMMHeader::Deserialize(Buffer::Iterator start) {
	NS_LOG_DEBUG("Class: CMMHeader " << this);

	Buffer::Iterator i = start;
	m_seq = i.ReadNtohU64();

	unsigned char temp[sizeof(VehicleInfo)];
	i.Read(temp, sizeof(VehicleInfo));
	memcpy(&m_vehicleInfo, &temp, sizeof(VehicleInfo) );

	return GetSerializedSize();
}
/*****************  CMMHeader END  **********************/

/*****************  JackMHeader  **********************/
NS_OBJECT_ENSURE_REGISTERED(JackMHeader);

TypeId JackMHeader::GetTypeId() {
	static TypeId typeId = TypeId("ns3::JackMHeader")
							.SetParent<PCTTHeader>()
							.AddConstructor<JackMHeader>();
	return typeId;
}

JackMHeader::JackMHeader() {
	NS_LOG_DEBUG("Class JackMHeader " << this);
	m_seq = 0;
}

JackMHeader::~JackMHeader() {
	NS_LOG_DEBUG("Class JackMHeader " << this);
}

uint64_t JackMHeader::GetSeq() const {
	return m_seq;
}
void JackMHeader::SetSeq(uint64_t seq) {
	m_seq = seq;
}

void JackMHeader::SetVehicleInfo(VehicleInfo info) {
	NS_LOG_DEBUG("Class JackMHeader " << this);
	m_vehicleInfo = info;
}

JackMHeader::VehicleInfo JackMHeader::GetVehicleInfo() {
	NS_LOG_DEBUG("Class JackMHeader " << this);
	return m_vehicleInfo;
}

TypeId JackMHeader::GetInstanceTypeId(void) const {
	NS_LOG_DEBUG("Class: JackMHeader " << this);
	return GetTypeId ();
}

void JackMHeader::Print(std::ostream &os) const {
	NS_LOG_DEBUG("Class: JackMHeader " << this);
	os << "(seq = " << m_seq
			<< "NODE_ID = " << m_vehicleInfo.nodeId
			<< "PACKET_ID = " << m_vehicleInfo.packetId
			<< "NONMEMBER_ID = " << m_vehicleInfo.nonMemberId
			<< ")";
}

uint32_t JackMHeader::GetSerializedSize(void) const {
	NS_LOG_DEBUG("Class: JackMHeader " << this);
	return sizeof(uint64_t) + sizeof(VehicleInfo);
}

void JackMHeader::Serialize(Buffer::Iterator start) const {
	NS_LOG_DEBUG("Class: JackMHeader " << this);
	Buffer::Iterator i = start;
	i.WriteHtonU64(m_seq);

	// Write mobility structure
	unsigned char temp[sizeof(VehicleInfo)];
	memcpy( temp, &m_vehicleInfo, sizeof(VehicleInfo));
	i.Write(temp, sizeof(VehicleInfo));
}

uint32_t JackMHeader::Deserialize(Buffer::Iterator start) {
	NS_LOG_DEBUG("Class: JackMHeader " << this);

	Buffer::Iterator i = start;
	m_seq = i.ReadNtohU64();

	unsigned char temp[sizeof(VehicleInfo)];
	i.Read(temp, sizeof(VehicleInfo));
	memcpy(&m_vehicleInfo, &temp, sizeof(VehicleInfo) );

	return GetSerializedSize();
}
/*****************  JackMHeader END  **********************/

}
