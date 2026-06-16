#include "social-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SOCIALHeader");

/*****************  SOCIALHeader  **********************/
NS_OBJECT_ENSURE_REGISTERED(SOCIALHeader);

TypeId SOCIALHeader::GetTypeId() {
	static TypeId typeId = TypeId("ns3::SOCIALHeader").SetParent<Header>()
							.AddConstructor<SOCIALHeader>();
	return typeId;
}

SOCIALHeader::SOCIALHeader() {
	NS_LOG_DEBUG("Class SOCIALHeader SOCIALHeader Method");
}

SOCIALHeader::~SOCIALHeader() {
	NS_LOG_DEBUG("Class SOCIALHeader: ~SOCIALHeader Method");
}

TypeId SOCIALHeader::GetInstanceTypeId(void) const {
	NS_LOG_DEBUG("Class SOCIALHeader " << this);
	return GetTypeId();
}

void SOCIALHeader::Print(std::ostream &os) const {
	NS_LOG_DEBUG("Class SOCIALHeader " << this);
}

uint32_t SOCIALHeader::GetSerializedSize(void) const {
	NS_LOG_DEBUG("Class SOCIALHeader " << this);
	return 0;
}

void SOCIALHeader::Serialize(Buffer::Iterator start) const {
	NS_LOG_DEBUG("Class SOCIALHeader " << this);
}

uint32_t SOCIALHeader::Deserialize(Buffer::Iterator start) {
	NS_LOG_DEBUG("Class SOCIALHeader " << this);
	return 0;
}
/*****************  SOCIALHeader END **********************/

/*****************  BeaconHeader  **********************/
NS_OBJECT_ENSURE_REGISTERED(BeaconHeader);

TypeId BeaconHeader::GetTypeId() {
	static TypeId typeId = TypeId("ns3::BeaconHeader").SetParent<SOCIALHeader>()
							.AddConstructor<BeaconHeader>();
	return typeId;
}

BeaconHeader::BeaconHeader() {
	NS_LOG_DEBUG("Class BeaconHeader " << this);
	m_seq = 0;
}

BeaconHeader::~BeaconHeader() {
	NS_LOG_DEBUG("Class BeaconHeader " << this);
}

uint64_t BeaconHeader::GetSeq() const {
	return m_seq;
}
void BeaconHeader::SetSeq(uint64_t seq) {
	m_seq = seq;
}

uint16_t BeaconHeader::GetLenNeighborsList() const {
	return lenNeighborsList;
}
void BeaconHeader::SetLenNeighborsList(uint16_t lenNeighborList) {
	lenNeighborsList = lenNeighborList;
}

/*std::vector<uint32_t> BeaconHeader::GetPathList() const{
	NS_LOG_DEBUG("Class: BeaconHeader" << this);
	return pathList;
}

void BeaconHeader::SetPathList(std::vector<uint32_t> list){
	NS_LOG_DEBUG("Class: BeaconHeader" << this);
	pathList = list;
}*/

std::vector<std::vector<uint16_t>> BeaconHeader::GetNeighborsList() const{
	NS_LOG_DEBUG("Class: BeaconHeader" << this);
	return neighborsList;
}
void BeaconHeader::SetNeighborsList(std::vector<std::vector<uint16_t>> list){
	NS_LOG_DEBUG("Class: BeaconHeader" << this);
	neighborsList = list;
}

void BeaconHeader::SetVehicleInfo(VehicleInfo info) {
	NS_LOG_DEBUG("Class BeaconHeader " << this);
	m_vehicleInfo = info;
}

BeaconHeader::VehicleInfo BeaconHeader::GetVehicleInfo() {
	NS_LOG_DEBUG("Class BeaconHeader " << this);
	return m_vehicleInfo;
}

TypeId BeaconHeader::GetInstanceTypeId(void) const {
	NS_LOG_DEBUG("Class: BeaconHeader " << this);
	return GetTypeId ();
}

void BeaconHeader::Print(std::ostream &os) const {
	NS_LOG_DEBUG("Class: BeaconHeader " << this);
	os << "(seq = " << m_seq
			<< "ID = " << m_vehicleInfo.id
			<< "IS_RELAY = " << m_vehicleInfo.isRelay
			<< "RELAY_ID = " << m_vehicleInfo.relayId
			<< "POSITION = " << m_vehicleInfo.position
			//<< "DIRETION = " << m_vehicleInfo.direction
			<< "IS_PERFIL_SOCIAL = " << m_vehicleInfo.isPerfilSocial
			<< "IS_GATEWAY = " << m_vehicleInfo.isGateway

			/////////STR/////////
			<< "DEGREE = " << m_vehicleInfo.score
			<< "CLOSENESS = " << m_vehicleInfo.closenessScore
			<< "EIGENVECTOR = " << m_vehicleInfo.eigenScore

			<< "ATIVIDADE SOCIAL = " << m_vehicleInfo.atividadeSocial
			//<< "AMIZADE = " << m_vehicleInfo.amizade
			<< ")";
}

