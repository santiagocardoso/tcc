/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include <string>
#include <ctime>
#include <iomanip>
#include "ns3/netanim-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/wave-module.h"
#include "ns3/internet-module.h"
#include "ns3/minuet-helper.h"
#include "ns3/base-station-helper.h"
#include "ns3/minuet-base-station.h"
#include "ns3/mobility-module.h"
#include "../model/icr-utils.h"

//#define SIMULATION_TIME_FORMAT(s) Seconds(s)

using namespace ns3;
using namespace std;

NS_LOG_COMPONENT_DEFINE("MINUETScenarioTest");

void
PrintInLog(vector<Ptr<EventDetected>>, vector<Ptr<BaseStation>>);

void GerarValoresAleatorios() {
    const char* folderPath = "/root/ns3/ns-allinone-3.29/ns-3.29/src/minuet/utils/trace/";
    const std::string filePath = std::string(folderPath) + "random.txt";

    std::ifstream checkFile(filePath);
    if (checkFile.good()) {
        std::cout << "Arquivo random.txt já existe. Nenhuma alteração feita.\n";
        return;
    }

    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo para escrita: " << filePath << std::endl;
        return;
    }

    int totalVeiculos = MinuetConfig::TOTAL_NODES;

    std::random_device rd;
    std::mt19937 gen(rd());

    auto gerarLinha = [&](const std::string& nome, int min, int max) {
        std::uniform_int_distribution<> dist(min, max);
        outFile << nome;
        for (int i = 0; i < totalVeiculos; ++i) {
            outFile << " " << dist(gen);
        }
        outFile << "\n";
    };

    gerarLinha("av", 2000, 2025);
    gerarLinha("ec", 0, 4);

    std::vector<int> im_values;
    std::vector<int> tc_values;
    std::uniform_int_distribution<> dist_im(18, 80);

    for (int i = 0; i < totalVeiculos; ++i) {
        int idade_motorista = dist_im(gen);
        int max_tc = idade_motorista - 18;

        std::uniform_int_distribution<> dist_tc(0, max_tc);
        int tempo_carteira = dist_tc(gen);
        
        im_values.push_back(idade_motorista);
        tc_values.push_back(tempo_carteira);
    }
    outFile << "im";
    for (int val : im_values) { outFile << " " << val; }
    outFile << "\n";

    outFile << "tc";
    for (int val : tc_values) { outFile << " " << val; }
    outFile << "\n";

    std::uniform_int_distribution<> dist_pv(60, 300);
    outFile << "pv";
    for (int i = 0; i < totalVeiculos; ++i) {
        outFile << " " << dist_pv(gen);
    }
    outFile << "\n";

    outFile << "lp";
    std::vector<double> weights_lp = {90.0, 10.0};
    std::discrete_distribution<> dist_lp(weights_lp.begin(), weights_lp.end());
    for (int i = 0; i < totalVeiculos; ++i) {
        outFile << " " << dist_lp(gen);
    }
    outFile << "\n";

    outFile << "tt";
    // Pesos: [-2, 2-4, 4-8, 8+]
    std::vector<double> weights_tt = {65.0, 20.0, 10.0, 5.0};
    std::discrete_distribution<> dist_tt_category(weights_tt.begin(), weights_tt.end());

    for (int i = 0; i < totalVeiculos; ++i) {
        int category = dist_tt_category(gen);
        int tt_value = 0;
        
        switch (category) {
            case 0: {
                std::uniform_int_distribution<> dist_range(1, 2);
                tt_value = dist_range(gen);
                break;
            }
            case 1: {
                std::uniform_int_distribution<> dist_range(3, 4);
                tt_value = dist_range(gen);
                break;
            }
            case 2: {
                std::uniform_int_distribution<> dist_range(5, 8);
                tt_value = dist_range(gen);
                break;
            }
            case 3: {
                std::uniform_int_distribution<> dist_range(9, 12);
                tt_value = dist_range(gen);
                break;
            }
        }
        outFile << " " << tt_value;
    }
    outFile << "\n";

    outFile << "vt";
    // Pesos: {Emergência, Público, Comercial, Particular}
    std::vector<double> weights_vt = {5.0, 10.0, 15.0, 70.0};
    std::discrete_distribution<> dist_vt(weights_vt.begin(), weights_vt.end());
    for (int i = 0; i < totalVeiculos; ++i) {
        outFile << " " << dist_vt(gen);
    }
    outFile << "\n";

    outFile.close();
    std::cout << "Arquivo random.txt gerado com sucesso!\n";
}

