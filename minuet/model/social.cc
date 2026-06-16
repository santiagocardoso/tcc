#include "ns3/core-module.h"
#include "social.h"
#include "minuet-utils.h"
#include "time.h"
#include <stdlib.h>     /* srand, rand */
#include <sys/stat.h>
#include <iomanip>  // Para std::setprecision
#include <sstream>  // Para std::ostringstream
#include <cstdint>

#include <chrono>
#include <ctime>

#include "mcda-utils.h"
#include "icr-utils.h"


std::map<std::string, std::map<int, int>> SOCIAL::m_randomData;
bool SOCIAL::m_dataLoaded = false;

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SOCIAL");

/*************** ClusteringManagerInterface ************************/
NS_OBJECT_ENSURE_REGISTERED (SOCIALInterface);

SOCIALInterface::SOCIALInterface() {
	//NS_LOG_FUNCTION(this);
}
SOCIALInterface::~SOCIALInterface() {
	NS_LOG_FUNCTION(this);
}

void SOCIALInterface::AddCommunicationLayer(const Ptr<CommunicationLayer>& comunicationLayer) {
	SOCIALInterface::m_communicationLayer = comunicationLayer;
	SOCIALInterface::m_communicationLayer->AttachSOCIALInterface(this);
}
/*************** END ClusteringManagerInterface ************************/

/*************** VehicleSOCIAL ************************/

VehicleSOCIAL::VehicleSOCIAL(){/* Empty */}

VehicleSOCIAL::VehicleSOCIAL(uint16_t id, uint32_t score, bool isRelay, uint16_t relayId, Vector position, bool isPerfilSocial, bool isGateway, uint32_t eigenScore, float closenessScore, vector<vector<uint16_t>> neighborsList, uint16_t atividadeSocial) { //Todas
//VehicleSOCIAL::VehicleSOCIAL(uint16_t id, uint32_t score, bool isRelay, uint16_t relayId, Vector position, bool isPerfilSocial, bool isGateway, uint32_t eigenScore, float closenessScore, vector<vector<uint16_t>> neighborsList) { //STR
//VehicleSOCIAL::VehicleSOCIAL(uint16_t id, uint32_t atividadeSocial, bool isRelay, uint16_t relayId, Vector position, bool isPerfilSocial, bool isGateway) { //Atividade Social
//VehicleSOCIAL::VehicleSOCIAL(uint16_t id, uint32_t amizade, bool isRelay, uint16_t relayId, Vector position, bool isPerfilSocial, bool isGateway) { //Amizade
	m_id = id;
	m_score = score; //STR
	m_eigenScore = eigenScore; //STR
	m_closenessScore = closenessScore; //STR
	m_atividadeSocial = atividadeSocial; //Atividade Social
	//m_amizade = amizade; //Amizade
	m_isRelay = isRelay;
	m_relayId = relayId;
	m_position = position;
	m_isPerfilSocial = isPerfilSocial;
	m_isGateway = isGateway;
	m_neighborsList = neighborsList; //STR
	m_timeExpire = Simulator::Now().GetSeconds();
}

VehicleSOCIAL::~VehicleSOCIAL(){/* Empty */}

uint16_t VehicleSOCIAL::GetRelayId() const {
	return m_relayId;
}

void VehicleSOCIAL::SetRelayId(uint16_t relayId) {
	m_relayId = relayId;
}

Vector VehicleSOCIAL::GetDirection() const {
	return m_direction;
}

void VehicleSOCIAL::SetDirection(Vector direction) {
	m_direction = direction;
}

uint16_t VehicleSOCIAL::GetId() const {
	return m_id;
}

void VehicleSOCIAL::SetId(uint16_t id) {
	m_id = id;
}

bool VehicleSOCIAL::IsRelay() const {
	return m_isRelay;
}

void VehicleSOCIAL::SetIsRelay(bool isRelay) {
	m_isRelay = isRelay;
}

bool VehicleSOCIAL::IsGateway() const {
	return m_isGateway;
}

void VehicleSOCIAL::SetIsGateway(bool isGateway) {
	m_isGateway = isGateway;
}

Vector VehicleSOCIAL::GetPosition() const {
	return m_position;
}

void VehicleSOCIAL::SetPosition(const Vector position) {
	m_position = position;
}

uint32_t VehicleSOCIAL::GetScore() const {
	return m_score;
}

void VehicleSOCIAL::SetScore(uint32_t score) {
	m_score = score;
}

uint32_t VehicleSOCIAL::GetEigenScore() const {
	return m_eigenScore;
}

void VehicleSOCIAL::SetEigenScore(uint32_t eigenScore) {
	m_eigenScore = eigenScore;
}

float VehicleSOCIAL::GetClosenessScore() const {
	return m_closenessScore;
}

void VehicleSOCIAL::SetClosenessScore(float closenessScore) {
	m_closenessScore = closenessScore;
}

uint16_t VehicleSOCIAL::GetAtividadeSocial() const {
	return m_atividadeSocial;
}

void VehicleSOCIAL::SetAtividadeSocial(uint16_t atividadeSocial) {
	m_atividadeSocial = atividadeSocial;
}

double_t VehicleSOCIAL::GetTimeExpire() const {
	return m_timeExpire;
}

bool VehicleSOCIAL::IsPerfilSocial() const {
	return m_isPerfilSocial;
}

void VehicleSOCIAL::SetIsPerfilSocial(bool isPerfilSocial) {
	m_isPerfilSocial = isPerfilSocial;
}

void VehicleSOCIAL::SetTimeExpire(double_t timeExpire) {
	m_timeExpire = timeExpire;
}

vector<vector<uint16_t>> VehicleSOCIAL::GetNeighborsList() const{
	return m_neighborsList;
}

/*************** END VehicleSOCIAL ************************/

NS_OBJECT_ENSURE_REGISTERED(SOCIAL);

TypeId SOCIAL::GetTypeId() {
	static TypeId typeId =
			TypeId("ns3::SOCIAL")
			.AddConstructor<SOCIAL>()
			.SetParent<Object>()
			.AddAttribute("Node",
					"The Node of the SOCIAL", PointerValue(),
					MakePointerAccessor(&SOCIAL::m_node),
					MakePointerChecker<Node>())
			.AddAttribute("CommunicationLayer",
					"The Communication Instance", PointerValue(),
					MakePointerAccessor(&SOCIAL::m_communicationLayer),
					MakePointerChecker<CommunicationLayer>());

	return typeId;
}

SOCIAL::SOCIAL () {
	//NS_LOG_FUNCTION(this);
	m_sentCounter = 0;
	m_outOfSim = true;

    if (!m_dataLoaded) {
        std::ifstream file("/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/trace/random.txt");
        
        std::string line;
        while (getline(file, line)) {
            std::istringstream iss(line);
            std::string key;
            int value;

			iss >> key;

			int vehicle_index = 0;
            while (iss >> value) {
                m_randomData[key][vehicle_index] = value;
                vehicle_index++;
            }
        }
        file.close();

        m_dataLoaded = true;
        std::cout << "Arquivo random.txt OK" << std::endl;
    }
}

SOCIAL::~SOCIAL() {
	//NS_LOG_FUNCTION(this);
}

std::map<uint16_t, Ptr<Node>> nodeMap;

void SOCIAL::StartClustering () {
	//NS_LOG_FUNCTION(this);
	//NS_LOG_INFO("RUNNING STARTCLUSTERING!" << m_node->GetId());

	m_mobilityModel = m_node->GetObject<MobilityModel>();
	nodeMap[m_node->GetId()] = m_node;
	
	m_outOfSim = false;

	AddCommunicationLayer(m_communicationLayer);

	m_isRelay = false;
	m_relayId = NaN; //m_node->GetId();
	m_isGateway = false;

	m_score = 0;
	m_eigenScore = 0;
	m_closenessScore = 0;

	m_atividadeSocial = 0;

	wBC = wCC = wDC = wEC = 0.25;

	int x = MinuetConfig::RandonNumberGeneratorBetweenRange(0, 100);

	/*RngSeedManager::SetSeed (m_node->GetId());
	RngSeedManager::SetRun(1);
	int x = RngSeedManager::GetNextStreamIndex();*/

	if ((x % 100) < 90){
		//NS_LOG_INFO("Menor que 90: " << x);
		m_isPerfilSocial = true;
	}
	else{
		//NS_LOG_INFO("Maior que 90: " << x);
		m_isPerfilSocial = true;
	}

	//if(!m_isPerfilSocial)
		//NS_LOG_INFO("É falso");

	MinuetConfig::SetTotalNodes(MinuetConfig::GetTotalNodes() + 1);

	PrintInLog("SOCIAL Algorithm Started! " + std::to_string(MinuetConfig::GetTotalNodes()) + " na comunidade.");
	//NS_LOG_INFO("SOCIAL Algorithm Started! " << std::to_string(MinuetConfig::GetTotalNodes()) << " na comunidade.");

	m_sendEvent = Simulator::Schedule (Seconds(m_node->GetId() * 0.0005), &SOCIAL::SendBeacon, this);
	//Simulator::Schedule (Seconds(m_node->GetId() * 0.000001), &SOCIAL::SendBeacon, this);
	//SendBeacon();

	m_cleanEvent = Simulator::Schedule (Seconds(m_node->GetId() * 0.0005), &SOCIAL::CleanUp, this);
	m_maintenanceEvent = Simulator::Schedule (Seconds(m_node->GetId() * 0.0005), &SOCIAL::EventsListMaintenance, this);
	//m_neighborMaintenanceEvent = Simulator::Schedule (Seconds(m_node->GetId() * 0.001), &SOCIAL::NeighborListMaintenance, this);

	m_clusteringStarted = true;
}

