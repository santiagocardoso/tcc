/*
 * minuet-utils.cc
 *
 *  Created on: 30 de out de 2018
 *      Author: everaldo
 */

#include "minuet-utils.h"
#include "ctime"
#include <sys/stat.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("MinuetConfig");

static uint32_t tNode;
static bool pastaCriada = false;

//Criar tabela de amizade entre os nós da simulação
static const uint16_t menorId = 0;		//É necessário definir o nó com menor ID do cenário
static const uint16_t maiorId = 1;		//É necessário definir o nó com maior ID do cenário
//static const int tam = maiorId - menorId;
static const int tam = 2;
static uint16_t amizade[tam][tam];
static vector<Vector> basestationsPos;

vector<double_t>
MinuetConfig::RandonNumbersGeneratorBetweenMaxAndMin(uint64_t totalValues, uint64_t maxValue, uint64_t minValue) {
	vector<double_t> values(totalValues);

	for (uint64_t i = 0 ; i < totalValues ; i++) {
		int value = minValue + (rand() % (maxValue - minValue + 1));
		values[i] = value;
	}

	return values;
}

vector<double_t>
MinuetConfig::DoubleRandonNumbersGeneratorBetweenMaxAndMin(uint32_t totalValues, double_t maxValue, double_t minValue) {
	vector<double_t> values(totalValues);

	for (uint64_t i = 0 ; i < totalValues ; i++) {
		double_t value = minValue + ((double_t)rand() / RAND_MAX) * (maxValue - minValue);
		values[i] = value;
	}

	return values;
}

int MinuetConfig::RandonNumberGeneratorBetweenRange(uint32_t min, uint32_t max) {
	int x = (rand() % max) + min;
	//cout << x << endl;
	return x;
}

uint32_t MinuetConfig::GetTotalNodes(){
	return tNode;
}
void MinuetConfig::SetTotalNodes(uint32_t totalN){
	tNode = totalN;
}

// $ns_ at 36.0 "$g(4) start"; # SUMO-ID: AVL---_CN_4_0_0
// $ns_ at 80.0 "$g(4) stop"; # SUMO-ID: AVL---_CN_4_0_0
vector<vector<double_t>>
MinuetConfig::GetInitialAndFinalTimeByNode(uint64_t totalNodes, string pathTraceActivityFile, double_t initialTime, double_t finalTime) {
	vector<vector<double_t>> initialAndFinalTime(totalNodes);
	string line;
	ifstream file;
	smatch sm1, sm2, sm3, sm4;
	string::size_type sz;
	double_t initialTimeNode, finalTimeNode;
	double_t nodeId = 0.0;
	uint32_t totalNode = 0;

	//MinuetConfig::totalNode = 0;

	if(totalNode == 0){
		;/*Empty*/
	}

	regex re1 ("\\$ns_ at [0-9.]+ \"\\$g\\([0-9]+\\) start\";");
	regex re2 ("\\$ns_ at [0-9.]+ \"\\$g\\([0-9]+\\) stop\";");
	regex re3 ("[0-9.]+");

	file.open(pathTraceActivityFile.c_str());

	if(file.is_open()){

		while(getline(file, line)) {
			while(regex_search (line,sm1,re1)) {
//				cout << line << endl;
				string line1 = sm1[0];
				double_t infoNode[2];
				for(int i = 0 ; regex_search (line1,sm2,re3) ; i++) {
					infoNode[i] = stod(sm2[0], &sz);
					line1 = sm2.suffix().str();
				}

//				cout << "Node #" <<  infoNode[1] << " Start in " << infoNode[0] << endl;

				initialTimeNode = infoNode[0];
				nodeId = infoNode[1];

				line = sm1.suffix().str();
			}

			while(regex_search (line,sm3,re2)) {
//				cout << line << endl;
				string line1 = sm3[0];
				double_t infoNode[2];
				for(int i = 0 ; regex_search (line1,sm4,re3) ; i++) {
					infoNode[i] = stod(sm4[0], &sz);
					line1 = sm4.suffix().str();
				}

//				cout << "Node #" <<  infoNode[1] << " Stop in " << infoNode[0] << endl;

				finalTimeNode = infoNode[0];
				if(nodeId != infoNode[1]) {
					cerr << "## ERROR: NodeId diferent!!" << endl;
					exit(EXIT_FAILURE);
				}



				if(initialTime <= finalTimeNode && initialTimeNode <= finalTime) {
					if(finalTime < finalTimeNode)
						finalTimeNode = finalTime;

					if(initialTimeNode < initialTime)
						initialTimeNode = initialTime;

//					cout << "Insert Node #" << nodeId << " Start: " << initialTimeNode << " Stop: " << finalTimeNode << endl;
					totalNode++;

					initialAndFinalTime[(int)nodeId].push_back(initialTimeNode);
					initialAndFinalTime[(int)nodeId].push_back(finalTimeNode);

				}

				line = sm3.suffix().str();
			}
		}
	}

	cout << "Total Nodes between " << initialTime << " and " << finalTime << " - " << totalNode << endl;

	file.close();

	return initialAndFinalTime;
}