int
main (int argc, char *argv[]) {
	Time::SetResolution (Time::NS);

	CommandLine cmd;
	cmd.Parse (argc, argv);


    // open log file for output
    //	std::ofstream os;
    //	os.open (MinuetConfig::LOG_FILE.c_str());

	// Enable Packet Metadata to check Type IDs of the Headers upon receiving messages
	PacketMetadata::Enable();

	/*-------------------- Configure Logging System ------------------*/
	LogLevel logLevel = (LogLevel) (LOG_PREFIX_ALL | LOG_LEVEL_FUNCTION);

	// Enable logging
	LogComponentEnable("MINUETScenarioTest", logLevel);
	LogComponentEnable ("AppMINUET", logLevel);
//	LogComponentEnable ("MINUETHelper", logLevel);
//	LogComponentEnable ("Ns2MobilityHelper", logLevel);
//	LogComponentEnable ("MinuetConfig", logLevel);
//	LogComponentEnable ("DetectionLayer", logLevel);
//	LogComponentEnable ("AnnouncementLayer", logLevel);
//	LogComponentEnable ("MINUET", logLevel);
//	LogComponentEnable ("CommunicationLayer", logLevel);
//	LogComponentEnable ("MINUETHeader", logLevel);
//	LogComponentEnable ("MonitoringLayer", logLevel);
//	LogComponentEnable ("DCA", logLevel);
//	LogComponentEnable ("PCTT", logLevel);
//	LogComponentEnable ("PCTTVehicle", logLevel);
//	LogComponentEnable ("SOCIAL", logLevel);
//	LogComponentEnable ("BaseStation", logLevel);
//	LogComponentEnable ("MINUETEvent", logLevel);
//	LogComponentEnable ("ClusteringManager", logLevel);
//	LogComponentEnable ("Socket", logLevel);
//	LogComponentEnable ("Ns2MobilityHelper", logLevel);


	NS_LOG_INFO("/------------------------------------------------\\");
	NS_LOG_INFO(" - SOCIABLE Scenario Test -> Starting - " << MinuetConfig::GetCurrentDateTime());
	NS_LOG_INFO("\\------------------------------------------------/");
	/*---------------------------------------------------------------*/

	//Criar pasta dos logs com o horário de início desta simulação
	MinuetConfig::MakeLogDirectory(); 

	// Define initial and final time to nodes between interval time of the scenario
	vector<vector<double_t>> initialEndFinalTimeNode = MinuetConfig::GetInitialAndFinalTimeByNode(MinuetConfig::TOTAL_NODES, MinuetConfig::TRACE_ACTTIVITY_FILE, MinuetConfig::START_TIME_APP, MinuetConfig::FINAL_TIME_APP);


	// Create all nodes.
	NodeContainer nodeContainer;
	nodeContainer.Create (MinuetConfig::TOTAL_NODES);
	NS_LOG_INFO ("Node Container Created");

	// Create Ns2MobilityHelper with the specified trace log file as parameter
	Ns2MobilityHelper ns2 = Ns2MobilityHelper (MinuetConfig::TRACE_MOBILITY_FILE);
	ns2.Install (); // configure movements for each node, while reading trace file
	NS_LOG_INFO ("Mobility Node Defined");

	// Create all base stations nodes.
	NodeContainer baseStationNodeContainer;
	baseStationNodeContainer.Create (MinuetConfig::TOTAL_BASE_STATION);
	NS_LOG_INFO ("Base Station Container Created");

	// Criar arquivo "random.txt"
	GerarValoresAleatorios();

	/******************* Configure Base Stations *********************/

	vector<Ptr<BaseStation>> baseStationsVector = BaseStationUtils::BaseStationGenerator();

//	for(auto baseStation : baseStationsVector){
//		cout << "Base Station: ID#" << baseStation->GetBaseStationId() << " PosX: " << baseStation->GetPosX() << " PosY: " << baseStation->GetPosY() << endl;
//	}

	for(uint64_t i = 0 ; i < MinuetConfig::TOTAL_BASE_STATION ; i++) {
		MobilityHelper mobility;

		Ptr<ListPositionAllocator> listPositionAllocator = CreateObject<ListPositionAllocator>();
		listPositionAllocator->Add(Vector(baseStationsVector[i]->GetPosX(),
				   	   	   	   	   	   	  baseStationsVector[i]->GetPosY(),
										  0));

		MinuetConfig::AddBasestationPos(Vector(baseStationsVector[i]->GetPosX(), baseStationsVector[i]->GetPosY(), 0));

		mobility.SetPositionAllocator(listPositionAllocator);
		mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");

		mobility.Install(baseStationNodeContainer.Get(i));
	}

	NS_LOG_INFO ("Base Station Positions Defined");

	/***************** END Configure Base Stations ********************/

	// Configure Wifi Channel
	YansWifiChannelHelper wifiChannelHelper = YansWifiChannelHelper::Default ();

	Ptr<YansWifiChannel> wifiChannel = wifiChannelHelper.Create ();
	NS_LOG_INFO ("Wifi Channel Configured");

	// Configure Wifi Physic Layer of the Nodes
	YansWifiPhyHelper wifiPhyLayer =  YansWifiPhyHelper::Default();
	wifiPhyLayer.SetChannel (wifiChannel);
	wifiPhyLayer.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);
	wifiPhyLayer.Set ("TxPowerStart", DoubleValue((double_t)MinuetConfig::TX_POWER_START));
	wifiPhyLayer.Set ("TxPowerEnd", DoubleValue((double_t)MinuetConfig::TX_POWER_END));
	wifiPhyLayer.Set ("TxGain", DoubleValue((double_t)MinuetConfig::TX_GAIN));
	wifiPhyLayer.Set ("RxGain", DoubleValue((double_t)MinuetConfig::RX_GAIN));
	wifiPhyLayer.Set ("EnergyDetectionThreshold", DoubleValue((double_t)MinuetConfig::ENERGY_DETECTION_THRESHOLD));
	wifiPhyLayer.Set ("CcaMode1Threshold", DoubleValue((double_t)MinuetConfig::CCA_MODEL_THRESHOLD));