void SOCIAL::StopClustering () {
	//NS_LOG_FUNCTION(this);
	//NS_LOG_INFO("RUNNING STOPCLUSTERING! " << m_node->GetId());

	//Flag declarando-se fora da simulação
	m_outOfSim = true;

	//Cancelando eventos ativos
	m_sendEvent.Cancel();
	m_cleanEvent.Cancel();
	m_maintenanceEvent.Cancel();

	if (scheduledCheckEvents.size() != 0) {
		for (EventId eventId : scheduledCheckEvents) {
			eventId.Cancel();
		}
		scheduledCheckEvents.clear();
	}

	//cout << m_node->GetId();

	m_clusteringStarted = false;
	MinuetConfig::SetTotalNodes(MinuetConfig::GetTotalNodes() - 1);

	//NS_LOG_INFO("SOCIAL Algorithm Stopped! " << std::to_string(MinuetConfig::GetTotalNodes()) << " na comunidade.");

	PrintInLog("SOCIAL Algorithm Stopped! " + std::to_string(MinuetConfig::GetTotalNodes()) + " na comunidade.");
}

void SOCIAL::ReceiveControlMessage (Ptr<Packet> packet, Address addr) {
	//NS_LOG_FUNCTION(this);
	//NS_LOG_INFO("MESSAGE RECEIVED!");

	time_t ini, end;

	time(&ini);

	if (!m_outOfSim) {
		BeaconHeader beaconHeader;
		BeaconHeader::VehicleInfo vehicleInfo;

		packet->RemoveHeader(beaconHeader);

		//if(beaconHeader.GetLenNeighborsList() == beaconHeader.GetNeighborsList().size()){
			vehicleInfo = beaconHeader.GetVehicleInfo();
			//cout << vehicleInfo.direction.x << " " << vehicleInfo.direction.y << " " << vehicleInfo.direction.z << endl;
			VehicleSOCIAL vehicle2(vehicleInfo.id, vehicleInfo.score, vehicleInfo.isRelay, vehicleInfo.relayId, vehicleInfo.position, vehicleInfo.isPerfilSocial, vehicleInfo.isGateway, vehicleInfo.eigenScore, vehicleInfo.closenessScore, beaconHeader.GetNeighborsList(), vehicleInfo.atividadeSocial); //Todas

			vehicle2.SetDirection(vehicleInfo.direction);
			//VehicleSOCIAL vehicle2(vehicleInfo.id, vehicleInfo.score, vehicleInfo.isRelay, vehicleInfo.relayId, vehicleInfo.position, vehicleInfo.isPerfilSocial, vehicleInfo.isGateway, vehicleInfo.eigenScore, vehicleInfo.closenessScore, beaconHeader.GetNeighborsList()); //STR
			//VehicleSOCIAL vehicle2(vehicleInfo.id, vehicleInfo.atividadeSocial, vehicleInfo.isRelay, vehicleInfo.relayId, vehicleInfo.position, vehicleInfo.isPerfilSocial, vehicleInfo.isGateway); //Atividade Social
			//VehicleSOCIAL vehicle2(vehicleInfo.id, vehicleInfo.amizade, vehicleInfo.isRelay, vehicleInfo.relayId, vehicleInfo.position, vehicleInfo.isPerfilSocial, vehicleInfo.isGateway); //Amizade

			PrintInLog("Receive Control Message: Beacon");

			HandleBeacon(vehicle2);
		/*} else {
			vector<vector<uint16_t>> auxList;
			bool isOnPacketList = false;
			for (uint32_t j=0; j<packetList.size(); j++){
				if((beaconHeader.GetSeq() == (packetList[j].GetSeq()+1) || beaconHeader.GetSeq() == (packetList[j].GetSeq()-1)) && (beaconHeader.GetVehicleInfo().id == packetList[j].GetVehicleInfo().id)){
					isOnPacketList = true;
					if(beaconHeader.GetSeq() < packetList[j].GetSeq()){
						auxList = beaconHeader.GetNeighborsList();
						for (uint32_t i=0; i<packetList[j].GetNeighborsList().size(); i++){
							 auxList.push_back(packetList[j].GetNeighborsList()[i]);
						}
					} else {
						auxList = packetList[j].GetNeighborsList();
						for (uint32_t i=0; i<beaconHeader.GetNeighborsList().size(); i++){
							 auxList.push_back(beaconHeader.GetNeighborsList()[i]);
						}
					}
					packetList.erase(packetList.begin() + j);
					break;
				}
			}
			if(isOnPacketList){
				vehicleInfo = beaconHeader.GetVehicleInfo();

				VehicleSOCIAL vehicle2(vehicleInfo.id, vehicleInfo.score, vehicleInfo.isRelay, vehicleInfo.relayId, vehicleInfo.position, vehicleInfo.isPerfilSocial, vehicleInfo.isGateway, vehicleInfo.eigenScore, vehicleInfo.closenessScore, auxList);

				PrintInLog("Receive Control Message: Beacon");

				HandleBeacon(vehicle2);
			} else {
				packetList.push_back(beaconHeader);
			}
		}*/

		//NS_LOG_DEBUG(Simulator::Now().GetSeconds());
		if (int(Simulator::Now().GetSeconds()) == 29000){
			string perfil = (m_isPerfilSocial && vehicle2.IsPerfilSocial()) ? "1" : "0";
			NS_LOG_DEBUG(perfil);
			//PrintInGetEdgesOnAnInstant(std::to_string(m_node->GetId()) + "," + std::to_string(vehicle2.GetId()) + ",undirected,1," + perfil);
		}

	}

	time(&end);

	//double tempo = end - ini;

	//std::cout << "A função ReceiveControlMessage foi executada em " << tempo << " segundos.\n";
}