//Get current date/time, format is DD-MM-YYYY HH:mm:ss and add to log files names
string
MinuetConfig::GetCurrentDateTime() {
	time_t timer;
	struct tm *horarioLocal;

	time(&timer); // Obtem informações de data e hora
	horarioLocal = localtime(&timer); // Converte a hora atual para a hora local

	int dia = horarioLocal->tm_mday;
	int mes = horarioLocal->tm_mon + 1;
	int ano = horarioLocal->tm_year + 1900;

	int hora = horarioLocal->tm_hour;
	int min  = horarioLocal->tm_min;
	//int sec  = horarioLocal->tm_sec;

	string currentDate = std::to_string(dia) + "-" + std::to_string(mes) + "-" + std::to_string(ano) + "_" + std::to_string(hora) + ":" + std::to_string(min);
	/*string path = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + currentDate;
	if (!pastaCriada){
		mkdir(path.c_str(), 0777);
		pastaCriada = true;
	}*/
	return currentDate;
}

uint16_t MinuetConfig::GetAmizade(uint16_t i, uint16_t j){
	return amizade[i][j];
}
void MinuetConfig::SetAmizade(uint16_t i, uint16_t j, uint16_t valor){
	amizade[i][j] = valor;
	amizade[j][i] = valor;
}

void MinuetConfig::AddBasestationPos(Vector position){
	basestationsPos.push_back(position);
}

vector<Vector>::iterator MinuetConfig::GetBasestationPosIterator(){
	return basestationsPos.begin();
}

vector<Vector>::iterator MinuetConfig::GetBasestationPosEnd(){
	return basestationsPos.end();
}

uint16_t MinuetConfig::GetMenorId(){
	return menorId;
}

void MinuetConfig::MakeLogDirectory(){
	string currentDate = MinuetConfig::GetCurrentDateTime();
	string path = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + currentDate;
	string pathDump = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + currentDate + "/dump/";
	string pathDumpRd = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + currentDate + "/dump/rd/";
	string pathDumpSd = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + currentDate + "/dump/sd/";
	if (!pastaCriada){
		mkdir(path.c_str(), 0777);
		mkdir(pathDump.c_str(), 0777);
		mkdir(pathDumpRd.c_str(), 0777);
		mkdir(pathDumpSd.c_str(), 0777);
		pastaCriada = true;
	}

    /*
	for(int i=0; i<tam; i++){
		MinuetConfig::SetAmizade(i, i, 0);
		for(int j=i+1; j<tam; j++){
			int x = MinuetConfig::RandonNumberGeneratorBetweenRange(0, 100);
			if(x<10){
				MinuetConfig::SetAmizade(i, j, 0);
			} else if(x>=10 && x<30){
				MinuetConfig::SetAmizade(i, j, 1);
			} else if(x>=30 && x<70){
				MinuetConfig::SetAmizade(i, j, 2);
			} else if(x>=70 && x<90){
				MinuetConfig::SetAmizade(i, j, 3);
			} else {
				MinuetConfig::SetAmizade(i, j, 4);
			}
		}
	}
    */

	return;
}