//	wifiPhyLayer.SetErrorRateModel("ns3::YansErrorRateModel");
	NS_LOG_INFO ("Wifi Physic Layer of the Node Defined");

	// Configure Wifi Physic Layer of the Base Station
	YansWifiPhyHelper wifiPhyLayerBaseStation =  YansWifiPhyHelper::Default();
	wifiPhyLayerBaseStation.SetChannel (wifiChannel);
	wifiPhyLayerBaseStation.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);
	wifiPhyLayerBaseStation.Set ("TxPowerStart", DoubleValue((double_t)MinuetConfig::TX_POWER_START_BASE_STATION));
	wifiPhyLayerBaseStation.Set ("TxPowerEnd", DoubleValue((double_t)MinuetConfig::TX_POWER_END_BASE_STATION));
	wifiPhyLayerBaseStation.Set ("TxGain", DoubleValue((double_t)MinuetConfig::TX_GAIN_BASE_STATION));
	wifiPhyLayerBaseStation.Set ("RxGain", DoubleValue((double_t)MinuetConfig::RX_GAIN_BASE_STATION));
	wifiPhyLayerBaseStation.Set ("EnergyDetectionThreshold", DoubleValue((double_t)MinuetConfig::ENERGY_DETECTION_THRESHOLD_BASE_STATION));
	wifiPhyLayerBaseStation.Set ("CcaMode1Threshold", DoubleValue((double_t)MinuetConfig::CCA_MODEL_THRESHOLD_BASE_STATION));