void SOCIAL::HandleBeacon (VehicleSOCIAL vehicle2) {
	//NS_LOG_FUNCTION(this);
	// printf("Beacon recebido %i: %zu %zu\n", m_node->GetId(), closeNeighbors.size());

	time_t ini, end;

	time(&ini);

	//if(m_node->GetId() == 1265){
	//	//Instrução vazia
	//	NS_LOG_DEBUG("1265");
	//}

	if(!m_isPerfilSocial)
		return;


	//NS_LOG_INFO("HANDLING BEACON FROM NODE: " << vehicle2.GetId());

	double_t separation = CalculateDistance(m_mobilityModel->GetPosition(), vehicle2.GetPosition());

	//PrintInLog("Handling Beacon From Node #" + std::to_string(vehicle2.GetId()) + " AngleDiff: " + std::to_string(angleDiff) + " Separation: " + std::to_string(separation));

	bool isCloseNeighbor = false;
	bool isGateway = false;
	vector<VehicleSOCIAL>::iterator vehicle;
	vector<VehicleSOCIAL>::iterator closeNeighbor;

	for (closeNeighbor = closeNeighbors.begin() ; closeNeighbor != closeNeighbors.end() ; closeNeighbor++) {
		if ((*closeNeighbor).GetId() == vehicle2.GetId()) {
			isCloseNeighbor = true;
			break;
		}
	}
	//cout << vehicle2.GetId() << ": " << vehicle2.GetDirection().x << " " << vehicle2.GetDirection().y << " " << vehicle2.GetDirection().z << " " << endl;
	if (separation < SOCIALUtils::MAX_SEPARATION_CLOSENEIGHBORS && vehicle2.IsPerfilSocial() == true) {
		if(!isCloseNeighbor) {
			//NS_LOG_INFO("CLUSTERING NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" CLOSENEIGHBORS WITH SEPARATION: " << separation);
			PrintInLog("Adding Node #" + std::to_string(vehicle2.GetId()) + " in Close Neighbors List");
			// cout << "Adding Node #" << std::to_string(vehicle2.GetId()) << " in Close Neighbors List" << endl;
			closeNeighbors.push_back(vehicle2);

			//if(!m_isPerfilSocial)
			//	return;

			bool isNewNeighbor = false;
			for (uint32_t i = 0; i<novosVizinhos.size(); i++){
				if (novosVizinhos[i][0] == vehicle2.GetId()){
					isNewNeighbor = true;
					break;
				}
			}

			if(!isNewNeighbor){
				vector<uint16_t> viz;
				viz.push_back(vehicle2.GetId());
				viz.push_back((uint16_t) Simulator::Now().GetSeconds());
				//NS_LOG_INFO("Veículo " << viz[0] << " adicionado à novos vizinhos de " << m_node->GetId() << " no instante " << (uint16_t) Simulator::Now().GetSeconds() << " segundos");
				novosVizinhos.push_back(viz);
			}


		} else {
			//NS_LOG_INFO("UPDATE NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" CLOSENEIGHBORS");
			PrintInLog("Update Node #" + std::to_string(vehicle2.GetId()) + " in Close Neighbors List");

			if((*closeNeighbor).IsGateway()){
				isGateway = true;
				//NS_LOG_INFO(vehicle2.GetId() << " já era gateway");
			}

			(*closeNeighbor).SetDirection(vehicle2.GetDirection());
			(*closeNeighbor).SetPosition(vehicle2.GetPosition());
			(*closeNeighbor).SetIsRelay(vehicle2.IsRelay());
			(*closeNeighbor).SetRelayId(vehicle2.GetRelayId());
			(*closeNeighbor).SetScore(vehicle2.GetScore());
			(*closeNeighbor).SetIsPerfilSocial(vehicle2.IsPerfilSocial());
			(*closeNeighbor).SetIsGateway(vehicle2.IsGateway());
			(*closeNeighbor).SetEigenScore(vehicle2.GetEigenScore());
			(*closeNeighbor).SetClosenessScore(vehicle2.GetClosenessScore());
			(*closeNeighbor).SetTimeExpire((*closeNeighbor).GetTimeExpire() + SOCIALUtils::EXPIRY_TIME_INTERVAL_1);

		}

		if (MinuetConfig::wSTR != 0){

			//Verifica se o vehicle2 é novo gateway (eigenscore++) ou não é mais gateway (eigenscore--)
			if (!isGateway && vehicle2.IsGateway()){ //Verifica se o nó não era gateway e agora é
				m_eigenScore++;
				//NS_LOG_INFO(vehicle2.GetId() << " não era gateway e agora é!");
			} else if (isGateway && !vehicle2.IsGateway()){ //Verifica se o nó era gateway e agora não é mais
				m_eigenScore--;
				//NS_LOG_INFO(vehicle2.GetId() << " era gateway e agora não é mais!");
			}

			//Verifica se vehicle2, que foi adicionado ou atualizado recentemente ao closeNeighbors, já existe em m_neighborsList.
			//Não é necessário verificar todos os closeneighbors, apenas o vehicle2.
			//for (closeNeighbor = closeNeighbors.begin() ; closeNeighbor != closeNeighbors.end() ; closeNeighbor++) {

			vector<vector<uint16_t>> auxListV2 = vehicle2.GetNeighborsList();

			//NS_LOG_DEBUG("Início: " << auxListV2.size() << ", " << vehicle2.GetNeighborsList().size() << ", " << m_neighborsList.size());

			for (uint32_t j = 0 ; j< auxListV2.size() ; j++) {
				if(auxListV2[j][0] == m_node->GetId()){
					auxListV2.erase(auxListV2.begin() + j);
					j--;
					continue;
				}

				if(auxListV2[j][1]>3){
					auxListV2.erase(auxListV2.begin() + j);
					j--;
					continue;
				}
			}

			bool isNeighbor = false;
			for(uint32_t i = 0; i<m_neighborsList.size(); i++){

				if(m_neighborsList[i][2] == vehicle2.GetId()){
					isNeighbor = true;
					m_neighborsList[i][0] = vehicle2.GetId();
					m_neighborsList[i][1] = 1;
					continue;
				}

				bool isOnNeighborList = false;
				if(m_neighborsList[i][0] == vehicle2.GetId() && (m_neighborsList[i][1] > 1) ){
					for (uint32_t j = 0 ; j< auxListV2.size() ; j++) {
						if(m_neighborsList[i][2] == auxListV2[j][2]){
							if(auxListV2[j][1]<4){
								m_neighborsList[i][1] = auxListV2[j][1] + 1;
								auxListV2.erase(auxListV2.begin() + j);
								isOnNeighborList = true;
								break;
							}
							else{
								auxListV2.erase(auxListV2.begin() + j);
								isOnNeighborList = false;
								break;
							}

						}
					}
					if(!isOnNeighborList){
						m_neighborsList.erase(m_neighborsList.begin() + i);
					}
				} else {
					for (uint32_t j = 0; j<auxListV2.size(); j++){
						if (m_neighborsList[i][2] == auxListV2[j][2]){
							if(((auxListV2[j][1] + 1) < m_neighborsList[i][1]) && ((auxListV2[j][1] + 1) < 4 )){
								m_neighborsList[i][0] = vehicle2.GetId();
								m_neighborsList[i][1] = auxListV2[j][1] + 1;
								m_neighborsList[i][2] = auxListV2[j][2];

							}
							auxListV2.erase(auxListV2.begin() + j);
							break;
						}
					}
				}
			}

			if(!isNeighbor){
				vector<uint16_t> m_pathList;
				m_pathList.push_back(vehicle2.GetId());
				m_pathList.push_back(1);
				m_pathList.push_back(vehicle2.GetId());

				m_neighborsList.push_back(m_pathList);
			}

			//NS_LOG_DEBUG("Após atualizar: " << auxListV2.size() << ", " << m_neighborsList.size());

			for (uint32_t i = 0; i < auxListV2.size(); i++){
				if(m_neighborsList.size() > 70)
					break;
				vector<uint16_t> m_pathList;
				m_pathList.push_back(vehicle2.GetId());
				m_pathList.push_back(auxListV2[i][1] + 1);
				m_pathList.push_back(auxListV2[i][2]);

				m_neighborsList.push_back(m_pathList);

				auxListV2.erase(auxListV2.begin() + i);
				i--;
			}

			//NS_LOG_DEBUG("Final: " << auxListV2.size() << ", " << m_neighborsList.size());

		}
		/*bool isOnNeighborList = false;

		//Verifica se o "vehicle2" está na lista de vizinhos e atualiza para nós com 1 salto, caso esteja
		for(uint32_t i = 0; i<m_neighborsList.size(); i++){
			//Esse if funciona, mas não precisava. Invés de "m_neighborList[i].size()-1", poderia usar m_neighborList[i][2])
			//if((*closeNeighbor).GetId() == m_neighborsList[i][2]){
			if(vehicle2.GetId() == m_neighborsList[i][2]){
				//vector<uint16_t> m_pathList;
				//m_pathList.push_back((*closeNeighbor).GetId());
				//m_pathList.push_back(1);
				//m_pathList.push_back((*closeNeighbor).GetId());

				//m_neighborsList.push_back(m_pathList);
				m_neighborsList[i][0] = vehicle2.GetId();
				m_neighborsList[i][1] = 1;
				isOnNeighborList = true;
				break;
			}
		}

		//Verifica se o "vehicle2" NÃO está na lista de vizinhos e acrescenta o "vehicle2" com 1 salto
		if(!isOnNeighborList){
			vector<uint16_t> m_pathList;
			m_pathList.push_back(vehicle2.GetId());
			m_pathList.push_back(1);
			m_pathList.push_back(vehicle2.GetId());

			m_neighborsList.push_back(m_pathList);

			//std::cout << m_neighborsList[0][1] << std::endl;
		}
		//}

		vector<uint16_t> onNeighborList;
		for(uint32_t i = 0; i<vehicle2.GetNeighborsList().size(); i++){
			for (uint32_t j = 0 ; j< m_neighborsList.size() ; j++) {
				//Se o atual ID já estiver na lista de vizinho do nó atual, pula para o próximo
				if (vehicle2.GetId() == m_neighborsList[j][2]){
					continue;
				}


				if (m_neighborsList[j][2] == vehicle2.GetNeighborsList()[i][2]){
					onNeighborList.push_back(vehicle2.GetNeighborsList()[i][2]);
					if( ((vehicle2.GetNeighborsList()[i][1] + 1) < m_neighborsList[j][1]) && ((vehicle2.GetNeighborsList()[i][1] + 1) < 4 )){

						m_neighborsList[j][0] = vehicle2.GetId();
						m_neighborsList[j][1] = vehicle2.GetNeighborsList()[i][1] + 1;
						m_neighborsList[j][2] = vehicle2.GetNeighborsList()[i][2];

						*for (uint32_t k = 0; k < vehicle2.GetNeighborsList()[i].size(); k++){
							m_neighborsList[j].push_back(vehicle2.GetNeighborsList()[i][k]);
						}*
					}
					break;
				}

			}
		}

		if(onNeighborList.size()>0){

			for(uint32_t j = 0; j < vehicle2.GetNeighborsList().size(); j++){

				if (vehicle2.GetNeighborsList()[j][2] == m_node->GetId()){
					//Se o atual ID já estiver na lista de vizinho do nó atual, pula para o próximo
					continue;
				}

				bool isOnNeighborList = false;
				for(uint32_t k = 0; k < onNeighborList.size(); k++){
					if(vehicle2.GetNeighborsList()[j][2] == onNeighborList[k]){
						isOnNeighborList = true;
						break;
					}
				}

				if(!isOnNeighborList){
					for(uint32_t k = 0; k < m_neighborsList.size(); k++){
						if (vehicle2.GetNeighborsList()[j][2] == m_neighborsList[k][2]){
							//Se o atual ID já estiver na lista de vizinho do nó atual, pula para o próximo
							isOnNeighborList = true;
							break;
						}
					}
				}

				if(!isOnNeighborList){
					if((vehicle2.GetNeighborsList()[j][1] + 1) < 4){

						vector<uint16_t> m_pathList;
						m_pathList.push_back(vehicle2.GetId());
						m_pathList.push_back(vehicle2.GetNeighborsList()[j][1] + 1);
						m_pathList.push_back(vehicle2.GetNeighborsList()[j][2]);
						*for (uint32_t k = 0; k < vehicle2.GetNeighborsList()[j][1]; k++){
							m_pathList.push_back(vehicle2.GetNeighborsList()[j][k]);
						///}*
						//m_pathList.insert(m_pathList.end(), vehicle2.GetNeighborsList()[j].begin(), vehicle2.GetNeighborsList()[j].end());
						//m_neighborsList[i].insert(m_neighborsList[i].end(), vehicle2.GetNeighborsList()[i].begin(), vehicle2.GetNeighborsList()[i].end());
						m_neighborsList.push_back(m_pathList);
					}
				}
			}
		}



		for(uint32_t i = 0; i<m_neighborsList.size(); i++){
			bool isOnNeighborList = false;
			if(m_neighborsList[i][0] == vehicle2.GetId() && (m_neighborsList[i][1] > 1) ){
				for (uint32_t j = 0 ; j< vehicle2.GetNeighborsList().size() ; j++) {
					if(m_neighborsList[i][2] == vehicle2.GetNeighborsList()[j][2]){
						isOnNeighborList = true;
						break;
					}
				}
				if(!isOnNeighborList){
					m_neighborsList.erase(m_neighborsList.begin() + i);
				}
			}
		}*/
	} else {
		// cout << "else" << endl;
	}

	float closenessScore = 0;
	//Calcula closenessScore do nó atual
	for(uint32_t i = 0; i<m_neighborsList.size(); i++){
		closenessScore = closenessScore + m_neighborsList[i][1];
	}

	closenessScore = m_neighborsList.size()/closenessScore;

	m_score = closeNeighbors.size();
	m_closenessScore = closenessScore;
	m_atividadeSocial = novosVizinhos.size();


	//NS_LOG_INFO("NODE: " << m_node->GetId() << " TEM " << m_score << " VIZINHOS E " << m_atividadeSocial << " SÃO NOVOS VIZINHOS.");

	time(&end);

	//double tempo = end - ini;

	//std::cout << "A função HandleBeacon foi executada em " << tempo << " segundos.\n";
	//NS_LOG_INFO("NODE: " << m_node->GetId() << " TEM " << m_score << " VIZINHOS E " << m_eigenScore << " SÃO GATEWAYS.");
}