/******************************  Central Region Scenario ***********************************/
const string MinuetConfig::TRACE_EVENTS_FILE = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/trace/events/TraceEvents-TCC-centro-alta.ev";
const string MinuetConfig::TRACE_BASE_STATIONS_FILE = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/trace/base_station/TraceBaseStations-TCC-centro.bs";
const string MinuetConfig::TRACE_MOBILITY_FILE = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/trace/tcl/TCC/centro/alta/mobiTracerFinal.tcl";
const string MinuetConfig::TRACE_ACTTIVITY_FILE = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/trace/tcl/TCC/centro/alta/activityTracerFinal.tcl";
const string MinuetConfig::TRACE_CONFIG_FILE = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/trace/tcl/TCC/centro/alta/configTracerFinal.tcl";

const string MinuetConfig::TRACE_NETANIM_FILE = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/trace/netanim/LustNetAnim.xml";
const string MinuetConfig::LOG_FILE_APP_MINUET = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/logFileAppMINUET.log";
const string MinuetConfig::LOG_FILE_MINUET = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/logFileMINUET.log";
const string MinuetConfig::LOG_FILE_DETECTION_LAYER = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/logFileDetectionLayer.log";
const string MinuetConfig::LOG_FILE_ANNOUNCEMENT_LAYER = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/logFileAnnouncementLayer.log";
const string MinuetConfig::LOG_FILE_MONITORING_LAYER = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/logFileMonitoringLayer.log";
const string MinuetConfig::LOG_FILE_COMMUNICATION_LAYER = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/logFileCommunicationLayer.log";
const string MinuetConfig::LOG_FILE_CLUSTERING_MANAGER = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/logFileClusteringManager.log";
const string MinuetConfig::LOG_FILE_CLUSTERING_ALGORITHM = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/logFileClusteringAlgorithm.log";
const string MinuetConfig::LOG_FILE_BASE_STATIONS = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/logFileBaseStation.log";

const string MinuetConfig::LOG_FILE_VELOCITIES = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/logFileVelocities.log";
const string MinuetConfig::LOG_FILE = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/logFile.log";
const string MinuetConfig::LOG_FILE_MINUETCENARIO = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/logFileMINUETCenario.log";

// Configure Parameters Dump Files
const string MinuetConfig::SD_FILE_NAME = "sd_file_";
const string MinuetConfig::RD_FILE_NAME = "rd_file_";
const string MinuetConfig::TOTAL_FRAMES_SENT_FILE_NAME = "totalFramesSent_";
const string MinuetConfig::SD_FILE_PATH = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/dump/sd/";
const string MinuetConfig::RD_FILE_PATH = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/log/" + MinuetConfig::GetCurrentDateTime() + "/dump/rd/";

// Configure Parameters Wifi Physic Layer
const string MinuetConfig::PHYSIC_MODE = "OfdmRate6MbpsBW10MHz";

// Configure Parameters Wifi Physic Layer Base Station
const string MinuetConfig::PHYSIC_MODE_BASE_STATION = "OfdmRate6MbpsBW10MHz";

// Configure Parameters Wifi Transport Layer
const string MinuetConfig::IP = "10.0.0.0";
const string MinuetConfig::MASK = "255.0.0.0";

// Protocols Sockets
const string MinuetConfig::SEND_CONTROL_PROTOCOL = "ns3::UdpSocketFactory";
const string MinuetConfig::LISTEN_CONTROL_PROTOCOL = "ns3::UdpSocketFactory";
const string MinuetConfig::SEND_DATA_PROTOCOL = "ns3::UdpSocketFactory";
const string MinuetConfig::LISTEN_DATA_PROTOCOL = "ns3::UdpSocketFactory";

// Protocols Sockets Base Station Nodes
const string MinuetConfig::LISTEN_DATA_PROTOCOL_BASE_STATION = "ns3::UdpSocketFactory";

// Configure Parameters Monitoring Layer
const string MinuetConfig::VIDEO_TRACE_FILE_NAME = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/trace/video_stream/st_highway_cif.trace";
/*****************************************************************************************/

} // namespace ns3
