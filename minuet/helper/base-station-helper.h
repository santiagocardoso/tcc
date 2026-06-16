/*
 * base-station-helper.h
 *
 *  Created on: 26 de nov de 2018
 *      Author: everaldo
 */

#ifndef BASE_STATION_HELPER_H_
#define BASE_STATION_HELPER_H_

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/minuet-base-station.h"

namespace ns3 {

class BaseStationHelper {
public:
	BaseStationHelper();
	virtual ~BaseStationHelper();

	/**
	 *	Install an ns3::AppBaseStation on the base station node configured with all the attributes set with SetAttribute.
	 *
	 *	\param node The node on which an AppBaseStation will be installed.
	 *	\returns Container of Ptr (ApplicationContainer) to the applications installed.
	 **/
	ApplicationContainer Install(Ptr<Node> node) const;

private:
	ObjectFactory m_factory;

	Ptr<Application> InstallPriv(Ptr<Node> node) const;

};

} // namespace ns3

#endif /* BASE_STATION_HELPER_H_ */