Ptr<Node> FindNodeById(uint16_t nodeId) {
    auto it = nodeMap.find(nodeId);
    if (it != nodeMap.end())
        return it->second;
	else
        return nullptr;
}

uint16_t SOCIAL::RelayElection() {
    time_t ini, end;
    time(&ini);

    if (closeNeighbors.empty()) return UINT16_MAX;

    vector<vector<double>> evaluationMatrix;
    vector<int> ids;

    double totalNeighborSpeed_kmh = 0.0;

    for (const auto& neighbor : closeNeighbors) {
        uint16_t neighborId = neighbor.GetId();
        Ptr<Node> neighborNode = FindNodeById(neighborId);

        if (neighborNode) {
            Vector neighborVelocity = neighborNode->GetObject<MobilityModel>()->GetVelocity();
            totalNeighborSpeed_kmh += neighborVelocity.GetLength() * 3.6;
        }
    }

    double averageClusterSpeed_kmh = totalNeighborSpeed_kmh / static_cast<double>(closeNeighbors.size());

    // --- CONSTRUÇÃO DA MATRIZ DE AVALIAÇÃO ---
    for (const auto& neighbor : closeNeighbors) {
        uint16_t neighborId = neighbor.GetId();
        Ptr<Node> neighborNode = FindNodeById(neighborId);
        if (!neighborNode) continue; 

        auto neighborMobility = neighborNode->GetObject<MobilityModel>();
        Vector neighborPosition = neighborMobility->GetPosition();
        Vector neighborVelocity = neighborMobility->GetVelocity();

        double magnitudeVelocity_kmh = neighborVelocity.GetLength() * 3.6;

        // C2: Cluster Velocity Difference
        double cc = CalculateClusterVelocityScore(magnitudeVelocity_kmh, averageClusterSpeed_kmh);

        // C3: Distance to closest basestation
        Vector closestBasestationPosition;        
        double cd = MinuetConfig::FLOOR_SIZE_X + MinuetConfig::FLOOR_SIZE_Y;
        for (auto basestationPos = MinuetConfig::GetBasestationPosIterator(); basestationPos != MinuetConfig::GetBasestationPosEnd(); ++basestationPos) {
            double currentDistance = CalculateDistance(*basestationPos, neighborPosition);

            if (currentDistance < cd) {
                cd = currentDistance;
                closestBasestationPosition = *basestationPos;
            }
        }

        // C1: Direction (Theta)
        double c0 = 0.0;
        double velocityMagnitude = neighborVelocity.GetLength();

        if (velocityMagnitude > 0.0) {
            Vector vectorToClosestBS = closestBasestationPosition - neighborPosition;
            double vectorToBSMagnitude = vectorToClosestBS.GetLength();

            if (vectorToBSMagnitude > 0.0) {
                double dotProduct = (vectorToClosestBS.x * neighborVelocity.x) + (vectorToClosestBS.y * neighborVelocity.y);
                double cosTheta = dotProduct / (vectorToBSMagnitude * velocityMagnitude);
                c0 = (cosTheta + 1.0) / 2.0;
            }
            else {c0 = 1.0;}
        }

        // C4: Number of neighbors
        double cn = (double)neighbor.GetScore();

        // C5: Velocity
        double cv = CalculateAverageSpeedScore(magnitudeVelocity_kmh);

        // random.txt
        int av = m_randomData["av"][neighborId];
        int ec = m_randomData["ec"][neighborId];
        int tc = m_randomData["tc"][neighborId];
        int im = m_randomData["im"][neighborId];
        int pv = m_randomData["pv"][neighborId];
        int lp = m_randomData["lp"][neighborId];
        int tt = m_randomData["tt"][neighborId];
        int vt = m_randomData["vt"][neighborId];

        // C6: Vehicle age
        double ca = CalculateVehicleAgeScore(av);

        // C7: Fuel efficiency
        double ce = CalculateFuelEfficiencyScore(ec);
        
        // C8: Time licensed x Drivers age x Vehicle potency
        double timeLicensed = CalculateTimeLicensedScore(tc);
        double ageVsPotency = CalculateAgeVsPotencyScore(pv, im, lp);
        double ci = (0.5 * timeLicensed + 0.5 * ageVsPotency);
        
        // C9: Time traveled
        double ct = CalculateTimeTraveledScore(tt);
        
        // C10: Vehicle type
        double cm = CalculateVehicleTypeScore(vt);

        ids.push_back(neighborId);
        evaluationMatrix.push_back({c0, cc, cd, cn, cv, ca, ce, ci, ct, cm});
    }

	if (evaluationMatrix.empty()) return -1;
	int bestIndex = -1;

	// --- PRÉ-PROCESSAMENTO ---
	vector<vector<double>> processedMatrix = evaluationMatrix;
	double minDistance = processedMatrix[0][2], maxDistance = processedMatrix[0][2];
    double minNeighbors = processedMatrix[0][3], maxNeighbors = processedMatrix[0][3];
	for (const auto& entry : processedMatrix) {
		minDistance = std::min(minDistance, entry[2]);
        maxDistance = std::max(maxDistance, entry[2]);
        minNeighbors = std::min(minNeighbors, entry[3]);
        maxNeighbors = std::max(maxNeighbors, entry[3]);
	}
	for (auto& entry : processedMatrix) {
        if (maxDistance > minDistance) {
            double normalized_dist = (entry[2] - minDistance) / (maxDistance - minDistance);
            entry[2] = 1.0 - normalized_dist;
        } 
        else {entry[2] = 1.0;}

        if (maxNeighbors > minNeighbors) {
            entry[3] = (entry[3] - minNeighbors) / (maxNeighbors - minNeighbors);
        } 
        else {entry[3] = 1.0;}
    }

    /*
    // RANDOM Functions
    int bestIndexRandom = -1;
    vector<double> scoresRandom(ids.size(), 0.0);
    if (ids.size() > 0) {
        bestIndexRandom = rand() % ids.size();
        scoresRandom[bestIndexRandom] = 1.0;
        bestIndex = bestIndexRandom;
    }
    if (bestIndex != -1) {
        uint64_t tempo = Simulator::Now().GetNanoSeconds();
        LogAllScores("RANDOM", ids, processedMatrix, scoresRandom, tempo);
    }
    */

	// AHP Functions
	int bestIndexAhp = -1;
	vector<vector<double>> matrizDeJulgamentoAHP = getDefaultAHPJudgmentMatrix();
	vector<vector<double>> normalizedJudgmentAHP = normalizeAHPMatrix(matrizDeJulgamentoAHP);
	vector<double> weightsAHP = calculateAHPWeights(normalizedJudgmentAHP);
    //vector<double> scoresAHP = scoreAHPAlternativesSimple(processedMatrix, weightsAHP);
    vector<double> scoresAHP = scoreAHPAlternatives(processedMatrix, weightsAHP);
	bestIndexAhp = bestAlternative(scoresAHP);
    bestIndex = bestIndexAhp;
	if (bestIndexAhp != -1) {
		uint64_t tempo = Simulator::Now().GetNanoSeconds();
		LogAllScores("AHP", ids, processedMatrix, scoresAHP, tempo);
	}

    /*
    // PROMETHEE
	int bestIndexPromethee = -1;
	vector<vector<double>> preferenceMatrix = calculatePreferenceMatrix(processedMatrix, weightsAHP);
	vector<double> phiPlus = calculatePositiveFlows(preferenceMatrix);
	vector<double> phiMinus = calculateNegativeFlows(preferenceMatrix);
	vector<double> netFlows = calculateNetFlows(phiPlus, phiMinus);
	bestIndexPromethee = bestAlternativePromethee(netFlows);
    bestIndex = bestIndexPromethee;
    //if (bestIndexPromethee != -1) {
	//	uint64_t tempo = Simulator::Now().GetNanoSeconds();
	//	LogAllScores("PROMETHEE", ids, processedMatrix, netFlows, tempo);
	//}
    */

    /*
	// T0OPSIS Functions
	int bestIndexTopsis = -1;
    vector<vector<double>> normalizedMatrixTopsis = normalize(processedMatrix);
    vector<vector<double>> weightedMatrixTopsis = multiplyWeights(normalizedMatrixTopsis, weightsAHP);
    vector<double> idealTopsis = idealSolution(weightedMatrixTopsis);
    vector<double> antiIdealTopsis = negativeIdealSolution(weightedMatrixTopsis);
    vector<double> euclideanDistanceIdealTopsis = euclideanDistance(weightedMatrixTopsis, idealTopsis);
    vector<double> euclideanDistanceAntiIdealTopsis = euclideanDistance(weightedMatrixTopsis, antiIdealTopsis);
    vector<double> coeffsTopsis = closenessCoefficients(euclideanDistanceIdealTopsis, euclideanDistanceAntiIdealTopsis);
    bestIndexTopsis = bestAlternative(coeffsTopsis);
    bestIndex = bestIndexTopsis;
	//if (bestIndexTopsis != -1) {
	//	uint64_t tempo = Simulator::Now().GetNanoSeconds();
	//	LogAllScores("TOPSIS", ids, processedMatrix, coeffsTopsis, tempo); 
	//}
    */

    /*
    // Border Count
	std::map<int, int> bordaScores;
	for (int id : ids) {bordaScores[id] = 0;}
	int n = ids.size();
	UpdateBordaScores(scoresAHP, ids, n, bordaScores);
    UpdateBordaScores(netFlows, ids, n, bordaScores);
    UpdateBordaScores(coeffsTopsis, ids, n, bordaScores);
	int winnerId = -1;
	int maxBordaScore = -1;
	std::vector<double> finalBordaScoresForLog;
	for (int id : ids) {
		finalBordaScoresForLog.push_back(bordaScores[id]);
		if (bordaScores[id] > maxBordaScore) {
			maxBordaScore = bordaScores[id];
			winnerId = id;
		}
	}
	if (winnerId != -1) {
		auto it = std::find(ids.begin(), ids.end(), winnerId);
		if (it != ids.end()) {
			bestIndex = std::distance(ids.begin(), it);
		}
	}
	if (bestIndex != -1) {
		uint64_t tempo = Simulator::Now().GetNanoSeconds();
		LogAllScores("BORDA", ids, processedMatrix, finalBordaScoresForLog, tempo);
	}
    */

	time(&end);
	if (bestIndex != -1 && !ids.empty()) {
    	return ids[bestIndex];
	}
	return -1;
}

