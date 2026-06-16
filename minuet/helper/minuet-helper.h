/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef MINUET_HELPER_H
#define MINUET_HELPER_H

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-model.h"
#include "ns3/app-minuet.h"

namespace ns3 {

class MINUETHelper {
public:
	MINUETHelper();
	virtual ~MINUETHelper();

	/**
	 *	Install an ns3::AppMINUET on the node configured with all the attributes set with SetAttribute.
	 *
	 *	\param node The node on which an AppMINUET will be installed.
	 *	\param eventsDetectedVector The events detected by node. If vector is empyt then node no detected events
	 *	\param initialEndFinalTimeNode The initial end final time of the node
	 *	\returns Container of Ptr (ApplicationContainer) to the applications installed.
	 **/
	ApplicationContainer Install(Ptr<Node> node, vector<Ptr<EventDetected>> eventsDetectedVector, vector<double_t> initialEndFinalTimeNode) const;

	/**
	 *	Install an ns3::AppMINUET on the node configured with all the attributes set with SetAttribute.
	 *
	 *	\param nodeName The node on which an AppMINUET will be installed.
	 *	\param eventsDetectedVector The events detected by node. If vector is empyt then node no detected events
	 *	\param initialEndFinalTimeNode The initial end final time of the node
	 *	\returns Container of Ptr (ApplicationContainer) to the applications installed.
	 **/
	ApplicationContainer Install(std::string nodeName, vector<Ptr<EventDetected>> eventsDetectedVector, vector<double_t> initialEndFinalTimeNode) const;

private:
	ObjectFactory m_factory;

	Ptr<Application> InstallPriv(Ptr<Node> node, vector<Ptr<EventDetected>> eventsDetectedVector, vector<double_t> initialEndFinalTimeNode) const;

};

}

#endif /* MINUET_HELPER_H */

