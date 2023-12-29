/*
 * demos.c
 *
 *  Created on: Dec 29, 2023
 *      Author: tryfinally
 */

#include "demos.h"

#include "utils/menu_selection.h"

#include "Demos/IsisAntSdemo.h"
#include "Demos/isis_ants2_demo.h"
#include "Demos/GomEPSdemo.h"
#include "Demos/IsisSPdemo.h"
#include "Demos/IsisSPv2demo.h"
#include "Demos/IsisTRXUVdemo.h"
#include "Demos/IsisTRXVUdemo.h"
#include "Demos/IsisMTQv1demo.h"
#include "Demos/IsisMTQv2demo.h"
#include "Demos/cspaceADCSdemo.h"
#include "Demos/ScsGeckoDemo.h"
#include "Demos/IsisHSTxSdemo.h"
#include "Demos/isis_eps_demo.h"
#include "Demos/tausat2_pdhudemo.h"


static MenuAction menu[] = {
			{ TRXVUtest, "TRXVU Tests" },
			{ HSTxStest, "HSTxS Tests" },
			{ AntStest, "Antenna Tests" },
			{ SolarPanelv2test, "Solar Panels Tests" },
			{ IsisMTQv2test, "MTQ V2 Tests" },
			{ isis_eps__test, "ISIS EPS Tests" },
			{ GomEPStest, "GOM EPS Tests" },
			MENU_ITEM_END
};

Boolean demo_tests(void)
{
	MenuDisplay(menu);
	return TRUE;
}