uint16_t SOCIAL::RelayElectionSTR () {


	time_t ini, end;

	time(&ini);

	//NS_LOG_FUNCTION(this);
	//NS_LOG_INFO("ELECTING Relay!");

	vector<VehicleSOCIAL>::iterator closeNeighbor;

	float degreeScore = 0;
	float closenessScore = 0;
	float eigenScore = 0;
	float betweenScore = 0;
	float str = 0;

	float highestScore = 0;
	float highestDegreeScore;
	float highestClosenessScore;
	float highestEigenScore;
	float highestBetweenScore;
	uint16_t highestScoreId = NaN;

	highestBetweenScore = betweenScore;
	highestClosenessScore = closenessScore;
	highestDegreeScore = degreeScore;
	highestEigenScore = eigenScore;

	if (highestBetweenScore && highestClosenessScore && highestDegreeScore && highestEigenScore){
		;/*Empty*/
	}


	for (closeNeighbor = closeNeighbors.begin() ; closeNeighbor != closeNeighbors.end() ; closeNeighbor++) {
		str = 0;
		degreeScore = (*closeNeighbor).GetScore();
		closenessScore = (*closeNeighbor).GetClosenessScore();
		eigenScore = (*closeNeighbor).GetEigenScore();
		betweenScore = 0;

		for (uint32_t i = 0; i<m_neighborsList.size(); i++) {
			if((*closeNeighbor).GetId() == m_neighborsList[i][0]){
				betweenScore++;
			}
		}

		//betweenScore = (2*betweenScore) / ( pow(m_neighborsList.size(), 2) - (3*m_neighborsList.size()) + 2 );
		betweenScore = betweenScore / m_neighborsList.size();

		if(degreeScore == 0){
			eigenScore = 0;
		} else {
			eigenScore = eigenScore/degreeScore;
		}

		degreeScore = degreeScore/(MinuetConfig::GetTotalNodes()-1);
		if(degreeScore>1)
			degreeScore = 1;


		//NS_LOG_INFO("Nó " << (*closeNeighbor).GetId() << " degree: " << degreeScore << " closeness: " << closenessScore << " betweenness: " << betweenScore << " eigen: " <<  eigenScore);

		str =  (wBC * betweenScore) + (wCC * closenessScore) + (wDC * degreeScore) + (wEC * eigenScore);
		if(str > highestScore){
			highestBetweenScore = betweenScore;
			highestClosenessScore = closenessScore;
			highestDegreeScore = degreeScore;
			highestEigenScore = eigenScore;

			highestScore = str;
			highestScoreId = (*closeNeighbor).GetId();
		}
	}


	if(highestScoreId == 0){
		//NS_LOG_ERROR("CAN'T FIND THE NEXT RELAY NODE!");
		//cerr << "## ERROR: Can't find the next relay node!" << endl;
		//exit(1);
		return -1;
	}
	else{
		PrintInLog("NODE " + std::to_string(highestScoreId) + " IS THE NEXT RELAY WITH STR " + std::to_string(highestScore) + " DEGREE " + std::to_string(highestDegreeScore) + ", CLOSENESS " + std::to_string(highestClosenessScore) + ", BETWEENNESS: " + std::to_string(highestBetweenScore) + ", EIGENVECTOR: " +  std::to_string(highestEigenScore) + ". WITH QoS WEIGHT " + std::to_string(wBC*3) + " AND QoE WEIGHT " + std::to_string(wEC));
		//NS_LOG_INFO("NODE " << std::to_string(highestScoreId) << " IS THE NEXT RELAY WITH STR " << std::to_string(highestScore) << " DEGREE " << std::to_string(highestDegreeScore) << ", CLOSENESS " << std::to_string(highestClosenessScore) << ", BETWEENNESS: " << std::to_string(highestBetweenScore) << ", EIGENVECTOR: " <<  std::to_string(highestEigenScore) << ". WITH QoS WEIGHT " << std::to_string(wBC*3) << " AND QoE WEIGHT " << std::to_string(wEC));
		//NS_LOG_INFO("NODE " << highestScoreId << " IS THE NEXT RELAY WITH STR " << str << " DEGREE " << degreeScore << ", CLOSENESS " << closenessScore << ", BETWEENNESS: " << betweenScore << ", EIGENVECTOR: " <<  eigenScore);
		return highestScoreId;

	}

	time(&end);

	//double tempo = end - ini;

	//std::cout << "A função RelayElection foi executada em " << tempo << " segundos.\n";
}

