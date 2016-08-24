## GGUtil
##
## Utilities for dealing with Great Generals.
##
## Notes
##   - Must be initialized externally by calling init()
##
## Copyright (c) 2008 The BUG Mod.
##
## Author: EmperorFool

from CvPythonExtensions import *
import BugUtil
import FontUtil

gc = CyGlobalContext()

g_ePromo = -1
g_promoButton = ""
g_cGreatGeneral = ""
g_cUniquePower = ""

def init():
	global g_ePromo
	g_ePromo = gc.getInfoTypeForString("PROMOTION_LEADER")
	global g_promoButton
	g_promoButton = gc.getPromotionInfo(g_ePromo).getButton()
	global g_cGreatGeneral
	g_cGreatGeneral = FontUtil.getChar(FontSymbols.GREAT_GENERAL_CHAR)

def getPromotionId():
	return g_ePromo

def getPromotion():
	return gc.getPromotionInfo(g_ePromo)

def getGreatGeneralText(iNeededExp):
	return BugUtil.getText("INTERFACE_NEXT_GREAT_GENERAL_XP", 
			(g_cGreatGeneral, iNeededExp))

#Civ4 Reimagined
def getUniquePowerText(iUniquePowerLevel, iUniquePowerRate, iUniquePowerRateAfterDecimalPoint, iTurnsUniquePower):
	
	if (iUniquePowerLevel == 0):
		g_cUniquePower = "[I]"
	if(iUniquePowerLevel == 1):
		g_cUniquePower = "[II]"
	if(iUniquePowerLevel == 2):
		g_cUniquePower = "[III]"
	if(iUniquePowerLevel == 3):
		g_cUniquePower = "[IV]"
	if(iUniquePowerLevel == 4):
		g_cUniquePower = "[V]"
	if(iUniquePowerLevel == 5):
		g_cUniquePower = "[-]"	
		
	return BugUtil.getText("INTERFACE_NEXT_UNIQUE_POWER_LEVEL", 
			(g_cUniquePower, iUniquePowerRate, iUniquePowerRateAfterDecimalPoint, iTurnsUniquePower))