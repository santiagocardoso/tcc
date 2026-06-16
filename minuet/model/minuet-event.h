/*
 * minuet-event.h
 *
 *  Created on: 18 de out de 2018
 *      Author: everaldo
 */

#ifndef MINUET_EVENT_H_
#define MINUET_EVENT_H_

#include <regex>
#include <iostream>
#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "minuet-utils.h"

using namespace std;

namespace ns3 {

class EventDetected : public Object {
public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId(void) const;

	EventDetected();
	~EventDetected();

	uint32_t GetEventId() const;
	void SetEventId(uint32_t eventId);

	uint32_t GetNodeEventId() const;
	void SetNodeEventId(uint32_t NODE_EVENT);

	bool IsFixed() const;
	void SetFixed(bool isFixed);

	const Ptr<MobilityModel>& GetMobilityModel() const;
	void SetMobilityModel(const Ptr<MobilityModel>& mobilityModel);

	const Time& GetOccurenceTime() const;
	void SetOccurenceTime(const Time& occurenceTime);

	const Time& GetDetectionTime() const;
	void SetDetectionTime(const Time& stepTime);

	const Time& GetLastUpdated() const;
	void SetLastUpdated(const Time& lastUpdated);

	const Time& GetDuration() const;
	void SetDuration(const Time& duration);

private:
	uint32_t m_eventId;

	bool m_fixed; // The event is fixed or mobility

	Ptr<MobilityModel> m_mobilityModel;

	Time m_occurenceTime;
	Time m_detectionTime; // Time detection
	Time m_duration; // Time life of the event detected
	Time m_lastUpdated;

	uint32_t m_nodeEventId;
};

class EventUtils {
public:
	static vector<Ptr<EventDetected>> EventsGenerator(NodeContainer nodeContainer);

private:
	static Ptr<EventDetected> CreateEvent(uint32_t eventId, double_t occurenceTime, bool isFixed, double_t duration, Ptr<MobilityModel> mobilityModel, uint32_t nodeEvent);
};

} // namespace ns3

#endif /* MINUET_EVENT_H_ */