uint16_t SOCIAL::RelayElectionAtivSocial() {
	vector<VehicleSOCIAL>::iterator closeNeighbor;

	uint16_t highestAtivSocial=0;
	uint16_t highestScoreId = NaN;

	for (closeNeighbor = closeNeighbors.begin() ; closeNeighbor != closeNeighbors.end() ; closeNeighbor++) {
		if((*closeNeighbor).GetAtividadeSocial() > highestAtivSocial){
			highestAtivSocial = (*closeNeighbor).GetAtividadeSocial();
			highestScoreId = (*closeNeighbor).GetId();
		}
	}

	if(highestScoreId == 0){
			//NS_LOG_ERROR("CAN'T FIND THE NEXT RELAY NODE!");
			//cerr << "## ERROR: Can't find the next relay node!" << endl;
			//exit(1);
			return -1;
	}
	else{
		PrintInLog("NODE " + std::to_string(highestScoreId) + " IS THE NEXT RELAY WITH SOCIAL ACTIVITY " + std::to_string(highestAtivSocial));
		//NS_LOG_INFO("NODE " << std::to_string(highestScoreId) << " IS THE NEXT RELAY WITH SOCIAL ACTIVITY " << std::to_string(highestAtivSocial));
		//NS_LOG_INFO("NODE " << highestScoreId << " IS THE NEXT RELAY WITH STR " << str << " DEGREE " << degreeScore << ", CLOSENESS " << closenessScore << ", BETWEENNESS: " << betweenScore << ", EIGENVECTOR: " <<  eigenScore);
		return highestScoreId;

	}

	//return highestScoreId;
}

void SOCIAL::RelayElectionAmizade() {
	/*vector<VehicleSOCIAL>::iterator closeNeighbor;

	uint16_t highestAmizade=0;
	uint16_t highestScoreId = NaN;

	for (closeNeighbor = closeNeighbors.begin() ; closeNeighbor != closeNeighbors.end() ; closeNeighbor++) {
		if(MinuetConfig::GetAmizade((m_node->GetId()-MinuetConfig::GetMenorId()), ((*closeNeighbor).GetId())-MinuetConfig::GetMenorId()) > highestAmizade){
			highestAmizade = MinuetConfig::GetAmizade((m_node->GetId()-MinuetConfig::GetMenorId()), ((*closeNeighbor).GetId())-MinuetConfig::GetMenorId());
			highestScoreId = (*closeNeighbor).GetId();
		}
	}

	if(highestScoreId == 0){
			//NS_LOG_ERROR("CAN'T FIND THE NEXT RELAY NODE!");
			//cerr << "## ERROR: Can't find the next relay node!" << endl;
			//exit(1);
			return -1;
	}
	else{
		PrintInLog("NODE " + std::to_string(highestScoreId) + " IS THE NEXT RELAY WITH FRIENDSHIP " + std::to_string(highestAmizade));
		//NS_LOG_INFO("NODE " << std::to_string(highestScoreId) << " IS THE NEXT RELAY WITH FRIENDSHIP " << std::to_string(highestAmizade));
		//NS_LOG_INFO("NODE " << highestScoreId << " IS THE NEXT RELAY WITH STR " << str << " DEGREE " << degreeScore << ", CLOSENESS " << closenessScore << ", BETWEENNESS: " << betweenScore << ", EIGENVECTOR: " <<  eigenScore);
		return highestScoreId;

	}*/

	time_t ini, end;

	time(&ini);

	vector<VehicleSOCIAL>::iterator closeNeighbor;

	// float score = 0 ;
	float degreeScore = 0;
	float closenessScore = 0;
	float eigenScore = 0;
	float betweenScore = 0;
	float str = 0;
	float atividadeSocial = 0;
	float amizade = 0;

	float highestScore = 0;
	float highestDegreeScore;
	float highestClosenessScore;
	float highestEigenScore;
	float highestBetweenScore;
	float highestAtividadeSocial;
	float highestAmizade;
	float highestSTR;
	uint16_t highestScoreId = 0;

	highestBetweenScore = betweenScore;
	highestClosenessScore = closenessScore;
	highestDegreeScore = degreeScore;
	highestEigenScore = eigenScore;
	highestSTR = str;
	highestAtividadeSocial = atividadeSocial;
	highestAmizade = amizade;

	if (highestBetweenScore && highestClosenessScore && highestDegreeScore && highestEigenScore){
		;/*Empty*/
	}

	NS_LOG_DEBUG("INFORMAÇÕES DO NÓ: " << m_node->GetId());

	for (closeNeighbor = closeNeighbors.begin() ; closeNeighbor != closeNeighbors.end() ; closeNeighbor++) {
		str = 0;
		degreeScore = (*closeNeighbor).GetScore();
		closenessScore = (*closeNeighbor).GetClosenessScore();
		eigenScore = (*closeNeighbor).GetEigenScore();
		betweenScore = 0;

		for (uint32_t i = 0; i<m_neighborsList.size(); i++) {
			if((*closeNeighbor).GetId() == m_neighborsList[i][0]){
				betweenScore++;
			}
		}

		//betweenScore = (2*betweenScore) / ( pow(m_neighborsList.size(), 2) - (3*m_neighborsList.size()) + 2 );
		betweenScore = betweenScore / m_neighborsList.size();

		//Se o STR não for calculado, isto é, a lista de vizinhos não existir, BC e CC são NaN, esse if serve para não dar esse problema.
		if(!m_neighborsList.size()){
			betweenScore = 0;
			closenessScore = 0;
		}

		if(degreeScore == 0){
			eigenScore = 0;
		} else {
			eigenScore = eigenScore/degreeScore;
		}

		degreeScore = degreeScore/(MinuetConfig::GetTotalNodes()-1);
		if(degreeScore>1)
			degreeScore = 1;


		NS_LOG_INFO("Nó " << (*closeNeighbor).GetId() << " degree: " << degreeScore << " closeness: " << closenessScore << " betweenness: " << betweenScore << " eigen: " <<  eigenScore);

		if (MinuetConfig::wSTR)
			str = float((wBC * betweenScore) + (wCC * closenessScore) + (wDC * degreeScore) + (wEC * eigenScore));
		else
			str = 0;

		amizade = (float) MinuetConfig::GetAmizade((m_node->GetId()-MinuetConfig::GetMenorId()), ((*closeNeighbor).GetId())-MinuetConfig::GetMenorId())/4;

		atividadeSocial = (float) (*closeNeighbor).GetAtividadeSocial()/(*closeNeighbor).GetScore();

		NS_LOG_DEBUG("Nó " << (*closeNeighbor).GetId() << " Str: " << str << " Atividade Social: " << atividadeSocial << " Amizade: " << amizade);
		PrintInLog("Nó " + std::to_string(m_node->GetId()) + " tem relacionamento com " + std::to_string((*closeNeighbor).GetId()) + " que possui Str: " + std::to_string(str) + " Atividade Social: " + std::to_string(atividadeSocial) + " Amizade: " + std::to_string(amizade));
		//wSTR = 1;
		//wAm = 1;
		//wAS = 1;

		// score = ((MinuetConfig::wSTR * str) + (MinuetConfig::wAm * amizade) + (MinuetConfig::wAS * atividadeSocial));

	}


	if(highestScoreId == 0){
		//NS_LOG_ERROR("CAN'T FIND THE NEXT RELAY NODE!");
		//cerr << "## ERROR: Can't find the next relay node!" << endl;
		//exit(1);
		return;
	}
	else{
		//PrintInLog("NODE " + std::to_string(highestScoreId) + " IS THE NEXT RELAY WITH STR " + std::to_string(highestScore) + " DEGREE " + std::to_string(highestDegreeScore) + ", CLOSENESS " + std::to_string(highestClosenessScore) + ", BETWEENNESS: " + std::to_string(highestBetweenScore) + ", EIGENVECTOR: " +  std::to_string(highestEigenScore) + ". WITH QoS WEIGHT " + std::to_string(wBC*3) + " AND QoE WEIGHT " + std::to_string(wEC));
		PrintInLog("NODE " + std::to_string(highestScoreId) + " IS THE NEXT RELAY WITH SCORE: " + std::to_string(highestScore) + " STR: " + std::to_string((MinuetConfig::wSTR * highestSTR)) + " SOCIAL ACTIVITY: " + std::to_string((MinuetConfig::wAS * highestAtividadeSocial)) + " AND FRIENDSHIP: " + std::to_string((MinuetConfig::wAm * highestAmizade)));
		//NS_LOG_INFO("NODE " << std::to_string(highestScoreId) << " IS THE NEXT RELAY WITH SCORE " << std::to_string(highestScore) << " STR " << std::to_string(highestSTR) << ", ATIVIDADE SOCIAL " << std::to_string(highestAtividadeSocial) << ", AMIZADE: " << std::to_string(highestAmizade));
		//NS_LOG_INFO("NODE " << (*closeNeighbor).GetId() << " IS THE NEXT RELAY WITH STR: " << (MinuetConfig::wSTR * str) << " SOCIAL ACTIVITY: " << (MinuetConfig::wAS * atividadeSocial) << " AND FRIENDSHIP: " << (MinuetConfig::wAm * amizade));
		//NS_LOG_INFO("NODE " << highestScoreId << " IS THE NEXT RELAY WITH STR " << str << " DEGREE " << degreeScore << ", CLOSENESS " << closenessScore << ", BETWEENNESS: " << betweenScore << ", EIGENVECTOR: " <<  eigenScore);
		return;

	}

	time(&end);

}

