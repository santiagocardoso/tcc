#include "dca-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DCAHeader");

/*****************  DCAHeader  **********************/
NS_OBJECT_ENSURE_REGISTERED(DCAHeader);

TypeId DCAHeader::GetTypeId() {
	static TypeId typeId = TypeId("ns3::DCAHeader").SetParent<Header>()
							.AddConstructor<DCAHeader>();
	return typeId;
}

DCAHeader::DCAHeader() {
	NS_LOG_DEBUG("Class DCAHeader DCAHeader Method");
}

DCAHeader::~DCAHeader() {
	NS_LOG_DEBUG("Class DCAHeader: ~DCAHeader Method");
}

TypeId DCAHeader::GetInstanceTypeId(void) const {
	NS_LOG_DEBUG("Class DCAHeader " << this);
	return GetTypeId();
}

void DCAHeader::Print(std::ostream &os) const {
	NS_LOG_DEBUG("Class DCAHeader " << this);
}

uint32_t DCAHeader::GetSerializedSize(void) const {
	NS_LOG_DEBUG("Class DCAHeader " << this);
	return 0;
}

void DCAHeader::Serialize(Buffer::Iterator start) const {
	NS_LOG_DEBUG("Class DCAHeader " << this);
}

uint32_t DCAHeader::Deserialize(Buffer::Iterator start) {
	NS_LOG_DEBUG("Class DCAHeader " << this);
	return 0;
}
/*****************  DCAHeader END **********************/

/*****************  BeaconHeader2  **********************/
NS_OBJECT_ENSURE_REGISTERED(BeaconHeader2);

TypeId BeaconHeader2::GetTypeId() {
	static TypeId typeId = TypeId("ns3::BeaconHeader2").SetParent<DCAHeader>()
							.AddConstructor<BeaconHeader2>();
	return typeId;
}

BeaconHeader2::BeaconHeader2() {
	NS_LOG_DEBUG("Class BeaconHeader2 " << this);
	m_seq = 0;
}

BeaconHeader2::~BeaconHeader2() {
	NS_LOG_DEBUG("Class BeaconHeader2 " << this);
}

uint64_t BeaconHeader2::GetSeq() const {
	return m_seq;
}
void BeaconHeader2::SetSeq(uint64_t seq) {
	m_seq = seq;
}

void BeaconHeader2::SetVehicleInfo(VehicleInfo info) {
	NS_LOG_DEBUG("Class BeaconHeader2 " << this);
	m_vehicleInfo = info;
}

BeaconHeader2::VehicleInfo BeaconHeader2::GetVehicleInfo() {
	NS_LOG_DEBUG("Class BeaconHeader2 " << this);
	return m_vehicleInfo;
}

TypeId BeaconHeader2::GetInstanceTypeId(void) const {
	NS_LOG_DEBUG("Class: BeaconHeader2 " << this);
	return GetTypeId ();
}

void BeaconHeader2::Print(std::ostream &os) const {
	NS_LOG_DEBUG("Class: BeaconHeader2 " << this);
	os << "(seq = " << m_seq
			<< "ID = " << m_vehicleInfo.id
			<< "IS_CLUSTER_HEAD = " << m_vehicleInfo.isClusterHead
			<< "CLUSTER_HEAD_ID = " << m_vehicleInfo.clusterHeadId
			<< "POSITION = " << m_vehicleInfo.position
			<< "DIRETION = " << m_vehicleInfo.direction << ")";
}

uint32_t BeaconHeader2::GetSerializedSize(void) const {
	NS_LOG_DEBUG("Class: BeaconHeader2 " << this);
	return sizeof(uint64_t) + sizeof(VehicleInfo);
}

void BeaconHeader2::Serialize(Buffer::Iterator start) const {
	NS_LOG_DEBUG("Class: BeaconHeader2 " << this);
	Buffer::Iterator i = start;
	i.WriteHtonU64(m_seq);

	// Write mobility structure
	unsigned char temp[sizeof(VehicleInfo)];
	memcpy(temp, &m_vehicleInfo, sizeof(VehicleInfo));
	i.Write(temp, sizeof(VehicleInfo));
}

uint32_t BeaconHeader2::Deserialize(Buffer::Iterator start) {
	NS_LOG_DEBUG("Class: BeaconHeader2 " << this);

	Buffer::Iterator i = start;
	m_seq = i.ReadNtohU64();

	unsigned char temp[sizeof(VehicleInfo)];
	i.Read(temp, sizeof(VehicleInfo));
	memcpy(&m_vehicleInfo, &temp, sizeof(VehicleInfo) );

	return GetSerializedSize();
}
/*****************  BeaconHeader2 END  **********************/

}
