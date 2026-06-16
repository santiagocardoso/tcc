/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "minuet-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MINUETHelper");

MINUETHelper::MINUETHelper()
{
	NS_LOG_FUNCTION (this);
	m_factory.SetTypeId("ns3::AppMINUET");
}

MINUETHelper::~MINUETHelper()
{
	NS_LOG_FUNCTION (this);
}

ApplicationContainer
MINUETHelper::Install(Ptr<Node> node, vector<Ptr<EventDetected>> eventsOccurenceVector, vector<double_t> initialEndFinalTimeNode) const
{
	NS_LOG_FUNCTION (this);
	ApplicationContainer apps;
	apps.Add(InstallPriv(node, eventsOccurenceVector, initialEndFinalTimeNode));
	return apps;
}

ApplicationContainer
MINUETHelper::Install(std::string nodeName, vector<Ptr<EventDetected>> eventsOccurenceVector, vector<double_t> initialEndFinalTimeNode) const
{
	NS_LOG_FUNCTION (this);
	ApplicationContainer apps;
	Ptr<Node> node = Names::Find<Node> (nodeName);
	apps.Add(InstallPriv(node, eventsOccurenceVector, initialEndFinalTimeNode));
	return apps;
}

Ptr<Application>
MINUETHelper::InstallPriv(Ptr<Node> node, vector<Ptr<EventDetected>> eventsOccurenceVector, vector<double_t> initialEndFinalTimeNode) const
{
	NS_LOG_FUNCTION (this);
	Ptr<AppMINUET> app = m_factory.Create<AppMINUET> ();
	app->SetEventsOccurenceVector(eventsOccurenceVector);

	if(!initialEndFinalTimeNode.empty()) {
		app->SetInitialNodeTime(Seconds(initialEndFinalTimeNode[0]));
		app->SetEndNodeTime(Seconds(initialEndFinalTimeNode[1]));
	} else {
		app->SetInitialNodeTime(Seconds(0.));
		app->SetEndNodeTime(Seconds(0.));
	}

	node->AddApplication (app);
	return app;
}

}