uint32_t BeaconHeader::GetSerializedSize(void) const {
	time_t ini, end;

	time(&ini);
	//NS_LOG_DEBUG("Class: BeaconHeader " << this);
	//uint32_t nElem = 0;

	//for (uint32_t i = 0; i<neighborsList.size(); i++){
	//	nElem = nElem + neighborsList[i][1];
	//}

	//nElem = neighborsList.size()*3;
	//std::cout << "nElem " << nElem << std::endl;

	//uint32_t valor = sizeof(uint64_t) + sizeof(uint16_t) + sizeof(VehicleInfo) + sizeof(uint64_t) + (neighborsList.size() * (sizeof(uint64_t))) + (nElem * sizeof(uint16_t));

	//std::cout << "Valor " << valor << std::endl;

	time(&end);

	//double tempo = end - ini;

	//std::cout << "A função GetSerializeSize foi executada em " << tempo << " segundos.\n";

	//return valor;
	//return sizeof(uint64_t) + sizeof(uint16_t) + sizeof(VehicleInfo);
	return sizeof(uint64_t) + sizeof(uint16_t) + sizeof(VehicleInfo) + sizeof(uint64_t) + sizeof(std::vector<std::vector<uint32_t>>) + (neighborsList.size() * 3 * (sizeof(uint64_t)));
}

void BeaconHeader::Serialize(Buffer::Iterator start) const {

	time_t ini, end;

	time(&ini);

	//NS_LOG_DEBUG("Class: BeaconHeader " << this);


	//uint16_t pathList[3];
	uint32_t teste = 0;

	Buffer::Iterator i = start;

	//std::cout << "Inicio " << teste << std::endl;

	i.WriteHtonU64(m_seq);
	teste = teste + sizeof(m_seq);

	//std::cout << "m_seq " << teste << std::endl;

	i.WriteHtonU16(lenNeighborsList);
	teste = teste + sizeof(lenNeighborsList);

	//std::cout << "lenNeighborsList " << teste << std::endl;

	// Write mobility structure
	unsigned char temp[sizeof(VehicleInfo)];
	memcpy( temp, &m_vehicleInfo, sizeof(VehicleInfo));
	i.Write(temp, sizeof(VehicleInfo));

	teste = teste + sizeof(VehicleInfo);

	//std::cout << "VehicleInfo " << teste << std::endl;

	i.WriteHtonU16(neighborsList.size());
	teste = teste + sizeof(neighborsList.size());

	//std::cout << "neighborsList.size() " << teste << std::endl;

	//NS_LOG_INFO(neighborsList.size());

	for(auto id : neighborsList){
		i.WriteHtonU16(id[0]);
		teste = teste + sizeof(id[0]);
		i.WriteHtonU16(id[1]);
		teste = teste + sizeof(id[1]);
		i.WriteHtonU16(id[2]);
		teste = teste + sizeof(id[2]);
		//std::cout << "pathList.size() " << teste << std::endl;
		//for(auto nodeId : pathList){
			//i.WriteHtonU16(id[2]);
			//teste = teste + sizeof(id[2]);
			//std::cout << "nodeId " << teste << std::endl;
			//NS_LOG_INFO(id[2]);

		//}

	}

	/*NS_LOG_DEBUG("Class: BeaconHeader " << this);
	Buffer::Iterator i = start;
	i.WriteHtonU64(m_seq);

	i.WriteHtonU16(neighborsList.size());

	// Write mobility structure
	unsigned char temp[sizeof(VehicleInfo)];
	memcpy( temp, &m_vehicleInfo, sizeof(VehicleInfo));
	i.Write(temp, sizeof(VehicleInfo));*/

	time(&end);

	//double tempo = end - ini;

	//std::cout << "A função Serialize foi executada em " << tempo << " segundos.\n";
	//::cout << "Final " << teste << std::endl;
}

uint32_t BeaconHeader::Deserialize(Buffer::Iterator start) {
	time_t ini, end;

	time(&ini);

	//NS_LOG_DEBUG("Class: BeaconHeader " << this);

	//uint16_t nElem = 0;

	Buffer::Iterator i = start;
	m_seq = i.ReadNtohU64();

	lenNeighborsList = i.ReadNtohU16();

	unsigned char temp[sizeof(VehicleInfo)];
	i.Read(temp, sizeof(VehicleInfo));
	memcpy(&m_vehicleInfo, &temp, sizeof(VehicleInfo));

	uint32_t listSize =  i.ReadNtohU16();
	//uint32_t pListSize;

	for(uint32_t k = 0; k < listSize; k++){
		//pListSize = i.ReadNtohU16();
		//nElem = nElem + pListSize;
		std::vector<uint16_t> pathList;
		//for(uint16_t j=0; j<pListSize; j++){
			//uint16_t nodeId = i.ReadNtohU16();
			//NS_LOG_INFO(nodeId << " ID" << std::endl);
			pathList.push_back(i.ReadNtohU16());
			pathList.push_back(i.ReadNtohU16());
			pathList.push_back(i.ReadNtohU16());
		//}

		neighborsList.push_back(pathList);
	}

	/*NS_LOG_DEBUG("Class: BeaconHeader " << this);

	Buffer::Iterator i = start;
	m_seq = i.ReadNtohU64();

	lenNeighborsList = i.ReadNtohU16();

	unsigned char temp[sizeof(VehicleInfo)];
	i.Read(temp, sizeof(VehicleInfo));
	memcpy(&m_vehicleInfo, &temp, sizeof(VehicleInfo) );*/

	time(&end);

	//double tempo = end - ini;

	//std::cout << "A função Deserialize foi executada em " << tempo << " segundos.\n";

	return GetSerializedSize();

}
/*****************  BeaconHeader END  **********************/

}