void SOCIAL::Purge (uint32_t vehicleId, vector<VehicleSOCIAL>& vehicleVector) {
	//NS_LOG_FUNCTION(this);

	for(vector<VehicleSOCIAL>::iterator vehicle = vehicleVector.begin() ; vehicle < vehicleVector.end() ; vehicle++) {
		if ((*vehicle).GetId() == vehicleId) {
			vehicle = vehicleVector.erase(vehicle);
			PrintInLog("Purge Node #" + std::to_string(vehicleId) + " from List");
		}
	}
}

void SOCIAL::CleanUp () {
	//NS_LOG_FUNCTION(this);

	time_t ini, end;

	time(&ini);

	for(vector<VehicleSOCIAL>::iterator vehicle = closeNeighbors.begin() ; vehicle < closeNeighbors.end() ; vehicle++) {
		if ((*vehicle).GetTimeExpire() <= Simulator::Now().GetSeconds()) {
			//NS_LOG_INFO("CLEANING UP VEHICLE: " << (*vehicle).GetId() << " FROM VEHICLE "<< m_node->GetId() <<  " CLOSENEIGHBORS");
			for(uint32_t i = 0; i<novosVizinhos.size(); i++){
				if((*vehicle).GetId() == novosVizinhos[i][0]){
					novosVizinhos.erase(novosVizinhos.begin() + i);
					break;
				}
			}
			// cout << "cleanup " << vehicle->GetId() << " m_node: "<< m_node->GetId() << endl;
			vehicle = closeNeighbors.erase(vehicle);
		}
	}

	/////////////////////STR////////////////////////////
	//Calcula o eigenvector após o CleanUp
	m_eigenScore = 0;
	for(vector<VehicleSOCIAL>::iterator vehicle = closeNeighbors.begin() ; vehicle < closeNeighbors.end() ; vehicle++) {
		if((*vehicle).IsGateway() == true){
			m_eigenScore++;
			//NS_LOG_INFO("COUNTING EIGENSCORE FROM VEHICLE " << m_node->GetId() << " SCORE IS " << m_eigenScore);
		}
	}

	//Limpa os nós que eram alcançados por nós que não são mais closeNeighbor
	for(uint32_t i = 0; i<m_neighborsList.size(); i++) {
		bool isOnCloseNeighborList = false;
		for(vector<VehicleSOCIAL>::iterator vehicle = closeNeighbors.begin() ; vehicle < closeNeighbors.end() ; vehicle++){
			if( m_neighborsList[i][0] == (*vehicle).GetId() ){
				isOnCloseNeighborList = true;
				break;
			}
		}

		if(isOnCloseNeighborList == false){
			m_neighborsList.erase(m_neighborsList.begin() + i);
			i--;
		}
	}

	float closenessScore = 0;
	//Calcula closenessScore do nó atual após o CleanUp
	for(uint32_t i = 0; i<m_neighborsList.size(); i++){
		closenessScore = closenessScore + m_neighborsList[i][1];
	}

	closenessScore = m_neighborsList.size()/closenessScore;

	m_score = closeNeighbors.size();
	m_closenessScore = closenessScore;
	///////////////END OF STR///////////////////////////


	/////////////////////Atividade Social////////////////////////////
	for(uint32_t i = 0; i<novosVizinhos.size(); i++){
		if((uint16_t) Simulator::Now().GetSeconds() - novosVizinhos[i][1] > 3){
			novosVizinhos.erase(novosVizinhos.begin() + i);
			i--;
		}
	}

	m_atividadeSocial = novosVizinhos.size();
	///////////////END OF Atividade Social///////////////////////////

	m_cleanEvent = Simulator::Schedule (Seconds(SOCIALUtils::CLEAN_UP_TIMER), &SOCIAL::CleanUp, this);

	time(&end);

	//double tempo = end - ini;

	//std::cout << "A função CleanUp foi executada em " << tempo << " segundos.\n";
}

void SOCIAL::VerifyRelay (uint32_t vehicleId) {
	//NS_LOG_FUNCTION(this);

}

void SOCIAL::HasHigherScore (uint32_t vehicleId) {
	//NS_LOG_FUNCTION(this);

}

void SOCIAL::CheckIncreaseByThreshold (VehicleSOCIAL vehicle2, double_t separation, double_t angleDiff) {
	NS_LOG_FUNCTION(this);

	double_t separationThreshold = separation + (separation * SOCIALUtils::THRESHOLD);
	double_t angleDiffThreshold = angleDiff + (angleDiff * SOCIALUtils::THRESHOLD);

	double_t newSeparation = CalculateDistance(m_mobilityModel->GetPosition(), vehicle2.GetPosition());
	double_t newAngleDiff = GetAngleDiff(m_mobilityModel->GetVelocity(), vehicle2.GetDirection());

	vehicle2.SetTimeExpire(Simulator::Now().GetSeconds() + SOCIALUtils::EXPIRY_TIME_INTERVAL_3);

	//NS_LOG_INFO("SETTING NODE "<< vehicle2.GetId() <<" TIMEEXPIRE TO: " << vehicle2.GetTimeExpire());

	if ((newSeparation <= separationThreshold) && (newAngleDiff <= angleDiffThreshold)) {
		//neighbors.push_back(vehicle2);

		if (separation < SOCIALUtils::MAX_SEPARATION_CLOSENEIGHBORS) {
			//NS_LOG_INFO("CLUSTERING NODE "<< vehicle2.GetId() <<" INTO "<< m_node->GetId() <<" CLOSENEIGHBORS WITH SEPARATION: " << separation);
			closeNeighbors.push_back(vehicle2);
		}
	}
}

void SOCIAL::CheckIncreaseByThresholdInAngleAbove45(VehicleSOCIAL vehicle2, double_t separation, double_t angleDiff) {
	//NS_LOG_FUNCTION(this);

	double_t separationThreshold = separation + (separation * SOCIALUtils::THRESHOLD);
	double_t angleDiffThreshold = angleDiff + (angleDiff * SOCIALUtils::THRESHOLD);

	double_t newSeparation = CalculateDistance(m_mobilityModel->GetPosition(), vehicle2.GetPosition());
	double_t newAngleDiff = GetAngleDiff(m_mobilityModel->GetVelocity(), vehicle2.GetDirection());

	if((newSeparation <= separationThreshold) && (newAngleDiff <= angleDiffThreshold)) {

		/*for(vector<VehicleSOCIAL>::iterator neighbor = neighbors.begin() ; neighbor < neighbors.end() ; neighbor++) {
			if((*neighbor).GetId() == vehicle2.GetId())
				(*neighbor).SetTimeExpire((*neighbor).GetTimeExpire() + SOCIALUtils::EXPIRY_TIME_INTERVAL_5);
		}*/

	} else {

		PrintInLog("Taking Node: " + std::to_string(vehicle2.GetId()) + " out of Cluster");
		//Purge(vehicle2.GetId(), neighbors);

	}

	for(auto vehicle : closeNeighbors) {
		if(vehicle2.GetId() == vehicle.GetId())
			Purge(vehicle2.GetId(), closeNeighbors);
	}

	//NS_LOG_INFO("SETTING NODE "<< vehicle2.GetId() <<" TIMEEXPIRE TO: " << vehicle2.GetTimeExpire());
}