//	wifiPhyLayerBaseStation.SetErrorRateModel("ns3::YansErrorRateModel");
	NS_LOG_INFO ("Wifi Physic Layer of the Base Station Defined");

	NqosWaveMacHelper nqosWaveMacHelper = NqosWaveMacHelper::Default ();

	Wifi80211pHelper wifi80211pHelper = Wifi80211pHelper::Default ();
	wifi80211pHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
											  "DataMode", StringValue(MinuetConfig::PHYSIC_MODE),
											  "ControlMode", StringValue(MinuetConfig::PHYSIC_MODE));

	Wifi80211pHelper wifi80211pBaseStationHelper = Wifi80211pHelper::Default ();
	wifi80211pBaseStationHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
														 "DataMode", StringValue(MinuetConfig::PHYSIC_MODE_BASE_STATION),
														 "ControlMode", StringValue(MinuetConfig::PHYSIC_MODE_BASE_STATION));


	NetDeviceContainer netDeviceContainer = wifi80211pHelper.Install(wifiPhyLayer, nqosWaveMacHelper, nodeContainer);
	NetDeviceContainer netDeviceBaseStationContainer = wifi80211pBaseStationHelper.Install(wifiPhyLayerBaseStation, nqosWaveMacHelper, baseStationNodeContainer);

	InternetStackHelper internetStackHelper;
	internetStackHelper.Install (nodeContainer);
	internetStackHelper.Install (baseStationNodeContainer);
	NS_LOG_INFO ("Internet Stack Installed in Nodes and Base Stations");

	// Assign IP Address
	Ipv4AddressHelper ipv4;
	ipv4.SetBase(MinuetConfig::IP.c_str(), MinuetConfig::MASK.c_str());
	Ipv4InterfaceContainer a = ipv4.Assign (netDeviceContainer);
	Ipv4InterfaceContainer i = ipv4.Assign (netDeviceBaseStationContainer);
	NS_LOG_INFO ("IP Address Assigned in Node and Base Stations");

	vector<Ptr<EventDetected>> eventsOccurenceVector = EventUtils::EventsGenerator(nodeContainer);

	// Create LOG_File_MINUETCenario.log
	PrintInLog(eventsOccurenceVector, baseStationsVector);

	for(auto event : eventsOccurenceVector) {
		cout << "Event ID: " << event->GetEventId() << endl;
		cout << "Occurence Time: " << event->GetOccurenceTime() << endl;
		cout << "Is Fixed: " << event->IsFixed() << endl;
		cout << "Position: " << event->GetMobilityModel()->GetPosition() << endl;
		cout << "Duration: " << event->GetDuration() << endl;
	}

	// Configure callback for logging
//	Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange", MakeBoundCallback (&CourseChange, &os));

//	Ptr<Node> event = CreateObject<Node>();
//	AnimationInterface anim (MinuetConfig::TRACE_NETANIM_FILE);
//	anim.SetMaxPktsPerTraceFile(500000);
//
//	/////// Event ////////
//	MobilityHelper mobility;
//
//	Ptr<ListPositionAllocator> listPositionAllocator = CreateObject<ListPositionAllocator>();
//	listPositionAllocator->Add(Vector(6410.70, 6806.54,0));
//	mobility.SetPositionAllocator(listPositionAllocator);
//	mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
//	mobility.Install(event);
//	anim.UpdateNodeColor(event, 0, 0, 255);
	////////////////////////

	ApplicationContainer apps;
	for (uint32_t u = 0 ; u < nodeContainer.GetN() ; u++) {
		MINUETHelper minuetHelper;
		apps.Add(minuetHelper.Install(nodeContainer.Get(u), eventsOccurenceVector, initialEndFinalTimeNode[u]));
	}
	NS_LOG_INFO ("AppMINUET Installed in Nodes");

	// Install apps in base station nodes
	for (uint32_t u = 0 ; u < baseStationNodeContainer.GetN() ; u++) {
		BaseStationHelper baseStationHelper;
		apps.Add(baseStationHelper.Install(baseStationNodeContainer.Get(u)));
		//anim.UpdateNodeColor(baseStationNodeContainer.Get(u), 0, 255, 0);
	}

	NS_LOG_INFO ("AppBaseStation Intalled in Base Stations");

	apps.Start (Seconds(MinuetConfig::START_TIME_APP));
	apps.Stop (Seconds(MinuetConfig::FINAL_TIME_APP));

	Simulator::Stop (Seconds (MinuetConfig::TOTAL_TIME_SIMULATION));
	Simulator::Run ();
	Simulator::Destroy ();

	NS_LOG_INFO("/------------------------------------------------\\");
	NS_LOG_INFO(" - SOCIABLE Scenario Test -> Terminated - " << MinuetConfig::GetCurrentDateTime());
	NS_LOG_INFO("\\------------------------------------------------/");
	/*---------------------------------------------------------------*/

	// string email = "sendmail alisson.yury@cc.ci.ufpb.br  < " + MinuetConfig::LOG_FILE_MINUETCENARIO;
	// system(email.c_str());

	return EXIT_SUCCESS;
}

