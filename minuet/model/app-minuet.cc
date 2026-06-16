/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "app-minuet.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("AppMINUET");
NS_OBJECT_ENSURE_REGISTERED (AppMINUET);

TypeId AppMINUET::GetTypeId(void) {
	static TypeId tid =
			TypeId("ns3::AppMINUET").SetParent<Application>()
			.AddConstructor<AppMINUET>()
			.AddAttribute("InitialNodeTime",
				"Initial Node Time", TimeValue(),
				MakeTimeAccessor(&AppMINUET::m_initialNodeTime),
				MakeTimeChecker())
			.AddAttribute("EndNodeTime",
				"End Node Time", TimeValue(),
				MakeTimeAccessor(&AppMINUET::m_endNodeTime),
				MakeTimeChecker());

	return tid;
}

AppMINUET::AppMINUET() {
	NS_LOG_FUNCTION (this);
	m_minuet = nullptr;
}

AppMINUET::~AppMINUET() {
	NS_LOG_FUNCTION (this);
}

void
AppMINUET::StartApplication (void) {
	NS_LOG_FUNCTION (this);

	if((m_initialNodeTime != m_endNodeTime) && (m_initialNodeTime < m_endNodeTime)) { //
		if ((Simulator::Now() <= m_initialNodeTime) && (Simulator::Now() <= m_endNodeTime)) {
			Simulator::Schedule(m_initialNodeTime - Simulator::Now(), &AppMINUET::StartMINUET, this);
			Simulator::Schedule(m_endNodeTime - Simulator::Now(), &AppMINUET::StopMINUET, this);
		} else {
			NS_LOG_WARN("### Impossible start MINUET: InitialTimeNode is bigger or iqual than EndNodeTime!");
		}
	}
}

void
AppMINUET::StopApplication (void) {
	NS_LOG_FUNCTION (this);

}

void AppMINUET::StartMINUET() {
	NS_LOG_FUNCTION (this);

	if (!m_minuet && !m_eventsOccurenceVector.empty()) {
		m_minuet = CreateObjectWithAttributes<MINUET>("Node", PointerValue(GetNode()));
		m_minuet->Start(m_eventsOccurenceVector); // Start MINUET Technique
	} else {
		NS_LOG_INFO("### Impossible start MINUET: MINUET already created!");
	}
}

void AppMINUET::StopMINUET() {
	NS_LOG_FUNCTION (this);
	NS_LOG_INFO("Stop MINUET");

	if (m_minuet) {
		m_minuet->Stop(); // Stop MINUET Technique
	} else {
		NS_LOG_INFO("### Impossible stop MINUET: MINUET not created!");
	}
}


void
AppMINUET::SetEventsOccurenceVector(const vector<Ptr<EventDetected> >& eventsOccurenceVector) {
	NS_LOG_FUNCTION (this);
	m_eventsOccurenceVector = eventsOccurenceVector;
}

void
AppMINUET::SetInitialNodeTime(Time initialNodeTime) {
	m_initialNodeTime = initialNodeTime;
}

void
AppMINUET::SetEndNodeTime(Time endNodeTime) {
	m_endNodeTime = endNodeTime;
}

void AppMINUET::PrintInLog(string message) {
	ofstream os;
	os.open (MinuetConfig::LOG_FILE_APP_MINUET.c_str(), ofstream::out | ofstream::app);
	os << Simulator::Now().GetNanoSeconds() << "ns - AppMINUET - Node #" << m_node->GetId() << ": " << message << endl;
	os.close();
}

}
