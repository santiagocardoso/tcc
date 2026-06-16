/*
 * minuet-utils.h
 *
 *  Created on: 30 de out de 2018
 *      Author: everaldo
 */

#ifndef MINUET_UTILS_H_
#define MINUET_UTILS_H_

#include <iostream>
#include <regex>
#include <random>
#include <string>
#include "ns3/core-module.h"

#define PI 3.14159265

using namespace std;

namespace ns3 {

class MinuetConfig {

public:
	static vector<double_t> RandonNumbersGeneratorBetweenMaxAndMin(uint64_t totalValues, uint64_t maxValue, uint64_t minValue);
	static vector<double_t> DoubleRandonNumbersGeneratorBetweenMaxAndMin(uint32_t totalValues, double_t maxValue, double_t minValue);
	static int RandonNumberGeneratorBetweenRange(uint32_t min, uint32_t max);
	static vector<vector<double_t>> GetInitialAndFinalTimeByNode(uint64_t totalNodes, string pathTraceActivityFile, double_t initialTime, double_t finalTime);
	static string GetCurrentDateTime();
	static void MakeLogDirectory();

	static uint32_t GetTotalNodes();
	static void SetTotalNodes(uint32_t totalN);

	static uint16_t GetAmizade(uint16_t i, uint16_t j);
	static void SetAmizade(uint16_t i, uint16_t j, uint16_t valor);

	static void AddBasestationPos(Vector position);
	static vector<Vector>::iterator GetBasestationPosIterator();
	static vector<Vector>::iterator GetBasestationPosEnd();

	static uint16_t GetMenorId();


	/**************************** Configuration Parameters ***********************************/
	static constexpr double_t FLOOR_SIZE_X = 8155.47; //      Define floor size in X axis
	static constexpr double_t FLOOR_SIZE_Y = 8054.39; //      Define floor size in Y axis
	static constexpr double_t FLOOR_SIZE_X_MIN = 4158.01;     //      Define floor size minimal in X axis (min-X)
	static constexpr double_t FLOOR_SIZE_Y_MIN = 6178.78;     //      Define floor size minimal in Y axis (min-Y)
	static constexpr uint32_t TOTAL_NODES = 2614; // Total nodes in scenario
	static constexpr uint64_t TOTAL_TIME_SIMULATION = 31321; //87903.0;	// Total steptime simulation
	static constexpr double_t START_TIME_APP = 30600.00;	// Start time of the application
	static constexpr double_t FINAL_TIME_APP = 31321.0;	// Final time of the application

	static const string TRACE_MOBILITY_FILE;	// Path Trace Mobility File  -- OBS.: Defined in .cc
	static const string TRACE_ACTTIVITY_FILE;	// Path Trace Activity File  -- OBS.: Defined in .cc
	static const string TRACE_CONFIG_FILE;	// Path Trace Config File  -- OBS.: Defined in .cc
	static const string TRACE_NETANIM_FILE;	// Path Trace NetAnim File  -- OBS.: Defined in .cc
	static const string LOG_FILE_APP_MINUET;	// Path log file of the App MINUET -- OBS.: Defined in .cc
	static const string LOG_FILE_MINUET;	// Path log file of the MINUET -- OBS.: Defined in .cc
	static const string LOG_FILE_DETECTION_LAYER;	// Path log file of the Detection Layer -- OBS.: Defined in .cc
	static const string LOG_FILE_ANNOUNCEMENT_LAYER;	// Path log file of the Announcement Layer -- OBS.: Defined in .cc
	static const string LOG_FILE_MONITORING_LAYER;	// Path log file of the Monitoring Layer -- OBS.: Defined in .cc
	static const string LOG_FILE_COMMUNICATION_LAYER;	// Path log file of the Communication Layer -- OBS.: Defined in .cc
	static const string LOG_FILE_CLUSTERING_MANAGER;	// Path log file of the Clustering Manager -- OBS.: Defined in .cc
	static const string LOG_FILE_CLUSTERING_ALGORITHM;	// Path log file of the Clustering Algorithm -- OBS.: Defined in .cc
	static const string LOG_FILE_BASE_STATIONS;	// Path log file to Base Stations -- OBS.: Defined in .cc
	static const string LOG_FILE_VELOCITIES;	// Path log file to Velocities -- OBS.: Defined in .cc
	static const string LOG_FILE;
	static const string LOG_FILE_MINUETCENARIO; // Path log file to MINUETCenario -- OBS.: Defined in .cc

	// Configure Parameters Dump Files
    static const string SD_FILE_PATH;
    static const string RD_FILE_PATH;
    static const string SD_FILE_NAME;
    static const string RD_FILE_NAME;
    static const string TOTAL_FRAMES_SENT_FILE_NAME;

	// Configure Parameters Wifi Physic Layer
	static constexpr double_t TX_POWER_START = 28.0;	// The minimum available transmission power level (dBm).
	static constexpr double_t TX_POWER_END = 28.0;	// The maximum available transmission power level (dBm).
	static constexpr double_t TX_GAIN = 9.0;	// The transmission gain (dB).
	static constexpr double_t RX_GAIN = 9.0;	// The reception gain (dB).
	static constexpr double_t ENERGY_DETECTION_THRESHOLD = -61.8;	// The energy of a received signal should be higher than this threshold (dbm) to allow the PHY layer to detect the signal.
	static constexpr double_t CCA_MODEL_THRESHOLD = -64.8;	// The CCA threshold (dBm). The energy of a received signal should be higher than this threshold to allow the PHY layer to declare CCA BUSY state.
	static const string PHYSIC_MODE;	// Configure Physic Mode -- OBS.: Defined in .cc

