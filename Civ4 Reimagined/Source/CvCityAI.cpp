// cityAI.cpp

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvCityAI.h"
#include "CvGameAI.h"
#include "CvPlot.h"
#include "CvArea.h"
#include "CvPlayerAI.h"
#include "CvTeamAI.h"
#include "CyCity.h"
#include "CyArgsList.h"
#include "CvInfos.h"
#include "FProfiler.h"

#include "CvDLLPythonIFaceBase.h"
#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
/*                                                                                              */
/* AI logging                                                                                   */
/************************************************************************************************/
#include "BetterBTSAI.h"
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

#define BUILDINGFOCUS_FOOD					(1 << 1)
#define BUILDINGFOCUS_PRODUCTION			(1 << 2)
#define BUILDINGFOCUS_GOLD					(1 << 3)
#define BUILDINGFOCUS_RESEARCH				(1 << 4)
#define BUILDINGFOCUS_CULTURE				(1 << 5)
#define BUILDINGFOCUS_DEFENSE				(1 << 6)
#define BUILDINGFOCUS_HAPPY					(1 << 7)
#define BUILDINGFOCUS_HEALTHY				(1 << 8)
#define BUILDINGFOCUS_EXPERIENCE			(1 << 9)
#define BUILDINGFOCUS_MAINTENANCE			(1 << 10)
#define BUILDINGFOCUS_SPECIALIST			(1 << 11)
#define BUILDINGFOCUS_ESPIONAGE				(1 << 12)
#define BUILDINGFOCUS_BIGCULTURE			(1 << 13)
//#define BUILDINGFOCUS_WORLDWONDER			(1 << 14)
#define BUILDINGFOCUS_WORLDWONDER			(1 << 14 | 1 << 16) // K-Mod (WORLDWONDER implies WONDEROK)
#define BUILDINGFOCUS_DOMAINSEA				(1 << 15)
#define BUILDINGFOCUS_WONDEROK				(1 << 16)
#define BUILDINGFOCUS_CAPITAL				(1 << 17)

// Public Functions...

CvCityAI::CvCityAI()
{
	m_aiEmphasizeYieldCount = new int[NUM_YIELD_TYPES];
	m_aiEmphasizeCommerceCount = new int[NUM_COMMERCE_TYPES];
	m_bForceEmphasizeCulture = false;
	m_aiSpecialYieldMultiplier = new int[NUM_YIELD_TYPES];
	m_aiPlayerCloseness = new int[MAX_PLAYERS];
	m_aiConstructionValue.assign(GC.getNumBuildingClassInfos(), -1); // K-Mod

	m_pbEmphasize = NULL;

	AI_reset();
}


CvCityAI::~CvCityAI()
{
	AI_uninit();

	SAFE_DELETE_ARRAY(m_aiEmphasizeYieldCount);
	SAFE_DELETE_ARRAY(m_aiEmphasizeCommerceCount);
	SAFE_DELETE_ARRAY(m_aiSpecialYieldMultiplier);
	SAFE_DELETE_ARRAY(m_aiPlayerCloseness);
}


void CvCityAI::AI_init()
{
	AI_reset();

	//--------------------------------
	// Init other game data
	AI_assignWorkingPlots();

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      11/14/09                                jdog5000      */
/*                                                                                              */
/* City AI, Worker AI                                                                           */
/************************************************************************************************/
/* original bts code
	AI_updateWorkersNeededHere();
	
	AI_updateBestBuild();
*/
	AI_updateBestBuild();

	AI_updateWorkersNeededHere();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
}


void CvCityAI::AI_uninit()
{
	SAFE_DELETE_ARRAY(m_pbEmphasize);
}


// FUNCTION: AI_reset()
// Initializes data members that are serialized.
void CvCityAI::AI_reset()
{
	int iI;

	AI_uninit();

	m_iEmphasizeAvoidGrowthCount = 0;
	m_iEmphasizeGreatPeopleCount = 0;
	m_bForceEmphasizeCulture = false;

	m_bAssignWorkDirty = false;
	m_bChooseProductionDirty = false;

	m_routeToCity.reset();

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiEmphasizeYieldCount[iI] = 0;
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiEmphasizeCommerceCount[iI] = 0;
	}

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		m_aiBestBuildValue[iI] = NO_BUILD;
	}

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		m_aeBestBuild[iI] = NO_BUILD;
	}
	
	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSpecialYieldMultiplier[iI] = 0;
	}
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiPlayerCloseness[iI] = 0;
	}
	AI_ClearConstructionValueCache(); // K-Mod
	m_iCultureWeight = 30; // K-Mod

	m_iCachePlayerClosenessTurn = -1;
	m_iCachePlayerClosenessDistance = -1;
	
	m_iNeededFloatingDefenders = -1;
	m_iNeededFloatingDefendersCacheTurn = -1;

	m_iWorkersNeeded = 0;
	m_iWorkersHave = 0;

	FAssertMsg(m_pbEmphasize == NULL, "m_pbEmphasize not NULL!!!");
	FAssertMsg(GC.getNumEmphasizeInfos() > 0,  "GC.getNumEmphasizeInfos() is not greater than zero but an array is being allocated in CvCityAI::AI_reset");
	m_pbEmphasize = new bool[GC.getNumEmphasizeInfos()];
	for (iI = 0; iI < GC.getNumEmphasizeInfos(); iI++)
	{
		m_pbEmphasize[iI] = false;
	}
}


void CvCityAI::AI_doTurn()
{
	PROFILE_FUNC();

	int iI;

	if (!isHuman())
	{
		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			setForceSpecialistCount(((SpecialistTypes)iI), 0);
		}
	}
	
    if (!isHuman())
	{
	    AI_stealPlots();
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      11/14/09                                jdog5000      */
/*                                                                                              */
/* City AI, Worker AI                                                                           */
/************************************************************************************************/
/* original bts code
	AI_updateWorkersNeededHere();
	
	AI_updateBestBuild();
*/
	if (!isDisorder()) // K-Mod
		AI_updateBestBuild();

	AI_updateWorkersNeededHere();
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	AI_updateRouteToCity();

	if (isHuman())
	{
	    if (isProductionAutomated())
	    {
	        AI_doHurry();
	    }
		return;
	}

	AI_doPanic();

	AI_doDraft();

	AI_doHurry();

	AI_doEmphasize();
}


// (Most of this function has been rewritten for K-Mod)
void CvCityAI::AI_assignWorkingPlots()
{
	PROFILE_FUNC();

	/* original bts code
	if (0 != GC.getDefineINT("AI_SHOULDNT_MANAGE_PLOT_ASSIGNMENT"))
	{
		return;
	} */ // K-Mod. that option would break a bunch of stuff.

	// remove any plots we can no longer work for any reason
	verifyWorkingPlots();

	// if we have more specialists of any type than this city can have, reduce to the max
	for (SpecialistTypes i = (SpecialistTypes)0; i < GC.getNumSpecialistInfos(); i=(SpecialistTypes)(i+1))
	{
		if (!isSpecialistValid(i))
		{
			if (getSpecialistCount(i) > getMaxSpecialistCount(i))
			{
				setSpecialistCount(i, getMaxSpecialistCount(i));
			}
			// K-Mod. Apply the cap to forced specialist count as well.
			if (getForceSpecialistCount(i) > getMaxSpecialistCount(i))
			{
				setForceSpecialistCount(i, getMaxSpecialistCount(i));
			}

			FAssert(isSpecialistValid(i));
		}
	}
	
	// always work the home plot (center)
	CvPlot* pHomePlot = getCityIndexPlot(CITY_HOME_PLOT);
	if (pHomePlot != NULL)
	{
		setWorkingPlot(CITY_HOME_PLOT, ((getPopulation() > 0) && canWork(pHomePlot)));
	}
	
	// keep removing the worst citizen until we are not over the limit
	while (extraPopulation() < 0)
	{
		if (!AI_removeWorstCitizen())
		{
			FAssertMsg(false, "failed to remove extra population");
			break;
		}
	}
	
	// extraSpecialists() is less than extraPopulation()
	FAssertMsg(extraSpecialists() >= 0, "extraSpecialists() is expected to be non-negative (invalid Index)");

	// K-Mod. If the city is in disorder, then citizen assignment doesn't matter.
	if (isDisorder())
	{
		FAssert((getWorkingPopulation() + getSpecialistPopulation()) <= (totalFreeSpecialists() + getPopulation()));
		AI_setAssignWorkDirty(false);

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
		}
		return;
	}

	//update the special yield multiplier to be current
	AI_updateSpecialYieldMultiplier();

	// Remove all assigned plots before automatic assignment.
	/*if (!isHuman() || isCitizensAutomated())
	{
		for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			setWorkingPlot(iI, false);
		}
	}*/

	// make sure at least the forced amount of specialists are assigned
	// K-Mod note: it's best if we don't clear all working specialists,
	// because AI_specialistValue uses our current GPP rate in its evaluation.
	bool bNewlyForcedSpecialists = false;
	for (SpecialistTypes i = (SpecialistTypes)0; i < GC.getNumSpecialistInfos(); i=(SpecialistTypes)(i+1))
	{
		int iForcedSpecialistCount = getForceSpecialistCount(i);

		if (getSpecialistCount(i) < iForcedSpecialistCount)
		{
			setSpecialistCount(i, iForcedSpecialistCount);
			bNewlyForcedSpecialists = true;
			FAssert(isSpecialistValid(i));
		}
	}

	// If we added new specialists, we might need to remove something else.
	if (bNewlyForcedSpecialists)
	{
		while (extraPopulation() < 0)
		{
			if (!AI_removeWorstCitizen())
			{
				FAssertMsg(false, "failed to remove extra population");
				break;
			}
		}
	}

	// do we have population unassigned
	while (extraPopulation() > 0)
	{
		// (AI_addBestCitizen now handles forced specialist logic)
		if (!AI_addBestCitizen(/*bWorkers*/ true, /*bSpecialists*/ true))
		{
			FAssertMsg(false, "failed to assign extra population");
			break;
		}
	}

	// if we still have population to assign, assign specialists
	while (extraSpecialists() > 0)
	{
		if (!AI_addBestCitizen(/*bWorkers*/ false, /*bSpecialists*/ true))
		{
			FAssertMsg(false, "failed to assign extra specialist");
			break;
		}
	}

	FAssertMsg(extraSpecialists() >= 0, "added too many specialists");

	// if automated, look for better choices than the current ones
	if (!isHuman() || isCitizensAutomated())
	{
		AI_juggleCitizens();
	}

	// at this point, we should not be over the limit
	FAssert((getWorkingPopulation() + getSpecialistPopulation()) <= (totalFreeSpecialists() + getPopulation()));
	FAssert(extraPopulation() == 0); // K-Mod.

	AI_setAssignWorkDirty(false);

	if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
	{
		gDLL->getInterfaceIFace()->setDirty(CitizenButtons_DIRTY_BIT, true);
	}
}


void CvCityAI::AI_updateAssignWork()
{
	if (AI_isAssignWorkDirty())
	{
		AI_assignWorkingPlots();
	}
}

bool CvCityAI::AI_ignoreGrowth()
{
	PROFILE_FUNC();

	if (!AI_isEmphasizeYield(YIELD_FOOD) && !AI_isEmphasizeGreatPeople())
	{
		if (!AI_foodAvailable((isHuman()) ? 0 : 1))
		{
			return true;
		}
	}

	return false;
}

// units of ~400x commerce
int CvCityAI::AI_specialistValue(SpecialistTypes eSpecialist, bool bRemove, bool bIgnoreFood, int iGrowthValue) const
{
	// K-Mod. To reduce code duplication, this function now uses AI_jobChangeValue. (original code deleted)
	if (bRemove)
	{
		return -AI_jobChangeValue(std::make_pair(false, -1), std::make_pair(true, eSpecialist), bIgnoreFood, false, iGrowthValue);
	}
	else
	{
		return AI_jobChangeValue(std::make_pair(true, eSpecialist), std::make_pair(false, -1), bIgnoreFood, false, iGrowthValue);
	}
}

// K-Mod. The value of a long-term specialist, for use in calculating great person value, and value of free specialists from buildings.
// value is roughly 4 * 100 * commerce
int CvCityAI::AI_permanentSpecialistValue(SpecialistTypes eSpecialist) const
{
	const CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	const int iCommerceValue = 4;
	const int iProdValue = 7;
	const int iFoodValue = 11;

	int iValue = 0;

	// AI_getYieldMultipliers takes a little bit of time, so lets only do it if we need to.
	bool bHasYield = false;
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if (kPlayer.specialistYield(eSpecialist, (YieldTypes)iI) != 0)
		{
			bHasYield = true;
			break;
		}
	}

	if (bHasYield)
	{
		int iFoodX, iProdX, iComX, iUnused;
		AI_getYieldMultipliers(iFoodX, iProdX, iComX, iUnused);
		// I'm going to dilute the yield value multipliers, because they are ultimately less stable than commerce multipliers.
		iValue += iFoodValue * kPlayer.specialistYield(eSpecialist, YIELD_FOOD) * AI_yieldMultiplier(YIELD_FOOD) * (100+iFoodX) / 200;
		iValue += iProdValue * kPlayer.specialistYield(eSpecialist, YIELD_PRODUCTION) * AI_yieldMultiplier(YIELD_PRODUCTION) * (100+iProdX) / 200;
		iValue += iCommerceValue * kPlayer.specialistYield(eSpecialist, YIELD_COMMERCE) * AI_yieldMultiplier(YIELD_COMMERCE) * (100+iComX) / 200;
	}

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		int iTemp = iCommerceValue * kPlayer.specialistCommerce(eSpecialist, (CommerceTypes)iI);
		if (iTemp != 0)
		{
			iTemp *= getTotalCommerceRateModifier((CommerceTypes)iI);
			iTemp *= kPlayer.AI_commerceWeight((CommerceTypes)iI, this);
			iTemp /= 100;
			iValue += iTemp;
		}
	}
	
	// Civ4 Reimagined
	int iGreatPeopleRate = kPlayer.isNoGreatPeople() ? 0 : GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange();

	if (iGreatPeopleRate != 0)
	{
		int iGPPValue = 4;
		
		int iTempValue = 100 * iGreatPeopleRate * iGPPValue;
		
		// Scale based on how often this city will actually get a great person.
		/* Actually... don't do that. That's not the kind of thing we should take into account for permanent specialists.
		int iCityRate = getGreatPeopleRate();
		int iHighestRate = 0;
		int iLoop;
		for( CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop) )
		{
			int x = pLoopCity->getGreatPeopleRate();
			if (x > iHighestRate)
				iHighestRate = x;
		}
		if (iHighestRate > iCityRate)
		{
			iTempValue *= 100;
			iTempValue /= (2*100*(iHighestRate+3))/(iCityRate+3) - 100;
		} */
		iTempValue *= kPlayer.AI_getGreatPersonWeight((UnitClassTypes)GC.getSpecialistInfo(eSpecialist).getGreatPeopleUnitClass());
		iTempValue /= 100;
		
		iTempValue *= getTotalGreatPeopleRateModifier();
		iTempValue /= 100;
		
		iValue += iTempValue;
	}
	
	int iExperience = GC.getSpecialistInfo(eSpecialist).getExperience();
	if (0 != iExperience)
	{
		int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);
		int iHasMetCount = GET_TEAM(getTeam()).getHasMetCivCount(true);

		int iTempValue = 100 * iExperience * ((iHasMetCount > 0) ? 4 : 2);
		if (iProductionRank <= kPlayer.getNumCities()/2 + 1)
		{
			iTempValue += 100 * iExperience *  4;
		}
		iTempValue += (getMilitaryProductionModifier() * iExperience * 6); // was * 8

		iTempValue *= 100;
		iTempValue /= (100+15*(getFreeExperience()/5));

		iValue += iTempValue;
	}

	return iValue;
}

// Heavily edited by K-Mod. (note, I've deleted a lot of the old code from BtS and from BBAI, and some of my changes are unmarked.)
void CvCityAI::AI_chooseProduction()
{
	PROFILE_FUNC();

	bool bWasFoodProduction = isFoodProduction();
	bool bDanger = AI_isDanger();

	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	if (isProduction())
	{
		if (getProduction() > 0)
		{

			if ((getProductionUnitAI() == UNITAI_SETTLE) && kPlayer.AI_isFinancialTrouble())
			{

			}
			//if we are killing our growth to train this, then finish it.
			else if (!bDanger && isFoodProduction())
			{
				if ((area()->getAreaAIType(getTeam()) != AREAAI_DEFENSIVE))
				{
					return;
				}
			}
			// if less than 3 turns left, keep building current item
			else if (getProductionTurnsLeft() <= 3)
			{
				return;
			}

			// if building a combat unit, and we have no defenders, keep building it
			UnitTypes eProductionUnit = getProductionUnit();
			if (eProductionUnit != NO_UNIT)
			{
				if (plot()->getNumDefenders(getOwnerINLINE()) == 0)
				{
					if (GC.getUnitInfo(eProductionUnit).getCombat() > 0)
					{
						return;
					}
				}
			}

			// if we are building a wonder, do not cancel, keep building it (if no danger)
			/* original bts code
			BuildingTypes eProductionBuilding = getProductionBuilding();
			if (!bDanger && eProductionBuilding != NO_BUILDING && 
				isLimitedWonderClass((BuildingClassTypes) GC.getBuildingInfo(eProductionBuilding).getBuildingClassType()))
			{
				return;
			} */
			// K-Mod. same idea, but with a few more conditions
			BuildingTypes eProductionBuilding = getProductionBuilding();
			if (eProductionBuilding != NO_BUILDING && isLimitedWonderClass((BuildingClassTypes) GC.getBuildingInfo(eProductionBuilding).getBuildingClassType()))
			{
				int iCompletion = 100*getBuildingProduction(eProductionBuilding)/std::max(1, getProductionNeeded(eProductionBuilding));
				int iThreshold = 25;
				iThreshold += kPlayer.AI_isLandWar(area()) ? 40 : 0;
				iThreshold += kPlayer.AI_isDoStrategy(AI_STRATEGY_TURTLE) ? 25 : 0; // (in addition to land war)
				iThreshold += !isWorldWonderClass((BuildingClassTypes) GC.getBuildingInfo(eProductionBuilding).getBuildingClassType()) ? 10 : 0;
				if (iCompletion >= iThreshold)
					return;
			}
			// K-Mod end
		}

		clearOrderQueue();
	}

	//if (GET_PLAYER(getOwner()).isAnarchy()) // original bts code
	if (isDisorder()) // K-Mod
	{
		return;
	}

	// only clear the dirty bit if we actually do a check, multiple items might be queued
	AI_setChooseProductionDirty(false);
	if (bWasFoodProduction)
	{
		AI_assignWorkingPlots();
	}

	// allow python to handle it
	if (GC.getUSE_AI_CHOOSE_PRODUCTION_CALLBACK()) // K-Mod. block unused python callbacks
	{
		CyCity* pyCity = new CyCity(this);
		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "AI_chooseProduction", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer
		if (lResult == 1)
		{
			return;
		}
	}

	//if (isHuman() && isProductionAutomated())
	if (isHuman())
	{
		AI_buildGovernorChooseProduction();
		return;
	}

	if (isBarbarian())
	{
		AI_barbChooseProduction();
		return;
	}

	CvArea* pArea = area();
	CvArea* pWaterArea = waterArea(true);
	bool bMaybeWaterArea = false;
	bool bWaterDanger = false;
	bool bWaterAreaRelevant = false;

	if (pWaterArea != NULL)
	{
		bMaybeWaterArea = true;
		if (GET_TEAM(getTeam()).AI_isWaterAreaRelevant(pWaterArea))
		{
			bWaterAreaRelevant = true;
		}

		bWaterDanger = kPlayer.AI_getWaterDanger(plot(), 4) > 0;
	}

	bool bHasMetHuman = GET_TEAM(getTeam()).hasMetHuman();
	//bool bLandWar = ((pArea->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (pArea->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE) || (pArea->getAreaAIType(getTeam()) == AREAAI_MASSING));
	bool bLandWar = kPlayer.AI_isLandWar(pArea); // K-Mod
	bool bDefenseWar = (pArea->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE);
	bool bAssaultAssist = (pArea->getAreaAIType(getTeam()) == AREAAI_ASSAULT_ASSIST);
	bool bTotalWar = GET_TEAM(getTeam()).getWarPlanCount(WARPLAN_TOTAL, true); // K-Mod
	bool bAssault = bAssaultAssist || (pArea->getAreaAIType(getTeam()) == AREAAI_ASSAULT) || (pArea->getAreaAIType(getTeam()) == AREAAI_ASSAULT_MASSING);
	bool bPrimaryArea = kPlayer.AI_isPrimaryArea(pArea);
	bool bFinancialTrouble = kPlayer.AI_isFinancialTrouble();
	int iNumCitiesInArea = pArea->getCitiesPerPlayer(getOwnerINLINE());
	bool bImportantCity = false; //be very careful about setting this.
	int iCultureRateRank = findCommerceRateRank(COMMERCE_CULTURE);
	int iCulturalVictoryNumCultureCities = GC.getGameINLINE().culturalVictoryNumCultureCities();

	int iWarSuccessRating = GET_TEAM(getTeam()).AI_getWarSuccessRating();
	int iEnemyPowerPerc = GET_TEAM(getTeam()).AI_getEnemyPowerPercent(true);


	if( !bLandWar && !bAssault && GET_TEAM(getTeam()).isAVassal() )
	{
		bLandWar = GET_TEAM(getTeam()).isMasterPlanningLandWar(area());

		if( !bLandWar )
		{
			bAssault = GET_TEAM(getTeam()).isMasterPlanningSeaWar(area());
		}
	}

	bool bGetBetterUnits = kPlayer.AI_isDoStrategy(AI_STRATEGY_GET_BETTER_UNITS);
	bool bDagger = kPlayer.AI_isDoStrategy(AI_STRATEGY_DAGGER);
	bool bAggressiveAI = GC.getGameINLINE().isOption(GAMEOPTION_AGGRESSIVE_AI);
	bool bAlwaysPeace = GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_PEACE);

	/* original bts code
	int iUnitCostPercentage = (kPlayer.calculateUnitCost() * 100) / std::max(1, kPlayer.calculatePreInflatedCosts()); */
	int iUnitSpending = kPlayer.AI_unitCostPerMil(); // K-Mod. (note, this is around 3x bigger than the original formula)
	int iWaterPercent = AI_calculateWaterWorldPercent();

	int iBuildUnitProb = AI_buildUnitProb();
	iBuildUnitProb /= kPlayer.AI_isDoStrategy(AI_STRATEGY_ECONOMY_FOCUS) ? 2 : 1; // K-Mod

	int iExistingWorkers = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_WORKER);
	int iNeededWorkers = kPlayer.AI_neededWorkers(pArea);
	// Sea worker need independent of whether water area is militarily relevant
	int iNeededSeaWorkers = (bMaybeWaterArea) ? AI_neededSeaWorkers() : 0;
	int iExistingSeaWorkers = (waterArea(true) != NULL) ? kPlayer.AI_totalWaterAreaUnitAIs(waterArea(true), UNITAI_WORKER_SEA) : 0;

	int iAreaBestFoundValue;
	int iNumAreaCitySites = kPlayer.AI_getNumAreaCitySites(getArea(), iAreaBestFoundValue);

	int iWaterAreaBestFoundValue = 0;
	CvArea* pWaterSettlerArea = bWaterAreaRelevant ? pWaterArea : NULL;
	if( pWaterSettlerArea == NULL )
	{
		pWaterSettlerArea = GC.getMap().findBiggestArea(true);

		if( GET_PLAYER(getOwnerINLINE()).AI_totalWaterAreaUnitAIs(pWaterSettlerArea, UNITAI_SETTLER_SEA) == 0 )
		{
			pWaterSettlerArea = NULL;
		}
	}
	int iNumWaterAreaCitySites = (pWaterSettlerArea == NULL) ? 0 : kPlayer.AI_getNumAdjacentAreaCitySites(pWaterSettlerArea->getID(), getArea(), iWaterAreaBestFoundValue);
	int iNumSettlers = kPlayer.AI_totalUnitAIs(UNITAI_SETTLE);

	bool bIsCapitalArea = false;
	int iNumCapitalAreaCities = 0;
	if (kPlayer.getCapitalCity() != NULL)
	{
		iNumCapitalAreaCities = kPlayer.getCapitalCity()->area()->getCitiesPerPlayer(getOwnerINLINE());
		if (getArea() == kPlayer.getCapitalCity()->getArea())
		{
			bIsCapitalArea = true;
		}
	}

	int iMaxSettlers = 0;
	if (!bFinancialTrouble)
	{
		iMaxSettlers= std::min((kPlayer.getNumCities() + 1) / 2, iNumAreaCitySites + iNumWaterAreaCitySites);
		if (bLandWar || bAssault)
		{
			iMaxSettlers = (iMaxSettlers + 2) / 3;
		}
	}

	bool bChooseWorker = false;

	if (iNumCitiesInArea > 2)
	{
		if (kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2))
		{
			if (iCultureRateRank <= iCulturalVictoryNumCultureCities + 1)
			{
				// if we do not have enough cities, then the highest culture city will not get special attention
				if (iCultureRateRank > 1 || (kPlayer.getNumCities() > (iCulturalVictoryNumCultureCities + 1)))
				{
					if ((((iNumAreaCitySites + iNumWaterAreaCitySites) > 0) && (kPlayer.getNumCities() < 6)) && (GC.getGameINLINE().getSorenRandNum(2, "AI Less Culture More Expand") == 0))
					{
						bImportantCity = false;
					}
					else
					{
						bImportantCity = true;
					}
				}
			}
		}
	}

	// Free experience for various unit domains
	int iFreeLandExperience = getSpecialistFreeExperience() + getDomainFreeExperience(DOMAIN_LAND);
	int iFreeSeaExperience = getSpecialistFreeExperience() + getDomainFreeExperience(DOMAIN_SEA);
	int iFreeAirExperience = getSpecialistFreeExperience() + getDomainFreeExperience(DOMAIN_AIR);

	int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);

	// K-Mod.
	BuildingTypes eBestBuilding = AI_bestBuildingThreshold(); // go go value cache!
	int iBestBuildingValue = (eBestBuilding == NO_BUILDING) ? 0 : AI_buildingValue(eBestBuilding);
	// for the purpose of adjusting production probabilities, scale the building value up for early eras
	// (because early game buildings are relatively weaker)
	if (GC.getNumEraInfos() > 1)
	{
		FAssert(kPlayer.getCurrentEra() < GC.getNumEraInfos());
		iBestBuildingValue *= 2*(GC.getNumEraInfos()-1) - kPlayer.getCurrentEra();
		iBestBuildingValue /= GC.getNumEraInfos()-1;
	}
	// also, reduce the value to encourage early expansion until we reach the recommend city target
	{
		int iCitiesTarget = GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities();
		if (iNumAreaCitySites > 0 && kPlayer.getNumCities() < iCitiesTarget)
		{
			iBestBuildingValue *= kPlayer.getNumCities() + iCitiesTarget;
			iBestBuildingValue /= 2*iCitiesTarget;
		}
	}

	// Check for military exemption for commerce cities and underdeveloped cities.
	// Don't give exemptions to cities that don't have anything good to build anyway.
	bool bUnitExempt = false;
	if (iBestBuildingValue >= 40)
	{
		if (iProductionRank-1 > kPlayer.getNumCities()/2)
		{
			bool bBelowMedian = true;
			for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
			{
				// I'd use the total commerce rank, but there currently isn't a cached value of that.
				int iRank = findCommerceRateRank((CommerceTypes)iI);
				if (iRank < iProductionRank)
				{
					bUnitExempt = true;
					break;
				}
				if (iRank-1 < kPlayer.getNumCities()/2)
					bBelowMedian = false;
			}

			if (bBelowMedian)
				bUnitExempt = true;
		}
	}
	// K-Mod end
	
	
	
	if (eBestBuilding != NO_BUILDING)
	{
		const CvBuildingInfo& kBestBuilding = GC.getBuildingInfo(eBestBuilding);
		if( gCityLogLevel >= 3 ) logBBAI("      City %S pop %d considering new production: iProdRank %d, iBuildUnitProb %d%s, iBestBuildingValue %d (%S)", getName().GetCString(), getPopulation(), iProductionRank, iBuildUnitProb, bUnitExempt?"*":"", iBestBuildingValue, kBestBuilding.getDescription());
	}
	else
	{
		if( gCityLogLevel >= 3 ) logBBAI("      City %S pop %d considering new production: iProdRank %d, iBuildUnitProb %d%s", getName().GetCString(), getPopulation(), iProductionRank, iBuildUnitProb, bUnitExempt?"*":"");
	}

	// if we need to pop borders, then do that immediately if we have drama and can do it
	if (getCultureLevel() <= 1)
	{
		// K-Mod. If our best building is a cultural building, just start building it.
		if (eBestBuilding != NO_BUILDING && AI_countGoodTiles(true, false) > 0)
		{
			const CvBuildingInfo& kBestBuilding = GC.getBuildingInfo(eBestBuilding);
			if (kBestBuilding.getCommerceChange(COMMERCE_CULTURE) + kBestBuilding.getObsoleteSafeCommerceChange(COMMERCE_CULTURE) > 0)
			{
				pushOrder(ORDER_CONSTRUCT, eBestBuilding);
				return;
			}
		}
		// K-Mod end
		if (AI_chooseProcess(COMMERCE_CULTURE))
		{
			return;
		}
	}

	if (plot()->getNumDefenders(getOwnerINLINE()) == 0) // XXX check for other team's units?
	{
		if( gCityLogLevel >= 2 ) logBBAI("      City %S uses no defenders", getName().GetCString());

		if (AI_chooseUnit(UNITAI_CITY_DEFENSE))
		{
			return;
		}

		if (AI_chooseUnit(UNITAI_CITY_COUNTER))
		{
			return;
		}

		if (AI_chooseUnit(UNITAI_CITY_SPECIAL))
		{
			return;
		}

		if (AI_chooseUnit(UNITAI_ATTACK))
		{
			return;
		}
	}

	if( kPlayer.isStrike() )
	{
		int iStrikeFlags = 0;
		iStrikeFlags |= BUILDINGFOCUS_GOLD;
		iStrikeFlags |= BUILDINGFOCUS_MAINTENANCE;

		if(AI_chooseBuilding(iStrikeFlags))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses strike building (w/ flags)", getName().GetCString());
			return;
		}

		if (AI_chooseBuilding())
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses strike building (w/o flags)", getName().GetCString());
			return;
		}
	}

	// K-Mod. Make room for a 'best project'. -1 indicates that we haven't yet calculated the best project.
	// Well evaluate it soon if we are aiming for a space victory; otherwise we'll just leave it until the end.
	ProjectTypes eBestProject = NO_PROJECT;
	int iProjectValue = -1;
	// K-Mod end

	// K-Mod, short-circuit production choice if we already have something really good in mind
	if (kPlayer.getNumCities() > 1) // don't use this short circuit if this is our only city.
	{
		if (kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_SPACE4))
		{
			eBestProject = AI_bestProject(&iProjectValue);
			if (eBestProject != NO_PROJECT && iProjectValue > iBestBuildingValue)
			{
				int iOdds = std::max(0, 100 * iProjectValue / (3 * iProjectValue + 300) - 10);
				if (GC.getGameINLINE().getSorenRandNum(100, "Build Project short circuit 1") < iOdds)
				{
					pushOrder(ORDER_CREATE, eBestProject);
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose project short-circuit 1. (project value: %d, building value: %d, odds: %d)", getName().GetCString(), iProjectValue, iBestBuildingValue, iOdds);
					return;
				}
			}
		}
		
		if (!(bLandWar && iWarSuccessRating < -50))
		{
			int iOdds = std::max(0, 100 * iBestBuildingValue / (3 * iBestBuildingValue + 300) - 10);
			if (AI_chooseBuilding(0, INT_MAX, 0, iOdds))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses building value short-circuit 1 (odds: %d)", getName().GetCString(), iOdds);
				return;
			}
		}
	}
	// K-Mod end

	// So what's the right detection of defense which works in early game too?
	int iPlotSettlerCount = (iNumSettlers == 0) ? 0 : plot()->plotCount(PUF_isUnitAIType, UNITAI_SETTLE, -1, getOwnerINLINE());
	int iPlotCityDefenderCount = plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_DEFENSE, -1, getOwnerINLINE());
	if( kPlayer.getCurrentEra() == 0 )
	{
		// Warriors are blocked from UNITAI_CITY_DEFENSE, in early game this confuses AI city building
		if( kPlayer.AI_totalUnitAIs(UNITAI_CITY_DEFENSE) <= kPlayer.getNumCities() )
		{
			// Removed by Civ4 Reimagined:
			//if( kPlayer.AI_bestCityUnitAIValue(UNITAI_CITY_DEFENSE, this) == 0 )
			{
				//iPlotCityDefenderCount = plot()->plotCount(PUF_canDefend, -1, -1, getOwnerINLINE(), NO_TEAM, PUF_isDomainType, DOMAIN_LAND);
				// Civ4 Reimagined: Don't count Scouts
				iPlotCityDefenderCount += plot()->plotCount(PUF_isUnitAIType, UNITAI_ATTACK, -1, getOwnerINLINE());
			}
		}
	}

	//minimal defense.
	//if (iPlotCityDefenderCount <= iPlotSettlerCount)
	if (!bUnitExempt && iPlotSettlerCount > 0 && iPlotCityDefenderCount <= iPlotSettlerCount)
	{
		if( gCityLogLevel >= 2 ) logBBAI("      City %S needs escort for existing settler", getName().GetCString());
		if (AI_chooseUnit(UNITAI_CITY_DEFENSE))
		{
			// BBAI TODO: Does this work right after settler is built???
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses escort existing settler 1 defense", getName().GetCString());
			return;
		}

		if (AI_chooseUnit(UNITAI_ATTACK))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses escort existing settler 1 attack", getName().GetCString());
			return;
		}
	}
    
	if (getPopulation() > 5 && getCommerceRate(COMMERCE_CULTURE) == 0)
	{
		if( !(kPlayer.AI_isDoStrategy(AI_STRATEGY_TURTLE)) )
		{
			if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 30))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses zero culture build", getName().GetCString());
				return;
			}
		}
	}

	// Early game worker logic
/***
**** K-Mod, 10/sep/10, Karadoc
**** Changed "AI_totalBestBuildValue(area()) > 10" to "(AI_totalBestBuildValue(area())+50*iLandBonuses) > 100"
**** Also, I moved the iLandBonuses declaration here. It use to be lower down.
***/
	int iLandBonuses = AI_countNumImprovableBonuses(true, kPlayer.getCurrentResearch());

	if( isCapital() && (GC.getGame().getElapsedGameTurns() < ((30 * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent()) / 100)))
	{
		if( !bDanger && !(kPlayer.AI_isDoStrategy(AI_STRATEGY_TURTLE)) )
		{	
			if (!bWaterDanger && (getPopulation() < 3) && (iNeededSeaWorkers > 0))
			{
				if (iExistingSeaWorkers == 0)
				{
					// Build workboat first since it doesn't stop growth
					if (AI_chooseUnit(UNITAI_WORKER_SEA))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker sea 1a", getName().GetCString());
						return;
					}
				}
			}

			//if( iExistingWorkers == 0 && AI_totalBestBuildValue(area()) > 10)
			if (iExistingWorkers == 0 && AI_totalBestBuildValue(area())+50*iLandBonuses > 100)
			{
				if (!bChooseWorker && AI_chooseUnit(UNITAI_WORKER))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker 1a", getName().GetCString());
					return;
				}
				bChooseWorker = true;
			}
		}
	}

	if( !(bDefenseWar && iWarSuccessRating < -50) && !bDanger )
	{
		if ((iExistingWorkers == 0))
		{
/***
**** K-Mod, 10/sep/10, Karadoc
**** I've taken iLandBonuses from here and moved it higher for use elsewhere.
***/
			//int iLandBonuses = AI_countNumImprovableBonuses(true, kPlayer.getCurrentResearch());
			if ((iLandBonuses > 1) || (getPopulation() > 3 && iNeededWorkers > 0))
			{
				if (!bChooseWorker && AI_chooseUnit(UNITAI_WORKER))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker 1", getName().GetCString());
					return;
				}
				bChooseWorker = true;
			}

			if (!bWaterDanger && (iNeededSeaWorkers > iExistingSeaWorkers) && (getPopulation() < 3))
			{
				if (AI_chooseUnit(UNITAI_WORKER_SEA))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker sea 1", getName().GetCString());
					return;
				}
			}

			if (iLandBonuses >= 1  && getPopulation() > 1)
    		{
				if (!bChooseWorker && AI_chooseUnit(UNITAI_WORKER))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker 2", getName().GetCString());
					return;
				}
				bChooseWorker = true;
    		}
		}
	}

	/* original bts code (disabled by K-Mod. I don't think these are helpful)
	if ( kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION3) )
	{
        if ((goodHealth() - badHealth(true, 0)) < 1)
		{
			if ( AI_chooseBuilding(BUILDINGFOCUS_HEALTHY, 20, 0, (kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION4) ? 50 : 20)) )
			{
				return;
			}
		}
	}

	if( GET_TEAM(getTeam()).isAVassal() && GET_TEAM(getTeam()).isCapitulated() )
	{
		if( !bLandWar )
		{
			if ((goodHealth() - badHealth(true, 0)) < 1)
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_HEALTHY, 30, 0, 3*getPopulation()))
				{
					return;
				}
			}

			if ((getPopulation() > 3) && (getCommerceRate(COMMERCE_CULTURE) < 5))
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 30, 0 + 3*iWarTroubleThreshold, 3*getPopulation()))
				{
					return;
				}
			}
		}
	}*/

	// -------------------- BBAI Notes -------------------------
	// Minimal attack force, both land and sea
    if (bDanger) 
    {
		int iAttackNeeded = 4;
		iAttackNeeded += std::max(0, AI_neededDefenders() - plot()->plotCount(PUF_isUnitAIType, UNITAI_CITY_DEFENSE, -1, getOwnerINLINE()));

		if( kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK) <  iAttackNeeded)
		{
    		if (AI_chooseUnit(UNITAI_ATTACK))
    		{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses danger minimal attack", getName().GetCString());
    			return;
    		}
		}
    }
    
    if (bMaybeWaterArea)
	{
		// Civ4 Reimagined: Moved !bFinancialTrouble further inside
		if( !(bLandWar && iWarSuccessRating < -30) && !bDanger )
		{
			// Civ4 Reimagined
			const int iNumSeaUnits = kPlayer.AI_totalAreaUnitAIs(pWaterArea, UNITAI_ATTACK_SEA) + kPlayer.AI_totalAreaUnitAIs(pWaterArea, UNITAI_ATTACK_SEA);
			const bool bShipsNeeded = iNumSeaUnits < kPlayer.countNumCoastalCitiesByArea(pWaterArea);

			if (bShipsNeeded && (bWaterDanger || (bPrimaryArea && kPlayer.AI_countNumAreaHostileUnits(pWaterArea, true, false, false, false) > 0)))
			{
				if (AI_chooseUnit(UNITAI_ATTACK_SEA))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses minimal naval", getName().GetCString());
					return;
				}

				if (AI_chooseUnit(UNITAI_RESERVE_SEA))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses minimal naval", getName().GetCString());
					return;
				}
			}
		
			if (bWaterAreaRelevant && !bFinancialTrouble)
			{
				int iOdds = -1;
				if (iAreaBestFoundValue == 0 || iWaterAreaBestFoundValue > iAreaBestFoundValue)
				{
					iOdds = 100;
				}
				else if (iWaterPercent > 60)
				{
					iOdds = 13;
				}

				if( iOdds >= 0 )
				{
					// if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_EXPLORE_SEA) == 0) // original code
					if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_EXPLORE_SEA) == 0 && kPlayer.AI_neededExplorers(pWaterArea) > 0) // K-Mod!
					{
						if (AI_chooseUnit(UNITAI_EXPLORE_SEA, iOdds))
						{
							if( gCityLogLevel >= 2 ) logBBAI("      City %S uses early sea explore", getName().GetCString());
							return;
						}
					}

					// BBAI TODO: Really only want to do this if no good area city sites ... 13% chance on water heavy maps
					// of slow start, little benefit
					if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_SETTLER_SEA) == 0)
					{
						if (AI_chooseUnit(UNITAI_SETTLER_SEA, iOdds))
						{
							if( gCityLogLevel >= 2 ) logBBAI("      City %S uses early settler sea", getName().GetCString());
							return;
						}
					}
				}
			}
		}
	}
    
	// -------------------- BBAI Notes -------------------------
	// Top normal priorities
	
	/* if (!bPrimaryArea && !bLandWar)
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_FOOD, 60, 10 + 2*iWarTroubleThreshold, 50))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose BUILDINGFOCUS_FOOD 1", getName().GetCString());
			return;
		}
	}
	
	if (!bDanger && ((kPlayer.getCurrentEra() > (GC.getGame().getStartEra() + iProductionRank / 2))) || (kPlayer.getCurrentEra() > (GC.getNumEraInfos() / 2)))
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_PRODUCTION, 20 - iWarTroubleThreshold, 15, ((bLandWar || bAssault) ? 25 : -1)))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose BUILDINGFOCUS_PRODUCTION 1", getName().GetCString());
			return;	
		}

		if( !(bDefenseWar && iWarSuccessRatio < -30) )
		{
			if ((iExistingWorkers < ((iNeededWorkers + 1) / 2)))
			{
				if( getPopulation() > 3 || (iProductionRank < (kPlayer.getNumCities() + 1) / 2) )
				{
					if (!bChooseWorker && AI_chooseUnit(UNITAI_WORKER))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker 3", getName().GetCString());
						return;
					}
					bChooseWorker = true;
				}
			}
		}
	} */ // K-Mod disabled this stuff
	
	bool bCrushStrategy = kPlayer.AI_isDoStrategy(AI_STRATEGY_CRUSH);
	int iNeededFloatingDefenders = (isBarbarian() || bCrushStrategy) ?  0 : kPlayer.AI_getTotalFloatingDefendersNeeded(pArea);
	int iMaxUnitSpending = kPlayer.AI_maxUnitCostPerMil(area(), iBuildUnitProb); // K-Mod. (note: this has a different scale to the original code).

	if (kPlayer.AI_isDoStrategy(AI_STRATEGY_ECONOMY_FOCUS)) // K-Mod
	{
		iNeededFloatingDefenders = (2 * iNeededFloatingDefenders + 2)/3;
	}
 	int iTotalFloatingDefenders = (isBarbarian() ? 0 : kPlayer.AI_getTotalFloatingDefenders(pArea));

	UnitTypeWeightArray floatingDefenderTypes;
	floatingDefenderTypes.push_back(std::make_pair(UNITAI_CITY_DEFENSE, 125));
	floatingDefenderTypes.push_back(std::make_pair(UNITAI_CITY_COUNTER, 100));
	//floatingDefenderTypes.push_back(std::make_pair(UNITAI_CITY_SPECIAL, 0));
	floatingDefenderTypes.push_back(std::make_pair(UNITAI_RESERVE, 100));
	floatingDefenderTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 80)); // K-Mod, down from 100

	if (iTotalFloatingDefenders < ((iNeededFloatingDefenders + 1) / (bGetBetterUnits ? 3 : 2)))
	{
		if (!bUnitExempt && iUnitSpending < iMaxUnitSpending + 5)
		{
			if (pArea->getAreaAIType(getTeam()) != AREAAI_NEUTRAL || kPlayer.AI_isDoStrategy(AI_STRATEGY_ALERT1) ||
				GC.getGameINLINE().getSorenRandNum(iNeededFloatingDefenders, "AI floating def 1") > iTotalFloatingDefenders * (2*iUnitSpending + iMaxUnitSpending)/std::max(1, 3*iMaxUnitSpending))
			if (AI_chooseLeastRepresentedUnit(floatingDefenderTypes))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose floating defender 1", getName().GetCString());
				return;
			}
		}
	}

	// If losing badly in war, need to build up defenses and counter attack force
	//if( bLandWar && (iWarSuccessRatio < -30 || iEnemyPowerPerc > 150) )
	// K-Mod. I've changed the condition on this; but to be honest, I'm thinking it should just be completely removed.
	// The noraml unit selection function should know what kind of units we should be building...
	if (bLandWar && iWarSuccessRating < -10 && iEnemyPowerPerc - iWarSuccessRating > 150)
	{
		UnitTypeWeightArray defensiveTypes;
		defensiveTypes.push_back(std::make_pair(UNITAI_COUNTER, 100));
		defensiveTypes.push_back(std::make_pair(UNITAI_ATTACK, 100));
		defensiveTypes.push_back(std::make_pair(UNITAI_RESERVE, 60));
		//defensiveTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 60));
		defensiveTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 80));
		if ( bDanger || (iTotalFloatingDefenders < (5*iNeededFloatingDefenders)/(bGetBetterUnits ? 6 : 4)))
		{
			defensiveTypes.push_back(std::make_pair(UNITAI_CITY_DEFENSE, 200));
			defensiveTypes.push_back(std::make_pair(UNITAI_CITY_COUNTER, 50));
		}

		int iOdds = iBuildUnitProb;
		if( iWarSuccessRating < -50 )
		{
			iOdds += abs(iWarSuccessRating/3);
		}
		// K-Mod
		iOdds *= (-iWarSuccessRating+20 + iBestBuildingValue);
		iOdds /= (-iWarSuccessRating + 2 * iBestBuildingValue);
		// K-Mod end
		if( bDanger )
		{
			iOdds += 10;
		}

		if (AI_chooseLeastRepresentedUnit(defensiveTypes, iOdds))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose losing extra defense with odds %d", getName().GetCString(), iOdds);
			return;
		}
	}

	if( !(bDefenseWar && iWarSuccessRating < -50) )
	{
/***
**** K-Mod, 10/sep/10, Karadoc
**** Disabled iExistingWorkers == 0. Changed Pop > 3 to Pop >=3.
**** Changed Rank < (cities + 1)/2 to Rank <= ...  So that it can catch the case where we only have 1 city.
***/
		//if (!(iExistingWorkers == 0))
		{
			//if (!bDanger && iExistingWorkers < (iNeededWorkers + 1) / 2)
			// Civ4 Reimagined: Bigger chance to build worker
			if (!bDanger && iExistingWorkers < ((iNeededWorkers * 2) / 3))
			{
				if (getPopulation() >= 3 || (iProductionRank <= (kPlayer.getNumCities() + 1) / 2))
				{
					if (!bChooseWorker && AI_chooseUnit(UNITAI_WORKER))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker 4", getName().GetCString());
						return;
					}
					bChooseWorker = true;
				}
			}
		}
	}
	
	//do a check for one tile island type thing?
    //this can be overridden by "wait and grow more"
/***
**** K-Mod, 10/sep/10, Karadoc
**** It was "if (bDanger", I have changed it to "if (!bDanger"
***/
    if (!bDanger && iExistingWorkers == 0 && (isCapital() || (iNeededWorkers > 0) || (iNeededSeaWorkers > iExistingSeaWorkers)))
    {
		if( !(bDefenseWar && iWarSuccessRating < -30) && !(kPlayer.AI_isDoStrategy(AI_STRATEGY_TURTLE)) )
		{
			if ((AI_countNumBonuses(NO_BONUS, /*bIncludeOurs*/ true, /*bIncludeNeutral*/ true, -1, /*bLand*/ true, /*bWater*/ false) > 0) || 
				(isCapital() && (getPopulation() > 3) && iNumCitiesInArea > 1))
    		{
				if (!bChooseWorker && AI_chooseUnit(UNITAI_WORKER))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker 5", getName().GetCString());
					return;
				}
				bChooseWorker = true;
    		}

			if (iNeededSeaWorkers > iExistingSeaWorkers)
			{
				if (AI_chooseUnit(UNITAI_WORKER_SEA))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker sea 2", getName().GetCString());
					return;
				}
			}
		}
    }

	if( !(bDefenseWar && iWarSuccessRating < -30) )
	{
		if (!bWaterDanger && iNeededSeaWorkers > iExistingSeaWorkers)
		{
			if (AI_chooseUnit(UNITAI_WORKER_SEA))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker sea 3", getName().GetCString());
				return;
			}
		}
	}

	if	(!bLandWar && !bAssault && getCommerceRate(COMMERCE_CULTURE) == 0)
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, bAggressiveAI ? 10 : 20, 0, bAggressiveAI ? 33 : 50))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses minimal culture rate", getName().GetCString());
			return;
		}
	}
	
	int iMinFoundValue = kPlayer.AI_getMinFoundValue();
	if (bDanger)
	{
		iMinFoundValue *= 3;
		iMinFoundValue /= 2;
	}

	// BBAI TODO: Check that this works to produce early rushes on tight maps
	if (!bUnitExempt && !bGetBetterUnits && (bIsCapitalArea) && (iAreaBestFoundValue < (iMinFoundValue * 2)))
	{
		//Building city hunting stack.

		if ((getDomainFreeExperience(DOMAIN_LAND) == 0) && (getYieldRate(YIELD_PRODUCTION) > 4))
		{
    		if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, (kPlayer.getCurrentEra() > 1) ? 0 : 7, 33))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses special BUILDINGFOCUS_EXPERIENCE 1a", getName().GetCString());
				return;
			}
		}

		int iStartAttackStackRand = 0;
		if (pArea->getCitiesPerPlayer(BARBARIAN_PLAYER) > 0)
		{
			iStartAttackStackRand += 15;
		}
		if ((pArea->getNumCities() - iNumCitiesInArea) > 0)
		{
			iStartAttackStackRand += iBuildUnitProb / 2;
		}

		if( iStartAttackStackRand > 0 )
		{
			int iAttackCityCount = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK_CITY);
			int iAttackCount = iAttackCityCount + kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK);

			if( (iAttackCount) == 0 )
			{
				if( !bFinancialTrouble )
				{
					if (AI_chooseUnit(UNITAI_ATTACK, iStartAttackStackRand))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose start stack rand", getName().GetCString());
						return;
					}
				}
			}
			else
			{
				if( (iAttackCount > 1) && (iAttackCityCount == 0) )
				{
					if (AI_chooseUnit(UNITAI_ATTACK_CITY))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose start city attack stack", getName().GetCString());
						return;
					}
				}
				else if (iAttackCount < (3 + iBuildUnitProb / 10))
				{
					if (AI_chooseUnit(UNITAI_ATTACK))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose add to city attack stack", getName().GetCString());
						return;
					}
				}
			}
		}
	}

	//opportunistic wonder build (1)
	//if (!bDanger && (!hasActiveWorldWonder()) && (kPlayer.getNumCities() <= 3))
	if (!bDanger && (!hasActiveWorldWonder()) && (kPlayer.getNumCities() <= 3) && (kPlayer.getNumCities() > 1 || iNumSettlers > 0)) // K-Mod
	{
		// For small civ at war, don't build wonders unless winning
		//if( !bLandWar || (iWarSuccessRating > 30) )
		if (pArea->getAreaAIType(getTeam()) == AREAAI_NEUTRAL) // K-Mod. Don't do this if there is any war at all.
		{
			int iWonderTime = GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getWonderConstructRand(), "Wonder Construction Rand");
			// Civ4 Reimagined
			if (kPlayer.AI_isDoStrategy(AI_STRATEGY_SPECIALIST_ECONOMY))
			{
				iWonderTime *= 3;
				iWonderTime /= 2;
			}
			iWonderTime /= 5;
			iWonderTime += 7;
			if (AI_chooseBuilding(BUILDINGFOCUS_WORLDWONDER, iWonderTime))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses opportunistic wonder build 1", getName().GetCString());
				return;
			}
		}
	}
	
	if (!bDanger && !bIsCapitalArea && area()->getCitiesPerPlayer(getOwnerINLINE()) > iNumCapitalAreaCities)
	{
		// BBAI TODO:  This check should be done by player, not by city and optimize placement
		// If losing badly in war, don't build big things
		if( !bLandWar || (iWarSuccessRating > -30) )
		{
			if( kPlayer.getCapitalCity() == NULL || area()->getPopulationPerPlayer(getOwnerINLINE()) > kPlayer.getCapitalCity()->area()->getPopulationPerPlayer(getOwnerINLINE()) )
			{
				if (AI_chooseBuilding(BUILDINGFOCUS_CAPITAL, 15))
				{
					return;
				}
			}
		}
	}

	// K-Mod.
	if (iProjectValue < 0 && (kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_SPACE3) || !(bLandWar && iWarSuccessRating < 30)))
		eBestProject = AI_bestProject(&iProjectValue);
	// K-Mod end

	int iSpreadUnitThreshold = 1000;

	if( bLandWar )
	{
		iSpreadUnitThreshold += 800 - 10*iWarSuccessRating;
	}
	iSpreadUnitThreshold += 1000*plot()->plotCount(PUF_isUnitAIType, UNITAI_MISSIONARY, -1, getOwnerINLINE());

	UnitTypes eBestSpreadUnit = NO_UNIT;
	int iBestSpreadUnitValue = -1;

	if( !bDanger && !(kPlayer.AI_isDoStrategy(AI_STRATEGY_TURTLE)) )
	{
		int iSpreadUnitRoll = (100 - iBuildUnitProb) / 3;
		iSpreadUnitRoll += bLandWar ? 0 : 10;
		// K-Mod
		iSpreadUnitRoll *= (200 + std::max(iProjectValue, iBestBuildingValue));
		iSpreadUnitRoll /= (100 + 3 * std::max(iProjectValue, iBestBuildingValue));
		// K-Mod end
		
		if (AI_bestSpreadUnit(true, true, iSpreadUnitRoll, &eBestSpreadUnit, &iBestSpreadUnitValue))
		{	
			if (iBestSpreadUnitValue > iSpreadUnitThreshold)
			{
				if (AI_chooseUnit(eBestSpreadUnit, UNITAI_MISSIONARY))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose missionary 1", getName().GetCString());
					return;
				}
				FAssertMsg(false, "AI_bestSpreadUnit should provide a valid unit when it returns true");
			}
		}
	}

	// K-Mod
	if (eBestProject != NO_PROJECT && iProjectValue > iBestBuildingValue)
	{
		int iOdds = 100 * (iProjectValue - iBestBuildingValue) / (iProjectValue + iBestBuildingValue + iBuildUnitProb);
		if (GC.getGameINLINE().getSorenRandNum(100, "Build Project chance 1") < iOdds)
		{
			pushOrder(ORDER_CREATE, eBestProject);
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose project 1. (project value: %d, building value: %d, odds: %d)", getName().GetCString(), iProjectValue, iBestBuildingValue, iOdds);
			return;
		}
	}
	// K-Mod end

	//minimal defense.
	//if (!bUnitExempt && iPlotCityDefenderCount < (AI_minDefenders() + iPlotSettlerCount))
	// K-Mod.. take into account any defenders that are on their way. (recall that in AI_guardCityMinDefender, defenders can be shuffled around)
	// (I'm doing the min defender check twice for efficiency - so that we don't count targetmissionAIs when we don't need to)
	if (!bUnitExempt && !(bFinancialTrouble && iUnitSpending > iMaxUnitSpending)
		&& iPlotCityDefenderCount < (AI_minDefenders() + iPlotSettlerCount)
		&& iPlotCityDefenderCount < (AI_minDefenders() + iPlotSettlerCount - kPlayer.AI_plotTargetMissionAIs(plot(), MISSIONAI_GUARD_CITY)))
	// K-Mod end
	{
		if (AI_chooseUnit(UNITAI_CITY_DEFENSE))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose min defender", getName().GetCString());
			return;
		}

		if (AI_chooseUnit(UNITAI_ATTACK))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose min defender (attack ai)", getName().GetCString());
			return;
		}
	}
	
	if( !(bDefenseWar && iWarSuccessRating < -50) )
	{
		if ((iAreaBestFoundValue > iMinFoundValue) || (iWaterAreaBestFoundValue > iMinFoundValue))
		{
			// BBAI TODO: Needs logic to check for early settler builds, settler builds in small cities, whether settler sea exists for water area sites?
			if (pWaterArea != NULL && bWaterAreaRelevant)
			{
				int iTotalCities = kPlayer.getNumCities();
				int iSettlerSeaNeeded = std::min(iNumWaterAreaCitySites, ((iTotalCities + 4) / 8) + 1);
				if (kPlayer.getCapitalCity() != NULL)
				{
					int iOverSeasColonies = iTotalCities - kPlayer.getCapitalCity()->area()->getCitiesPerPlayer(getOwnerINLINE());
					int iLoop = 2;
					int iExtras = 0;
					while (iOverSeasColonies >= iLoop)
					{
						iExtras++;
						iLoop += iLoop + 2;
					}
					iSettlerSeaNeeded += std::min(kPlayer.AI_totalUnitAIs(UNITAI_WORKER) / 4, iExtras);
				}
				if (bAssault)
				{
					iSettlerSeaNeeded = std::min(1, iSettlerSeaNeeded);
				}
				
				if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_SETTLER_SEA) < iSettlerSeaNeeded)
				{
					if (AI_chooseUnit(UNITAI_SETTLER_SEA))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses main settler sea", getName().GetCString());
						return;
					}
				}
			}
			
			if (iPlotSettlerCount == 0)
			{
				if( gCityLogLevel >= 2 ) logBBAI("numSettlers: %d, maxSettlers: %d, %S", iNumSettlers, iMaxSettlers, bLandWar ? "bLandWar" : "");
				if ((iNumSettlers < iMaxSettlers))
				{
					if (AI_chooseUnit(UNITAI_SETTLE, bLandWar ? 50 : -1))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses build settler 1", getName().GetCString());

						//if (kPlayer.getNumMilitaryUnits() <= kPlayer.getNumCities() + 1)
						if (kPlayer.getNumMilitaryUnits() <= kPlayer.getNumCities() * 2) //Civ4 Reimagined
						{
							if (AI_chooseUnit(UNITAI_CITY_DEFENSE))
							{
								if( gCityLogLevel >= 2 ) logBBAI("      City %S uses build settler 1 extra quick defense", getName().GetCString());
								return;
							}
						}
						
						return;
					}
				}
			}
		}
	}

	// Civ4 Reimagined: Compare pirate power with power of strongest ships of neighboring civs
	if ((pWaterArea != NULL) && bWaterAreaRelevant && !bLandWar && !bAssault && !bFinancialTrouble && !bUnitExempt)
	{
		const int iPirateCount = kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_PIRATE_SEA);
		int iNeededPirates = kPlayer.countNumCoastalCitiesByArea(pArea);

		if (iPirateCount < iNeededPirates)
		{
			if (AI_pirateValue() > 49)
			{
				if (AI_chooseUnit(UNITAI_PIRATE_SEA))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose pirate (needed Pirates: %d)", getName().GetCString(), iNeededPirates);
					return;
				}
			}
		}
	}

	// don't build frivolous things if this is an important city unless we at war
	// Civ4 Reimagined: Explore first
    if (!bImportantCity || bLandWar || bAssault)
    {
		if (!bLandWar && !bDanger && (!bFinancialTrouble || GC.getGameINLINE().getGameTurn() < 10))
        {
			if (iPlotCityDefenderCount > 0 && kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_EXPLORE) < (kPlayer.AI_neededExplorers(pArea)))
			{
				if (AI_chooseUnit(UNITAI_EXPLORE))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses build explore", getName().GetCString());
					return;
				}
			}
        }
		
        if (bPrimaryArea && !bFinancialTrouble)
        {
            if (kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK) == 0)
            {
                if (AI_chooseUnit(UNITAI_ATTACK))
                {
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses build attack", getName().GetCString());
                    return;
                }
            }
        }

		// K-Mod (the spies stuff used to be lower down)
		int iNumSpies = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_SPY) + kPlayer.AI_getNumTrainAIUnits(UNITAI_SPY);
		int iNeededSpies = iNumCitiesInArea / 3;
		iNeededSpies += bPrimaryArea ? (kPlayer.getCommerceRate(COMMERCE_ESPIONAGE)+50)/100 : 0;
		iNeededSpies *= GC.getLeaderHeadInfo(kPlayer.getPersonalityType()).getEspionageWeight();
		iNeededSpies /= 100;
		{
			const CvCity* pCapitalCity = kPlayer.getCapitalCity();
			if (pCapitalCity != NULL && pCapitalCity->area() == area())
				iNeededSpies++;
		}
		iNeededSpies -= (bDefenseWar ? 1 : 0);
		if (kPlayer.AI_isDoStrategy(AI_STRATEGY_ESPIONAGE_ECONOMY))
			iNeededSpies++;
		else
			iNeededSpies /= kPlayer.AI_isDoStrategy(AI_STRATEGY_ECONOMY_FOCUS) ? 2 : 1;

		if (iNumSpies < iNeededSpies)
		{
			int iOdds = (kPlayer.AI_isDoStrategy(AI_STRATEGY_ESPIONAGE_ECONOMY) || GET_TEAM(getTeam()).getAnyWarPlanCount(true)) ?45 : 35;
			iOdds *= 50 + std::max(iProjectValue, iBestBuildingValue);
			iOdds /= 20 + 2 * std::max(iProjectValue, iBestBuildingValue);
			iOdds *= iNeededSpies;
			iOdds /= 4*iNumSpies + iNeededSpies;
			iOdds -= bUnitExempt ? 10 : 0; // not completely exempt, but at least reduced probability.
			iOdds = std::max(0, iOdds);
			if (AI_chooseUnit(UNITAI_SPY, iOdds))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S chooses spy with %d/%d needed, at %d odds", getName().GetCString(), iNumSpies, iNeededSpies, iOdds);
				return;
			}
		}
		// K-Mod end

		if( bDefenseWar || (bLandWar && (iWarSuccessRating < -30)) )
		{
			UnitTypeWeightArray panicDefenderTypes;
			panicDefenderTypes.push_back(std::make_pair(UNITAI_RESERVE, 100));
			panicDefenderTypes.push_back(std::make_pair(UNITAI_COUNTER, 100));
			panicDefenderTypes.push_back(std::make_pair(UNITAI_COLLATERAL, 100));
			panicDefenderTypes.push_back(std::make_pair(UNITAI_ATTACK, 100));

        	if (AI_chooseLeastRepresentedUnit(panicDefenderTypes, (bGetBetterUnits ? 40 : 60) - iWarSuccessRating/3))
        	{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose panic defender", getName().GetCString());
        		return;
        	}
        }
    }
	
	// Civ4 Reimagined: Moved this up and made some minor changes
	int iNukeWeight = kPlayer.AI_nukeWeight();
	BuildingTypes eSilo = (BuildingTypes)GC.getInfoTypeForString("BUILDING_MISSILE_SILO");
	
	if ((getNumRealBuilding(eSilo) > 0) && !bAlwaysPeace && !bUnitExempt && !bFinancialTrouble)
	{
		if ((kPlayer.AI_isDoStrategy(AI_STRATEGY_OWABWNW) || GC.getGame().getSorenRandNum(1200, "AI consider Nuke") < std::min(400, iNukeWeight))
			&& (!bAssault || GC.getGame().getSorenRandNum(400, "AI consider Nuke despite assult") < std::min(200, 50 + iNukeWeight/2)))
		{
			int iTotalNukes = kPlayer.AI_totalUnitAIs(UNITAI_ICBM);
			int iNukesWanted = 1 + 2 * std::min(kPlayer.getNumCities(), GC.getGame().getNumCities() - kPlayer.getNumCities());
			// Civ4 Reimagined: Fixed brackets
			if ((iTotalNukes < iNukesWanted) && (GC.getGame().getSorenRandNum(100, "AI train nuke MWAHAHAH") < (90 - (80 * iTotalNukes / iNukesWanted))))
			{
				if (pWaterArea && bWaterAreaRelevant && kPlayer.AI_totalUnitAIs(UNITAI_MISSILE_CARRIER_SEA) < 
					iTotalNukes/2 && GC.getGame().getSorenRandNum(3, "AI train boat instead") == 0)
				{
					if (AI_chooseUnit(UNITAI_MISSILE_CARRIER_SEA, 50))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S builds missile carrier sea instead of nuke", getName().GetCString());
						return;
					}
				}

				if (AI_chooseUnit(UNITAI_ICBM))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S builds nuke because arsenal is too small", getName().GetCString());
					return;
				}
			}
		}
	}

	/* original bts code. We dont' need this. It just ends up putting aqueducts everywhere.
	if (AI_chooseBuilding(BUILDINGFOCUS_FOOD, 60, 10, (bLandWar ? 30 : -1)))
	{
		if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose BUILDINGFOCUS_FOOD 3", getName().GetCString());
		return;
	} */
	
	//oppurunistic wonder build
	if (!bDanger && (!hasActiveWorldWonder() || (kPlayer.getNumCities() > 3)))
	{
		// For civ at war, don't build wonders if losing
		if (!bTotalWar && (!bLandWar || iWarSuccessRating > 0)) // was -30
		{	
			int iWonderTime = GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getWonderConstructRand(), "Wonder Construction Rand");
			// Civ4 Reimagined
			if (kPlayer.AI_isDoStrategy(AI_STRATEGY_SPECIALIST_ECONOMY))
			{
				iWonderTime *= 3;
				iWonderTime /= 2;
			}
			iWonderTime /= 5;
			//iWonderTime += 8;
			iWonderTime += 16; //Civ4 Reimagined
			/*if (AI_chooseBuilding(BUILDINGFOCUS_WORLDWONDER, iWonderTime))
						{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses oppurtunistic wonder build 2", getName().GetCString());
				return;
			}*/
			// K-Mod
			// Reduce the max time when at war (arbitrary - but then again, this part of the AI is not for strategy. It's for flavour.)
			if (pArea->getAreaAIType(getTeam()) != AREAAI_NEUTRAL)
				iWonderTime = iWonderTime * 2/3;
			// And only build the wonder if it is at least as valuable as the building we would have chosen anyway.
			BuildingTypes eBestWonder = AI_bestBuildingThreshold(BUILDINGFOCUS_WORLDWONDER, iWonderTime);
			if (eBestWonder != NO_BUILDING && AI_buildingValue(eBestWonder) >= iBestBuildingValue)
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses opportunistic wonder build 2", getName().GetCString());
				pushOrder(ORDER_CONSTRUCT, eBestWonder);
				return;
			}
			// K-Mod end
		}
	}

	// K-Mod, short-circuit 2 - a strong chance to build some high value buildings.
	{
		//int iOdds = std::max(0, (bLandWar || (bAssault && pWaterArea) ? 80 : 130) * iBestBuildingValue / (iBestBuildingValue + 20 + iBuildUnitProb) - 25);
		int iOdds = std::max(0, (bLandWar || (bAssault && pWaterArea && bWaterAreaRelevant) ? 90 - iBuildUnitProb/4 : 150 - iBuildUnitProb/2) * 
			iBestBuildingValue / (iBestBuildingValue + 40 + iBuildUnitProb) - 20);
		if (AI_chooseBuilding(0, INT_MAX, 0, iOdds))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses building value short-circuit 2 (odds: %d)", getName().GetCString(), iOdds);
   			return;
		}
	}

	// note: this is the only worker test that allows us to reach the full number of needed workers.
	if (!(bLandWar && iWarSuccessRating < -30) && !bDanger)
	{
		if (iExistingWorkers < iNeededWorkers)
		{
			//if (3*AI_getWorkersHave()/2 < AI_getWorkersNeeded() // local
			//	|| GC.getGame().getSorenRandNum(80, "choose worker 6") > iBestBuildingValue + (iBuildUnitProb + 50)/2)
			// Civ4 Reimagined: Bigger chance to build worker
			if (3*AI_getWorkersHave()/2 < AI_getWorkersNeeded() // local
				|| GC.getGame().getSorenRandNum(100, "choose worker 6") + 50 > iBestBuildingValue + (iBuildUnitProb + 50)/2)
			{
				if (!bChooseWorker && AI_chooseUnit(UNITAI_WORKER))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose worker 6", getName().GetCString());
					return;
				}
				bChooseWorker = true;
			}
		}
	}
	// K-Mod end

	if( !bDanger )
	{
		if (iBestSpreadUnitValue > ((iSpreadUnitThreshold * (bLandWar ? 80 : 60)) / 100))
		{
			if (AI_chooseUnit(eBestSpreadUnit, UNITAI_MISSIONARY))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose missionary 2", getName().GetCString());
				return;
			}
			FAssertMsg(false, "AI_bestSpreadUnit should provide a valid unit when it returns true");
		}
	}

	if ((getDomainFreeExperience(DOMAIN_LAND) == 0) && (getYieldRate(YIELD_PRODUCTION) > 4))
	{
    	if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, (kPlayer.getCurrentEra() > 1) ? 0 : 7, 33))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses special BUILDINGFOCUS_EXPERIENCE 1", getName().GetCString());
			return;
		}
	}

	int iCarriers = kPlayer.AI_totalUnitAIs(UNITAI_CARRIER_SEA);
	
	// Revamped logic for production for invasions
	if (iUnitSpending < iMaxUnitSpending + 25) // was + 10 (new unit spending metric)
	{
		bool bBuildAssault = bAssault;
		CvArea* pAssaultWaterArea = NULL;
		if (NULL != pWaterArea && bWaterAreaRelevant)
		{
			// Coastal city extra logic

			pAssaultWaterArea = pWaterArea;

			// If on offensive and can't reach enemy cities from here, act like using AREAAI_ASSAULT
			if( (pAssaultWaterArea != NULL) && !bBuildAssault )
			{
				if( (GET_TEAM(getTeam()).getAnyWarPlanCount(true) > 0) )
				{
					if( (pArea->getAreaAIType(getTeam()) != AREAAI_DEFENSIVE) )
					{
						// BBAI TODO: faster to switch to checking path for some selection group?
						if( !(plot()->isHasPathToEnemyCity(getTeam())) )
						{
							bBuildAssault = true;
						}
					}
				}
			}
		}

		if( bBuildAssault )
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses build assault", getName().GetCString());

			UnitTypes eBestAssaultUnit = NO_UNIT; 
			if (NULL != pAssaultWaterArea)
			{
				kPlayer.AI_bestCityUnitAIValue(UNITAI_ASSAULT_SEA, this, &eBestAssaultUnit);
			}
			else
			{
				kPlayer.AI_bestCityUnitAIValue(UNITAI_ASSAULT_SEA, NULL, &eBestAssaultUnit);
			}
			
			int iBestSeaAssaultCapacity = 0;
			if (eBestAssaultUnit != NO_UNIT)
			{
				iBestSeaAssaultCapacity = GC.getUnitInfo(eBestAssaultUnit).getCargoSpace();
			}

			int iAreaAttackCityUnits = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK_CITY);
			
			int iUnitsToTransport = iAreaAttackCityUnits;
			iUnitsToTransport += kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK);
			iUnitsToTransport += kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_COUNTER)/2;

			int iLocalTransports = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ASSAULT_SEA);
			int iTransportsAtSea = 0;
			if (NULL != pAssaultWaterArea)
			{
				iTransportsAtSea = kPlayer.AI_totalAreaUnitAIs(pAssaultWaterArea, UNITAI_ASSAULT_SEA);
			}
			else
			{
				iTransportsAtSea = kPlayer.AI_totalUnitAIs(UNITAI_ASSAULT_SEA)/2;
			}

			//The way of calculating numbers is a bit fuzzy since the ships
			//can make return trips. When massing for a war it'll train enough
			//ships to move it's entire army. Once the war is underway it'll stop
			//training so many ships on the assumption that those out at sea
			//will return...

			int iTransports = iLocalTransports + (bPrimaryArea ? iTransportsAtSea/2 : iTransportsAtSea/4);
			int iTransportCapacity = iBestSeaAssaultCapacity*(iTransports);

			if (NULL != pAssaultWaterArea)
			{
				int iEscorts = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ESCORT_SEA);
				iEscorts += kPlayer.AI_totalAreaUnitAIs(pAssaultWaterArea, UNITAI_ESCORT_SEA);

				int iTransportViability = kPlayer.AI_calculateUnitAIViability(UNITAI_ASSAULT_SEA, DOMAIN_SEA);

				//int iDesiredEscorts = ((1 + 2 * iTransports) / 3);
				// Civ4 Reimagined
				int iDesiredEscorts = iTransports * 3 / 2 + kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_CARRIER_SEA); // K-Mod
				if( iTransportViability > 95 )
				{
					// Transports are stronger than escorts (usually Galleons and Caravels)
					iDesiredEscorts /= 2; // was /3
				}
				
				/*if (iEscorts < iDesiredEscorts)
				{
					if (AI_chooseUnit(UNITAI_ESCORT_SEA, (iEscorts < iDesiredEscorts/3) ? -1 : 50)) */
				// K-Mod
				if (iEscorts < iDesiredEscorts && iDesiredEscorts > 0)
				{
					int iOdds = 100;
					iOdds *= iDesiredEscorts;
					iOdds /= iDesiredEscorts + 2*iEscorts;
					if (AI_chooseUnit(UNITAI_ESCORT_SEA, iOdds))
				// K-Mod end
					{
						AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA, 12);
						return;
					}
				}
			
				UnitTypes eBestAttackSeaUnit = NO_UNIT;  
				kPlayer.AI_bestCityUnitAIValue(UNITAI_ATTACK_SEA, this, &eBestAttackSeaUnit);
				if (eBestAttackSeaUnit != NO_UNIT)
				{
					// (tweaked for K-Mod)
					int iAttackSea = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_ATTACK_SEA);
					iAttackSea += kPlayer.AI_totalAreaUnitAIs(pAssaultWaterArea, UNITAI_ATTACK_SEA);

					int iDesiredAttackSea = 1 + 4*iTransports / (GC.getUnitInfo(eBestAttackSeaUnit).getBombardRate() == 0 ? 10 : 3);
						
					if (iAttackSea < iDesiredAttackSea)
					{
						int iOdds = 20 + 50 * (iDesiredAttackSea - iAttackSea)/iDesiredAttackSea;
						if (iUnitSpending > iMaxUnitSpending)
							iOdds /= 3;
						// Note: there is a hard limit condition on iUnitSpending earlier in the code.

						if (GC.getGameINLINE().getSorenRandNum(100, "Build AttackSea") < iOdds && AI_chooseUnit(eBestAttackSeaUnit, UNITAI_ATTACK_SEA))
						{
							AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA, 12);
							return;
						}
					}
				}
				
				if (iUnitsToTransport > iTransportCapacity)
				{
					//if ((iUnitSpending < iMaxUnitSpending) || (iUnitsToTransport > 2*iTransportCapacity))
					// K-Mod
					if (iUnitSpending < iMaxUnitSpending ||
						GC.getGameINLINE().getSorenRandNum(100, "Build Transport") < 100 * (iUnitsToTransport - iTransportCapacity) / std::max(1, iTransportCapacity))
					// K-Mod end
					{
						if (AI_chooseUnit(UNITAI_ASSAULT_SEA))
						{
							AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA, 8);
							return;
						}
					}
				}
			}

			if (iUnitSpending < iMaxUnitSpending)
			{
				if (NULL != pAssaultWaterArea)
				{
					if (!bFinancialTrouble && iCarriers < (kPlayer.AI_totalUnitAIs(UNITAI_ASSAULT_SEA) / 4))
					{
						// Reduce chances of starting if city has low production
						// Civ4 Reimagined
						if (AI_chooseUnit(UNITAI_CARRIER_SEA, (iProductionRank <= ((kPlayer.getNumCities() / 3) + 1)) ? -1 : 50))
						{
							AI_chooseBuilding(BUILDINGFOCUS_DOMAINSEA, 16);
							return;
						}
					}
				}
			}

			// Consider building more land units to invade with
			int iTrainInvaderChance = iBuildUnitProb + 10;

			iTrainInvaderChance += (bAggressiveAI ? 15 : 0);
			iTrainInvaderChance += bTotalWar ? 10 : 0; // K-Mod
			iTrainInvaderChance /= (bAssaultAssist ? 2 : 1);
			iTrainInvaderChance /= (bImportantCity ? 2 : 1);
			iTrainInvaderChance /= (bGetBetterUnits ? 2 : 1);

			// K-Mod
			iTrainInvaderChance *= (100 + std::max(iProjectValue, iBestBuildingValue));
			iTrainInvaderChance /= (20 + 3 * std::max(iProjectValue, iBestBuildingValue));
			// K-Mod end

			iUnitsToTransport *= 9;
			iUnitsToTransport /= 10;

			if( (iUnitsToTransport > iTransportCapacity) && (iUnitsToTransport > (bAssaultAssist ? 2 : 4)*iBestSeaAssaultCapacity) )
			{
				// Already have enough
				iTrainInvaderChance /= 2;
			}
			else if( iUnitsToTransport < (iLocalTransports*iBestSeaAssaultCapacity) )
			{
				iTrainInvaderChance += 15;
			}

			if( getPopulation() < 4 )
			{
				// Let small cities build themselves up first
				iTrainInvaderChance /= (5 - getPopulation());
			}

			UnitTypeWeightArray invaderTypes;
			invaderTypes.push_back(std::make_pair(UNITAI_ATTACK_CITY, 110)); // was 100
			invaderTypes.push_back(std::make_pair(UNITAI_COUNTER, 40)); // was 50
			invaderTypes.push_back(std::make_pair(UNITAI_ATTACK, 50)); // was 40
			if( kPlayer.AI_isDoStrategy(AI_STRATEGY_AIR_BLITZ) )
			{
				invaderTypes.push_back(std::make_pair(UNITAI_PARADROP, 20));
			}

			if (AI_chooseLeastRepresentedUnit(invaderTypes, iTrainInvaderChance))
			{
				if( !bImportantCity && (iUnitsToTransport >= (iLocalTransports*iBestSeaAssaultCapacity)) )
				{
					// Have time to build barracks first
					AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 20);
				}
				return;
			}

			if (iUnitSpending < (iMaxUnitSpending))
			{
				int iMissileCarriers = kPlayer.AI_totalUnitAIs(UNITAI_MISSILE_CARRIER_SEA);
			
				if (!bFinancialTrouble && iMissileCarriers > 0 && !bImportantCity)
				{
					if( (iProductionRank <= ((kPlayer.getNumCities() / 2) + 1)) )
					{
						UnitTypes eBestMissileCarrierUnit = NO_UNIT;  
						kPlayer.AI_bestCityUnitAIValue(UNITAI_MISSILE_CARRIER_SEA, NULL, &eBestMissileCarrierUnit);
						if (eBestMissileCarrierUnit != NO_UNIT)
						{
							FAssert(GC.getUnitInfo(eBestMissileCarrierUnit).getDomainCargo() == DOMAIN_AIR);
							
							int iMissileCarrierAirNeeded = iMissileCarriers * GC.getUnitInfo(eBestMissileCarrierUnit).getCargoSpace();
							
							if ((kPlayer.AI_totalUnitAIs(UNITAI_MISSILE_AIR) < iMissileCarrierAirNeeded) || 
								(bPrimaryArea && (kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_MISSILE_CARRIER_SEA) * GC.getUnitInfo(eBestMissileCarrierUnit).getCargoSpace() < kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_MISSILE_AIR))))
							{
								// Don't always build missiles, more likely if really low
								if (AI_chooseUnit(UNITAI_MISSILE_AIR, (kPlayer.AI_totalUnitAIs(UNITAI_MISSILE_AIR) < iMissileCarrierAirNeeded/2) ? 50 : 20))
								{
									if( gCityLogLevel >= 2 ) logBBAI("      City %S uses build missile", getName().GetCString());
									return;
								}
							}
						}
					}
				}
			}
		}
	}
	
	UnitTypeWeightArray airUnitTypes;

    int iAircraftNeed = 0;
    int iAircraftHave = 0;
    UnitTypes eBestAttackAircraft = NO_UNIT;
    UnitTypes eBestMissile = NO_UNIT;
    
	if (iUnitSpending < (iMaxUnitSpending + 12) && (!bImportantCity || bDefenseWar) ) // K-Mod. was +4, now +12 for the new unit spending metric
	{
		if( bLandWar || bAssault || (iFreeAirExperience > 0) || (GC.getGame().getSorenRandNum(3, "AI train air") == 0) )
		{
			int iBestAirValue = kPlayer.AI_bestCityUnitAIValue(UNITAI_ATTACK_AIR, this, &eBestAttackAircraft);
			int iBestMissileValue = kPlayer.AI_bestCityUnitAIValue(UNITAI_MISSILE_AIR, this, &eBestMissile);
			if ((iBestAirValue + iBestMissileValue) > 0)
			{
				iAircraftHave = kPlayer.AI_totalUnitAIs(UNITAI_ATTACK_AIR) + kPlayer.AI_totalUnitAIs(UNITAI_DEFENSE_AIR) + kPlayer.AI_totalUnitAIs(UNITAI_MISSILE_AIR);
				if (NO_UNIT != eBestAttackAircraft)
				{
					iAircraftNeed = (2 + kPlayer.getNumCities() * (3 * GC.getUnitInfo(eBestAttackAircraft).getAirCombat())) / (2 * std::max(1, GC.getGame().getBestLandUnitCombat()));
					int iBestDefenseValue = kPlayer.AI_bestCityUnitAIValue(UNITAI_DEFENSE_AIR, this);
					if (iBestDefenseValue > 0)
					{
						iAircraftNeed = std::max(iAircraftNeed, kPlayer.AI_getTotalAirDefendersNeeded()); // K-Mod
						if (iBestAirValue > iBestDefenseValue)
							iAircraftNeed = iAircraftNeed*3/2;
					}
					/* if ((iBestDefenseValue > 0) && (iBestAirValue > iBestDefenseValue))
					{
						iAircraftNeed *= 3;
						iAircraftNeed /= 2;
					} */ // (original code, reworded above.)
				}
				if (iBestMissileValue > 0)
				{
					iAircraftNeed = std::max(iAircraftNeed, 1 + kPlayer.getNumCities() / 2);
				}
				
				bool bAirBlitz = kPlayer.AI_isDoStrategy(AI_STRATEGY_AIR_BLITZ);
				bool bLandBlitz = kPlayer.AI_isDoStrategy(AI_STRATEGY_LAND_BLITZ);
				if (bAirBlitz)
				{
					iAircraftNeed *= 3;
					iAircraftNeed /= 2;
				}
				else if (bLandBlitz)
				{
					iAircraftNeed /= 2;
					iAircraftNeed += 1;
				}
				
				airUnitTypes.push_back(std::make_pair(UNITAI_ATTACK_AIR, bAirBlitz ? 125 : 80));
				airUnitTypes.push_back(std::make_pair(UNITAI_DEFENSE_AIR, bLandBlitz ? 100 : 100));
				if (iBestMissileValue > 0)
				{
					airUnitTypes.push_back(std::make_pair(UNITAI_MISSILE_AIR, bAssault ? 60 : 40));
				}
				
				//airUnitTypes.push_back(std::make_pair(UNITAI_ICBM, 20));
				airUnitTypes.push_back(std::make_pair(UNITAI_ICBM, 20 * iNukeWeight / 100)); // K-Mod
				
				if (iAircraftHave * 2 < iAircraftNeed)
				{
					if (AI_chooseLeastRepresentedUnit(airUnitTypes))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses build least represented air", getName().GetCString());
						return;
					}
				}
				// Additional check for air defenses
				int iFightersHave = kPlayer.AI_totalUnitAIs(UNITAI_DEFENSE_AIR);

				if( 3*iFightersHave < iAircraftNeed )
				{
					if (AI_chooseUnit(UNITAI_DEFENSE_AIR))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses build air defence", getName().GetCString());
						return;
					}
				}
			}
		}
	}

	// Check for whether to produce planes to fill carriers
	if ( (bLandWar || bAssault) && iUnitSpending < (iMaxUnitSpending))
	{			
		if (iCarriers > 0 && !bImportantCity)
		{
			UnitTypes eBestCarrierUnit = NO_UNIT;  
			kPlayer.AI_bestCityUnitAIValue(UNITAI_CARRIER_SEA, NULL, &eBestCarrierUnit);
			if (eBestCarrierUnit != NO_UNIT)
			{
				FAssert(GC.getUnitInfo(eBestCarrierUnit).getDomainCargo() == DOMAIN_AIR);
				
				int iCarrierAirNeeded = iCarriers * GC.getUnitInfo(eBestCarrierUnit).getCargoSpace();

				// Reduce chances if city gives no air experience
				if (kPlayer.AI_totalUnitAIs(UNITAI_CARRIER_AIR) < iCarrierAirNeeded)
				{
					// Civ4 Reimagined: Increased odds a little bit
					if (AI_chooseUnit(UNITAI_CARRIER_AIR, (iFreeAirExperience > 0) ? -1 : 45))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses build carrier air", getName().GetCString());
						return;
					}
				}
			}
		}
	}

	/* original code
	if (!bAlwaysPeace && !(bLandWar || bAssault) && (kPlayer.AI_isDoStrategy(AI_STRATEGY_OWABWNW) || (GC.getGame().getSorenRandNum(12, "AI consider Nuke") == 0)))
	{
		if( !bFinancialTrouble )
		{
			int iTotalNukes = kPlayer.AI_totalUnitAIs(UNITAI_ICBM);
			int iNukesWanted = 1 + 2 * std::min(kPlayer.getNumCities(), GC.getGame().getNumCities() - kPlayer.getNumCities());
			if ((iTotalNukes < iNukesWanted) && (GC.getGame().getSorenRandNum(100, "AI train nuke MWAHAHAH") < (90 - (80 * iTotalNukes) / iNukesWanted)))
			{
				if ((pWaterArea != NULL))
				{
					if (AI_chooseUnit(UNITAI_MISSILE_CARRIER_SEA, 50))
					{
						return;
					}
				}

				if (AI_chooseUnit(UNITAI_ICBM))
				{
					return;
				}
			}
		}
	} */

	// Assault case now completely handled above
	if (!bAssault && (!bImportantCity || bDefenseWar) && (iUnitSpending < iMaxUnitSpending))
	{
		if (!bFinancialTrouble && (bLandWar || (bDagger && !bGetBetterUnits)))
		{
			//int iTrainInvaderChance = iBuildUnitProb + 10;
			int iTrainInvaderChance = iBuildUnitProb + (bTotalWar ? 16 : 8); // K-Mod

			if (bAggressiveAI)
			{
				iTrainInvaderChance += 15;
			}

			if( bGetBetterUnits )
			{
				iTrainInvaderChance /= 2;
			}
			else if ((pArea->getAreaAIType(getTeam()) == AREAAI_MASSING) || (pArea->getAreaAIType(getTeam()) == AREAAI_ASSAULT_MASSING))
			{
				iTrainInvaderChance = (100 - ((100 - iTrainInvaderChance) / (bCrushStrategy ? 6 : 3)));
			}        	

			if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 20, 0, bDefenseWar ? 10 : 30))
			{
				return;
			}

			UnitTypeWeightArray invaderTypes;
			invaderTypes.push_back(std::make_pair(UNITAI_ATTACK_CITY, 100));
			invaderTypes.push_back(std::make_pair(UNITAI_COUNTER, 50));
			invaderTypes.push_back(std::make_pair(UNITAI_ATTACK, 40));
			invaderTypes.push_back(std::make_pair(UNITAI_PARADROP, (kPlayer.AI_isDoStrategy(AI_STRATEGY_AIR_BLITZ) ? 30 : 20) / (bAssault ? 2 : 1)));
			//if (!bAssault)
			if (!bAssault && !bCrushStrategy) // K-Mod
			{
				if (kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_PILLAGE) <= ((iNumCitiesInArea + 1) / 2))
				{
					invaderTypes.push_back(std::make_pair(UNITAI_PILLAGE, 30));
				}
			}

			// K-Mod - get more seige units for crush
			if (bCrushStrategy && GC.getGameINLINE().getSorenRandNum(100, "City AI extra crush bombard") < iTrainInvaderChance)
			{
				UnitTypes eCityAttackUnit = NO_UNIT;
				kPlayer.AI_bestCityUnitAIValue(UNITAI_ATTACK_CITY, this, &eCityAttackUnit);
				if (eCityAttackUnit != NO_UNIT && GC.getUnitInfo(eCityAttackUnit).getBombardRate() > 0)
				{
					if (AI_chooseUnit(eCityAttackUnit, UNITAI_ATTACK_CITY))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses extra crush bombard", getName().GetCString());
						return;
					}
				}
			}
			// K-Mod end

			if (AI_chooseLeastRepresentedUnit(invaderTypes, iTrainInvaderChance))
			{
				return;
			}
		}
	}

	if ((pWaterArea != NULL) && bWaterAreaRelevant && !bDefenseWar && !bAssault)
	{
		if( !bFinancialTrouble )
		{
			// Force civs with foreign colonies to build a few assault transports to defend the colonies
			if( kPlayer.AI_totalUnitAIs(UNITAI_ASSAULT_SEA) < (kPlayer.getNumCities() - iNumCapitalAreaCities)/3 )
			{
				if (AI_chooseUnit(UNITAI_ASSAULT_SEA))
				{
					return;
				}
			}

			if (kPlayer.AI_calculateUnitAIViability(UNITAI_SETTLER_SEA, DOMAIN_SEA) < 61)
			{
				// Force civs to build escorts for settler_sea units
				if( kPlayer.AI_totalUnitAIs(UNITAI_SETTLER_SEA) > kPlayer.AI_getNumAIUnits(UNITAI_RESERVE_SEA) )
				{
					if (AI_chooseUnit(UNITAI_RESERVE_SEA))
					{
						return;
					}
				}
			}
		}
	}
	
	if (!bLandWar && !bFinancialTrouble)
	{
		if ((pWaterArea != NULL && bWaterAreaRelevant) && (iWaterPercent > 40))
		{
			if (kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_SPY) > 0)
			{
				if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_SPY_SEA) == 0)
				{
					if (AI_chooseUnit(UNITAI_SPY_SEA))
					{
						return;
					}
				}
			}
		}
	}
	
	if (iBestSpreadUnitValue > ((iSpreadUnitThreshold * 40) / 100))
	{
		if (AI_chooseUnit(eBestSpreadUnit, UNITAI_MISSIONARY))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose missionary 3", getName().GetCString());
			return;
		}
		FAssertMsg(false, "AI_bestSpreadUnit should provide a valid unit when it returns true");
	}
		
	if (!bUnitExempt && iTotalFloatingDefenders < iNeededFloatingDefenders && (!bFinancialTrouble || bLandWar))
	{
		if (AI_chooseLeastRepresentedUnit(floatingDefenderTypes, 50))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose floating defender 2", getName().GetCString());
			return;
		}
	}

	if (bLandWar && !bDanger)
	{
		if (iNumSettlers < iMaxSettlers)
		{
			if (!bFinancialTrouble)
			{
				if (iAreaBestFoundValue > iMinFoundValue)
				{
					if (AI_chooseUnit(UNITAI_SETTLE))
					{
						if( gCityLogLevel >= 2 ) logBBAI("      City %S uses build settler 2", getName().GetCString());
						return;
					}
				}
			}
		}
	}
	
	//if ((iProductionRank <= ((kPlayer.getNumCities() > 8) ? 3 : 2))
	// Ideally we'd look at relative production, not just rank.
	//if (iProductionRank <= kPlayer.getNumCities()/9 + 2	&& getPopulation() > 3)
	if (iProductionRank <= kPlayer.getNumCities()/7 + 1	&& getPopulation() > 3)
	{
		//int iWonderRand = 8 + GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getWonderConstructRand(), "Wonder Construction Rand");
		int iWonderRand = 40 + GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getWonderConstructRand(), "Wonder Construction Rand"); //Civ4 Reimagined
		
		// Civ4 Reimagined
		if (kPlayer.AI_isDoStrategy(AI_STRATEGY_SPECIALIST_ECONOMY))
		{
			iWonderRand *= 3;
			iWonderRand /= 2;
		}
		
		// increase chance of going for an early wonder
		if (GC.getGameINLINE().getElapsedGameTurns() < (100 * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent() / 100) && iNumCitiesInArea > 1)
		{
			iWonderRand *= 35;
			iWonderRand /= 100;
		}
		else if (iNumCitiesInArea >= 3)
		{
			iWonderRand *= 30;
			iWonderRand /= 100;
		}
		else
		{
			iWonderRand *= 25;
			iWonderRand /= 100;
		}
		
		if (bAggressiveAI)
		{
			iWonderRand *= 2;
			iWonderRand /= 3;
		}

		/* if (bLandWar && bTotalWar)
		{
			iWonderRand *= 2;
			iWonderRand /= 3;
		} */
		// K-Mod. When losing a war, it's not really an "opportune" time to build a wonder...
		if (bLandWar)
			iWonderRand = iWonderRand * 2/3;
		if (bTotalWar)
			iWonderRand = iWonderRand * 2/3;
		if (iWarSuccessRating < 0)
			iWonderRand = iWonderRand * 10/(10-iWarSuccessRating);
		// K-Mod end
		
		int iWonderRoll = GC.getGameINLINE().getSorenRandNum(100, "Wonder Build Rand");
		
		// Civ4 Reimagined
		if (iProductionRank == 1)
		{
			iWonderRoll /= 3;
		}
		else if (iProductionRank == 2)
		{
			iWonderRoll *= 2;
			iWonderRoll /= 3;
		}
		
		if (iWonderRoll < iWonderRand)
		{
			int iWonderMaxTurns = 20 + ((iWonderRand - iWonderRoll) * 2);
			if (bLandWar)
			{
				iWonderMaxTurns /= 2;
			}
			
			if (AI_chooseBuilding(BUILDINGFOCUS_WORLDWONDER, iWonderMaxTurns))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses opportunistic wonder build 3", getName().GetCString());
				return;
			}
		}
	}
	
	if (iUnitSpending < iMaxUnitSpending + 12 && !bFinancialTrouble) // was +4 (new metric)
	{
		if ((iAircraftHave * 2 >= iAircraftNeed) && (iAircraftHave < iAircraftNeed))
		{
			int iOdds = 33;

			if( iFreeAirExperience > 0 || (iProductionRank <= (1 + kPlayer.getNumCities()/2)) )
			{
				iOdds = -1;
			}

			if (AI_chooseLeastRepresentedUnit(airUnitTypes, iOdds))
			{
				return;
			}
		}
	}

	if (!bLandWar)
	{
		if (pWaterArea != NULL && bWaterAreaRelevant && bFinancialTrouble)
		{
			if (kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_MISSIONARY) > 0)
			{
				if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_MISSIONARY_SEA) == 0)
				{
					if (AI_chooseUnit(UNITAI_MISSIONARY_SEA))
					{
						return;
					}
				}
			}
		}
	}


	if (getCommerceRateTimes100(COMMERCE_CULTURE) == 0)
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_CULTURE, 30))
		{
			return;
		}
	}

	if (!bAlwaysPeace )
	{
	    if (!bDanger)
	    {
			if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 20, 0, 3*getPopulation()))
            {
                return;
            }
	    }

		if (AI_chooseBuilding(BUILDINGFOCUS_DEFENSE, 20, 0, bDanger ? -1 : 3*getPopulation()))
		{
			return;
		}
		
		if (bDanger)
	    {
            if (AI_chooseBuilding(BUILDINGFOCUS_EXPERIENCE, 20, 0, 2*getPopulation()))
            {
                return;
            }
	    }
	}

	// Short-circuit 3 - a last chance to catch important buildings
	{
		int iOdds = std::max(0, (bLandWar ? 160 : 220) * iBestBuildingValue / (iBestBuildingValue + 32) - 100);
		if (AI_chooseBuilding(0, INT_MAX, 0, iOdds))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses building value short-circuit 3 (odds: %d)", getName().GetCString(), iOdds);
   			return;
		}
	}

	if (!bLandWar)
	{
		if (pWaterArea != NULL && bWaterAreaRelevant)
		{
			if (bPrimaryArea)
			{
				if (kPlayer.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_EXPLORE_SEA) < std::min(1, kPlayer.AI_neededExplorers(pWaterArea)))
				{
					// Civ4 Reimagined
					if (pWaterArea->getNumTiles() > 70)
					{
						if (AI_chooseUnit(UNITAI_EXPLORE_SEA))
						{
							return;
						}
					}
				}
			}
		}
	}

	if (!bUnitExempt && plot()->plotCheck(PUF_isUnitAIType, UNITAI_CITY_COUNTER, -1, getOwnerINLINE()) == NULL)
	{
		if (AI_chooseUnit(UNITAI_CITY_COUNTER))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose city counter", getName().GetCString());
			return;
		}
	}

	// we do a similar check lower, in the landwar case. (umm. no you don't. I'm changing this.)
	//if (!bLandWar && bFinancialTrouble)
	if (bFinancialTrouble)
	{
		if (AI_chooseBuilding(BUILDINGFOCUS_GOLD))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose financial trouble gold", getName().GetCString());
			return;
		}
	}

	bool bChooseUnit = false;
	if (iUnitSpending < iMaxUnitSpending + 15) // was +5 (new metric)
	{
		// K-Mod
		iBuildUnitProb *= (250 + std::max(iProjectValue, iBestBuildingValue));
		iBuildUnitProb /= (100 + 3 * std::max(iProjectValue, iBestBuildingValue));
		// K-Mod end

		if ((bLandWar) ||
			  ((kPlayer.getNumCities() <= 3) && (GC.getGameINLINE().getElapsedGameTurns() < 60)) ||
			  (!bUnitExempt && GC.getGameINLINE().getSorenRandNum(100, "AI Build Unit Production") < iBuildUnitProb) ||
				(isHuman() && (getGameTurnFounded() == GC.getGameINLINE().getGameTurn())))
		{
			if (AI_chooseUnit())
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose unit by probability", getName().GetCString());
				return;
			}

			bChooseUnit = true;
		}
	}

	// Only cities with reasonable production
	/* original bts code
	if ((iProductionRank <= ((kPlayer.getNumCities() > 8) ? 3 : 2))
		&& (getPopulation() > 3))
	{
		if (AI_chooseProject())
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose project 2", getName().GetCString());
			return;
		}
	} */
	// K-Mod
	FAssert((eBestProject == NO_PROJECT) == (iProjectValue <= 0));
	if (iProjectValue < 0)
		eBestProject = AI_bestProject(&iProjectValue);
	if (iProjectValue > iBestBuildingValue)
	{
		FAssert(eBestProject != NO_PROJECT);
		pushOrder(ORDER_CREATE, eBestProject);
		if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose project 2. (project value: %d, building value: %d)", getName().GetCString(), iProjectValue, iBestBuildingValue);
		return;
	}

	ProcessTypes eBestProcess = AI_bestProcess();
	if (eBestProcess != NO_PROCESS)
	{
		// unfortunately, the evaluation of eBestProcess is duplicated.
		// Here we calculate roughly how many turns it would take for the building to pay itself back, where the cost it the value we could have had from building a process.. 
		// Note that as soon as we start working the process, our citizens will rearrange in a way that is likely to devalue the process for the next turn.
		int iOdds = 100;
		if (eBestBuilding != NO_BUILDING)
		{
			int iBuildingCost = AI_processValue(eBestProcess) * getProductionTurnsLeft(eBestBuilding, 0);
			int iScaledTime = 10000 * iBuildingCost / (std::max(1, iBestBuildingValue) * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent());
			iOdds = 100*(iScaledTime - 400)/(iScaledTime + 600); // <= 4 turns means 0%. 20 turns ~ 61%.
		}
		if (iOdds > 0 && GC.getGameINLINE().getSorenRandNum(100, "AI choose process") < iOdds)
		{
			pushOrder(ORDER_MAINTAIN, eBestProcess);
			if (gCityLogLevel >= 2) logBBAI("      City %S uses choose process by value", getName().GetCString());
			return;
		}
	}
	// K-Mod end

	if (AI_chooseBuilding())
	{
		if( gCityLogLevel >= 2 ) logBBAI("      City %S uses choose building by probability", getName().GetCString());
		return;
	}
	
	if (!bChooseUnit && !bFinancialTrouble && kPlayer.AI_isDoStrategy(AI_STRATEGY_FINAL_WAR))
	{
		if (AI_chooseUnit())
		{
			if (gCityLogLevel >= 2) logBBAI("      City %S uses choose unit by default", getName().GetCString());
			return;
		}
	}

	//if (AI_chooseProcess())
	if (eBestProcess != NO_PROCESS)
	{
		pushOrder(ORDER_MAINTAIN, eBestProcess);
		if (gCityLogLevel >= 2) logBBAI("      City %S uses choose process by default", getName().GetCString());
		return;
	}
}

UnitTypes CvCityAI::AI_bestUnit(bool bAsync, AdvisorTypes eIgnoreAdvisor, UnitAITypes* peBestUnitAI)
{
	CvArea* pWaterArea;
	int aiUnitAIVal[NUM_UNITAI_TYPES];
	UnitTypes eUnit;
	UnitTypes eBestUnit;
	bool bWarPlan;
	bool bDefense;
	bool bLandWar;
	bool bAssault;
	bool bPrimaryArea;
	bool bAreaAlone;
	bool bFinancialTrouble;
	bool bWarPossible;
	bool bDanger;
	int iHasMetCount;
	int iMilitaryWeight;
	int iCoastalCities = 0;
	int iBestValue;
	int iI;

	if (peBestUnitAI != NULL)
	{
		*peBestUnitAI = NO_UNITAI;
	}

	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE()); // K-Mod

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      11/30/08                                jdog5000      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
/* original bts code
	pWaterArea = waterArea();
*/
	pWaterArea = waterArea(true);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	bWarPlan = (GET_TEAM(getTeam()).getAnyWarPlanCount(true) > 0);
	bDefense = (area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE);
	//bLandWar = (bDefense || (area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING));
	bLandWar = kOwner.AI_isLandWar(area()); // K-Mod
	bAssault = (area()->getAreaAIType(getTeam()) == AREAAI_ASSAULT);
	bPrimaryArea = kOwner.AI_isPrimaryArea(area());
	bAreaAlone = kOwner.AI_isAreaAlone(area());
	bFinancialTrouble = kOwner.AI_isFinancialTrouble();
	bWarPossible = GET_TEAM(getTeam()).AI_isWarPossible();
	bDanger = AI_isDanger();

	iHasMetCount = GET_TEAM(getTeam()).getHasMetCivCount(true);
	iMilitaryWeight = kOwner.AI_militaryWeight(area());
	int iNumCitiesInArea = area()->getCitiesPerPlayer(getOwnerINLINE());

	if (pWaterArea != NULL)
	{
		iCoastalCities = kOwner.countNumCoastalCitiesByArea(pWaterArea);
	}

	for (iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		aiUnitAIVal[iI] = 0;
	}

	if (!bFinancialTrouble && ((bPrimaryArea) ? (kOwner.findBestFoundValue() > 0) : (area()->getBestFoundValue(getOwnerINLINE()) > 0)))
	{
		aiUnitAIVal[UNITAI_SETTLE]++;
	}

	aiUnitAIVal[UNITAI_WORKER] += kOwner.AI_neededWorkers(area());

	aiUnitAIVal[UNITAI_ATTACK] += ((iMilitaryWeight / ((bWarPlan || bLandWar || bAssault) ? 7 : 12)) + ((bPrimaryArea && bWarPossible) ? 2 : 0) + 1);

	aiUnitAIVal[UNITAI_CITY_DEFENSE] += (iNumCitiesInArea + 1);
	aiUnitAIVal[UNITAI_CITY_COUNTER] += ((5 * (iNumCitiesInArea + 1)) / 8);
	aiUnitAIVal[UNITAI_CITY_SPECIAL] += ((iNumCitiesInArea + 1) / 2);

	if (bWarPossible)
	{
		aiUnitAIVal[UNITAI_ATTACK_CITY] += ((iMilitaryWeight / ((bWarPlan || bLandWar || bAssault) ? 10 : 17)) + ((bPrimaryArea) ? 1 : 0));
		aiUnitAIVal[UNITAI_COUNTER] += ((iMilitaryWeight / ((bWarPlan || bLandWar || bAssault) ? 13 : 22)) + ((bPrimaryArea) ? 1 : 0));
		// Civ4 Reimagined
		aiUnitAIVal[UNITAI_PARADROP] += ((iMilitaryWeight / ((bWarPlan || bLandWar || bAssault) ? 7 : 11)) + ((bPrimaryArea) ? 1 : 0));

		//aiUnitAIVal[UNITAI_DEFENSE_AIR] += (kOwner.getNumCities() + 1);
		aiUnitAIVal[UNITAI_DEFENSE_AIR] += kOwner.AI_getTotalAirDefendersNeeded(); // K-Mod
		aiUnitAIVal[UNITAI_CARRIER_AIR] += kOwner.AI_countCargoSpace(UNITAI_CARRIER_SEA);
		aiUnitAIVal[UNITAI_MISSILE_AIR] += kOwner.AI_countCargoSpace(UNITAI_MISSILE_CARRIER_SEA);

		if (bPrimaryArea)
		{
			//aiUnitAIVal[UNITAI_ICBM] += std::max((kOwner.getTotalPopulation() / 25), ((GC.getGameINLINE().countCivPlayersAlive() + GC.getGameINLINE().countTotalNukeUnits()) / (GC.getGameINLINE().countCivPlayersAlive() + 1)));
			// K-Mod
			aiUnitAIVal[UNITAI_ICBM] += std::max((kOwner.getTotalPopulation() / 25), ((GC.getGameINLINE().countFreeTeamsAlive() + GC.getGameINLINE().countTotalNukeUnits() + GC.getGameINLINE().getNukesExploded()) / (GC.getGameINLINE().countFreeTeamsAlive() + 1)));
		}
	}

	if (isBarbarian())
	{
		aiUnitAIVal[UNITAI_ATTACK] *= 2;
	}
	else
	{
		if (!bLandWar)
		{
			aiUnitAIVal[UNITAI_EXPLORE] += kOwner.AI_neededExplorers(area());
		}

		if (pWaterArea != NULL)
		{
			aiUnitAIVal[UNITAI_WORKER_SEA] += AI_neededSeaWorkers();

			if ((kOwner.getNumCities() > 3) || (area()->getNumUnownedTiles() < 10))
			{
				//if (bPrimaryArea)
				// Civ4 Reimagined
				if (bPrimaryArea && pWaterArea->getNumTiles() > 70)
				{
					aiUnitAIVal[UNITAI_EXPLORE_SEA] += kOwner.AI_neededExplorers(pWaterArea);
				}

				if (bPrimaryArea && (kOwner.findBestFoundValue() > 0) && (pWaterArea->getNumTiles() > 300))
				{
					aiUnitAIVal[UNITAI_SETTLER_SEA]++;
				}

				if (bPrimaryArea && (kOwner.AI_totalAreaUnitAIs(area(), UNITAI_MISSIONARY) > 0) && (pWaterArea->getNumTiles() > 400))
				{
					aiUnitAIVal[UNITAI_MISSIONARY_SEA]++;
				}

				if (bPrimaryArea && (kOwner.AI_totalAreaUnitAIs(area(), UNITAI_SPY) > 0) && (pWaterArea->getNumTiles() > 500))
				{
					aiUnitAIVal[UNITAI_SPY_SEA]++;
				}

				// Civ4 Reimagined: Don't build pirates randomly
				//aiUnitAIVal[UNITAI_PIRATE_SEA] += pWaterArea->getNumTiles() / 600;

				if (bWarPossible)
				{
					// K-Mod note: this is bogus. TODO: change it so that it scales properly with map size.
					// Changed by Civ4 Reimagined
					aiUnitAIVal[UNITAI_ATTACK_SEA] += std::min((pWaterArea->getNumTiles() / 150), ((((iCoastalCities * 2) + (iMilitaryWeight / 8)) / ((bAssault) ? 4 : 6)) + ((bPrimaryArea) ? 1 : 0)));
					aiUnitAIVal[UNITAI_RESERVE_SEA] += std::min((pWaterArea->getNumTiles() / 200), ((((iCoastalCities * 2) + (iMilitaryWeight / 7)) / 5) + ((bPrimaryArea) ? 1 : 0)));
					aiUnitAIVal[UNITAI_ESCORT_SEA] += (kOwner.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_ASSAULT_SEA) + (kOwner.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_CARRIER_SEA) * 2));
					aiUnitAIVal[UNITAI_ASSAULT_SEA] += std::min((pWaterArea->getNumTiles() / 250), ((((iCoastalCities * 2) + (iMilitaryWeight / 6)) / ((bAssault) ? 5 : 8)) + ((bPrimaryArea) ? 1 : 0)));
					aiUnitAIVal[UNITAI_CARRIER_SEA] += std::min((pWaterArea->getNumTiles() / 300), ((((iCoastalCities * 2) + (iMilitaryWeight / 7)) / 6) + ((bPrimaryArea) ? 1 : 0)));
					aiUnitAIVal[UNITAI_MISSILE_CARRIER_SEA] += std::min((pWaterArea->getNumTiles() / 350), ((((iCoastalCities * 2) + (iMilitaryWeight / 7)) / 7) + ((bPrimaryArea) ? 1 : 0)));
				}
			}
		}

		if ((iHasMetCount > 0) && bWarPossible)
		{
			if (bLandWar || bAssault || !bFinancialTrouble || (kOwner.calculateUnitCost() == 0))
			{
				// Changed by Civ4 Reimagined
				aiUnitAIVal[UNITAI_ATTACK] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 9 : 16)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
				aiUnitAIVal[UNITAI_ATTACK_CITY] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 7 : 15)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
				aiUnitAIVal[UNITAI_COLLATERAL] += ((iMilitaryWeight / ((bDefense) ? 8 : 14)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
				aiUnitAIVal[UNITAI_PILLAGE] += ((iMilitaryWeight / ((bLandWar) ? 10 : 19)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
				aiUnitAIVal[UNITAI_RESERVE] += ((iMilitaryWeight / ((bLandWar) ? 12 : 17)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
				aiUnitAIVal[UNITAI_COUNTER] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 9 : 16)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));
				aiUnitAIVal[UNITAI_PARADROP] += ((iMilitaryWeight / ((bLandWar || bAssault) ? 6 : 12)) + ((bPrimaryArea && !bAreaAlone) ? 1 : 0));

				//aiUnitAIVal[UNITAI_ATTACK_AIR] += (kOwner.getNumCities() + 1);
				// K-Mod (extra air attack and defence). Note: iMilitaryWeight is (pArea->getPopulationPerPlayer(getID()) + pArea->getCitiesPerPlayer(getID())
				aiUnitAIVal[UNITAI_ATTACK_AIR] += (bLandWar ? 2 : 1) * kOwner.getNumCities() + 1;
				aiUnitAIVal[UNITAI_DEFENSE_AIR] += (bDefense ? 1 : 0) * kOwner.getNumCities() + 1; // it would be nice if this was based on enemy air power...

				if (pWaterArea != NULL)
				{
					if ((kOwner.getNumCities() > 3) || (area()->getNumUnownedTiles() < 10))
					{
						aiUnitAIVal[UNITAI_ATTACK_SEA] += std::min((pWaterArea->getNumTiles() / 100), ((((iCoastalCities * 2) + (iMilitaryWeight / 10)) / ((bAssault) ? 5 : 7)) + ((bPrimaryArea) ? 1 : 0)));
						aiUnitAIVal[UNITAI_RESERVE_SEA] += std::min((pWaterArea->getNumTiles() / 150), ((((iCoastalCities * 2) + (iMilitaryWeight / 11)) / 8) + ((bPrimaryArea) ? 1 : 0)));
					}
				}
			}
			// K-Mod
			if (bLandWar && !bDefense && !isHuman() && GET_TEAM(getTeam()).getWarPlanCount(WARPLAN_TOTAL, true))
			{
				// if we're winning, then focus on capturing cities.
				int iSuccessRatio = GET_TEAM(getTeam()).AI_getWarSuccessRating();
				if (iSuccessRatio > 0)
				{
					aiUnitAIVal[UNITAI_ATTACK] += iSuccessRatio * iMilitaryWeight / 800;
					aiUnitAIVal[UNITAI_ATTACK_CITY] += iSuccessRatio * iMilitaryWeight / 1000;
					aiUnitAIVal[UNITAI_COUNTER] += iSuccessRatio * iMilitaryWeight / 1400;
					aiUnitAIVal[UNITAI_PARADROP] += iSuccessRatio * iMilitaryWeight / 1000;
				}
			}
			// K-Mod end
		}
	}

	// XXX this should account for air and heli units too...
	// K-Mod. Human players don't choose the AI type of the units they build. Therefore we shouldn't use the unit AI counts to decide what to build next.
	if (isHuman())
	{
		aiUnitAIVal[UNITAI_SETTLE] = 0;
		aiUnitAIVal[UNITAI_WORKER] = 0;
		aiUnitAIVal[UNITAI_WORKER_SEA] = 0;
		aiUnitAIVal[UNITAI_EXPLORE] = 0;
		aiUnitAIVal[UNITAI_EXPLORE_SEA] = 0;
		aiUnitAIVal[UNITAI_ATTACK_CITY] /= 3;
		aiUnitAIVal[UNITAI_COLLATERAL] /= 2;
		aiUnitAIVal[UNITAI_PILLAGE] /= 3;
	}
	else
	// K-Mod end
	{
		for (iI = 0; iI < NUM_UNITAI_TYPES; iI++)
		{
			if (kOwner.AI_unitAIDomainType((UnitAITypes)iI) == DOMAIN_SEA)
			{
				if (pWaterArea != NULL)
				{
					aiUnitAIVal[iI] -= kOwner.AI_totalWaterAreaUnitAIs(pWaterArea, ((UnitAITypes)iI));
				}
			}
			else if ((kOwner.AI_unitAIDomainType((UnitAITypes)iI) == DOMAIN_AIR) || (iI == UNITAI_ICBM))
			{
				aiUnitAIVal[iI] -= kOwner.AI_totalUnitAIs((UnitAITypes)iI);
			}
			else
			{
				aiUnitAIVal[iI] -= kOwner.AI_totalAreaUnitAIs(area(), ((UnitAITypes)iI));
			}
		}
	}

	aiUnitAIVal[UNITAI_SETTLE] *= ((bDanger) ? 8 : 12); // was ? 8 : 20
	aiUnitAIVal[UNITAI_WORKER] *= ((bDanger) ? 2 : 7);
	aiUnitAIVal[UNITAI_ATTACK] *= 3;
	aiUnitAIVal[UNITAI_ATTACK_CITY] *= 5; // K-Mod, up from *4
	aiUnitAIVal[UNITAI_COLLATERAL] *= 5;
	aiUnitAIVal[UNITAI_PILLAGE] *= 3;
	aiUnitAIVal[UNITAI_RESERVE] *= 3;
	aiUnitAIVal[UNITAI_COUNTER] *= 3;
	aiUnitAIVal[UNITAI_CITY_DEFENSE] *= 2;
	aiUnitAIVal[UNITAI_CITY_COUNTER] *= 2;
	aiUnitAIVal[UNITAI_CITY_SPECIAL] *= 2;
	aiUnitAIVal[UNITAI_EXPLORE] *= ((bDanger) ? 6 : 15);
	//aiUnitAIVal[UNITAI_ICBM] *= 18;
	aiUnitAIVal[UNITAI_ICBM] *= 18 * kOwner.AI_nukeWeight() / 100; // K-Mod
	aiUnitAIVal[UNITAI_WORKER_SEA] *= ((bDanger) ? 3 : 10);
	aiUnitAIVal[UNITAI_ATTACK_SEA] *= 5;
	aiUnitAIVal[UNITAI_RESERVE_SEA] *= 4;
	aiUnitAIVal[UNITAI_ESCORT_SEA] *= 20;
	aiUnitAIVal[UNITAI_EXPLORE_SEA] *= 18;
	aiUnitAIVal[UNITAI_ASSAULT_SEA] *= 14;
	aiUnitAIVal[UNITAI_SETTLER_SEA] *= 16;
	aiUnitAIVal[UNITAI_MISSIONARY_SEA] *= 12;
	aiUnitAIVal[UNITAI_SPY_SEA] *= 10;
	aiUnitAIVal[UNITAI_CARRIER_SEA] *= 8;
	aiUnitAIVal[UNITAI_MISSILE_CARRIER_SEA] *= 8;
	// Civ4 Reimagined
	//aiUnitAIVal[UNITAI_PIRATE_SEA] *= 5;
	aiUnitAIVal[UNITAI_ATTACK_AIR] *= 6;
	aiUnitAIVal[UNITAI_DEFENSE_AIR] *= 4; // K-Mod, up from *3
	aiUnitAIVal[UNITAI_CARRIER_AIR] *= 15;
	aiUnitAIVal[UNITAI_MISSILE_AIR] *= 15;

	// K-Mod
	if (GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_CRUSH))
	{
		aiUnitAIVal[UNITAI_ATTACK_CITY] *= 2;
		aiUnitAIVal[UNITAI_ATTACK] *= 2;
		aiUnitAIVal[UNITAI_ATTACK_AIR] *= 3;
	}
	if (GET_TEAM(getTeam()).AI_getRivalAirPower() <= 8 * GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), UNITAI_DEFENSE_AIR))
	{
		// unfortunately, I don't have an easy way to get the approximate power of our air defence units.
		// So I'm just going to assume the power of each unit is around 12 - the power of a fighter plane.
		aiUnitAIVal[UNITAI_DEFENSE_AIR] /= 4;
	}
	// K-Mod end

	for (iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		aiUnitAIVal[iI] *= std::max(0, (GC.getLeaderHeadInfo(getPersonalityType()).getUnitAIWeightModifier(iI) + 100));
		aiUnitAIVal[iI] /= 100;
	}

	iBestValue = 0;
	eBestUnit = NO_UNIT;

	for (iI = 0; iI < NUM_UNITAI_TYPES; iI++)
	{
		if (aiUnitAIVal[iI] > 0)
		{
			if (bAsync)
			{
				aiUnitAIVal[iI] += GC.getASyncRand().get(iMilitaryWeight, "AI Best UnitAI ASYNC");
			}
			else
			{
				aiUnitAIVal[iI] += GC.getGameINLINE().getSorenRandNum(iMilitaryWeight, "AI Best UnitAI");
			}

			if (aiUnitAIVal[iI] > iBestValue)
			{
				eUnit = AI_bestUnitAI(((UnitAITypes)iI), bAsync, eIgnoreAdvisor);

				if (eUnit != NO_UNIT)
				{
					// Civ4 Reimagined
					int iProductionMultiplier = getUnitProductionMultiplier(eUnit);
					if (iProductionMultiplier == 0)
					{
						for (HurryTypes i = (HurryTypes)0; i < GC.getNumHurryInfos(); i=(HurryTypes)(i+1))
						{
							if (GET_PLAYER(getOwnerINLINE()).canHurry(i))
							{
								const CvHurryInfo& kHurryInfo = GC.getHurryInfo(i);
								if (kHurryInfo.getGoldPerProduction() > 0 && kHurryInfo.isUnits())
								{
									int iHurryGold = getHurryGold(i, getHurryCost(false, eUnit, false));
									if (iHurryGold * 2 <= GET_PLAYER(getOwnerINLINE()).getGold() - GET_PLAYER(getOwnerINLINE()).AI_goldTarget(true))
									{
										//iProductionMultiplier = 25;
										iProductionMultiplier = 100;
										break;
									}
								}
							}
						}
					}
					
					if (aiUnitAIVal[iI] * iProductionMultiplier / 100 > iBestValue)
					{
						iBestValue = aiUnitAIVal[iI];
						eBestUnit = eUnit;
						if (peBestUnitAI != NULL)
						{
							*peBestUnitAI = ((UnitAITypes)iI);
						}
					}
				}
			}
		}
	}

	return eBestUnit;
}


// Choose the best unit to build for the given AI type.
// Note: a lot of this function has been restructured / rewritten for K-Mod
UnitTypes CvCityAI::AI_bestUnitAI(UnitAITypes eUnitAI, bool bAsync, AdvisorTypes eIgnoreAdvisor)
{
	PROFILE_FUNC();
	FAssertMsg(eUnitAI != NO_UNITAI, "UnitAI is not assigned a valid value");

	bool bGrowMore = false;

	//if (foodDifference() > 0)
	if (foodDifference(true, true) > 0) // K-Mod
	{
		// BBAI NOTE: This is where small city worker and settler production is blocked
		if (GET_PLAYER(getOwnerINLINE()).getNumCities() <= 2)
		{
			/* original bts code
			bGrowMore = ((getPopulation() < 3) && (AI_countGoodTiles(true, false, 100) >= getPopulation())); */
			// K-Mod. We need to allow the starting city to build a worker at size 1.
			bGrowMore = (eUnitAI != UNITAI_WORKER || GET_PLAYER(getOwner()).AI_totalAreaUnitAIs(area(), UNITAI_WORKER) > 0)
				&& getPopulation() < 3 && AI_countGoodTiles(true, false, 100) >= getPopulation();
			// K-Mod end
		}
		else
		{
			bGrowMore = ((getPopulation() < 3) || (AI_countGoodTiles(true, false, 100) >= getPopulation()));
		}

		if (!bGrowMore && (getPopulation() < 6) && (AI_countGoodTiles(true, false, 80) >= getPopulation()))
		{
			if ((getFood() - (getFoodKept() / 2)) >= (growthThreshold() / 2))
			{
				if ((angryPopulation(1) == 0) && (healthRate(false, 1) == 0))
				{
					bGrowMore = true;
				}
			}
		}
		// K-Mod
		else if (bGrowMore)
		{
			if (angryPopulation(1) > 0)
				bGrowMore = false;
		}
		// K-Mod end
	}


	std::vector<std::pair<int, UnitTypes> > candidates;
	int iBestBaseValue = 0;

	for (int i = 0; i < GC.getNumUnitClassInfos(); i++)
	{
		UnitTypes eLoopUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(i);

		if (eLoopUnit == NO_UNIT)
			continue;

		if (eIgnoreAdvisor != NO_ADVISOR && GC.getUnitInfo(eLoopUnit).getAdvisorType() == eIgnoreAdvisor)
			continue;

		if (bGrowMore && isFoodProduction(eLoopUnit))
			continue;

		if (!canTrain(eLoopUnit))
			continue;

		int iValue = GET_PLAYER(getOwnerINLINE()).AI_unitValue(eLoopUnit, eUnitAI, area());
		if (iValue > 0)
		{
			// Civ4 Reimagined		
			if (!bGrowMore && isFoodProduction(eLoopUnit))
			{
				iValue *= 3;
				iValue /= 2;
			}
			candidates.push_back(std::make_pair(iValue, eLoopUnit));
			if (iValue > iBestBaseValue)
				iBestBaseValue = iValue;
		}
	}

	int iBestValue = 0;
	UnitTypes eBestUnit = NO_UNIT;

	for (size_t i = 0; i < candidates.size(); i++)
	{
		UnitTypes eLoopUnit = candidates[i].second;
		int iValue = candidates[i].first;

		if (iValue < iBestBaseValue*2/3 || (eUnitAI == UNITAI_EXPLORE && iValue < iBestBaseValue))
			continue;

		const CvUnitInfo& kUnitInfo = GC.getUnitInfo(eLoopUnit);

		iValue *= (getProductionExperience(eLoopUnit) + 10);
		iValue /= 10;

		// Take into account free promotions, and avaiable promotions that suit the AI type.
		// Note: it might be better if this was in AI_unitValue rather than here, but the
		// advantage of doing it here is that we can check city promotions at the same time.
        int iPromotionValue = 0;
		bool bHasGoodPromotion = false; // check that appropriate non-free promotions are available
        for (PromotionTypes p = (PromotionTypes)0; p < GC.getNumPromotionInfos(); p=(PromotionTypes)(p+1))
        {
			bool bFree = kUnitInfo.getFreePromotions(p);
			const CvPromotionInfo& kPromotionInfo = GC.getPromotionInfo(p);

            if (!bFree && isFreePromotion(p))
			{
				if (kUnitInfo.getUnitCombatType() != NO_UNITCOMBAT && kPromotionInfo.getUnitCombat(kUnitInfo.getUnitCombatType()))
                {
					bFree = true;
                }
            }

			if (!bFree)
			{
				for (TraitTypes t = (TraitTypes)0; t < GC.getNumTraitInfos(); t=(TraitTypes)(t+1))
				{
					if (hasTrait(t) && GC.getTraitInfo(t).isFreePromotion(p))
					{
						if (kUnitInfo.getUnitCombatType() != NO_UNITCOMBAT && GC.getTraitInfo(t).isFreePromotionUnitCombat(kUnitInfo.getUnitCombatType()))
						{
							bFree = true;
						}
					}
				}
			}

			// For simplicity, I'll assume that each promotion only gives one type of bonus.
			// (eg. if the promotion gives + city defence, assume that it's a defensive promotion)
			// This may not be true in general, but I think it's a good enough approximation.
			if (!bHasGoodPromotion)
			{
				if (eUnitAI == UNITAI_ATTACK_CITY)
				{
					if (kPromotionInfo.getCityAttackPercent() > 0)
					{
						if (bFree || ::isPromotionValid(p, eLoopUnit, false))
						{
							bHasGoodPromotion = true;
							iPromotionValue += 15;
						}
					}
				}
				else if (eUnitAI == UNITAI_CITY_DEFENSE)
				{
					if (kPromotionInfo.getCityDefensePercent() > 0)
					{
						if (bFree || ::isPromotionValid(p, eLoopUnit, false))
						{
							bHasGoodPromotion = true;
							iPromotionValue += 12;
						}
					}
				}
			}

			if (bFree)
			{
				// Note: in the original bts code, all free promotions were worth 15 points.
				if (kPromotionInfo.getCityAttackPercent() > 0)
				{
					if (eUnitAI == UNITAI_CITY_DEFENSE)
						iPromotionValue += 4;
					else if (eUnitAI == UNITAI_ATTACK || eUnitAI == UNITAI_ATTACK_CITY)
						iPromotionValue += 20;
					else
						iPromotionValue += 8;
				}
				else if (kPromotionInfo.isAmphib())
				{
					if (eUnitAI == UNITAI_CITY_DEFENSE)
					{
						iPromotionValue += 4;
					}
					else if (eUnitAI == UNITAI_ATTACK || eUnitAI == UNITAI_ATTACK_CITY)
					{
						AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());
						if (eAreaAI == AREAAI_ASSAULT_MASSING)
							iPromotionValue += 35;
						else if (eAreaAI == AREAAI_ASSAULT || eAreaAI == AREAAI_ASSAULT_ASSIST)
							iPromotionValue += 20;
						else
							iPromotionValue += 12;
					}
					else
					{
						iPromotionValue += 8;
					}
				}
				else if (kPromotionInfo.getCityDefensePercent() > 0)
				{
					if (eUnitAI == UNITAI_CITY_DEFENSE)
						iPromotionValue += 15;
					else if (eUnitAI == UNITAI_ATTACK || eUnitAI == UNITAI_ATTACK_CITY)
						iPromotionValue += 4;
					else
						iPromotionValue += 8;
				}
				else if (kPromotionInfo.getSameTileHealChange())
				{
					if (eUnitAI == UNITAI_ATTACK_CITY)
						iPromotionValue += 4;
					else
						iPromotionValue += 12;
				}
				else if (kPromotionInfo.getCombatPercent() > 0)
					iPromotionValue += 15;
				else
					iPromotionValue += 12; // generic
			}
		}


		iValue *= (iPromotionValue + 100);
		iValue /= 100;

		if (bAsync)
		{
			iValue *= (GC.getASyncRand().get(50, "AI Best Unit ASYNC") + 100);
			iValue /= 100;
		}
		else
		{
			iValue *= (GC.getGameINLINE().getSorenRandNum(50, "AI Best Unit") + 100);
			iValue /= 100;
		}


		// K-Mod note: I've removed the happy-from-hurry calculation, because it was inaccurate and difficult to fix.
		// (int iBestHappy = 0; ...)

		/* original bts code
		iValue *= (GET_PLAYER(getOwnerINLINE()).getNumCities() * 2);
		iValue /= (GET_PLAYER(getOwnerINLINE()).getUnitClassCountPlusMaking((UnitClassTypes)iI) + GET_PLAYER(getOwnerINLINE()).getNumCities() + 1); */
		// K-Mod
		{
			int iUnits = GET_PLAYER(getOwnerINLINE()).getUnitClassCountPlusMaking((UnitClassTypes)kUnitInfo.getUnitClassType());
			int iCities = GET_PLAYER(getOwnerINLINE()).getNumCities();

			iValue *= 6 + iUnits + 4*iCities;
			iValue /= 3 + 3*iUnits + 2*iCities;
			// this is a factor between 1/3 and 2. It's equal to 1 roughly when iUnits == iCities.
		}
		// K-Mod end

		FAssert((MAX_INT / 1000) > iValue);
		iValue *= 1000;

		bool bIsSuicide = GC.getUnitInfo(eLoopUnit).isSuicide();

		if (bIsSuicide)
		{
			//much of this is compensated
			iValue /= 3;
		}

		//iValue /= std::max(1, (getProductionTurnsLeft(eLoopUnit, 0) + (GC.getUnitInfo(eLoopUnit).isSuicide() ? 1 : 4)));
		// K-Mod. The number of turns is usually not so important. What's important is how much it is going to cost us to build this unit.
		int iProductionNeeded = getProductionNeeded(eLoopUnit) - getUnitProduction(eLoopUnit);
		int iProductionModifier = getBaseYieldRateModifier(YIELD_PRODUCTION, getProductionModifier(eLoopUnit)); // note: this is a 'multiplier' rather than modifier.
		int iProductionMultiplier = getUnitProductionMultiplier(eLoopUnit); // Civ4 Reimagined
		
		// Civ4 Reimagined
		if (iProductionMultiplier == 0)
		{
			for (HurryTypes i = (HurryTypes)0; i < GC.getNumHurryInfos(); i=(HurryTypes)(i+1))
			{
				if (GET_PLAYER(getOwnerINLINE()).canHurry(i))
				{
					const CvHurryInfo& kHurryInfo = GC.getHurryInfo(i);
					if (kHurryInfo.getGoldPerProduction() > 0 && kHurryInfo.isUnits())
					{
						int iHurryGold = getHurryGold(i, getHurryCost(false, eLoopUnit, false));
						if (iHurryGold * 2 <= GET_PLAYER(getOwnerINLINE()).getGold() - GET_PLAYER(getOwnerINLINE()).AI_goldTarget(true))
						{
							iProductionMultiplier = 100;
							break;
						}
					}
				}
			}
		}
		
		
		// Civ4 Reimagined: Added iProductionMultiplier
		iValue *= iProductionModifier * iProductionMultiplier / 100;
		iValue /= std::max(1, iProductionNeeded + getBaseYieldRate(YIELD_PRODUCTION) * iProductionModifier / 100); // a bit of dilution.
		// maybe we should have a special bonus for building it in 1 turn?  ... maybe not.
		// K-Mod end

		iValue = std::max(1, iValue);

		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			eBestUnit = eLoopUnit;
		}
	}
	/*
	if (gCityLogLevel >= 2 && getUnitProductionMultiplier(eBestUnit) == 0)
	{
		logBBAI("choose mercenary %S with value %d", GC.getUnitInfo(eBestUnit).getDescription(), iBestValue);
	}
	*/

	return eBestUnit;
}


BuildingTypes CvCityAI::AI_bestBuilding(int iFocusFlags, int iMaxTurns, bool bAsync, AdvisorTypes eIgnoreAdvisor)
{
	return AI_bestBuildingThreshold(iFocusFlags, iMaxTurns, /*iMinThreshold*/ 0, bAsync, eIgnoreAdvisor);
}

BuildingTypes CvCityAI::AI_bestBuildingThreshold(int iFocusFlags, int iMaxTurns, int iMinThreshold, bool bAsync, AdvisorTypes eIgnoreAdvisor)
{
	/*BuildingTypes eLoopBuilding;
	BuildingTypes eBestBuilding;
	bool bAreaAlone;
	int iProductionRank;
	int iTurnsLeft;
	int iValue;
	int iTempValue;
	int iBestValue;
	int iI, iJ; */ // K-Mod. I've moved the declarations to where they are actually used - to reduce the likelihood of mistakes

	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE()); // K-Mod (and I've replaced all other GET_PLAYER calls in this function)

	bool bAreaAlone = kOwner.AI_isAreaAlone(area());
	int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);

	int iBestValue = 0;
	BuildingTypes eBestBuilding = NO_BUILDING;

	if (iFocusFlags & BUILDINGFOCUS_CAPITAL)
	{
		int iBestTurnsLeft = iMaxTurns > 0 ? iMaxTurns : MAX_INT;
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingTypes eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));

			if (NO_BUILDING != eLoopBuilding)
			{
				if (GC.getBuildingInfo(eLoopBuilding).isCapital())
				{
					if (canConstruct(eLoopBuilding))
					{
						int iTurnsLeft = getProductionTurnsLeft(eLoopBuilding, 0);

						if (iTurnsLeft <= iBestTurnsLeft)
						{
							eBestBuilding = eLoopBuilding;
							iBestTurnsLeft = iTurnsLeft;
						}
					}
				}
			}
		}

		return eBestBuilding;
	}

	// K-Mote note: I've rearranged most of the code below to improve readability and efficiency.
	// Some changes are marked, but most are not.
	// (At least 6 huge nested 'if' blocks have been replaced with 'continue' conditions.)
	for (BuildingClassTypes eLoopClass = (BuildingClassTypes)0; eLoopClass < GC.getNumBuildingClassInfos(); eLoopClass=(BuildingClassTypes)(eLoopClass+1))
	{
		if (kOwner.isBuildingClassMaxedOut(eLoopClass, GC.getBuildingClassInfo(eLoopClass).getExtraPlayerInstances()))
			continue;

		BuildingTypes eLoopBuilding = (BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eLoopClass));

		if (eLoopBuilding == NO_BUILDING || getNumBuilding(eLoopBuilding) >= GC.getCITY_MAX_NUM_BUILDINGS())
			continue;

		if (iFocusFlags & BUILDINGFOCUS_WORLDWONDER && !isWorldWonderClass(eLoopClass))
			continue;

		if (isLimitedWonderClass(eLoopClass))
		{
			if (isProductionAutomated())
				continue;
			if (iFocusFlags != 0 && !(iFocusFlags & BUILDINGFOCUS_WONDEROK))
				continue;
		}

		const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eLoopBuilding);

		if (eIgnoreAdvisor != NO_ADVISOR && eIgnoreAdvisor == kBuilding.getAdvisorType())
			continue;

		if (!canConstruct(eLoopBuilding))
			continue;

		if (isProductionAutomated())
		{
			bool bHasPrereqs = false;
			for (int iJ = 0; !bHasPrereqs && iJ < GC.getNumBuildingClassInfos(); iJ++)
				bHasPrereqs = (kBuilding.getPrereqNumOfBuildingClass(iJ) > 0) || (kBuilding.getPercentPrereqNumOfBuildingClass(iJ) > 0); // Civ4 Reimagined

			if (bHasPrereqs)
				continue;
		}

		int iValue = AI_buildingValue(eLoopBuilding, iFocusFlags, iMinThreshold, bAsync);

		if (iValue <= 0)
			continue;

		//

		/* original bts code
		if (kBuilding.getFreeBuildingClass() != NO_BUILDINGCLASS)
		{
			BuildingTypes eFreeBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(kBuilding.getFreeBuildingClass());
			if (NO_BUILDING != eFreeBuilding)
			{
				//iValue += (AI_buildingValue(eFreeBuilding, iFocusFlags) * (kOwner.getNumCities() - kOwner.getBuildingClassCountPlusMaking((BuildingClassTypes)kBuilding.getFreeBuildingClass())));
				iValue += (AI_buildingValue(eFreeBuilding, iFocusFlags, 0, bAsync) * (kOwner.getNumCities() - kOwner.getBuildingClassCountPlusMaking((BuildingClassTypes)kBuilding.getFreeBuildingClass())));
			}
		} */ // Moved into AI_buildingValue

		// K-Mod
		TechTypes eObsoleteTech = (TechTypes)kBuilding.getObsoleteTech();
		TechTypes eSpObsoleteTech = kBuilding.getSpecialBuildingType() == NO_SPECIALBUILDING
			? NO_TECH
			: (TechTypes)GC.getSpecialBuildingInfo((SpecialBuildingTypes)(kBuilding.getSpecialBuildingType())).getObsoleteTech();

		if ((eObsoleteTech != NO_TECH && kOwner.getCurrentResearch() == eObsoleteTech) || (eSpObsoleteTech != NO_TECH && kOwner.getCurrentResearch() == eSpObsoleteTech))
			iValue /= 2;
		// K-Mod end

		int iTurnsLeft = getProductionTurnsLeft(eLoopBuilding, 0);

		// K-Mod
		// Block construction of limited buildings in bad places
		// (the value check is just for efficiency. 1250 takes into account the possible +25 random boost)
		if (iFocusFlags == 0 && iValue * 1250 / std::max(1, iTurnsLeft + 3) >= iBestValue)
		{
			int iLimit = limitedWonderClassLimit(eLoopClass);
			if (iLimit == -1)
			{
				// We're not out of the woods yet. Check for prereq buildings.
				for (int iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
				{
					if (kBuilding.getPrereqNumOfBuildingClass(iJ) > 0)
					{
						// I wish this was easier to calculate...
						int iBuilt = kOwner.getBuildingClassCount(eLoopClass);
						int iBuilding = kOwner.getBuildingClassMaking(eLoopClass);
						int iPrereqEach = kOwner.getBuildingClassPrereqBuilding(eLoopBuilding, (BuildingClassTypes)iJ, -iBuilt);
						int iPrereqBuilt = kOwner.getBuildingClassCount((BuildingClassTypes)iJ);
						FAssert(iPrereqEach > 0);
						iLimit = iPrereqBuilt / iPrereqEach - iBuilt - iBuilding;
						// Civ4 Reimagined: This assertion is not always true. E.g. you can lose cities with the required building.
						//FAssert(iLimit > 0);
						break;
					}
				}
			}
			//if (iLimit != -1)
			// Civ4 Reimagined
			if (iLimit > 0)
			{
				const int iMaxNumWonders = (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman()) ? GC.getDefineINT("MAX_NATIONAL_WONDERS_PER_CITY_FOR_OCC") : GC.getDefineINT("MAX_NATIONAL_WONDERS_PER_CITY");

				if (isNationalWonderClass(eLoopClass) && iMaxNumWonders != -1)
				{
					iValue *= iMaxNumWonders + 1 - getNumNationalWonders();
					iValue /= iMaxNumWonders + 1;
				}

				int iLoop;
				for (CvCity* pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
				{
					if (pLoopCity->canConstruct(eLoopBuilding))
					{
						int iLoopValue = pLoopCity->AI_buildingValue(eLoopBuilding, 0, 0, bAsync);
						if (isNationalWonderClass(eLoopClass) && iMaxNumWonders != -1)
						{
							iLoopValue *= iMaxNumWonders + 1 - pLoopCity->getNumNationalWonders();
							iLoopValue /= iMaxNumWonders + 1;
						}
						if (80 * iLoopValue > 100 * iValue)
						{
							if (--iLimit <= 0)
							{
								iValue = 0;
								break;
							}
						}
					}
				}
				// Subtract some points from wonder value, just to stop us from wasting it
				if (isNationalWonderClass(eLoopClass))
					iValue -= 40 + (iMaxNumWonders > 0 ? 60 * getNumNationalWonders() / iMaxNumWonders : 0);
			}
		}
		// K-Mod end

		if (isWorldWonderClass(eLoopClass))
		{
			if (iProductionRank <= std::min(3, ((kOwner.getNumCities() + 6) / 6))) // Civ4 Reimagined
			{
				int iTempValue;
				if (bAsync)
				{
					iTempValue = GC.getASyncRand().get(GC.getLeaderHeadInfo(getPersonalityType()).getWonderConstructRand(), "Wonder Construction Rand ASYNC");
				}
				else
				{
					iTempValue = GC.getGameINLINE().getSorenRandNum(GC.getLeaderHeadInfo(getPersonalityType()).getWonderConstructRand(), "Wonder Construction Rand");
				}

				if (bAreaAlone)
				{
					iTempValue *= 2;
				}
				
				// Civ4 Reimagined
				iTempValue *= 2;
				if (kOwner.AI_isDoStrategy(AI_STRATEGY_SPECIALIST_ECONOMY))
				{
					iTempValue *= 3;
					iTempValue /= 2;
				}
				
				iValue += iTempValue;
			}
		}

		if (bAsync)
		{
			iValue *= (GC.getASyncRand().get(25, "AI Best Building ASYNC") + 100);
			iValue /= 100;
		}
		else
		{
			iValue *= (GC.getGameINLINE().getSorenRandNum(25, "AI Best Building") + 100);
			iValue /= 100;
		}

		//iValue += getBuildingProduction(eLoopBuilding);
		iValue += getBuildingProduction(eLoopBuilding) / 4; // K-Mod

		bool bValid = iMaxTurns <= 0 ? true : false;
		if (!bValid)
		{
			bValid = (iTurnsLeft <= GC.getGameINLINE().AI_turnsPercent(iMaxTurns, GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent()));
		}
		if (!bValid)
		{
			for (int iHurry = 0; iHurry < GC.getNumHurryInfos(); ++iHurry)
			{
				if (canHurryBuilding((HurryTypes)iHurry, eLoopBuilding, true))
				{
					if (AI_getHappyFromHurry((HurryTypes)iHurry, eLoopBuilding, true) > 0)
					{
						bValid = true;
						break;
					}
				}
			}
		}

		if (bValid)
		{
			FAssert((MAX_INT / 1000) > iValue);
			iValue *= 1000;
			iValue /= std::max(1, (iTurnsLeft + 3));

			iValue = std::max(1, iValue);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestBuilding = eLoopBuilding;
			}
		}
	}
	return eBestBuilding;
}


/* original BtS code. (I don't see the point of this function being separate to the "threshold" version)
int CvCityAI::AI_buildingValue(BuildingTypes eBuilding, int iFocusFlags) const
{
	return AI_buildingValueThreshold(eBuilding, iFocusFlags, 0);
} */

// XXX should some of these count cities, buildings, etc. based on teams (because wonders are shared...)
// XXX in general, this function needs to be more sensitive to what makes this city unique (more likely to build airports if there already is a harbor...)
// This function has been heavily edited for K-Mod
// Scale is roughly 4 = 1 commerce / turn
int CvCityAI::AI_buildingValue(BuildingTypes eBuilding, int iFocusFlags, int iThreshold, bool bConstCache, bool bAllowRecursion) const
{
	PROFILE_FUNC();

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	BuildingClassTypes eBuildingClass = (BuildingClassTypes) kBuilding.getBuildingClassType();
	int iLimitedWonderLimit = limitedWonderClassLimit(eBuildingClass);
	bool bIsLimitedWonder = (iLimitedWonderLimit >= 0);
	// K-Mod. This new value, iPriorityFactor, is used to boost the value of productivity buildings without overvaluing productivity.
	// The point is to get the AI to build productiviy buildings quickly, but not if they come with large negative side effects.
	// I may use it for other adjustments in the future.
	int iPriorityFactor = 100;

	// bRemove means that we're evaluating the cost of losing this building rather than adding it.
	// the definition used here is just a kludge because there currently isn't any other to tell the difference.
	// Currently, bRemove is only in a few parts of the evaluation where it is particularly important;
	// for example, bRemove is critical for calculating the lost value of obsoleting walls and castles.
	// There are several sections which could, in the future, be improved using bRemove -
	// but I don't see it as a high priority.
	bool bRemove = getNumBuilding(eBuilding) >= GC.getCITY_MAX_NUM_BUILDINGS();

	// Veto checks: return zero if the building is not suitable.
	// Note: these checks are essentially the same as in the original code, they've just been moved.
	if (iFocusFlags & BUILDINGFOCUS_WORLDWONDER)
	{
		if (!isWorldWonderClass(eBuildingClass) || findBaseYieldRateRank(YIELD_PRODUCTION) <= 3)
		{
			// Note / TODO: the production condition is from the original BtS code.
			// I intend to remove / change that condition in the future.
			return 0;
		}
	}
	
	// Civ4 Reimagined
	if (isWorldWonderClass(eBuildingClass) && findYieldRateRank(YIELD_PRODUCTION) > std::max(3, kOwner.getCurrentEra() - 1))
	{
		if (kBuilding.getBonusFatCross() == NO_BONUS)
		{
			return 0;
		}
	}
	
	if (kBuilding.isCapital())
		return 0;

	for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (kBuilding.getReligionChange(iI) > 0)
		{
			if (!GET_TEAM(getTeam()).hasHolyCity((ReligionTypes)iI))
			{
				return 0;
			}
		}
	}

	// Construction value cache.
	// Note: the WONDEROK and WORLDWONDER flags should not affect the final value - and so cache should not be disabled by those flags.
	const bool bNeutralFlags = (iFocusFlags & ~(BUILDINGFOCUS_WONDEROK | BUILDINGFOCUS_WORLDWONDER)) == 0;
	const bool bUseConstructionValueCache = bNeutralFlags && iThreshold == 0;
	if (bUseConstructionValueCache && m_aiConstructionValue[eBuildingClass] != -1)
	{
		return m_aiConstructionValue[eBuildingClass];
	}
	// K-Mod end

	ReligionTypes eStateReligion = kOwner.getStateReligion();
	
	// Civ4 Reimagined
	int iTotalReligonCount = kOwner.countTotalHasReligion();
	int iStateReligionPopulation = 0;
	int iStateReligionCount = 0;
	if (eStateReligion != NO_RELIGION)
	{
		int iLoop;
		for (CvCity* pLoopCity = kOwner.firstCity(&iLoop); pLoopCity; pLoopCity = kOwner.nextCity(&iLoop))
		{
			iStateReligionPopulation += pLoopCity->isHasReligion(eStateReligion) ? pLoopCity->getPopulation() : 0;
			iStateReligionCount += pLoopCity->isHasReligion(eStateReligion) ? 1 : 0;
		}
	}
	int iStateReligionCities = eStateReligion == NO_RELIGION ? 0 : kOwner.getHasReligionCount(eStateReligion);

	bool bAreaAlone = kOwner.AI_isAreaAlone(area());
	int iHasMetCount = GET_TEAM(getTeam()).getHasMetCivCount(true);

	int iFoodDifference = foodDifference(false, true); // K-Mod note: I've set this to ignore "food is production" so that building value is not distorted by that effect.

	// Reduce reaction to temporary happy/health problems
	// K-Mod
	int iHappinessLevel = happyLevel() - unhappyLevel() + getEspionageHappinessCounter()/2 - getMilitaryHappiness()/2;
	int iHealthLevel = goodHealth() - badHealth() + getEspionageHealthCounter()/2;
	// K-Mod end

	bool bProvidesPower = (kBuilding.isPower() || ((kBuilding.getPowerBonus() != NO_BONUS) && hasBonus((BonusTypes)(kBuilding.getPowerBonus()))) || kBuilding.isAreaCleanPower());

	int iTotalPopulation = kOwner.getTotalPopulation();
	int iNumCities = kOwner.getNumCities();
	int iNumCitiesInArea = area()->getCitiesPerPlayer(getOwnerINLINE());
	int iCitiesTarget = GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities(); // K-Mod

	bool bIsHighProductionCity = (findBaseYieldRateRank(YIELD_PRODUCTION) <= std::max(3, (iNumCities / 2)));

	int iCultureRank = findCommerceRateRank(COMMERCE_CULTURE);
	int iCulturalVictoryNumCultureCities = GC.getGameINLINE().culturalVictoryNumCultureCities();

	bool bFinancialTrouble = GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble();

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/08/10                                jdog5000      */
/*                                                                                              */
/* Victory Strategy AI                                                                          */
/************************************************************************************************/
	bool bCulturalVictory1 = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE1);
	bool bCulturalVictory2 = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2);
	bool bCulturalVictory3 = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3);

	bool bSpaceVictory1 = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_SPACE1);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/		

	bool bCanPopRush = GET_PLAYER(getOwnerINLINE()).canPopRush();
	bool bWarPlan = GET_TEAM(getTeam()).getAnyWarPlanCount(true) > 0; // K-Mod

	int iForeignTradeRoutes = 0;
	int iOverseaTradeRoutes = 0;
	int iNumTradeRoutes = getTradeRoutes();
	for (int iI = 0; iI < iNumTradeRoutes; ++iI)
	{
		CvCity* pTradeCity = getTradeCity(iI);
		if (NULL != pTradeCity)
		{
			// Civ4 Reimagined
			if (pTradeCity->getOwnerINLINE() != getID())
			{
				iForeignTradeRoutes++;
			}
			if (area() != pTradeCity->area() && pTradeCity->area()->getNumTiles() >= 30)
			{
				iOverseaTradeRoutes++;
			}
		}
	}

	const bool bForeignTrade = iForeignTradeRoutes > 0;

	int iValue = 0;

	for (int iPass = 0; iPass < 2; iPass++)
	{
		// K-Mod. This entire block was originally wrapped with the following condition:
		//
		// if ((iFocusFlags == 0) || (iValue > 0) || (iPass == 0))
		//
		// I've moved this condition to the end of the block, and tweaked it for better readability.

		if ((iFocusFlags & BUILDINGFOCUS_DEFENSE) || (iPass > 0))
		{
			// K-Mod. I've merged and modified the code from here and the code from higher up.
			if (!bAreaAlone)
			{
				if (GC.getGameINLINE().getBestLandUnit() == NO_UNIT || !GC.getUnitInfo(GC.getGameINLINE().getBestLandUnit()).isIgnoreBuildingDefense())
				{
					int iTemp = 0;
					// bombard reduction
					int iOldBombardMod = getBuildingBombardDefense() - (bRemove ? kBuilding.getBombardDefenseModifier() : 0);
					iTemp += std::max((bRemove ?0 :kBuilding.getDefenseModifier()) + getBuildingDefense(), getNaturalDefense()) * std::min(kBuilding.getBombardDefenseModifier(), 100-iOldBombardMod) / std::max(80, 8 * (100 - iOldBombardMod));
					// defence bonus
					//iTemp += std::max(0, std::min((bRemove? 0 :kBuilding.getDefenseModifier()) + getBuildingDefense() - getNaturalDefense() - 10, kBuilding.getDefenseModifier())) / 4;
					// Civ4 Reimagined
					iTemp += bRemove? 0 : kBuilding.getDefenseModifier() * 2;
					iValue += iTemp;
				}
			}
			iValue += kBuilding.getAllCityDefenseModifier() * iNumCities / (bWarPlan ? 4 : 5);
			iValue += kBuilding.getAirlift() * 10 + (iNumCitiesInArea < iNumCities && kBuilding.getAirlift() > 0 ? getPopulation()+25 : 0);

			int iAirDefense = -kBuilding.getAirModifier();
			if (iAirDefense > 0)
			{
				int iTemp = iAirDefense;
				iTemp *= std::min(200, 100 * GET_TEAM(getTeam()).AI_getRivalAirPower() / std::max(1, GET_TEAM(getTeam()).AI_getAirPower()+4*iNumCities));
				iTemp /= 200;
				iValue += iTemp;
			}
			

			//iValue += -kBuilding.getNukeModifier() / (GC.getGameINLINE().isNukesValid() && !GC.getGameINLINE().isNoNukes() ? 4 : 40);
			// Civ4 Reimagined
			iValue += -kBuilding.getNukeModifier() * std::max(1, GC.getGameINLINE().countTotalNukeUnits() + 2 * GC.getGameINLINE().getNukesExploded()) / (GC.getGameINLINE().isNukesValid() && !GC.getGameINLINE().isNoNukes() ? 40 : 400);
			// K-Mod end
		}

		if ((iFocusFlags & BUILDINGFOCUS_ESPIONAGE) || (iPass > 0))
		{
			iValue += kBuilding.getEspionageDefenseModifier() / 8;
		}

		if (((iFocusFlags & BUILDINGFOCUS_HAPPY) || (iPass > 0)) && !isNoUnhappiness())
		{
			int iBestHappy = 0;
			for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
			{
				if (canHurryBuilding((HurryTypes)iI, eBuilding, true))
				{
					int iHappyFromHurry = AI_getHappyFromHurry((HurryTypes)iI, eBuilding, true);
					if (iHappyFromHurry > iBestHappy)
					{
						iBestHappy = iHappyFromHurry;
					}
				}
			}
			iValue += iBestHappy * 10;

			if (kBuilding.isNoUnhappiness())
			{
				//iValue += ((iAngryPopulation * 10) + getPopulation());
				// K-Mod
				int iEstGrowth = iFoodDifference + std::max(0, -iHealthLevel+iFoodDifference);
				iValue += std::max(0, (getPopulation() - std::max(0, 2*iHappinessLevel)) * 2 + std::max(0, -iHappinessLevel) * 6 + std::max(0, -iHappinessLevel+iEstGrowth) * 4);
				// K-Mod end
			}
			int iGood, iBad = 0;
			int iBuildingActualHappiness = getAdditionalHappinessByBuilding(eBuilding,iGood,iBad);

			// K-Mod
			int iCitValue = 6 + kOwner.getCurrentEra(); // (estimating citizen value to be 6 + era commerce per turn)
			int iAngerDelta = std::max(0, -(iHappinessLevel+iBuildingActualHappiness)) - std::max(0, -iHappinessLevel);
			// High value for any immediate change in anger.
			//iValue -= iAngerDelta * 4 * iCitValue;
			iValue -= iAngerDelta * 8 * iCitValue; // Civ4 Reimagined: Higher value for unhappiness reduction
			// some extra value if we are still growing (this is a positive change bias)
			if (iAngerDelta < 0 && iFoodDifference > 1)
			{
				iValue -= 10 * iAngerDelta;
			}
			// finally, a little bit of value for happiness which gives us some padding
			int iHappyModifier = 10 * iCitValue / (3 + std::max(0, iHappinessLevel+iBuildingActualHappiness) + std::max(0, iHappinessLevel) + std::max(0, -iHealthLevel));
			iValue += std::max(0, iBuildingActualHappiness) * iHappyModifier;
			// The "iHappinessModifer" is used for some percentage-based happy effects. (note. this not the same magnitude as the original definition.)

			if (iHappinessLevel >= 10)
			{
				iHappyModifier = 1;
			}
			// K-Mod end

			iValue += (-kBuilding.getHurryAngerModifier() * getHurryPercentAnger()) / 100;

			for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
			{
				//iValue += (kBuilding.getCommerceHappiness(iI) * iHappyModifier) / 4;
				iValue += kBuilding.getCommerceHappiness(iI) * iHappyModifier / 50; // K-Mod (note, commercehappiness is already counted by iBuildingActualHappiness)
			}

			int iWarWearinessModifer = kBuilding.getWarWearinessModifier();
			if (iWarWearinessModifer != 0)
			{
				//iValue += (-iWarWearinessModifer * iHappyModifier) / 16;
				iValue += -iWarWearinessModifer * getPopulation() * iHappyModifier / (bWarPlan ? 400 : 1000); // K-Mod (again, the immediate effects of this are already counted)
			}
			
			// Civ4 Reimagined
			if (kBuilding.isNoWarAgainstSameFaithUnhappiness())
			{
				int iCount = 0;
				
				for (int iI = 0; iI < MAX_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iI).getTeam() != getTeam())
						{
							if (!atWar(GET_PLAYER((PlayerTypes)iI).getTeam(), getTeam()))
							{
								if (GET_PLAYER((PlayerTypes)iI).getStateReligion() != NO_RELIGION)
								{
									if (isHasReligion(GET_PLAYER((PlayerTypes)iI).getStateReligion()))
									{
										iCount += GET_PLAYER((PlayerTypes)iI).getHasReligionCount(GET_PLAYER((PlayerTypes)iI).getStateReligion());
									}
								}
							}
						}
					}
				}
				iValue += iCount * getPopulation() * iHappyModifier / (bWarPlan ? 600 : 1500);
			}

			/*iValue += (kBuilding.getAreaHappiness() * (iNumCitiesInArea - 1) * 8);
			iValue += (kBuilding.getGlobalHappiness() * iNumCities * 8);*/
			// K-Mod - just a tweak.. nothing fancy.
			iValue += kBuilding.getAreaHappiness() * (iNumCitiesInArea + iCitiesTarget/3) * iCitValue;
			iValue += kBuilding.getGlobalHappiness() * (iNumCities + iCitiesTarget/2) * iCitValue;
			// K-Mod end

			int iWarWearinessPercentAnger = kOwner.getWarWearinessPercentAnger();
			int iGlobalWarWearinessModifer = kBuilding.getGlobalWarWearinessModifier();
			if (iGlobalWarWearinessModifer != 0)
			{
				/* iValue += (-(((iGlobalWarWearinessModifer * iWarWearinessPercentAnger / 100) / GC.getPERCENT_ANGER_DIVISOR())) * iNumCities);
				iValue += (-iGlobalWarWearinessModifer * iHappyModifier) / 16; */
				iValue += iCitValue * iNumCities * -iGlobalWarWearinessModifer * (iWarWearinessPercentAnger + GC.getPERCENT_ANGER_DIVISOR()/(bWarPlan ? 10 : 20)) / (100 * GC.getPERCENT_ANGER_DIVISOR()); // K-Mod
			}

			for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
			{
				if (kBuilding.getBuildingHappinessChanges(iI) != 0)
				{
					iValue += (kBuilding.getBuildingHappinessChanges(iI) * kOwner.getBuildingClassCount((BuildingClassTypes)iI) * 8);
				}
			}
		}

		//if (((iFocusFlags & BUILDINGFOCUS_HEALTHY) || (iPass > 0)) && !isNoUnhealthyPopulation())
		if ((iFocusFlags & BUILDINGFOCUS_HEALTHY) || iPass > 0) // K-Mod
		{
			int iGood, iBad = 0;
			int iBuildingActualHealth = getAdditionalHealthByBuilding(eBuilding,iGood,iBad);

			// K-Mod. I've essentially rewritten this evaluation of health; but there may still be some bbai code / original bts code.
			int iCitValue = 6 + kOwner.getCurrentEra(); // (estimating citizen value to be 6 + era commerce per turn)
			int iWasteDelta = std::max(0, -(iHealthLevel+iBuildingActualHealth)) - std::max(0, -iHealthLevel);
			// High value for change in our food deficit.
			// Changed by Civ4 Reimagined
			iValue -= 3 * iCitValue * (std::max(0, -(iFoodDifference - iWasteDelta)) - std::max(0, -iFoodDifference));
			// medium value for change in waste
			iValue -= iCitValue * iWasteDelta;
			// some extra value if the change will help us grow (this is a positive change bias)
			if (iWasteDelta < 0 && iHappinessLevel > 0)
			{
				iValue -= iCitValue * iWasteDelta;
			}
			// finally, a little bit of value for health which gives us some padding
			//iValue += 10 * iCitValue * std::max(0, iBuildingActualHealth)/(6 + std::max(0, iHealthLevel+iBuildingActualHealth) + std::max(0, iHealthLevel));
			iValue += 10 * iCitValue * std::max(0, iBuildingActualHealth)/(3 + std::max(0, iHealthLevel+iBuildingActualHealth) + std::max(0, iHealthLevel)); //Civ4 Reimagined

			// If the GW threshold has been reached,
			// add some additional value for pollution reduction
			// Note. health benefits have already been evaluated
			if (iBad < 0 && GC.getGameINLINE().getGlobalWarmingIndex() > 0)
			{
				int iCleanValue = -2*iBad;

				iCleanValue *= (100 + 5*kOwner.getGwPercentAnger());
				iCleanValue /= 100;

				FAssert(iCleanValue >= 0);

				iValue += iCleanValue;
			}
			// K-Mod end

			iValue += kBuilding.getAreaHealth() * (iNumCitiesInArea-1) * 4;
			iValue += kBuilding.getGlobalHealth() * iNumCities * 4;
		}

		if (iFocusFlags & BUILDINGFOCUS_EXPERIENCE || iPass > 0)
		{
			// K-Mod.
			// Note. currently this new code matches the functionality of the old (deleted) code exactly,
			// except that the value is reduced in cities that we don't expect to be building troops in.
			int iWeight = 12;
			iWeight /= iHasMetCount > 0 ? 1 : 2;
			iWeight /= bWarPlan || bIsHighProductionCity ? 1 : 3;

			iValue += kBuilding.getFreeExperience() * iWeight;

			for (int iI = 0; iI < GC.getNumUnitCombatInfos(); iI++)
			{
				if (canTrain((UnitCombatTypes)iI))
				{
					iValue += kBuilding.getUnitCombatFreeExperience(iI) * iWeight / 2;
				}
			}

			for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
			{
				int iDomainExpValue = 0;
				switch (iI)
				{
				case DOMAIN_LAND:
					// full value
					iValue += kBuilding.getDomainFreeExperience(iI) * iWeight;
					break;
				case DOMAIN_SEA:
					// special case. Don't use 'iWeight', because for sea bIsHighProductionCity may be too strict.
					iValue += kBuilding.getDomainFreeExperience(iI) * 7;
					break;
				default:
					// half value.
					iValue += kBuilding.getDomainFreeExperience(iI) * iWeight/2;
					break;
				}
			}
			// K-Mod end
		}

		// since this duplicates BUILDINGFOCUS_EXPERIENCE checks, do not repeat on pass 1
		if ((iFocusFlags & BUILDINGFOCUS_DOMAINSEA))
		{
			iValue += (kBuilding.getFreeExperience() * ((iHasMetCount > 0) ? 16 : 8));

			for (int iUnitIndex = 0; iUnitIndex < GC.getNumUnitClassInfos(); iUnitIndex++)
			{
				UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iUnitIndex);

				if (NO_UNIT != eUnit)
				{
					CvUnitInfo& kUnitInfo = GC.getUnitInfo(eUnit);
					int iCombatType = kUnitInfo.getUnitCombatType();
					if (kUnitInfo.getDomainType() == DOMAIN_SEA && canTrain(eUnit) && iCombatType != NO_UNITCOMBAT)
					{
						iValue += (kBuilding.getUnitCombatFreeExperience(iCombatType) * ((iHasMetCount > 0) ? 6 : 3));
					}
				}
			}

			iValue += (kBuilding.getDomainFreeExperience(DOMAIN_SEA) * ((iHasMetCount > 0) ? 16 : 8));

			iValue += (kBuilding.getDomainProductionModifier(DOMAIN_SEA) / 4);
		}

		if ((iFocusFlags & BUILDINGFOCUS_MAINTENANCE) || (iFocusFlags & BUILDINGFOCUS_GOLD) || (iPass > 0))
		{
			// K-Mod, bugfix. getMaintenanceTimes100 is the total, with modifiers already applied.
			// (note. ideally we'd use "calculateBaseMaintenanceTimes100", and that would a avoid problem caused by "we love the X day".
			// but doing it this way is slightly faster.)
			if (kBuilding.getMaintenanceModifier())
			{
				int iExistingUpkeep = getMaintenanceTimes100();
				int iBaseMaintenance = 100 * iExistingUpkeep / std::max(1, 100 + getMaintenanceModifier());
				int iNewUpkeep = (iBaseMaintenance * std::max(0, 100 + getMaintenanceModifier() + kBuilding.getMaintenanceModifier())) / 100;

				int iTempValue = (iExistingUpkeep - iNewUpkeep) / 22; // slightly more then 4x savings, just to acomodate growth.

				iTempValue = iTempValue * (100+kOwner.getInflationRate()) / 100; // We want absolute savings, including inflation.

				/* iTempValue *= kOwner.AI_commerceWeight(COMMERCE_GOLD, 0); // (note, not just for this particular city - because this isn't direct gold production)
				iTempValue /= 100; */

				if (bFinancialTrouble)
					iTempValue *= 2;

				// Civ4 Reimagined: Expect colony maintenance to rise
				if (isColony())
				{
					iTempValue *= 3;
					iTempValue /= 2;
				}

				iValue += iTempValue;
			}
			// K-Mod end
		}

		if ((iFocusFlags & BUILDINGFOCUS_SPECIALIST) || (iPass > 0))
		{
			// K-Mod. (original code deleted)
			int iSpecialistsValue = 0;
			//int iUnusedSpecialists = 0;
			SpecialistTypes eDefaultSpecialist = (SpecialistTypes)GC.getDefineINT("DEFAULT_SPECIALIST");
			int iAvailableWorkers = iFoodDifference/2 + (eDefaultSpecialist == NO_SPECIALIST ? 0 : getSpecialistCount(eDefaultSpecialist));
			for (SpecialistTypes eLoopSpec = (SpecialistTypes)0; eLoopSpec < GC.getNumSpecialistInfos(); eLoopSpec = (SpecialistTypes)(eLoopSpec+1))
			{
				if (eLoopSpec == eDefaultSpecialist)
					continue;

				// Civ4 Reimagined
				int iLimit = std::max(0, getMaxSpecialistCount(eLoopSpec) - getSpecialistCount(eLoopSpec));

				if (kBuilding.getSpecialistCount(eLoopSpec) > 0 && iLimit <= 2)
				{
					int iTempValue = AI_specialistValue(eLoopSpec, false, false);

					//iTempValue *= (iLimit == 0 ? 60 : 0) + 40 * std::min(iAvailableWorkers, kBuilding.getSpecialistCount(eLoopSpec));
					iTempValue *= (iLimit == 0 ? 90 : 0) + 40 * std::min(iAvailableWorkers, kBuilding.getSpecialistCount(eLoopSpec)); // Civ4 Reimagined
					// I'm choosing not to reduce 'iAvailableWorkers'... It's a tough call. Either way, the answer is going to be wrong!
					iTempValue /= 100 + 200 * iLimit;

					iSpecialistsValue += iTempValue / 100;
				}
			}

			if (iSpecialistsValue > 0)
			{
				iValue += iSpecialistsValue;
			}
			// K-Mod end
		}

		if ((iFocusFlags & (BUILDINGFOCUS_GOLD | BUILDINGFOCUS_RESEARCH)) || iPass > 0)
		{
			// trade routes
			// K-Mod. (original code deleted)
			int iTotalTradeModifier = totalTradeModifier();
			int iTempValue = kBuilding.getTradeRoutes() * (getTradeRoutes() > 0 ? 5*getTradeYield(YIELD_COMMERCE) / getTradeRoutes() : 5 * (getPopulation() / 5 + 1) * iTotalTradeModifier / 100);
			//int iGlobalTradeValue = (6 * iTotalPopulation / (5 * iNumCities) + 1) * kOwner.AI_averageYieldMultiplier(YIELD_COMMERCE) / 100;
			// 1.2 * average population seems wrong. Instead, do something roughly compariable to what's used in CvPlayerAI::AI_civicValue.
			int iGlobalTradeValue = (bForeignTrade ? 5 : 3) * (2*(kOwner.getCurrentEra()+1) + GC.getNumEraInfos()) / GC.getNumEraInfos();

			iTempValue += 5 * kBuilding.getTradeRouteModifier() * getTradeYield(YIELD_COMMERCE) / std::max(1, iTotalTradeModifier);
				
			if (bForeignTrade)
			{
				iTempValue += 5 * iForeignTradeRoutes * kBuilding.getForeignTradeRouteModifier() * getTradeYield(YIELD_COMMERCE) / std::max(1, iTotalTradeModifier) / iNumTradeRoutes;
			}

			iTempValue += iTempValue += 5 * iOverseaTradeRoutes * kBuilding.getOverseaTradeRouteModifier() * getTradeYield(YIELD_COMMERCE) / std::max(1, iTotalTradeModifier) / iNumTradeRoutes;

			iTempValue *= AI_yieldMultiplier(YIELD_COMMERCE);
			iTempValue /= 100;

			// Civ4 Reimagined: Added AverageYieldMultiplier
			iTempValue += kBuilding.getCoastalTradeRoutes() * std::max((iCitiesTarget+1)/2, kOwner.countNumCoastalCities()) * iGlobalTradeValue * kOwner.AI_averageYieldMultiplier(YIELD_COMMERCE) / 100;
			iTempValue += kBuilding.getGlobalTradeRoutes() * std::max(iCitiesTarget, iNumCities) * iGlobalTradeValue * kOwner.AI_averageYieldMultiplier(YIELD_COMMERCE) / 100;
			// K-Mod end

			if (bFinancialTrouble)
			{
				iTempValue *= 2;
			}

			if (kOwner.isNoForeignTrade())
			{
				iTempValue /= 2; // was /3
			}

			iValue += iTempValue;
		}

		if (iPass > 0)
		{
			// K-Mod. The value of golden age buildings. (This was not counted by the original AI.)
			{
				int iGoldenPercent = kBuilding.isGoldenAge() ? 100 : 0;

				if (kBuilding.getGoldenAgeModifier() != 0)
				{
					iGoldenPercent *= kBuilding.getGoldenAgeModifier();
					iGoldenPercent /= 100;
					// It's difficult to estimate the value of the golden age modifier.
					// Firstly, we don't know how many golden ages we are going to have; but that's a relatively minor problem. We can just guess that.
					// A bigger problem is that the value of a golden age can change a lot depending on the state of the civilzation.
					// The upshot is that the value here is going to be rough...
					iGoldenPercent += 3 * kBuilding.getGoldenAgeModifier() * (GC.getNumEraInfos() - kOwner.getCurrentEra()) / (GC.getNumEraInfos() + 1);
				}
				if (iGoldenPercent > 0)
				{
					// note, the value returned by AI_calculateGoldenAgeValue is roughly in units of commerce points;
					// whereas, iValue in this function is roughly in units of 4 * commerce / turn.
					// I'm just going to say 44 points of golden age commerce is roughly worth 1 commerce per turn. (so conversion is 4/44)
					iValue += kOwner.AI_calculateGoldenAgeValue(false) * iGoldenPercent / (100 * 11);
				}
			}
			// K-Mod end

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      02/24/10                       jdog5000 & Afforess    */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
			if (kBuilding.isAreaCleanPower() && !(area()->isCleanPower(getTeam())))
			{
				int iLoop;
				for( CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop) )
				{
					if( pLoopCity->area() == area() )
					{
						if( pLoopCity->isDirtyPower() )
						{
							iValue += 12;
						}
						else if( !(pLoopCity->isPower()) )
						{
							//iValue += 8;
							iValue += 24; // K-Mod. Giving power should be more valuable than replacing existing power!
						}
					}
				}
			}

			if (kBuilding.getDomesticGreatGeneralRateModifier() != 0)
			{
				iValue += (kBuilding.getDomesticGreatGeneralRateModifier() / 10);
			}

			if (kBuilding.isAreaBorderObstacle() && !(area()->isBorderObstacle(getTeam())))
			{
				if( !GC.getGameINLINE().isOption(GAMEOPTION_NO_BARBARIANS) )
				{
					// Civ4 Reimagined
					iValue += (iNumCitiesInArea * 4) / std::max(1, (int)kOwner.getCurrentEra());

					if(GC.getGameINLINE().isOption(GAMEOPTION_RAGING_BARBARIANS))
					{
						// Civ4 Reimagined
						iValue += (iNumCitiesInArea * 4) / std::max(1, (int)kOwner.getCurrentEra());
					}
				}
			}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

			if (kBuilding.isGovernmentCenter())
			{
				FAssert(!(kBuilding.isCapital()));
				/* original bts code
				iValue += ((calculateDistanceMaintenance() - 3) * iNumCitiesInArea); */
				// K-mod. More bonus for colonies, because it reduces that extra maintenance too.
				int iTempValue = 2*(calculateDistanceMaintenance() - 2) * iNumCitiesInArea;
				const CvCity* pCapitalCity = kOwner.getCapitalCity();
				if (pCapitalCity == NULL || pCapitalCity->area() != area())
				{
					iTempValue *= 2;
				}
				iValue += iTempValue;
				// K-Mod end
			}

			// Civ4 Reimagined
			if (kBuilding.getAreaDistanceMaintenanceModifier() != 0)
			{
				iValue += iNumCitiesInArea * (-kBuilding.getAreaDistanceMaintenanceModifier()) / 4;
			}

			// Civ4 Reimagined
			if (kBuilding.getAreaCorporationMaintenanceModifier() != 0)
			{
				iValue += iNumCitiesInArea / iNumCities * kOwner.countTotalHasCorporation() * (-kBuilding.getAreaCorporationMaintenanceModifier()) / 2;
			}

			if (kBuilding.isMapCentering())
			{
				iValue++;
			}

			if (kBuilding.getFreeBonus() != NO_BONUS)
			{
				iValue += (kOwner.AI_bonusVal((BonusTypes)(kBuilding.getFreeBonus()), 1) *
					((kOwner.getNumTradeableBonuses((BonusTypes)(kBuilding.getFreeBonus())) == 0) ? 2 : 1) *
					(iNumCities + kBuilding.getNumFreeBonuses()));
			}

			if (kBuilding.getNoBonus() != NO_BONUS)
			{
				//iValue -= kOwner.AI_bonusVal((BonusTypes)kBuilding.getNoBonus());
				iValue -= kOwner.AI_bonusVal((BonusTypes)kBuilding.getNoBonus(), 0); // K-Mod
			}

			if (kBuilding.getFreePromotion() != NO_PROMOTION)
			{
				/* original bts code
				iValue += ((iHasMetCount > 0) ? 100 : 40); // XXX some sort of promotion value??? */
				// K-Mod.
				// Ideally, we'd use AI_promotionValue to work out what the promotion is worth
				// but unfortunately, that function requires a target unit, and I can't think of a good
				// way to choose a suitable unit for evaluation.
				// So.. I'm just going to do a really basic kludge to stop the Dun from being worth more than Red Cross
				const CvPromotionInfo& kInfo = GC.getPromotionInfo((PromotionTypes)kBuilding.getFreePromotion());
				bool bAdvanced = kInfo.getPrereqPromotion() != NO_PROMOTION ||
					kInfo.getPrereqOrPromotion1() != NO_PROMOTION || kInfo.getPrereqOrPromotion2() != NO_PROMOTION || kInfo.getPrereqOrPromotion3() != NO_PROMOTION;
				int iTemp = (bAdvanced ? 200 : 40);
				int iProduction = getYieldRate(YIELD_PRODUCTION);
				iTemp *= 2*iProduction;
				iTemp /= 30 + iProduction;
				iTemp *= getFreeExperience() + 1;
				iTemp /= getFreeExperience() + 2;
				iValue += iTemp;
				// cf. iValue += (kBuilding.getFreeExperience() * ((iHasMetCount > 0) ? 12 : 6));
				// K-Mod end
			}

			if (kBuilding.getCivicOption() != NO_CIVICOPTION)
			{
				for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
				{
					if (GC.getCivicInfo((CivicTypes)iI).getCivicOptionType() == kBuilding.getCivicOption())
					{
						if (!(kOwner.canDoCivics((CivicTypes)iI)))
						{
							//iValue += (kOwner.AI_civicValue((CivicTypes)iI) / 10);
							iValue += (kOwner.AI_civicValue((CivicTypes)iI) / 5); // Civ4 Reimagined
						}
					}
				}
			}
			
			// Civ4 Reimagined
			int iGreatPeopleRateModifier = kOwner.isNoGreatPeople() ? 0 : kBuilding.getGreatPeopleRateModifier();
			
			if (iGreatPeopleRateModifier > 0)
			{
				int iGreatPeopleRate = getBaseGreatPeopleRate();
				const int kTargetGPRate = 10;
				
				// Civ4 Reimagined
				if (kOwner.AI_isDoStrategy(AI_STRATEGY_SPECIALIST_ECONOMY))
				{
					iGreatPeopleRateModifier *= 3;
					iGreatPeopleRateModifier /= 2;
				}

				// either not a wonder, or a wonder and our GP rate is at least the target rate
				if (!bIsLimitedWonder || iGreatPeopleRate >= kTargetGPRate)
				{
					iValue += ((iGreatPeopleRateModifier * iGreatPeopleRate) / 16);
				}
				// otherwise, this is a limited wonder (aka National Epic), we _really_ do not want to build this here
				// subtract from the value (if this wonder has a lot of other stuff, we still might build it)
				else
				{
					iValue -= ((iGreatPeopleRateModifier * (kTargetGPRate - iGreatPeopleRate)) / 12);
				}
			}

			if (!kOwner.isNoGreatPeople()) //Civ4 Reimagined
				iValue += ((kBuilding.getGlobalGreatPeopleRateModifier() * iNumCities) / (kOwner.AI_isDoStrategy(AI_STRATEGY_SPECIALIST_ECONOMY) ? 5 : 8));

			// Civ4 Reimagined
			if (GET_PLAYER(getOwnerINLINE()).getMaxAnarchyTurns() > 0)
			{
				iValue += (-kBuilding.getAnarchyModifier()) * std::max(3, (GC.getNumEraInfos() - kOwner.getCurrentEra())) / (GC.getNumEraInfos() + 1);
				if (kBuilding.getNoAnarchyCivicOption() != NO_CIVICOPTION && !GET_PLAYER(getOwnerINLINE()).isHasNoAnarchyCivicOption((CivicOptionTypes)kBuilding.getNoAnarchyCivicOption()))
				{
					iValue += std::max(15, 30 * (GC.getNumEraInfos() - kOwner.getCurrentEra()) / (GC.getNumEraInfos() + 1));
				}
			}

			iValue += (-(kBuilding.getGlobalHurryModifier()) * 2);

			iValue += (kBuilding.getGlobalFreeExperience() * iNumCities * ((iHasMetCount > 0) ? 6 : 3));
			
			// Civ4 Reimagined
			iValue += (kBuilding.getStateReligionFreeExperience() * iStateReligionCities * ((iHasMetCount > 0) ? 6 : 3));
			
			// Civ4 Reimagined
			if (kBuilding.getReligionSpreadBoost() > 0)
			{
				if (eStateReligion != NO_RELIGION && GET_TEAM(getTeam()).hasHolyCity(eStateReligion))
				{
					CvCity* pHolyCity = GC.getGame().getHolyCity(eStateReligion);
					iValue += (pHolyCity->hasShrine(eStateReligion) ? 4 : 1) * kBuilding.getReligionSpreadBoost() * GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities();
				}
			}
			
			// Civ4 Reimagined
			iValue += kBuilding.getLootingModifier() * (bFinancialTrouble ? 2 : 1) * ((iHasMetCount > 0) ? 2 : 1) / (GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE) ? 20 : 10);

			// Civ4 Reimagined
			if (kBuilding.isUnlimitedBarbXP())
			{
				if( !GC.getGameINLINE().isOption(GAMEOPTION_NO_BARBARIANS) )
				{
					iValue += (iNumCities * 2) / std::max(1, (int)kOwner.getCurrentEra());

					if(GC.getGameINLINE().isOption(GAMEOPTION_RAGING_BARBARIANS))
					{
						iValue += iNumCities / std::max(1, (int)kOwner.getCurrentEra());
					}
				}
			}
			
			// Civ4 Reimagined
			if (kBuilding.isBarbarianGreatGeneral())
			{
				if( !GC.getGameINLINE().isOption(GAMEOPTION_NO_BARBARIANS) )
				{
					iValue += (iNumCities * 2) / std::max(1, (int)kOwner.getCurrentEra());

					if(GC.getGameINLINE().isOption(GAMEOPTION_RAGING_BARBARIANS))
					{
						iValue += iNumCities / std::max(1, (int)kOwner.getCurrentEra());
					}
				}
			}
			
			// Civ4 Reimagined
			if (kBuilding.isNoNonStateReligionSpread() && !kOwner.isNoNonStateReligionSpread())
			{
				if (bCulturalVictory2 || eStateReligion == NO_RELIGION)
				{
					iValue -= 3 * std::max(0, iNumCities + iStateReligionCities - iTotalReligonCount);
				}
				else if (eStateReligion == GC.getLeaderHeadInfo(kOwner.getLeaderType()).getFavoriteReligion() ||
					(kOwner.hasHolyCity(eStateReligion) && kOwner.countHolyCities() == 1))
				{
					iValue += iNumCities * 2; // protection vs espionage? depriving foreign civs of religion revenue?
				}

				int iNonStateReligionHappiness = kOwner.getNonStateReligionHappiness();
				iValue += -iNonStateReligionHappiness * iNumCities;
			}
			
			/* original bts code. (moved to where the rest of foodKept is valued)
			if (bCanPopRush)
			{
				iValue += kBuilding.getFoodKept() / 2;
			} */

			/* original bts code. (This stuff is already counted in the defense section.)
			iValue += kBuilding.getAirlift() * (getPopulation()*3 + 10);

			int iAirDefense = -kBuilding.getAirModifier();
			if (iAirDefense > 0)
			{
				if (((kOwner.AI_totalUnitAIs(UNITAI_DEFENSE_AIR) > 0) && (kOwner.AI_totalUnitAIs(UNITAI_ATTACK_AIR) > 0)) || (kOwner.AI_totalUnitAIs(UNITAI_MISSILE_AIR) > 0))
				{
					iValue += iAirDefense / ((iHasMetCount > 0) ? 2 : 4);
				}
			}

			iValue += kBuilding.getAirUnitCapacity() * (getPopulation() * 2 + 10);

			iValue += (-(kBuilding.getNukeModifier()) / ((iHasMetCount > 0) ? 10 : 20)); */

			iValue += std::max(0, kBuilding.getAirUnitCapacity() - plot()->airUnitSpaceAvailable(getTeam())/2) * (getPopulation() + 12); // K-Mod

			/* original bts code
			iValue += (kBuilding.getFreeSpecialist() * 16);
			iValue += (kBuilding.getAreaFreeSpecialist() * iNumCitiesInArea * 12);
			iValue += (kBuilding.getGlobalFreeSpecialist() * iNumCities * 12); */
			// K-Mod. Still very rough, but a bit closer to true value.  (this is for the Statue of Liberty)
			{
				int iFreeSpecialists = kBuilding.getFreeSpecialist() + kBuilding.getAreaFreeSpecialist() * iNumCitiesInArea + kBuilding.getGlobalFreeSpecialist() * iNumCities;
				if (iFreeSpecialists > 0)
				{
					int iSpecialistValue = 20 * 100; // rough base value
					// additional bonuses
					for (CommerceTypes i = (CommerceTypes)0; i < NUM_COMMERCE_TYPES; i = (CommerceTypes)(i+1))
					{
						if (kOwner.getSpecialistExtraCommerce(i))
							iSpecialistValue += kOwner.getSpecialistExtraCommerce(i) * 4 * kOwner.AI_commerceWeight(i);
					}
					iSpecialistValue += 8*std::max(0, kOwner.AI_averageGreatPeopleMultiplier()-100);

					iValue += iFreeSpecialists * iSpecialistValue / 100;
				}
			}
			// K-Mod end

			iValue += ((kBuilding.getWorkerSpeedModifier() * kOwner.AI_getNumAIUnits(UNITAI_WORKER)) / 10);

			int iMilitaryProductionModifier = kBuilding.getMilitaryProductionModifier();
			
			// Civ4 Reimagined
			for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
			{
				if (hasBonus((BonusTypes)iJ))
				{
					iMilitaryProductionModifier += kBuilding.getBonusUnitProductionModifier(iJ);
				}
			}
			
			if (iHasMetCount > 0 && iMilitaryProductionModifier > 0)
			{
				// either not a wonder, or a wonder and we are a high production city
				if (!bIsLimitedWonder || bIsHighProductionCity)
				{
					iValue += (iMilitaryProductionModifier / 4);

					// if a wonder, then pick one of the best cities
					if (bIsLimitedWonder)
					{
						// if one of the top 3 production cities, give a big boost
						if (findBaseYieldRateRank(YIELD_PRODUCTION) <= (2 + iLimitedWonderLimit))
						{
							iValue += (2 * iMilitaryProductionModifier) / (2 + findBaseYieldRateRank(YIELD_PRODUCTION));
						}
					}
					// otherwise, any of the top half of cities will do
					else if (bIsHighProductionCity)
					{
						iValue += iMilitaryProductionModifier / 4;
					}
					iValue += ((iMilitaryProductionModifier * (getFreeExperience() + getSpecialistFreeExperience())) / 10);
				}
				// otherwise, this is a limited wonder (aka Heroic Epic), we _really_ do not want to build this here
				// subtract from the value (if this wonder has a lot of other stuff, we still might build it)
				else
				{
					iValue -= (iMilitaryProductionModifier * findBaseYieldRateRank(YIELD_PRODUCTION)) / 5;
				}
			}

			// Civ4 Reimagined
			if (kBuilding.getNuclearProductionModifier() != 0)
			{
				const BuildingTypes eSilo = (BuildingTypes)GC.getInfoTypeForString("BUILDING_MISSILE_SILO");
				if (getNumRealBuilding(eSilo) > 0)
				{
					iValue += kBuilding.getNuclearProductionModifier() / 8;
				}
			}

			iValue += (kBuilding.getSpaceProductionModifier() / 5);
			iValue += ((kBuilding.getGlobalSpaceProductionModifier() * iNumCities) / 20);


			if (kBuilding.getGreatPeopleUnitClass() != NO_UNITCLASS)
			{
				iValue++; // XXX improve this for diversity...
			}

			// prefer to build great people buildings in places that already have some GP points
			//iValue += (kBuilding.getGreatPeopleRateChange() * 10) * (1 + (getBaseGreatPeopleRate() / 2));
			// K-Mod... here's some code like the specialist value function. Kind of long, but much better.
			
			// Civ4 Reimagined
			if (!kOwner.isNoGreatPeople())
			{
				int iTempValue = 100 * kBuilding.getGreatPeopleRateChange() * 2 * 4; // everything seems to be x4 around here
				int iCityRate = getGreatPeopleRate();
				int iHighestRate = 0;
				int iLoop;
				for( CvCity* pLoopCity = GET_PLAYER(getOwnerINLINE()).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(getOwnerINLINE()).nextCity(&iLoop) )
				{
					int x = pLoopCity->getGreatPeopleRate();
					if (x > iHighestRate)
						iHighestRate = x;
				}
				if (iHighestRate > iCityRate)
				{
					iTempValue *= 100;
					iTempValue /= (2*100*(iHighestRate+3))/(iCityRate+3) - 100;
				}

				iTempValue *= getTotalGreatPeopleRateModifier();
				iTempValue /= 100;
				
				// Civ4 Reimagined
				if (kOwner.AI_isDoStrategy(AI_STRATEGY_SPECIALIST_ECONOMY))
				{
					iTempValue *= 3;
					iTempValue /= 2;
				}

				iValue += iTempValue / 100;
			}
			// K-Mod end

			if (!bAreaAlone)
			{
				iValue += (kBuilding.getHealRateChange() / 2);
			}

			iValue += (kBuilding.getGlobalPopulationChange() * iNumCities * 4);

			//iValue += (kBuilding.getFreeTechs() * 80);
			// K-Mod. A slightly more nuanced evaluation of free techs (but still very rough)
			if (kBuilding.getFreeTechs() > 0)
			{
				int iTotalTechValue = 0;
				int iMaxTechValue = 0;
				int iTechCount = 0;

				for (TechTypes iI = (TechTypes)0; iI < GC.getNumTechInfos(); iI=(TechTypes)(iI+1))
				{
					if (kOwner.canResearch(iI, false, true))
					{
						int iTechValue = GET_TEAM(getTeam()).getResearchCost(iI);
						iTotalTechValue += iTechValue;
						iTechCount++;
						iMaxTechValue = std::max(iMaxTechValue, iTechValue);
					}
				}
				if (iTechCount > 0)
				{
					int iTechValue =  ((iTotalTechValue / iTechCount) + iMaxTechValue)/2;

					// It's hard to measure an instant boost with units of commerce per turn... So I'm just going to divide it by 10.
					iValue += iTechValue * 10 / GC.getGameSpeedInfo(GC.getGame().getGameSpeedType()).getResearchPercent();
				}
				// else: If there is nothing to research, a free tech is worthless.
			}

			// Civ4 Reimagined
			iValue += kBuilding.getEnemyWarWearinessModifier() / (GC.getGameINLINE().getCurrentEra() > 2 ? 2 : 5);
			
			// Civ4 Reimagined
			if (kBuilding.isNoConquestResistance())
			{
				iValue += bWarPlan ? 50 : 15;
			}
			
			// Civ4 Reimagined
			if (kBuilding.isAllowsNukes())
			{
				iValue += 200;
			}

			for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
			{
				if (kBuilding.getFreeSpecialistCount(iI) > 0)
				{
					//iValue += ((AI_specialistValue(((SpecialistTypes)iI), false, false) * kBuilding.getFreeSpecialistCount(iI)) / 50);
					// K-Mod
					iValue += AI_permanentSpecialistValue((SpecialistTypes)iI) * kBuilding.getFreeSpecialistCount(iI) / 100;
				}
			}

			for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
			{
				if (kBuilding.getImprovementFreeSpecialist(iI) > 0)
				{
					iValue += kBuilding.getImprovementFreeSpecialist(iI) * countNumImprovedPlots((ImprovementTypes)iI, true) * 50;
				}
			}

			for (int iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
			{
				iValue += (kBuilding.getDomainProductionModifier(iI) / 5);

				if (bIsHighProductionCity)
				{
					iValue += (kBuilding.getDomainProductionModifier(iI) / 5);
				}
			}

			for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
			{
				if (GC.getUnitInfo((UnitTypes)iI).getPrereqBuilding() == eBuilding)
				{
					iValue += GET_PLAYER(getOwnerINLINE()).AI_unitValue((UnitTypes)iI, (UnitAITypes)GC.getUnitInfo((UnitTypes)iI).getDefaultUnitAIType(), area()) / 7;

					if (kOwner.AI_totalAreaUnitAIs(area(), ((UnitAITypes)(GC.getUnitInfo((UnitTypes)iI).getDefaultUnitAIType()))) == 0)
					{
						iValue *= 2;
					}

					ReligionTypes eReligion = (ReligionTypes)(GC.getUnitInfo((UnitTypes)iI).getPrereqReligion());
					if (eReligion != NO_RELIGION)
					{
						//encouragement to get some minimal ability to train special units
						if (bCulturalVictory1 || isHolyCity(eReligion) || isCapital())
						{
							iValue += (2 + iNumCitiesInArea);
						}

						if (bCulturalVictory2 && GC.getUnitInfo((UnitTypes)iI).getReligionSpreads(eReligion))
						{
							//this gives a very large extra value if the religion is (nearly) unique
							//to no extra value for a fully spread religion.
							//I'm torn between huge boost and enough to bias towards the best monastery type.
							int iReligionCount = GET_PLAYER(getOwnerINLINE()).getHasReligionCount(eReligion);
							iValue += (100 * (iNumCities - iReligionCount)) / (iNumCities * (iReligionCount + 1));
						}
					}
				}
			}

			// is this building needed to build other buildings?

			// K-Mod
			// (I've deleted the original code for this section.)
			if (bAllowRecursion)
			{
				// (moved from AI_bestBuildingThreshold)
				if (kBuilding.getFreeBuildingClass() != NO_BUILDINGCLASS)
				{
					BuildingTypes eFreeBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(kBuilding.getFreeBuildingClass());
					if (NO_BUILDING != eFreeBuilding)
					{
						// K-Mod note: this is actually a pretty poor approximation, because the value of the free building is likely to be different in the other cities.
						// also, if the free building is very powerful, then our other cities will probably build it themselves before they get the freebie! (that's why I reduce the city count below)
						int iFreeBuildingValue = std::min(AI_buildingValue(eFreeBuilding, 0, 0, bConstCache, false), kOwner.getProductionNeeded(eFreeBuilding)/2);
						iValue += iFreeBuildingValue * (std::max(iCitiesTarget, kOwner.getNumCities()*2/3) - kOwner.getBuildingClassCountPlusMaking((BuildingClassTypes)kBuilding.getFreeBuildingClass()));
					}
				}

				// Civ4 Reimagined
				if (kBuilding.getAreaFreeBuildingClass() != NO_BUILDINGCLASS)
				{
					BuildingTypes eFreeBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(kBuilding.getAreaFreeBuildingClass());
					if (NO_BUILDING != eFreeBuilding)
					{
						int iFreeBuildingValue = std::min(AI_buildingValue(eFreeBuilding, 0, 0, bConstCache, false), kOwner.getProductionNeeded(eFreeBuilding)/2);
						iFreeBuildingValue *= iNumCitiesInArea;
						iFreeBuildingValue /= kOwner.getNumCities();
						iValue += iFreeBuildingValue * (std::max(iCitiesTarget, kOwner.getNumCities()*2/3) - kOwner.getBuildingClassCountPlusMaking((BuildingClassTypes)kBuilding.getAreaFreeBuildingClass()));
					}
				}

				//
				for (BuildingClassTypes eLoopClass = (BuildingClassTypes)0; eLoopClass < GC.getNumBuildingClassInfos(); eLoopClass = (BuildingClassTypes)(eLoopClass+1))
				{
					BuildingTypes eLoopBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eLoopClass); // K-Mod

					if (eLoopBuilding == NO_BUILDING)
						continue;

					int iPrereqBuildings = 0; // number of eBuilding required to build eLoopBuilding
					const CvBuildingInfo& kLoopBuilding = GC.getBuildingInfo(eLoopBuilding);
					int iLimitForLoopBuilding = limitedWonderClassLimit(eLoopClass);

					if ((kLoopBuilding.getPrereqNumOfBuildingClass(eBuildingClass) <= 0 && !kLoopBuilding.isBuildingClassNeededInCity(kBuilding.getBuildingClassType())) ||
						(iLimitForLoopBuilding > 0 && kOwner.getBuildingClassMaking(eLoopClass) >= iLimitForLoopBuilding) ||
						!kOwner.canConstruct(eLoopBuilding, false, true, false))
					{
						// either we don't need eBuilding in order to build eLoopBuilding, or we can't construct eLoopBuilding anyway
						// NOTE: the above call to canConstruct will return true even if the city already has the maximum number of national wonders. This is a minor flaw in the AI.
						continue;
					}

					if (kLoopBuilding.getPrereqNumOfBuildingClass(eBuildingClass) > 0)
					{
						// calculate how many more of eBuilding we actually need, given that we might be constructing some eLoopBuilding already.
						iPrereqBuildings = kOwner.getBuildingClassPrereqBuilding(eLoopBuilding, eBuildingClass, kOwner.getBuildingClassMaking(eLoopClass));
						
						FAssert(iPrereqBuildings > 0);

						// subtract the number of eBuildings we have already.
						// (Note, both BuildingClassCount and BuildingClassMaking are cached. This is fast.)
						iPrereqBuildings -= kOwner.getBuildingClassCount(eBuildingClass);
					}

					// only score the local building requirement if the civ-wide requirement is already met
					if (kLoopBuilding.isBuildingClassNeededInCity(kBuilding.getBuildingClassType()) &&
						getNumBuilding(eBuilding) == 0 && iPrereqBuildings <= 0)
					{
						if (kBuilding.getProductionCost() > 0 && kLoopBuilding.getProductionCost() > 0)
						{
							int iTempValue = AI_buildingValue(eLoopBuilding, 0, 0, bConstCache, false);
							if (iTempValue > 0)
							{
								// scale the bonus value by a rough approximation of how likely we are the build the thing
								// (note. the combined production cost is essentially the cost of completing kLoopBuilding given our current position.)
								iTempValue *= 2*kBuilding.getProductionCost();
								iTempValue /= 2*kBuilding.getProductionCost() + 3*kLoopBuilding.getProductionCost();
								
								// Civ4 Reimagined
								if (findYieldRateRank(YIELD_PRODUCTION) < 3 && isWorldWonderClass(eLoopClass))
								{
									iTempValue *= 3;
								}
								
								iValue += iTempValue;
							}
						}
					}

					if (iPrereqBuildings > 0 && iPrereqBuildings <= iNumCities)
					{
						// We've already subtracted the number of eBuilding that are already built.
						// Now we subtract the number of eBuilding that we are currently constructing.
						//iPrereqBuildings -= kOwner.getBuildingClassMaking(eBuildingClass) - (getFirstBuildingOrder(eBuilding) < 0 ? 0 : 1);
						iPrereqBuildings -= kOwner.getBuildingClassMaking(eBuildingClass); // (keep it simple)
						if (iPrereqBuildings > 0)
						{
							// Now we work out how valuable it would be to enable eLoopBuilding;
							// and then scale that value by how many more eBuildings we need.
							CvCity* pLoopCity;
							int iLoop;
							int iHighestValue = 0;
							int iCanBuildPrereq = 0;
							for (pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
							{
								if (pLoopCity->canConstruct(eBuilding) && pLoopCity->getProductionBuilding() != eBuilding)
									iCanBuildPrereq++;
							}
							if (iCanBuildPrereq >= iPrereqBuildings)
							{
								for (pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
								{
									if (pLoopCity->canConstruct(eLoopBuilding, false, true) && pLoopCity->getProductionBuilding() != eLoopBuilding)
										iHighestValue = std::max(pLoopCity->AI_buildingValue(eLoopBuilding, 0, 0, bConstCache, false), iHighestValue);
								}

								int iTempValue = iHighestValue;
								iTempValue *= iCanBuildPrereq + 3*iPrereqBuildings;
								iTempValue /= (iPrereqBuildings+1)*(3*iCanBuildPrereq + iPrereqBuildings);
								// That's between 1/(iPrereqBuildings+1) and 1/3*(iPrereqBuildings+1), depending on # needed and # buildable
								iValue += iTempValue;
							}
						}
					}
				}
			}
			// K-Mod end (prereqs)

			for (int iI = 0; iI < GC.getNumVoteSourceInfos(); ++iI)
			{
				if (kBuilding.getVoteSourceType() == iI)
				{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      05/24/10                              jdog5000        */
/*                                                                                              */
/* City AI, Victory Strategy AI                                                                 */
/************************************************************************************************/					
					int iTempValue = 0;
					if (kBuilding.isStateReligion())
					{
						int iShareReligionCount = 0;
						int iPlayerCount = 0;
						for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
						{
							CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
							if ((iPlayer != getOwner()) && kLoopPlayer.isAlive())
							{
								iPlayerCount++;
								if (GET_PLAYER(getOwnerINLINE()).getStateReligion() == kLoopPlayer.getStateReligion())
								{
									iShareReligionCount++;										
								}
							}
						}
						iTempValue += (200 * (1 + iShareReligionCount)) / (1 + iPlayerCount);
					}
					else
					{
						iTempValue += 100;
					}

					iValue += (iTempValue * (GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_DIPLOMACY1) ? 5 : 1));
				}

				// Value religion buildings based on AP gains
				if (GC.getGameINLINE().isDiploVote((VoteSourceTypes)iI))
				{
					if (GET_PLAYER(getOwnerINLINE()).isLoyalMember((VoteSourceTypes)iI))
					{
						ReligionTypes eReligion = GC.getGameINLINE().getVoteSourceReligion((VoteSourceTypes)iI);

						if (NO_RELIGION != eReligion && isHasReligion(eReligion))
						{
							if (kBuilding.getReligionType() == eReligion)
							{
								for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
								{
									int iChange = GC.getVoteSourceInfo((VoteSourceTypes)iI).getReligionYield(iYield);
									int iTempValue = iChange * 4; // was 6
									// K-Mod
									iTempValue *= AI_yieldMultiplier((YieldTypes)iYield);
									iTempValue /= 100;
									if (iI == YIELD_PRODUCTION)
									{
										// priority += 2.8% per 1% in production increase. roughly. More when at war.
										iPriorityFactor += std::min(100, (bWarPlan ? 320 : 280)*iTempValue/std::max(1, 4*getYieldRate(YIELD_PRODUCTION)));
									}
									// K-Mod end
									iTempValue *= kOwner.AI_yieldWeight((YieldTypes)iYield, this);
									iTempValue /= 100;

									iValue += iTempValue;
								}

								for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
								{
									int iChange = GC.getVoteSourceInfo((VoteSourceTypes)iI).getReligionCommerce(iCommerce);
									int iTempValue = iChange * 4;

									// K-Mod
									if (iTempValue == 0)
										continue;

									iTempValue *= getTotalCommerceRateModifier((CommerceTypes)iCommerce);
									iTempValue /= 100;
									// K-Mod end

									// +99 mirrors code below, I think because commerce weight can be pretty small. (It's so it rounds up, not down. - K-Mod)
									iTempValue *= kOwner.AI_commerceWeight((CommerceTypes)iCommerce, this);
									iTempValue = (iTempValue + 99) / 100;

									iValue += iTempValue;
								}
							}
						}
					}
				}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
			}

		}

		if (iPass > 0)
		{
			// K-Mod, I've moved this from inside the yield types loop; and I've increased the value to compensate.
			if (iFoodDifference > 0 && kBuilding.getFoodKept() != 0)
			{
				//iValue += kBuilding.getFoodKept() / 2;
				iValue += std::max(0, 2*(std::max(4, AI_getTargetPopulation()) - getPopulation())+(bCanPopRush ?3 :1)) * kBuilding.getFoodKept() / 4;
			}
			// K-Mod end

			for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				// K-Mod - I've shuffled some parts of this code around.
				int iTempValue = 0;

				/*iTempValue += ((kBuilding.getYieldModifier(iI) * getBaseYieldRate((YieldTypes)iI)) / 10);
				iTempValue += ((kBuilding.getPowerYieldModifier(iI) * getBaseYieldRate((YieldTypes)iI)) / ((bProvidesPower || isPower()) ? 12 : 15));*/
				// K-Mod
				int iBaseRate = getBaseYieldRate((YieldTypes)iI) + 2; // +2 just to represent potential growth.
				iTempValue += kBuilding.getYieldModifier(iI) * iBaseRate / 25;
				// Civ4 Reimagined
				if ((YieldTypes)iI == YIELD_PRODUCTION)
				{
					iTempValue += kBuilding.getBuildingProductionModifier() * iBaseRate / 40; // Civ4 Reimagined: originally 50
				}
				iTempValue += kBuilding.getPowerYieldModifier(iI) * iBaseRate / (bProvidesPower || isPower() ? 27 : 50);

				if (bProvidesPower && !isPower())
				{
					iTempValue += ((getPowerYieldRateModifier((YieldTypes)iI) * iBaseRate) / 27); // originally 12
				}

				for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
				{
					if (hasBonus((BonusTypes)iJ))
					{
						iTempValue += ((kBuilding.getBonusYieldModifier(iJ, iI) * iBaseRate) / 27); // originally 12
						// Civ4 Reimagined
						if ((YieldTypes)iI == YIELD_PRODUCTION)
						{
							iTempValue += (kBuilding.getBonusBuildingProductionModifier(iJ) * iBaseRate) / 40; // Civ4 Reimagined: originally 50
						}
					}
				}
				
				// Civ4 Reimagined
				if (kBuilding.isAnyTechYieldModifier())
				{
					for (int iJ = 0; iJ < GC.getNumTechInfos(); iJ++)
					{
						if (GET_TEAM(GET_PLAYER(getOwnerINLINE()).getTeam()).isHasTech((TechTypes)iJ))
						{
							iTempValue += ((kBuilding.getTechYieldModifier(iJ, iI) * iBaseRate) / 27); // originally 12
						}
					}
				}

				// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
				// we do _not_ want to build this here (unless the value was small anyway)
				//if (bIsLimitedWonder && (findBaseYieldRateRank((YieldTypes)iI) > (3 + iLimitedWonderLimit)))
				// K-Mod: lets say top 1/3 instead. There are now other mechanisms to stop us from wasting buildings.
				if (bIsLimitedWonder && findBaseYieldRateRank((YieldTypes)iI) > iNumCities/3 + 1 + iLimitedWonderLimit)
				{
					iTempValue *= -1;
				}

				// (K-Mod)...and now the things that should not depend on whether or not we have a good yield rank
				int iRawYieldValue = 0;
				if (iI != YIELD_COMMERCE) // K-Mod, don't count trade commerce here, because that has already been counted earlier... (the systme is a mess, I know.)
				{
					iRawYieldValue += ((kBuilding.getTradeRouteModifier() * getTradeYield((YieldTypes)iI)) / 26); // originally 12 (and 'iValue')
					//if (bForeignTrade)
					if (bForeignTrade && !kOwner.isNoForeignTrade()) // K-Mod
					{
						iRawYieldValue += ((kBuilding.getForeignTradeRouteModifier() * getTradeYield((YieldTypes)iI)) / 35); // originally 12 (and 'iValue')
					}
					// Civ4 Reimagined
					if (iOverseaTradeRoutes > 0)
					{
						iRawYieldValue += ((kBuilding.getOverseaTradeRouteModifier() * getTradeYield((YieldTypes)iI)) / 35);
					}
				}

				/* original bts code (We're inside a yield types loop. This would be triple counted here!)
				if (iFoodDifference > 0)
				{
					iValue += kBuilding.getFoodKept() / 2;
				} */

				if (kBuilding.getSeaPlotYieldChange(iI) > 0)
				{
					//iRawYieldValue += kBuilding.getSeaPlotYieldChange(iI) * AI_buildingSpecialYieldChangeValue(eBuilding, (YieldTypes)iI);
					iRawYieldValue += kBuilding.getSeaPlotYieldChange(iI) * AI_buildingSeaYieldChangeWeight(eBuilding, iFoodDifference > 0 && iHappinessLevel > 0); // K-Mod
				}
				if (kBuilding.getRiverPlotYieldChange(iI) > 0)
				{
					iRawYieldValue += (kBuilding.getRiverPlotYieldChange(iI) * countNumRiverPlots() * 3); // was 4
				}
				iRawYieldValue += (kBuilding.getYieldChange(iI) * 4); // was 6

				iRawYieldValue *= AI_yieldMultiplier((YieldTypes)iI);
				iRawYieldValue /= 100;
				
				iTempValue += iRawYieldValue;

				for (int iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
				{
					iTempValue += ((kBuilding.getSpecialistYieldChange(iJ, iI) * kOwner.getTotalPopulation()) / 5);
				}
				iTempValue += (kBuilding.getGlobalSeaPlotYieldChange(iI) * kOwner.countNumCoastalCities() * 8);
				iTempValue += ((kBuilding.getAreaYieldModifier(iI) * iNumCitiesInArea) / 3);
				iTempValue += ((kBuilding.getGlobalYieldModifier(iI) * iNumCities) / 3);

				if (iTempValue != 0)
				{
					/* original bts code (this is rolled into AI_yieldWeight)
					if (bFinancialTrouble && iI == YIELD_COMMERCE)
					{
						iTempValue *= 2;
					}*/

					// K-Mod
					if (iI == YIELD_PRODUCTION)
					{
						// priority += 2.4% per 1% in production increase. roughly. More when at war.
						iPriorityFactor += std::min(100, (bWarPlan ? 280 : 240)*iTempValue/std::max(1, 4*getYieldRate(YIELD_PRODUCTION)));
					}
					// K-Mod end

					iTempValue *= kOwner.AI_yieldWeight((YieldTypes)iI, this);
					iTempValue /= 100;

					// (limited wonder condition use to be here. I've moved it. - Karadoc)

					iValue += iTempValue;
				}
			}
		}
		else
		{
			if (iFocusFlags & BUILDINGFOCUS_FOOD)
			{

				iValue += kBuilding.getFoodKept();

				if (kBuilding.getSeaPlotYieldChange(YIELD_FOOD) > 0)
				{
					//int iTempValue = kBuilding.getSeaPlotYieldChange(YIELD_FOOD) * AI_buildingSpecialYieldChangeValue(eBuilding, YIELD_FOOD);
					int iTempValue = kBuilding.getSeaPlotYieldChange(YIELD_FOOD) * AI_buildingSeaYieldChangeWeight(eBuilding, iFoodDifference > 0 && iHappinessLevel > 0); // K-Mod
					if ((iTempValue < 8) && (getPopulation() > 3))
					{
						// don't bother
					}
					else
					{
						iValue += ((iTempValue * 4) / std::max(2, iFoodDifference));
					}
				}

				if (kBuilding.getRiverPlotYieldChange(YIELD_FOOD) > 0)
				{
					iValue += (kBuilding.getRiverPlotYieldChange(YIELD_FOOD) * countNumRiverPlots() * 4);
				}
			}

			if (iFocusFlags & BUILDINGFOCUS_PRODUCTION)
			{
				int iTempValue = ((kBuilding.getYieldModifier(YIELD_PRODUCTION) * getBaseYieldRate(YIELD_PRODUCTION)) / 20);
				iTempValue += ((kBuilding.getPowerYieldModifier(YIELD_PRODUCTION) * getBaseYieldRate(YIELD_PRODUCTION)) / ((bProvidesPower || isPower()) ? 24 : 30));
				if (kBuilding.getSeaPlotYieldChange(YIELD_PRODUCTION) > 0)
				{
					int iNumWaterPlots = countNumWaterPlots();
					if (!bIsLimitedWonder || (iNumWaterPlots > NUM_CITY_PLOTS / 2))
					{
						iTempValue += kBuilding.getSeaPlotYieldChange(YIELD_PRODUCTION) * iNumWaterPlots;
					}
				}
				if (kBuilding.getRiverPlotYieldChange(YIELD_PRODUCTION) > 0)
				{
					iTempValue += (kBuilding.getRiverPlotYieldChange(YIELD_PRODUCTION) * countNumRiverPlots() * 4);
				}
				if (bProvidesPower && !isPower())
				{
					//iTempValue += ((getPowerYieldRateModifier(YIELD_PRODUCTION) * getBaseYieldRate(YIELD_PRODUCTION)) / 12);
					iTempValue += ((getPowerYieldRateModifier(YIELD_PRODUCTION) * getBaseYieldRate(YIELD_PRODUCTION)) / 24); // K-Mod, consistency
				}

				// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
				// we do _not_ want to build this here (unless the value was small anyway)
				if (bIsLimitedWonder && (findBaseYieldRateRank(YIELD_PRODUCTION) > (3 + iLimitedWonderLimit)))
				{
					iTempValue *= -1;
				}

				iValue += iTempValue;
			}

			if (iFocusFlags & BUILDINGFOCUS_GOLD)
			{
				int iTempValue = ((kBuilding.getYieldModifier(YIELD_COMMERCE) * getBaseYieldRate(YIELD_COMMERCE)));
				iTempValue *= kOwner.getCommercePercent(COMMERCE_GOLD);

				if (bFinancialTrouble)
				{
					iTempValue *= 2;
				}

				iTempValue /= 3000;

				/*if (MAX_INT == aiCommerceRank[COMMERCE_GOLD])
				{
					aiCommerceRank[COMMERCE_GOLD] = findCommerceRateRank(COMMERCE_GOLD);
				}*/

				// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
				// we do _not_ want to build this here (unless the value was small anyway)
				if (bIsLimitedWonder && (findCommerceRateRank(COMMERCE_GOLD) > (3 + iLimitedWonderLimit)))
				{
					iTempValue *= -1;
				}

				iValue += iTempValue;
			}
		}

		if (iPass > 0)
		{
			for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
			{
				int iTempValue = 0;

				iTempValue += (kBuilding.getCommerceChange(iI) * 4);
				iTempValue += (kBuilding.getObsoleteSafeCommerceChange(iI) * 4);
				// Civ4 Reimagind
				iTempValue += kBuilding.getCommercePerCultureLevel(iI) * getCultureLevel() * 4;
				iTempValue += kBuilding.getCommercePerWorldWonder(iI) * getNumWorldWonders() * 4;

				// Civ4 Reimagind
				if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
				{
					iTempValue += kBuilding.getCommerceFromCoast(iI) * 4;
				}

				// Civ4 Reimagind
		        if (getMaxAirlift() > 0)
		        {
		            iTempValue += kBuilding.getCommerceFromAirlift(iI);
		        }

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/13/10                              jdog5000        */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
				if( kBuilding.getReligionType() != NO_RELIGION && kBuilding.getReligionType() == kOwner.getStateReligion() )
				{
					iTempValue += kOwner.getStateReligionBuildingCommerce((CommerceTypes)iI) * 3;
				}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
				iTempValue += ((kBuilding.getSpecialistExtraCommerce(iI) * kOwner.getTotalPopulation()) / 3); // Moved from below (K-Mod)
				//iTempValue *= 100 + kBuilding.getCommerceModifier(iI);
				iTempValue *= getTotalCommerceRateModifier((CommerceTypes)iI) + kBuilding.getCommerceModifier(iI); // K-Mod. Note that getTotalCommerceRateModifier() includes the +100.
				iTempValue /= 100;

				if (kBuilding.getCommerceChangeDoubleTime(iI) > 0)
				{
					if ((kBuilding.getCommerceChange(iI) > 0) || (kBuilding.getObsoleteSafeCommerceChange(iI) > 0))
					{
						//iTempValue += (1000 / kBuilding.getCommerceChangeDoubleTime(iI));
						iTempValue += iTempValue * 250 / kBuilding.getCommerceChangeDoubleTime(iI); // K-Mod (still very rough...)
					}
				}

				if ((CommerceTypes)iI == COMMERCE_CULTURE)
				{
					if (bCulturalVictory1)
					{
						iTempValue *= 2;
					}

					// K-Mod. Build culture buildings quickly to pop our borders (but not wonders / special buildings)
					if (iTempValue > 0 && !bIsLimitedWonder && kBuilding.getProductionCost() > 0 &&
						getCultureLevel() <= (CultureLevelTypes)1 && getCommerceRateTimes100(COMMERCE_CULTURE) < 100)
					{
						// Civ4 Reimagined
						iTempValue += 20 + 20 * AI_countGoodTiles(true, true);
						iPriorityFactor += 100; // Civ4 Reimagined, was 25
						if (gCityLogLevel > 0) logBBAI("		Culture building boost: %d (unworked good tiles: %d)", iTempValue, AI_countGoodTiles(true, true));
					}
					// K-Mod end
				}

				// K-mod. the getCommerceChangeDoubleTime bit use to be here. I moved it up to be before the culture value boost.

				// add value for a commerce modifier
				int iCommerceModifier = kBuilding.getCommerceModifier(iI);

				// Civ4 Reimagined
				if (kBuilding.isAnyTechCommerceModifier())
				{
					for (int iJ = 0; iJ < GC.getNumTechInfos(); iJ++)
					{
						if (GET_TEAM(GET_PLAYER(getOwnerINLINE()).getTeam()).isHasTech((TechTypes)iJ))
						{
							iCommerceModifier += kBuilding.getTechCommerceModifier(iJ, iI);
						}
					}
				}

				int iBaseCommerceRate = getBaseCommerceRate((CommerceTypes) iI);
				// K-Mod. inflate the base commerce rate, to account for the fact that commerce multipliers give us flexibility.
				
				// Civ4 Reimagined
				if (isCapital() && kOwner.getCapitalCommerceRateModifierPerHappinessSurplus((CommerceTypes)iI) > 0)
				{
					int iGood, iBad = 0;
					iCommerceModifier += getAdditionalHappinessByBuilding(eBuilding,iGood,iBad) * kOwner.getCapitalCommerceRateModifierPerHappinessSurplus((CommerceTypes)iI);
				}
				
				{
					int x = std::max(5, kOwner.getCommercePercent((CommerceTypes)iI));
					if (iI == COMMERCE_CULTURE)
					{
						x += x <= 45 && bCulturalVictory1 ? 10 : 0;
						x += x <= 45 && bCulturalVictory2 ? 10 : 0;
						x += x <= 45 && bCulturalVictory3 ? 10 : 0;
					}
					iBaseCommerceRate += getYieldRate(YIELD_COMMERCE) * x * (100 - x) / 10000;
				}
				// K-Mod end
				int iCommerceMultiplierValue = iCommerceModifier * iBaseCommerceRate;
				if (iI == COMMERCE_CULTURE && iCommerceModifier != 0)
				{
					// K-Mod: bug fix, and improvement. (the old code was missing /= 100, and it was too conditional)
					// (disabled indefinitely. culture pressure is counted in other ways; so I want to test without this boost.)
					//iCommerceMultiplierValue *= culturePressureFactor() + 100;
					//iCommerceMultiplierValue /= 200;
					// K-Mod end

					// K-Mod. the value of culture is now boosted primarily inside AI_commerceWeight
					// so I've decreased the value boost in the following block.
					if (bCulturalVictory1)
					{
						// if this is one of our top culture cities, then we want to build this here first!
						if (iCultureRank <= iCulturalVictoryNumCultureCities)
						{
							iCommerceMultiplierValue /= 15; // was 8

							// if we at culture level 3, then these need to get built asap
							if (bCulturalVictory3)
							{
								int iLoop;
								int iHighestRate = 0;
								for (CvCity* pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
								{
									iHighestRate = std::max(iHighestRate, pLoopCity->getCommerceRate(COMMERCE_CULTURE));
								}
								FAssert(iHighestRate >= getCommerceRate(COMMERCE_CULTURE));

								// its most important to build in the lowest rate city, but important everywhere
								iCommerceMultiplierValue += (iHighestRate - getCommerceRate(COMMERCE_CULTURE)) * iCommerceModifier / 15; // was 8
							}
						}
						else
						{
							//int iCountBuilt = kOwner.getBuildingClassCountPlusMaking(eBuildingClass);
							int iCountBuilt = kOwner.getBuildingClassCount(eBuildingClass); // K-Mod (to match the number used by getBuildingClassPrereqBuilding)

							// do we have enough buildings to build extras?
							bool bHaveEnough = true;

							// if its limited and the limit is less than the number we need in culture cities, do not build here
							if (bIsLimitedWonder && (iLimitedWonderLimit <= iCulturalVictoryNumCultureCities))
							{
								bHaveEnough = false;
							}

							for (int iJ = 0; bHaveEnough && iJ < GC.getNumBuildingClassInfos(); iJ++)
							{
								// K-Mod: bug fix.
								/* original bts code
								// count excess the number of prereq buildings which do not have this building built for yet
								int iPrereqBuildings = kOwner.getBuildingClassPrereqBuilding(eBuilding, (BuildingClassTypes) iJ, -iCountBuilt);

								// do we not have enough built (do not count ones in progress)
								if (iPrereqBuildings > 0 && kOwner.getBuildingClassCount((BuildingClassTypes) iJ) <  iPrereqBuildings)
								{
									bHaveEnough = false;
								}*/

								// Whatever that code was meant to do, I'm pretty sure it was wrong.
								int iPrereqBuildings = kOwner.getBuildingClassPrereqBuilding(eBuilding, (BuildingClassTypes) iJ, iCulturalVictoryNumCultureCities - iCountBuilt);
								if (kOwner.getBuildingClassCount((BuildingClassTypes) iJ) < iPrereqBuildings)
									bHaveEnough = false;
								// K-Mod end
							}

							// if we have enough and our rank is close to the top, then possibly build here too
							if (bHaveEnough && (iCultureRank - iCulturalVictoryNumCultureCities) <= 3)
							{
								iCommerceMultiplierValue /= 20; // was 12
							}
							// otherwise, we really do not want to build this here
							else
							{
								iCommerceMultiplierValue /= 50; // was 30
							}
						}
					}
					else
					{
						iCommerceMultiplierValue /= 25; // was 15

						// increase priority if we need culture oppressed city
						// K-Mod: moved this to outside of the current "if".
						// It should still apply even when going for a cultural victory!
						//iCommerceMultiplierValue *= (100 - calculateCulturePercent(getOwnerINLINE()));
					}
				}
				else
				{
					iCommerceMultiplierValue /= 25; // was 15
				}
				iTempValue += iCommerceMultiplierValue;

				// K-Mod. help get the ball rolling on espionage.
				if (iI == COMMERCE_ESPIONAGE && iTempValue > 0 && !GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
				{
					// priority += 1% per 1% increase in total espionage, more for big espionage strategy
					iPriorityFactor += std::min(100, (kOwner.AI_isDoStrategy(AI_STRATEGY_BIG_ESPIONAGE)?150 : 100) * iTempValue/std::max(1, 4*kOwner.getCommerceRate(COMMERCE_ESPIONAGE)));
				}

				// ... and increase the priority of research buildings if aiming for a space victory.
				if ((CommerceTypes)iI == COMMERCE_RESEARCH && bSpaceVictory1)
				{
					iPriorityFactor += std::min(25, iTempValue/2);
				}
				// K-Mod end

				iTempValue += ((kBuilding.getGlobalCommerceModifier(iI) * iNumCities) / 4);
				// iTempValue += ((kBuilding.getSpecialistExtraCommerce(iI) * kOwner.getTotalPopulation()) / 3); // moved up (K-Mod)

				/* original bts code
				if (eStateReligion != NO_RELIGION)
				{
					iTempValue += (kBuilding.getStateReligionCommerce(iI) * kOwner.getHasReligionCount(eStateReligion) * 3);
				} */
				// K-Mod. A more accurate calculation of the value from increasing commerce on all state religion buildings. (eg. Sankore)
				if (eStateReligion != NO_RELIGION && kBuilding.getStateReligionCommerce(iI) != 0)
				{
					int iCount = 0;
					for (BuildingClassTypes eLoopClass = (BuildingClassTypes)0; eLoopClass < GC.getNumBuildingClassInfos(); eLoopClass = (BuildingClassTypes)(eLoopClass+1))
					{
						BuildingTypes eLoopBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eLoopClass); // K-Mod

						if (eLoopBuilding != NO_BUILDING &&
							GC.getBuildingInfo(eLoopBuilding).getReligionType() == eStateReligion &&
							!GET_TEAM(kOwner.getTeam()).isObsoleteBuilding(eLoopBuilding) &&
							!kOwner.isWrongCivicBuilding(eLoopBuilding)) // Civ4 Reimagined
						{
							iCount += kOwner.getBuildingClassCountPlusMaking(eLoopClass);
						}
					}
					iCount = std::max(iCount, kOwner.getHasReligionCount(eStateReligion));
					iTempValue += iCount * 35 * kBuilding.getStateReligionCommerce(iI) * kOwner.AI_averageCommerceMultiplier((CommerceTypes)iI) / 1000; //Civ4 Reimagined: kBuilding.getStateReligionCommerce(iI) was missing
				}
				// K-Mod end

				if (kBuilding.getGlobalReligionCommerce() != NO_RELIGION)
				{
					/*iTempValue += (GC.getReligionInfo((ReligionTypes)(kBuilding.getGlobalReligionCommerce())).getGlobalReligionCommerce(iI) * GC.getGameINLINE().countReligionLevels((ReligionTypes)(kBuilding.getGlobalReligionCommerce())) * 2);
					if (eStateReligion == (ReligionTypes)(kBuilding.getGlobalReligionCommerce()))
					{
					    iTempValue += 10;
					}*/
					// K-Mod
					int iExpectedSpread = GC.getGameINLINE().countReligionLevels((ReligionTypes)kBuilding.getGlobalReligionCommerce());
					iExpectedSpread += (GC.getNumEraInfos() - kOwner.getCurrentEra() + (eStateReligion == (ReligionTypes)(kBuilding.getGlobalReligionCommerce())? 2 : 0)) * GC.getWorldInfo(GC.getMap().getWorldSize()).getDefaultPlayers();
					iTempValue += GC.getReligionInfo((ReligionTypes)kBuilding.getGlobalReligionCommerce()).getGlobalReligionCommerce(iI) * iExpectedSpread * 4;
				}

				// K-Mod: I've moved the corporation stuff that use to be here to outside this loop so that it isn't quadriple counted

				if (kBuilding.isCommerceFlexible(iI))
				{
					if (!(kOwner.isCommerceFlexible((CommerceTypes)iI)))
					{
						iTempValue += 40;
					}
				}

				if (kBuilding.isCommerceChangeOriginalOwner(iI))
				{
					if ((kBuilding.getCommerceChange(iI) > 0) || (kBuilding.getObsoleteSafeCommerceChange(iI) > 0))
					{
						iTempValue++;
					}
				}

				if (iTempValue != 0)
				{
					if (bFinancialTrouble && iI == COMMERCE_GOLD)
					{
						iTempValue *= 2;
					}

					iTempValue *= kOwner.AI_commerceWeight(((CommerceTypes)iI), this);
					iTempValue = (iTempValue + 99) / 100;

					// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
					// we do _not_ want to build this here (unless the value was small anyway)
					/*if (MAX_INT == aiCommerceRank[iI])
					{
						aiCommerceRank[iI] = findCommerceRateRank((CommerceTypes) iI);
					}*/
					/* original bts code
					if (bIsLimitedWonder && ((aiCommerceRank[iI] > (3 + iLimitedWonderLimit)))
						|| (bCulturalVictory1 && (iI == COMMERCE_CULTURE) && (aiCommerceRank[iI] == 1)))
					{
						iTempValue *= -1;

						// for culture, just set it to zero, not negative, just about every wonder gives culture
						if (iI == COMMERCE_CULTURE)
						{
							iTempValue = 0;
						}
					} */
					// K-Mod, let us build culture wonders to defend againt culture pressure
					// also, lets say top 1/3 instead. There are now other mechanisms to stop us from wasting buildings.
					if (bIsLimitedWonder)
					{
						if (iI == COMMERCE_CULTURE)
						{
							if (bCulturalVictory1 &&
								((bCulturalVictory2 && findCommerceRateRank((CommerceTypes)iI) == 1) ||
								findCommerceRateRank((CommerceTypes)iI) > iNumCities/3 + 1 + iLimitedWonderLimit))
							{
								iTempValue = 0;
							}
						}

						else if (findCommerceRateRank((CommerceTypes)iI) > iNumCities/3 + 1 + iLimitedWonderLimit)
						{
							iTempValue *= -1;
						}
					}
					// K-Mod end

					iValue += iTempValue;
				}
			}

			// corp evaluation moved here, and rewriten for K-Mod
			{
				CorporationTypes eCorporation = (CorporationTypes)kBuilding.getFoundsCorporation();
				int iCorpValue = 0;
				int iExpectedSpread = kOwner.AI_isDoVictoryStrategyLevel4() ? 45 : 70 - (bWarPlan ? 10 : 0);
				// note: expected spread starts as percent (for precision), but is later converted to # of cities.
				if (kOwner.isNoCorporations())
					iExpectedSpread = 0;
				if (NO_CORPORATION != eCorporation && iExpectedSpread > 0)
				{
					//iCorpValue = kOwner.AI_corporationValue(eCorporation, this);
					// K-Mod: consider the corporation for the whole civ, not just this city.
					iCorpValue = kOwner.AI_corporationValue(eCorporation);

					for (int iCorp = 0; iCorp < GC.getNumCorporationInfos(); iCorp++)
					{
						if (iCorp != eCorporation)
						{
							if (kOwner.hasHeadquarters((CorporationTypes)iCorp))
							{
								if (GC.getGame().isCompetingCorporation(eCorporation, (CorporationTypes)iCorp))
								{
									// This new corp is no good to us if our competing corp is already better.
									// note: evaluation of the competing corp for this particular city is ok.
									if (kOwner.AI_corporationValue((CorporationTypes)iCorp, this) > iCorpValue)
									{
										iExpectedSpread = 0;
										break;
									}
									// expect to spread the corp to fewer cities.
									iExpectedSpread /= 2;
								}
							}
						}
					}
					// convert spread from percent to # of cities
					iExpectedSpread = iExpectedSpread * iNumCities / 100;

					// scale corp value by the expected spread
					iCorpValue *= iExpectedSpread;

					// Rescale from 100x commerce down to 4x commerce. (AI_corporationValue returns roughly 100x commerce)
					iCorpValue *= 4;
					iCorpValue /= 100;
				}

				if (kBuilding.getGlobalCorporationCommerce() != NO_CORPORATION)
				{
					iExpectedSpread += GC.getGameINLINE().countCorporationLevels((CorporationTypes)(kBuilding.getGlobalCorporationCommerce()));
					
					//Civ4 Reimagined
					iExpectedSpread += GC.getCorporationInfo((CorporationTypes)(kBuilding.getGlobalCorporationCommerce())).getSpreadFactor() * GC.getGameINLINE().getNumCities() / 500;

					if (iExpectedSpread > 0)
					{
						for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
						{
							int iHqValue = 4 * GC.getCorporationInfo((CorporationTypes)(kBuilding.getGlobalCorporationCommerce())).getHeadquarterCommerce(iI) * iExpectedSpread;
							if (iHqValue != 0)
							{
								iHqValue *= getTotalCommerceRateModifier((CommerceTypes)iI);
								iHqValue *= kOwner.AI_commerceWeight((CommerceTypes)iI, this);
								iHqValue /= 10000;
							}
							// use rank as a tie-breaker... with number of national wonders thrown in to,
							// (I'm trying to boost the chance that the AI will put wallstreet with its corp HQs.)
							if (iHqValue > 0)
							{
								iHqValue *= 3*iNumCities - findCommerceRateRank((CommerceTypes)iI) - getNumNationalWonders()/2;
								iHqValue /= 2*iNumCities;
							}
							iCorpValue += iHqValue;
						}
					}
				}

				if (iCorpValue > 0)
				{
					iValue += iCorpValue;
				}
			}
			// K-Mod end (corp)

			for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
			{
				if (kBuilding.getReligionChange(iI) > 0)
				{
					if (GET_TEAM(getTeam()).hasHolyCity((ReligionTypes)iI))
					{
						iValue += (kBuilding.getReligionChange(iI) * ((eStateReligion == iI) ? 10 : 1));
					}
				}
			}

			if (NO_VOTESOURCE != kBuilding.getVoteSourceType())
			{
				iValue += 100;
			}
		}
		else
		{
			if (iFocusFlags & BUILDINGFOCUS_GOLD)
			{
				int iTempValue = ((kBuilding.getCommerceModifier(COMMERCE_GOLD) * getBaseCommerceRate(COMMERCE_GOLD)) / 40);

				if (iTempValue != 0)
				{
					if (bFinancialTrouble)
					{
						iTempValue *= 2;
					}

					/*if (MAX_INT == aiCommerceRank[COMMERCE_GOLD])
					{
						aiCommerceRank[COMMERCE_GOLD] = findCommerceRateRank(COMMERCE_GOLD);
					}*/

					// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
					// we do _not_ want to build this here (unless the value was small anyway)
					if (bIsLimitedWonder && (findCommerceRateRank(COMMERCE_GOLD) > (3 + iLimitedWonderLimit)))
					{
						iTempValue *= -1;
					}
					iValue += iTempValue;
				}

				iValue += (kBuilding.getCommerceChange(COMMERCE_GOLD) * 4);
				iValue += (kBuilding.getObsoleteSafeCommerceChange(COMMERCE_GOLD) * 4);
				// Civ4 Reimagined
				iValue += (kBuilding.getCommercePerCultureLevel(COMMERCE_GOLD) * getCultureLevel() * 4);
				iValue += (kBuilding.getCommercePerWorldWonder(COMMERCE_GOLD) * getNumWorldWonders() * 4);

				// Civ4 Reimagind
				if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
				{
					iValue += kBuilding.getCommerceFromCoast(COMMERCE_GOLD) * 4;
				}

				// Civ4 Reimagind
		        if (getMaxAirlift() > 0)
		        {
		            iValue += kBuilding.getCommerceFromAirlift(COMMERCE_GOLD);
		        }
			}

			if (iFocusFlags & BUILDINGFOCUS_RESEARCH)
			{
				int iTempValue = ((kBuilding.getCommerceModifier(COMMERCE_RESEARCH) * getBaseCommerceRate(COMMERCE_RESEARCH)) / 40);

				if (iTempValue != 0)
				{
					/*if (MAX_INT == aiCommerceRank[COMMERCE_RESEARCH])
					{
						aiCommerceRank[COMMERCE_RESEARCH] = findCommerceRateRank(COMMERCE_RESEARCH);
					}*/

					// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
					// we do _not_ want to build this here (unless the value was small anyway)
					if (bIsLimitedWonder && (findCommerceRateRank(COMMERCE_RESEARCH) > (3 + iLimitedWonderLimit)))
					{
						iTempValue *= -1;
					}

					iValue += iTempValue;
				}
				iValue += (kBuilding.getCommerceChange(COMMERCE_RESEARCH) * 4);
				iValue += (kBuilding.getObsoleteSafeCommerceChange(COMMERCE_RESEARCH) * 4);
				// Civ4 Reimagined
				iValue += (kBuilding.getCommercePerCultureLevel(COMMERCE_RESEARCH) * getCultureLevel() * 4);
				iValue += (kBuilding.getCommercePerWorldWonder(COMMERCE_RESEARCH) * getNumWorldWonders() * 4);

				// Civ4 Reimagind
				if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
				{
					iValue += kBuilding.getCommerceFromCoast(COMMERCE_RESEARCH) * 4;
				}

				// Civ4 Reimagind
		        if (getMaxAirlift() > 0)
		        {
		            iValue += kBuilding.getCommerceFromAirlift(COMMERCE_RESEARCH);
		        }
			}

			if (iFocusFlags & BUILDINGFOCUS_CULTURE)
			{
				int iTempValue = (kBuilding.getCommerceChange(COMMERCE_CULTURE) * 3);
				iTempValue += (kBuilding.getObsoleteSafeCommerceChange(COMMERCE_CULTURE) * 3);
				if (GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
				{
					iTempValue += (kBuilding.getCommerceChange(COMMERCE_ESPIONAGE) * 3);
					iTempValue += (kBuilding.getObsoleteSafeCommerceChange(COMMERCE_ESPIONAGE) * 3);
					// Civ4 Reimagined
					iTempValue += (kBuilding.getCommercePerCultureLevel(COMMERCE_ESPIONAGE) * getCultureLevel() * 3);
					iTempValue += (kBuilding.getCommercePerWorldWonder(COMMERCE_ESPIONAGE) * getNumWorldWonders() * 3);

					// Civ4 Reimagind
					if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
					{
						iValue += kBuilding.getCommerceFromCoast(COMMERCE_ESPIONAGE) * 3;
					}
				}

				//if ((getCommerceRate(COMMERCE_CULTURE) == 0) && (AI_calculateTargetCulturePerTurn() == 1))
				if (getCommerceRate(COMMERCE_CULTURE) == 0)
				{
					if (iTempValue >= 3)
					{
						iTempValue += 7;
					}
				}

				// K-Mod, this stuff was moved from below
				iTempValue += ((kBuilding.getCommerceModifier(COMMERCE_CULTURE) * getBaseCommerceRate(COMMERCE_CULTURE)) / 15);
				if (GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
				{
					iTempValue += ((kBuilding.getCommerceModifier(COMMERCE_ESPIONAGE) * getBaseCommerceRate(COMMERCE_ESPIONAGE)) / 15);
				}
				// K-Mod end

				if (iTempValue != 0)
				{
					/*if (MAX_INT == aiCommerceRank[COMMERCE_CULTURE])
					{
						aiCommerceRank[COMMERCE_CULTURE] = findCommerceRateRank(COMMERCE_CULTURE);
					}*/

					// if this is a limited wonder, and we are not one of the top 4 in this category, 
					// do not count the culture value
					// we probably do not want to build this here (but we might)
					/* original bts code
					if (bIsLimitedWonder && (findCommerceRateRank(COMMERCE_CULTURE) > (3 + iLimitedWonderLimit)))
					{
						iTempValue  = 0;
					}*/
					// K-Mod. The original code doesn't take prereq buildings into account, and it was in the wrong place.
					// To be honest, I think this "building focus" flag system is pretty bad; but I'm fixing it anyway.
					if (findCommerceRateRank(COMMERCE_CULTURE) > iCulturalVictoryNumCultureCities)
					{
						bool bAvoid = false;
						if (bIsLimitedWonder && findCommerceRateRank(COMMERCE_CULTURE) - iLimitedWonderLimit >= iCulturalVictoryNumCultureCities)
							bAvoid = true;
						for (int iJ = 0; !bAvoid && iJ < GC.getNumBuildingClassInfos(); iJ++)
						{
							int iPrereqBuildings = kOwner.getBuildingClassPrereqBuilding(eBuilding, (BuildingClassTypes) iJ, iCulturalVictoryNumCultureCities - kOwner.getBuildingClassCount(eBuildingClass));
							if (kOwner.getBuildingClassCount((BuildingClassTypes) iJ) < iPrereqBuildings)
								bAvoid = true;
						}
						if (bAvoid)
							iTempValue = 0;
					}
					// K-Mod end
					iValue += iTempValue;
				}

				/* original bts code. (I've moved this stuff up to be before the limited wonder checks.
				iValue += ((kBuilding.getCommerceModifier(COMMERCE_CULTURE) * getBaseCommerceRate(COMMERCE_CULTURE)) / 15);
				if (GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
				{
					iValue += ((kBuilding.getCommerceModifier(COMMERCE_ESPIONAGE) * getBaseCommerceRate(COMMERCE_ESPIONAGE)) / 15);
				}*/
			}

			if (iFocusFlags & BUILDINGFOCUS_BIGCULTURE)
			{
				int iTempValue = (kBuilding.getCommerceModifier(COMMERCE_CULTURE) / 5);
				if (iTempValue != 0)
				{
					/*if (MAX_INT == aiCommerceRank[COMMERCE_CULTURE])
					{
						aiCommerceRank[COMMERCE_CULTURE] = findCommerceRateRank(COMMERCE_CULTURE);
					}*/

					// if this is a limited wonder, and we are not one of the top 4 in this category, 
					// do not count the culture value
					// we probably do not want to build this here (but we might)
					if (bIsLimitedWonder && (findCommerceRateRank(COMMERCE_CULTURE) > (3 + iLimitedWonderLimit)))
					{
						iTempValue  = 0;
					}

					iValue += iTempValue;
				}
			}

			//if (iFocusFlags & BUILDINGFOCUS_ESPIONAGE || (GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE) && (iFocusFlags & BUILDINGFOCUS_CULTURE)))
			// K-Mod: the "no espionage" stuff is already taken into account in the culture section.
			if (iFocusFlags & BUILDINGFOCUS_ESPIONAGE && !GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
			{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/09/10                                jdog5000      */
/*                                                                                              */
/* City AI                                                                                      */
/************************************************************************************************/
				// K-Mod, changed this section.
				int iTempValue = ((kBuilding.getCommerceModifier(COMMERCE_ESPIONAGE) * getBaseCommerceRate(COMMERCE_ESPIONAGE)) / 50);

				if (iTempValue != 0)
				{
					// if this is a limited wonder, and we are not one of the top 4 in this category, subtract the value
					// we do _not_ want to build this here (unless the value was small anyway)
					if (bIsLimitedWonder && (findCommerceRateRank(COMMERCE_ESPIONAGE) > (3 + iLimitedWonderLimit)))
					{
						iTempValue *= -1;
					}

					iValue += iTempValue;
				}
				iTempValue = (kBuilding.getCommerceChange(COMMERCE_ESPIONAGE) * 4);
				iTempValue += (kBuilding.getObsoleteSafeCommerceChange(COMMERCE_ESPIONAGE) * 4);
				iTempValue += kBuilding.getCommercePerCultureLevel(COMMERCE_ESPIONAGE) * getCultureLevel() * 4; // Civ4 Reimagined
				iTempValue += kBuilding.getCommercePerWorldWonder(COMMERCE_ESPIONAGE) * getNumWorldWonders() * 4; // Civ4 Reimagined

				// Civ4 Reimagind
				if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
				{
					iTempValue += kBuilding.getCommerceFromCoast(COMMERCE_ESPIONAGE) * 4;
				}

				// Civ4 Reimagind
		        if (getMaxAirlift() > 0)
		        {
		            iTempValue += kBuilding.getCommerceFromAirlift(COMMERCE_ESPIONAGE);
		        }

				iTempValue *= 100 + getTotalCommerceRateModifier(COMMERCE_ESPIONAGE) + kBuilding.getCommerceModifier(COMMERCE_ESPIONAGE);
				iValue += iTempValue / 100;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
			}
		}

		/* original bts code
		if ((iThreshold > 0) && (iPass == 0))
		{
			if (iValue < iThreshold)
			{
				iValue = 0;
			}
		} */
		// K-Mod
		if (!bNeutralFlags && iValue <= iThreshold)
		{
			iValue = 0;
			break;
		}
		// K-Mod end
	}

	iValue = std::max(0, iValue);

	// K-Mod
	if (iValue > 0)
	{
		// priority factor
		if (isWorldWonderClass(eBuildingClass))
		{
			//iPriorityFactor += 20; // this could be adjusted based on iWonderConstructRand, or on rival's tech, or whatever...
			// Civ4 Reimagined
			iPriorityFactor += 20 + GC.getLeaderHeadInfo(getPersonalityType()).getWonderConstructRand();
			// Civ4 Reimagined
			if (kOwner.AI_isDoStrategy(AI_STRATEGY_SPECIALIST_ECONOMY))
			{
				iPriorityFactor *= 3;
				iPriorityFactor /= 2;
			}
		}

		if (kBuilding.getProductionCost() > 0)
		{
			iValue *= iPriorityFactor;
			iValue /= 100;
		}

		// Civ4 Reimagined: Use AI weight also for building suggestions
		iValue += kBuilding.getAIWeight();

		// flavour factor. (original flavour code deleted)
		if (!isHuman())
		{
			//if (iValue > 0)
			if (iValue > 0 && kBuilding.getProductionCost() > 0 && !bRemove) // K-Mod. Only use flavour adjustments for constructing ordinary buildings.
			{
				int iFlavour = 0;
				for (int iI = 0; iI < GC.getNumFlavorTypes(); iI++)
				{
					//iValue += (kOwner.AI_getFlavorValue((FlavorTypes)iI) * kBuilding.getFlavorValue(iI));
					iFlavour += std::min(kOwner.AI_getFlavorValue((FlavorTypes)iI), kBuilding.getFlavorValue(iI)); // K-Mod
				}
				iValue = iValue * (10 + iFlavour) / 10; // K-Mod. (This will give +100% for 10-10 flavour matchups.)
			}
		}
	}

	// constructionValue cache
	if (bUseConstructionValueCache && !bConstCache)
	{
		// please don't hate me for using const_cast.
		// The const-correctness was my idea in the first place, but the original code doesn't make it easy for me.
		const_cast<std::vector<int>&>(m_aiConstructionValue)[eBuildingClass] = iValue;
	}
	// K-Mod end

	return iValue;
}


// This function has been significantly modified for K-Mod
ProjectTypes CvCityAI::AI_bestProject(int* piBestValue)
{
	int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);

	int iBestValue = 0;
	ProjectTypes eBestProject = NO_PROJECT;

	for (ProjectTypes i = (ProjectTypes)0; i < GC.getNumProjectInfos(); i = (ProjectTypes)(i+1))
	{
		const CvProjectInfo& kLoopProject = GC.getProjectInfo(i);

		if (!canCreate(i))
			continue; // can't build it. skip to the next project.

		int iTurnsLeft = getProductionTurnsLeft(i, 0);
		int iRelativeTurns = (100 * iTurnsLeft + 50) / GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getCreatePercent();

		if (iRelativeTurns > 10 && kLoopProject.getMaxTeamInstances() > 0 && GET_TEAM(getTeam()).isHuman())
			continue; // not fast enough to risk blocking our human allies from building it.

		if (iRelativeTurns > 20 && iProductionRank > std::max(3, GET_PLAYER(getOwnerINLINE()).getNumCities()/2))
			continue; // not fast enough to risk blocking our more productive cities from building it.

		// otherwise, the project is something we can consider building!

		int iValue = AI_projectValue(i);

		// give a small chance of building global projects, regardless of strategy, just for a bit of variety.
		if ((kLoopProject.getEveryoneSpecialUnit() != NO_SPECIALUNIT) ||
			  (kLoopProject.getEveryoneSpecialBuilding() != NO_SPECIALBUILDING) ||
			  kLoopProject.isAllowsNukes())
		{
			if (GC.getGameINLINE().getSorenRandNum(100, "Project Everyone") == 0)
			{
				iValue++;
			}
		}

		if (iValue <= 0)
			continue; // the project is worthless. Skip it.

		bool bVictory = false;
		bool bGoodFit = false;

		if (GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_SPACE3))
		{
			for (VictoryTypes j = (VictoryTypes)0; j < GC.getNumVictoryInfos(); j = (VictoryTypes)(j+1))
			{
				if (GC.getGameINLINE().isVictoryValid(j) && kLoopProject.getVictoryThreshold(j) > 0)
				{
					bVictory = true;

					// count the total number of projects we still require for this type of victory.
					int iNeededPieces = 0;
					for (ProjectTypes k = (ProjectTypes)0; k < GC.getNumProjectInfos(); k = (ProjectTypes)(k+1))
					{
						// try not to confuse loop project i, with loop project k, or loop victory j...
						iNeededPieces += std::max(0, GC.getProjectInfo(k).getVictoryThreshold(j) - GET_TEAM(getTeam()).getProjectCount(k));
					}

					if (GET_TEAM(getTeam()).getProjectCount(i) < kLoopProject.getVictoryThreshold(j))
					{
						// we need more of this project. ie. it is a high priority.
						FAssert(iNeededPieces > 0);
						bGoodFit = bGoodFit || iProductionRank <= iNeededPieces;
					}
					else
					{
						// build this with high priority, but save our best cities for the projects that we still need.
						bGoodFit = bGoodFit || iProductionRank > iNeededPieces;
					}
				}
			}
		}

		if (!bVictory)
		{
			// work out how many of this project we can still build
			int iLimit = -1;
			if (kLoopProject.getMaxGlobalInstances() >= 0) // global limit
			{
				int iRemaining = std::max(0, kLoopProject.getMaxGlobalInstances() - GC.getGameINLINE().getProjectCreatedCount(i) - GET_TEAM(getTeam()).getProjectMaking(i));
				if (iLimit < 0 || iRemaining < iLimit)
					iLimit = iRemaining;
			}
			if (kLoopProject.getMaxTeamInstances() >= 0) // team limit
			{
				int iRemaining = std::max(0, kLoopProject.getMaxTeamInstances() - GET_TEAM(getTeam()).getProjectCount(i) - GET_TEAM(getTeam()).getProjectMaking(i));
				if (iLimit < 0 || iRemaining < iLimit)
					iLimit = iRemaining;
			}
			bGoodFit = iProductionRank <= iLimit;
		}

		if (bGoodFit)
		{
			// building the project in this city is probably a good idea.
			iValue += getProjectProduction(i) + (bVictory ? getProductionNeeded(i)/4 + iValue/2 : 0);
		}
		else
		{
			if (bVictory)
			{
				iValue *= 3;
				iValue /= iRelativeTurns + 3;
			}
			else
			{
				iValue *= 5;
				iValue /= iRelativeTurns + 5;
			}
		}

		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			eBestProject = i;
		}
	}

	if (piBestValue) // note: piBestValue is set even if there is no best project.
		*piBestValue = iBestValue;

	return eBestProject;
}

// This function has been completely rewriten for K-Mod
// The return value is roughly in units of 4 * commerce per turn, to match AI_buildingValue.
// However, note that most projects don't actually give commerce per turn - so the evaluation is quite rough.
int CvCityAI::AI_projectValue(ProjectTypes eProject)
{
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	const CvTeam& kTeam = GET_TEAM(kOwner.getTeam());
	const CvProjectInfo& kProject = GC.getProjectInfo(eProject);

	int iValue = 0;

	if (kProject.getTechShare() > 0)
	{
		if (kProject.getTechShare() < GET_TEAM(getTeam()).getHasMetCivCount(true) && !kOwner.AI_avoidScience())
		{
			TechTypes eSampleTech = kOwner.getCurrentResearch();

			if (eSampleTech == NO_TECH)
			{
				// what a pest...
				for (TechTypes i = (TechTypes)0; i < GC.getNumTechInfos(); i = (TechTypes)(i+1))
				{
					if (kTeam.isHasTech(i) && (eSampleTech == NO_TECH || GC.getTechInfo(eSampleTech).getResearchCost() < GC.getTechInfo(i).getResearchCost()))
						eSampleTech =  i;
				}
			}
			FAssert(eSampleTech != NO_TECH);
			if (eSampleTech != NO_TECH)
			{
				int iRelativeTechScore = kTeam.getBestKnownTechScorePercent();
				int iTechValue = 50 * kTeam.getResearchCost(eSampleTech) / std::max(1, iRelativeTechScore);
				iTechValue = 100 * iTechValue / std::max(1, iRelativeTechScore); // again, for emphasis.
				iTechValue *= (2 * GC.getNumEraInfos() - kOwner.getCurrentEra());
				iTechValue /= std::max(1, 2 * GC.getNumEraInfos() - GC.getGameINLINE().getStartEra());
				iValue += iTechValue;
			}
			//iValue += (20 / GC.getProjectInfo(eProject).getTechShare());
		}
	}

	// SDI
	if (kProject.getNukeInterception() > 0)
	{
		int iForeignNukes = 0; // we're going to cheat a little bit, by counting nukes that we probably shouldn't know about.
		int iTeamsMet = 0;
		for (PlayerTypes i = (PlayerTypes)0; i < MAX_CIV_PLAYERS; i = (PlayerTypes)(i + 1))
		{
			const CvPlayerAI& kLoopPlayer = GET_PLAYER(i);
			if (kLoopPlayer.getTeam() != kOwner.getTeam() && kTeam.isHasMet(kLoopPlayer.getTeam()))
			{
				iForeignNukes += kLoopPlayer.getNumNukeUnits();
				iTeamsMet++;
			}
		}
		if (!GC.getGameINLINE().isNoNukes() || iForeignNukes > iTeamsMet)
		{
			int iTargetValue = 10 + (getYieldRate(YIELD_PRODUCTION) * 5 + getYieldRate(YIELD_COMMERCE) * 3)/2; // a very rough estimate of the cost of being nuked
			int iEstimatedNukeAttacks = iForeignNukes * (2 + iTeamsMet) / (2 + 2*iTeamsMet) + (GC.getGameINLINE().isNoNukes() ? 0 : 2 + GC.getGameINLINE().getNukesExploded() / (2 + iTeamsMet));
			iValue += kProject.getNukeInterception() * iEstimatedNukeAttacks * iTargetValue / 100;
		}
		//iValue += (GC.getProjectInfo(eProject).getNukeInterception() / 10);
	}
	// Manhattan project
	if (kProject.isAllowsNukes() && !GC.getGameINLINE().isNoNukes())
	{
		// evaluating this is difficult, because it enables the nukes for enemies.
		// In general, I want the AI to lean in favour of -not- building the Manhattan project, so that the human players generally get to decide whether or not the game will have nukes.
		// But I do want the AI to build it if it will be particular adventagious for them. eg. when they want a conquest victory, and they know that their enemies don't have uranium...
		if (kOwner.AI_isDoStrategy(AI_STRATEGY_CRUSH | AI_STRATEGY_DAGGER) || kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST4))
		{
			int iNukeValue = 0;

			for (UnitTypes i = (UnitTypes)0; i < GC.getNumUnitInfos(); i = (UnitTypes)(i+1))
			{
				const CvUnitInfo& kLoopUnit = GC.getUnitInfo(i);

				if (kLoopUnit.getNukeRange() < 0 || kLoopUnit.getProductionCost() < 0)
					continue; // either not a unit, or not normally buildable

				for (PlayerTypes j = (PlayerTypes)0; j < MAX_CIV_PLAYERS; j = (PlayerTypes)(j+1))
				{
					const CvPlayer& kLoopPlayer = GET_PLAYER(j);
					if (kLoopPlayer.isAlive() && GC.getCivilizationInfo(kLoopPlayer.getCivilizationType()).getCivilizationUnits(kLoopUnit.getUnitClassType()) == i &&
						(kLoopPlayer.getTeam() == kOwner.getTeam() || kTeam.isHasMet(kLoopPlayer.getTeam())))
					{
						const CvTeam& kLoopTeam = GET_TEAM(kLoopPlayer.getTeam());
						int iTemp;

						if (kLoopPlayer.getID() == kOwner.getID())
							iTemp = 85 + GC.getLeaderHeadInfo(kOwner.getPersonalityType()).getConquestVictoryWeight()/2; // victory weight is between 0 and 100. (usually around 30).
						else if (kLoopPlayer.getTeam() == kOwner.getTeam())
							iTemp = 90;
						else if (kTeam.AI_getWarPlan(kLoopPlayer.getTeam()) != NO_WARPLAN)
							iTemp = -100;
						else
							iTemp = std::max(-100, (kOwner.AI_getAttitudeWeight(j) - 125)/2);

						// tech prereqs.  reduce the value for each missing prereq
						if (!kLoopTeam.isHasTech((TechTypes)(kLoopUnit.getPrereqAndTech())))
						{
							iTemp /= 2;
							if (!kLoopPlayer.canResearch((TechTypes)kLoopUnit.getPrereqAndTech()))
								iTemp /= 3;
						}

						for (int k = 0; k < GC.getNUM_UNIT_AND_TECH_PREREQS(); k++)
						{
							TechTypes ePrereqTech = (TechTypes)kLoopUnit.getPrereqAndTechs(k);
							if (ePrereqTech != NO_TECH && !kLoopTeam.isHasTech(ePrereqTech))
							{
								iTemp /= 2;
								if (!kLoopPlayer.canResearch(ePrereqTech))
									iTemp /= 3;
							}
						}

						// resource prereq.
						BonusTypes ePrereqBonus = (BonusTypes)kLoopUnit.getPrereqAndBonus();
						if (ePrereqBonus != NO_BONUS && !kLoopPlayer.hasBonus(ePrereqBonus) && kLoopPlayer.countOwnedBonuses(ePrereqBonus) == 0)
						{
							iTemp /= 5;
						}

						iTemp *= 3*kLoopPlayer.getPower();
						iTemp /= std::max(1, 2*kOwner.getPower()+kLoopPlayer.getPower());

						iNukeValue += iTemp;
					}
				}
			}
			if (iNukeValue > 0)
			{
				// ok. At this point, the scale of iNukeValue is roughly a percentage of the number of different nuke units which would be helpful to us... kind of.
				// It's very likely to be to be less than 100. In a situation where nukes would be very helpful, I estimate that iNukeValue would currently be around 30.

				// I'm just going to do a very rough job or rescaling it to be more like the other project value.
				// But first, I want to make a few more situational adjustments to the value.
				iNukeValue *= (2 + kTeam.getAnyWarPlanCount(true));
				iNukeValue /= 2;

				//
				iNukeValue *= 10 + (getYieldRate(YIELD_PRODUCTION) * 5 + getYieldRate(YIELD_COMMERCE) * 3)/2; // cf. iTargetValue in the SDI section.
				iNukeValue /= 25;
				iValue += iNukeValue;
			}
		}
	}

	// Space victory
	// How am I meant to gauge the value of a direct step towards victory? It just doesn't conform to the usual metrics...
	// this is going to be very arbitrary...  -- and it will be based on the original BtS code!
	int iSpaceValue = 0;

	// a project which enables other projects... We're talking about the Apolo Program
	for (int iI = 0; iI < GC.getNumProjectInfos(); iI++)
	{
		iSpaceValue += (std::max(0, (GC.getProjectInfo((ProjectTypes)iI).getProjectsNeeded(eProject) - GET_TEAM(getTeam()).getProjectCount(eProject))) * 8); // was *10
	}
	if (kOwner.AI_isDoVictoryStrategy(AI_VICTORY_SPACE1))
		iSpaceValue = 3*iSpaceValue/2; // a boost to compound with the other boosts lower down.

	// projects which are required components for victory. (ie. components of the spaceship)
	for (int iI = 0; iI < GC.getNumVictoryInfos(); iI++)
	{
		if (GC.getGameINLINE().isVictoryValid((VictoryTypes)iI) && kProject.getVictoryThreshold(iI) > 0)
		{
			/* iSpaceValue += 20;
			iSpaceValue += std::max(0, kProject.getVictoryThreshold(iI) - GET_TEAM(getTeam()).getProjectCount(eProject)) * 20; */
			iSpaceValue += 15;
			iSpaceValue += std::max(0, kProject.getVictoryMinThreshold(iI) - GET_TEAM(getTeam()).getProjectCount(eProject)) * (kOwner.AI_isDoVictoryStrategy(AI_VICTORY_SPACE4) ? 60 : 30);
			iSpaceValue += kProject.getSuccessRate();
			iSpaceValue += kProject.getVictoryDelayPercent() / (4 * kProject.getVictoryThreshold(iI));
			//
		}
	}

	if (kOwner.AI_isDoVictoryStrategy(AI_VICTORY_SPACE4))
		iSpaceValue *= 4;
	else if (kOwner.AI_isDoVictoryStrategy(AI_VICTORY_SPACE3))
		iSpaceValue *= 3;
	else if (kOwner.AI_isDoVictoryStrategy(AI_VICTORY_SPACE2))
		iSpaceValue *= 2;
	else if (!kOwner.AI_isDoVictoryStrategy(AI_VICTORY_SPACE1) && kOwner.AI_isDoVictoryStrategyLevel4())
		iSpaceValue = 2*iSpaceValue/3;

	if (area()->getAreaAIType(kOwner.getTeam()) != AREAAI_NEUTRAL)
		iSpaceValue = area()->getAreaAIType(kOwner.getTeam()) == AREAAI_DEFENSIVE ? iSpaceValue/2 : 2*iSpaceValue/3;

	iValue += iSpaceValue;

	return iValue;
}

ProcessTypes CvCityAI::AI_bestProcess(CommerceTypes eCommerceType) const
{
	int iBestValue = 0;
	ProcessTypes eBestProcess = NO_PROCESS;

	for (int iI = 0; iI < GC.getNumProcessInfos(); iI++)
	{
		if (canMaintain((ProcessTypes)iI))
		{
			int iValue = AI_processValue((ProcessTypes)iI, eCommerceType);

			if (iValue > iBestValue)
			{
				iBestValue = iValue;
				eBestProcess = ((ProcessTypes)iI);
			}
		}
	}

	return eBestProcess;
}

// K-Mod. I've rearranged / rewriten most of this function.
// units of ~4x commerce
int CvCityAI::AI_processValue(ProcessTypes eProcess, CommerceTypes eCommerceType) const
{
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	bool bValid = (eCommerceType == NO_COMMERCE);
	
	int iValue = 0;

	/* if (GC.getProcessInfo(eProcess).getProductionToCommerceModifier(COMMERCE_GOLD) && GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble())
	{
		iValue += GC.getProcessInfo(eProcess).getProductionToCommerceModifier(COMMERCE_GOLD);
	} */

	// pop borders
	if (getCultureLevel() <= 1)
	{
		iValue += GC.getProcessInfo(eProcess).getProductionToCommerceModifier(COMMERCE_CULTURE);
	}

	int iAdjustFactor = 0;
	for (CommerceTypes i = (CommerceTypes)0; i < NUM_COMMERCE_TYPES; i = (CommerceTypes)(i+1))
	{
		iAdjustFactor += kOwner.getCommercePercent(i) * kOwner.AI_averageCommerceMultiplier(i);
	}
	iAdjustFactor /= 100;

	for (CommerceTypes i = (CommerceTypes)0; i < NUM_COMMERCE_TYPES; i = (CommerceTypes)(i+1))
	{
		int iTempValue = GC.getProcessInfo(eProcess).getProductionToCommerceModifier(i);

		if (iTempValue == 0)
			continue;

		if (i == eCommerceType && iTempValue > 0)
		{
			bValid = true;
			iTempValue *= 2;
		}

		// K-Mod. Calculate the quantity of commerce produced.
		iTempValue *= getYieldRate(YIELD_PRODUCTION);
		//iTempValue /= 100; // keep this factor of 100 for now.
		// Culture is local, the other commerce types are non-local.
		// We don't want the non-local commerceWeights in this function, because maintaining a process is just a short-term arrangement.
		iTempValue *= kOwner.AI_commerceWeight(i, i == COMMERCE_CULTURE ? this : 0);
		iTempValue /= 100;
		// K-Mod end

		/* iTempValue *= GET_PLAYER(getOwnerINLINE()).AI_averageCommerceExchange(i);
		iTempValue /= 60; */
		// K-Mod. Amplify the value of commerce processes with low average multipliers, so that we can run higher percentages in commerce types with high average multipliers.
		if (kOwner.isCommerceFlexible(i) && kOwner.getCommercePercent(i) > 0)
		{
			/* iTempValue *= 100 + kOwner.getCommercePercent(i) * (iAdjustFactor - 100) / 100;
			iTempValue /= std::max(100, 100 + kOwner.getCommercePercent(i) * (kOwner.AI_averageCommerceMultiplier(i) - 100) / 100); */
			// (that wasn't a strong enough effect)
			iTempValue *= iAdjustFactor * std::max(100, 200 - 2*kOwner.getCommercePercent(i)) / 100;
			iTempValue /= std::max(100, kOwner.AI_averageCommerceMultiplier(i) + iAdjustFactor * std::max(0, 100 - kOwner.getCommercePercent(i)) / 100);
		}
		// K-Mod end

		iValue += iTempValue;
	}

	// note, currently iValue has units of 100x commerce. We want to return 4x commerce.
	return (bValid ? iValue/25 : 0);
}


int CvCityAI::AI_neededSeaWorkers()
{
	CvArea* pWaterArea;
	int iNeededSeaWorkers = 0;

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      01/01/09                                jdog5000      */
/*                                                                                              */
/* Worker AI                                                                                    */
/************************************************************************************************/
	pWaterArea = waterArea(true);
	
	if (pWaterArea == NULL)
	{
		return 0;
	}

	iNeededSeaWorkers += GET_PLAYER(getOwnerINLINE()).countUnimprovedBonuses(pWaterArea, plot());

	// Check if second water area city can reach was any unimproved bonuses
	pWaterArea = secondWaterArea();
	if (pWaterArea != NULL)
	{
		iNeededSeaWorkers += GET_PLAYER(getOwnerINLINE()).countUnimprovedBonuses(pWaterArea, plot());
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	return iNeededSeaWorkers;
}


bool CvCityAI::AI_isDefended(int iExtra)
{
	PROFILE_FUNC();

	return ((plot()->plotCount(PUF_canDefendGroupHead, -1, -1, getOwnerINLINE(), NO_TEAM, PUF_isCityAIType) + iExtra) >= AI_neededDefenders()); // XXX check for other team's units?
}

/* original BTS code
bool CvCityAI::AI_isAirDefended(int iExtra)
{
	PROFILE_FUNC();

	return ((plot()->plotCount(PUF_canAirDefend, -1, -1, getOwnerINLINE(), NO_TEAM, PUF_isDomainType, DOMAIN_AIR) + iExtra) >= AI_neededAirDefenders()); // XXX check for other team's units?
} */

/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						10/17/08		jdog5000		*/
/* 																			*/
/* 	Air AI																	*/
/********************************************************************************/
// Function now answers question of whether city has enough ready air defense, no longer just counts fighters
/* bool CvCityAI::AI_isAirDefended(bool bCountLand, int iExtra)
{
	PROFILE_FUNC();
	
	int iAirDefenders = iExtra;
	int iAirIntercept = 0;
	int iLandIntercept = 0;

	CvUnit* pLoopUnit;
	CLLNode<IDInfo>* pUnitNode = plot()->headUnitNode();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = plot()->nextUnitNode(pUnitNode);

		if ((pLoopUnit->getOwnerINLINE() == getOwnerINLINE()))
		{
			if ( pLoopUnit->canAirDefend() )
			{
				if( pLoopUnit->getDomainType() == DOMAIN_AIR )
				{
					// can find units which are already air patrolling using group activity
					if( pLoopUnit->getGroup()->getActivityType() == ACTIVITY_INTERCEPT )
					{
						iAirIntercept += pLoopUnit->currInterceptionProbability();
					}
					else
					{
						// Count air units which can air patrol
						if( pLoopUnit->getDamage() == 0 && !pLoopUnit->hasMoved() )
						{
							if( pLoopUnit->AI_getUnitAIType() == UNITAI_DEFENSE_AIR )
							{
								iAirIntercept += pLoopUnit->currInterceptionProbability();
							}
							else
							{
								iAirIntercept += pLoopUnit->currInterceptionProbability()/3;
							}
						}

					}
				}
				else if( pLoopUnit->getDomainType() == DOMAIN_LAND )
				{
					iLandIntercept += pLoopUnit->currInterceptionProbability();
				}
			}
		}
	}

	iAirDefenders += (iAirIntercept/100);

	if( bCountLand )
	{
		iAirDefenders += (iLandIntercept/100);
	}

	int iNeededAirDefenders = AI_neededAirDefenders();
	bool bHaveEnough = (iAirDefenders >= iNeededAirDefenders);

	return bHaveEnough;
} */ // disabled by K-Mod
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END								*/
/********************************************************************************/

// K-Mod. I've reverted AI_isAirDefended back to its original simple functionality.
// I've done this because my new version of AI_neededAirDefenders is not suitable for use in the bbai version of AI_isAirDefended.
// I may put some more work into this stuff in the future if I ever work through CvUnitAI::AI_defenseAirMove
bool CvCityAI::AI_isAirDefended(bool bCountLand, int iExtra) // function signature changed to match bbai usage.
{
	PROFILE_FUNC();

	return plot()->plotCount(PUF_isAirIntercept, -1, -1, getOwnerINLINE()) + iExtra >= AI_neededAirDefenders();
}


/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      04/25/10                                jdog5000      */
/*                                                                                              */
/* War strategy AI, Barbarian AI                                                                */
/************************************************************************************************/
int CvCityAI::AI_neededDefenders()
{
	PROFILE_FUNC();
	int iDefenders;
	bool bOffenseWar = ((area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING));
	bool bDefenseWar = ((area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE));
	
	if (!(GET_TEAM(getTeam()).AI_isWarPossible()))
	{
		return 1;
	}

	if (isBarbarian())
	{
		iDefenders = GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getBarbarianInitialDefenders();
		iDefenders += ((getPopulation() + 2) / 7);
		return iDefenders;
	}

	iDefenders = 1;
	
	if (hasActiveWorldWonder() || isCapital() || isHolyCity())
	{
		iDefenders++;

		if( GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_ALERT1) || GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_TURTLE) )
		{
			iDefenders++;
		}
	}
	
	if (!GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_CRUSH))
	{
		iDefenders += AI_neededFloatingDefenders();
	}
	else
	{
		iDefenders += (AI_neededFloatingDefenders() + 2) / 4;
	}
	
	if (bDefenseWar || GET_PLAYER(getOwner()).AI_isDoStrategy(AI_STRATEGY_ALERT2))
	{
		if (!(plot()->isHills()))
		{
			iDefenders++;
		}
	}
	
	if ((GC.getGame().getGameTurn() - getGameTurnAcquired()) < 10)
	{
/* original code
		if (bOffenseWar)
		{
			if (!hasActiveWorldWonder() && !isHolyCity())
			{
				iDefenders /= 2;
				iDefenders = std::max(1, iDefenders);
			}
		}
	}
	
	if (GC.getGame().getGameTurn() - getGameTurnAcquired() < 10)
	{
		iDefenders = std::max(2, iDefenders);
		if (AI_isDanger())
		{
			iDefenders ++;
		}
		if (bDefenseWar)
		{
			iDefenders ++;
		}
*/
		iDefenders = std::max(2, iDefenders);

		if (bOffenseWar && getTotalDefense(true) > 0)
		{
			if (!hasActiveWorldWonder() && !isHolyCity())
			{
				iDefenders /= 2;
			}
		}
		
		if (AI_isDanger())
		{
			iDefenders++;
		}
		if (bDefenseWar)
		{
			iDefenders++;
		}
	}
	
	if (GET_PLAYER(getOwnerINLINE()).AI_isDoStrategy(AI_STRATEGY_LAST_STAND))
	{
		iDefenders += 10;
	}

	if( GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3) )
	{
		if( findCommerceRateRank(COMMERCE_CULTURE) <= GC.getGameINLINE().culturalVictoryNumCultureCities() )
		{
			iDefenders += 4;

			if( bDefenseWar )
			{
				iDefenders += 2;
			}
		}
	}

	if( GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_SPACE3) )
	{
		if( isCapital() || isProductionProject())
		{
			iDefenders += 4;

			if( bDefenseWar )
			{
				iDefenders += 3;
			}
		}

		if( isCapital() && GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_SPACE4) )
		{
			iDefenders += 6;
		}
	}
	
	iDefenders = std::max(iDefenders, AI_minDefenders());

	return iDefenders;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

int CvCityAI::AI_minDefenders()
{
	int iDefenders = 1;
	int iEra = GET_PLAYER(getOwnerINLINE()).getCurrentEra();
	if (iEra > 0)
	{
		iDefenders++;
	}
	if (((iEra - GC.getGame().getStartEra() / 2) >= GC.getNumEraInfos() / 2) && isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		iDefenders++;
	}
	
	return iDefenders;
}
	
int CvCityAI::AI_neededFloatingDefenders()
{
	if (m_iNeededFloatingDefendersCacheTurn != GC.getGame().getGameTurn())
	{
		AI_updateNeededFloatingDefenders();
	}
	return m_iNeededFloatingDefenders;	
}

void CvCityAI::AI_updateNeededFloatingDefenders()
{
	int iFloatingDefenders = GET_PLAYER(getOwnerINLINE()).AI_getTotalFloatingDefendersNeeded(area());
		
	int iTotalThreat = std::max(1, GET_PLAYER(getOwnerINLINE()).AI_getTotalAreaCityThreat(area()));
	
	iFloatingDefenders -= area()->getCitiesPerPlayer(getOwnerINLINE());
	
	iFloatingDefenders *= AI_cityThreat();
	iFloatingDefenders += (iTotalThreat / 2);
	iFloatingDefenders /= iTotalThreat;
	
	m_iNeededFloatingDefenders = iFloatingDefenders;
	m_iNeededFloatingDefendersCacheTurn = GC.getGame().getGameTurn();
}

// This function has been completely rewritten for K-Mod. (The original code has been deleted.)
// My version is still very simplistic, but it has the advantage of being consistent with other AI calculations.
int CvCityAI::AI_neededAirDefenders()
{
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	if (!GET_TEAM(kOwner.getTeam()).AI_isWarPossible())
	{
		return 0;
	}

	const int iRange = 5;

	// Essentially I'm going to bootstrap off the hard work already done in AI_neededFloatingDefenders.
	// We'll just count the number of floating defenders needed in nearby cities; and use this to
	// calculate what proportion of our airforce we should deploy here.
	int iNearbyFloaters = 0;
	int iNearbyCities = 0;
	int iTotalFloaters = 0;

	// Note: checking the nearby plots directly to look for cities may scale better than this
	//       on very large maps; but that would make it harder to work out iTotalFloaters.
	int iLoop;
	for (CvCity* pLoopCity = kOwner.firstCity(&iLoop); pLoopCity; pLoopCity = kOwner.nextCity(&iLoop))
	{
		// static_cast to get around weird code structure...
		int iFloaters = static_cast<CvCityAI*>(pLoopCity)->AI_neededFloatingDefenders();

		iTotalFloaters += iFloaters;
		if (plotDistance(plot(), pLoopCity->plot()) <= iRange)
		{
			iNearbyCities++;
			iNearbyFloaters += iFloaters;
			// Note: floaters for this city really should be divided by the number of cities near
			// pLoopCity rather than thenumber near 'this'. But that would be a longer calculation.
		}
	}
	FAssert(iNearbyCities > 0);
	// 'iNearbyFloaters / iTotalFloaters' is the proportion of our air defenders we should use in this city.
	// ie. a proportion of the total air defenders for our civ we already have or expect to have.
	int iNeeded = (std::max(kOwner.AI_getTotalAirDefendersNeeded(), kOwner.AI_getNumAIUnits(UNITAI_DEFENSE_AIR)) * iNearbyFloaters + (iNearbyCities * iTotalFloaters)/2) / std::max(1, iNearbyCities * iTotalFloaters);
	return std::min(iNeeded, getAirUnitCapacity(kOwner.getTeam())); // Capped at the air capacity of the city.

	//Note: because of the air capacity cap, and rounding, and the shortcut used for the nearbyFloater averaging,
	// the sum of AI_neededAirDefenders for each city won't necessarily equal our intended total.
}

bool CvCityAI::AI_isDanger()
{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/20/09                                jdog5000      */
/*                                                                                              */
/* City AI, Efficiency                                                                          */
/************************************************************************************************/
	//return GET_PLAYER(getOwnerINLINE()).AI_getPlotDanger(plot(), 2, false);
	return GET_PLAYER(getOwnerINLINE()).AI_getAnyPlotDanger(plot(), 2, false);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/	
}


int CvCityAI::AI_getEmphasizeAvoidGrowthCount() const
{
	return m_iEmphasizeAvoidGrowthCount;
}


bool CvCityAI::AI_isEmphasizeAvoidGrowth() const
{
	return (AI_getEmphasizeAvoidGrowthCount() > 0);
}


int CvCityAI::AI_getEmphasizeGreatPeopleCount() const
{
	return m_iEmphasizeGreatPeopleCount;
}


bool CvCityAI::AI_isEmphasizeGreatPeople() const
{
	return (AI_getEmphasizeGreatPeopleCount() > 0);
}


bool CvCityAI::AI_isAssignWorkDirty() const
{
	return m_bAssignWorkDirty;
}


void CvCityAI::AI_setAssignWorkDirty(bool bNewValue)
{
	m_bAssignWorkDirty = bNewValue;
}


bool CvCityAI::AI_isChooseProductionDirty() const
{
	return m_bChooseProductionDirty;
}


void CvCityAI::AI_setChooseProductionDirty(bool bNewValue)
{
	m_bChooseProductionDirty = bNewValue;
}


CvCity* CvCityAI::AI_getRouteToCity() const
{
	return getCity(m_routeToCity);
}


void CvCityAI::AI_updateRouteToCity()
{
	CvCity* pLoopCity;
	CvCity* pBestCity;
	int iValue;
	int iBestValue;
	int iLoop;
	int iI;

	gDLL->getFAStarIFace()->ForceReset(&GC.getRouteFinder());

	iBestValue = MAX_INT;
	pBestCity = NULL;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
		{
			for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
			{
				if (pLoopCity != this)
				{
					if (pLoopCity->area() == area())
					{
						if (!(gDLL->getFAStarIFace()->GeneratePath(&GC.getRouteFinder(), getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), false, getOwnerINLINE(), true)))
						{
							iValue = plotDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());

							if (iValue < iBestValue)
							{
								iBestValue = iValue;
								pBestCity = pLoopCity;
							}
						}
					}
				}
			}
		}
	}

	if (pBestCity != NULL)
	{
		m_routeToCity = pBestCity->getIDInfo();
	}
	else
	{
		m_routeToCity.reset();
	}
}


int CvCityAI::AI_getEmphasizeYieldCount(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiEmphasizeYieldCount[eIndex];
}


bool CvCityAI::AI_isEmphasizeYield(YieldTypes eIndex) const
{
	return (AI_getEmphasizeYieldCount(eIndex) > 0);
}


int CvCityAI::AI_getEmphasizeCommerceCount(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return (m_aiEmphasizeCommerceCount[eIndex] > 0);
}


bool CvCityAI::AI_isEmphasizeCommerce(CommerceTypes eIndex) const
{
	return (AI_getEmphasizeCommerceCount(eIndex) > 0);
}


bool CvCityAI::AI_isEmphasize(EmphasizeTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumEmphasizeInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_pbEmphasize != NULL, "m_pbEmphasize is not expected to be equal with NULL");
	return m_pbEmphasize[eIndex];
}


void CvCityAI::AI_setEmphasize(EmphasizeTypes eIndex, bool bNewValue)
{
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumEmphasizeInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (AI_isEmphasize(eIndex) != bNewValue)
	{
		m_pbEmphasize[eIndex] = bNewValue;

		if (GC.getEmphasizeInfo(eIndex).isAvoidGrowth())
		{
			m_iEmphasizeAvoidGrowthCount += ((AI_isEmphasize(eIndex)) ? 1 : -1);
			FAssert(AI_getEmphasizeAvoidGrowthCount() >= 0);
		}

		if (GC.getEmphasizeInfo(eIndex).isGreatPeople())
		{
			m_iEmphasizeGreatPeopleCount += ((AI_isEmphasize(eIndex)) ? 1 : -1);
			FAssert(AI_getEmphasizeGreatPeopleCount() >= 0);
		}

		for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			if (GC.getEmphasizeInfo(eIndex).getYieldChange(iI))
			{
				m_aiEmphasizeYieldCount[iI] += ((AI_isEmphasize(eIndex)) ? 1 : -1);
				FAssert(AI_getEmphasizeYieldCount((YieldTypes)iI) >= 0);
			}
		}

		for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			if (GC.getEmphasizeInfo(eIndex).getCommerceChange(iI))
			{
				m_aiEmphasizeCommerceCount[iI] += ((AI_isEmphasize(eIndex)) ? 1 : -1);
				FAssert(AI_getEmphasizeCommerceCount((CommerceTypes)iI) >= 0);
			}
		}

		AI_assignWorkingPlots();

		if ((getOwnerINLINE() == GC.getGameINLINE().getActivePlayer()) && isCitySelected())
		{
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
		}
	}
}

void CvCityAI::AI_forceEmphasizeCulture(bool bNewValue)
{
	if (m_bForceEmphasizeCulture != bNewValue)
	{
		m_bForceEmphasizeCulture = bNewValue;

		m_aiEmphasizeCommerceCount[COMMERCE_CULTURE] += (bNewValue ? 1 : -1);
		FAssert(m_aiEmphasizeCommerceCount[COMMERCE_CULTURE] >= 0);
	}
}


int CvCityAI::AI_getBestBuildValue(int iIndex)
{
	FAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(iIndex < NUM_CITY_PLOTS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiBestBuildValue[iIndex];
}


int CvCityAI::AI_totalBestBuildValue(CvArea* pArea)
{
	CvPlot* pLoopPlot;
	int iTotalValue;
	int iI;

	iTotalValue = 0;

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->area() == pArea)
				{
					if ((pLoopPlot->getImprovementType() == NO_IMPROVEMENT) || !(GET_PLAYER(getOwnerINLINE()).isOption(PLAYEROPTION_SAFE_AUTOMATION) && !(pLoopPlot->getImprovementType() == (GC.getDefineINT("RUINS_IMPROVEMENT")))))
					{
						iTotalValue += AI_getBestBuildValue(iI);
					}
				}
			}
		}
	}

	return iTotalValue;
}

int CvCityAI::AI_clearFeatureValue(int iIndex)
{
	CvPlot* pPlot = plotCity(getX_INLINE(), getY_INLINE(), iIndex);
	FAssert(pPlot != NULL);
	
	FeatureTypes eFeature = pPlot->getFeatureType();
	FAssert(eFeature != NO_FEATURE);
	
	CvFeatureInfo& kFeatureInfo = GC.getFeatureInfo(eFeature);
	
	/* original bts code
	int iValue = 0;
	iValue += kFeatureInfo.getYieldChange(YIELD_FOOD) * 100;
	iValue += kFeatureInfo.getYieldChange(YIELD_PRODUCTION) * 60;
	iValue += kFeatureInfo.getYieldChange(YIELD_COMMERCE) * 40;
	
	if (iValue > 0 && pPlot->isBeingWorked())
	{
		iValue *= 3;
		iValue /= 2;
	}
	if (iValue != 0)
	{
		BonusTypes eBonus = pPlot->getBonusType(getTeam());
		if (eBonus != NO_BONUS)
		{
			iValue *= 3;
			if (pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				if (GC.getImprovementInfo(pPlot->getImprovementType()).isImprovementBonusTrade(eBonus))
				{
					iValue *= 4;
				}
			}
		}
	} */
	// K-Mod. All that yield change stuff is taken into account by the improvement evaluation function anyway.
	// ... except the bit about keeping good features on top of bonuses
	int iValue = 0;
	BonusTypes eBonus = pPlot->getNonObsoleteBonusType(getTeam());
	if (eBonus != NO_BONUS && !GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getBonusInfo(eBonus).getTechCityTrade()))
	{
		iValue += kFeatureInfo.getYieldChange(YIELD_FOOD) * 100;
		iValue += kFeatureInfo.getYieldChange(YIELD_PRODUCTION) * 80; // was 60
		iValue += kFeatureInfo.getYieldChange(YIELD_COMMERCE) * 40;
		iValue *= 2;
		// that should be enough incentive to keep good features until we have the tech to decide on the best improvement.
	}
	// K-Mod end
	
	int iHealthValue = 0;
	if (kFeatureInfo.getHealthPercent() != 0)
	{
		int iHealth = goodHealth() - badHealth();
		
		/* original bts code
		iHealthValue += (6 * kFeatureInfo.getHealthPercent()) / std::max(3, 1 + iHealth);
		if (iHealthValue > 0 && !pPlot->isBeingWorked())
		{
			iHealthValue *= 3;
			iHealthValue /= 2;
		} */
		// K-Mod -
		//iHealthValue += (iHealth < 0 ? 100 : 400/(4+iHealth)) + 100 * pPlot->getPlayerCityRadiusCount(getOwnerINLINE());
		// Civ4 Reimagined: Stop overvaluing forests!
		iHealthValue += (iHealth < 0 ? 50 : 200/(4+iHealth)) + 50 * pPlot->getPlayerCityRadiusCount(getOwnerINLINE());
		iHealthValue *= kFeatureInfo.getHealthPercent();
		iHealthValue /= 100;
		// note: health is not any more valuable when we aren't working it.
		// That kind of thing should be handled by the chop code.
		// K-Mod end
	}
	iValue += iHealthValue;

	// K-Mod
	// We don't want defensive features adjacent to our city
	if (iIndex <= 8)
	{
		iValue -= kFeatureInfo.getDefenseModifier()/2;
	}

	if (GC.getGame().getGwEventTally() >= 0) // if GW Threshold has been reached
	{
		iValue += kFeatureInfo.getWarmingDefense() * (150 + 5 * GET_PLAYER(getOwner()).getGwPercentAnger()) / 100;
	}
	// K-Mod end
	
	if (iValue > 0)
	{
		if (pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(pPlot->getImprovementType()).isRequiresFeature())
			{
				iValue += 500;
			}
		}
		
		if (GET_PLAYER(getOwnerINLINE()).getAdvancedStartPoints() >= 0)
		{
			iValue += 400;
		}
	}
	
	return -iValue;
}

// K-Mod. if aiYields == 0, then it will be calculated based on the 'best build' of the plot.
bool CvCityAI::AI_isGoodPlot(int iPlot, int* aiYields) const
{
	int tempArray[NUM_YIELD_TYPES];

	FAssert(iPlot != CITY_HOME_PLOT); // it doesn't matter if this assert is wrong. I just don't expect to ever want this for the center plot.
	CvPlot* pPlot = getCityIndexPlot(iPlot);

	if (!pPlot || pPlot->getWorkingCity() != this)
	{
		FAssertMsg(aiYields == 0, "yields specified for non-existent plot");
		return false;
	}

	if (aiYields == 0)
	{
		aiYields = tempArray;

		BuildTypes eBuild = NO_BUILD;
		if (m_aeBestBuild[iPlot] != NO_BUILD && m_aiBestBuildValue[iPlot] > 50)
			eBuild = m_aeBestBuild[iPlot];

		if (eBuild != NO_BUILD)
		{
			for (YieldTypes y = (YieldTypes)0; y < NUM_YIELD_TYPES; y=(YieldTypes)(y+1))
			{
				aiYields[y] = pPlot->getYieldWithBuild(eBuild, y, true);
			}
		}
		else
		{
			for (YieldTypes y = (YieldTypes)0; y < NUM_YIELD_TYPES; y=(YieldTypes)(y+1))
			{
				aiYields[y] = pPlot->getYield(y);
			}
		}
	}

	FAssert((1+GC.getFOOD_CONSUMPTION_PER_POPULATION())*10 > 27); // the numbers used here are arbitrary; and I want to make sure high food tiles count as 'good'.
	return aiYields[YIELD_FOOD]*10 + aiYields[YIELD_PRODUCTION]*7 + aiYields[YIELD_COMMERCE]*4 > (pPlot->isWater() ? 27 : 21);
}
// K-Mod end

// K-Mod rewriten to use my new function - AI_isGoodPlot.
int CvCityAI::AI_countGoodPlots() const
{
	int iCount = 0;

	for (int i = 1; i < NUM_CITY_PLOTS; i++)
	{
		iCount += AI_isGoodPlot(i) ? 1 : 0;
	}
	return iCount;
}

int CvCityAI::AI_countWorkedPoorPlots() const
{
	int iCount = 0;

	for (int i = 1; i < NUM_CITY_PLOTS; i++)
	{
		iCount += isWorkingPlot(i) && !AI_isGoodPlot(i) ? 1 : 0;
	}
	return iCount;
}

// K-Mod, based on BBAI ideas
int CvCityAI::AI_getTargetPopulation() const
{
	int iHealth = goodHealth() - badHealth() + getEspionageHealthCounter();
	int iTargetSize = AI_countGoodPlots() + std::max(0, getSpecialistPopulation() - totalFreeSpecialists() - 1);
	// Civ4 Reimagined
	iTargetSize += GET_PLAYER(getOwnerINLINE()).AI_isDoStrategy(AI_STRATEGY_SPECIALIST_ECONOMY) ? 3 : 0;
	if (GET_PLAYER(getOwnerINLINE()).AI_getFlavorValue(FLAVOR_GROWTH) > 0)
		iTargetSize += iTargetSize / 6; // specialists.

	iTargetSize = std::min(iTargetSize, 2 + getPopulation() + iHealth/2);

	if (iTargetSize < getPopulation())
	{
		iTargetSize = std::max(iTargetSize, getPopulation() - AI_countWorkedPoorPlots() + std::min(0, iHealth/2));
	}

	iTargetSize = std::min(iTargetSize, 1 + getPopulation()+(happyLevel()-unhappyLevel()+getEspionageHappinessCounter()));

	return iTargetSize;
}

// K-Mod note: This function was once used for Debug only, but I've made it the one and only way to get the yield multipliers.
// This way we don't have to put up with duplicated code.
// This function has been mostly rewritten for K-Mod. Some parts of the original code still exist (particularly near the end), but most of the old code is gone.
void CvCityAI::AI_getYieldMultipliers(int &iFoodMultiplier, int &iProductionMultiplier, int &iCommerceMultiplier, int &iDesiredFoodChange) const
{
	PROFILE_FUNC();

	iFoodMultiplier = 100;
	iCommerceMultiplier = 100;
	iProductionMultiplier = 100;

	int aiUnworkedYield[NUM_YIELD_TYPES] = {};
	int aiWorkedYield[NUM_YIELD_TYPES] = {};
	int iUnworkedPlots = 0;
	int iWorkedPlots = 0;

	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	bool bFinancial = kPlayer.getExtraYieldThreshold(YIELD_COMMERCE) > 0; //Civ4 Reimagined
	int iGoodTileCount = 0;

	for (int i = 1; i < NUM_CITY_PLOTS; i++)
	{
		CvPlot* pLoopPlot = getCityIndexPlot(i);
		if (!pLoopPlot || pLoopPlot->getWorkingCity() != this)
			continue;

		BuildTypes eBuild = NO_BUILD;
		if (m_aeBestBuild[i] != NO_BUILD && m_aiBestBuildValue[i] > 50)
			eBuild = m_aeBestBuild[i];

		int aiPlotYields[NUM_YIELD_TYPES];

		if (eBuild != NO_BUILD)
		{
			for (YieldTypes y = (YieldTypes)0; y < NUM_YIELD_TYPES; y=(YieldTypes)(y+1))
				aiPlotYields[y] = pLoopPlot->getYieldWithBuild(eBuild, y, true);
		}
		else
		{
			for (YieldTypes y = (YieldTypes)0; y < NUM_YIELD_TYPES; y=(YieldTypes)(y+1))
				aiPlotYields[y] = pLoopPlot->getYield(y); // K-Mod note. unfortunately, this counts GA yield whereas getYieldWithBuild does not.
		}

		bool bGoodPlot = AI_isGoodPlot(i, aiPlotYields);

		if (bGoodPlot)
		{
			iGoodTileCount++;
			// note: worked plots are already counted.
			if (!pLoopPlot->isBeingWorked())
			{
				for (YieldTypes y = (YieldTypes)0; y < NUM_YIELD_TYPES; y=(YieldTypes)(y+1))
				{
					//aiUnworkedYield[y] += aiPlotYields[y];
					// Civ4 Reimagined
					aiUnworkedYield[y] += pLoopPlot->getYield(y);
				}

				iUnworkedPlots++;
				// note. this can count plots that are low on food.
			}
		}

		if (pLoopPlot->isBeingWorked())
		{
			for (YieldTypes y = (YieldTypes)0; y < NUM_YIELD_TYPES; y=(YieldTypes)(y+1))
				aiWorkedYield[y] += aiPlotYields[y];

			iWorkedPlots++;
		}
	}
	// I'd like to use AI_getTargetPopulation here, to avoid code duplication, but that would result in us doing a bunch of unneccessary recalculations.
	int iSpecialistCount = getSpecialistPopulation() - totalFreeSpecialists();
	
	int iHealth = goodHealth() - badHealth() + getEspionageHealthCounter();
	int iTargetSize = iGoodTileCount + std::max(0, iSpecialistCount - 1);
	// Civ4 Reimagined
	iTargetSize += kPlayer.AI_isDoStrategy(AI_STRATEGY_SPECIALIST_ECONOMY) ? 3 : 0;
	if (kPlayer.AI_getFlavorValue(FLAVOR_GROWTH) > 0)
		iTargetSize += iGoodTileCount / 6;

	iTargetSize = std::min(iTargetSize, 2 + getPopulation() + iHealth/2);

	if (iTargetSize < getPopulation())
	{
		iTargetSize = std::max(iTargetSize, getPopulation() - AI_countWorkedPoorPlots() + std::min(0, iHealth/2));
	}

	iTargetSize = std::min(iTargetSize, 1 + getPopulation()+(happyLevel()-unhappyLevel()+getEspionageHappinessCounter()));

	// total food and production include yield from buildings, corporations, specialists, and so on.
	int iFoodTotal = getBaseYieldRate(YIELD_FOOD);
	int iProductionTotal = getBaseYieldRate(YIELD_PRODUCTION);

	int iFutureFoodAdjustment = 0;
	if (iUnworkedPlots > 0)
	{
		// calculate approximately how much extra food we could work if we used our specialists and our extra population.
		iFutureFoodAdjustment = std::min(iSpecialistCount + std::max(0, iTargetSize - getPopulation()), iUnworkedPlots) * aiUnworkedYield[YIELD_FOOD] / iUnworkedPlots;
	}
	iFoodTotal += iFutureFoodAdjustment;
	iProductionTotal += aiUnworkedYield[YIELD_PRODUCTION];

	int iExtraFoodForGrowth = 0;
	if (iTargetSize > getPopulation())
	{
		iExtraFoodForGrowth = (iTargetSize - getPopulation())/2 + 1;
	}

	int iFoodDifference = iFoodTotal - (iTargetSize * GC.getFOOD_CONSUMPTION_PER_POPULATION() + iExtraFoodForGrowth);

	iDesiredFoodChange = -iFoodDifference + std::max(0, -iHealth);
	//

	/* if (iFoodDifference < 0)
	{
		iFoodMultiplier +=  -iFoodDifference * 4;
	}

	if (iFoodDifference > 4)
	{
		iFoodMultiplier -= 8 + 4 * iFoodDifference;
	} */

	int iProductionTarget = 1 + std::max(getPopulation(), iTargetSize * 3 / 5);

	if (iProductionTotal < iProductionTarget)
	{
		iProductionMultiplier += 8 * (iProductionTarget - iProductionTotal);
	}

	// K-mod
	if (kPlayer.AI_getFlavorValue(FLAVOR_PRODUCTION) > 0 || kPlayer.AI_getFlavorValue(FLAVOR_MILITARY) >= 10)
	{
		/* iProductionMultiplier *= 113 + 2 * kPlayer.AI_getFlavorValue(FLAVOR_PRODUCTION) + kPlayer.AI_getFlavorValue(FLAVOR_MILITARY);
		iProductionMultiplier /= 100; */
		iCommerceMultiplier *= 100;
		iCommerceMultiplier /= 113 + 2 * kPlayer.AI_getFlavorValue(FLAVOR_PRODUCTION) + kPlayer.AI_getFlavorValue(FLAVOR_MILITARY);
	}
	// K-Mod end

	int iNetCommerce = kPlayer.AI_getAvailableIncome(); // K-Mod
	int iNetExpenses = kPlayer.calculateInflatedCosts() + std::max(0, -kPlayer.getGoldPerTurn()); // unofficial patch

	int iRatio = (100 * iNetExpenses) / std::max(1, iNetCommerce);

	if (iRatio > 40)
	{
		//iCommerceMultiplier += (33 * (iRatio - 40)) / 60;
		// K-Mod, just a bit steeper
		iCommerceMultiplier += (50 * (iRatio - 40)) / 60;
	}

	int iProductionAdvantage = 100 * AI_yieldMultiplier(YIELD_PRODUCTION);
	iProductionAdvantage /= kPlayer.AI_averageYieldMultiplier(YIELD_PRODUCTION);
	iProductionAdvantage *= kPlayer.AI_averageYieldMultiplier(YIELD_COMMERCE);
	iProductionAdvantage /= AI_yieldMultiplier(YIELD_COMMERCE);

	// adjust based on # of cities. (same as the original bts code)
	int iNumCities = kPlayer.getNumCities();
	FAssert(iNumCities > 0);
	iProductionAdvantage = (iProductionAdvantage * (iNumCities - 1) + 200) / (iNumCities + 1);

	iProductionMultiplier *= iProductionAdvantage;
	iProductionMultiplier /= 100;

	iCommerceMultiplier *= 100;
	iCommerceMultiplier /= iProductionAdvantage;
	
	// Civ4 Reimagined
	if (bFinancial)
	{
		iCommerceMultiplier *= 5;
		iCommerceMultiplier /= 4;
	}

	/* int iGreatPeopleAdvantage = 100 * getTotalGreatPeopleRateModifier();
	iGreatPeopleAdvantage /= kPlayer.AI_averageGreatPeopleMultiplier();
	iGreatPeopleAdvantage = (iGreatPeopleAdvantage * (iNumCities - 1) + 200) / (iNumCities + 1);
	iGreatPeopleAdvantage += 200; // dilute
	iGreatPeopleAdvantage /= 3;

	//With great people we want to slightly increase food priority at the expense of commerce
	//this gracefully handles both wonder and specialist based GPP...
	iCommerceMultiplier *= 100;
	iCommerceMultiplier /= iGreatPeopleAdvantage;
	iFoodMultiplier *= iGreatPeopleAdvantage;
	iFoodMultiplier /= 100; */

	// Note: the AI does not use this kind of emphasis
	if (isHuman())
	{
		/* if (AI_isEmphasizeYield(YIELD_FOOD))
		{
			iFoodMultiplier *= 140; // was 130
			iFoodMultiplier /= 100;
		} */ // moved lower
		if (AI_isEmphasizeYield(YIELD_PRODUCTION))
		{
			iProductionMultiplier *= 130; // was 140
			iProductionMultiplier /= 100;
			// K-Mod
			if (!AI_isEmphasizeYield(YIELD_COMMERCE))
			{
				iCommerceMultiplier *= 80;
				iCommerceMultiplier /= 100;
			}
			// K-Mod end
		}
		if (AI_isEmphasizeYield(YIELD_COMMERCE))
		{
			iCommerceMultiplier *= 140;
			iCommerceMultiplier /= 100;
		}
	}
	else
	{
		// K-Mod. strategy / personality modifiers.
		// Note: these should be roughly consistent with the modifiers used in AI_updateSpecialYieldMultiplier
		if (kPlayer.AI_isDoStrategy(AI_STRATEGY_PRODUCTION))
		{
			iProductionMultiplier += 20;
			iCommerceMultiplier -= 20;
		}
		else if (findBaseYieldRateRank(YIELD_PRODUCTION) <= kPlayer.getNumCities()/3 && findBaseYieldRateRank(YIELD_PRODUCTION) < findBaseYieldRateRank(YIELD_COMMERCE))
		{
			iProductionMultiplier += 10;
			iCommerceMultiplier -= 10;
		}

		if (kPlayer.AI_getFlavorValue(FLAVOR_PRODUCTION) > 0)
		{
			iProductionMultiplier += 5 + 2*kPlayer.AI_getFlavorValue(FLAVOR_PRODUCTION);
		}

		if (kPlayer.AI_isDoStrategy(AI_STRATEGY_ECONOMY_FOCUS))
		{
			iProductionMultiplier -= 10;
			iCommerceMultiplier += 20;
		}
		/* else if (kPlayer.AI_getFlavorValue(FLAVOR_SCIENCE) + kPlayer.AI_getFlavorValue(FLAVOR_GOLD) > 2)
		{
			iCommerceMultiplier += 5 + kPlayer.AI_getFlavorValue(FLAVOR_SCIENCE) + kPlayer.AI_getFlavorValue(FLAVOR_GOLD);
		} */
		// K-Mod end
	}

	if (iProductionMultiplier < 100)
	{
		iProductionMultiplier = 10000 / (200 - iProductionMultiplier);
	}
	if (iCommerceMultiplier < 100)
	{
		iCommerceMultiplier = 10000 / (200 - iCommerceMultiplier);
	}

	// K-Mod. experimental food value
	//iFoodMultiplier = (80 * iProduction + 40 * iCommerce) / (food needed for jobs)  * (Food needed for jobs + growth) / (food have total)
	//iFoodMultiplier = (80 * iProductionMultiplier * iProductionTotal + 40 * iCommerceMultiplier * iCommerceTotal) * (iFoodTotal + iDesiredFoodChange) / std::max(1, 100 * iTargetSize * GC.getFOOD_CONSUMPTION_PER_POPULATION() * iFoodTotal);

	// emphasise the yield that we are working, so that the weakest of the 'good' plots don't drag down the average quite so much.
	// note: the multipliers on production and commerce should match the numbers used in AI_getImprovementValue.
	// changed by Civ4 Reimagined
	iFoodMultiplier = 70 * iProductionMultiplier * (aiUnworkedYield[YIELD_PRODUCTION] + aiWorkedYield[YIELD_PRODUCTION] * 2) * (iUnworkedPlots + iWorkedPlots);
	iFoodMultiplier += 40 * iCommerceMultiplier * (aiUnworkedYield[YIELD_COMMERCE] + aiWorkedYield[YIELD_COMMERCE] * 2) * (iUnworkedPlots + iWorkedPlots);
	iFoodMultiplier /= 100 * std::max(1, iUnworkedPlots + 2*iWorkedPlots);
	iFoodMultiplier += 180 * iDesiredFoodChange;
	iFoodMultiplier = std::max(iFoodMultiplier, 200); // (a minimum yield value for small cities.)

	//iFoodMultiplier *= iTargetSize;
	//iFoodMultiplier /= std::max(iTargetSize, getPopulation() + iUnworkedPlots - std::max(0, iDesiredFoodChange));

	iFoodMultiplier *= (iFoodTotal + iDesiredFoodChange);
	iFoodMultiplier /= std::max(1, (iUnworkedPlots + iWorkedPlots) * GC.getFOOD_CONSUMPTION_PER_POPULATION() * (iFoodTotal-iExtraFoodForGrowth));
	
	// Civ4 Reimagined
	if (kPlayer.canPopRush())
	{
		iFoodMultiplier *= 125;
		iFoodMultiplier /= 100;
	}

	// Note: this food multiplier calculation still doesn't account for possible food yield multipliers. Sorry.

	if (isHuman() && AI_isEmphasizeYield(YIELD_FOOD))
	{
		iFoodMultiplier *= 140;
		iFoodMultiplier /= 100;
	}
	// K-Mod end

	if (iFoodMultiplier < 100)
	{
		iFoodMultiplier = 10000 / (200 - iFoodMultiplier);
	}
	
	// Civ4 Reimagined
	iFoodMultiplier = std::min(1000, iFoodMultiplier);
	iProductionMultiplier = std::min(1000, iProductionMultiplier);
	iCommerceMultiplier = std::min(1000, iCommerceMultiplier);
}

int CvCityAI::AI_getImprovementValue(CvPlot* pPlot, ImprovementTypes eImprovement, int iFoodPriority, int iProductionPriority, int iCommercePriority, int iDesiredFoodChange, int iClearFeatureValue, bool bEmphasizeIrrigation, BuildTypes* peBestBuild) const
{
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE()); // K-Mod

	// first check if the improvement is valid on this plot
	// this also allows us work out whether or not the improvement will remove the plot feature...
	int iBestTempBuildValue = 0;
	BuildTypes eBestTempBuild = NO_BUILD;

	const bool bFinancial = kOwner.getExtraYieldThreshold(YIELD_COMMERCE) > 0; // Civ4 Reimagined
	const bool bSpecialistEconomy = kOwner.AI_isDoStrategy(AI_STRATEGY_SPECIALIST_ECONOMY); // Civ4 Reimagined
	const BonusTypes eBonus = pPlot->getBonusType(getTeam());
	const BonusTypes eNonObsoleteBonus = pPlot->getNonObsoleteBonusType(getTeam());

	bool bValid = false;
	bool bIgnoreFeature = false;

	if (eImprovement == pPlot->getImprovementType())
	{
		bValid = true;
	}
	else
	{
		int iValue;
		for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
		{
			BuildTypes eBuild = ((BuildTypes)iJ);

			if (GC.getBuildInfo(eBuild).getImprovement() == eImprovement)
			{
				if (kOwner.canBuild(pPlot, eBuild, false))
				{
					iValue = 10000;

					iValue /= (GC.getBuildInfo(eBuild).getTime() + 1);

					// XXX feature production???

					if (iValue > iBestTempBuildValue)
					{
						iBestTempBuildValue = iValue;
						eBestTempBuild = eBuild;
					}
				}
			}
		}

		if (eBestTempBuild != NO_BUILD)
		{
			bValid = true;

			if (pPlot->getFeatureType() != NO_FEATURE)
			{
				if (GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pPlot->getFeatureType()))
				{
					bIgnoreFeature = true;

					if (GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange(YIELD_PRODUCTION) > 0)
					{
						if (eNonObsoleteBonus == NO_BONUS)
						{
							if (kOwner.isOption(PLAYEROPTION_LEAVE_FORESTS))
							{
								bValid = false;
							}
							else if (healthRate() < 0 && GC.getFeatureInfo(pPlot->getFeatureType()).getHealthPercent() > 0)
							{
								bValid = false;
							}
							// Civ4 Reimagined
							else if (kOwner.getFeatureHappiness(pPlot->getFeatureType()) > 0 && happyLevel() < 2)
							{
								bValid = false;
							}
						}
					}
				}
			}
		}
	}

	if (!bValid)
	{
		if (peBestBuild != NULL)
			*peBestBuild = NO_BUILD;
		return 0;
	}

	// Now get the valid of the improvement itself.

	ImprovementTypes eFinalImprovement = finalImprovementUpgrade(eImprovement);

	if (eFinalImprovement == NO_IMPROVEMENT)
	{
		eFinalImprovement = eImprovement;
	}

	int iValue = 0;
	//int aiDiffYields[NUM_YIELD_TYPES]; // removed by K-Mod (replaced by time-weighted yields!)
	//int aiFinalYields[NUM_YIELD_TYPES];

	if (eBonus != NO_BONUS)
	{
		if (eNonObsoleteBonus != NO_BONUS)
		{
			//if (GC.getImprovementInfo(eFinalImprovement).isImprovementBonusTrade(eNonObsoleteBonus))
			if (kOwner.doesImprovementConnectBonus(eFinalImprovement, eNonObsoleteBonus))
			{
				// K-Mod
				// Civ4 Reimagined: Changed multiplier from 50 to 100
				iValue += (kOwner.AI_bonusVal(eNonObsoleteBonus, 1) * 100);
				iValue += 100;
				// K-Mod end
			}
			else
			{
				// K-Mod, bug fix. (original code deleted now.)
				// Presumablly the original author wanted to subtract 1000 if eBestBuild would take away the bonus; not ... the nonsense they actually wrote.
				if (kOwner.doesImprovementConnectBonus(pPlot->getImprovementType(), eNonObsoleteBonus))
				{
					// By the way, AI_bonusVal is typically 10 for the first bonus, and 2 for subsequent.
					iValue -= (kOwner.AI_bonusVal(eNonObsoleteBonus, -1) * 50);
					iValue -= 100;
				}
			}
		}
	}
	else
	{
		if (eImprovement != pPlot->getImprovementType()) // Civ4 Reimagined
		{
			for (int iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
			{
				if (GC.getImprovementInfo(eFinalImprovement).getImprovementBonusDiscoverRand(iJ) > 0)
				{
					iValue++;
				}
			}
		}
	}

	//if (iValue >= 0) // condition disabled by K-Mod. (maybe the yield will be worth it!)
	{
		// K-Mod. Get a weighted average of the yields for improvements which upgrade (eg. cottages).
		int iTimeScale = 60;
		// Civ4 Reimagined: UpgradeRate can be 0
		if (kOwner.getImprovementUpgradeRate() > 0)
		{
			iTimeScale += (100*GC.getGameINLINE().getElapsedGameTurns()/GC.getGameINLINE().getEstimateEndTurn() < 30 ? 10 : 0);
			iTimeScale -= (kOwner.AI_isDoVictoryStrategyLevel4() ? 30 : (100*GC.getGameINLINE().getElapsedGameTurns()/GC.getGameINLINE().getEstimateEndTurn() > 70 ? 10 : 0));
			iTimeScale += (kOwner.AI_isDoVictoryStrategy(AI_STRATEGY_ECONOMY_FOCUS) ? 50 : 0);
			iTimeScale -= (GET_TEAM(getTeam()).getWarPlanCount(WARPLAN_TOTAL, true) ? 20 : (kOwner.AI_getFlavorValue(FLAVOR_MILITARY) > 0) ? 10 : 0);
			if (eNonObsoleteBonus != NO_BONUS && !kOwner.doesImprovementConnectBonus(eImprovement, eNonObsoleteBonus))
				iTimeScale = std::min(30, iTimeScale);
			iTimeScale = std::max(iTimeScale, 20);
			// Other adjustments?

			// Adjustments to match calculation in CvPlot::doImprovementUpgrade.
			iTimeScale = iTimeScale * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getImprovementPercent()/100;
			iTimeScale = iTimeScale * GC.getEraInfo(GC.getGameINLINE().getStartEra()).getImprovementPercent()/100;
			//Civ4 Reimagined: getImprovementUpgradeRate now returns a value scaled by turns times 100
			iTimeScale *= 100;
			iTimeScale = iTimeScale / kOwner.getImprovementUpgradeRate();
		} 
		else
		{
			iTimeScale = 1;
		}

		std::vector<float> weighted_final_yields(NUM_YIELD_TYPES);
		std::vector<float> weighted_yield_diffs(NUM_YIELD_TYPES);

		// Getting the time-weighted yields for the new and old improvements; then use them to calculate the final yield and yield difference.
		AI_timeWeightedImprovementYields(pPlot, eImprovement, iTimeScale, weighted_final_yields);
		AI_timeWeightedImprovementYields(pPlot, pPlot->getImprovementType(), iTimeScale, weighted_yield_diffs);

		for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			weighted_final_yields[iJ] += pPlot->calculateNatureYield((YieldTypes)iJ, getTeam(), bIgnoreFeature);
			weighted_yield_diffs[iJ] = weighted_final_yields[iJ] - (weighted_yield_diffs[iJ] + pPlot->calculateNatureYield((YieldTypes)iJ, getTeam()));
		}
		
		// Civ4 Reimagined: Temporary workaround (AI sometimes replaces good improvements with forts...)
		if (eBonus != NO_BONUS)
		{
			if (kOwner.doesImprovementConnectBonus(eFinalImprovement, eNonObsoleteBonus) && kOwner.doesImprovementConnectBonus(pPlot->getImprovementType(), eNonObsoleteBonus))
			{
				if (weighted_yield_diffs[YIELD_FOOD] <= 0 && weighted_yield_diffs[YIELD_PRODUCTION] <= 0 && weighted_yield_diffs[YIELD_COMMERCE] <= 0)
				{
					if (pPlot->getPlayerCityRadiusCount(getOwner()) > 0)
					{
						return 0;
					}
				}
			}
		}				

		// K-Mod
		/* If this improvement results in a change in food, then building it will result in a change in the food multiplier.
		We should try to preempt that change to prevent best-build from oscillating.
		In our situation, we have iDesiredFoodChange ~= -iFoodDifference and aiDiffYields[0] == 2 * food change from the improvement.
		Unfortunately, it's a bit of a lengthy calculation to work out all of the factors involved in iFoodPriority.
		So I'll just use a very rough approximation. Hopefully it will be better than nothing.
		*/
		int iCorrectedFoodPriority = iFoodPriority;
		if (weighted_yield_diffs[YIELD_FOOD] != 0 && isWorkingPlot(pPlot))
		{
			int iTotalFood = 16 * GC.getFOOD_CONSUMPTION_PER_POPULATION();
			// 16 is arbitrary. It would be possible to get something better using targetPop and so on, but that would be slower...
			iCorrectedFoodPriority = (int)(iCorrectedFoodPriority * (iTotalFood - weighted_yield_diffs[YIELD_FOOD]) / std::max(1, iTotalFood));
		}
		FAssert(iCorrectedFoodPriority == iFoodPriority || (iCorrectedFoodPriority < iFoodPriority == weighted_yield_diffs[YIELD_FOOD] > 0));
		// This corrected priority isn't perfect, but I think it will be better than nothing.
		// K-Mod end

		iValue += (int)(weighted_yield_diffs[YIELD_FOOD] * (bSpecialistEconomy ? 125 : 100) * iCorrectedFoodPriority / 100); // Civ4 Reimagined
		iValue += (int)(weighted_yield_diffs[YIELD_PRODUCTION] * 70 * iProductionPriority / 100); // Civ4 Reimagined, was 80
		iValue += (int)(weighted_yield_diffs[YIELD_COMMERCE] * 40 * iCommercePriority / 100);

		// K-Mod. If we're going to have too much food regardless of the improvement on this plot, then reduce the food value
		if (iDesiredFoodChange < 0 && -iDesiredFoodChange >= weighted_final_yields[YIELD_FOOD] - weighted_yield_diffs[YIELD_FOOD])
		{
			iValue -= (int)(weighted_yield_diffs[YIELD_FOOD] * iCorrectedFoodPriority * 40 / 100); // reduce the weight of food. (cf. values above.)
		}
		// K-Mod end
		
		//logBBAI("diffyields: %d, %d, %d", (int)weighted_yield_diffs[YIELD_FOOD], (int)weighted_yield_diffs[YIELD_PRODUCTION], (int)weighted_yield_diffs[YIELD_COMMERCE]);
		//logBBAI("finalyields: %d, %d, %d", (int)weighted_final_yields[YIELD_FOOD], (int)weighted_final_yields[YIELD_PRODUCTION], (int)weighted_final_yields[YIELD_COMMERCE]);
		
		//logBBAI("value before: %d", iValue);

		//if (iValue > 0)
		// Civ4 Reimagined: Don't ignore the current improvement!
		if (iValue > 0 || eImprovement == pPlot->getImprovementType())
		{
			// this is mainly to make it improve better tiles first
			//flood plain > grassland > plain > tundra
			iValue += (int)(weighted_final_yields[YIELD_FOOD] * 8); // was 10
			iValue += (int)(weighted_final_yields[YIELD_PRODUCTION] * 7); // was 6
			iValue += (int)(weighted_final_yields[YIELD_COMMERCE] * 4);

			if (weighted_final_yields[YIELD_FOOD] >= GC.getFOOD_CONSUMPTION_PER_POPULATION())
			{
				//this is a food yielding tile
				if (iCorrectedFoodPriority > 100)
				{
					iValue *= 100 + iCorrectedFoodPriority;
					iValue /= 200;
				}
				if (iDesiredFoodChange > 0)
				{
					//iValue += (10 * (1 + aiDiffYields[YIELD_FOOD]) * (1 + aiFinalYields[YIELD_FOOD] - GC.getFOOD_CONSUMPTION_PER_POPULATION()) * iDesiredFoodChange * iCorrectedFoodPriority) / 100;
					iValue += (int)(10 * (1 + weighted_yield_diffs[YIELD_FOOD]) * (1 + weighted_final_yields[YIELD_FOOD] - GC.getFOOD_CONSUMPTION_PER_POPULATION()) * std::min(1 + iDesiredFoodChange/3, 4) * iCorrectedFoodPriority / 100); // K-Mod
				}
				if (iCommercePriority > 100)
				{
					//iValue += (int)(iValue*((iCommercePriority - 100) * weighted_yield_diffs[YIELD_COMMERCE]))/200;
					// Civ4 Reimagined: Use finalyields instead of diffyield so we do not ignore this factor when improvement = current improvement.
					iValue += (int)(iValue*((iCommercePriority - 100) * weighted_final_yields[YIELD_COMMERCE]))/200;
				}
			}

			if ((iCorrectedFoodPriority < 100) && (iProductionPriority > 100))
			{
				iValue *= (int)(200 + ((iProductionPriority - 100)*weighted_final_yields[YIELD_PRODUCTION]));
				iValue /= 200;
			}
			if (eNonObsoleteBonus == NO_BONUS)
			{
				if (iDesiredFoodChange > 0)
				{
					//We want more food.
					iValue *= 2 + std::max(0, (int)weighted_yield_diffs[YIELD_FOOD]);
					iValue /= 2 * (1 + std::max(0, -(int)weighted_yield_diffs[YIELD_FOOD]));
				}
			}
		}

		if (bEmphasizeIrrigation && GC.getImprovementInfo(eFinalImprovement).isCarriesIrrigation())
		{
			iValue += 500;
		}

		if (getImprovementFreeSpecialists(eFinalImprovement) > 0)
		{
			iValue += 2000;
		}

		int iHappiness = GC.getImprovementInfo(eFinalImprovement).getHappiness();
		if ((iHappiness != 0) && !(kOwner.getAdvancedStartPoints() >= 0))
		{
			//int iHappyLevel = iHappyAdjust + (happyLevel() - unhappyLevel(0));
			int iHappyLevel = happyLevel() - unhappyLevel(0); // iHappyAdjust isn't currently being used.
			if (eImprovement == pPlot->getImprovementType())
			{
				iHappyLevel -= iHappiness;
			}
			int iHealthLevel = (goodHealth() - badHealth(false, 0));

			int iHappyValue = 0;
			if (iHappyLevel <= 0)
			{
				iHappyValue += 400;
			}
			bool bCanGrow = true;// (getYieldRate(YIELD_FOOD) > foodConsumption());

			/* original bts code
			if (iHappyLevel <= iHealthLevel)
			{
				iHappyValue += 200 * std::max(0, (bCanGrow ? std::min(6, 2 + iHealthLevel - iHappyLevel) : 0) - iHappyLevel);
			}
			else */ // commented out by K-Mod
			{
				iHappyValue += 200 * std::max(0, (bCanGrow ? 1 : 0) - iHappyLevel);
			}
			if (!pPlot->isBeingWorked())
			{
				iHappyValue *= 4;
				iHappyValue /= 3;
			}
			//iHappyValue += std::max(0, (pPlot->getCityRadiusCount() - 1)) * ((iHappyValue > 0) ? iHappyLevel / 2 : 200);
			// K-Mod
			iHappyValue *= (pPlot->getPlayerCityRadiusCount(getOwner()) + 1);
			iHappyValue /= 2;
			//
			iValue += iHappyValue * iHappiness;
		}

		if (!isHuman())
		{
			// Changed by Civ4 Reimagined
			iValue *= std::max(0, (GC.getLeaderHeadInfo(getPersonalityType()).getImprovementWeightModifier(eFinalImprovement) + 100));
			iValue /= 100;
		}

		//if (pPlot->getImprovementType() == NO_IMPROVEMENT)
		// Civ4 Reimagined
		if (pPlot->getImprovementType() == NO_IMPROVEMENT || pPlot->getImprovementType() == eImprovement)
		{
			// Civ4 Reimagined
			if (pPlot->getFeatureType() != NO_FEATURE && GC.getFeatureInfo(pPlot->getFeatureType()).getHealthPercent() > 0 && healthRate() < 0 && eBestTempBuild != NO_BUILD && GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pPlot->getFeatureType()))
			{
				iValue /= 3;
			}
			else if (pPlot->isBeingWorked()  // K-Mod. (don't boost the value if it means removing a good feature.)
				&& (iClearFeatureValue >= 0 || eBestTempBuild == NO_BUILD || !GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pPlot->getFeatureType())))
			{
				iValue *= 5;
				iValue /= 4;
			}

			/* original bts code
			if (eBestTempBuild != NO_BUILD)
			{
				if (pPlot->getFeatureType() != NO_FEATURE)
				{
					if (GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pPlot->getFeatureType()))
					{
						CvCity* pCity;
						iValue += pPlot->getFeatureProduction(eBestTempBuild, getTeam(), &pCity) * 2;
						FAssert(pCity == this);

						iValue += iClearFeatureValue;
					}
				}
			} */ // K-Mod. I've moved this out of the if statement, because it should apply regardless of whether there is already an improvement on the plot.
		}
		else
		{
			// cottage/villages (don't want to chop them up if turns have been invested)
			ImprovementTypes eImprovementDowngrade = (ImprovementTypes)GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementPillage();
			/* original bts code
			while (eImprovementDowngrade != NO_IMPROVEMENT)
			{
				CvImprovementInfo& kImprovementDowngrade = GC.getImprovementInfo(eImprovementDowngrade);
				iValue -= kImprovementDowngrade.getUpgradeTime() * 8;
				eImprovementDowngrade = (ImprovementTypes)kImprovementDowngrade.getImprovementPillage();
			} */
			// K-Mod. Be careful not to get trapped in an infinite loop of improvement downgrades.
			if (eImprovementDowngrade != NO_IMPROVEMENT)
			{
				std::set<ImprovementTypes> cited_improvements;
				while (eImprovementDowngrade != NO_IMPROVEMENT && cited_improvements.insert(eImprovementDowngrade).second)
				{
					const CvImprovementInfo& kImprovementDowngrade = GC.getImprovementInfo(eImprovementDowngrade);
					//iValue -= kImprovementDowngrade.getUpgradeTime() * 8;
					iValue -= kImprovementDowngrade.getUpgradeTime() * 24; // Civ4 Reimagined
					eImprovementDowngrade = (ImprovementTypes)kImprovementDowngrade.getImprovementPillage();
				}
			}
			// K-Mod end

			if (GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementUpgrade() != NO_IMPROVEMENT)
			{
				//iValue -= (GC.getImprovementInfo(pPlot->getImprovementType()).getUpgradeTime() * 8 * (pPlot->getUpgradeProgress())) / std::max(1, GC.getGameINLINE().getImprovementUpgradeTime(pPlot->getImprovementType()));
				// Civ4 Reimagined: getUpgradeProcess now scales with turn times 100
				iValue -= (GC.getImprovementInfo(pPlot->getImprovementType()).getUpgradeTime() * 8 * (pPlot->getUpgradeProgress() / 100)) / std::max(1, GC.getGameINLINE().getImprovementUpgradeTime(pPlot->getImprovementType()));
			}

			// Civ4 Reimagined: I don't undestand what this code is useful for... Removed this because it encourages alternating improvements on the same tile and makes the evaluation more complicated.
			/*
			if (eNonObsoleteBonus == NO_BONUS)
			{
				if (isWorkingPlot(pPlot))
				{
					if (((iCorrectedFoodPriority < 100) && (weighted_final_yields[YIELD_FOOD] >= GC.getFOOD_CONSUMPTION_PER_POPULATION())) || (GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementPillage() != NO_IMPROVEMENT))
					{
						iValue -= 70;
						iValue *= 2;
						iValue /= 3;
					}
				}
			}
			*/

			if (kOwner.isOption(PLAYEROPTION_SAFE_AUTOMATION))
			{
				iValue /= 4;	//Greatly prefer builds which are legal.
			}
		}
		// K-Mod. Feature value. (moved from the 'no improvement' block above.)
		if (pPlot->getFeatureType() != NO_FEATURE && eBestTempBuild != NO_BUILD && GC.getBuildInfo(eBestTempBuild).isFeatureRemove(pPlot->getFeatureType()))
		{
			//CvCity* pCity;
			//iValue += pPlot->getFeatureProduction(eBestTempBuild, getTeam(), &pCity) * 2; // handle chop value elsewhere
			//FAssert(pCity == this);
			iValue += iClearFeatureValue;
		}
		// K-Mod end
	}
	if (peBestBuild != NULL)
		*peBestBuild = eBestTempBuild;

	return iValue;
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	
BuildTypes CvCityAI::AI_getBestBuild(int iIndex) const
{
	FAssertMsg(iIndex >= 0, "iIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(iIndex < NUM_CITY_PLOTS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aeBestBuild[iIndex];
}


int CvCityAI::AI_countBestBuilds(CvArea* pArea) const
{
	CvPlot* pLoopPlot;
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->area() == pArea)
				{
					if (AI_getBestBuild(iI) != NO_BUILD)
					{
						iCount++;
					}
				}
			}
		}
	}

	return iCount;
}

// Note: this function has been somewhat mangled by K-Mod
void CvCityAI::AI_updateBestBuild()
{
	int iFoodMultiplier, iProductionMultiplier, iCommerceMultiplier, iDesiredFoodChange;
	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE()); // K-Mod
	
	if (gCityLogLevel >= 2)
		logBBAI("update best build in %S", getName().GetCString());

	AI_getYieldMultipliers(iFoodMultiplier, iProductionMultiplier, iCommerceMultiplier, iDesiredFoodChange);

	/* I've disabled these for now
	int iHappyAdjust = 0;
	int iHealthAdjust = 0; */

	bool bChop = false;

	if (getProductionProcess() == NO_PROCESS) // K-Mod. (never chop if building a process.)
	{
		if (!bChop)
		{
			ProjectTypes eProductionProject = getProductionProject();
			if (eProductionProject != NO_PROJECT && AI_projectValue(eProductionProject) > 0)
				bChop = true;
		}
		if (!bChop)
		{
			BuildingTypes eProductionBuilding = getProductionBuilding();
			if (eProductionBuilding != NO_BUILDING && isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eProductionBuilding).getBuildingClassType())))
				bChop = true;
		}

		// K-Mod. Chop when a new city with low productivity is trying to construct a (useful) building.
		if (!bChop && getProductionBuilding() != NO_BUILDING)
		{
			// ideally this would be based on the value of what we are building, and on the number of things we still want to build.
			// But it currently isn't easy to get that infomation, so I'm just going to use arbitrary minimums. Sorry.
			// (The scale of this is roughly a 2 pop whip or 2 mines of productivity - higher with productive flavour.)
			if (kOwner.AI_getFlavorValue(FLAVOR_PRODUCTION) > 0
				? (getHighestPopulation() <= 6 && getBaseYieldRate(YIELD_PRODUCTION) <= 10)
				: (getHighestPopulation() <= 4 && getBaseYieldRate(YIELD_PRODUCTION) <= 7))
			{
				bChop = true;
			}
		}
		// Chop when trying to expand, and when at war -- but only if there is not financial trouble.
		if (!bChop)
		{
			int iDummy;
			if (kOwner.AI_isLandWar(area()) ||
				(kOwner.getNumCities() < GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities() && kOwner.AI_getNumAreaCitySites(getArea(), iDummy) > 0))
			{
				if (!kOwner.AI_isFinancialTrouble())
					bChop = true;
			}
		}
		// Chop for workers, if we are short.
		if (!bChop && getProductionUnitAI() == UNITAI_WORKER)
		{
			if (area()->getNumAIUnits(getOwnerINLINE(), UNITAI_WORKER) < std::max(area()->getCitiesPerPlayer(getOwnerINLINE()), GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities()*3/2))
				bChop = true;
		}
		// K-Mod end
	}

	/*if (getProductionBuilding() != NO_BUILDING)
	{
		iHappyAdjust += getBuildingHappiness(getProductionBuilding());
		iHealthAdjust += getBuildingHealth(getProductionBuilding());
	}*/

	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

			if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
			{
				int iLastBestBuildValue = m_aiBestBuildValue[iI];
				BuildTypes eLastBestBuildType = m_aeBestBuild[iI];
				
				// Civ4 Reimagined
				if (isWorkingPlot(iI))
				{
					bChop = true;
				}

				//AI_bestPlotBuild(pLoopPlot, &(m_aiBestBuildValue[iI]), &(m_aeBestBuild[iI]), iFoodMultiplier, iProductionMultiplier, iCommerceMultiplier, bChop, iHappyAdjust, iHealthAdjust, iDesiredFoodChange);
				AI_bestPlotBuild(pLoopPlot, &(m_aiBestBuildValue[iI]), &(m_aeBestBuild[iI]), iFoodMultiplier, iProductionMultiplier, iCommerceMultiplier, bChop, 0, 0, iDesiredFoodChange);
				//int iWorkerCount = GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD); // K-Mod, originally this was all workers at the city.
				/* m_aiBestBuildValue[iI] *= 4;
				m_aiBestBuildValue[iI] += 3 + iWorkerCount; // (round up)
				m_aiBestBuildValue[iI] /= (4 + iWorkerCount); */ // disabled by K-Mod. I don't think this really helps us.

				if (m_aiBestBuildValue[iI] > 0)
				{
					FAssert(m_aeBestBuild[iI] != NO_BUILD);
				}
				if (m_aeBestBuild[iI] != NO_BUILD)
				{
					FAssert(m_aiBestBuildValue[iI] > 0);
				}

				// K-Mod, make some adjustments to our yield weights based on our new bestbuild
				if (m_aeBestBuild[iI] != eLastBestBuildType) // [really we want (isWorking || was good plot), but that's harder and more expensive...]
				{
					if (isWorkingPlot(iI)) // [or was 'good plot' with previous build]
					{
						// its a new best-build, so lets adjust our multiplier values.
						// This adjustment is rough, but better than nothing. (at least, I hope so...)
						// (The most accurate way to do this would be to use AI_getYieldMultipliers; but I think that would be too slow.)
						int iDelta;

						// food
						iDelta =   m_aeBestBuild[iI] == NO_BUILD ? pLoopPlot->getYield(YIELD_FOOD) : pLoopPlot->getYieldWithBuild(m_aeBestBuild[iI], YIELD_FOOD, true); // new
						iDelta -= eLastBestBuildType == NO_BUILD ? pLoopPlot->getYield(YIELD_FOOD) : pLoopPlot->getYieldWithBuild(eLastBestBuildType, YIELD_FOOD, true); // old
						if (iDelta != 0)
						{
							int iTotalFood = 16 * GC.getFOOD_CONSUMPTION_PER_POPULATION();
							iFoodMultiplier *= iTotalFood - iDelta;
							iFoodMultiplier /= std::max(1, iTotalFood);
							// cf. adjustment in AI_getImprovementValue.

							iDesiredFoodChange -= iDelta;
						}

						// production
						iDelta =   m_aeBestBuild[iI] == NO_BUILD ? pLoopPlot->getYield(YIELD_PRODUCTION) : pLoopPlot->getYieldWithBuild(m_aeBestBuild[iI], YIELD_PRODUCTION, true); // new
						iDelta -= eLastBestBuildType == NO_BUILD ? pLoopPlot->getYield(YIELD_PRODUCTION) : pLoopPlot->getYieldWithBuild(eLastBestBuildType, YIELD_PRODUCTION, true); // old
						if (iDelta != 0)
						{
							int iProductionTotal = getBaseYieldRate(YIELD_PRODUCTION);
							int iProductionTarget = 1 + getPopulation();
							// note: the true values depend on unworked plots and so on. this is just a rough approximation.
							if (iProductionTotal < iProductionTarget)
							{
								iProductionMultiplier -= 6 * std::min(iDelta, iProductionTarget - iProductionTotal);
								// cf. iProductionMultiplier += 8 * (iProductionTarget - iProductionTotal);
							}
							// iProductionTotal += iDelta;
						}
						// Happiness modifers.. maybe I'll do this later, after testing etc.
					}

					// since best-build has changed, cancel all current build missions on this plot
					if (eLastBestBuildType != NO_BUILD)
					{
						int iLoop;
						for(CvSelectionGroup* pLoopSelectionGroup = kOwner.firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = kOwner.nextSelectionGroup(&iLoop))
						{
							if (pLoopSelectionGroup->AI_getMissionAIPlot() == pLoopPlot && pLoopSelectionGroup->AI_getMissionAIType() == MISSIONAI_BUILD)
							{
								FAssert(pLoopSelectionGroup->getHeadUnitAI() == UNITAI_WORKER || pLoopSelectionGroup->getHeadUnitAI() == UNITAI_WORKER_SEA);
								pLoopSelectionGroup->clearMissionQueue();
							}
						}
					}
				}
				// K-Mod end
			}
		}
	}


	{	//new experimental yieldValue calcuation
		short aiYields[NUM_YIELD_TYPES];
		int iBestPlot = -1;
		int iBestPlotValue = -1;
		
		int iBestUnworkedPlotValue = 0;
		
		int aiValues[NUM_CITY_PLOTS];
		int iGrowthValue = AI_growthValuePerFood(); // K-Mod
		
		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			if (iI != CITY_HOME_PLOT)
			{
				CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

				if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
				{
					int iValue = 0; // Civ4 Reimagined
					if (m_aeBestBuild[iI] != NO_BUILD)
					{
						for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							aiYields[iJ] = pLoopPlot->getYieldWithBuild(m_aeBestBuild[iI], (YieldTypes)iJ, true);
						}

						iValue = AI_yieldValue(aiYields, 0, false, false, true, true, iGrowthValue);
						aiValues[iI] = iValue;

						// K-Mod
						// Also evaluate the _change_ in yield, so that we aren't always tinkering with our best plots.
						for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							aiYields[iJ] -= pLoopPlot->getYield((YieldTypes)iJ);
						}
						iValue += AI_yieldValue(aiYields, 0, false, false, true, true, iGrowthValue);
						// Note: AI_yieldValue wasn't intended to be used with negative yields. But I think it will work; and it doesn't need to be perfectly accurate anyway.

						// priority increase for chopping when we want to chop
						if (bChop && pLoopPlot->getFeatureType() != NO_FEATURE && GC.getBuildInfo(m_aeBestBuild[iI]).isFeatureRemove(pLoopPlot->getFeatureType()))
						{
							CvCity* pCity;
							int iChopProduction = pLoopPlot->getFeatureProduction(m_aeBestBuild[iI], getTeam(), &pCity);
							if (iChopProduction > 0)
							{
								iValue += (iI <= 8 && GC.getFeatureInfo(pLoopPlot->getFeatureType()).getDefenseModifier() > 0) ? 20 : 10;
								iValue += iChopProduction * 2;
							}
							// note: the scale of iValue here is roughly 4x commerce per turn. So a boost of 40 would be signficant.
							FAssert(pCity == this);
						}
						// make some minor adjustments to prioritize plots that are easy to access, and plots which aren't already improved.
						if (iValue > 0)
						{
							if (pLoopPlot->getRouteType() != NO_ROUTE)
								iValue += 2;
							if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
								iValue += 4;
							if (pLoopPlot->getNumCultureRangeCities(getOwnerINLINE()) > 1)
								iValue += 1;
						}
						// K-Mod end

						iValue = std::max(0, iValue);

						m_aiBestBuildValue[iI] *= iValue + 100;
						m_aiBestBuildValue[iI] /= 100;

						if (iValue > iBestPlotValue)
						{
							iBestPlot = iI;
							iBestPlotValue = iValue;
						}
					}
					// Civ4 Reimagined
					else
					{
						for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							aiYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
						}
						iValue = AI_yieldValue(aiYields, 0, false, false, true, true, iGrowthValue);
					}
					
					if (!pLoopPlot->isBeingWorked())
					{
						iBestUnworkedPlotValue = std::max(iBestUnworkedPlotValue, iValue);
					}
					if (gCityLogLevel >= 2)
					{
						//logBBAI("%S plot (%d,%d): %d", getName().GetCString(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), iValue);
					}
				}
			}
		}
		if (iBestPlot != -1)
		{
			//m_aiBestBuildValue[iBestPlot] *= 2;
			m_aiBestBuildValue[iBestPlot] = m_aiBestBuildValue[iBestPlot] * 3 / 2; // K-Mod
		}
		if (gCityLogLevel >= 2)
			logBBAI("best unworked plotvalue in %S: %d", getName().GetCString(), iBestUnworkedPlotValue);

		//Prune plots which are sub-par.
		// K-Mod. I've rearranged the following code. But kept most of the original functionality.
		if (iBestUnworkedPlotValue > 0)
		{
			PROFILE("AI_updateBestBuild pruning phase");
			for (int iI = 1; iI < NUM_CITY_PLOTS; iI++) // skip the city plot
			{
				CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);

				if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
				{
					// K-Mod. If the new improvement will upgrade over time, then don't mark it as being low-priority. We want to build it sooner rather than later.
					if (m_aeBestBuild[iI] != NO_BUILD && GC.getBuildInfo(m_aeBestBuild[iI]).getImprovement() != NO_IMPROVEMENT &&
						GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(m_aeBestBuild[iI]).getImprovement()).getImprovementUpgrade() == NO_IMPROVEMENT)
					{
						if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
						{
							//if (!pLoopPlot->isBeingWorked() && aiValues[iI] <= iBestUnworkedPlotValue && aiValues[iI] < 400) // was 500. (reduced due to some rescaling)
							if (!pLoopPlot->isBeingWorked() && aiValues[iI] < iBestUnworkedPlotValue && aiValues[iI] < 400 && m_aiBestBuildValue[iI] < 200) // Civ4 Reimagined
							{
								m_aiBestBuildValue[iI] = 1;
							}
						}
						else
						{
							for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
							{
								aiYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
							}

							int iValue = AI_yieldValue(aiYields, 0, false, false, true, true, iGrowthValue);
							if (iValue > aiValues[iI])
							{
								m_aiBestBuildValue[iI] = 1;
							}
						}
					}
				}
			}
		}
	}
}

// Protected Functions...

void CvCityAI::AI_doDraft(bool bForce)
{
	PROFILE_FUNC();

	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	FAssert(!isHuman());
	if (isBarbarian())
	{
		return;
	}

	if (canConscript())
	{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      07/12/09                                jdog5000      */
/*                                                                                              */
/* City AI, War Strategy AI                                                                     */
/************************************************************************************************/
		// if (GC.getGameINLINE().AI_combatValue(getConscriptUnit()) > 33) // disabled by K-Mod
		{
			if (bForce)
			{
				conscript();
				return;
			}
			//bool bLandWar = ((area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING));
			bool bLandWar = kOwner.AI_isLandWar(area()); // K-Mod
			bool bDanger = (!AI_isDefended() && AI_isDanger());
			int iUnitCostPerMil = kOwner.AI_unitCostPerMil(); // K-Mod

			// Don't go broke from drafting
			//if( !bDanger && kOwner.AI_isFinancialTrouble() )
			if (!bDanger && iUnitCostPerMil > kOwner.AI_maxUnitCostPerMil(area(), 50)) // K-Mod. (cf. conditions for scraping units in AI_doTurnUnitPost)
			{
				return;
			}

			// K-Mod. See if our drafted unit is particularly good value.
			// (cf. my calculation in CvPlayerAI::AI_civicValue)
			UnitTypes eConscriptUnit = getConscriptUnit();
			int iConscriptPop = std::max(1, GC.getUnitInfo(eConscriptUnit).getProductionCost() / GC.getDefineINT("CONSCRIPT_POPULATION_PER_COST"));

			// call it "good value" if we get at least 1.4 times the normal hammers-per-conscript-pop.
			// (with standard settings, this only happens for riflemen)
			bool bGoodValue = 10 * GC.getUnitInfo(eConscriptUnit).getProductionCost() / (iConscriptPop * GC.getDefineINT("CONSCRIPT_POPULATION_PER_COST")) >= 14;

			// one more thing... it's not "good value" if we already have too many troops.
			if (!bLandWar && bGoodValue)
			{
				bGoodValue = iUnitCostPerMil <= kOwner.AI_maxUnitCostPerMil(area(), 20) + kOwner.AI_getFlavorValue(FLAVOR_MILITARY);
			}
			// K-Mod end - although, I've put a bunch of 'bGoodValue' conditions in all through the rest of this function.

			// Don't shrink cities too much
			//int iConscriptPop = getConscriptPopulation();
			if (!bGoodValue && !bDanger && (3 * (getPopulation() - iConscriptPop) < getHighestPopulation() * 2) )
			{
				return;
			}

			// Large cities want a little spare happiness
			int iHappyDiff = GC.getDefineINT("CONSCRIPT_POP_ANGER") - iConscriptPop + (bGoodValue ? 0 : getPopulation()/10);

			if ((bGoodValue || bLandWar) && angryPopulation(iHappyDiff) == 0)
			{
				bool bWait = true;

				if( bWait && kOwner.AI_isDoStrategy(AI_STRATEGY_TURTLE) )
				{
					// Full out defensive
					/* original bts code
					if( bDanger || (getPopulation() >= std::max(5, getHighestPopulation() - 1)) )
					{
						bWait = false;
					}
					else if( AI_countWorkedPoorPlots() >= 1 )
					{
						bWait = false;
					}*/

					// K-Mod: full out defensive indeed. We've already checked for happiness, and we're desperate for units.
					// Just beware of happiness sources that might expire - such as military happiness.
					if (getConscriptAngerTimer() == 0 || AI_countWorkedPoorPlots() > 0)
						bWait = false;
				}

				if( bWait && bDanger )
				{
					// If city might be captured, don't hold back
					/* BBAI code
					int iOurDefense = GET_TEAM(getTeam()).AI_getOurPlotStrength(plot(),0,true,false,true);
					int iEnemyOffense = GET_PLAYER(getOwnerINLINE()).AI_getEnemyPlotStrength(plot(),2,false,false);

					if( (iOurDefense == 0) || (3*iEnemyOffense > 2*iOurDefense) ) */
					// K-Mod
					int iOurDefense = kOwner.AI_localDefenceStrength(plot(), getTeam(), DOMAIN_LAND, 0);
					int iEnemyOffense = kOwner.AI_localAttackStrength(plot(), NO_TEAM, DOMAIN_LAND, 2);
					if (iOurDefense < iEnemyOffense)
					// K-Mod end
					{
						bWait = false;
					}
				}

				if( bWait )
				{
					// Non-critical, only burn population if population is not worth much
					//if ((getConscriptAngerTimer() == 0) && (AI_countWorkedPoorPlots() > 1))
					if ((getConscriptAngerTimer() == 0 || isNoUnhappiness()) // K-Mod
						&& (bGoodValue || AI_countWorkedPoorPlots() > 0 || foodDifference(false, true)+getFood() < 0 || (foodDifference(false, true) < 0 && healthRate() <= -4)))
					{
						//if( (getPopulation() >= std::max(5, getHighestPopulation() - 1)) )
						// We're working poor tiles. What more do you want?
						{
							bWait = false;
						}
					}
				}

				if( !bWait && gCityLogLevel >= 2 )
				{
					logBBAI("      City %S (size %d, highest %d) chooses to conscript with danger: %d, land war: %d, poor tiles: %d%s", getName().GetCString(), getPopulation(), getHighestPopulation(), bDanger, bLandWar, AI_countWorkedPoorPlots(), bGoodValue ? ", good value" : "");
				}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

				if (!bWait)
				{
					conscript();
				}
			}
		}
	}
}

// This function has been heavily edited for K-Mod
void CvCityAI::AI_doHurry(bool bForce)
{
	PROFILE_FUNC();

	FAssert(!isHuman() || isProductionAutomated());

	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	
	if (kOwner.isBarbarian())
	{
		return;
	}

	if (getProduction() == 0 && !bForce)
	{
		return;
	}

	UnitTypes eProductionUnit = getProductionUnit();
	UnitAITypes eProductionUnitAI = getProductionUnitAI();
	BuildingTypes eProductionBuilding = getProductionBuilding();

	for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		if (!canHurry((HurryTypes)iI))
			continue;

		if (bForce)
		{
			hurry((HurryTypes)iI);
			break;
		}

		// gold hurry information
		int iHurryGold = hurryGold((HurryTypes)iI);
		int iGoldTarget = 0;
		int iGoldCost = 0;
		if (iHurryGold > 0)
		{
			if (iHurryGold > kOwner.getGold() - kOwner.AI_goldTarget(true))
				continue; // we don't have enough gold for this hurry type.
			iGoldTarget = kOwner.AI_goldTarget();
			iGoldCost = iHurryGold;
			if (kOwner.getGold() - iHurryGold >= iGoldTarget)
			{
				iGoldCost *= 100;
				iGoldCost /= 100 + 50 * (kOwner.getGold() - iHurryGold) / std::max(iGoldTarget, iHurryGold);
			}
			if (kOwner.isHuman())
				iGoldCost = iGoldCost * 3 / 2;
		}
		

		// population hurry information
		int iHurryAngerLength = hurryAngerLength((HurryTypes)iI);
		int iHurryPopulation = hurryPopulation((HurryTypes)iI);

		int iHappyDiff = 0;
		int iHappy = 0;
		int iPopCost = 0;
		

		if (iHurryPopulation > 0)
		{
			if (!isNoUnhappiness())
			{
				iHappyDiff = iHurryPopulation - GC.getDefineINT("HURRY_POP_ANGER");

				if (iHurryAngerLength > 0 && getHurryAngerTimer() > 1)
					iHappyDiff -= ROUND_DIVIDE((kOwner.AI_getFlavorValue(FLAVOR_GROWTH) > 0 ? 4 : 3) * getHurryAngerTimer(), iHurryAngerLength);
			}

			iHappy = happyLevel() - unhappyLevel();

			if (iHappyDiff > 0 && iGoldCost == 0)
			{
				if (iHappy < 0 && iHurryPopulation < -4*iHappy) // don't kill 4 citizens to remove 1 angry citizen.
				{
					if (gCityLogLevel >= 2)
						logBBAI("      City %S whips to reduce unhappiness", getName().GetCString() );
					hurry((HurryTypes)iI);
					return;
				}
			}
			else if (iHappy + iHappyDiff < 0)
			{
				continue; // not enough happiness to afford this hurry
			}

			if (iHappy + iHappyDiff >= 1 && iGoldCost == 0 && foodDifference() < -iHurryPopulation)
			{
				if (gCityLogLevel >= 2)
					logBBAI("      City %S whips to reduce food loss", getName().GetCString() );
				hurry((HurryTypes)iI);
				return;
			}

			iPopCost = AI_citizenSacrificeCost(iHurryPopulation, iHappy, GC.getDefineINT("HURRY_POP_ANGER"), iHurryAngerLength);
			iPopCost += std::max(0, 6 * -iHappyDiff) * iHurryAngerLength;

			if (kOwner.isHuman())
				iPopCost = iPopCost * 3 / 2;

			// subtract overflow from the cost; but only if we can be confident the city isn't being over-whipped. (iHappyDiff has been adjusted above based on the anger-timer)
			if (iHappyDiff > 0 || isNoUnhappiness() || (iHappy > 1 && (getHurryAngerTimer() <= 1 || iHurryAngerLength == 0)))
			{
				int iOverflow = (hurryProduction((HurryTypes)iI) - productionLeft()); // raw overflow
				iOverflow *= kOwner.AI_getFlavorValue(FLAVOR_PRODUCTION) > 0 ? 6 : 4; // value factor
				iOverflow *= getBaseYieldRateModifier(YIELD_PRODUCTION); // limit which multiplier apply to the overflow
				iOverflow /= std::max(1, getBaseYieldRateModifier(YIELD_PRODUCTION, getProductionModifier()));
				iPopCost -= iOverflow;
			}

			// convert units from 4x commerce to 1x commerce
			iPopCost /= 4;
		}

		int iTotalCost = iPopCost + iGoldCost;

		if (eProductionUnit != NO_UNIT)
		{
			const CvUnitInfo& kUnitInfo = GC.getUnitInfo(eProductionUnit);

			int iValue = 0;
			if (kOwner.AI_isFinancialTrouble())
				iTotalCost = std::max(0, iTotalCost); // overflow is not good when it is being used to build units that we don't want.
			else
			{
				iValue = productionLeft();
				
				// Civ4 Reimagined
				int iProductionMultiplier = getUnitProductionMultiplier(eProductionUnit);
				iValue *= 200 - iProductionMultiplier;
				iValue /= 100;
				
				switch (eProductionUnitAI)
				{
				case UNITAI_WORKER:
				case UNITAI_SETTLE:
				case UNITAI_WORKER_SEA:
					iValue *= kUnitInfo.isFoodProduction() ? 5 : 4;
					iValue += (eProductionUnitAI == UNITAI_SETTLE && area()->getNumAIUnits(getOwnerINLINE(), eProductionUnitAI) == 0
						? 24 : 14) * (getProductionTurnsLeft(eProductionUnit, 1)-1); // cf. value of commerce/turn
					break;
				case UNITAI_SETTLER_SEA:
				case UNITAI_EXPLORE_SEA:
					iValue *= kOwner.AI_getNumAIUnits(eProductionUnitAI) == 0 ? 5 : 4;
					break;
				default:
					if (kUnitInfo.getUnitCombatType() == NO_UNITCOMBAT)
						iValue *= 4;
					else
					{
						// Civ4 Reimagined: Bonus experience with Mercenaries
						if (iGoldCost > 0)
						{
							iValue *= std::max(1, 3 + 5 - getProductionExperience(eProductionUnit));
							iValue /= GET_TEAM(kOwner.getTeam()).getAtWarCount(true,true) > 0 ? 3 : 6;
						}
						
						if (AI_isDanger())
						{
							iValue *= 6;
						}
						else if (kUnitInfo.getDomainType() == DOMAIN_SEA &&
							(area()->getAreaAIType(kOwner.getTeam()) == AREAAI_ASSAULT ||
							area()->getAreaAIType(kOwner.getTeam()) == AREAAI_ASSAULT_ASSIST ||
							area()->getAreaAIType(kOwner.getTeam()) == AREAAI_ASSAULT_MASSING))
						{
							iValue *= 5;
						}
						else
						{
							if (area()->getAreaAIType(kOwner.getTeam()) == AREAAI_DEFENSIVE)
							{
								int iSuccessRating = GET_TEAM(kOwner.getTeam()).AI_getWarSuccessRating();
								iValue *= iSuccessRating < -5 ? (iSuccessRating < -50 ? 6 : 5) : 4;
							}
							else if (kOwner.AI_isDoStrategy(AI_STRATEGY_CRUSH) &&
								(area()->getAreaAIType(kOwner.getTeam()) == AREAAI_OFFENSIVE || area()->getAreaAIType(kOwner.getTeam()) == AREAAI_MASSING))
							{
								int iSuccessRating = GET_TEAM(kOwner.getTeam()).AI_getWarSuccessRating();
								iValue *= iSuccessRating < 35 ? (iSuccessRating < 1 ? 6 : 5) : 4;
							}
							else
							{
								if (area()->getAreaAIType(kOwner.getTeam()) == AREAAI_NEUTRAL)
									iValue *= kOwner.AI_unitCostPerMil() >= kOwner.AI_maxUnitCostPerMil(area(), AI_buildUnitProb()) ? 0 : 3;
								else
									iValue *= 4;
							}
						}
					}
					break;
				}
				iValue /= 4 + std::max(0, -iHappyDiff);
			}
			if (iValue > iTotalCost)
			{
				if( gCityLogLevel >= 2 )
				{
					logBBAI("      City %S (%d) hurries %S. %d pop (%d) + %d gold (%d) to save %d turns. (value %d) (hd %d)",
						getName().GetCString(), getPopulation(), GC.getUnitInfo(eProductionUnit).getDescription(0), iHurryPopulation, iPopCost, iHurryGold, iGoldCost, getProductionTurnsLeft(eProductionUnit, 1), iValue, iHappyDiff);
				}

				hurry((HurryTypes)iI);
				return;
			}
		}
		else if (eProductionBuilding != NO_BUILDING)
		{
			const CvBuildingInfo& kBuildingInfo = GC.getBuildingInfo(eProductionBuilding);

			int iValue = AI_buildingValue(eProductionBuilding) * (getProductionTurnsLeft(eProductionBuilding, 1) - 1);
			
			// Civ4 Reimagined
			if (isWorldWonderClass((BuildingClassTypes)kBuildingInfo.getBuildingClassType()))
			{
				iValue *= 2;
			}
			iValue /= std::max(4, 4 - iHappyDiff) + (iHurryPopulation+1)/2;

			if (iValue > iTotalCost)
			{
				if( gCityLogLevel >= 2 )
				{
					logBBAI("      City %S (%d) hurries %S. %d pop (%d) + %d gold (%d) to save %d turns with %d building value (%d) (hd %d)",
						getName().GetCString(), getPopulation(), kBuildingInfo.getDescription(0), iHurryPopulation, iPopCost, iHurryGold, iGoldCost,
						getProductionTurnsLeft(eProductionBuilding, 1), AI_buildingValue(eProductionBuilding), iValue, iHappyDiff);
				}

				hurry((HurryTypes)iI);
				return;
			}
		}
	}
}


// Improved use of emphasize by Blake, to go with his whipping strategy - thank you!
void CvCityAI::AI_doEmphasize()
{
	PROFILE_FUNC();

	FAssert(!isHuman());

	bool bFirstTech;
	bool bEmphasize;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/08/10                                jdog5000      */
/*                                                                                              */
/* Victory Strategy AI                                                                          */
/************************************************************************************************/
	bool bCultureVictory = GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	//Note from Blake:
	//Emphasis proved to be too clumsy to manage AI economies,
	//as such it's been nearly completely phased out by 
	//the AI_specialYieldMultiplier system which allows arbitary
	//value-boosts and works very well.
	//Ideally the AI should never use emphasis.
	int iI;
	
	if (GET_PLAYER(getOwnerINLINE()).getCurrentResearch() != NO_TECH)
	{
		bFirstTech = GET_PLAYER(getOwnerINLINE()).AI_isFirstTech(GET_PLAYER(getOwnerINLINE()).getCurrentResearch());
	}
	else
	{
		bFirstTech = false;
	}

	int iPopulationRank = findPopulationRank();

	for (iI = 0; iI < GC.getNumEmphasizeInfos(); iI++)
	{
		bEmphasize = false;

		if (GC.getEmphasizeInfo((EmphasizeTypes)iI).getYieldChange(YIELD_FOOD) > 0)
		{

		}

		if (GC.getEmphasizeInfo((EmphasizeTypes)iI).getYieldChange(YIELD_PRODUCTION) > 0)
		{

		}
		
		if (AI_specialYieldMultiplier(YIELD_PRODUCTION) < 50)
		{
			if (GC.getEmphasizeInfo((EmphasizeTypes)iI).getYieldChange(YIELD_COMMERCE) > 0)
			{
				if (bFirstTech)
				{
					bEmphasize = true;
				}
			}

			if (GC.getEmphasizeInfo((EmphasizeTypes)iI).getCommerceChange(COMMERCE_RESEARCH) > 0)
			{
				if (bFirstTech && !bCultureVictory)
				{
					if (iPopulationRank < ((GET_PLAYER(getOwnerINLINE()).getNumCities() / 4) + 1))
					{
						bEmphasize = true;
					}
				}
			}
			
			if (GC.getEmphasizeInfo((EmphasizeTypes)iI).isGreatPeople())
			{
				int iHighFoodTotal = 0;
				int iHighFoodPlotCount = 0;
				int iHighHammerPlotCount = 0;
				int iHighHammerTotal = 0;
				int iGoodFoodSink = 0;
				int iFoodPerPop = GC.getFOOD_CONSUMPTION_PER_POPULATION();
				for (int iPlot = 0; iPlot < NUM_CITY_PLOTS; iPlot++)
				{
					CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iPlot);
					if (pLoopPlot != NULL && pLoopPlot->getWorkingCity() == this)
					{
						int iFood = pLoopPlot->getYield(YIELD_FOOD);
						if (iFood > iFoodPerPop)
						{
							iHighFoodTotal += iFood;
							iHighFoodPlotCount++;
						}
						int iHammers = pLoopPlot->getYield(YIELD_PRODUCTION);
						if ((iHammers >= 3) && ((iHammers + iFood) >= 4))
						{
							iHighHammerPlotCount++;
							iHighHammerTotal += iHammers;
						}
						int iCommerce = pLoopPlot->getYield(YIELD_COMMERCE);
						if ((iCommerce * 2 + iHammers * 3) > 9)
						{
							iGoodFoodSink += std::max(0, iFoodPerPop - iFood);
						}
					}
				}
				
				if ((iHighFoodTotal + iHighFoodPlotCount - iGoodFoodSink) >= foodConsumption(true))
				{
					if ((iHighHammerPlotCount < 2) && (iHighHammerTotal < (getPopulation())))
					{
						if (AI_countGoodTiles(true, false, 100, true) < getPopulation())
						{
							bEmphasize = true;
						}
					}
				}
			}
		}

		AI_setEmphasize(((EmphasizeTypes)iI), bEmphasize);
	}
}

bool CvCityAI::AI_chooseUnit(UnitAITypes eUnitAI, int iOdds)
{
	UnitTypes eBestUnit;
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE()); // Civ4 Reimagined

	if (eUnitAI != NO_UNITAI)
	{
		eBestUnit = AI_bestUnitAI(eUnitAI);
	}
	else
	{
		eBestUnit = AI_bestUnit(false, NO_ADVISOR, &eUnitAI);
	}

	if (eBestUnit != NO_UNIT)
	{
		/* original BBAI code
		if( iOdds < 0 ||
			getUnitProduction(eBestUnit) > 0 ||
			GC.getGameINLINE().getSorenRandNum(100, "City AI choose unit") < iOdds ) */
		// K-Mod. boost the odds based on our completion percentage.
		if (iOdds < 0 ||
			GC.getGameINLINE().getSorenRandNum(100, "City AI choose unit")
			< iOdds + 100 * getUnitProduction(eBestUnit) / std::max(1, getProductionNeeded(eBestUnit)))
		// K-Mod end
		{
			// Civ4 Reimagined
			if (getUnitProductionMultiplier(eBestUnit) > 0)
			{
				pushOrder(ORDER_TRAIN, eBestUnit, eUnitAI);
				return true;
			}
			else
			{
				// Civ4 Reimagined: Hire mercenary when not having the resources
				for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
				{
					if (!GET_PLAYER(getOwnerINLINE()).canHurry((HurryTypes)iI))
						continue;
					int iHurryGold = getHurryGold((HurryTypes)iI, getHurryCost(false, eBestUnit, false));
					if (iHurryGold > 0 && iHurryGold <= kOwner.getGold() - kOwner.AI_goldTarget(true))
					{
						pushOrder(ORDER_TRAIN, eBestUnit, eUnitAI);
						if (gCityLogLevel >= 2)
							logBBAI("      City %S hires mercenary without resource", getName().GetCString() );
						hurry((HurryTypes)iI);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CvCityAI::AI_chooseUnit(UnitTypes eUnit, UnitAITypes eUnitAI)
{
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE()); // Civ4 Reimagined
	if (eUnit != NO_UNIT)
	{
		// Civ4 Reimagined
		if (getUnitProductionMultiplier(eUnit) > 0)
		{
			pushOrder(ORDER_TRAIN, eUnit, eUnitAI);
			return true;
		}
		else
		{
			// Civ4 Reimagined: Hire mercenary when not having the resources
			for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
			{
				if (!GET_PLAYER(getOwnerINLINE()).canHurry((HurryTypes)iI))
						continue;
				int iHurryGold = getHurryGold((HurryTypes)iI, getHurryCost(false, eUnit, false));
				if (iHurryGold > 0 && iHurryGold <= kOwner.getGold() - kOwner.AI_goldTarget(true))
				{
					pushOrder(ORDER_TRAIN, eUnit, eUnitAI);
					if (gCityLogLevel >= 2)
						logBBAI("      City %S hires mercenary without resource", getName().GetCString() );
					hurry((HurryTypes)iI);
					return true;
				}
			}
		}
	}
	return false;	
}


bool CvCityAI::AI_chooseDefender()
{
	if (plot()->plotCheck(PUF_isUnitAIType, UNITAI_CITY_SPECIAL, -1, getOwnerINLINE()) == NULL)
	{
		if (AI_chooseUnit(UNITAI_CITY_SPECIAL))
		{
			return true;
		}
	}

	if (plot()->plotCheck(PUF_isUnitAIType, UNITAI_CITY_COUNTER, -1, getOwnerINLINE()) == NULL)
	{
		if (AI_chooseUnit(UNITAI_CITY_COUNTER))
		{
			return true;
		}
	}

	if (AI_chooseUnit(UNITAI_CITY_DEFENSE))
	{
		return true;
	}

	return false;
}

bool CvCityAI::AI_chooseLeastRepresentedUnit(UnitTypeWeightArray &allowedTypes, int iOdds)
{
	int iValue;

	UnitTypeWeightArray::iterator it;
	
	/* original bts code
 	std::multimap<int, UnitAITypes, std::greater<int> > bestTypes;
 	std::multimap<int, UnitAITypes, std::greater<int> >::iterator best_it; */
	std::vector<std::pair<int, UnitAITypes> > bestTypes; // K-Mod
 
	for (it = allowedTypes.begin(); it != allowedTypes.end(); ++it)
	{
		iValue = it->second;
		iValue *= 750 + GC.getGameINLINE().getSorenRandNum(250, "AI choose least represented unit");
		iValue /= 1 + GET_PLAYER(getOwnerINLINE()).AI_totalAreaUnitAIs(area(), it->first);
		//bestTypes.insert(std::make_pair(iValue, it->first));
		bestTypes.push_back(std::make_pair(iValue, it->first)); // K-Mod
	}
 
	// K-Mod
	std::sort(bestTypes.begin(), bestTypes.end(), std::greater<std::pair<int, UnitAITypes> >());
	std::vector<std::pair<int, UnitAITypes> >::iterator best_it;
	// K-Mod end
 	for (best_it = bestTypes.begin(); best_it != bestTypes.end(); ++best_it)
 	{
		if (AI_chooseUnit(best_it->second, iOdds))
		{
			return true;
		}
 	}
	return false;
}

bool CvCityAI::AI_bestSpreadUnit(bool bMissionary, bool bExecutive, int iBaseChance, UnitTypes* eBestSpreadUnit, int* iBestSpreadUnitValue)
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	CvTeamAI& kTeam = GET_TEAM(getTeam());
	CvGame& kGame = GC.getGame();
	
	FAssert(eBestSpreadUnit != NULL && iBestSpreadUnitValue != NULL);

	int iBestValue = 0;

	if (bMissionary)
	{
		for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); iReligion++)
		{
			ReligionTypes eReligion = (ReligionTypes)iReligion;
			if (isHasReligion(eReligion))
			{
				int iHasCount = kPlayer.getHasReligionCount(eReligion);
				FAssert(iHasCount > 0);
				int iRoll = (iHasCount > 4) ? iBaseChance : (((100 - iBaseChance) / iHasCount) + iBaseChance);
				if (kPlayer.AI_isDoStrategy(AI_STRATEGY_MISSIONARY))
				{
					iRoll *= (kPlayer.getStateReligion() == eReligion) ? 170 : 65;
					iRoll /= 100;
				}
				if (kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE2))
				{
					iRoll += 25;
				}
				else if (!kTeam.hasHolyCity(eReligion) && !(kPlayer.getStateReligion() == eReligion && iHasCount < kPlayer.getNumCities()))
				{
					//iRoll /= 2;
					// Civ4 Reimagined
					iRoll /= 20;
					/* if (kPlayer.isNoNonStateReligionSpread())
					{
						iRoll /= 2;
					} */ // disabled by K-mod
				}

				if (iRoll > kGame.getSorenRandNum(100, "AI choose missionary"))
				{
					int iReligionValue = kPlayer.AI_missionaryValue(area(), eReligion);
					if (iReligionValue > 0)
					{
						for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
						{
							UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

							if (eLoopUnit != NO_UNIT)
							{
								CvUnitInfo& kUnitInfo = GC.getUnitInfo(eLoopUnit);
								if (kUnitInfo.getReligionSpreads(eReligion) > 0)
								{
									if (canTrain(eLoopUnit))
									{
										int iValue = iReligionValue;
										iValue /= kUnitInfo.getProductionCost();
										
										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											*eBestSpreadUnit = eLoopUnit;
											*iBestSpreadUnitValue = iReligionValue;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	if (bExecutive)
	{
		for (int iCorporation = 0; iCorporation < GC.getNumCorporationInfos(); iCorporation++)
		{
			CorporationTypes eCorporation = (CorporationTypes)iCorporation;
			// Civ4 Reimagined
			if (isActiveCorporation(eCorporation) && kTeam.hasHeadquarters(eCorporation))
			{
				int iHasCount = kPlayer.getHasCorporationCount(eCorporation);
				FAssert(iHasCount > 0);
				int iRoll = 10 + (iHasCount > 4) ? iBaseChance : (((100 - iBaseChance) / iHasCount) + iBaseChance);
				/* original bts code
				if (!kTeam.hasHeadquarters(eCorporation))
				{
					iRoll /= 8;
				} */
				// K-Mod
				
				if (iHasCount >= kPlayer.getNumCities())
				{
					iRoll /= 2;
				}
				
				// Civ4 Reimagined
				for (int iCorp = 0; iCorp < GC.getNumCorporationInfos(); iCorp++)
				{
					if (kGame.isCorporationFounded((CorporationTypes)iCorp) && kGame.isCompetingCorporation(eCorporation, (CorporationTypes)iCorp))
					{
						if (kTeam.hasHeadquarters(eCorporation))
						{
							if (kPlayer.AI_corporationValue((CorporationTypes)iCorp, this) > kPlayer.AI_corporationValue(eCorporation, this))
							{
								iRoll = 0;
							}
						}
						else if (2 * kPlayer.AI_corporationValue((CorporationTypes)iCorp, this) > 3 * kPlayer.AI_corporationValue(eCorporation, this)) 
						{
							// Civ4 Reimagined, was 2
							iRoll /= 3;
						}
					}
				}
				
				// K-Mod end
				
				if (iRoll > kGame.getSorenRandNum(100, "AI choose executive"))
				{
					int iCorporationValue = kPlayer.AI_executiveValue(area(), eCorporation);
					if (iCorporationValue > 0)
					{
						for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
						{
							UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

							if (eLoopUnit != NO_UNIT)
							{
								CvUnitInfo& kUnitInfo = GC.getUnitInfo(eLoopUnit);
								if (kUnitInfo.getCorporationSpreads(eCorporation) > 0)
								{
									if (canTrain(eLoopUnit))
									{
										/* original bts code
										int iValue = iCorporationValue;
										iValue /= kUnitInfo.getProductionCost();

										int iLoop;
										int iTotalCount = 0;
										int iPlotCount = 0;
										for (CvUnit* pLoopUnit = kPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kPlayer.nextUnit(&iLoop))
										{
											if ((pLoopUnit->AI_getUnitAIType() == UNITAI_MISSIONARY) && (pLoopUnit->getUnitInfo().getCorporationSpreads(eCorporation) > 0))
											{
												iTotalCount++;
												if (pLoopUnit->plot() == plot())
												{
													iPlotCount++;
												}
											}
										}
										iCorporationValue /= std::max(1, (iTotalCount / 4) + iPlotCount); */
										// K-Mod
										int iExistingUnits = kPlayer.getUnitClassCount((UnitClassTypes)iI) + kPlayer.getUnitClassMaking((UnitClassTypes)iI)/2;
										iCorporationValue *= 3;
										iCorporationValue /= iExistingUnits > 1 ? 2 + iExistingUnits : 3;

										int iValue = iCorporationValue;
										iValue /= kUnitInfo.getProductionCost();
										// K-Mod end

										/* original bts code
										int iCost = std::max(0, GC.getCorporationInfo(eCorporation).getSpreadCost() * (100 + GET_PLAYER(getOwnerINLINE()).calculateInflationRate()));
										iCost /= 100;

										if (kPlayer.getGold() >= iCost)
										{
											iCost *= GC.getDefineINT("CORPORATION_FOREIGN_SPREAD_COST_PERCENT");
											iCost /= 100;
											if (kPlayer.getGold() < iCost && iTotalCount > 1)
											{
												iCorporationValue /= 2;
											}
										}
										else if (iTotalCount > 1)
										{
											iCorporationValue /= 5;
										} */
										if (iValue > iBestValue)
										{
											iBestValue = iValue;
											*eBestSpreadUnit = eLoopUnit;
											*iBestSpreadUnitValue = iCorporationValue;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	return (*eBestSpreadUnit != NULL);
}

bool CvCityAI::AI_chooseBuilding(int iFocusFlags, int iMaxTurns, int iMinThreshold, int iOdds)
{
	BuildingTypes eBestBuilding;

	eBestBuilding = AI_bestBuildingThreshold(iFocusFlags, iMaxTurns, iMinThreshold);

	if (eBestBuilding != NO_BUILDING)
	{
		/* bbai code
		if( iOdds < 0 || 
			getBuildingProduction(eBestBuilding) > 0 ||
			GC.getGameINLINE().getSorenRandNum(100,"City AI choose building") < iOdds ) */
		// K-Mod
		int iRand;
		if (iOdds < 0 ||
			(iRand = GC.getGameINLINE().getSorenRandNum(100, "City AI choose building")) < iOdds ||
			iRand < iOdds + 100*getBuildingProduction(eBestBuilding)/std::max(1, getProductionNeeded(eBestBuilding)))
		// K-Mod end
		{
			pushOrder(ORDER_CONSTRUCT, eBestBuilding);
			return true;
		}
	}

	return false;
}

bool CvCityAI::AI_chooseProject()
{
	ProjectTypes eBestProject;

	eBestProject = AI_bestProject();

	if (eBestProject != NO_PROJECT)
	{
		pushOrder(ORDER_CREATE, eBestProject);
		return true;
	}

	return false;
}


bool CvCityAI::AI_chooseProcess(CommerceTypes eCommerceType)
{
	ProcessTypes eBestProcess;

	eBestProcess = AI_bestProcess(eCommerceType);

	if (eBestProcess != NO_PROCESS)
	{
		pushOrder(ORDER_MAINTAIN, eBestProcess);
		return true;
	}

	return false;
}


// Returns true if a worker was added to a plot...
bool CvCityAI::AI_addBestCitizen(bool bWorkers, bool bSpecialists, int* piBestPlot, SpecialistTypes* peBestSpecialist)
{
	PROFILE_FUNC();

	int iGrowthValue = AI_growthValuePerFood(); // K-Mod
	
	// Civ4 Reimagined
	if ( happyLevel() - unhappyLevel(0) <= 0 )
	{
		iGrowthValue = 0;
	}

	int iBestValue = -1;
	SpecialistTypes eBestSpecialist = NO_SPECIALIST;

	if (bSpecialists)
	{
		// K-Mod. I've deleted a lot of original BtS code for handling forced specialists and replaced it with a simpler method.
		// We allow specialists only if they are either a forced type, or there are no forced types available.
		// (the original code attempted to assign specialists in the same proportions to their force counts.)
		bool bForcedSpecAvailable = false;
		for (SpecialistTypes i = (SpecialistTypes)0; i < GC.getNumSpecialistInfos(); i = (SpecialistTypes)(i+1))
		{
			if (getForceSpecialistCount(i) > 0 && isSpecialistValid(i, 1))
				bForcedSpecAvailable = true;
		}
		
		for (SpecialistTypes i = (SpecialistTypes)0; i < GC.getNumSpecialistInfos(); i = (SpecialistTypes)(i+1))
		{
			if (isSpecialistValid(i, 1) && (!bForcedSpecAvailable || getForceSpecialistCount(i) > 0))
			{
				int iValue = AI_specialistValue(i, false, false, iGrowthValue);
				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestSpecialist = i;
				}
			}
		}
	}

	int iBestPlot = -1;
	if (bWorkers)
	{
		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			if (iI != CITY_HOME_PLOT)
			{
				if (!isWorkingPlot(iI))
				{
					CvPlot* pLoopPlot = getCityIndexPlot(iI);

					if (pLoopPlot != NULL)
					{
						if (canWork(pLoopPlot))
						{
							int iValue = AI_plotValue(pLoopPlot, false, false, false, iGrowthValue);

							if (iValue > iBestValue)
							{
								iBestValue = iValue;
								iBestPlot = iI;
								eBestSpecialist = NO_SPECIALIST;
							}
						}
					}
				}
			}
		}
	}
	
	if (eBestSpecialist != NO_SPECIALIST)
	{
		changeSpecialistCount(eBestSpecialist, 1);
		if (piBestPlot != NULL)
		{
			FAssert(peBestSpecialist != NULL);
			*peBestSpecialist = eBestSpecialist;
			*piBestPlot = -1;
		}
		return true;
	}
	else if (iBestPlot != -1)
	{
		setWorkingPlot(iBestPlot, true);
		if (piBestPlot != NULL)
		{
			FAssert(peBestSpecialist != NULL);
			*peBestSpecialist = NO_SPECIALIST;
			*piBestPlot = iBestPlot;
			
		}
		return true;
	}

	return false;
}


// Returns true if a worker was removed from a plot...
bool CvCityAI::AI_removeWorstCitizen(SpecialistTypes eIgnoreSpecialist)
{
	// if we are using more specialists than the free ones we get
	if (extraFreeSpecialists() < 0)
	{
		// does generic 'citizen' specialist exist?
		if (GC.getDefineINT("DEFAULT_SPECIALIST") != NO_SPECIALIST)
		{
			// is ignore something other than generic citizen?
			if (eIgnoreSpecialist != GC.getDefineINT("DEFAULT_SPECIALIST"))
			{
				// do we have at least one more generic citizen than we are forcing?
				if (getSpecialistCount((SpecialistTypes)(GC.getDefineINT("DEFAULT_SPECIALIST"))) > getForceSpecialistCount((SpecialistTypes)(GC.getDefineINT("DEFAULT_SPECIALIST"))))
				{
					// remove the extra generic citzen
					changeSpecialistCount(((SpecialistTypes)(GC.getDefineINT("DEFAULT_SPECIALIST"))), -1);
					return true;
				}
			}
		}
	}

	//bAvoidGrowth = AI_avoidGrowth();
	//bIgnoreGrowth = AI_ignoreGrowth();
	int iGrowthValue = AI_growthValuePerFood();

	int iWorstValue = MAX_INT;
	SpecialistTypes eWorstSpecialist = NO_SPECIALIST;
	int iWorstPlot = -1;

	// if we are using more specialists than the free ones we get
	if (extraFreeSpecialists() < 0)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			if (eIgnoreSpecialist != iI)
			{
				if (getSpecialistCount((SpecialistTypes)iI) > getForceSpecialistCount((SpecialistTypes)iI))
				{
					int iValue = AI_specialistValue((SpecialistTypes)iI, true, false, iGrowthValue);

					if (iValue < iWorstValue)
					{
						iWorstValue = iValue;
						eWorstSpecialist = ((SpecialistTypes)iI);
						iWorstPlot = -1;
					}
				}
			}
		}
	}
	
	// check all the plots we working
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		if (iI != CITY_HOME_PLOT)
		{
			if (isWorkingPlot(iI))
			{
				CvPlot* pLoopPlot = getCityIndexPlot(iI);

				if (pLoopPlot != NULL)
				{
					int iValue = AI_plotValue(pLoopPlot, true, false, false, iGrowthValue);

					if (iValue < iWorstValue)
					{
						iWorstValue = iValue;
						eWorstSpecialist = NO_SPECIALIST;
						iWorstPlot = iI;
					}
				}
			}
		}
	}

	if (eWorstSpecialist != NO_SPECIALIST)
	{
		changeSpecialistCount(eWorstSpecialist, -1);
		return true;
	}
	else if (iWorstPlot != -1)
	{
		setWorkingPlot(iWorstPlot, false);
		return true;
	}
	
	// if we still have not removed one, then try again, but do not ignore the one we were told to ignore
	if (extraFreeSpecialists() < 0)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			if (getSpecialistCount((SpecialistTypes)iI) > 0)
			{
				int iValue = AI_specialistValue((SpecialistTypes)iI, true, false, iGrowthValue);

				if (iValue < iWorstValue)
				{
					iWorstValue = iValue;
					eWorstSpecialist = ((SpecialistTypes)iI);
					iWorstPlot = -1;
				}
			}
		}
	}

	if (eWorstSpecialist != NO_SPECIALIST)
	{
		changeSpecialistCount(eWorstSpecialist, -1);
		// K-Mod. If we had to remove a forced specialist, reduce the force count to match what we did.
		if (getSpecialistCount(eWorstSpecialist) < getForceSpecialistCount(eWorstSpecialist)) 
		{
			setForceSpecialistCount(eWorstSpecialist, getSpecialistCount(eWorstSpecialist));
		}

		return true;
	}

	return false;
}

// This function has been completely rewritten for K-Mod - original code deleted
void CvCityAI::AI_juggleCitizens()
{
	PROFILE_FUNC();

	int iTotalFreeSpecialists = totalFreeSpecialists();
	bool bAnyForcedSpecs = isSpecialistForced();

	// work out how much food deficit would be acceptable
	int iStarvingAllowance = 0;
	{
		int iFoodLevel = getFood();
		int iFoodToGrow = growthThreshold();
		int iHappinessLevel = happyLevel() - unhappyLevel(0);

		if (AI_isEmphasizeAvoidGrowth() || iHappinessLevel < (isHuman() ? 0 : 1))
		{
			iStarvingAllowance = std::max(0, (iFoodLevel - std::max(1, ((7 * iFoodToGrow) / 10))));
			iStarvingAllowance /= (iHappinessLevel+getEspionageHappinessCounter()/2 >= 0 ? 2 : 1);
		}
	}

	//

	typedef std::pair<int, std::pair<bool, int> > PotentialJob_t; // (value, (isSpecialist, plot/specialist index))
	// I'd like to use std::tuple<int, bool, int>, but it isn't supported by this version of C++
	std::vector<PotentialJob_t> worked_jobs;
	std::vector<PotentialJob_t> unworked_jobs;
	std::vector<std::pair<bool, int> > new_jobs; // jobs assigned by this juggling process

	bool bDone = false;
	int iCycles = 0;
	do
	{
		int iGrowthValue = AI_growthValuePerFood(); // recalcuate on each cycle?
		int iFoodPerTurn = getYieldRate(YIELD_FOOD) - foodConsumption();

		worked_jobs.clear();
		unworked_jobs.clear();

		// populate jobs lists.
		// evaluate plots
		for (int i = 1; i < NUM_CITY_PLOTS; i++)
		{
			CvPlot* pLoopPlot = getCityIndexPlot(i);

			if (pLoopPlot == NULL)
				continue;

			if (isWorkingPlot(i))
			{
				int iValue = AI_plotValue(pLoopPlot, false, false, iFoodPerTurn >= 0, iGrowthValue);
				worked_jobs.push_back(PotentialJob_t(iValue, std::make_pair(false, i)));
				// Note: the juggling process works better if worked and unworked plots are compared in the same way.
				// So I'm using 'bRemove = false' here even though we would be removing this worker.
			}
			else if (canWork(pLoopPlot))
			{
				int iValue = AI_plotValue(pLoopPlot, false, false, iFoodPerTurn >= 0, iGrowthValue);
				unworked_jobs.push_back(PotentialJob_t(iValue, std::make_pair(false, i)));
			}
		}

		// check if it is still possible to assign new specialists of types that are forced
		bool bForcedSpecAvailable = false;
		if (bAnyForcedSpecs)
		{
			for (SpecialistTypes i = (SpecialistTypes)0; i < GC.getNumSpecialistInfos(); i = (SpecialistTypes)(i+1))
			{
				if (getForceSpecialistCount(i) > 0 && isSpecialistValid(i, 1))
					bForcedSpecAvailable = true;
			}
		}

		// evaluate specialists
		for (SpecialistTypes i = (SpecialistTypes)0; i < GC.getNumSpecialistInfos(); i = (SpecialistTypes)(i+1))
		{
			if (getSpecialistCount(i) > getForceSpecialistCount(i))
			{
				// don't allow unforced specialists unless none of the forced type are available
				int iValue = bForcedSpecAvailable && getForceSpecialistCount(i) == 0
					? 0
					: AI_specialistValue(i, false, false, iGrowthValue);
				worked_jobs.push_back(PotentialJob_t(iValue, std::make_pair(true, i)));
			}
			if (isSpecialistValid(i, 1))
			{
				int iValue = bForcedSpecAvailable && getForceSpecialistCount(i) == 0
					? 0
					: AI_specialistValue(i, false, false, iGrowthValue);
				unworked_jobs.push_back(PotentialJob_t(iValue, std::make_pair(true, i)));

				if (getForceSpecialistCount(i) > 0)
					bForcedSpecAvailable = true;
			}
		}

		//
		std::sort(worked_jobs.begin(), worked_jobs.end());
		std::sort(unworked_jobs.begin(), unworked_jobs.end(), std::greater<PotentialJob_t>());
		// if values are equal - prefer to work the specialist job. (perhaps a different city can use the plot)
		// Note: PotentialJob_t is (value, (bSpecialist, index)), and bSpecialist == true is a higher value than bSpecialist == false.

		std::vector<PotentialJob_t>::iterator worked_it = worked_jobs.begin();
		std::vector<PotentialJob_t>::iterator unworked_it = unworked_jobs.begin();
		bool bTakeNewJob = false;

		while (!bTakeNewJob && worked_it != worked_jobs.end() && unworked_it != unworked_jobs.end()
			&& unworked_it->first > worked_it->first)
		{
			bTakeNewJob = true; // default position is the take the new job.

			// Don't take the job if it would make us starve
			int iCurrentFood = worked_it->second.first
				? GET_PLAYER(getOwnerINLINE()).specialistYield((SpecialistTypes)worked_it->second.second, YIELD_FOOD)
				: getCityIndexPlot(worked_it->second.second)->getYield(YIELD_FOOD);

			int iNextFood = unworked_it->second.first
				? GET_PLAYER(getOwnerINLINE()).specialistYield((SpecialistTypes)unworked_it->second.second, YIELD_FOOD)
				: getCityIndexPlot(unworked_it->second.second)->getYield(YIELD_FOOD);

			if (iFoodPerTurn >= 0 && iFoodPerTurn + iNextFood - iCurrentFood + iStarvingAllowance < 0)
			{
				bTakeNewJob = false;
			}
			// block removal of free specialists
			else if (worked_it->second.first && !unworked_it->second.first &&
				getSpecialistPopulation() <= iTotalFreeSpecialists)
			{
				bTakeNewJob = false;
			}
			// if forced specialists are still available, don't allow non-forced specialists.
			else if (bForcedSpecAvailable && unworked_it->second.first && getForceSpecialistCount((SpecialistTypes)unworked_it->second.second) == 0 &&
				unworked_it->second.first && getForceSpecialistCount((SpecialistTypes)unworked_it->second.second) == 0)
			{
				bTakeNewJob = false;
			}
			// don't remove jobs that were assigned by the juggling process
			else if (std::find(new_jobs.begin(), new_jobs.end(), worked_it->second) != new_jobs.end())
			{
				bTakeNewJob = false;
			}
			// finally, don't take the new job if a direct comparison shows that it is not more valuable than the old job.
			// (exception, switching away from zero-value jobs, such as unwanted specialists)
			else if (worked_it->first > 0 && AI_jobChangeValue(unworked_it->second, worked_it->second, false, false, iGrowthValue) <= 0)
			{
				bTakeNewJob = false;
			}

			// If we can't do this job switch, check another option. Loop through worked jobs first, then unworked jobs.
			if (!bTakeNewJob)
			{
				++worked_it;
				if (worked_it == worked_jobs.end() || worked_it->first >= unworked_it->first)
				{
					worked_it = worked_jobs.begin();
					++unworked_it;
				}
			}
			// else we're done.
		}

		if (!bTakeNewJob)
		{
			bDone = true; // no more job swaps. So we're finished.
		}
		else
		{
			// remove the current job
			if (worked_it->second.first)
			{
				FAssert(getSpecialistCount((SpecialistTypes)worked_it->second.second) > 0);
				changeSpecialistCount((SpecialistTypes)worked_it->second.second, -1);
			}
			else
			{
				FAssert(isWorkingPlot(worked_it->second.second));
				setWorkingPlot(worked_it->second.second, false);
			}

			// assign the new job
			if (unworked_it->second.first)
			{
				FAssert(isSpecialistValid((SpecialistTypes)unworked_it->second.second, 1));
				changeSpecialistCount((SpecialistTypes)unworked_it->second.second, 1);
			}
			else
			{
				FAssert(!isWorkingPlot(unworked_it->second.second));
				setWorkingPlot(unworked_it->second.second, true);
			}
			// add the new job to the new jobs list
			new_jobs.push_back(unworked_it->second);
		}

		if (iCycles > getPopulation() + iTotalFreeSpecialists)
		{
			// This isn't a serious problem. I just want to know how offen it happens.
			//FAssertMsg(false, "juggle citizens failed to find a stable solution.");
			PROFILE("juggle citizen failure");
			bDone = true;
		}
		iCycles++;
	} while (!bDone);

	// Record keeping, to test efficiency.
#ifdef LOG_JUGGLE_CITIZENS
	{
		TCHAR message[20];
		_snprintf(message, 20, "%d+%d : %d\n", getPopulation(), iTotalFreeSpecialists, iCycles);
		gDLL->logMsg("juggle_log.txt", message ,false, false);
	}
#endif
}

// K-Mod. Estimate the cost of recovery after losing iQuantity number of citizens in this city.
// (units of 4x commerce.)
int CvCityAI::AI_citizenSacrificeCost(int iCitLoss, int iHappyLevel, int iNewAnger, int iAngerTimer)
{
	PROFILE_FUNC();

	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	iCitLoss = std::min(getPopulation()-1, iCitLoss);
	if (iCitLoss < 1)
		return 0;

	if (isNoUnhappiness())
	{
		iNewAnger = 0;
		iAngerTimer = 0;
	}

	short iYields[NUM_YIELD_TYPES] = {};
	const short iYieldWeights[NUM_YIELD_TYPES] =
	{
		9, // food
		7, // production
		4, // commerce
	};
	std::vector<int> job_scores;
	int iTotalScore = 0;

	for (int i = 0; i < -iHappyLevel; i++)
		job_scores.push_back(0);

	if ((int)job_scores.size() < iCitLoss)
	{
		FAssert(CITY_HOME_PLOT == 0); // this is why the following loop starts at 1
		for (int i = 1; i < NUM_CITY_PLOTS; i++)
		{
			if (isWorkingPlot(i))
			{
				CvPlot* pLoopPlot = getCityIndexPlot(i);
				FAssert(pLoopPlot && pLoopPlot->getWorkingCity() == this);

				// ideally, the plots would be scored using AI_plotValue, but I'm worried that would be too slow.
				// so here's a really rough estimate of the plot value
				int iPlotScore = 0;
				for (int j = 0; j < NUM_YIELD_TYPES; j++)
				{
					int y = pLoopPlot->getYield((YieldTypes)j);
					iYields[j] += y;
					iPlotScore += y * iYieldWeights[j];
				}
				if (pLoopPlot->getImprovementType() != NO_IMPROVEMENT && GC.getImprovementInfo(pLoopPlot->getImprovementType()).getImprovementUpgrade() != NO_IMPROVEMENT)
				{
					iYields[YIELD_COMMERCE] += 2;
					iPlotScore += 2 * iYieldWeights[YIELD_COMMERCE];
				}
				iTotalScore += iPlotScore;
				job_scores.push_back(iPlotScore);
			}
		}
	}
	if ((int)job_scores.size() < iCitLoss)
	{
		for (int i = 0; i < GC.getNumSpecialistInfos(); i++)
		{
			if (getSpecialistCount((SpecialistTypes)i) > 0)
			{
				// very rough..
				int iSpecScore = 0;
				for (int j = 0; j < NUM_YIELD_TYPES; j++)
				{
					int y = kOwner.specialistYield((SpecialistTypes)i, (YieldTypes)j)*3/2;
					iYields[j] += y;
					iSpecScore += y * iYieldWeights[j];
				}
				for (int j = 0 ; j < NUM_COMMERCE_TYPES; j++)
				{
					int c = kOwner.specialistCommerce((SpecialistTypes)i, (CommerceTypes)j)*3/2;
					iYields[YIELD_COMMERCE] += c;
					iSpecScore += c * iYieldWeights[YIELD_COMMERCE];
				}
				iTotalScore += iSpecScore * getSpecialistCount((SpecialistTypes)i);
				job_scores.resize(job_scores.size() + getSpecialistCount((SpecialistTypes)i), iSpecScore);
			}
		}
	}

	if ((int)job_scores.size() < iCitLoss)
	{
		FAssertMsg(false, "Not enough job data to calculate citizen loss cost.");
		int iBogusData = (1+GC.getFOOD_CONSUMPTION_PER_POPULATION()) * iYieldWeights[YIELD_FOOD];
		job_scores.resize(iCitLoss, iBogusData);
		iTotalScore += iBogusData;
	}

	FAssert((int)job_scores.size() >= iCitLoss);
	std::partial_sort(job_scores.begin(), job_scores.begin()+iCitLoss, job_scores.end());
	int iAverageScore = ROUND_DIVIDE(iTotalScore, job_scores.size());

	int iWastedFood = -healthRate();
	int iTotalFood = getYieldRate(YIELD_FOOD) - iWastedFood;

	// calculate the total cost-per-turn from missing iCitLoss citizens.
	int iBaseCostPerTurn = 0;
	for (int j = 0; j < NUM_YIELD_TYPES; j++)
	{
		if (iYields[j] > 0)
			iBaseCostPerTurn += 4 * iYields[j] * AI_yieldMultiplier((YieldTypes)j) * kOwner.AI_yieldWeight((YieldTypes)j) / 100;
	}
	// reduce value of food used for production - otherwise our citizens might be overvalued due to working extra food.
	if (isFoodProduction())
	{
		int iExtraFood = iTotalFood - getPopulation() * GC.getFOOD_CONSUMPTION_PER_POPULATION();
		iExtraFood = std::min(iExtraFood, iYields[YIELD_FOOD]*AI_yieldMultiplier(YIELD_FOOD)/100);
		iBaseCostPerTurn -= 4 * iExtraFood * (kOwner.AI_yieldWeight(YIELD_FOOD)-kOwner.AI_yieldWeight(YIELD_PRODUCTION)); // note that we're keeping a factor of 100.
	}
	//

	int iCost = 0; // accumulator for the final return value
	int iScoreLoss = 0;
	for (int i = 0; i < iCitLoss; i++)
	{
		// since the score estimate is very rough, I'm going to flatten it out a bit by combining it with the average score
		job_scores[i] = (2*job_scores[i] + iAverageScore + 2)/3;
		iScoreLoss += job_scores[i];
	}

	for (int i = iCitLoss; i > 0; i--)
	{
		int iFoodLoss = kOwner.getGrowthThreshold(getPopulation() - i) * (105 - getMaxFoodKeptPercent()) / 100;
		int iFoodRate = iTotalFood - (iScoreLoss * AI_yieldMultiplier(YIELD_FOOD) * iYields[YIELD_FOOD])/std::max(1, iTotalScore * 100);
		iFoodRate -= (getPopulation() - i) * GC.getFOOD_CONSUMPTION_PER_POPULATION();
		iFoodRate += std::min(iWastedFood, i);
		iFoodRate += iHappyLevel <= 0 ? i : 0; // if we're at the happiness cap, assume we've got some spare food that we're not currently working.

		int iRecoveryTurns = iFoodRate > 0 ? (iFoodLoss+iFoodRate-1) / iFoodRate : iFoodLoss;

		int iCostPerTurn = iBaseCostPerTurn;
		iCostPerTurn *= iScoreLoss;
		iCostPerTurn /= std::max(1, iTotalScore * 100);

		// extra food from reducing the population:
		iCostPerTurn -= 4 * (std::min(iWastedFood, i) + i*GC.getFOOD_CONSUMPTION_PER_POPULATION()) * kOwner.AI_yieldWeight(YIELD_FOOD)/100;
		iCostPerTurn += 2*i; // just a little bit of extra cost, to show that we care...

		FAssert(iRecoveryTurns > 0); // iCostPerTurn <= 0 is possible, but it should be rare

		// recovery isn't complete if the citizen is still angry
		iAngerTimer -= iRecoveryTurns;
		if (iAngerTimer > 0 && iHappyLevel + i - iNewAnger < 0)
		{
			iCost += iAngerTimer * GC.getFOOD_CONSUMPTION_PER_POPULATION() * 4 * kOwner.AI_yieldWeight(YIELD_FOOD) / 100;
			iRecoveryTurns += iAngerTimer;
		}

		iCost += iRecoveryTurns * iCostPerTurn;

		iScoreLoss -= job_scores[i-1];
	}
	// if there is any anger left over after regrowing, we might as well count some cost for that too.
	if (iAngerTimer > 0 && iHappyLevel - 1 - iNewAnger < 0)
	{
		int iGrowthTime = std::max(1, iTotalFood - getPopulation()*GC.getFOOD_CONSUMPTION_PER_POPULATION()); // just the food rate here
		iGrowthTime = (kOwner.getGrowthThreshold(getPopulation()) - getFood() + iGrowthTime - 1)/iGrowthTime; // now it's the time.

		iAngerTimer -= iGrowthTime;
		if (iAngerTimer > 0)
		{
			int iCostPerTurn = iBaseCostPerTurn;

			iCostPerTurn *= iAverageScore * (iNewAnger + 1 - iHappyLevel);
			iCostPerTurn /= std::max(1, iTotalScore * 100);

			iCostPerTurn += (iNewAnger + 1 - iHappyLevel) * GC.getFOOD_CONSUMPTION_PER_POPULATION() * 4 * kOwner.AI_yieldWeight(YIELD_FOOD) / 100;

			FAssert(iCostPerTurn > 0 && iAngerTimer > 0);

			iCost += iCostPerTurn * iAngerTimer;
		}
	}
	if (kOwner.AI_getFlavorValue(FLAVOR_GROWTH) > 0)
	{
		iCost *= 120 + kOwner.AI_getFlavorValue(FLAVOR_GROWTH);
		iCost /= 100;
	}
	
	// Civ4 Reimagined: Cost is too low
	iCost *= 2;

	return iCost;
}
// K-Mod end

bool CvCityAI::AI_potentialPlot(short* piYields) const
{
	int iNetFood = piYields[YIELD_FOOD] - GC.getFOOD_CONSUMPTION_PER_POPULATION();

	if (iNetFood < 0)
	{
 		if (piYields[YIELD_FOOD] == 0)
		{
			if (piYields[YIELD_PRODUCTION] + piYields[YIELD_COMMERCE] < 2)
			{
				return false;
			}
		}
		else
		{
			if (piYields[YIELD_PRODUCTION] + piYields[YIELD_COMMERCE] == 0)
			{
				return false;
			}
		}
	}

	return true;
}


bool CvCityAI::AI_foodAvailable(int iExtra) const
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;
	bool abPlotAvailable[NUM_CITY_PLOTS];
	int iFoodCount;
	int iPopulation;
	int iBestPlot;
	int iValue;
	int iBestValue;
	int iI;

	iFoodCount = 0;

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		abPlotAvailable[iI] = false;
	}

	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = getCityIndexPlot(iI);

		if (pLoopPlot != NULL)
		{
			if (iI == CITY_HOME_PLOT)
			{
				iFoodCount += pLoopPlot->getYield(YIELD_FOOD);
			}
			else if ((pLoopPlot->getWorkingCity() == this) && AI_potentialPlot(pLoopPlot->getYield()))
			{
				abPlotAvailable[iI] = true;
			}
		}
	}

	iPopulation = (getPopulation() + iExtra);

	while (iPopulation > 0)
	{
		iBestValue = 0;
		iBestPlot = CITY_HOME_PLOT;

		for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			if (abPlotAvailable[iI])
			{
				iValue = getCityIndexPlot(iI)->getYield(YIELD_FOOD);

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					iBestPlot = iI;
				}
			}
		}

		if (iBestPlot != CITY_HOME_PLOT)
		{
			iFoodCount += iBestValue;
			abPlotAvailable[iBestPlot] = false;
		}
		else
		{
			break;
		}

		iPopulation--;
	}

	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		iFoodCount += (GC.getSpecialistInfo((SpecialistTypes)iI).getYieldChange(YIELD_FOOD) * getFreeSpecialistCount((SpecialistTypes)iI));
	}

	if (iFoodCount < foodConsumption(false, iExtra))
	{
		return false;
	}

	return true;
}


// K-Mod: I've rewritten large chunks of this function. I've deleted much of the original code, rather than commenting it; just to keep things a bit tidier and clearer.
// Note. I've changed the scale to match the building evaluation code: ~4x commerce.
// Used for AI_PlotValue, AI_SpecialistValue and others...
int CvCityAI::AI_yieldValue(short* piYields, short* piCommerceYields, bool bRemove, bool bIgnoreFood, bool bIgnoreStarvation, bool bWorkerOptimization, int iGrowthValue) const
{
	PROFILE_FUNC();
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	//const int iBaseProductionValue = 9; // (was 7 before I removed the averageCommerceExchange factor from the commerce values.)
	const int iBaseProductionValue = 7; // Civ4 Reimagined
	//const int iBaseCommerceValue = 4;
	const int iBaseCommerceValue = 5; // Civ4 Reimagined

	bool bEmphasizeFood = AI_isEmphasizeYield(YIELD_FOOD);
	bool bFoodIsProduction = isFoodProduction();
	//bool bCanPopRush = GET_PLAYER(getOwnerINLINE()).canPopRush();

	// a kludge to handle the NULL yields easily.
	short aiZeroYields[NUM_YIELD_TYPES] = {};
	if (piYields == NULL)
		piYields = aiZeroYields;

	int iBaseProductionModifier = getBaseYieldRateModifier(YIELD_PRODUCTION);
	int iExtraProductionModifier = getProductionModifier();
	int iProductionTimes100 = piYields[YIELD_PRODUCTION] * (iBaseProductionModifier + iExtraProductionModifier);
	int iCommerceYieldTimes100 = piYields[YIELD_COMMERCE] * getBaseYieldRateModifier(YIELD_COMMERCE);
	int iFoodYieldTimes100 = piYields[YIELD_FOOD] * getBaseYieldRateModifier(YIELD_FOOD);
	int iFoodYield = iFoodYieldTimes100/100;

	int iValue = 0; // total value

	// Commerce
	int iCommerceValue = 0;
	ProcessTypes eProcess = getProductionProcess();

	for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		int iCommerceTimes100 = iCommerceYieldTimes100 * kOwner.getCommercePercent((CommerceTypes)iI) / 100;
		if (piCommerceYields != NULL)
		{
			iCommerceTimes100 += piCommerceYields[iI] * 100;
		}

		iCommerceTimes100 *= getTotalCommerceRateModifier((CommerceTypes)iI);
		iCommerceTimes100 /= 100;

		//FAssert(iCommerceTimes100 >= 0);

		if (eProcess != NO_PROCESS)
			iCommerceTimes100 += GC.getProcessInfo(getProductionProcess()).getProductionToCommerceModifier(iI) * iProductionTimes100 / 100;

		if (iCommerceTimes100 != 0)
		{
			int iCommerceWeight = kOwner.AI_commerceWeight((CommerceTypes)iI, this); // (Should we still use this with bWorkerOptimization?)
			if (AI_isEmphasizeCommerce((CommerceTypes)iI))
			{
				iCommerceWeight *= 2;
			}
			// Civ4 Reimagined: Prefer research when going for religion tech at beginning of game
			if (iI == COMMERCE_RESEARCH && GC.getGameINLINE().getElapsedGameTurns() < 20)
			{
				for (int iJ = 0; iJ < GC.getNumReligionInfos(); iJ++)
				{
					TechTypes eReligionTech = (TechTypes)GC.getReligionInfo((ReligionTypes)iJ).getTechPrereq();
					if (eReligionTech == GET_PLAYER(getOwnerINLINE()).getCurrentResearch())
					{
						iCommerceWeight *= 2;
						break;
					}
				}
			}
			
			if (!bWorkerOptimization && iI == COMMERCE_CULTURE && getCultureLevel() <= (CultureLevelTypes)1)
			{
				// bring on the artists
				if (getCommerceRateTimes100(COMMERCE_CULTURE) - (bRemove ? iCommerceTimes100 : 0) < 100)
				{
					iCommerceValue += 20 * (iCommerceTimes100 > 0 ? 1 : -1);
				}
				iCommerceWeight = std::max(iCommerceWeight, 200);
			}

			//iCommerceValue += iCommerceWeight * iCommerceTimes100 * iBaseCommerceValue * GET_PLAYER(getOwnerINLINE()).AI_averageCommerceExchange((CommerceTypes)iI) / 1000000;
			iCommerceValue += iCommerceWeight * iCommerceTimes100 * iBaseCommerceValue / 10000; // K-Mod. (averageCommerceExchange should be part of commerceWeight if we want it, and we probably don't want it anyway.)
		}
	}
	//

	// Production
	int iProductionValue = 0;
	if (eProcess == NO_PROCESS)
	{
		iProductionTimes100 += bFoodIsProduction ? iFoodYieldTimes100 : 0;

		iProductionValue += iProductionTimes100 * iBaseProductionValue / 100;
		// If city has more than enough food, but very little production, add large value to production
		// Particularly helps coastal cities with plains forests
		// (based on BBAI code)
		if (!bWorkerOptimization && iProductionTimes100 > 0 && !bFoodIsProduction && isProduction())
		{
			if (!isHuman() || AI_isEmphasizeYield(YIELD_PRODUCTION)
				|| (!bEmphasizeFood && !AI_isEmphasizeYield(YIELD_COMMERCE) && !AI_isEmphasizeGreatPeople()))
			{
				// Civ4 Reimagined Todo: Bonus when building wonder
				BuildingTypes eProductionBuilding = getProductionBuilding();
				if (eProductionBuilding != NO_BUILDING)
				{
					if (isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eProductionBuilding).getBuildingClassType())))
					{
						iProductionValue *= 2;
					}
				}
				
				// don't worry about minimum production if there is any hurry type available for us. If we need the productivity, we can just buy it.
				bool bAnyHurry = false;
				for (HurryTypes i = (HurryTypes)0; !bAnyHurry && i < GC.getNumHurryInfos(); i=(HurryTypes)(i+1))
					bAnyHurry = kOwner.canHurry(i);

				if (!bAnyHurry && foodDifference(false)-(bRemove ? iFoodYield : 0) >= GC.getFOOD_CONSUMPTION_PER_POPULATION())
				{
					/*if (getYieldRate(YIELD_PRODUCTION) - (bRemove ? iProductionTimes100/100 : 0)  < 1 + getPopulation()/3)
					{
						iValue += 60 + iBaseProductionValue * iProductionTimes100 / 100;
					}*/
					int iCurrentProduction = getCurrentProductionDifference(true, false) - (bRemove ? iProductionTimes100/100 : 0);
					if (iCurrentProduction < 1 + getPopulation()/3)
					{
						iValue += 5 * iBaseProductionValue * std::min(iProductionTimes100, (1 + getPopulation()/3 - iCurrentProduction)*100) / 100;
					}
				}
			}
		}
		if (!isProduction() && !isHuman())
		{
			iProductionValue /= 2;
		}
	}
	//

	int iSlaveryValue = 0;

	int iFoodGrowthValue = 0;

	if (!bIgnoreFood && iFoodYield != 0)
	{
		if (iGrowthValue < 0)
			iGrowthValue = AI_growthValuePerFood();

		// tiny food factor, to ensure that even when we don't want to grow, 
		// we still prefer more food if everything else is equal
		iValue += bFoodIsProduction ? 0 : (iFoodYieldTimes100+50)/100;

		int iConsumtionPerPop = GC.getFOOD_CONSUMPTION_PER_POPULATION();
		int iFoodPerTurn = getYieldRate(YIELD_FOOD) - foodConsumption() - (bRemove? iFoodYield : 0);
		int iFoodLevel = getFood();
		int iFoodToGrow = growthThreshold();
		int iHealthLevel = goodHealth() - badHealth();
		int iHappinessLevel = (isNoUnhappiness() ? std::max(3, iHealthLevel + 5) : happyLevel() - unhappyLevel(0));
		int iPopulation = getPopulation();

		const int iAdjustedFoodPerTurn = iFoodPerTurn; // (currently unused)
		// (I'd like to lower this when evaluating plots we might switch to - but currently there is no way to know

		// if we not human, allow us to starve to half full if avoiding growth
		if (!bIgnoreStarvation)
		{
			int iStarvingAllowance = 0;
			if (AI_isEmphasizeAvoidGrowth() || iHappinessLevel < (isHuman() ? 0 : 1))
			{
				iStarvingAllowance = std::max(0, (iFoodLevel - std::max(1, ((7 * iFoodToGrow) / 10))));
				iStarvingAllowance /= 1
					+ (iHappinessLevel+getEspionageHappinessCounter()/2 >= 0 ? 1 : 0)
					+ (iHealthLevel+getEspionageHealthCounter() > 0 ? 1 : 0);
			}

			// if still starving
			if (iFoodPerTurn + iStarvingAllowance < 0)
			{
				// if working plots all like this one will save us from starving
				//if (((bReassign?1:0)+iExtraPopulationThatCanWork+std::max(0, getSpecialistPopulation() - totalFreeSpecialists())) * iFoodYield >= -iFoodPerTurn)
				/*if ((iExtraPopulationThatCanWork+std::max(bReassign?1:0, getSpecialistPopulation() - totalFreeSpecialists())) * iFoodYield >= -iFoodPerTurn + (bReassign ? std::min(iFoodYield, iConsumtionPerPop) : 0))
				{
					iValue += 2048;
				}*/

				// value food high, but not forced
				//iValue += 36 * std::min(iFoodYield, -iFoodPerTurn+(bReassign ? iConsumtionPerPop : 0));
				iValue += (iHappinessLevel >= 0 ? 2: 1)*std::max(iGrowthValue, iBaseProductionValue*3) * std::min(iFoodYield, -(iFoodPerTurn + iStarvingAllowance));
				// note. iGrowthValue only counts unworked plots - so it isn't entirely suitable for this. Hence the arbitrary minimum value.
			}
		}

		// if food isn't production, then adjust for growth
		if (bWorkerOptimization || !bFoodIsProduction)
		{
			int iPopToGrow = 0;
			if (!AI_isEmphasizeAvoidGrowth())
			{
				int iFutureHappy = 0; // K-Mod. Happiness boost we expect before we grow. (originally, the boost was added directly to iHappyLevel)

				// only do relative checks on food if we want to grow AND we not emph food
				// the emp food case will just give a big boost to all food under all circumstances
				//if (bWorkerOptimization || (!bIgnoreGrowth && !bEmphasizeFood))
				if (iGrowthValue > 0)
				{
					// we have less than 10 extra happy, do some checks to see if we can increase it
					//if (iHappinessLevel < 10)
					if (iHappinessLevel < 5) // K-Mod. make it 5. We shouldn't really be worried about growing 10 pop all at once...
					{
						// if we have anger becase no military, do not count it, on the assumption that it will 
						// be remedied soon, and that we still want to grow
						if (getMilitaryHappinessUnits() == 0)
						{
							if (kOwner.getNumCities() > 2)
							{
								iHappinessLevel += ((GC.getDefineINT("NO_MILITARY_PERCENT_ANGER") * (iPopulation + 1)) / GC.getPERCENT_ANGER_DIVISOR());
							}
						}

						const int kMaxHappyIncrease = 2;

						// if happy is large enough so that it will be over zero after we do the checks
						if (iHappinessLevel + kMaxHappyIncrease > 0)
						{
							int iNewFoodPerTurn = iFoodPerTurn + iFoodYield; // not including reassignment penalty, because it's better to overestimate food here.
							int iApproxTurnsToGrow = (iNewFoodPerTurn > 0) ? ((iFoodToGrow - iFoodLevel + iNewFoodPerTurn-1) / iNewFoodPerTurn) : MAX_INT;

							// do we have hurry anger?
							int iHurryAngerTimer = getHurryAngerTimer();
							if (iHurryAngerTimer > 0)
							{
								int iTurnsUntilAngerIsReduced = iHurryAngerTimer % flatHurryAngerLength();
								
								// angry population is bad but if we'll recover by the time we grow...
								if (iTurnsUntilAngerIsReduced <= iApproxTurnsToGrow)
								{
									iFutureHappy++;
								}
							}

							// do we have conscript anger?
							int iConscriptAngerTimer = getConscriptAngerTimer();
							if (iConscriptAngerTimer > 0)
							{
								int iTurnsUntilAngerIsReduced = iConscriptAngerTimer % flatConscriptAngerLength();
								
								// angry population is bad but if we'll recover by the time we grow...
								if (iTurnsUntilAngerIsReduced <= iApproxTurnsToGrow)
								{
									iFutureHappy++;
								}
							}

							// do we have defy resolution anger?
							int iDefyResolutionAngerTimer = getDefyResolutionAngerTimer();
							if (iDefyResolutionAngerTimer > 0)
							{
								int iTurnsUntilAngerIsReduced = iDefyResolutionAngerTimer % flatDefyResolutionAngerLength();

								// angry population is bad but if we'll recover by the time we grow...
								if (iTurnsUntilAngerIsReduced <= iApproxTurnsToGrow)
								{
									iFutureHappy++;
								}
							}
						}
					}

					if (bEmphasizeFood)
					{
						//If we are emphasize food, pay less heed to caps.
						iHealthLevel += 5;
						iFutureHappy += 2;
					}

					bool bBarFull = iFoodLevel + iAdjustedFoodPerTurn > iFoodToGrow * 80 / 100;

					int iPopToGrow = std::max(0, iHappinessLevel+iFutureHappy);
					int iGoodTiles = AI_countGoodTiles(iHealthLevel > 0, true, 50, true);
					iGoodTiles += AI_countGoodSpecialists(iHealthLevel > 0);

					if (!bEmphasizeFood)
					{
						iPopToGrow = std::min(iPopToGrow, iGoodTiles + (bWorkerOptimization ? 1 : 0));
						if (AI_isEmphasizeYield(YIELD_PRODUCTION) || AI_isEmphasizeGreatPeople())
							iPopToGrow = std::min(iPopToGrow, 2);
						else if (AI_isEmphasizeYield(YIELD_COMMERCE))
							iPopToGrow = std::min(iPopToGrow, 3);
					}

					// if we have growth pontential, then get the food bar close to full
					bool bFillingBar = false;
					if (iPopToGrow == 0 && iHappinessLevel >= 0 && iGoodTiles >= 0 && iHealthLevel >= 0)
					{
						if (!bBarFull)
						{
							//if (AI_specialYieldMultiplier(YIELD_PRODUCTION) < 50)
							{
								bFillingBar = true;
							}
						}
					}

					// We don't want to count growth value for tiles with nothing else to contribute.
					// But since AI_yieldValue is used for AI_jobChangeValue, we can't assume that we're evaluating a plot.
					const bool bRelativeComparison = true; // (placeholder, just in case we restore this kind of check in the future)
					if ((iPopToGrow > 0 || bFillingBar) && (bRelativeComparison || iFoodYield - iConsumtionPerPop > 0 || iProductionValue > 0 || iCommerceValue > 0))
					{
						if (bFillingBar)
							iGrowthValue = iGrowthValue * iFoodToGrow / std::max(1, 2*iFoodToGrow + iFoodLevel + iAdjustedFoodPerTurn);

						if (iHealthLevel < (bFillingBar ? 0 : 1))
							iGrowthValue = iGrowthValue * 2/3; 

						//iFoodGrowthValue = iFoodYield * iFactorPopToGrow;
						// K-Mod. iGrowthValue is the initial value per piece of food, but the value decreases by iDevalueRate for each piece.
						// Think of the integral of iGrowthValue * (100 - iDevalueRate*(iAdjustedFoodPerTurn+x))/100, with respect to x.
						int iDevalueRate = 0;
						if (!bEmphasizeFood)
						{
							if (bFillingBar)
								iDevalueRate = 25 + 15 * (iFoodLevel + iAdjustedFoodPerTurn) / iFoodToGrow;
							else
								iDevalueRate = 20 - std::min(5, iPopToGrow)*3;

							if (iHealthLevel < 1)
								iDevalueRate += 5;
						}
						int iBestFoodYield = std::max(0, std::min(iFoodYield, 100/std::max(1, iDevalueRate) - iAdjustedFoodPerTurn)); // maximum value for this amount of food.
						iFoodGrowthValue = iBestFoodYield * iGrowthValue * (100 - iDevalueRate*iAdjustedFoodPerTurn) / 100;
						iFoodGrowthValue -= iBestFoodYield * iBestFoodYield * iDevalueRate * iGrowthValue / 200;
						FAssert(iFoodGrowthValue >= 0);
						// K-Mod end
					}
				}

				//Slavery evaluation
				// K-Mod. Rescaled values and conditions.
				if (!bWorkerOptimization && isProduction() && kOwner.canPopRush() && getHurryAngerTimer() <= std::min(3,getPopulation()/2)+2*iHappinessLevel) // K-Mod
				{
					//iSlaveryValue = 30 * 14 * std::max(0, aiYields[YIELD_FOOD] - ((iHealthLevel < 0) ? 1 : 0));
					int iProductionPerPop = 0;
					for (HurryTypes eHurry = (HurryTypes)0; eHurry < GC.getNumHurryInfos(); eHurry = (HurryTypes)(eHurry+1))
					{
						if (kOwner.canHurry(eHurry))
						{
							//iProductionPerPop = std::max(iProductionPerPop, GC.getGameINLINE().getProductionPerPopulation(eHurry) * iBaseProductionModifier / 100);
							iProductionPerPop = std::max(iProductionPerPop, GC.getGameINLINE().getProductionPerPopulation(eHurry)); // Don't use modifiers weights, because slavery usage is erratic.
						}
					}

					// Note: '80' means that we're only counting 90% of the potential production, because slavery require careful micromangement to get 100% of the value.
					// Ideally, we'd try to take our infrastructure needs into account. (ie. how much do we still need to build.)
					iSlaveryValue = 80 * iProductionPerPop * iBaseProductionValue * std::max(0, iFoodYield - ((iHealthLevel < 0) ? 1 : 0));
					iSlaveryValue /= std::max(10*100, (growthThreshold() * (100 - getMaxFoodKeptPercent())));

					iSlaveryValue *= 100;
					iSlaveryValue /= getHurryCostModifier(true);

					iSlaveryValue *= iConsumtionPerPop * 2;
					//iSlaveryValue /= iConsumtionPerPop * 2 + std::max(0, iAdjustedFoodDifference);
					iSlaveryValue /= iConsumtionPerPop * 2 + std::max(0, iAdjustedFoodPerTurn - iConsumtionPerPop * iHappinessLevel);
				}
			}
		}
	}

	// Note: iSlaveryValue use to be counted in the production section. (There are argument for and against - but this is easier.)
	int iFoodValue = iFoodGrowthValue;

	if (iSlaveryValue > iFoodGrowthValue)
		iFoodValue = (iSlaveryValue + iFoodGrowthValue)/2; // Use the average - to account for the fact that we won't always be making use of slavery.

	// Civ4 Reimagined
	iFoodValue *= 125;
	iFoodValue /= 100;

	//Lets have some fun with the multipliers, this basically bluntens the impact of
	//massive bonuses.....
	
	//normalize the production... this allows the system to account for rounding
	//and such while preventing an "out to lunch smoking weed" scenario with
	//unusually high transient production modifiers.
	//Other yields don't have transient bonuses in quite the same way.

	// Rounding can be a problem, particularly for small commerce amounts.  Added safe guards to make
	// sure commerce is counted, even if just a tiny amount.
	if (AI_isEmphasizeYield(YIELD_PRODUCTION))
	{
		iProductionValue *= 140; // was 130
		iProductionValue /= 100;
		
		if (isFoodProduction())
		{
			iFoodValue *= 140; // was 130
			iFoodValue /= 100;
		}
	}
	else if (iProductionValue > 0 && AI_isEmphasizeYield(YIELD_COMMERCE))
	{
		iProductionValue *= 75;
		iProductionValue /= 100;
		iProductionValue = std::max(1,iProductionValue);
	}

	if (AI_isEmphasizeYield(YIELD_FOOD))
	{
		if (!isFoodProduction())
		{
			iFoodValue *= 130;
			iFoodValue /= 100;
		}
	}
	/* else if (iFoodValue > 0 && (AI_isEmphasizeYield(YIELD_PRODUCTION) || AI_isEmphasizeYield(YIELD_COMMERCE)))
	{
		iFoodValue *= 75; // was 75 for production, and 80 for commerce. (now same for both.)
		iFoodValue /= 100;
		iFoodValue = std::max(1, iFoodValue);
	} */ // K-Mod. Food value is now scaled elsewhere.

	if (AI_isEmphasizeYield(YIELD_COMMERCE))
	{
		iCommerceValue *= 140; // was 130
		iCommerceValue /= 100;
	}
	else if (iCommerceValue > 0 && AI_isEmphasizeYield(YIELD_PRODUCTION))
	{
		iCommerceValue *= 75; // was 60
		iCommerceValue /= 100;
		iCommerceValue = std::max(1, iCommerceValue);
	}
		
	if( iProductionValue > 0 )
	{
		if (isFoodProduction())
		{
			iProductionValue *= 100 + (bWorkerOptimization ? 0 : AI_specialYieldMultiplier(YIELD_PRODUCTION));
			iProductionValue /= 100;
		}
		else
		{
			iProductionValue *= iBaseProductionModifier;
			iProductionValue /= (iBaseProductionModifier + iExtraProductionModifier);

			// Note: iSlaveryValue use to be added here. Now it is counted as food value instead.
			iProductionValue *= (100 + (bWorkerOptimization ? 0 : AI_specialYieldMultiplier(YIELD_PRODUCTION)));

			iProductionValue /= kOwner.AI_averageYieldMultiplier(YIELD_PRODUCTION);
		}

		iValue += std::max(1,iProductionValue);
	}

	if( iCommerceValue > 0 )
	{
		iCommerceValue *= (100 + (bWorkerOptimization ? 0 : AI_specialYieldMultiplier(YIELD_COMMERCE)));
		iCommerceValue /= kOwner.AI_averageYieldMultiplier(YIELD_COMMERCE);
		iValue += std::max(1, iCommerceValue);
	}

	if( iFoodValue > 0 )
	{
		iFoodValue *= 100;
		iFoodValue /= kOwner.AI_averageYieldMultiplier(YIELD_FOOD);
		iValue += std::max(1, iFoodValue);
	}

	return iValue;
}

// units of 400x commerce
int CvCityAI::AI_plotValue(CvPlot* pPlot, bool bRemove, bool bIgnoreFood, bool bIgnoreStarvation, int iGrowthValue) const
{
	// K-Mod. To reduce code duplication, this function now uses AI_jobChangeValue. (original code deleted)
	FAssert(pPlot);
	FAssert(getCityPlotIndex(pPlot) < NUM_CITY_PLOTS);
	if (bRemove)
	{
		return -AI_jobChangeValue(std::make_pair(false, -1), std::make_pair(false, getCityPlotIndex(pPlot)), bIgnoreFood, bIgnoreStarvation, iGrowthValue);
	}
	else
	{
		return AI_jobChangeValue(std::make_pair(false, getCityPlotIndex(pPlot)), std::make_pair(false, -1), bIgnoreFood, bIgnoreStarvation, iGrowthValue);
	}
}

// K-Mod. value gained by working new_job, and stopping work of old_job.
// jobs are (bIsSpecialist, iIndex) pairs. iIndex < 0 indicates "no job".
// Return value is roughly 400x commerce per turn.
// This function replaces AI_plotValue and AI_specialistValue.
int CvCityAI::AI_jobChangeValue(std::pair<bool, int> new_job, std::pair<bool, int> old_job, bool bIgnoreFood, bool bIgnoreStarvation, int iGrowthValue) const
{
	PROFILE_FUNC();

	FAssert(new_job.second < 0 || (new_job.first ? new_job.second < GC.getNumSpecialistInfos() : getCityIndexPlot(new_job.second)));
	FAssert(old_job.second < 0 || (old_job.first ? old_job.second < GC.getNumSpecialistInfos() : getCityIndexPlot(old_job.second)));

	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());

	int iTotalValue = 0;

	// Calculate and evaluate direct changes in yields
	short aiYieldGained[NUM_YIELD_TYPES] = {};
	short aiYieldLost[NUM_YIELD_TYPES] = {};

	for (YieldTypes i = (YieldTypes)0; i < NUM_YIELD_TYPES; i=(YieldTypes)(i+1))
	{
		int iYield = 0;
		if (new_job.second >= 0)
		{
			// Civ4 Reimagined: Added specialistThresholdExtraYield
			iYield += new_job.first
				? kOwner.specialistYield((SpecialistTypes)new_job.second, i) + kOwner.getSpecialistThresholdExtraYield((SpecialistTypes)new_job.second, i)
				: getCityIndexPlot(new_job.second)->getYield(i);
		}

		if (old_job.second >= 0)
		{
			// Civ4 Reimagined: Added specialistThresholdExtraYield
			iYield -= old_job.first
				? kOwner.specialistYield((SpecialistTypes)old_job.second, i) + kOwner.getSpecialistThresholdExtraYield((SpecialistTypes)old_job.second, i)
				: getCityIndexPlot(old_job.second)->getYield(i);
		}

		if (iYield >= 0)
			aiYieldGained[i] = iYield;
		else
			aiYieldLost[i] = -iYield;
	}
	int iYieldValue = 0;
	iYieldValue += 100 * AI_yieldValue(aiYieldGained, NULL, false, bIgnoreFood, bIgnoreStarvation, false, iGrowthValue);
	iYieldValue -= 100 * AI_yieldValue(aiYieldLost, NULL, true, bIgnoreFood, bIgnoreStarvation, false, iGrowthValue);

	// Check whether either the old job or the new job involves an upgradable improvement. (eg. a cottage)
	// Note, AI_finalImprovementYieldDifference will update the yield vectors.
	bool bUpgradeYields = false;
	if (new_job.second >= 0 && !new_job.first && AI_finalImprovementYieldDifference(getCityIndexPlot(new_job.second), aiYieldGained))
		bUpgradeYields = true;

	if (old_job.second >= 0 && !old_job.first && AI_finalImprovementYieldDifference(getCityIndexPlot(old_job.second), aiYieldLost))
		bUpgradeYields = true;

	if (bUpgradeYields)
	{
		for (int i = 0; i < NUM_YIELD_TYPES; i++)
		{
			int iYield = aiYieldGained[i] - aiYieldLost[i];
			if (iYield >= 0)
			{
				aiYieldGained[i] = iYield;
				aiYieldLost[i] = 0;
			}
			else
			{
				aiYieldGained[i] = 0;
				aiYieldLost[i] = -iYield;
			}
		}
		// Combine the old and new values in the same way as AI_plotValue. (cf. AI_plotValue)
		int iFinalYieldValue = 0;
		iFinalYieldValue += 100 * AI_yieldValue(aiYieldGained, NULL, false, bIgnoreFood, bIgnoreStarvation, false, iGrowthValue);
		iFinalYieldValue -= 100 * AI_yieldValue(aiYieldLost, NULL, true, bIgnoreFood, bIgnoreStarvation, false, iGrowthValue);

		if (iFinalYieldValue > iYieldValue)
			iYieldValue = (40 * iYieldValue + 60 * iFinalYieldValue) / 100;
		else
			iYieldValue = (60 * iYieldValue + 40 * iFinalYieldValue) / 100;
	}

	iTotalValue += iYieldValue;
	// (end of yield value)

	// Special consideration of plot improvements.
	int iImprovementsValue = 0;
	if (new_job.second >= 0 && !new_job.first)
	{
		iImprovementsValue += AI_specialPlotImprovementValue(getCityIndexPlot(new_job.second));
	}
	if (old_job.second >= 0 && !old_job.first)
	{
		iImprovementsValue -= AI_specialPlotImprovementValue(getCityIndexPlot(old_job.second));
	}
	iTotalValue += iImprovementsValue;

	// Specialst extras
	if (new_job.first || old_job.first)
	{
		// Raw commerce value
		// (plots don't give raw commerce)
		short aiCommerceGained[NUM_COMMERCE_TYPES] = {};
		short aiCommerceLost[NUM_COMMERCE_TYPES] = {};

		for (CommerceTypes i = (CommerceTypes)0; i < NUM_COMMERCE_TYPES; i=(CommerceTypes)(i+1))
		{
			int iCommerce = 0;

			if (new_job.second >= 0 && new_job.first)
				iCommerce += kOwner.specialistCommerce((SpecialistTypes)new_job.second, i);

			if (old_job.second >= 0 && old_job.first)
				iCommerce -= kOwner.specialistCommerce((SpecialistTypes)old_job.second, i);

			if (iCommerce >= 0)
				aiCommerceGained[i] = iCommerce;
			else
				aiCommerceLost[i] = -iCommerce;
		}
		iTotalValue += 100 * AI_yieldValue(NULL, aiCommerceGained, false, bIgnoreFood, bIgnoreStarvation, false, iGrowthValue);
		iTotalValue -= 100 * AI_yieldValue(NULL, aiCommerceLost, true, bIgnoreFood, bIgnoreStarvation, false, iGrowthValue);

		int iGPPGained = 0;
		int iGPPLost = 0;
		if (new_job.second >= 0 && new_job.first)
			// Civ4 Reimagined
			//iGPPGained += GC.getSpecialistInfo((SpecialistTypes)new_job.second).getGreatPeopleRateChange();
			iGPPGained += kOwner.isNoGreatPeople() ? 0 : GC.getSpecialistInfo((SpecialistTypes)new_job.second).getGreatPeopleRateChange();
		if (old_job.second >= 0 && old_job.first)
			// Civ4 Reimagined
			//iGPPLost += GC.getSpecialistInfo((SpecialistTypes)old_job.second).getGreatPeopleRateChange();
			iGPPLost += kOwner.isNoGreatPeople() ? 0 : GC.getSpecialistInfo((SpecialistTypes)old_job.second).getGreatPeopleRateChange();

		if (iGPPGained || iGPPLost)
		{
			int iEmphasisCount = AI_isEmphasizeYield(YIELD_COMMERCE) + AI_isEmphasizeYield(YIELD_FOOD) + AI_isEmphasizeYield(YIELD_PRODUCTION);

			int iBaseValue = AI_isEmphasizeGreatPeople()
				? 12
				: (iEmphasisCount > 0)
					? 3
					: 4;
			// note: each point of iEmphasisCount reduces the value at the end.

			int iGPPValue = 0;

			// Value for GPP, dependant on great person type. (Note: currently for humans, great person weights are all 100.)
			if (iGPPGained)
				iGPPValue += iGPPGained * iBaseValue * kOwner.AI_getGreatPersonWeight((UnitClassTypes)GC.getSpecialistInfo((SpecialistTypes)new_job.second).getGreatPeopleUnitClass());
			if (iGPPLost)
				iGPPValue -= iGPPLost * iBaseValue * kOwner.AI_getGreatPersonWeight((UnitClassTypes)GC.getSpecialistInfo((SpecialistTypes)old_job.second).getGreatPeopleUnitClass());

			iGPPValue *= getTotalGreatPeopleRateModifier();
			iGPPValue /= 100;

			// Bonus value for GPP when we're close to getting another great person
			if (!isHuman() || AI_isEmphasizeGreatPeople())
			{
				int iProgress = getGreatPeopleProgress();
				if (iProgress > 0)
				{
					int iThreshold = kOwner.greatPeopleThreshold();
					int iCloseBonus = (iGPPGained - iGPPLost) * (isHuman() ? 1 : 4) * iBaseValue * iProgress / iThreshold;
					iCloseBonus *= iProgress;
					iCloseBonus /= iThreshold;
					iGPPValue += iCloseBonus;
				}
			}

			// Scale based on how often this city will actually get a great person.
			if (!AI_isEmphasizeGreatPeople())
			{
				int iCityRate = getGreatPeopleRate() + iGPPGained - iGPPLost;
				int iHighestRate = 0;
				int iLoop;
				for (CvCity* pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
				{
					iHighestRate = std::max(iHighestRate, pLoopCity->getGreatPeopleRate());
				}
				if (iHighestRate > iCityRate)
				{
					iGPPValue *= 100;
					iGPPValue /= (2*100*(iHighestRate+8))/(iCityRate+8) - 100; // the +8 is just so that we don't block ourselves from assigning the first couple of specialists.
				}
				// each successive great person costs more points. So the points are effectively worth less...
				// (note: I haven't tried to match this value decrease with the actual cost increase,
				//  because the value of the great people changes as well.)
				iGPPValue *= 100;
				iGPPValue /= 90 + 7 * kOwner.getGreatPeopleCreated(); // it would be nice if we had a flavour modifier for this.
			}

			//iTempValue /= kOwner.AI_averageGreatPeopleMultiplier();
			// K-Mod note: ultimately, I don't think the value should be divided by the average multiplier.
			// because more great people points is always better, regardless of what the average multiplier is.
			// However, because of the flawed way that food is currently evaluated, I need to dilute the value of GPP
			// so that specialists don't get value more highly than food tiles. (I hope to correct this later.)
			iGPPValue *= 100;
			iGPPValue /= (300 + kOwner.AI_averageGreatPeopleMultiplier())/4;

			iGPPValue /= (1 + iEmphasisCount);

			// Civ4 Reimagined
			if (kOwner.AI_isDoStrategy(AI_STRATEGY_SPECIALIST_ECONOMY) && getPopulation() > 3)
			{
				if (foodDifference(true,true) >= GC.getFOOD_CONSUMPTION_PER_POPULATION())
				{
					iGPPValue *= 3;
				}
			}

			iTotalValue += iGPPValue;

			// Evaluate military experience from specialists
			int iExperience = 0;
			if (new_job.second >= 0 && new_job.first)
				iExperience += GC.getSpecialistInfo((SpecialistTypes)new_job.second).getExperience();
			if (old_job.second >= 0 && old_job.first)
				iExperience += GC.getSpecialistInfo((SpecialistTypes)old_job.second).getExperience();

			if (iExperience != 0)
			{
				int iProductionRank = findYieldRateRank(YIELD_PRODUCTION);

				int iExperienceValue = 100 * iExperience * 4;
				if (iProductionRank <= kOwner.getNumCities()/2 + 1)
				{
					iExperienceValue += 100 * iExperience *  4;
				}
				iExperienceValue += (getMilitaryProductionModifier() * iExperience * 8);

				iTotalValue += iExperienceValue;
			}
		}

		// Devalue generic citizens (for no specific reason). (cf. AI_specialistValue)
		/* if (no gpp)
		{
			SpecialistTypes eGenericCitizen = (SpecialistTypes) GC.getDefineINT("DEFAULT_SPECIALIST");

			if (eGenericCitizen != NO_SPECIALIST)
			{
				if (new_job.first && new_job.second == eGenericCitizen)
					iTotalValue = iTotalValue * 80 / 100;
				if (old_job.first && old_job.second == eGenericCitizen)
					iTotalValue = iTotalValue * 100 / 80;
			}
		} */
	}

	return iTotalValue;
}

// K-Mod. Difference between current yields and yields after plot improvement reaches final upgrade.
// piYields will have final yields added to it, and current yields subtracted.
// Return true iff any yields are changed.
bool CvCityAI::AI_finalImprovementYieldDifference(CvPlot* pPlot, short* piYields) const
{
	FAssert(pPlot);
	FAssert(piYields);

	const CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	bool bAnyChange = false;

	// Civ4 Reimagined
	if (kOwner.getImprovementUpgradeRate() <= 0)
		return false;

	ImprovementTypes eCurrentImprovement = pPlot->getImprovementType();
	ImprovementTypes eFinalImprovement = finalImprovementUpgrade(eCurrentImprovement);
	if (eFinalImprovement != NO_IMPROVEMENT && eFinalImprovement != eCurrentImprovement)
	{
		for (YieldTypes i = (YieldTypes)0; i < NUM_YIELD_TYPES; i=(YieldTypes)(i+1))
		{
			int iYieldDiff = pPlot->calculateImprovementYieldChange(eFinalImprovement, i, getOwnerINLINE()) - pPlot->calculateImprovementYieldChange(eCurrentImprovement, i, getOwnerINLINE());
			if (kOwner.getExtraYieldThreshold(i) > 0)
				iYieldDiff += (pPlot->getYield(i) >= kOwner.getExtraYieldThreshold(i) ? -GC.getEXTRA_YIELD() : 0) + (pPlot->getYield(i)+iYieldDiff >= kOwner.getExtraYieldThreshold(i) ? GC.getEXTRA_YIELD() : 0);

			if (iYieldDiff != 0)
			{
				bAnyChange = true;
				piYields[i] += iYieldDiff;
			}
		}
	}
	return bAnyChange;
}

// For improvements which upgrade when worked, this function calculates a time-weighted average of their yields.
// The average is calculated with continous weighting such that ~63% of the weight is in the time from now until 'time_scale' turns have past.
// (More precisely, the weight drops exponentially w.r.t. the number of turns, decreasing by a factor of `e` for each `time_scale` turns.)
bool CvCityAI::AI_timeWeightedImprovementYields(CvPlot const* pPlot, ImprovementTypes eImprovement, int time_scale, std::vector<float>& weighted_yields) const
{
	PROFILE_FUNC();

	FAssert(pPlot);
	FAssert(time_scale > 0);

	// This is an experimental function, designed to be 'the right way' to evaluate improvements which upgrade over time, with no concern for calculation speed.
	// If it isn't too slow, I'll use it for lots of things.
	// weighted yield = integral from 0 to inf. of (yield(t) * e^(-t/scale)), w.r.t.  t

	weighted_yields.assign(NUM_YIELD_TYPES, 0);
	std::set<ImprovementTypes> cited_improvements;
	int iTotalTurns = 0;

	while (eImprovement != NO_IMPROVEMENT && cited_improvements.insert(eImprovement).second)
	{
		const CvImprovementInfo& kImprovement = GC.getImprovementInfo(eImprovement);

		// piecewise integration
		// each piece is exp(-t0/T) - exp(t1/T)
		const float value_factor = exp(-(float)iTotalTurns/time_scale) - (kImprovement.getImprovementUpgrade() != NO_IMPROVEMENT ? exp(-(float)(iTotalTurns+kImprovement.getUpgradeTime())/time_scale) : 0);

		for (YieldTypes i = (YieldTypes)0; i < NUM_YIELD_TYPES; i=(YieldTypes)(i+1))
		{
			int iYieldDiff = pPlot->calculateImprovementYieldChange(eImprovement, i, getOwnerINLINE());
			//if (kOwner.getExtraYieldThreshold(i) > 0) // todo: Extra yield for finacial civs (see AI_finalImprovementYieldDifference for guidence.)
				// ;

			weighted_yields[i] += iYieldDiff * value_factor;
		}

		iTotalTurns += kImprovement.getUpgradeTime();
		eImprovement = (ImprovementTypes)kImprovement.getImprovementUpgrade();
	}

	if (eImprovement != NO_IMPROVEMENT)
	{
		// If we didn't reach final improvemnet; normalise the yield values based on what we've got so far.
		for (size_t i = 0; i < NUM_YIELD_TYPES; ++i)
		{
			weighted_yields[i] /= (1-exp(-(float)iTotalTurns/time_scale));
		}
	}

	return cited_improvements.size() > 1; // return true if the improvement had any upgrades
}

// K-Mod. Value for working a plot in addition to its yields.
// (Returns ~400x commerce per turn.)
int CvCityAI::AI_specialPlotImprovementValue(CvPlot* pPlot) const
{
	FAssert(pPlot);
	int iValue = 0;

	ImprovementTypes eImprovement = pPlot->getImprovementType();

	if (eImprovement != NO_IMPROVEMENT)
	{
		if (GC.getImprovementInfo(eImprovement).getImprovementUpgrade() != NO_IMPROVEMENT)
		{
			// Prefer plots that are close to upgrading, but not over immediate yield differences. (kludge)
			//iValue += 100 * pPlot->getUpgradeProgress() / std::max(1, GC.getGameINLINE().getImprovementUpgradeTime(eImprovement));
			// Civ 4 Reimagined: getUpgradeProcess now scales with turns times 100
			iValue += pPlot->getUpgradeProgress() / std::max(1, GC.getGameINLINE().getImprovementUpgradeTime(eImprovement));
		}

		// small value bonus for the possibility of popping new resources. (cf. CvGame::doFeature)
		if (pPlot->getBonusType(getTeam()) == NO_BONUS)
		{
			for (BonusTypes i = (BonusTypes)0; i < GC.getNumBonusInfos(); i=(BonusTypes)(i+1))
			{
				if (GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBonusInfo(i).getTechReveal())))
				{
					if (GC.getImprovementInfo(eImprovement).getImprovementBonusDiscoverRand(i) > 0)
					{
						iValue += 20;
					}
				}
			}
		}
	}
	return iValue;
}

// K-Mod. Value of each unit of food for the purpose of population growth.
// Units ~4x commerce. Note: the value here is at the high end.. it should be reduced based on things such as the happiness cap.
int CvCityAI::AI_growthValuePerFood() const
{
	int iFoodMultiplier = getBaseYieldRateModifier(YIELD_FOOD);
	int iConsumtionPerPop = std::max(1, GC.getFOOD_CONSUMPTION_PER_POPULATION());

	std::vector<int> jobs;
	
	//Civ4 Reimagined
	const CvPlayer& kOwner = GET_PLAYER(getOwnerINLINE());
	bool bSlavery = kOwner.canPopRush();
	
	for (int i = 1; i < NUM_CITY_PLOTS; i++)
	{
		if (isWorkingPlot(i))
			continue;

		CvPlot* pLoopPlot = getCityIndexPlot(i);
		if (pLoopPlot == 0)
			continue;

		int iValue = AI_plotValue(pLoopPlot, false, true, true, -1);
		
		// Civ4 Reimagined: Greater food value with slavery
		if (pLoopPlot->getYield(YIELD_FOOD) > 2)
		{
			iValue *= 100 * iConsumtionPerPop + (iFoodMultiplier + (bSlavery ? 30 : 10)) * pLoopPlot->getYield(YIELD_FOOD);
		} else
		{
			iValue *= 100 * iConsumtionPerPop + iFoodMultiplier * pLoopPlot->getYield(YIELD_FOOD);
		}
		iValue /= 100 * iConsumtionPerPop;

		jobs.push_back(iValue);
	}

	for (SpecialistTypes i = (SpecialistTypes)0; i < GC.getNumSpecialistInfos(); i = (SpecialistTypes)(i+1))
	{
		int iAvailable = std::min(3, getMaxSpecialistCount(i) - getSpecialistCount(i));

		if (iAvailable > 0)
		{
			FAssert(isSpecialistValid(i, iAvailable)); // Civ4 Reimagined Todo: Check if this makes sense
			int iValue = AI_specialistValue(i, false, true, -1);
			iValue *= 100 * iConsumtionPerPop + iFoodMultiplier * kOwner.specialistYield(i, YIELD_FOOD);
			iValue /= 100 * iConsumtionPerPop;

			while (--iAvailable >= 0)
				jobs.push_back(iValue);
		}
	}

	// ok. we've surveyed the available jobs. Now, the growth value..
	while (jobs.size() < 3)
		jobs.push_back(0);
	std::partial_sort(jobs.begin(), jobs.begin() + 3, jobs.end(), std::greater<int>());

	return (jobs[0]*4 + jobs[1]*2 + jobs[2]*1) / (700 * iConsumtionPerPop);
}
// K-Mod end


int CvCityAI::AI_experienceWeight()
{
	//return ((getProductionExperience() + getDomainFreeExperience(DOMAIN_SEA)) * 2);
	// K-Mod
	return 2 * getProductionExperience() + getDomainFreeExperience(DOMAIN_LAND) + getDomainFreeExperience(DOMAIN_SEA);
	// K-Mod end
}


// BBAI / K-Mod
int CvCityAI::AI_buildUnitProb()
{
	int iProb;
	iProb = (GC.getLeaderHeadInfo(getPersonalityType()).getBuildUnitProb() + AI_experienceWeight());
	
	//Civ4 Reimagined: small adjustment
	iProb += 8;

	if (!isBarbarian() && GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble())
	{
		iProb /= 2;
	}
	
	if( GET_TEAM(getTeam()).getHasMetCivCount(false) == 0 )
	{
		iProb /= 2;
	}

	if (GET_PLAYER(getOwnerINLINE()).getCurrentEra() < GC.getGameINLINE().getCurrentEra())
	{
		iProb *= std::max(40, 100 - 20 * (GC.getGameINLINE().getCurrentEra() - GET_PLAYER(getOwnerINLINE()).getCurrentEra()));
		iProb /= 100;
	}

	iProb *= (100 + 2*getMilitaryProductionModifier());
	iProb /= 100;
	//return iProb;
	return std::min(100, iProb); // experimental (K-Mod)
}

// Civ4 Reimagined: Changed piBestValue from int to long
void CvCityAI::AI_bestPlotBuild(CvPlot* pPlot, long* piBestValue, BuildTypes* peBestBuild, int iFoodPriority, int iProductionPriority, int iCommercePriority, bool bChop, int iHappyAdjust, int iHealthAdjust, int iDesiredFoodChange)
{
	PROFILE_FUNC();

	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE()); // K-Mod

	BuildTypes eBestBuild;

	bool bEmphasizeIrrigation = false;
	int iBestValue;

	if (piBestValue != NULL)
	{
		*piBestValue = 0;
	}
	if (peBestBuild != NULL)
	{
		*peBestBuild = NO_BUILD;
	}

	if (pPlot->getWorkingCity() != this)
	{
		return;
	}

	//When improving new plots only, count emphasis twice
	//helps to avoid too much tearing up of old improvements.

	// K-Mod: I've deleted the section about counting emphasis twice...
	// It's absurd to think it would help avoid tearing up old improvements.
	// If the emphasis changes as soon as the improvment is built, then that is likely to cause us to tear up the improvement!

	FAssertMsg(pPlot->getOwnerINLINE() == getOwnerINLINE(), "pPlot must be owned by this city's owner");

	BonusTypes eBonus = pPlot->getBonusType(getTeam());
	BonusTypes eNonObsoleteBonus = pPlot->getNonObsoleteBonusType(getTeam());

	/* original code
	bool bHasBonusImprovement = false;

	if (eNonObsoleteBonus != NO_BONUS)
	{
		if (pPlot->getImprovementType() != NO_IMPROVEMENT)
		{
			if (GC.getImprovementInfo(pPlot->getImprovementType()).isImprovementBonusTrade(eNonObsoleteBonus))
			{
				bHasBonusImprovement = true;
			}
		}
	} */
	bool bHasBonusImprovement = pPlot->getNonObsoleteBonusType(getTeam(), true) != NO_BONUS; // K-Mod
	
	//logBBAI("Best Build Evaluation (%d, %d)", pPlot->getX_INLINE(), pPlot->getY_INLINE());

	iBestValue = 0;
	eBestBuild = NO_BUILD;

	int iClearFeatureValue = 0;
	// K-Mod. I've removed the yield change evaluation from AI_clearFeatureValue (to avoid double counting it elsewhere)
	// but for the parts of this function, we want to count that value...
	int iClearValue_wYield = 0; // So I've made this new number for that purpose.

	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		iClearFeatureValue = AI_clearFeatureValue(getCityPlotIndex(pPlot));

		const CvFeatureInfo& kFeatureInfo = GC.getFeatureInfo(pPlot->getFeatureType());
		iClearValue_wYield = iClearFeatureValue;
		iClearValue_wYield -= kFeatureInfo.getYieldChange(YIELD_FOOD) * 100 * iFoodPriority / 100;
		iClearValue_wYield -= kFeatureInfo.getYieldChange(YIELD_PRODUCTION) * 60 * iProductionPriority / 100;
		iClearValue_wYield -= kFeatureInfo.getYieldChange(YIELD_COMMERCE) * 40 * iCommercePriority / 100;
	}

	if (!bHasBonusImprovement)
	{
		bEmphasizeIrrigation = false;

		CvPlot* pAdjacentPlot;
		CvPlot* pAdjacentPlot2;
		BonusTypes eTempBonus;

		//It looks unwieldly but the code has to be rigid to avoid "worker ADD"
		//where they keep connecting then disconnecting a crops resource or building
		//multiple farms to connect a single crop resource.
		//isFreshWater is used to ensure invalid plots are pruned early, the inner loop
		//really doesn't run that often.

		//using logic along the lines of "Will irrigating me make crops wet"
		//wont really work... it does have to "am i the tile the crops want to be irrigated"

		//I optimized through the use of "isIrrigated" which is just checking a bool...
		//once everything is nicely irrigated, this code should be really fast...
		if ((pPlot->isIrrigated()) || (pPlot->isFreshWater() && pPlot->canHavePotentialIrrigation()))
		{
			for (int iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
			{
				pAdjacentPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

				if ((pAdjacentPlot != NULL) && (pAdjacentPlot->getOwner() == getOwner()) && (pAdjacentPlot->isCityRadius()))
				{
					if (!pAdjacentPlot->isFreshWater())
					{
						//check for a city? cities can conduct irrigation and that effect is quite
						//useful... so I think irrigate cities.
						if (pAdjacentPlot->isPotentialIrrigation())
						{
							CvPlot* eBestIrrigationPlot = NULL;


							for (int iJ = 0; iJ < NUM_DIRECTION_TYPES; iJ++)
							{
								pAdjacentPlot2 = plotDirection(pAdjacentPlot->getX_INLINE(), pAdjacentPlot->getY_INLINE(), ((DirectionTypes)iJ));
								if ((pAdjacentPlot2 != NULL) && (pAdjacentPlot2->getOwner() == getOwner()))
								{
									eTempBonus = pAdjacentPlot2->getNonObsoleteBonusType(getTeam());
									if (pAdjacentPlot->isIrrigated())
									{
										//the irrigation has to be coming from somewhere
										if (pAdjacentPlot2->isIrrigated())
										{
											//if we find a tile which is already carrying irrigation
											//then lets not replace that one...
											eBestIrrigationPlot = pAdjacentPlot2;

											if ((pAdjacentPlot2->isCity()) || (eTempBonus != NO_BONUS) || (!pAdjacentPlot2->isCityRadius()))
											{
												if (pAdjacentPlot2->isFreshWater())
												{
													//these are all ideal for irrigation chains so stop looking.
													break;
												}
											}
										}

									}
									else
									{
										if (pAdjacentPlot2->getNonObsoleteBonusType(getTeam()) == NO_BONUS)
										{
											if (pAdjacentPlot2->canHavePotentialIrrigation() && pAdjacentPlot2->isIrrigationAvailable())
											{
												//could use more sophisticated logic
												//however this would rely on things like smart irrigation chaining
												//of out-of-city plots
												eBestIrrigationPlot = pAdjacentPlot2;
												break;
											}
										}
									}
								}
							}

							if (pPlot == eBestIrrigationPlot)
							{
								bEmphasizeIrrigation = true;
								break;
							}
						}
					}
				}
			}
		}
	}

	for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		ImprovementTypes eImprovement = ((ImprovementTypes)iI);
		BuildTypes eBestTempBuild;
		int iValue = AI_getImprovementValue(pPlot, eImprovement, iFoodPriority, iProductionPriority, iCommercePriority, iDesiredFoodChange, iClearFeatureValue, bEmphasizeIrrigation, &eBestTempBuild);
		if (eBestTempBuild != NO_BUILD)
		{
			//logBBAI("(%d, %d) %S: %d", pPlot->getX_INLINE(), pPlot->getY_INLINE(), GC.getBuildInfo(eBestTempBuild).getDescription(), iValue);
		}
		if (iValue > iBestValue)
		{
			iBestValue = iValue;
			eBestBuild = eBestTempBuild;
		}
	}

	// K-Mod. Don't chop the feature if we need it for our best improvement!
	if (eBestBuild == NO_BUILD
		? (pPlot->getImprovementType() == NO_IMPROVEMENT || GC.getImprovementInfo(pPlot->getImprovementType()).isRequiresFeature())
		: (GC.getBuildInfo(eBestBuild).getImprovement() != NO_IMPROVEMENT && GC.getImprovementInfo((ImprovementTypes)GC.getBuildInfo(eBestBuild).getImprovement()).isRequiresFeature()))
	{
		bChop = false;
		//logBBAI("Needs feature... (%d, %d)", pPlot->getX_INLINE(), pPlot->getY_INLINE());
	}
	else if (iClearValue_wYield > 0)
	// K-Mod end
	{
		FAssert(pPlot->getFeatureType() != NO_FEATURE);

		{
			/*if ((GC.getFeatureInfo(pPlot->getFeatureType()).getHealthPercent() < 0) ||
				((GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange(YIELD_FOOD) + GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange(YIELD_PRODUCTION) + GC.getFeatureInfo(pPlot->getFeatureType()).getYieldChange(YIELD_COMMERCE)) < 0))*/
			// Disabled by K-Mod. That stuff is already taken into account by iClearValue_wYield.
			{
				for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
				{
					BuildTypes eBuild = ((BuildTypes)iI);

					if (GC.getBuildInfo(eBuild).getImprovement() == NO_IMPROVEMENT)
					{
						if (GC.getBuildInfo(eBuild).isFeatureRemove(pPlot->getFeatureType()))
						{
							if (kOwner.canBuild(pPlot, eBuild))
							{
								int iValue = iClearValue_wYield;
								CvCity* pCity;
								iValue += (pPlot->getFeatureProduction(eBuild, getTeam(), &pCity) * 10); // Civ4 Reimagined: back to 10

								iValue *= 400;
								iValue /= std::max(1, (GC.getBuildInfo(eBuild).getFeatureTime(pPlot->getFeatureType()) + 100));

								//if ((iValue > iBestValue) || ((iValue > 0) && (eBestBuild == NO_BUILD)))
								if (iValue > iBestValue) // K-Mod. (removed redundant checks)
								{
									iBestValue = iValue;
									eBestBuild = eBuild;
								}
							}
						}
					}
				}
			}
		}
	}

	//Chop - maybe integrate this better with the other feature-clear code tho the logic
	//is kinda different
	if (bChop && (eBonus == NO_BONUS) && (pPlot->getFeatureType() != NO_FEATURE) &&
		(pPlot->getImprovementType() == NO_IMPROVEMENT) && !(kOwner.isOption(PLAYEROPTION_LEAVE_FORESTS)))
	{
		for (int iI = 0; iI < GC.getNumBuildInfos(); iI++)
		{
			BuildTypes eBuild = ((BuildTypes)iI);
			if (GC.getBuildInfo(eBuild).getImprovement() == NO_IMPROVEMENT)
			{
				if (GC.getBuildInfo(eBuild).isFeatureRemove(pPlot->getFeatureType()))
				{
					if (kOwner.canBuild(pPlot, eBuild))
					{
						CvCity* pCity;
						int iValue = (pPlot->getFeatureProduction(eBuild, getTeam(), &pCity)) * 10;
						FAssert(pCity == this);

						if (iValue > 0)
						{
							// K-Mod. Inflate the production value in the early expansion phase of the game.
							int iCitiesTarget = GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities();
							if (kOwner.getNumCities() < iCitiesTarget && kOwner.AI_getNumCitySites() > 0)
							{
								iValue = iValue * 3*iCitiesTarget / std::max(1, 2*kOwner.getNumCities() + iCitiesTarget);
							}
							// Increase the value if it is large compared to the city's natural production rate
							// (note: iValue is chop production * 10)
							iValue += iValue / (getBaseYieldRate(YIELD_PRODUCTION)*20 + iValue);
							// K-Mod end
							
							// Civ4 Reimagined
							//logBBAI("Feature Remove Value: %d (%d, %d)", iValue, pPlot->getX_INLINE(), pPlot->getY_INLINE());

							iValue += iClearValue_wYield;
							
							// K-Mod
							if (!pPlot->isBeingWorked() && iClearFeatureValue < 0)
							{
								iValue += iClearFeatureValue/2; // extra points for passive feature bonuses such as health
							}
							// K-Mod end

							if (iValue > 0)
							{
								if (kOwner.AI_isDoStrategy(AI_STRATEGY_DAGGER))
								{
									iValue += 20;
									iValue *= 2;
								}
								// K-Mod, flavour production, military, and growth.
								if (kOwner.AI_getFlavorValue(FLAVOR_PRODUCTION) > 0 || kOwner.AI_getFlavorValue(FLAVOR_MILITARY) + kOwner.AI_getFlavorValue(FLAVOR_GROWTH) > 2)
								{
									iValue *= 110 + 3 * kOwner.AI_getFlavorValue(FLAVOR_PRODUCTION) + 2 * kOwner.AI_getFlavorValue(FLAVOR_MILITARY) + 2 * kOwner.AI_getFlavorValue(FLAVOR_GROWTH);
									iValue /= 100;
								}
								// K-Mod end
								iValue *= 500;
								iValue /= std::max(1, (GC.getBuildInfo(eBuild).getFeatureTime(pPlot->getFeatureType()) + 100));

								if (iValue > iBestValue)
								{
									iBestValue = iValue;
									eBestBuild = eBuild;
								}
							}
						}
					}
				}
			}
		}
	}


	for (int iI = 0; iI < GC.getNumRouteInfos(); iI++)
	{
		RouteTypes eRoute = (RouteTypes)iI;
		RouteTypes eOldRoute = pPlot->getRouteType();

		if (eRoute != eOldRoute)
		{
			int iTempValue = 0;
			if (pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				if ((eOldRoute == NO_ROUTE) || (GC.getRouteInfo(eRoute).getValue() > GC.getRouteInfo(eOldRoute).getValue()))
				{
					iTempValue += ((GC.getImprovementInfo(pPlot->getImprovementType()).getRouteYieldChanges(eRoute, YIELD_FOOD)) * 100);
					iTempValue += ((GC.getImprovementInfo(pPlot->getImprovementType()).getRouteYieldChanges(eRoute, YIELD_PRODUCTION)) * 80); // was 60
					iTempValue += ((GC.getImprovementInfo(pPlot->getImprovementType()).getRouteYieldChanges(eRoute, YIELD_COMMERCE)) * 40);
				}

				if (pPlot->isBeingWorked())
				{
					iTempValue *= 2;
				}
				//road up bonuses if sort of bored.
				//if ((eOldRoute == NO_ROUTE) && (eBonus != NO_BONUS))
				if (!pPlot->isWater() && eOldRoute == NO_ROUTE && eBonus != NO_BONUS) // K-Mod
				{
					iTempValue += (pPlot->isConnectedToCapital() ? 10 : 30);
				}
			}

			if (iTempValue > 0)
			{
				for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
				{
					BuildTypes eBuild = ((BuildTypes)iJ);
					if (GC.getBuildInfo(eBuild).getRoute() == eRoute)
					{
						if (kOwner.canBuild(pPlot, eBuild, false))
						{
							//the value multiplier is based on the default time...
							int iValue = iTempValue * 5 * 300;
							iValue /= GC.getBuildInfo(eBuild).getTime();

							if ((iValue > iBestValue) || ((iValue > 0) && (eBestBuild == NO_BUILD)))
							{
								iBestValue = iValue;
								eBestBuild = eBuild;
							}
						}
					}
				}
			}
		}
	}

	if (eBestBuild != NO_BUILD)
	{
		FAssertMsg(iBestValue > 0, "iBestValue is expected to be greater than 0");

		/*
		//Now modify the priority for this build.
		if (GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble())
		{
			if (GC.getBuildInfo(eBestBuild).getImprovement() != NO_IMPROVEMENT)
			{
				iBestValue += (iBestValue * std::max(0, aiBestDiffYields[YIELD_COMMERCE])) / 4;
				iBestValue = std::max(1, iBestValue);
			}

		}
		*/

		if (piBestValue != NULL)
		{
			*piBestValue = iBestValue;
		}
		if (peBestBuild != NULL)
		{
			*peBestBuild = eBestBuild;
		}
	}
}


int CvCityAI::AI_getHappyFromHurry(HurryTypes eHurry) const
{
	return AI_getHappyFromHurry(hurryPopulation(eHurry));
}

int CvCityAI::AI_getHappyFromHurry(int iHurryPopulation) const
{
	PROFILE_FUNC();

	int iHappyDiff = iHurryPopulation - GC.getDefineINT("HURRY_POP_ANGER");
	if (iHappyDiff > 0)
	{
		if (getHurryAngerTimer() <= 1)
		{
			if (2 * angryPopulation(1) - healthRate(false, 1) > 1)
			{
				return iHappyDiff;
			}
		}
	}

	return 0;
}

int CvCityAI::AI_getHappyFromHurry(HurryTypes eHurry, UnitTypes eUnit, bool bIgnoreNew) const
{
	return AI_getHappyFromHurry(getHurryPopulation(eHurry, getHurryCost(true, eUnit, bIgnoreNew)));
}

int CvCityAI::AI_getHappyFromHurry(HurryTypes eHurry, BuildingTypes eBuilding, bool bIgnoreNew) const
{
	return AI_getHappyFromHurry(getHurryPopulation(eHurry, getHurryCost(true, eBuilding, bIgnoreNew)));
}


// K-Mod. I'm essentially rewriten this function.
// note: units ~ commerce x100
int CvCityAI::AI_cityValue() const
{
	PROFILE_FUNC();
	int iCosts;
	int iValue;
	int iHapValue;
	int iLoop;
	int iCount;
	
	/* original bts code
	AreaAITypes eAreaAI = area()->getAreaAIType(getTeam());
    if ((eAreaAI == AREAAI_OFFENSIVE) || (eAreaAI == AREAAI_MASSING) || (eAreaAI == AREAAI_DEFENSIVE))
    {
        return 0;
    } */  // This isn't relevant to city value. It should be handled elsewhere.

	// K-Mod: disorder causes the commerce rates to go to zero... so that would  mess up our calculations
	//if (isDisorder())
	if (isDisorder())
		return 0;
	// K-Mod end

	iValue = 0;
	iHapValue = 0;
	iCount = 0;

	/* original bts code
	iValue += getCommerceRateTimes100(COMMERCE_GOLD);
	iValue += getCommerceRateTimes100(COMMERCE_RESEARCH);
	iValue += 100 * getYieldRate(YIELD_PRODUCTION);
	iValue -= 3 * calculateColonyMaintenanceTimes100(); */
	// K-Mod. The original code fails for civs using high espionage or high culture.
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	iValue += getCommerceRateTimes100(COMMERCE_RESEARCH) * kOwner.AI_commerceWeight(COMMERCE_RESEARCH);
	iValue += getCommerceRateTimes100(COMMERCE_ESPIONAGE) * kOwner.AI_commerceWeight(COMMERCE_ESPIONAGE);
	iValue /= 100;
	// Culture isn't counted, because its value is local. Unfortunately this will mean that civs running 100% culture
	// (ie CULTURE4) will give up their non-core cities. It could be argued that this would be good strategy,
	// but the problem is that CULTURE4 doesn't always run its full course. ... so I'm going to make a small ad hoc adjustment...
	iValue += 100 * getYieldRate(YIELD_PRODUCTION);
	iValue *= kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE4)? 2 : 1;
	// Gold value is not weighted, and does not get the cultural victory boost, because gold is directly comparable to maintenance.
	iValue += getCommerceRateTimes100(COMMERCE_GOLD);

	//int iCosts = calculateColonyMaintenanceTimes100() + 2*getMaintenanceTimes100()/3;
	
	// Civ4 Reimagined
	int iSplitCities = area()->getCitiesPerPlayer(getOwnerINLINE());
	iCosts = getCommerceRateTimes100(COMMERCE_RESEARCH) * kOwner.AI_commerceWeight(COMMERCE_RESEARCH) * kOwner.getVassalCommerceGain() / 100;
	iCosts += getCommerceRateTimes100(COMMERCE_ESPIONAGE) * kOwner.AI_commerceWeight(COMMERCE_ESPIONAGE) * kOwner.getVassalCommerceGain() / 100;
	iCosts += getCommerceRateTimes100(COMMERCE_GOLD) * kOwner.getVassalCommerceGain();
	iCosts /= 100;
	iCosts += (unhappyLevel() - happyLevel()) * kOwner.getVassalCommerceGain();
	iCosts += calculateColonyMaintenanceTimes100() + getMaintenanceTimes100();
	// Reduced NumCitiesMaintenaince in all cities after split
	iCosts += kOwner.getNumCities() - iSplitCities;

	iCosts = iCosts * (100+kOwner.getInflationRate()) / 100;
	
	// Civ4 Reimagined: split if we have not enough workers there
	iCosts += iCosts/4 * std::max(0,(kOwner.AI_neededWorkers(area()) / 4 - kOwner.AI_totalAreaUnitAIs(area(), UNITAI_WORKER)));
	
	/*
	if (iCosts != 0)
	{
		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			iCosts += GC.getCivicInfo(kOwner.getCivics((CivicOptionTypes)iI)).getColonyTradeModifier();
		}
	}
	*/
	
	// slightly encourage empire split when aiming for a diplomatic victory
	if (kOwner.AI_isDoVictoryStrategy(AI_VICTORY_DIPLOMACY3) && kOwner.getCommercePercent(COMMERCE_GOLD) > 0)
	{
		iCosts = iCosts * 4 / 3;
	}
	
	// Civ4 Reimagined: discourage empire split when aiming for domination victory
	if (kOwner.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION3) || kOwner.AI_isDoVictoryStrategy(AI_VICTORY_DOMINATION4))
	{
		iValue *= 2;
	}
	
	int iTargetPop = std::max(5, AI_getTargetPopulation()); // target pop is not a good measure for small cities w/ unimproved tiles.
	if (getPopulation() > 0 && getPopulation() < iTargetPop)
	{
		iValue *= iTargetPop + 3;
		iValue /= getPopulation() + 3;
		iCosts *= getPopulation() + 6;
		iCosts /= iTargetPop + 6;
	}
	
	// Civ4 Reimagined: Additional vassal happiness after split
	for (CvCity* pLoopCity = kOwner.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kOwner.nextCity(&iLoop))
	{
		if (pLoopCity->area() == area() || pLoopCity->isNoUnhappiness())
		{
			continue;
		}
		int iCurrentHappy = 100*(pLoopCity->happyLevel() - pLoopCity->unhappyLevel(1));
		// I'm only going to subtract half of the commerce happiness, because we might not be using that commerce for only happiness.
		iCurrentHappy -= 50*std::max(0, pLoopCity->getCommerceHappiness());
		int iTestHappy = iCurrentHappy + 100;
		iHapValue += std::max(0, -iCurrentHappy) - std::max(0, -iTestHappy); // change in the number of angry citizens
		// a small bonus for happiness beyond what we need
		iHapValue += 100*(std::max(0, iTestHappy) - std::max(0, iCurrentHappy))/(400 + std::max(0, iTestHappy) + std::max(0, iCurrentHappy));
		iCount++;
	}
	
	iCosts += ((0 == iCount) ? 0 : iHapValue / iCount) * kOwner.getNumCities();
	
	if (gCityLogLevel >= 2 && iCosts != 0) logBBAI("%S Colony Value: %d , Cost: %d (HapValue: %d)", getName().GetCString(), iValue, iCosts, iHapValue);
	
	iValue -= iCosts;
	// K-Mod end

	return iValue;
}


// Modified by Civ4 Reimagined: Added Danger Code
bool CvCityAI::AI_doPanic()
{
	//bool bLandWar = ((area()->getAreaAIType(getTeam()) == AREAAI_OFFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_DEFENSIVE) || (area()->getAreaAIType(getTeam()) == AREAAI_MASSING));
	bool bLandWar = GET_PLAYER(getOwnerINLINE()).AI_isLandWar(area()); // K-Mod
	bool bDanger = false; // Civ4 Reimagined
	int iRatio = 0;
	
	// Civ4 Reimagined
	if (plot()->plotCount(PUF_canDefendGroupHead, -1, -1, getOwnerINLINE(), NO_TEAM, PUF_isCityAIType) < 2)
	{
		if (AI_isDanger())
		{
			bDanger = true;
		}
	}
	
	if (!bDanger && bLandWar) //Civ4 Reimagined
	{
		int iOurDefense = GET_PLAYER(getOwnerINLINE()).AI_localDefenceStrength(plot(), getTeam());
		int iEnemyOffense = GET_PLAYER(getOwnerINLINE()).AI_localAttackStrength(plot(), NO_TEAM);
		iRatio = (100 * iEnemyOffense) / (std::max(1, iOurDefense));
	}

	if (iRatio > 100 || bDanger) // Civ4 Reimagined
	{
		UnitTypes eProductionUnit = getProductionUnit();

		if (eProductionUnit != NO_UNIT)
		{
			if (getProduction() > 0)
			{
				if (GC.getUnitInfo(eProductionUnit).getCombat() > 0)
				{
					AI_doHurry(true);
					return true;
				}
			}
		}
		
		if ((GC.getGame().getSorenRandNum(2, "AI choose panic unit") == 0) && AI_chooseUnit(UNITAI_CITY_COUNTER))
		{
			AI_doHurry((iRatio > 140));	
		}
		else if (AI_chooseUnit(UNITAI_CITY_DEFENSE))
		{
			AI_doHurry((iRatio > 140));
		}
		else if (AI_chooseUnit(UNITAI_ATTACK))
		{
			AI_doHurry((iRatio > 140));
		}
		
	}
	return false;
}

/* disabled by K-Mod. (this function is buggy, and obsolete)
int CvCityAI::AI_calculateCulturePressure(bool bGreatWork) const
{
	PROFILE_FUNC();
    CvPlot* pLoopPlot;
    BonusTypes eNonObsoleteBonus;
    int iValue;
    int iTempValue;
    int iI;

    iValue = 0;
    for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
    {
        pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
		if (pLoopPlot != NULL)
		{
		    if (pLoopPlot->getOwnerINLINE() == NO_PLAYER)
		    {
		        iValue++;
		    }
		    else
		    {
                iTempValue = pLoopPlot->calculateCulturePercent(getOwnerINLINE());
                if (iTempValue == 100)
                {
                    //do nothing
                }
                else if ((iTempValue == 0) || (iTempValue > 75))
                {
                    iValue++;
                }
                else
                {
                    iTempValue = (100 - iTempValue);
                    FAssert(iTempValue > 0);
                    FAssert(iTempValue <= 100);

                    if (iI != CITY_HOME_PLOT)
                    {
                        iTempValue *= 4;
                        iTempValue /= NUM_CITY_PLOTS;
                    }

                    eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(getTeam());

                    if (eNonObsoleteBonus != NO_BONUS)
                    {
                        iTempValue += (GET_PLAYER(getOwnerINLINE()).AI_bonusVal(eNonObsoleteBonus, 0) * ((GET_PLAYER(getOwnerINLINE()).getNumTradeableBonuses(eNonObsoleteBonus) == 0) ? 4 : 2));
                    }
					//if ((iTempValue > 80) && (pLoopPlot->getOwnerINLINE() == getID()))
					if ((iTempValue > 80) && (pLoopPlot->getOwnerINLINE() == getOwnerINLINE())) // unofficial_patch
                    {
                        //captured territory special case
                        iTempValue *= (100 - iTempValue);
                        iTempValue /= 100;
                    }

                    if (pLoopPlot->getTeam() == getTeam())
                    {
                        iTempValue /= (bGreatWork ? 10 : 2);
                    }
                    else
                    {
                        iTempValue *= 2;
                        if (bGreatWork)
                        {
                            if (GET_PLAYER(getOwnerINLINE()).AI_getAttitude(pLoopPlot->getOwnerINLINE()) == ATTITUDE_FRIENDLY)
                            {
                                //iValue /= 10;
								iTempValue /= 10; // K-Mod
                            }
                        }
                    }

                    iValue += iTempValue;
                }
            }
		}
    }

    return iValue;
} */

// K-Mod: I've completely butchered this function. ie. deleted the bulk of it, and rewritten it.
void CvCityAI::AI_buildGovernorChooseProduction()
{
	PROFILE_FUNC();

	AI_setChooseProductionDirty(false);
	clearOrderQueue();

	CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE());
	CvArea* pWaterArea = waterArea();
	bool bWasFoodProduction = isFoodProduction();
	bool bDanger = AI_isDanger();

	BuildingTypes eBestBuilding = AI_bestBuildingThreshold(); // go go value cache!
	int iBestBuildingValue = (eBestBuilding == NO_BUILDING) ? 0 : AI_buildingValue(eBestBuilding);

    // pop borders
	if (getCultureLevel() <= (CultureLevelTypes)1 && getCommerceRate(COMMERCE_CULTURE) == 0)
	{
		if (eBestBuilding != NO_BUILDING && AI_countGoodTiles(true, false) > 0)
		{
			const CvBuildingInfo& kBestBuilding = GC.getBuildingInfo(eBestBuilding);
			if (kBestBuilding.getCommerceChange(COMMERCE_CULTURE) + kBestBuilding.getObsoleteSafeCommerceChange(COMMERCE_CULTURE) > 0
				&& (GC.getNumCultureLevelInfos() < 2 || getProductionTurnsLeft(eBestBuilding, 0) <= GC.getGame().getCultureThreshold((CultureLevelTypes)2)))
			{
				pushOrder(ORDER_CONSTRUCT, eBestBuilding);
				return;
			}
		}
		if (AI_countGoodTiles(true, true, 60) == 0 && AI_chooseProcess(COMMERCE_CULTURE))
		{
			return;
		}
	}
	
	//workboat
	if (pWaterArea != NULL)
	{
		if (kOwner.AI_totalWaterAreaUnitAIs(pWaterArea, UNITAI_WORKER_SEA) == 0)
		{
			if (AI_neededSeaWorkers() > 0)
			{
				bool bLocalResource = AI_countNumImprovableBonuses(true, NO_TECH, false, true) > 0;
				if (bLocalResource || iBestBuildingValue < 20)
				{
					int iOdds = 100;
					iOdds *= 50 + iBestBuildingValue;
					iOdds /= 50 + (bLocalResource ? 3 : 5) * iBestBuildingValue;
					if (AI_chooseUnit(UNITAI_WORKER_SEA, iOdds))
					{
						return;
					}
				}
			}
		}
	}
	
	if (kOwner.AI_isPrimaryArea(area())) // if its not a primary area, let the player ship units in.
	{
		//worker
		if (!bDanger)
		{
			int iExistingWorkers = kOwner.AI_totalAreaUnitAIs(area(), UNITAI_WORKER);
			int iNeededWorkers = kOwner.AI_neededWorkers(area());

			if (iExistingWorkers < (iNeededWorkers + 2)/3) // I don't want to build more workers than the player actually wants.
			{
				int iOdds = 100;
				iOdds *= iNeededWorkers - iExistingWorkers;
				iOdds /= std::max(1, iNeededWorkers);
				iOdds *= 50 + iBestBuildingValue;
				iOdds /= 50 + 5 * iBestBuildingValue;

				if (AI_chooseUnit(UNITAI_WORKER, iOdds))
				{
					return;
				}
			}
		}

		// adjust iBestBuildValue for the remaining comparisons. (military and spies.)
		if (GC.getNumEraInfos() > 1)
		{
			FAssert(kOwner.getCurrentEra() < GC.getNumEraInfos());
			iBestBuildingValue *= 2*(GC.getNumEraInfos()-1) - kOwner.getCurrentEra();
			iBestBuildingValue /= GC.getNumEraInfos()-1;
		}

		//military
		bool bWar = area()->getAreaAIType(kOwner.getTeam()) != AREAAI_NEUTRAL;
		// (I'd like to check AI_neededFloatingDefenders, but I don't want to have to calculate it just for this singluar case.)
		if (bDanger || iBestBuildingValue < (bWar ? 70 : 35))
		{
			int iOdds = (bWar ? 100 : 60) - kOwner.AI_unitCostPerMil()/2;
			iOdds += AI_experienceWeight();
			iOdds *= 100 + 2*getMilitaryProductionModifier();
			iOdds /= 100;

			iOdds *= 60 + iBestBuildingValue;
			iOdds /= 60 + 10 * iBestBuildingValue;

			if (GC.getGameINLINE().getSorenRandNum(100, "City governor choose military") < iOdds)
			{
				UnitAITypes eUnitAI;
				UnitTypes eBestUnit = AI_bestUnit(false, NO_ADVISOR, &eUnitAI);
				if (eBestUnit != NO_UNIT)
				{
					const CvUnitInfo& kUnit = GC.getUnitInfo(eBestUnit);

					if (kUnit.getUnitCombatType() != NO_UNITCOMBAT)
					{
						BuildingTypes eExperienceBuilding = AI_bestBuildingThreshold(BUILDINGFOCUS_EXPERIENCE);

						if (eExperienceBuilding != NO_BUILDING)
						{
							const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eExperienceBuilding);
							if (kBuilding.getFreeExperience() > 0 ||
								kBuilding.getUnitCombatFreeExperience(kUnit.getUnitCombatType()) > 0 ||
								kBuilding.getDomainFreeExperience(kUnit.getDomainType()) > 0)
							{
								// This building helps the unit we want
								// ...so do the building first.
								pushOrder(ORDER_CONSTRUCT, eBestBuilding);
								return;
							}
						}
					}
					// otherwise, we're ready to build the unit
					// Civ4 Reimagined
					if (getUnitProductionMultiplier(eBestUnit) > 0)
					{
						pushOrder(ORDER_TRAIN, eBestUnit, eUnitAI);
						return;
					}
					else
					{
						// Civ4 Reimagined: Hire mercenary when not having the resources
						for (int iI = 0; iI < GC.getNumHurryInfos(); iI++)
						{
							if (!canHurry((HurryTypes)iI))
								continue;
							int iHurryGold = hurryGold((HurryTypes)iI);
							if (iHurryGold > 0 && iHurryGold <= kOwner.getGold() - kOwner.AI_goldTarget(true))
							{
								pushOrder(ORDER_TRAIN, eBestUnit, eUnitAI);
								if (gCityLogLevel >= 2)
									logBBAI("      City %S hires mercenary without resource", getName().GetCString() );
								hurry((HurryTypes)iI);
								return;
							}
						}
					}
				}
			}
		}

		//spies
		if (!kOwner.AI_isAreaAlone(area()))
		{
			int iNumSpies = kOwner.AI_totalAreaUnitAIs(area(), UNITAI_SPY) + kOwner.AI_getNumTrainAIUnits(UNITAI_SPY);
			int iNeededSpies = 2 + area()->getCitiesPerPlayer(kOwner.getID()) / 5;
			iNeededSpies += kOwner.getCommercePercent(COMMERCE_ESPIONAGE)/20;

			if (iNumSpies < iNeededSpies)
			{
				int iOdds = 35 - iBestBuildingValue/3;
				if (iOdds > 0)
				{
					iOdds *= (40 + iBestBuildingValue);
					iOdds /= (20 + 3 * iBestBuildingValue);
					iOdds *= iNeededSpies;
					iOdds /= (4*iNumSpies+iNeededSpies);
					if (AI_chooseUnit(UNITAI_SPY, iOdds))
					{
						return;
					}
				}
			}
		}

		//spread
		int iSpreadUnitOdds = std::max(0, 80 - iBestBuildingValue*2);

		int iSpreadUnitThreshold = 1200 + (bWar ? 800: 0) + iBestBuildingValue * 25;
		// is it wrong to use UNITAI values for human players?
		iSpreadUnitThreshold += kOwner.AI_totalAreaUnitAIs(area(), UNITAI_MISSIONARY) * 1000;

		UnitTypes eBestSpreadUnit = NO_UNIT;
		int iBestSpreadUnitValue = -1;
		if (AI_bestSpreadUnit(true, true, iSpreadUnitOdds, &eBestSpreadUnit, &iBestSpreadUnitValue))
		{
			if (iBestSpreadUnitValue > iSpreadUnitThreshold)
			{
				if (AI_chooseUnit(eBestSpreadUnit, UNITAI_MISSIONARY))
				{
					return;
				}
				FAssertMsg(false, "AI_bestSpreadUnit should provide a valid unit when it returns true");
			}
		}
	}

	//process
	ProcessTypes eBestProcess = AI_bestProcess();
	if (eBestProcess != NO_PROCESS)
	{
		// similar to AI_chooseProduction
		int iOdds = 100;
		if (eBestBuilding != NO_BUILDING)
		{
			int iBuildingCost = AI_processValue(eBestProcess) * getProductionTurnsLeft(eBestBuilding, 0);
			int iScaledTime = 10000 * iBuildingCost / (std::max(1, iBestBuildingValue) * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent());
			iOdds = 100*(iScaledTime - 400)/(iScaledTime + 600); // <= 4 turns means 0%. 20 turns ~ 61%.
		}
		if (iOdds > 0 && GC.getGameINLINE().getSorenRandNum(100, "AI choose process") < iOdds)
		{
			pushOrder(ORDER_MAINTAIN, eBestProcess);
			return;
		}
	}

	//default
	if (AI_chooseBuilding())
		return;

	/*if (AI_chooseProcess())
		return; */

    if (AI_chooseUnit())
		return;
}

// K-Mod. This is a chunk of code that I moved out of AI_chooseProduction. The only reason I've moved it is to reduce clutter in the other function.
void CvCityAI::AI_barbChooseProduction()
{
	const CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());

	CvArea* pArea = area();
	CvArea* pWaterArea = waterArea(true);
	bool bMaybeWaterArea = false;
	bool bWaterDanger = false;
	bool bDanger = AI_isDanger();

	if (pWaterArea != NULL)
	{
		bMaybeWaterArea = true;
		if (!GET_TEAM(getTeam()).AI_isWaterAreaRelevant(pWaterArea))
		{
			pWaterArea = NULL;
		}

		bWaterDanger = kPlayer.AI_getWaterDanger(plot(), 4) > 0;
	}

	int iNumCitiesInArea = pArea->getCitiesPerPlayer(getOwnerINLINE());

	int iBuildUnitProb = AI_buildUnitProb();

	int iExistingWorkers = kPlayer.AI_totalAreaUnitAIs(pArea, UNITAI_WORKER);
	int iNeededWorkers = kPlayer.AI_neededWorkers(pArea);
	// Sea worker need independent of whether water area is militarily relevant
	int iNeededSeaWorkers = (bMaybeWaterArea) ? AI_neededSeaWorkers() : 0;
	int iExistingSeaWorkers = (waterArea(true) != NULL) ? kPlayer.AI_totalWaterAreaUnitAIs(waterArea(true), UNITAI_WORKER_SEA) : 0;
	int iWaterPercent = AI_calculateWaterWorldPercent();

	if (!AI_isDefended(plot()->plotCount(PUF_isUnitAIType, UNITAI_ATTACK, -1, getOwnerINLINE()))) // XXX check for other team's units?
	{
		if (AI_chooseDefender())
		{
			return;
		}

		if (AI_chooseUnit(UNITAI_ATTACK))
		{
			return;
		}
	}

	// K-Mod. Allow barbs to pop their borders when they can.
	if (getCultureLevel() <= 1 && AI_chooseProcess(COMMERCE_CULTURE))
		return;
	// K-Mod end

	if (!bDanger && (2*iExistingWorkers < iNeededWorkers) && (AI_getWorkersNeeded() > 0) && (AI_getWorkersHave() == 0))
	{
		if( getPopulation() > 1 || (GC.getGameINLINE().getGameTurn() - getGameTurnAcquired() > (15 * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent())/100) )
		{
			if (AI_chooseUnit(UNITAI_WORKER))
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses barb choose worker 1", getName().GetCString());
				return;
			}
		}
	}

	if (!bDanger && !bWaterDanger && (iNeededSeaWorkers > 0))
	{
		if (AI_chooseUnit(UNITAI_WORKER_SEA))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses barb choose worker sea 1", getName().GetCString());
			return;
		}
	}

	int iFreeLandExperience = getSpecialistFreeExperience() + getDomainFreeExperience(DOMAIN_LAND);
	iBuildUnitProb += (3 * iFreeLandExperience);

	bool bRepelColonists = false;
	if( area()->getNumCities() > area()->getCitiesPerPlayer(BARBARIAN_PLAYER) + 2 )
	{
		if( area()->getCitiesPerPlayer(BARBARIAN_PLAYER) > area()->getNumCities()/3 )
		{
			// New world scenario with invading colonists ... fight back!
			bRepelColonists = true;
			iBuildUnitProb += 8*(area()->getNumCities() - area()->getCitiesPerPlayer(BARBARIAN_PLAYER));
		}
	}

	if (!bDanger && GC.getGameINLINE().getSorenRandNum(100, "AI Build Unit Production") > iBuildUnitProb)
	{

		int iBarbarianFlags = 0;
		if( getPopulation() < 4 ) iBarbarianFlags |= BUILDINGFOCUS_FOOD;
		iBarbarianFlags |= BUILDINGFOCUS_PRODUCTION;
		iBarbarianFlags |= BUILDINGFOCUS_EXPERIENCE;
		if( getPopulation() > 3 ) iBarbarianFlags |= BUILDINGFOCUS_DEFENSE;

		if (AI_chooseBuilding(iBarbarianFlags, 15))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses barb AI_chooseBuilding with flags and iBuildUnitProb = %d", getName().GetCString(), iBuildUnitProb);
			return;
		}

		if( GC.getGameINLINE().getSorenRandNum(100, "AI Build Unit Production") > iBuildUnitProb)
		{
			if (AI_chooseBuilding())
			{
				if( gCityLogLevel >= 2 ) logBBAI("      City %S uses barb AI_chooseBuilding without flags and iBuildUnitProb = %d", getName().GetCString(), iBuildUnitProb);
				return;
			}
		}
	}

	if (plot()->plotCount(PUF_isUnitAIType, UNITAI_ASSAULT_SEA, -1, getOwnerINLINE()) > 0)
	{
		if (AI_chooseUnit(UNITAI_ATTACK_CITY))
		{
			if( gCityLogLevel >= 2 ) logBBAI("      City %S uses barb choose attack city for transports", getName().GetCString());
			return;
		}
	}

	if (!bDanger && (pWaterArea != NULL) && (iWaterPercent > 30))
	{
		if (GC.getGameINLINE().getSorenRandNum(3, "AI Coast Raiders!") == 0)
		{
			if (kPlayer.AI_totalUnitAIs(UNITAI_ASSAULT_SEA) <= (1 + kPlayer.getNumCities() / 2))
			{
				if (AI_chooseUnit(UNITAI_ASSAULT_SEA))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses barb choose transport", getName().GetCString());
					return;
				}
			}
		}
		if (GC.getGameINLINE().getSorenRandNum(110, "AI arrrr!") < (iWaterPercent + 10))
		{
			if (kPlayer.AI_totalUnitAIs(UNITAI_PIRATE_SEA) <= kPlayer.getNumCities())
			{
				if (AI_chooseUnit(UNITAI_PIRATE_SEA))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses barb choose pirate", getName().GetCString());
					return;
				}
			}

			if (kPlayer.AI_totalAreaUnitAIs(pWaterArea, UNITAI_ATTACK_SEA) < iNumCitiesInArea)
			{
				if (AI_chooseUnit(UNITAI_ATTACK_SEA))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses barb choose attack sea", getName().GetCString());
					return;
				}
			}
		}
	}

	if (GC.getGameINLINE().getSorenRandNum(2, "Barb worker") == 0)
	{
		if (!bDanger && (iExistingWorkers < iNeededWorkers) && (AI_getWorkersNeeded() > 0) && (AI_getWorkersHave() == 0))
		{
			if( getPopulation() > 1 )
			{
				if (AI_chooseUnit(UNITAI_WORKER))
				{
					if( gCityLogLevel >= 2 ) logBBAI("      City %S uses barb choose worker 2", getName().GetCString());
					return;
				}
			}
		}
	}

	UnitTypeWeightArray barbarianTypes;
	barbarianTypes.push_back(std::make_pair(UNITAI_ATTACK, 125));
	barbarianTypes.push_back(std::make_pair(UNITAI_ATTACK_CITY, (bRepelColonists ? 100 : 50)));
	barbarianTypes.push_back(std::make_pair(UNITAI_COUNTER, 100));
	barbarianTypes.push_back(std::make_pair(UNITAI_CITY_DEFENSE, 50));

	if (AI_chooseLeastRepresentedUnit(barbarianTypes))
	{
		return;
	}

	if (AI_chooseUnit())
	{
		return;
	}

	return;
}
// K-Mod end

int CvCityAI::AI_calculateWaterWorldPercent()
{
	int iI;
	int iWaterPercent = 0;
	int iTeamCityCount = 0;
	int iOtherCityCount = 0;
	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		if (GET_TEAM((TeamTypes)iI).isAlive())
		{
			if (iI == getTeam() || GET_TEAM((TeamTypes)iI).isVassal(getTeam())
				|| GET_TEAM(getTeam()).isVassal((TeamTypes)iI))
			{
				iTeamCityCount += GET_TEAM((TeamTypes)iI).countNumCitiesByArea(area());
			}
			else
			{
				iOtherCityCount += GET_TEAM((TeamTypes)iI).countNumCitiesByArea(area());
			}
		}
	}

	if (iOtherCityCount == 0)
	{
		iWaterPercent = 100;
	}
	else
	{
		iWaterPercent = 100 - ((iTeamCityCount + iOtherCityCount) * 100) / std::max(1, (GC.getGame().getNumCities()));
	}

	iWaterPercent *= 50;
	iWaterPercent /= 100;

	iWaterPercent += (50 * (2 + iTeamCityCount)) / (2 + iTeamCityCount + iOtherCityCount);

	iWaterPercent = std::max(1, iWaterPercent);


	return iWaterPercent;
}

//Please note, takes the yield multiplied by 100
int CvCityAI::AI_getYieldMagicValue(const int* piYieldsTimes100, bool bHealthy) const
{
	FAssert(piYieldsTimes100 != NULL);

	int iPopEats = GC.getFOOD_CONSUMPTION_PER_POPULATION();
	iPopEats += (bHealthy ? 0 : 1);
	iPopEats *= 100;

	int iValue = ((piYieldsTimes100[YIELD_FOOD] * 100 + piYieldsTimes100[YIELD_PRODUCTION]*70 + piYieldsTimes100[YIELD_COMMERCE]*40) - iPopEats * 102); // prod was *55
	iValue /= 100;
	return iValue;
}

//The magic value is basically "Look at this plot, is it worth working"
//-50 or lower means the plot is worthless in a "workers kill yourself" kind of way.
//-50 to -1 means the plot isn't worth growing to work - might be okay with emphasize though.
//Between 0 and 50 means it is marginal.
//50-100 means it's okay.
//Above 100 means it's definitely decent - seriously question ever not working it.
//This function deliberately doesn't use emphasize settings.
int CvCityAI::AI_getPlotMagicValue(CvPlot* pPlot, bool bHealthy, bool bWorkerOptimization) const
{
	int aiYields[NUM_YIELD_TYPES];
	ImprovementTypes eCurrentImprovement;
	ImprovementTypes eFinalImprovement;
	int iI;
	int iYieldDiff;

	FAssert(pPlot != NULL);

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		if ((bWorkerOptimization) && (pPlot->getWorkingCity() == this) && (AI_getBestBuild(getCityPlotIndex(pPlot)) != NO_BUILD))
		{
			aiYields[iI] = pPlot->getYieldWithBuild(AI_getBestBuild(getCityPlotIndex(pPlot)), (YieldTypes)iI, true);
		}
		else
		{
			aiYields[iI] = pPlot->getYield((YieldTypes)iI) * 100;
		}
	}

	eCurrentImprovement = pPlot->getImprovementType();

	if (eCurrentImprovement != NO_IMPROVEMENT)
	{
		eFinalImprovement = finalImprovementUpgrade(eCurrentImprovement);

		if ((eFinalImprovement != NO_IMPROVEMENT) && (eFinalImprovement != eCurrentImprovement))
		{
			for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
			{
				iYieldDiff = 100 * pPlot->calculateImprovementYieldChange(eFinalImprovement, ((YieldTypes)iI), getOwnerINLINE());
				iYieldDiff -= 100 * pPlot->calculateImprovementYieldChange(eCurrentImprovement, ((YieldTypes)iI), getOwnerINLINE());
				aiYields[iI] += iYieldDiff / 2;
			}
		}
	}

	return AI_getYieldMagicValue(aiYields, bHealthy);
}

//useful for deciding whether or not to grow... or whether the city needs terrain
//improvement.
//if healthy is false it assumes bad health conditions.
int CvCityAI::AI_countGoodTiles(bool bHealthy, bool bUnworkedOnly, int iThreshold, bool bWorkerOptimization) const
{
	CvPlot* pLoopPlot;
	int iI;
	int iCount;

	iCount = 0;
	for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = plotCity(getX_INLINE(),getY_INLINE(), iI);
		if ((iI != CITY_HOME_PLOT) && (pLoopPlot != NULL))
		{
			if (pLoopPlot->getWorkingCity() == this)
			{
				if (!bUnworkedOnly || !(pLoopPlot->isBeingWorked()))
				{
					if (AI_getPlotMagicValue(pLoopPlot, bHealthy, bWorkerOptimization) > iThreshold)
					{
						iCount++;
					}
				}
			}
		}
	}
	return iCount;
}

/* disabled by K-Mod. (this function always returns 1, so I've decided not to use it at all)
int CvCityAI::AI_calculateTargetCulturePerTurn() const
{
	**
	int iTarget = 0;
	
	bool bAnyGoodPlotUnowned = false;
	bool bAnyGoodPlotHighPressure = false;	
	
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		CvPlot* pLoopPlot = plotCity(getX_INLINE(),getY_INLINE(),iI);
        
        if (pLoopPlot != NULL)
        {
			if ((pLoopPlot->getBonusType(getTeam()) != NO_BONUS)
				|| (pLoopPlot->getYield(YIELD_FOOD) > GC.getFOOD_CONSUMPTION_PER_POPULATION()))
			{
				if (!pLoopPlot->isOwned())
				{
					bAnyGoodPlotUnowned = true;        			
				}
				else if (pLoopPlot->getOwnerINLINE() != getOwnerINLINE())
				{
					bAnyGoodPlotHighPressure = true;
				}
			}
        }
	}
	if (bAnyGoodPlotUnowned)
	{
		iTarget = 1;
	}
	if (bAnyGoodPlotHighPressure)
	{
		iTarget += getCommerceRate(COMMERCE_CULTURE) + 1;
	}
	return iTarget;
	**
	return 1;
} */
	
int CvCityAI::AI_countGoodSpecialists(bool bHealthy) const
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
	int iCount = 0;
	for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		SpecialistTypes eSpecialist = (SpecialistTypes)iI;

		int iValue = 0;

		iValue += 100 * kPlayer.specialistYield(eSpecialist, YIELD_FOOD);
		iValue += 65 * kPlayer.specialistYield(eSpecialist, YIELD_PRODUCTION);
		iValue += 40 * kPlayer.specialistYield(eSpecialist, YIELD_COMMERCE);

		iValue += 40 * kPlayer.specialistCommerce(eSpecialist, COMMERCE_RESEARCH);
		iValue += 40 * kPlayer.specialistCommerce(eSpecialist, COMMERCE_GOLD);
		iValue += 20 * kPlayer.specialistCommerce(eSpecialist, COMMERCE_ESPIONAGE);
		iValue += 15 * kPlayer.specialistCommerce(eSpecialist, COMMERCE_CULTURE);
		iValue += 25 * GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange();

		if (iValue >= (bHealthy ? 200 : 300))
		{
			//iCount += getMaxSpecialistCount(eSpecialist);
			// K-Mod
			// Civ4 Reimagined: Removed Specialist.isValid
			if (eSpecialist == GC.getDefineINT("DEFAULT_SPECIALIST"))
				return getPopulation(); // unlimited

			iCount += getMaxSpecialistCount(eSpecialist) - getSpecialistCount(eSpecialist);

			// Civ4 Reimagined: This assertion is not always true. This method is sometimes called during an AI turn when citizens have not been correctly assigned yet.
			//FAssert(getMaxSpecialistCount(eSpecialist) - getSpecialistCount(eSpecialist) >= 0);
			// K-Mod end
		}
	}
	//iCount -= getFreeSpecialist();
	iCount -= totalFreeSpecialists(); // K-Mod

	//return iCount;
	return std::max(0, iCount); // K-Mod
}
//0 is normal
//higher than zero means special.
// (K-Mod todo: this function is currently only used for CvCityAI::AI_stealPlots, and even there it is only used in a very binary way.
// I think I can make this function more versitile, and then use it to bring some consistency to other parts of the code.)
int CvCityAI::AI_getCityImportance(bool bEconomy, bool bMilitary)
{
    int iValue = 0;
	if (GET_PLAYER(getOwnerINLINE()).AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3)) // K-Mod (bbai used culture2)
    {
        int iCultureRateRank = findCommerceRateRank(COMMERCE_CULTURE);
        int iCulturalVictoryNumCultureCities = GC.getGameINLINE().culturalVictoryNumCultureCities();
        
        if (iCultureRateRank <= iCulturalVictoryNumCultureCities)
        {
            iValue += 100;
        
            if ((getCultureLevel() < (GC.getNumCultureLevelInfos() - 1)))
            {
                iValue += !bMilitary ? 100 : 0;
            }
            else
            {
                iValue += bMilitary ? 100 : 0;
            }
        }
    }
    
    return iValue;
}

// K-Mod
int CvCityAI::AI_calculateMilitaryOutput() const
{
	int iValue = 0;

	iValue = getBaseYieldRate(YIELD_PRODUCTION);
	//UnitTypes eDefaultUnit = getConscriptUnit();

	iValue *= 100 + getMilitaryProductionModifier() + 10 * getProductionExperience();
	iValue /= 100;

	return iValue;
}

void CvCityAI::AI_stealPlots()
{
    PROFILE_FUNC();
    CvPlot* pLoopPlot;
    CvCityAI* pWorkingCity;
    int iI;
    int iOtherImportance;
    
    int iImportance = AI_getCityImportance(true, false);

    for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
    {
        pLoopPlot = plotCity(getX_INLINE(),getY_INLINE(),iI);
        
        if (pLoopPlot != NULL)
        {
            if (iImportance > 0)
            {
                if (pLoopPlot->getOwnerINLINE() == getOwnerINLINE())
                {
                    pWorkingCity = static_cast<CvCityAI*>(pLoopPlot->getWorkingCity());
                    if ((pWorkingCity != this) && (pWorkingCity != NULL))
                    {
                        FAssert(pWorkingCity->getOwnerINLINE() == getOwnerINLINE());
                        iOtherImportance = pWorkingCity->AI_getCityImportance(true, false);
                        if (iImportance > iOtherImportance)
                        {
                            pLoopPlot->setWorkingCityOverride(this);
                        }
                    }
                }
            }
                
            if (pLoopPlot->getWorkingCityOverride() == this)
            {
                if (pLoopPlot->getOwnerINLINE() != getOwnerINLINE())
                {
                    pLoopPlot->setWorkingCityOverride(NULL);                    
                }
            }
        }
    }
}



// original description (obsolete):
// +1/+3/+5 plot based on base food yield (1/2/3)
// +4 if being worked.
// +4 if a bonus.
// Unworked ocean ranks very lowly. Unworked lake ranks at 3. Worked lake at 7.
// Worked bonus in ocean ranks at like 11
//int CvCityAI::AI_buildingSpecialYieldChangeValue(BuildingTypes kBuilding, YieldTypes eYield) const

// K-Mod. I've rewritten this function to give a lower but more consistent value.
// The function should return roughly 4x the number of affected plots which are expected to be worked.
// +4 if either being worked, or would be good after the bonus. (~consumption rate +4 commerce / 2 yield)
// +2/3 if would be ok after the bonus. (~consumption rate +2 commerce / 1 yield)
// +1 otherwise
int CvCityAI::AI_buildingSeaYieldChangeWeight(BuildingTypes eBuilding, bool bGrowing) const
{
	int iValue = 0;
	const CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	// only add the building's bonuses if it isn't already built - otherwise the building will be overvalued for sabortage missions.
	bool bBonuses = getNumBuilding(eBuilding) < GC.getCITY_MAX_NUM_BUILDINGS();

	for (int i = 0; i < NUM_CITY_PLOTS; i++)
	{
		CvPlot* pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), i);
		if (!pLoopPlot || pLoopPlot->getWorkingCity() != this)
			continue;

		// Civ4 Reimagined: Added lake case
		if (!pLoopPlot->isWater() || pLoopPlot->isLake())
			continue;

		if (pLoopPlot->isBeingWorked())
			iValue += 4;
		else
		{
			const int iCommerceValue = 4;
			const int iProdValue = 7;
			const int iFoodValue = 11;

			int iPlotValue =
				iFoodValue * (pLoopPlot->getYield(YIELD_FOOD) + (bBonuses ? kBuilding.getSeaPlotYieldChange(YIELD_FOOD) : 0)) +
				iProdValue * (pLoopPlot->getYield(YIELD_PRODUCTION) + (bBonuses ? kBuilding.getSeaPlotYieldChange(YIELD_PRODUCTION) : 0)) +
				iCommerceValue * (pLoopPlot->getYield(YIELD_COMMERCE) + (bBonuses ? kBuilding.getSeaPlotYieldChange(YIELD_COMMERCE) : 0));

			const int iBaseValue = iFoodValue * GC.getFOOD_CONSUMPTION_PER_POPULATION();

			if (iPlotValue > iBaseValue + 3 * iCommerceValue)
				iValue += 4;
			else if (iPlotValue >= iBaseValue + 2 * iCommerceValue)
				iValue += bGrowing ? 3 : 2;
			else
				iValue += 1;
		}
	}

	return iValue;
}
// K-Mod end


int CvCityAI::AI_yieldMultiplier(YieldTypes eYield) const
{
	PROFILE_FUNC();
	
	int iMultiplier = getBaseYieldRateModifier(eYield);
	
	if (eYield == YIELD_PRODUCTION)
	{
		iMultiplier += (getMilitaryProductionModifier() / 2);
	}
	
	if (eYield == YIELD_COMMERCE)
	{
		/* original code
		iMultiplier += (getCommerceRateModifier(COMMERCE_RESEARCH) * 60) / 100;
		iMultiplier += (getCommerceRateModifier(COMMERCE_GOLD) * 35) / 100;
		iMultiplier += (getCommerceRateModifier(COMMERCE_CULTURE) * 15) / 100; */
		// K-Mod
		// this breakdown seems wrong... lets try it a different way.
		const CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
		int iExtra = 0;
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			iExtra += getTotalCommerceRateModifier((CommerceTypes)iI) * kPlayer.getCommercePercent((CommerceTypes)iI);
		}
		// base commerce modifier compounds with individual commerce modifiers.
		iMultiplier *= iExtra;
		iMultiplier /= 10000;
		// K-Mod end
	}
	
	return iMultiplier;	
}
//this should be called before doing governor stuff.
//This is the function which replaces emphasis
//Could stand for a Commerce Variety to be added
//especially now that there is Espionage
void CvCityAI::AI_updateSpecialYieldMultiplier()
{
	PROFILE_FUNC();
	
	for (int iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSpecialYieldMultiplier[iI] = 0;
	}
	
	UnitTypes eProductionUnit = getProductionUnit();
	if (eProductionUnit != NO_UNIT)
	{
		/* original bts code
		if (GC.getUnitInfo(eProductionUnit).getDefaultUnitAIType() == UNITAI_WORKER_SEA)
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 50;
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 50;
		}
		if ((GC.getUnitInfo(eProductionUnit).getDefaultUnitAIType() == UNITAI_WORKER) ||
			(GC.getUnitInfo(eProductionUnit).getDefaultUnitAIType() == UNITAI_SETTLE))
		
		{
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 50;
		} */
		// K-Mod. note: when food is production, food is counted as production!
		UnitAITypes eUnitAI = (UnitAITypes)GC.getUnitInfo(eProductionUnit).getDefaultUnitAIType();
		if (eUnitAI == UNITAI_WORKER_SEA || eUnitAI == UNITAI_WORKER || eUnitAI == UNITAI_SETTLE)
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 50;
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 25;
		}
		// K-Mod end
	}

	BuildingTypes eProductionBuilding = getProductionBuilding();
	if (eProductionBuilding != NO_BUILDING)
	{
		if (isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eProductionBuilding).getBuildingClassType()))
			|| isProductionProject())
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 50;
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 25;					
		}
		m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += std::max(-25, GC.getBuildingInfo(eProductionBuilding).getFoodKept());
		
		/* original bts code
		if ((GC.getBuildingInfo(eProductionBuilding).getCommerceChange(COMMERCE_CULTURE) > 0)
			|| (GC.getBuildingInfo(eProductionBuilding).getObsoleteSafeCommerceChange(COMMERCE_CULTURE) > 0))
		{
			int iTargetCultureRate = AI_calculateTargetCulturePerTurn();
			if (iTargetCultureRate > 0)
			{
				if (getCommerceRate(COMMERCE_CULTURE) == 0)
				{
					m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 50;
				}
				else if (getCommerceRate(COMMERCE_CULTURE) < iTargetCultureRate)
				{
					m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 20;					
				}
			}
		} */ // emphasising production to get culture is nice, but not if it slows growth
	}
	
	// non-human production value increase
	if (!isHuman())
	{
		CvPlayerAI& kPlayer = GET_PLAYER(getOwnerINLINE());
		AreaAITypes eAreaAIType = area()->getAreaAIType(getTeam());

		// K-Mod. special strategy / personality adjustments
		if (kPlayer.AI_isDoStrategy(AI_STRATEGY_PRODUCTION))
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 20;
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 20;
		}
		else if (findBaseYieldRateRank(YIELD_PRODUCTION) <= kPlayer.getNumCities()/3 && findBaseYieldRateRank(YIELD_PRODUCTION) < findBaseYieldRateRank(YIELD_COMMERCE))
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 10;
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 10;
		}
		
		if (kPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE1 | AI_VICTORY_SPACE1))
		{
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] += 5;
		}

		if (kPlayer.AI_getFlavorValue(FLAVOR_PRODUCTION) > 0)
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 5 + 2*kPlayer.AI_getFlavorValue(FLAVOR_PRODUCTION);
		}
		if (kPlayer.AI_isDoStrategy(AI_STRATEGY_ECONOMY_FOCUS))
		{
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] -= 10;
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] += 20;
		}
		else if (kPlayer.AI_isDoVictoryStrategy(AI_STRATEGY_GET_BETTER_UNITS)) // doesn't stack with ec focus.
		{
			m_aiSpecialYieldMultiplier[YIELD_COMMERCE] += 20;
		}
		// K-Mod end
		
		if ((kPlayer.AI_isDoStrategy(AI_STRATEGY_DAGGER) && getPopulation() >= 4)
			|| (eAreaAIType == AREAAI_OFFENSIVE) || (eAreaAIType == AREAAI_DEFENSIVE) 
			|| (eAreaAIType == AREAAI_MASSING) || (eAreaAIType == AREAAI_ASSAULT))
		{
			/* original bts code
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 10;
			if (!kPlayer.AI_isFinancialTrouble())
			{
				m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= 40;
			} */
			// K-Mod. Don't sacrifice lots of commerce unless we're on the defensive, or this is 'total war'.
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += kPlayer.AI_isDoStrategy(AI_STRATEGY_DAGGER | AI_STRATEGY_CRUSH | AI_STRATEGY_TURTLE) ? 20 : 10;
			if (eAreaAIType != AREAAI_NEUTRAL && !kPlayer.AI_isFinancialTrouble() && !kPlayer.AI_isDoStrategy(AI_STRATEGY_ECONOMY_FOCUS | AI_STRATEGY_GET_BETTER_UNITS))
			{
				const CvTeamAI& kTeam = GET_TEAM(kPlayer.getTeam());
				bool bSeriousWar = eAreaAIType == AREAAI_DEFENSIVE || kPlayer.isBarbarian();
				for (TeamTypes i = (TeamTypes)0; !bSeriousWar && i < MAX_CIV_TEAMS; i=(TeamTypes)(i+1))
				{
					if (GET_TEAM(i).isAlive())
					{
						WarPlanTypes ePlan = kTeam.AI_getWarPlan(i);
						FAssert(ePlan == NO_WARPLAN || (kTeam.isHasMet(i) && GET_TEAM(i).isAlive()));
						bSeriousWar = ePlan == WARPLAN_PREPARING_TOTAL || ePlan == WARPLAN_TOTAL;
					}
				}
				m_aiSpecialYieldMultiplier[YIELD_COMMERCE] -= bSeriousWar ? 35 : 10;
			}
			// K-Mod end
		}
		
		//int iIncome = 1 + kPlayer.getCommerceRate(COMMERCE_GOLD) + kPlayer.getCommerceRate(COMMERCE_RESEARCH) + std::max(0, kPlayer.getGoldPerTurn());
		//int iExpenses = 1 + kPlayer.calculateInflatedCosts() - std::min(0, kPlayer.getGoldPerTurn());
		int iIncome = 1 + kPlayer.AI_getAvailableIncome(); // K-Mod
		int iExpenses = 1 + kPlayer.calculateInflatedCosts() + std::max(0, -kPlayer.getGoldPerTurn()); // K-Mod (just to be consistent with similar calculations)
		FAssert(iIncome > 0);
		
		int iRatio = (100 * iExpenses) / iIncome;
		//Gold -> Production Reduced To
		// 40- -> 100%
		// 60 -> 83%
		// 100 -> 28%
		// 110+ -> 14%
		m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] += 100;
		if (iRatio > 60)
		{
			//Greatly decrease production weight 
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] *= std::max(10, 120 - iRatio);
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] /= 72;
		}
		else if (iRatio > 40)
		{
			//Slightly decrease production weight.
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] *= 160 - iRatio;
			m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] /= 120;
		}
		m_aiSpecialYieldMultiplier[YIELD_PRODUCTION] -= 100;
	}
}

int CvCityAI::AI_specialYieldMultiplier(YieldTypes eYield) const
{
	return m_aiSpecialYieldMultiplier[eYield];
}


int CvCityAI::AI_countNumBonuses(BonusTypes eBonus, bool bIncludeOurs, bool bIncludeNeutral, int iOtherCultureThreshold, bool bLand, bool bWater)
{
    CvPlot* pLoopPlot;
    BonusTypes eLoopBonus;
    int iI;
    int iCount = 0;
    for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
    {
        pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
        
        if (pLoopPlot != NULL)
        {
        	if ((pLoopPlot->area() == area()) || (bWater && pLoopPlot->isWater()))
        	{
				eLoopBonus = pLoopPlot->getBonusType(getTeam());
				if (eLoopBonus != NO_BONUS)
				{
					if ((eBonus == NO_BONUS) || (eBonus == eLoopBonus))
					{
						if (bIncludeOurs && (pLoopPlot->getOwnerINLINE() == getOwnerINLINE()) && (pLoopPlot->getWorkingCity() == this))
						{
							iCount++;                    
						}
						else if (bIncludeNeutral && (!pLoopPlot->isOwned()))
						{
							iCount++;
						}
						else if ((iOtherCultureThreshold > 0) && (pLoopPlot->isOwned() && (pLoopPlot->getOwnerINLINE() != getOwnerINLINE())))
						{
							if ((pLoopPlot->getCulture(pLoopPlot->getOwnerINLINE()) - pLoopPlot->getCulture(getOwnerINLINE())) < iOtherCultureThreshold)
							{
								iCount++;
							}                        
						}
					}
				}
        	}
        }
    }
    
    
    return iCount;    
    
}

// BBAI. K-Mod: I've rearranged some stuff and fixed some bugs.
int CvCityAI::AI_countNumImprovableBonuses( bool bIncludeNeutral, TechTypes eExtraTech, bool bLand, bool bWater )
{
	CvPlot* pLoopPlot;
    BonusTypes eLoopBonus;
    int iI;
    int iCount = 0;
    for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
    {
        pLoopPlot = plotCity(getX_INLINE(), getY_INLINE(), iI);
        
        if (pLoopPlot != NULL)
        {
        	if ((bLand && pLoopPlot->area() == area()) || (bWater && pLoopPlot->isWater()))
        	{
				eLoopBonus = pLoopPlot->getBonusType(getTeam());
				if (eLoopBonus != NO_BONUS && (GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getBonusInfo(eLoopBonus).getTechCityTrade()) || GC.getBonusInfo(eLoopBonus).getTechCityTrade() == eExtraTech))
				{
					if ( ((pLoopPlot->getOwnerINLINE() == getOwnerINLINE()) && (pLoopPlot->getWorkingCity() == this)) || (bIncludeNeutral && (!pLoopPlot->isOwned())))
					{
						for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
						{
							BuildTypes eBuild = ((BuildTypes)iJ);
							ImprovementTypes eImp = (ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement();

							if (eImp != NO_IMPROVEMENT && pLoopPlot->canBuild(eBuild, getOwnerINLINE()))
							{
								if (GC.getImprovementInfo(eImp).isImprovementBonusTrade(eLoopBonus) || GC.getImprovementInfo(eImp).isActsAsCity())
								{
									if( GET_PLAYER(getOwnerINLINE()).canBuild(pLoopPlot, eBuild) )
									{
										iCount++;
										break;
									}
									else if( (eExtraTech != NO_TECH) )
									{
										if (GC.getBuildInfo(eBuild).getTechPrereq() == eExtraTech)
										{
											iCount++;
											break;
										}
									}
								}
							}
						}
					}
				}
        	}
        }
    }
    
    
    return iCount;
}
// bbai / K-Mod end

int CvCityAI::AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance)
{
	FAssert(GET_PLAYER(eIndex).isAlive());

	if ((m_iCachePlayerClosenessTurn != GC.getGame().getGameTurn())
		|| (m_iCachePlayerClosenessDistance != iMaxDistance))
	{
		AI_cachePlayerCloseness(iMaxDistance);
	}
	
	return m_aiPlayerCloseness[eIndex];	
}

// K-Mod
int CvCityAI::AI_highestTeamCloseness(TeamTypes eTeam)
{
	int iCloseness = -1;
	for (PlayerTypes i = (PlayerTypes)0; i < MAX_PLAYERS; i=(PlayerTypes)(i+1))
	{
		if (GET_PLAYER(i).getTeam() == eTeam)
		{
			iCloseness = std::max(iCloseness, AI_playerCloseness(i, DEFAULT_PLAYER_CLOSENESS));
		}
	}

	return iCloseness;
}
// K-Mod end

void CvCityAI::AI_cachePlayerCloseness(int iMaxDistance)
{
	PROFILE_FUNC();
	CvCity* pLoopCity;
	int iI;
	int iLoop;
	int iValue;
	int iTempValue;
	int iBestValue;

/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						5/16/10				jdog5000		*/
/* 																				*/
/* 	General AI, closeness changes												*/
/********************************************************************************/	
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive() && 
			((GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))))
		{
			iValue = 0;
			iBestValue = 0;
			for (pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
			{
				if( pLoopCity == this )
				{
					continue;
				}

				int iDistance = stepDistance(getX_INLINE(), getY_INLINE(), pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE());
				
				if (area() != pLoopCity->area() )
				{
					iDistance += 1;
					iDistance /= 2;
				}
				if (iDistance <= iMaxDistance)
				{
					if ( getArea() == pLoopCity->getArea() )
					{
						int iPathDistance = GC.getMap().calculatePathDistance(plot(), pLoopCity->plot());
						if (iPathDistance > 0)
						{
							iDistance = iPathDistance;
						}
					}
					if (iDistance <= iMaxDistance)
					{
						// Weight by population of both cities, not just pop of other city
						//iTempValue = 20 + 2*pLoopCity->getPopulation();
						iTempValue = 20 + pLoopCity->getPopulation() + getPopulation();

						iTempValue *= (1 + (iMaxDistance - iDistance));
						iTempValue /= (1 + iMaxDistance);
						
						//reduce for small islands.
						int iAreaCityCount = pLoopCity->area()->getNumCities();
						iTempValue *= std::min(iAreaCityCount, 5);
						iTempValue /= 5;
						if (iAreaCityCount < 3)
						{
							iTempValue /= 2;
						}
						
						if (pLoopCity->isBarbarian())
						{
							iTempValue /= 4;
						}
						
						iValue += iTempValue;					
						iBestValue = std::max(iBestValue, iTempValue);
					}
				}
			}
			m_aiPlayerCloseness[iI] = (iBestValue + iValue / 4);
		}
	}
/********************************************************************************/
/* 	BETTER_BTS_AI_MOD						END							        */
/********************************************************************************/
	
	m_iCachePlayerClosenessTurn = GC.getGame().getGameTurn();	
	m_iCachePlayerClosenessDistance = iMaxDistance;
}

// K-Mod
// return true if there is an adjacent plot not owned by us.
bool CvCityAI::AI_isFrontlineCity() const
{
	for (int i = 0; i < NUM_DIRECTION_TYPES; i++)
	{
		CvPlot* pAdjacentPlot = plotDirection(getX_INLINE(), getY_INLINE(), (DirectionTypes)i);

		if (pAdjacentPlot && !pAdjacentPlot->isWater() && pAdjacentPlot->getTeam() != getTeam())
			return true;
	}

	return false;
}
// K-Mod end

// K-Mod has made significant structural & function changes to this function. (loosely described in the comments throughout the function.)
int CvCityAI::AI_cityThreat(bool bDangerPercent)
{
	PROFILE_FUNC();

	int iTotalThreat = 0; // was (iValue)
	const CvPlayerAI& kOwner = GET_PLAYER(getOwnerINLINE()); // K-Mod
	bool bCrushStrategy = kOwner.AI_isDoStrategy(AI_STRATEGY_CRUSH);

	for (int iI = 0; iI < MAX_PLAYERS; iI++)
	{
		const CvPlayerAI& kLoopPlayer = GET_PLAYER((PlayerTypes)iI); // K-Mod
		//if ((iI != getOwner()) && GET_PLAYER((PlayerTypes)iI).isAlive())
		if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() != getTeam() && !GET_TEAM(kOwner.getTeam()).isVassal(kLoopPlayer.getTeam())) // K-Mod
		{
			int iAccessFactor = AI_playerCloseness((PlayerTypes)iI, DEFAULT_PLAYER_CLOSENESS); // (was "iTempValue")
			// (this is roughly 20 points for each neighbouring city.)

			// K-Mod
			// Add some more points for each plot the loop player owns in our fat-cross.
			if (iAccessFactor > 0)
			{
				for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
				{
					CvPlot* pLoopPlot = getCityIndexPlot(iJ);
					if (pLoopPlot && pLoopPlot->getOwnerINLINE() == iI)
					{
						iAccessFactor += (stepDistance(getX_INLINE(), getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE()) <= 1) ? 2 : 1;
					}
				}
			}

			// Evaluate the posibility of a naval assault.
			if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
			{
				// This evaluation may be expensive (when I finish writing it), so only do it if there is reason to be concerned.
				if (kOwner.AI_isDoVictoryStrategyLevel4() ||
					GET_TEAM(getTeam()).AI_getWarPlan(kLoopPlayer.getTeam()) != NO_WARPLAN ||
					(!kOwner.AI_isLandWar(area()) && kLoopPlayer.AI_getAttitude(getOwnerINLINE()) < ATTITUDE_PLEASED))
				{
					int iNavalAccess = 0;

					//PROFILE("AI_cityThreat naval assault");
					// (temporary calculation based on the original bts code. I intend to make this significantly more detailed - eventually.)
					/* int iCurrentEra = kOwner.getCurrentEra();
					iValue += std::max(0, ((10 * iCurrentEra) / 3) - 6); */

					int iEra = GC.getGameINLINE().getCurrentEra();
					iNavalAccess += std::max(0, 30*(iEra+1)/(GC.getNumEraInfos()+1) - 10);

					iAccessFactor = std::max(iAccessFactor, iNavalAccess);
				}
			}
			// K-Mod end

			if (iAccessFactor > 0)
			{
				int iCivFactor; // K-Mod. (originally the following code had iTempValue *= foo; rather than iCivFactor = foo;)

				if (bCrushStrategy && GET_TEAM(getTeam()).AI_getWarPlan(kLoopPlayer.getTeam()) != NO_WARPLAN)
				{
					iCivFactor = 400;
				}
				else if (atWar(getTeam(), kLoopPlayer.getTeam()))
				{
					iCivFactor = 300;
				}
				// Beef up border security before starting war, but not too much (bbai)
				else if ( GET_TEAM(getTeam()).AI_getWarPlan(kLoopPlayer.getTeam()) != NO_WARPLAN )
				{
					iCivFactor = 180;
				}
				else
				{
					switch (kOwner.AI_getAttitude((PlayerTypes)iI))
					// (K-Mod note: for good strategy, this should probably be _their_ attitude rather than ours.
					//  But perhaps for role-play it is better the way it is.)
					{
					case ATTITUDE_FURIOUS:
						iCivFactor = 180;
						break;

					case ATTITUDE_ANNOYED:
						iCivFactor = 130;
						break;

					case ATTITUDE_CAUTIOUS:
						iCivFactor = 100;
						break;

					case ATTITUDE_PLEASED:
						iCivFactor = 50;
						break;

					case ATTITUDE_FRIENDLY:
						iCivFactor = 20;
						break;

					default:
						FAssert(false);
						break;
					}

					// K-Mod
					// Reduce threat level of our vassals, particularly from capilutated vassals.
					if (GET_TEAM(kLoopPlayer.getTeam()).isVassal(getTeam()))
					{
						iCivFactor = std::min(iCivFactor, GET_TEAM(kLoopPlayer.getTeam()).isCapitulated() ? 30 : 50);
					}
					else
					{
						// Increase threat level for cities that we have captured from this player
						// I may add a turn limit later if this produces unwanted behaviour. (getGameTurn() - getGameTurnAcquired() < 40)
						if (getPreviousOwner() == iI)
							iCivFactor = iCivFactor * 3/2;

						// Don't get too comfortable if kLoopPlayer is using a conquest strategy.
						if (kLoopPlayer.AI_isDoVictoryStrategy(AI_VICTORY_CONQUEST4 | AI_VICTORY_DOMINATION4))
							iCivFactor = std::max(100, iCivFactor);
					}
					// K-Mod end

					if (bCrushStrategy)
					{
						iCivFactor /= 2;
					}
				}

				// K-Mod. Amplify the threat rating for rivals which have high power.
				if (kLoopPlayer.getPower() > kOwner.getPower())
				{
					/* bbai version
					iTempValue *= std::min( 400, (100 * GET_PLAYER((PlayerTypes)iI).getPower())/std::max(1, GET_PLAYER(getOwnerINLINE()).getPower()) );
					iTempValue /= 100; */

					// exclude population power. (Should this use the same power comparison used to calculate areaAI and war values?)
					int iLoopPower = kLoopPlayer.getPower() - getPopulationPower(kLoopPlayer.getTotalPopulation());
					int iOurPower = kOwner.getPower() - getPopulationPower(kOwner.getTotalPopulation());
					iCivFactor *= range(100 * iLoopPower/std::max(1, iOurPower), 100, 400);
					iCivFactor /= 100;
				}

				/* iTempValue /= 100;
				iTotalThreat += iTempValue; */
				iTotalThreat += iAccessFactor * iCivFactor / 100; // K-Mod
			}
		}
	}

	/* original bts code
	if (isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		int iCurrentEra = kOwner.getCurrentEra();
		iValue += std::max(0, ((10 * iCurrentEra) / 3) - 6); //there are better ways to do this
	} */ // this is now included in the iAccessFactor calculation

	/* original bts code
	iValue += getNumActiveWorldWonders() * 5;

	if (kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3))
	{
		iValue += 5;
		iValue += getCommerceRateModifier(COMMERCE_CULTURE) / 20;
		if (getCultureLevel() >= (GC.getNumCultureLevelInfos() - 2))
		{
			iValue += 20;
			if (getCultureLevel() >= (GC.getNumCultureLevelInfos() - 1))
			{
				iValue += 30;
			}
		}
	} */

	iTotalThreat += 3 * kOwner.AI_getPlotDanger(plot(), 3, false); // was 2 *

	// K-Mod. Increase the threat rating for high-value cities.
	// (Note: this replaces the culture & wonder stuff above)
	// (Note2: I may end up moving this stuff into AI_getCityImportance)
	if (iTotalThreat > 0)
	{
		int iImportanceFactor = 100;
		iImportanceFactor += 20 * getNumActiveWorldWonders();
		iImportanceFactor += isHolyCity() ? 20 : 0;
		for (CorporationTypes i = (CorporationTypes)0; i < GC.getNumCorporationInfos(); i=(CorporationTypes)(i+1))
		{
			if (isHeadquarters(i))
				iImportanceFactor += 10; // note: the corp HQ building counts as an active world wonder in addition to this value.
		}

		if (kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE3))
		{
			if (getCultureLevel() >= GC.getGameINLINE().culturalVictoryCultureLevel() - 1)
			{
				iImportanceFactor += 20;
				if (findCommerceRateRank(COMMERCE_CULTURE) <= GC.getGameINLINE().culturalVictoryNumCultureCities())
				{
					iImportanceFactor += 30;
					if (kOwner.AI_isDoVictoryStrategy(AI_VICTORY_CULTURE4))
					{
						iImportanceFactor += 100;
					}
				}
			}
		}
		if (isCapital())
		{
			iImportanceFactor = std::max(iImportanceFactor, 150);
			if (GET_TEAM(kOwner.getTeam()).AI_getLowestVictoryCountdown() > 0)
				iImportanceFactor += 100;
		}
		iTotalThreat = iTotalThreat * iImportanceFactor / 100;
	}
	// K-Mod end

	return iTotalThreat;
}

//Workers have/needed is not intended to be a strict
//target but rather an indication.
//if needed is at least 1 that means a worker
//will be doing something useful
int CvCityAI::AI_getWorkersHave()
{
	return m_iWorkersHave;	
}

int CvCityAI::AI_getWorkersNeeded()
{
	return m_iWorkersNeeded;
}

void CvCityAI::AI_changeWorkersHave(int iChange)
{
	m_iWorkersHave += iChange;
	//FAssert(m_iWorkersHave >= 0);
	m_iWorkersHave = std::max(0, m_iWorkersHave);
}
	
//This needs to be serialized for human workers.
void CvCityAI::AI_updateWorkersNeededHere()
{
	PROFILE_FUNC();

	CvPlot* pLoopPlot;

	short aiYields[NUM_YIELD_TYPES];

	int iWorkersNeeded = 0;
	int iWorkersHave = 0;
	int iUnimprovedWorkedPlotCount = 0;
	int iUnimprovedUnworkedPlotCount = 0;
	int iWorkedUnimprovableCount = 0;
	int iImprovedUnworkedPlotCount = 0;

	int iSpecialCount = 0;

	int iWorstWorkedPlotValue = MAX_INT;
	int iBestUnworkedPlotValue = 0;

	int iGrowthValue = AI_growthValuePerFood(); // K-Mod

	if (getProductionUnit() != NO_UNIT)
	{
		if (getProductionUnitAI() == UNITAI_WORKER)
		{
			if (getProductionTurnsLeft() <= 2)
			{
				iWorkersHave++;
			}
		}
	}
	for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
	{
		pLoopPlot = getCityIndexPlot(iI);

		if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this)
		{
			//if (pLoopPlot->getArea() == getArea())
			{
				//How slow is this? It could be almost NUM_CITY_PLOT times faster
				//by iterating groups and seeing if the plot target lands in this city
				//but since this is only called once/turn i'm not sure it matters.
				iWorkersHave += (GET_PLAYER(getOwnerINLINE()).AI_plotTargetMissionAIs(pLoopPlot, MISSIONAI_BUILD));

				iWorkersHave += pLoopPlot->plotCount(PUF_isUnitAIType, UNITAI_WORKER, -1, getOwner(), getTeam(), PUF_isNoMission, -1, -1);
				if (iI != CITY_HOME_PLOT)
				{
					if (pLoopPlot->getImprovementType() == NO_IMPROVEMENT)
					{
						if (pLoopPlot->isBeingWorked())
						{
							//if (AI_getBestBuild(iI) != NO_BUILD)
							if (AI_getBestBuild(iI) != NO_BUILD && pLoopPlot->getArea() == getArea()) // K-Mod
							{
								iUnimprovedWorkedPlotCount++;
							}
							else
							{
								iWorkedUnimprovableCount++;
							}
						}
						else
						{
							//if (AI_getBestBuild(iI) != NO_BUILD)
							if (AI_getBestBuild(iI) != NO_BUILD && pLoopPlot->getArea() == getArea()) // K-Mod
							{
								iUnimprovedUnworkedPlotCount++;
							}
						}
					}
					else
					{
						if (!pLoopPlot->isBeingWorked())
						{
							iImprovedUnworkedPlotCount++;
						}
					}

					for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
					{
						aiYields[iJ] = pLoopPlot->getYield((YieldTypes)iJ);
					}

					if (pLoopPlot->isBeingWorked())
					{
						int iPlotValue = AI_yieldValue(aiYields, NULL, false, false, true, true, iGrowthValue);
						iWorstWorkedPlotValue = std::min(iWorstWorkedPlotValue, iPlotValue);					
					}
					else
					{
						int iPlotValue = AI_yieldValue(aiYields, NULL, false, false, true, true, iGrowthValue);
						iBestUnworkedPlotValue = std::max(iBestUnworkedPlotValue, iPlotValue);
					}
				}
			}
		}
	}
	//specialists?

	//iUnimprovedWorkedPlotCount += std::min(iUnimprovedUnworkedPlotCount, iWorkedUnimprovableCount) / 2;
	// K-Mod
	{
		int iPopDelta = AI_getTargetPopulation() - getPopulation();
		int iFutureWork = std::max(0, iWorkedUnimprovableCount + range((iPopDelta+1)/2, 0, 3) - iImprovedUnworkedPlotCount);
		iUnimprovedWorkedPlotCount += (std::min(iUnimprovedUnworkedPlotCount, iFutureWork)+1) / 2;
	}
	// K-Mod end

	iWorkersNeeded += 2 * iUnimprovedWorkedPlotCount;

	int iBestPotentialPlotValue = -1;
	if (iWorstWorkedPlotValue != MAX_INT)
	{
		//Add an additional citizen to account for future growth.
		int iBestPlot = -1;
		SpecialistTypes eBestSpecialist = NO_SPECIALIST;

		if (angryPopulation() == 0)
		{
			AI_addBestCitizen(true, true, &iBestPlot, &eBestSpecialist);
		}
		
		for (int iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			if (iI != CITY_HOME_PLOT)
			{
				pLoopPlot = getCityIndexPlot(iI);

				if (NULL != pLoopPlot && pLoopPlot->getWorkingCity() == this && pLoopPlot->getArea() == getArea())
				{
					if (AI_getBestBuild(iI) != NO_BUILD)
					{
						for (int iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
						{
							aiYields[iJ] = pLoopPlot->getYieldWithBuild(m_aeBestBuild[iI], (YieldTypes)iJ, true);
						}

						int iPlotValue = AI_yieldValue(aiYields, NULL, false, false, true, true, iGrowthValue);
						ImprovementTypes eImprovement = (ImprovementTypes)GC.getBuildInfo(AI_getBestBuild(iI)).getImprovement();
						if (eImprovement != NO_IMPROVEMENT)
						{
							if ((getImprovementFreeSpecialists(eImprovement) > 0) || (GC.getImprovementInfo(eImprovement).getHappiness() > 0))
							{
								iSpecialCount ++;
							}
						}
						iBestPotentialPlotValue = std::max(iBestPotentialPlotValue, iPlotValue);
					}
				}
			}
		}

		if (iBestPlot != -1)
		{
			setWorkingPlot(iBestPlot, false);
		}
		if (eBestSpecialist != NO_SPECIALIST)
		{
			changeSpecialistCount(eBestSpecialist, -1);
		}

		if (iBestPotentialPlotValue > iWorstWorkedPlotValue)
		{
			iWorkersNeeded += 2;
		}
	}

	iWorkersNeeded += (std::max(0, iUnimprovedWorkedPlotCount - 1) * (GET_PLAYER(getOwnerINLINE()).getCurrentEra())) / 3;

	if (GET_PLAYER(getOwnerINLINE()).AI_isFinancialTrouble())
	{
		iWorkersNeeded *= 3;
		iWorkersNeeded /= 2;
	}

	if (iWorkersNeeded > 0)
	{
		iWorkersNeeded++;
		iWorkersNeeded = std::max(1, iWorkersNeeded / 3);
	}

	int iSpecialistExtra = std::min((getSpecialistPopulation() - totalFreeSpecialists()), iUnimprovedUnworkedPlotCount);
	iSpecialistExtra -= iImprovedUnworkedPlotCount;
	
	iWorkersNeeded += std::max(0, 1 + iSpecialistExtra) / 2;
	
	if (iWorstWorkedPlotValue <= iBestUnworkedPlotValue && iBestUnworkedPlotValue >= iBestPotentialPlotValue)
	{
		iWorkersNeeded /= 2;
	}
	if (angryPopulation(1) > 0)
	{
		iWorkersNeeded++;
		iWorkersNeeded /= 2;
	}
	
	iWorkersNeeded += (iSpecialCount + 1) / 2;
	
	iWorkersNeeded = std::max((iUnimprovedWorkedPlotCount + 1) / 2, iWorkersNeeded);
	
	m_iWorkersNeeded = iWorkersNeeded;
	m_iWorkersHave = iWorkersHave;
}

BuildingTypes CvCityAI::AI_bestAdvancedStartBuilding(int iPass)
{
	int iFocusFlags = 0;
	if (iPass >= 0)
	{
		iFocusFlags |= BUILDINGFOCUS_FOOD;
	}
	if (iPass >= 1)
	{
		iFocusFlags |= BUILDINGFOCUS_PRODUCTION;
	}
	if (iPass >= 2)
	{
		iFocusFlags |= BUILDINGFOCUS_EXPERIENCE;
	}
	if (iPass >= 3)
	{
		iFocusFlags |= (BUILDINGFOCUS_HAPPY | BUILDINGFOCUS_HEALTHY);
	}
	if (iPass >= 4)
	{
		iFocusFlags |= (BUILDINGFOCUS_GOLD | BUILDINGFOCUS_RESEARCH | BUILDINGFOCUS_MAINTENANCE);
		if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
		{
			iFocusFlags |= BUILDINGFOCUS_ESPIONAGE;
		}
	}
			
	return AI_bestBuildingThreshold(iFocusFlags, 0, std::max(0, 20 - iPass * 5));
}

//
//
//
void CvCityAI::read(FDataStreamBase* pStream)
{
	CvCity::read(pStream);

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(&m_iEmphasizeAvoidGrowthCount);
	pStream->Read(&m_iEmphasizeGreatPeopleCount);
	pStream->Read(&m_bAssignWorkDirty);
	pStream->Read(&m_bChooseProductionDirty);

	pStream->Read((int*)&m_routeToCity.eOwner);
	pStream->Read(&m_routeToCity.iID);

	pStream->Read(NUM_YIELD_TYPES, m_aiEmphasizeYieldCount);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiEmphasizeCommerceCount);
	pStream->Read(&m_bForceEmphasizeCulture);
	pStream->Read(NUM_CITY_PLOTS, m_aiBestBuildValue);
	pStream->Read(NUM_CITY_PLOTS, (int*)m_aeBestBuild);
	pStream->Read(GC.getNumEmphasizeInfos(), m_pbEmphasize);
	pStream->Read(NUM_YIELD_TYPES, m_aiSpecialYieldMultiplier);
	pStream->Read(&m_iCachePlayerClosenessTurn);
	pStream->Read(&m_iCachePlayerClosenessDistance);
	pStream->Read(MAX_PLAYERS, m_aiPlayerCloseness);
	pStream->Read(&m_iNeededFloatingDefenders);
	pStream->Read(&m_iNeededFloatingDefendersCacheTurn);
	pStream->Read(&m_iWorkersNeeded);
	pStream->Read(&m_iWorkersHave);
	// K-Mod
	if (uiFlag >= 1)
	{
		FAssert(m_aiConstructionValue.size() == GC.getNumBuildingClassInfos());
		pStream->Read(GC.getNumBuildingClassInfos(), &m_aiConstructionValue[0]);
	}
	if (uiFlag >= 2)
	{
		pStream->Read(&m_iCultureWeight);
	}
	// K-Mod end
}

//
//
//
void CvCityAI::write(FDataStreamBase* pStream)
{
	CvCity::write(pStream);

	uint uiFlag=2;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_iEmphasizeAvoidGrowthCount);
	pStream->Write(m_iEmphasizeGreatPeopleCount);
	pStream->Write(m_bAssignWorkDirty);
	pStream->Write(m_bChooseProductionDirty);

	pStream->Write(m_routeToCity.eOwner);
	pStream->Write(m_routeToCity.iID);

	pStream->Write(NUM_YIELD_TYPES, m_aiEmphasizeYieldCount);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiEmphasizeCommerceCount);
	pStream->Write(m_bForceEmphasizeCulture);
	pStream->Write(NUM_CITY_PLOTS, m_aiBestBuildValue);
	pStream->Write(NUM_CITY_PLOTS, (int*)m_aeBestBuild);
	pStream->Write(GC.getNumEmphasizeInfos(), m_pbEmphasize);
	pStream->Write(NUM_YIELD_TYPES, m_aiSpecialYieldMultiplier);
	pStream->Write(m_iCachePlayerClosenessTurn);
	pStream->Write(m_iCachePlayerClosenessDistance);
	pStream->Write(MAX_PLAYERS, m_aiPlayerCloseness);
	pStream->Write(m_iNeededFloatingDefenders);
	pStream->Write(m_iNeededFloatingDefendersCacheTurn);
	pStream->Write(m_iWorkersNeeded);
	pStream->Write(m_iWorkersHave);
	// K-Mod (note: cache needs to be saved, otherwise players who join mid-turn might go out of sync when the cache is used)
	pStream->Write(GC.getNumBuildingClassInfos(), &m_aiConstructionValue[0]); // uiFlag >= 1
	pStream->Write(m_iCultureWeight); // uiFlag >= 2
	// K-Mod end
}

// K-Mod
void CvCityAI::AI_ClearConstructionValueCache()
{
	m_aiConstructionValue.assign(GC.getNumBuildingClassInfos(), -1);
}

/***
**** K-Mod, 13/sep/10, Karadoc
**** Some AI code which I might use later.
****
***/
/*
void CvCityAI::GetTotalCitizenYields(short* piYields, int iCitizens)
{
	// assume we have iCitizens number of citiziens... how much yield are we getting?
}

int CvCityAI::GetPowerImprovement(int eUnit)
{
	// how much would it help our military to have access to "eUnit"?
}
*/

//Civ4 Reimagined
int CvCityAI::AI_pirateValue()
{
	CvArea* pArea = waterArea();

	if (pArea == NULL)
	{
		return 0;
	}

	UnitTypes eBestUnit = AI_bestUnitAI(UNITAI_PIRATE_SEA);

	if (eBestUnit == NO_UNIT)
	{
		return 0;
	}

	const int piratePower = GC.getUnitInfo(eBestUnit).getPowerValue();
	const CvPlayer& kPlayer = GET_PLAYER(getOwnerINLINE());
	const CvTeam& kTeam = GET_TEAM(getTeam());
	int victims = 0;
	int rivals = 0;

	for (int iI = 0; iI < MAX_PLAYERS; ++iI)
	{
		const CvPlayer& kOther = GET_PLAYER((PlayerTypes)iI);

		if (!kOther.isAlive() || iI == getID() || !kTeam.isHasMet(kOther.getTeam()) || kOther.isBarbarian())
			continue;

		if(!kOther.hasCoastalCitiesByWaterArea(pArea))
			continue;

		rivals++;
		
		const int otherPower = kOther.getBestUnitPower(DOMAIN_SEA);
		if (piratePower > otherPower)
		{
			if (kOther.getTeam() != kPlayer.getTeam() && 
			!GET_TEAM(kOther.getTeam()).isVassal(kPlayer.getTeam()) && 
			!kTeam.isVassal(kOther.getTeam()))
			{
				victims++;
			}
		}
	}

	if (rivals == 0)
	{
		return 0;
	}

	return 100 * victims / rivals;
}