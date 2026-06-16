/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef APP_MINUET_H
#define APP_MINUET_H

#include "ns3/core-module.h"
#include "minuet.h"
#include "dca.h"
#include "social.h"

using namespace std;

namespace ns3 {

class AppMINUET : public Application {

public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	AppMINUET();
	virtual ~AppMINUET();

	void SetEventsOccurenceVector(const vector<Ptr<EventDetected> >& eventsOccurenceVector);
	void SetInitialNodeTime(Time initialNodeTime);
	void SetEndNodeTime(Time endNodeTime);

private:
	/// inherited from Application base class.
	virtual void StartApplication (void);    // Called at time specified by Start
	virtual void StopApplication (void);     // Called at time specified by Stop

	void StartMINUET();
	void StopMINUET();

	void PrintInLog(string message);


	Ptr<DCA> m_dca;
	Ptr<SOCIAL> m_social;
	Ptr<MINUET> m_minuet;
//	Ptr<DCA> m_dca;

	vector<Ptr<EventDetected>> m_eventsOccurenceVector;

	Time m_initialNodeTime;
	Time m_endNodeTime;
};

}

#endif /* APP__MINUET_H */