	// Configure Parameters Wifi Transport Layer
	static const string IP;	// Define network IP -- OBS.: Defined in .cc
	static const string MASK;	// Define network mask -- OBS.: Defined in .cc

	// Configure Parameters Sockets
	static const string SEND_CONTROL_PROTOCOL;	// Type Id of the Send Control Protocol -- OBS.: Defined in .cc
	static const string LISTEN_CONTROL_PROTOCOL;	// Type Id of the Listen Control Protocol -- OBS.: Defined in .cc
	static const string SEND_DATA_PROTOCOL;	// Type Id of the Send Data Protocol -- OBS.: Defined in .cc
	static const string LISTEN_DATA_PROTOCOL;	// Type Id of the Listen Data Protocol -- OBS.: Defined in .cc
	static constexpr uint16_t PORT_DATA = 3355;	// Port of the data
	static constexpr uint16_t PORT_CONTROL = 4466;	// Port of the control

	// Configure Parameters Detection Layer
	static constexpr uint64_t CHECK_OCCURRENCE_EVENT_INTERVAL = 100000000;	// Define the checking occurrence event interval in nanoseconds // 5000000

	// Configure Parameters Announcement Layer
	static constexpr uint64_t ANNOUNCEMENT_DISSEMINATION_LIMIT_TIME = 3000000000; // Define maximum announcement time limit in nanosecond 15000000
	static constexpr uint64_t ANNOUNCEMENT_TIME_LIMIT = 5000000;
	static constexpr uint64_t ANNOUNCEMENT_INTERVAL = 1000000000; // Define the announcement frequency value 500000000

	// Configure Parameters Monitoring Device (Camera)
	static constexpr double_t MAX_VISION_FIELD_ANGLE = 180.0;	// Define maximum vision field angle of the monitoring device (In degrees)CHECK_OCCURRENCE_EVENT_INTERVAL 
	static constexpr double_t MAX_VISION_FIELD_DISTANCE = 50.0;	// Define the maximum distance between node and event to monitoring (In meters)

	// Configure Parameters Monitoring Layer
	static constexpr uint32_t PACKET_PAYLOAD_SIZE = 1000;	// Define the Packet Payload Size (In bytes)
	static constexpr double_t TIME_EXPIRE_GATEWAY = 30000000000;	// Total tolerance time during gateway state (in nanoseconds)
	// 600000000
	static constexpr uint64_t MONITORING_DISSEMINATION_LIMIT_TIME = 30000000000; // Define maximum monitoring dissemination time limit in nanosecond
	// 600000000
	static const string VIDEO_TRACE_FILE_NAME; // Define the video trace file name to monitoring layer. OBS.: Defined in .cc

	// Configure Parameters Event
	static const string TRACE_EVENTS_FILE;	// Path the trace events file name -- OBS.: Defined in .cc

	// Configure Parameters Base Stations
	static const string TRACE_BASE_STATIONS_FILE;	// Path the trace base stations file name -- OBS.: Defined in .cc
	static constexpr uint64_t TOTAL_BASE_STATION = 1;  // Defines the total base stations in scenario
	static constexpr double_t ANNOUNCEMENT_BASE_STATION_INTERVAL = 500000000;	// Interval to announcement base station message in nanoseconds

	static constexpr double_t TX_POWER_START_BASE_STATION = 28.0;	// The minimum available transmission power level (dBm).
	static constexpr double_t TX_POWER_END_BASE_STATION = 28.0;	// The maximum available transmission power level (dBm).
	static constexpr double_t TX_GAIN_BASE_STATION = 9.0;	// The transmission gain (dB).
	static constexpr double_t RX_GAIN_BASE_STATION = 9.0;	// The reception gain (dB).
	static constexpr double_t ENERGY_DETECTION_THRESHOLD_BASE_STATION = -61.8;	// The energy of a received signal should be higher than this threshold (dbm) to allow the PHY layer to detect the signal.
	static constexpr double_t CCA_MODEL_THRESHOLD_BASE_STATION = -64.8;	// The CCA threshold (dBm). The energy of a received signal should be higher than this threshold to allow the PHY layer to declare CCA BUSY state.
	static const string PHYSIC_MODE_BASE_STATION;	// Configure Physic Mode -- OBS.: Defined in .cc
	static const string LISTEN_DATA_PROTOCOL_BASE_STATION;	// Type Id of the Listen Data Protocol -- OBS.: Defined in .cc

	/// Configure Parameters Clustering Manager
	enum CLUSTERING_ALGORITHM {dca, msdc, pctt, social};	// Clustering Algorithms Types
	static const CLUSTERING_ALGORITHM SELECTED_CLUSTERING_ALGORITHM = social;
	static constexpr double_t wSTR = 0;
	static constexpr double_t wAS = 0.5;
	static constexpr double_t wAm = 0.5;
	/*****************************************************************************************/

};

//uint32_t MinuetConfig::totalNode;

} // namespace ns3

#endif /* MINUET_UTILS_H_ */