void SOCIAL::SendBeacon () {
	//NS_LOG_FUNCTION(this);

	time_t ini, end;

	time(&ini);

	PrintInLog("Sending Beacon Message TO " + std::to_string(m_score) + " VIZINHOS.");

	BeaconHeader::VehicleInfo info;
	//BeaconHeader::VehicleInfo info2;

	info.id = (uint16_t)m_node->GetId();

	/////////STR/////////
	info.score = m_score;
	info.eigenScore = m_eigenScore;
	info.closenessScore = m_closenessScore;

	info.atividadeSocial = m_atividadeSocial;
	//info.amizade = m_amizade;
	//info.isRelay = m_isRelay;
	info.relayId = m_relayId;
	info.isPerfilSocial = m_isPerfilSocial;
	info.isGateway = m_isGateway;
	info.position = m_mobilityModel->GetPosition();
	info.direction = m_mobilityModel->GetVelocity();

	// cout << m_node->GetId() << ": " << m_mobilityModel->GetVelocity().x << " " << m_mobilityModel->GetVelocity().y << " " << m_mobilityModel->GetVelocity().z << " " << endl;
	// velocity x y z

	/*if(m_neighborsList.size() > 30){
		BeaconHeader header;
		BeaconHeader header2;

		vector<vector<uint32_t>> neighborsList;
		vector<vector<uint32_t>> neighborsList2;


		header.SetSeq(m_sentCounter++);
		//header2.SetSeq(m_sentCounter);

		header.SetVehicleInfo(info);
		header2.SetVehicleInfo(info2);

		for (uint32_t i = 0; i < m_neighborsList.size(); i++){
			if(i<30)
				neighborsList.push_back(m_neighborsList[i]);
			else
				neighborsList2.push_back(m_neighborsList[i]);
		}

		header.SetNeighborsList(neighborsList);
		header2.SetNeighborsList(neighborsList2);

		SendMessage(header);
		SendMessage(header2);
	} else{

		BeaconHeader header;
		header.SetSeq(m_sentCounter++);
		header.SetVehicleInfo(info);
		header.SetNeighborsList(m_neighborsList);

		SendMessage(header);
	}*/

	BeaconHeader header;
	header.SetSeq(m_sentCounter++);
	header.SetLenNeighborsList(m_neighborsList.size());
	header.SetVehicleInfo(info);
	header.SetNeighborsList(m_neighborsList);

	SendMessage(header);

	m_sendEvent = Simulator::Schedule (Seconds(SOCIALUtils::BEACON_MESSAGE_TIMER), &SOCIAL::SendBeacon, this);

	time(&end);

	//double tempo = end - ini;

	//std::cout << "A função SendBeacon foi executada em " << tempo << " segundos.\n";
	//NS_LOG_INFO("SENDING THIS INFO: ID: " << info.id << " SCORE: " << info.score << " POSITION: " << info.position);

	/*if((int) Simulator::Now().GetSeconds() == 29000){
		NS_LOG_DEBUG("Total de veículos na comunidade -> " << MinuetConfig::GetTotalNodes());
		RelayElectionAmizade();
	}*/
}

void SOCIAL::SendMessage (BeaconHeader header) {
	//NS_LOG_FUNCTION(this);

	time_t ini, end;

	time(&ini);

	//NS_LOG_DEBUG("NODE: "<< m_node->GetId() <<" SENDING BEACON MESSAGE IN BROADCAST!");

	/*if(header.GetNeighborsList().size() > 80){
		BeaconHeader headerAux;
		BeaconHeader headerAux2;

		headerAux.SetSeq(header.GetSeq());
		headerAux2.SetSeq(m_sentCounter++);

		headerAux.SetLenNeighborsList(header.GetLenNeighborsList());
		headerAux2.SetLenNeighborsList(header.GetLenNeighborsList());

		headerAux.SetVehicleInfo(header.GetVehicleInfo());
		headerAux2.SetVehicleInfo(header.GetVehicleInfo());

		vector<vector<uint16_t>> neighborsList;
		vector<vector<uint16_t>> neighborsList2;

		for(uint32_t i=0; i<header.GetNeighborsList().size(); i++){
			if(i<(header.GetNeighborsList().size()/2))
				neighborsList.push_back(header.GetNeighborsList()[i]);
			else
				neighborsList2.push_back(header.GetNeighborsList()[i]);
		}

		headerAux.SetNeighborsList(neighborsList);
		headerAux2.SetNeighborsList(neighborsList2);


		Ptr<Packet> packet = Create<Packet>();
		Ptr<Packet> packet2 = Create<Packet>();

		packet->AddHeader(headerAux);
		packet2->AddHeader(headerAux2);

		m_communicationLayer->SendControlMenssage(packet);
		m_communicationLayer->SendControlMenssage(packet2);
	} else{*/

	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(header);


	//NS_LOG_DEBUG("NODE: "<< m_node->GetId() << " TEM " << m_score << " VIZINHOS, FAZ PARTE DO PERFIL SOCIAL " << m_isPerfilSocial << " E É RETRANSMISSOR " << m_isRelay);
	m_communicationLayer->SendControlMenssage(packet);
	//}

	time(&end);

	//double tempo = end - ini;

	//std::cout << "A função SendMessage foi executada em " << tempo << " segundos.\n";
}

double_t SOCIAL::GetAngleDiff (Vector v1, Vector v2) {
	NS_LOG_FUNCTION(this);

	double_t diff = 0.0;

	double l1 = v1.GetLength();
	double l2 = v2.GetLength();

	if ((l1 != 0) && (l2 != 0)) {
		diff = acos(((v1.x*v2.x) + (v1.y*v2.y)) / (l1*l2));
	}

	return (diff * 180) / PI;   //Convert from Radius to Degrees
}

bool SOCIAL::IsRelay () {
	//NS_LOG_FUNCTION(this);

	return m_isRelay && (m_relayId == m_node->GetId());
}

bool SOCIAL::IsGateway () {
	//NS_LOG_FUNCTION(this);

	return m_isGateway;
}

void SOCIAL::SetIsGateway(bool isGateway) {
	m_isGateway = isGateway;
}

bool SOCIAL::IsPerfilSocial () {
	//NS_LOG_FUNCTION(this);

	return m_isPerfilSocial;
}

bool SOCIAL::IsClusterMember () {
	NS_LOG_FUNCTION(this);

	return !m_isRelay && (m_relayId != m_node->GetId());
}

bool SOCIAL::IsIsolated () {
	NS_LOG_FUNCTION(this);

	return false;
}

uint16_t SOCIAL::GetRelayId(uint16_t op) {
	//NS_LOG_FUNCTION(this);

	/*if(IsRelay() || IsClusterMember()) {
		return m_relayId;
	} else {
		cerr << "## ERROR: In GetClusterId method node is not Cluster Head or Member Cluster" << endl;
		exit(1);
	}*/

	time_t ini, end;

	time(&ini);

	switch(op){
	case 0:
		wBC = wCC = wDC = 0.9/3;
		wEC = 0.1;
		break;
	case 1:
		wBC = wCC = wDC = 0.9/3;
		wEC = 0.1;
		break;
	case 2:
		wBC = wCC = wDC = 0.75/3;
		wEC = 0.25;
		break;
	case 3:
		wBC = wCC = wDC = 0.66/3;
		wEC = 0.34;
		break;
	case 4:
		wBC = wCC = wDC = 0.5/3;
		wEC = 0.5;
		break;
	case 5:
		wBC = wCC = wDC = 0.34/3;
		wEC = 0.66;
		break;
	case 6:
		wBC = wCC = wDC = 0.25/3;
		wEC = 0.75;
		break;
	case 7:
		wBC = wCC = wDC = 0.1/3;
		wEC = 0.9;
		break;
	default:
		wBC = wCC = wDC = 0.1/3;
		wEC = 0.9;
		break;
	}

	time(&end);

	//double tempo = end - ini;

	//std::cout << "A função GetRelayId foi executada em " << tempo << " segundos.\n";

	return RelayElection();
	//return RelayElectionSTR();
	//return RelayElectionAtivSocial();
	//return RelayElectionAmizade();
}

bool SOCIAL::IsStarted() {
	return m_clusteringStarted;
}

void SOCIAL::EventsListMaintenance () {
	//NS_LOG_FUNCTION(this);

	for(vector<EventId>::iterator eventIdIt =  scheduledCheckEvents.begin() ; eventIdIt < scheduledCheckEvents.end() ; eventIdIt++) {
		if((*eventIdIt).IsExpired()) {
			eventIdIt = scheduledCheckEvents.erase(eventIdIt);
		}
	}

	m_maintenanceEvent = Simulator::Schedule (Seconds(SOCIALUtils::EVENTS_LIST_MAINTENANCE_TIMER), &SOCIAL::EventsListMaintenance, this);
}

void SOCIAL::PrintInLog(string message) {
	//ofstream os;
	//os.open (MinuetConfig::LOG_FILE_CLUSTERING_ALGORITHM.c_str(), ofstream::out | ofstream::app);
	//os << Simulator::Now().GetNanoSeconds() << "ns - SOCIAL - Node #" << m_node->GetId() << " Pos (" << m_mobilityModel->GetPosition() << ") GroupId #" << m_relayId << " : " << message << endl;
	//os.close();
	return;
} // comentado TCC

void SOCIAL::PrintInGetEdgesOnAnInstant(string message) {
	time_t ini, end;

	time(&ini);
	ofstream os;
	string path = "ns-3.28/src/minuet/utils/log/EdgesOnAnInstant-AD.csv";
	os.open (path.c_str(), ofstream::out | ofstream::app);
	os << message << endl;
	os.close();
	time(&end);

	//double tempo = end - ini;

	//std::cout << "A função PrintInLog (Social) foi executada em " << tempo << " segundos.\n";
}
}
