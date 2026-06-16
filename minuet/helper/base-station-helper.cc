/*
 * base-station-helper.cc
 *
 *  Created on: 26 de nov de 2018
 *      Author: everaldo
 */

#include "base-station-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BaseStationHelper");

BaseStationHelper::BaseStationHelper()
{
	NS_LOG_FUNCTION (this);
	m_factory.SetTypeId("ns3::AppBaseStation");
}

BaseStationHelper::~BaseStationHelper()
{
	NS_LOG_FUNCTION (this);
}

ApplicationContainer
BaseStationHelper::Install(Ptr<Node> node) const
{
	NS_LOG_FUNCTION (this);
	ApplicationContainer apps;
	apps.Add(InstallPriv(node));
	return apps;
}

Ptr<Application>
BaseStationHelper::InstallPriv(Ptr<Node> node) const
{
	NS_LOG_FUNCTION (this);
	Ptr<AppBaseStation> app = m_factory.Create<AppBaseStation> ();
	node->AddApplication (app);
	return app;
}

} // namespace ns3