void
PrintInLog(vector<Ptr<EventDetected>> eventsOccurenceVector, vector<Ptr<BaseStation>> baseStationsVector){
	ofstream os;
	os.open (MinuetConfig::LOG_FILE_MINUETCENARIO.c_str(), ofstream::out | ofstream::app);

	if(MinuetConfig::wSTR){
		if(MinuetConfig::wAS){
			if(MinuetConfig::wAm){
				os << "Subject: Simulação STR+AS+Am";
			} else {
				os << "Subject: Simulação STR+AS";
			}
		} else if(MinuetConfig::wAm){
			os << "Subject: Simulação STR+Am";
		} else {
			os << "Subject: Simulação STR";
		}
	} else if(MinuetConfig::wAS){
		if(MinuetConfig::wAm){
			os << "Subject: Simulação AS+Am";
		} else {
			os << "Subject: Simulação AS";
		}
	} else if(MinuetConfig::wAm){
		os << "Subject: Simulação Am";
	} else {
		os << "Subject: Simulação sem estratégia social!";
	}

	if (MinuetConfig::START_TIME_APP == 19790)
		os << "-BD" << endl;
	else
		os << "-AD" << endl;


	os << "####### MINUETCenario Starting " << MinuetConfig::GetCurrentDateTime() << "!  #######" << endl;
	os << endl;
	os << "#### minuet-utils Configurations Parameters ####" << endl;
	os << "FLOOR_SIZE_X = " << MinuetConfig::FLOOR_SIZE_X << endl;
	os << "FLOOR_SIZE_Y = " << MinuetConfig::FLOOR_SIZE_Y << endl;
	os << "FLOOR_SIZE_X_MIN = " << MinuetConfig::FLOOR_SIZE_X_MIN << endl;
	os << "FLOOR_SIZE_Y_MIN = " << MinuetConfig::FLOOR_SIZE_Y_MIN << endl;
	os << "TOTAL_NODES = " << MinuetConfig::TOTAL_NODES << endl;
	os << "TOTAL_TIME_SIMULATION = " << MinuetConfig::TOTAL_TIME_SIMULATION << endl;
	os << "START_TIME_APP = " << MinuetConfig::START_TIME_APP << endl;
	os << "FINAL_TIME_APP = " << MinuetConfig::FINAL_TIME_APP << endl;
	os << "TRACE_MOBILITY_FILE = " << MinuetConfig::TRACE_MOBILITY_FILE << endl;
	os << "TRACE_ACTTIVITY_FILE = " << MinuetConfig::TRACE_ACTTIVITY_FILE << endl;
	os << "TRACE_CONFIG_FILE = " << MinuetConfig::TRACE_CONFIG_FILE << endl;
	os << "TRACE_NETANIM_FILE = " << MinuetConfig::TRACE_NETANIM_FILE << endl;
	os << "LOG_FILE_APP_MINUET = " << MinuetConfig::LOG_FILE_APP_MINUET << endl;
	os << "LOG_FILE_MINUET = " << MinuetConfig::LOG_FILE_MINUET << endl;
	os << "LOG_FILE_DETECTION_LAYER = " << MinuetConfig::LOG_FILE_DETECTION_LAYER << endl;
	os << "LOG_FILE_ANNOUNCEMENT_LAYER = " << MinuetConfig::LOG_FILE_ANNOUNCEMENT_LAYER << endl;
	os << "LOG_FILE_MONITORING_LAYER = " << MinuetConfig::LOG_FILE_MONITORING_LAYER << endl;
	os << "LOG_FILE_COMMUNICATION_LAYER = " << MinuetConfig::LOG_FILE_COMMUNICATION_LAYER << endl;
	os << "LOG_FILE_CLUSTERING_MANAGER = " << MinuetConfig::LOG_FILE_CLUSTERING_MANAGER << endl;
	os << "LOG_FILE_CLUSTERING_ALGORITHM = " << MinuetConfig::LOG_FILE_CLUSTERING_ALGORITHM << endl;
	os << "LOG_FILE_BASE_STATIONS = " << MinuetConfig::LOG_FILE_BASE_STATIONS << endl;

	os << endl;
	os << "#### Configure Parameters Wifi Physic Layer ####" << endl;
	os << "TX_POWER_START = " << MinuetConfig::TX_POWER_START << endl;
	os << "TX_POWER_END = " << MinuetConfig::TX_POWER_END << endl;
	os << "TX_GAIN = " << MinuetConfig::TX_GAIN << endl;
	os << "RX_GAIN = " << MinuetConfig::RX_GAIN << endl;
	os << "ENERGY_DETECTION_THRESHOLD = " << MinuetConfig::ENERGY_DETECTION_THRESHOLD << endl;
	os << "CCA_MODEL_THRESHOLD = " << MinuetConfig::CCA_MODEL_THRESHOLD << endl;
	os << "PHYSIC_MODE = " << MinuetConfig::PHYSIC_MODE << endl;
	os << endl;
	os << "#### Configure Parameters Wifi Transport Layer ####" << endl;
	os << "IP = " << MinuetConfig::IP << endl;
	os << "MASK = " << MinuetConfig::MASK << endl;
 	os << endl;
 	os << "#### Configure Parameters Sockets ####" << endl;
 	os << "SEND_CONTROL_PROTOCOL = " << MinuetConfig::SEND_CONTROL_PROTOCOL << endl;
 	os << "LISTEN_CONTROL_PROTOCOL = " << MinuetConfig::LISTEN_CONTROL_PROTOCOL << endl;
 	os << "SEND_DATA_PROTOCOL = " << MinuetConfig::SEND_DATA_PROTOCOL << endl;
 	os << "LISTEN_DATA_PROTOCOL = " << MinuetConfig::LISTEN_DATA_PROTOCOL << endl;
 	os << "PORT_DATA = " << MinuetConfig::PORT_DATA << endl;
 	os << "PORT_CONTROL = " << MinuetConfig::PORT_CONTROL << endl;
 	os << endl;
 	os << "#### Configure Parameters Detection Layer ####" << endl;
 	os << "CHECK_OCCURRENCE_EVENT_INTERVAL = " << MinuetConfig::CHECK_OCCURRENCE_EVENT_INTERVAL << endl;
 	os << endl;
 	os << "#### Configure Parameters Announcement Layer ####" << endl;
 	os << "ANNOUNCEMENT_DISSEMINATION_LIMIT_TIME = " << MinuetConfig::ANNOUNCEMENT_DISSEMINATION_LIMIT_TIME << endl;
 	os << "ANNOUNCEMENT_TIME_LIMIT = " << MinuetConfig::ANNOUNCEMENT_TIME_LIMIT << endl;
 	os << "ANNOUNCEMENT_INTERVAL = " << MinuetConfig::ANNOUNCEMENT_INTERVAL << endl;
 	os << endl;
 	os << "#### Configure Parameters Monitoring Device (Camera) ####" << endl;
 	os << "MAX_VISION_FIELD_ANGLE = " << MinuetConfig::MAX_VISION_FIELD_ANGLE << endl;
 	os << "MAX_VISION_FIELD_DISTANCE = " << MinuetConfig::MAX_VISION_FIELD_DISTANCE << endl;
 	os << endl;
 	os << "#### Configure Parameters Monitoring Layer ####" << endl;
 	os << "PACKET_PAYLOAD_SIZE = " << MinuetConfig::PACKET_PAYLOAD_SIZE << endl;
 	os << "TIME_EXPIRE_GATEWAY = " << MinuetConfig::TIME_EXPIRE_GATEWAY << endl;
 	os << "MONITORING_DISSEMINATION_LIMIT_TIME = " << MinuetConfig::MONITORING_DISSEMINATION_LIMIT_TIME << endl;
 	os << "VIDEO_TRACE_FILE_NAME = " << MinuetConfig::VIDEO_TRACE_FILE_NAME << endl;
 	os << endl;
	os << "#### Configure Parameters Event ####" << endl;
	os << "TRACE_EVENTS_FILE = " << MinuetConfig::TRACE_EVENTS_FILE <<endl;
	for(uint32_t f=0; f < eventsOccurenceVector.size(); f++){
		os << "EventId: " << eventsOccurenceVector[f]->GetEventId() << " - Initial Time: " << eventsOccurenceVector[f]->GetOccurenceTime() << " - IsFixed: " << eventsOccurenceVector[f]->IsFixed() << " - PosX: " << eventsOccurenceVector[f]->GetMobilityModel()->GetPosition().x << " - PosY: " << eventsOccurenceVector[f]->GetMobilityModel()->GetPosition().y << " - PosZ: " << eventsOccurenceVector[f]->GetMobilityModel()->GetPosition().z << " - Duration: " << eventsOccurenceVector[f]->GetDuration() << endl;
	}
	os << endl;
	os << "#### Configure Parameters Base Stations ####" << endl;
	os << "TRACE_BASE_STATIONS_FILE = " << MinuetConfig::TRACE_BASE_STATIONS_FILE << endl;
	os << "TOTAL_BASE_STATION = " << MinuetConfig::TOTAL_BASE_STATION << endl;
	os << "ANNOUNCEMENT_BASE_STATION_INTERVAL = " << MinuetConfig::ANNOUNCEMENT_BASE_STATION_INTERVAL << endl;
	os << "TX_POWER_START_BASE_STATION = " << MinuetConfig::TX_POWER_START_BASE_STATION << endl;
	os << "TX_POWER_END_BASE_STATION = " << MinuetConfig::TX_POWER_END_BASE_STATION << endl;
	os << "TX_GAIN_BASE_STATION = " << MinuetConfig::TX_GAIN_BASE_STATION << endl;
	os << "RX_GAIN_BASE_STATION = " << MinuetConfig::RX_GAIN_BASE_STATION << endl;
	os << "ENERGY_DETECTION_THRESHOLD_BASE_STATION = " << MinuetConfig::ENERGY_DETECTION_THRESHOLD_BASE_STATION << endl;
	os << "CCA_MODEL_THRESHOLD_BASE_STATION = " << MinuetConfig::CCA_MODEL_THRESHOLD_BASE_STATION << endl;
	os << "PHYSIC_MODE_BASE_STATION = " << MinuetConfig::PHYSIC_MODE_BASE_STATION << endl;
	os << "LISTEN_DATA_PROTOCOL_BASE_STATION = " << MinuetConfig::LISTEN_DATA_PROTOCOL_BASE_STATION << endl;
	for(uint32_t f=0; f < baseStationsVector.size(); f++){
		os << "BaseStationId: " << baseStationsVector[f]->GetBaseStationId() << " - PosX: " << baseStationsVector[f]->GetPosX() << " - PosY: " << baseStationsVector[f]->GetPosY() << endl;
	}
	os << endl;
	os << "#### Configure Parameters Clustering Manager ####" << endl;
	os << "SELECTED_CLUSTERING_ALGORITHM = " << MinuetConfig::SELECTED_CLUSTERING_ALGORITHM << endl;
	os << "#### Weights of Social Metrics ####" << endl;
	os << "STR WEIGHT = " << MinuetConfig::wSTR << endl;
	os << "SOCIAL ACTIVITY WEIGHT = " << MinuetConfig::wAS << endl;
	os << "FRIENDSHIP WEIGHT = " << MinuetConfig::wAm << endl;
	os.close();
}
