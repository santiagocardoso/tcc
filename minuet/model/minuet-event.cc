/*
 * minuet-event.cc
 *
 *  Created on: 18 de out de 2018
 *      Author: everaldo
 */

#include "minuet-event.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MINUETEvent");
NS_OBJECT_ENSURE_REGISTERED (EventDetected);

TypeId EventDetected::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::EventDetected").SetParent<Object>()
						.AddConstructor<EventDetected>()
						.AddAttribute("EventId",
								"The Event Id", UintegerValue(),
								MakeUintegerAccessor(&EventDetected::m_eventId),
								MakeUintegerChecker<uint32_t>())
						.AddAttribute("IsFixed",
								"The Evento is Fixed or Mobility", BooleanValue(),
								MakeBooleanAccessor(&EventDetected::m_fixed),
								MakeBooleanChecker())
						.AddAttribute("MobilityModel",
								"The MobilityModel of the Event in Occurence", PointerValue(),
								MakePointerAccessor(&EventDetected::m_mobilityModel),
								MakePointerChecker<MobilityModel>())
						.AddAttribute("OccurrenceTime",
								"The Occurence Time of the Event", TimeValue(),
								MakeTimeAccessor(&EventDetected::m_occurenceTime),
								MakeTimeChecker())
						.AddAttribute("Duration",
								"The Duration of the Detection", TimeValue(),
								MakeTimeAccessor(&EventDetected::m_duration),
								MakeTimeChecker())
						.AddAttribute("NodeEvent",
								"The Node of the Event ", UintegerValue(),
								MakeUintegerAccessor(&EventDetected::m_nodeEventId),
								MakeUintegerChecker<uint32_t>());

	return tid;
}

TypeId
EventDetected::GetInstanceTypeId(void) const {
	return GetTypeId();
}

EventDetected::EventDetected() {
	NS_LOG_FUNCTION(this);

	// Define defaults values
	m_fixed = true;
}

EventDetected::~EventDetected() {
	NS_LOG_FUNCTION(this);
}

uint32_t
EventDetected::GetEventId() const {
	return m_eventId;
}

void
EventDetected::SetEventId(uint32_t eventId) {
	m_eventId = eventId;
}

uint32_t
EventDetected::GetNodeEventId() const {
	return m_nodeEventId;
}

void
EventDetected::SetNodeEventId(uint32_t nodeEvent) {
	m_nodeEventId = nodeEvent;
}

const Time&
EventDetected::GetOccurenceTime() const {
	return m_occurenceTime;
}

void
EventDetected::SetOccurenceTime(const Time& occurenceTime) {
	m_occurenceTime = occurenceTime;
}

const Time&
EventDetected::GetDetectionTime() const {
	return m_detectionTime;
}

void
EventDetected::SetDetectionTime(const Time& detectionTime) {
	m_detectionTime = detectionTime;
}

const Time&
EventDetected::GetDuration() const {
	return m_duration;
}

void
EventDetected::SetDuration(const Time& duration) {
	m_duration = duration;
}

bool
EventDetected::IsFixed() const {
	return m_fixed;
}

void
EventDetected::SetFixed(bool fixed) {
	m_fixed = fixed;
}


const Ptr<MobilityModel>&
EventDetected::GetMobilityModel() const {
	return m_mobilityModel;
}

void
EventDetected::SetMobilityModel(const Ptr<MobilityModel>& mobilityModel) {
	m_mobilityModel = mobilityModel;
}

const Time&
EventDetected::GetLastUpdated() const {
	return m_lastUpdated;
}

void
EventDetected::SetLastUpdated(const Time& lastUpdated) {
	m_lastUpdated = lastUpdated;
}

/////////////////////////////////////////////////////////////////////////

vector<Ptr<EventDetected>>
EventUtils::EventsGenerator(NodeContainer nodeContainer) {
	vector<Ptr<EventDetected>> eventsOccurenceVector;
	string line;
	ifstream in_file;
	ofstream out_file;
	smatch sm1, sm2;
	string::size_type sz;
	Ptr<MobilityModel> mobilityModel;

	regex re1 ("[0-9.-]+");

	in_file.open((MinuetConfig::TRACE_EVENTS_FILE).c_str());
	if(!in_file.good()) {
		cerr << "## ERROR: In EventsGenerator file '" << MinuetConfig::TRACE_EVENTS_FILE << "' Not Found!!" << endl;
		exit(EXIT_FAILURE);
	}

	if(in_file.is_open()){
		while(getline(in_file, line)) {

			//Get Event Id
			regex_search (line,sm1,re1);
			uint32_t eventId = stod(sm1[0], &sz);
			line = sm1.suffix().str();

			// Get Occurence Time
			regex_search (line,sm1,re1);
			uint64_t occurrenceTime = stod(sm1[0], &sz);
			line = sm1.suffix().str();

//			cout << " Occurence Time: " << occurrenceTime << endl;

			//Get Mobility - IsFixed (0 OR 1)
			regex_search (line,sm1,re1);
			bool isFixed;
			istringstream(sm1[0]) >> isFixed;
			line = sm1.suffix().str();

//			cout << " Event is fixed: " << isFixed << endl;

			/// Get Position ///
			regex_search (line,sm1,re1);
			double_t positionX = stod(sm1[0], &sz);
			line = sm1.suffix().str();

			regex_search (line,sm1,re1);
			double_t positionY = stod(sm1[0], &sz);
			line = sm1.suffix().str();

			regex_search (line,sm1,re1);
			double_t positionZ = stod(sm1[0], &sz);
			line = sm1.suffix().str();

			Vector position = Vector(positionX, positionY, positionZ);

//			cout << "Occurence Position: (" << positionX << ", " << positionY << ", " << positionZ << ")" << endl;
			///////////////////

			//Get Duration
			regex_search (line,sm1,re1);
			double_t duration = stod(sm1[0], &sz);
			line = sm1.suffix().str();

//			cout << " Duration: " << duration << endl;

			//Get nodeEvent
			regex_search (line,sm1,re1);
			uint32_t nodeEvent = stod(sm1[0], &sz);
			line = sm1.suffix().str();

			if(!isFixed){
				mobilityModel = nodeContainer.Get(nodeEvent)->GetObject<MobilityModel>();
			}else{
				mobilityModel =  CreateObject<ConstantVelocityMobilityModel>();
				mobilityModel->SetAttribute("Position", VectorValue(position));
			}

			eventsOccurenceVector.push_back(CreateEvent(eventId, occurrenceTime, isFixed, duration, mobilityModel, nodeEvent));
		}
	}

	return eventsOccurenceVector;
}

Ptr<EventDetected>
EventUtils::CreateEvent(uint32_t eventId, double_t occurenceTime, bool isFixed, double_t duration, Ptr<MobilityModel> mobilityModel, uint32_t nodeEvent) {
	Ptr<EventDetected> eventDetected = CreateObjectWithAttributes<EventDetected>
												("EventId", UintegerValue(eventId),
												 "OccurrenceTime", TimeValue(Seconds(occurenceTime)),
												 "IsFixed", BooleanValue(isFixed),
												 "Duration", TimeValue(Seconds(duration)),
												 "MobilityModel", PointerValue(mobilityModel),
												 "NodeEvent", UintegerValue(nodeEvent));
	return eventDetected;
}

} // namespace ns3
