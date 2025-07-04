// player.cpp

#include "CvGameCoreDLL.h"
#include "CvGlobals.h"
#include "CvArea.h"
#include "CvGameAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvRandom.h"
#include "CvTeamAI.h"
#include "CvGameCoreUtils.h"
#include "CvPlayerAI.h"
#include "CvPlayer.h"
#include "CvGameCoreUtils.h"
#include "CvArtFileMgr.h"
#include "CvDiploParameters.h"
#include "CvInitCore.h"
#include "CyArgsList.h"
#include "CvInfos.h"
#include "CvPopupInfo.h"
#include "CvDiploParameters.h"
#include "FProfiler.h"
#include "CvGameTextMgr.h"
#include "CyCity.h"
#include "CyPlot.h"
#include "CyUnit.h"
#include "CvEventReporter.h"
#include "CvMapGenerator.h"

#include "CvDLLInterfaceIFaceBase.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvDLLEngineIFaceBase.h"
#include "CvDLLFAStarIFaceBase.h"
#include "CvDLLPythonIFaceBase.h"
//bbai
#include "CvDLLFlagEntityIFaceBase.h"
#include "BetterBTSAI.h"
//bbai end

// Public Functions...

CvPlayer::CvPlayer()
{
	m_aiSeaPlotYield = new int[NUM_YIELD_TYPES];
	m_aiLakePlotYield = new int[NUM_YIELD_TYPES]; // Civ4 Reimagined
	m_aiPeakYield = new int[NUM_YIELD_TYPES]; // Civ4 Reimagined
	m_aiPeakYieldChangeAdjacentToTerrace = new int[NUM_YIELD_TYPES]; // Civ4 Reimagined
	m_aiSpecialistThresholdExtraYield = new int[NUM_YIELD_TYPES]; // Civ4 Reimagined
	m_aiYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiExtraYield = new int[NUM_YIELD_TYPES]; // Civ4 Reimagined
	m_aiPeakAdjacencyExtraYield = new int[NUM_YIELD_TYPES]; // Civ4 Reimagined
	m_aiCityOnHillsExtraYield = new int[NUM_YIELD_TYPES]; // Civ4 Reimagined
	m_aiTownAdjacencyBonus = new int[NUM_YIELD_TYPES]; // Civ4 Reimagined
	m_aiCapitalExtraYieldFromCityPercent = new int[NUM_YIELD_TYPES]; // Civ4 Reimagined
	m_aiExtraYieldPerReligion = new int[NUM_YIELD_TYPES]; // Civ4 Reimagined
	m_aiCapitalYieldRateModifier = new int[NUM_YIELD_TYPES];
	m_aiExtraYieldThreshold = new int[NUM_YIELD_TYPES];
	m_aiTradeYieldModifier = new int[NUM_YIELD_TYPES];
	m_aiFreeCityCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiCommercePercent = new int[NUM_COMMERCE_TYPES];
	m_aiInitCityCommerce = new int[NUM_COMMERCE_TYPES]; // Civ4 Reimagined
	m_aiUniquePowerCommerceModifier = new int[NUM_COMMERCE_TYPES]; // Civ4 Reimagined
	m_aiCommerceRate = new int[NUM_COMMERCE_TYPES];
	m_aiCommerceRateModifier = new int[NUM_COMMERCE_TYPES];
	m_aiCapitalCommerceRateModifier = new int[NUM_COMMERCE_TYPES];
	m_aiCapitalCommerceRateModifierPerHappinessSurplus = new int[NUM_COMMERCE_TYPES]; // Civ4 Reimagined
	m_aiCommerceHappiness = new int[NUM_COMMERCE_TYPES]; // Civ4 Reimagined
	m_aiStateReligionBuildingCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiStateReligionCommercePerPopulationOverThreshold = new int[NUM_COMMERCE_TYPES]; // Civ4 Reimagined
	m_aiSpecialistExtraCommerce = new int[NUM_COMMERCE_TYPES];
	m_aiGreatPeopleExtraCommerceInCapital = new int[NUM_COMMERCE_TYPES]; // Civ4 Reimagined
	m_aiCommerceFlexibleCount = new int[NUM_COMMERCE_TYPES];
	m_aiGoldPerTurnByPlayer = new int[MAX_PLAYERS];
	m_aiEspionageSpendingWeightAgainstTeam = new int[MAX_TEAMS];
	
	m_aiDomainProductionModifiers = new int[NUM_DOMAIN_TYPES]; // Leoreth
	m_aiDomainExperienceModifiers = new int[NUM_DOMAIN_TYPES]; // Leoreth
	m_aiDomainExperienceGainModifiers = new int[NUM_DOMAIN_TYPES]; // Civ4 Reimagined

	m_aiMilitaryPower = new int[NUM_DOMAIN_TYPES]; // Civ4 Reimagined
	m_aiBestUnitPower = new int[NUM_DOMAIN_TYPES]; // Civ4 Reimagined
	m_aiExtraMovesInGoldenAge = new int[NUM_DOMAIN_TYPES]; // Civ4 Reimagined
	m_aiDomainGreatGeneralRateModifier = new int[NUM_DOMAIN_TYPES]; // Civ4 Reimagined
	m_aiIdeologyInfluence = new int[NUM_IDEOLOGY_TYPES]; // Civ4 Reimagind
	m_aiForeignTradeIdeologyModifier = new int[NUM_IDEOLOGY_TYPES]; // Civ4 Reimagind
	m_aiBonusRatioModifierPerIdeologyCiv = new int[NUM_IDEOLOGY_TYPES]; // Civ4 Reimagind
	m_aiIdeologyCombatExperienceModifier = new int[NUM_IDEOLOGY_TYPES]; // Civ4 Reimagined

	m_abFeatAccomplished = new bool[NUM_FEAT_TYPES];
	m_abOptions = new bool[NUM_PLAYEROPTION_TYPES];
	m_abNativeAmericanBonus = new bool[NUM_NATIVEBONUS_TYPES]; // Civ4 Reimagined

	m_paiBonusExport = NULL;
	m_paiBonusImport = NULL;
	m_paiImprovementCount = NULL;
	m_paiRadiusImprovementHappiness = NULL; // Civ4 Reimagined
	m_paiGreatSpyPointsFromImprovementInRadius = NULL; // Civ4 Reimagined
	m_paiFreeBuildingCount = NULL;
	m_paiExtraBuildingHappiness = NULL;
	m_paiExtraBuildingHealth = NULL;
	m_paiBuildingProductionModifiers = NULL; // Leoreth
	m_paiFeatureHappiness = NULL;
	m_paiUnitClassCount = NULL;
	m_paiUnitClassMaking = NULL;
	m_paiBuildingClassCount = NULL;
	m_paiBuildingClassMaking = NULL;
	m_paiHurryCount = NULL;
	m_paiSpecialBuildingNotRequiredCount = NULL;
	m_paiHasCivicOptionCount = NULL;
	m_paiHasNoAnarchyCivicOptionCount = NULL; // Civ4 Reimagined
	m_paiNoCivicUpkeepCount = NULL;
	m_paiHasReligionCount = NULL;
	m_paiHasCorporationCount = NULL;
	m_paiUpkeepCount = NULL;
	m_paiSpecialistValidCount = NULL;
	m_paiExtraSpecialists = NULL; // Civ4 Reimagined
	m_paiExtraSpecialistExperience = NULL; // Civ4 Reimagined
	m_paiExtraSpecialistHappiness = NULL; // Civ4 Reimagined
	m_paiFatcrossTerrainHappiness = NULL; // Civ4 Reimagined
	m_paiFatcrossTerrainCulture = NULL; // Civ4 Reimagined
	m_paiTerrainMovementCostModifier = NULL; // Civ4 Reimagined
	m_paiCapitalCommercePopulationThreshold = NULL; // Civ4 Reimagined
	m_paiCapitalCommerceModifier = NULL; // Civ4 Reimagined
	m_paiAveragePopCommerceModifier = NULL; // Civ4 Reimagined
	m_paiAveragePopCommerceModifierMaxPop = NULL; // Civ4 Reimagined
	m_paiAveragePopCommerceModifierMaxMod = NULL; // Civ4 Reimagined
	m_paiPlayerExtraAvailableBonuses = NULL; // Civ4 Reimagined
	m_paiUnitClassProductionModifier = NULL; // Civ4 Reimagined
	m_paiReligiousUnitClassProductionModifier = NULL; // Civ4 Reimagined
	m_paiCivicEffect = NULL; // Civ4 Reimagined
	m_paiRouteChange = NULL; // Civ4 Reimagined
	m_paiGreatPeopleRateChangeModifier = NULL; // Civ4 Reimagined

	m_pabResearchingTech = NULL;
	m_pabLoyalMember = NULL;
	m_pabUnlimitedSpecialistsWithTemple = NULL; // Civ4 Reimagined

	m_paeCivics = NULL;
	

	m_ppaaiSpecialistExtraYield = NULL;
	m_ppaaiSpecialistCommerceChanges = NULL; // Civ4 Reimagined
	m_ppaaiBonusHealthFromBuilding = NULL; // Civ4 Reimagined
	m_ppaaiImprovementYieldChange = NULL;
	m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus = NULL;
	m_ppaaiTerrainYieldChange = NULL; // Civ4 Reimagined
	m_ppaaiRadiusImprovementCommerceChange = NULL; // Civ4 Reimagined
	m_ppaaiBuildingYieldChange = NULL; // Civ4 Reimagined
	m_ppaiFeatureCommerce = NULL; // Civ4 Reimagined
	m_ppaiFeatureExtraYield = NULL; // Civ4 Reimagined

	reset(NO_PLAYER, true);
}


CvPlayer::~CvPlayer()
{
	uninit();

	SAFE_DELETE_ARRAY(m_aiSeaPlotYield);
	SAFE_DELETE_ARRAY(m_aiLakePlotYield); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiPeakYield); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiPeakYieldChangeAdjacentToTerrace); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiSpecialistThresholdExtraYield); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiExtraYield); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiPeakAdjacencyExtraYield); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiCityOnHillsExtraYield); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiTownAdjacencyBonus); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiCapitalExtraYieldFromCityPercent); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiExtraYieldPerReligion); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiCapitalYieldRateModifier);
	SAFE_DELETE_ARRAY(m_aiExtraYieldThreshold);
	SAFE_DELETE_ARRAY(m_aiTradeYieldModifier);
	SAFE_DELETE_ARRAY(m_aiFreeCityCommerce);
	SAFE_DELETE_ARRAY(m_aiCommercePercent);
	SAFE_DELETE_ARRAY(m_aiInitCityCommerce); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiUniquePowerCommerceModifier); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiCommerceRate);
	SAFE_DELETE_ARRAY(m_aiCommerceRateModifier);
	SAFE_DELETE_ARRAY(m_aiCapitalCommerceRateModifier);
	SAFE_DELETE_ARRAY(m_aiCapitalCommerceRateModifierPerHappinessSurplus); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiCommerceHappiness); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiStateReligionBuildingCommerce);
	SAFE_DELETE_ARRAY(m_aiStateReligionCommercePerPopulationOverThreshold); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiSpecialistExtraCommerce);
	SAFE_DELETE_ARRAY(m_aiGreatPeopleExtraCommerceInCapital); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiCommerceFlexibleCount);
	SAFE_DELETE_ARRAY(m_aiGoldPerTurnByPlayer);
	SAFE_DELETE_ARRAY(m_aiEspionageSpendingWeightAgainstTeam);
	SAFE_DELETE_ARRAY(m_aiDomainProductionModifiers); // Leoreth
	SAFE_DELETE_ARRAY(m_aiDomainExperienceModifiers); // Leoreth
	SAFE_DELETE_ARRAY(m_aiDomainExperienceGainModifiers); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiMilitaryPower); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiBestUnitPower); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiExtraMovesInGoldenAge); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiDomainGreatGeneralRateModifier); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiIdeologyInfluence); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiForeignTradeIdeologyModifier); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiBonusRatioModifierPerIdeologyCiv); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_aiIdeologyCombatExperienceModifier); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_abFeatAccomplished);
	SAFE_DELETE_ARRAY(m_abOptions);
	SAFE_DELETE_ARRAY(m_abNativeAmericanBonus); // Civ4 Reimagined
}


void CvPlayer::init(PlayerTypes eID)
{
	LeaderHeadTypes eBestPersonality;
	int iValue;
	int ipBestValue;
	int iI, iJ;

	//--------------------------------
	// Init saved data
	reset(eID);

	//--------------------------------
	// Init containers
	m_plotGroups.init();

	m_cities.init();

	m_units.init();

	m_selectionGroups.init();

	m_eventsTriggered.init();

	//--------------------------------
	// Init non-saved data
	setupGraphical();

	//--------------------------------
	// Init other game data
	FAssert(getTeam() != NO_TEAM);
	GET_TEAM(getTeam()).changeNumMembers(1);

	if ((GC.getInitCore().getSlotStatus(getID()) == SS_TAKEN) || (GC.getInitCore().getSlotStatus(getID()) == SS_COMPUTER))
	{
		setAlive(true);

		if (GC.getGameINLINE().isOption(GAMEOPTION_RANDOM_PERSONALITIES))
		{
			if (!isBarbarian() && !isMinorCiv())
			{
				ipBestValue = 0;
				eBestPersonality = NO_LEADER;

				for (iI = 0; iI < GC.getNumLeaderHeadInfos(); iI++)
				{
					if (iI != GC.getDefineINT("BARBARIAN_LEADER")) // XXX minor civ???
					{
						iValue = (1 + GC.getGameINLINE().getSorenRandNum(10000, "Choosing Personality"));

						for (iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
						{
							if (GET_PLAYER((PlayerTypes)iJ).isAlive())
							{
								if (GET_PLAYER((PlayerTypes)iJ).getPersonalityType() == ((LeaderHeadTypes)iI))
								{
									iValue /= 2;
								}
							}
						}

						if (iValue > ipBestValue)
						{
							ipBestValue = iValue;
							eBestPersonality = ((LeaderHeadTypes)iI);
						}
					}
				}

				if (eBestPersonality != NO_LEADER)
				{
					setPersonalityType(eBestPersonality);
				}
			}
		}

		changeBaseFreeUnits(GC.getDefineINT("INITIAL_BASE_FREE_UNITS"));
		changeBaseFreeMilitaryUnits(GC.getDefineINT("INITIAL_BASE_FREE_MILITARY_UNITS"));
		changeFreeUnitsPopulationPercent(GC.getDefineINT("INITIAL_FREE_UNITS_POPULATION_PERCENT"));
		changeFreeMilitaryUnitsPopulationPercent(GC.getDefineINT("INITIAL_FREE_MILITARY_UNITS_POPULATION_PERCENT"));
		changeGoldPerUnit(GC.getDefineINT("INITIAL_GOLD_PER_UNIT"));
		changeTradeRoutes(GC.getDefineINT("INITIAL_TRADE_ROUTES"));
		changeStateReligionHappiness(GC.getDefineINT("INITIAL_STATE_RELIGION_HAPPINESS"));
		changeNonStateReligionHappiness(GC.getDefineINT("INITIAL_NON_STATE_RELIGION_HAPPINESS"));
		changeVassalCommerceGain(GC.getDefineINT("VASSAL_COMMERCE_GAIN_PERCENT")); // Civ4 Reimagined

		for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			changeTradeYieldModifier(((YieldTypes)iI), GC.getYieldInfo((YieldTypes)iI).getTradeModifier());
		}

		for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			setCommercePercent((CommerceTypes)iI, GC.getCommerceInfo((CommerceTypes)iI).getInitialPercent(), true);
		}

		FAssertMsg((GC.getNumTraitInfos() > 0), "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::init");
		for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (hasTrait((TraitTypes)iI))
			{
				changeExtraHealth(GC.getTraitInfo((TraitTypes)iI).getHealth());
				changeExtraHappiness(GC.getTraitInfo((TraitTypes)iI).getHappiness());

				for (iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
				{
					changeExtraBuildingHappiness((BuildingTypes)iJ, GC.getBuildingInfo((BuildingTypes)iJ).getHappinessTraits(iI));
				}

				changeUpkeepModifier(GC.getTraitInfo((TraitTypes)iI).getUpkeepModifier());
				changeLevelExperienceModifier(GC.getTraitInfo((TraitTypes)iI).getLevelExperienceModifier());
				changeGreatPeopleRateModifier(GC.getTraitInfo((TraitTypes)iI).getGreatPeopleRateModifier());
				changeGreatGeneralRateModifier(GC.getTraitInfo((TraitTypes)iI).getGreatGeneralRateModifier());
				changeDomesticGreatGeneralRateModifier(GC.getTraitInfo((TraitTypes)iI).getDomesticGreatGeneralRateModifier());
				changeMaxGlobalBuildingProductionModifier(GC.getTraitInfo((TraitTypes)iI).getMaxGlobalBuildingProductionModifier());
				changeMaxTeamBuildingProductionModifier(GC.getTraitInfo((TraitTypes)iI).getMaxTeamBuildingProductionModifier());
				changeMaxPlayerBuildingProductionModifier(GC.getTraitInfo((TraitTypes)iI).getMaxPlayerBuildingProductionModifier());
				changeBuildingsProductionModifier(GC.getTraitInfo((TraitTypes)iI).getBuildingProductionModifier()); // Civ4 Reimagined
				changeDistanceMaintenanceModifier(GC.getTraitInfo((TraitTypes)iI).getDistanceMaintenanceModifier()); // Civ4 Reimagined
				changeBonusValueModifier(GC.getTraitInfo((TraitTypes)iI).getBonusValueModifier()); // Civ4 Reimagined
				changeVassalCommerceGain(GC.getTraitInfo((TraitTypes)iI).getVassalCommerceGain()); // Civ4 Reimagined
				changeIsNoPillageCount(GC.getTraitInfo((TraitTypes)iI).isNoPillage()); // Civ4 Reimagined
				
				
				for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					changeTradeYieldModifier(((YieldTypes)iJ), GC.getTraitInfo((TraitTypes)iI).getTradeYieldModifier(iJ));
				}

				for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
				{
					changeFreeCityCommerce(((CommerceTypes)iJ), GC.getTraitInfo((TraitTypes)iI).getCommerceChange(iJ));
					changeCommerceRateModifier(((CommerceTypes)iJ), GC.getTraitInfo((TraitTypes)iI).getCommerceModifier(iJ));
					changeInitCityCommerce(((CommerceTypes)iJ), GC.getTraitInfo((TraitTypes)iI).getFreeCommerce(iJ)); //Civ4 Reimagined
				}

				for (iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
				{
					if (GC.getCivicOptionInfo((CivicOptionTypes) iJ).getTraitNoUpkeep(iI))
					{
						changeNoCivicUpkeepCount(((CivicOptionTypes)iJ), 1);
					}
				}
			}
		}

		updateMaxAnarchyTurns();

		for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			updateExtraYieldThreshold((YieldTypes)iI);
		}

		for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			setCivics(((CivicOptionTypes)iI), ((CivicTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationInitialCivics(iI))));
		}

		for (iI = 0; iI < GC.getNumEventInfos(); iI++)
		{
			resetEventOccured((EventTypes)iI, false);
		}

		for (iI = 0; iI < GC.getNumEventTriggerInfos(); iI++)
		{
			resetTriggerFired((EventTriggerTypes)iI);
		}

		for (iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
		{
			UnitTypes eUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

			if (NO_UNIT != eUnit)
			{
				if (GC.getUnitInfo(eUnit).isFound())
				{
					setUnitExtraCost((UnitClassTypes)iI, getNewCityProductionValue());
				}
			}
		}
	}

	// Civ4 Reimagined
	if (isAlive())
	{
		updateUniquePowers((EraTypes)0);
	}

	// Civ4 Reimagined
	setupEurekas();

	AI_init();
}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      12/30/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
//
// Copy of CvPlayer::init but with modifications for use in the middle of a game
//
void CvPlayer::initInGame(PlayerTypes eID)
{
	LeaderHeadTypes eBestPersonality;
	int iValue;
	int ipBestValue;
	int iI, iJ;

	//--------------------------------
	// Init saved data
	reset(eID);

	//--------------------------------
	// Init containers
	m_plotGroups.init();

	m_cities.init();

	m_units.init();

	m_selectionGroups.init();

	m_eventsTriggered.init();

	//--------------------------------
	// Init non-saved data
	setupGraphical();

	//--------------------------------
	// Init other game data
	FAssert(getTeam() != NO_TEAM);

	// Some effects on team necessary if this is the only member of the team
	int iOtherTeamMembers = 0;
	for (iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if( iI != getID() )
		{
			if( GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam() )
			{
				iOtherTeamMembers++;
			}
		}
	}

	bool bTeamInit = false;
	if( (iOtherTeamMembers == 0) || GET_TEAM(getTeam()).getNumMembers() == 0 )
	{
		bTeamInit = true;
		GET_TEAM(getTeam()).init(getTeam());
		GET_TEAM(getTeam()).resetPlotAndCityData();
	}

	if( bTeamInit || (GET_TEAM(getTeam()).getNumMembers() == iOtherTeamMembers) )
	{
		GET_TEAM(getTeam()).changeNumMembers(1);
	}

	if ((GC.getInitCore().getSlotStatus(getID()) == SS_TAKEN) || (GC.getInitCore().getSlotStatus(getID()) == SS_COMPUTER))
	{
		setAlive(true);

		if (GC.getGameINLINE().isOption(GAMEOPTION_RANDOM_PERSONALITIES))
		{
			if (!isBarbarian() && !isMinorCiv())
			{
				ipBestValue = 0;
				eBestPersonality = NO_LEADER;

				for (iI = 0; iI < GC.getNumLeaderHeadInfos(); iI++)
				{
					if (iI != GC.getDefineINT("BARBARIAN_LEADER")) // XXX minor civ???
					{
						iValue = (1 + GC.getGameINLINE().getSorenRandNum(10000, "Choosing Personality"));

						for (iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
						{
							if (GET_PLAYER((PlayerTypes)iJ).isAlive())
							{
								if (GET_PLAYER((PlayerTypes)iJ).getPersonalityType() == ((LeaderHeadTypes)iI))
								{
									iValue /= 2;
								}
							}
						}

						if (iValue > ipBestValue)
						{
							ipBestValue = iValue;
							eBestPersonality = ((LeaderHeadTypes)iI);
						}
					}
				}

				if (eBestPersonality != NO_LEADER)
				{
					setPersonalityType(eBestPersonality);
				}
			}
		}

		changeBaseFreeUnits(GC.getDefineINT("INITIAL_BASE_FREE_UNITS"));
		changeBaseFreeMilitaryUnits(GC.getDefineINT("INITIAL_BASE_FREE_MILITARY_UNITS"));
		changeFreeUnitsPopulationPercent(GC.getDefineINT("INITIAL_FREE_UNITS_POPULATION_PERCENT"));
		changeFreeMilitaryUnitsPopulationPercent(GC.getDefineINT("INITIAL_FREE_MILITARY_UNITS_POPULATION_PERCENT"));
		changeGoldPerUnit(GC.getDefineINT("INITIAL_GOLD_PER_UNIT"));
		changeTradeRoutes(GC.getDefineINT("INITIAL_TRADE_ROUTES"));
		changeStateReligionHappiness(GC.getDefineINT("INITIAL_STATE_RELIGION_HAPPINESS"));
		changeNonStateReligionHappiness(GC.getDefineINT("INITIAL_NON_STATE_RELIGION_HAPPINESS"));
		changeVassalCommerceGain(GC.getDefineINT("VASSAL_COMMERCE_GAIN_PERCENT")); // Civ4 Reimagined

		for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			changeTradeYieldModifier(((YieldTypes)iI), GC.getYieldInfo((YieldTypes)iI).getTradeModifier());
		}

		for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			setCommercePercent((CommerceTypes)iI, GC.getCommerceInfo((CommerceTypes)iI).getInitialPercent(), true);
		}

		FAssertMsg((GC.getNumTraitInfos() > 0), "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::init");
		for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
		{
			if (hasTrait((TraitTypes)iI))
			{
				changeExtraHealth(GC.getTraitInfo((TraitTypes)iI).getHealth());
				changeExtraHappiness(GC.getTraitInfo((TraitTypes)iI).getHappiness());

				for (iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
				{
					changeExtraBuildingHappiness((BuildingTypes)iJ, GC.getBuildingInfo((BuildingTypes)iJ).getHappinessTraits(iI));
				}

				changeUpkeepModifier(GC.getTraitInfo((TraitTypes)iI).getUpkeepModifier());
				changeLevelExperienceModifier(GC.getTraitInfo((TraitTypes)iI).getLevelExperienceModifier());
				changeGreatPeopleRateModifier(GC.getTraitInfo((TraitTypes)iI).getGreatPeopleRateModifier());
				changeGreatGeneralRateModifier(GC.getTraitInfo((TraitTypes)iI).getGreatGeneralRateModifier());
				changeDomesticGreatGeneralRateModifier(GC.getTraitInfo((TraitTypes)iI).getDomesticGreatGeneralRateModifier());

				changeMaxGlobalBuildingProductionModifier(GC.getTraitInfo((TraitTypes)iI).getMaxGlobalBuildingProductionModifier());
				changeMaxTeamBuildingProductionModifier(GC.getTraitInfo((TraitTypes)iI).getMaxTeamBuildingProductionModifier());
				changeMaxPlayerBuildingProductionModifier(GC.getTraitInfo((TraitTypes)iI).getMaxPlayerBuildingProductionModifier());
				changeBuildingsProductionModifier(GC.getTraitInfo((TraitTypes)iI).getBuildingProductionModifier()); // Civ4 Reimagined
				changeDistanceMaintenanceModifier(GC.getTraitInfo((TraitTypes)iI).getDistanceMaintenanceModifier()); // Civ4 Reimagined
				changeBonusValueModifier(GC.getTraitInfo((TraitTypes)iI).getBonusValueModifier()); // Civ4 Reimagined
				changeVassalCommerceGain(GC.getTraitInfo((TraitTypes)iI).getVassalCommerceGain()); // Civ4 Reimagined
				changeIsNoPillageCount(GC.getTraitInfo((TraitTypes)iI).isNoPillage()); // Civ4 Reimagined

				for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
				{
					changeTradeYieldModifier(((YieldTypes)iJ), GC.getTraitInfo((TraitTypes)iI).getTradeYieldModifier(iJ));
				}

				for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
				{
					changeFreeCityCommerce(((CommerceTypes)iJ), GC.getTraitInfo((TraitTypes)iI).getCommerceChange(iJ));
					changeCommerceRateModifier(((CommerceTypes)iJ), GC.getTraitInfo((TraitTypes)iI).getCommerceModifier(iJ));
					changeInitCityCommerce(((CommerceTypes)iJ), GC.getTraitInfo((TraitTypes)iI).getFreeCommerce(iJ)); //Civ4 Reimagined
				}

				for (iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
				{
					if (GC.getCivicOptionInfo((CivicOptionTypes) iJ).getTraitNoUpkeep(iI))
					{
						changeNoCivicUpkeepCount(((CivicOptionTypes)iJ), 1);
					}
				}
			}
		}

		updateMaxAnarchyTurns();

		for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
		{
			updateExtraYieldThreshold((YieldTypes)iI);
		}

		for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			setCivics(((CivicOptionTypes)iI), ((CivicTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationInitialCivics(iI))));
		}

		// Reset all triggers at first, set those whose events have fired in next block
		for (iI = 0; iI < GC.getNumEventTriggerInfos(); iI++)
		{
			resetTriggerFired((EventTriggerTypes)iI);
		}

		for (iI = 0; iI < GC.getNumEventInfos(); iI++)
		{
/* original bts code
			resetEventOccured((EventTypes)iI, false);
*/
			// Has global trigger fired already?
				
			const EventTriggeredData* pEvent = NULL;
			for (iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
			{
				if( iJ != getID() )
				{
					pEvent = GET_PLAYER((PlayerTypes)iJ).getEventOccured((EventTypes)iI);
					if ( pEvent != NULL )
					{
						CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(pEvent->m_eTrigger);
						if( kTrigger.isGlobal() )
						{
							setTriggerFired( *pEvent, false, false );
							break;
						}
						else if( kTrigger.isTeam() && GET_PLAYER((PlayerTypes)iJ).getTeam() == getTeam() )
						{
							setTriggerFired( *pEvent, false, false );
							break;
						}
					}
				}
			}
			
			resetEventOccured((EventTypes)iI, false);
		}

		for (iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
		{
			UnitTypes eUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

			if (NO_UNIT != eUnit)
			{
				if (GC.getUnitInfo(eUnit).isFound())
				{
					setUnitExtraCost((UnitClassTypes)iI, getNewCityProductionValue());
				}
			}
		}
	}

	resetPlotAndCityData();

	// Civ4 Reimagined
	setupEurekas();

	AI_init();
}

// Civ4 Reimagined
void CvPlayer::setupEurekas()
{
	logBBAI("Setup eurekas for %d", getID());

	if (!isHuman() || isHumanDisabled())
	{
		for (int iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (!CvWString(GC.getTechInfo((TechTypes)iI).getHelp()).empty() && (TechTypes)iI != (TechTypes)GC.getInfoTypeForString("TECH_ECOLOGY"))
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)iI, getID(), true);
			}
		}
	}
}

//
// Reset all data for this player stored in plot and city objects
//
void CvPlayer::resetPlotAndCityData( )
{
	CvPlot* pLoopPlot;
	CvCity* pLoopCity;
	for (int iPlot = 0; iPlot < GC.getMapINLINE().numPlotsINLINE(); ++iPlot)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iPlot);

		pLoopPlot->setCulture(getID(), 0, false, false);
		pLoopPlot->setFoundValue(getID(), 0);

		pLoopCity = pLoopPlot->getPlotCity();
		if( pLoopCity != NULL )
		{
			pLoopCity->setCulture(getID(), 0, false, false);
			pLoopCity->changeNumRevolts(getID(), -pLoopCity->getNumRevolts(getID()));
			pLoopCity->setEverOwned(getID(), false);
			pLoopCity->setTradeRoute(getID(), false);
		}
	}
}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/


void CvPlayer::uninit()
{
	SAFE_DELETE_ARRAY(m_paiBonusExport);
	SAFE_DELETE_ARRAY(m_paiBonusImport);
	SAFE_DELETE_ARRAY(m_paiImprovementCount);
	SAFE_DELETE_ARRAY(m_paiRadiusImprovementHappiness); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiGreatSpyPointsFromImprovementInRadius); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiFreeBuildingCount);
	SAFE_DELETE_ARRAY(m_paiExtraBuildingHappiness);
	SAFE_DELETE_ARRAY(m_paiExtraBuildingHealth);
	SAFE_DELETE_ARRAY(m_paiBuildingProductionModifiers); // Leoreth
	SAFE_DELETE_ARRAY(m_paiUnitClassProductionModifier); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiReligiousUnitClassProductionModifier); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiFeatureHappiness);
	SAFE_DELETE_ARRAY(m_paiUnitClassCount);
	SAFE_DELETE_ARRAY(m_paiUnitClassMaking);
	SAFE_DELETE_ARRAY(m_paiBuildingClassCount);
	SAFE_DELETE_ARRAY(m_paiBuildingClassMaking);
	SAFE_DELETE_ARRAY(m_paiHurryCount);
	SAFE_DELETE_ARRAY(m_paiSpecialBuildingNotRequiredCount);
	SAFE_DELETE_ARRAY(m_paiHasCivicOptionCount);
	SAFE_DELETE_ARRAY(m_paiHasNoAnarchyCivicOptionCount); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiNoCivicUpkeepCount);
	SAFE_DELETE_ARRAY(m_paiHasReligionCount);
	SAFE_DELETE_ARRAY(m_paiHasCorporationCount);
	SAFE_DELETE_ARRAY(m_paiUpkeepCount);
	SAFE_DELETE_ARRAY(m_paiSpecialistValidCount);
	SAFE_DELETE_ARRAY(m_paiExtraSpecialists); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiExtraSpecialistExperience); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiExtraSpecialistHappiness); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiFatcrossTerrainHappiness); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiFatcrossTerrainCulture); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiTerrainMovementCostModifier); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiCapitalCommercePopulationThreshold); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiCapitalCommerceModifier); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiAveragePopCommerceModifier); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiAveragePopCommerceModifierMaxPop); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiAveragePopCommerceModifierMaxMod); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiPlayerExtraAvailableBonuses); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiCivicEffect); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiRouteChange); // Civ4 Reimagined
	SAFE_DELETE_ARRAY(m_paiGreatPeopleRateChangeModifier); // Civ4 Reimagined

	SAFE_DELETE_ARRAY(m_pabResearchingTech);
	SAFE_DELETE_ARRAY(m_pabLoyalMember);
	SAFE_DELETE_ARRAY(m_pabUnlimitedSpecialistsWithTemple); // Civ4 Reimagined

	SAFE_DELETE_ARRAY(m_paeCivics);
	


	m_triggersFired.clear();

	if (m_ppaaiSpecialistExtraYield != NULL)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiSpecialistExtraYield[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiSpecialistExtraYield);
	}

	// Civ4 Reimagined
	if (m_ppaaiSpecialistCommerceChanges != NULL)
	{
		for (int iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiSpecialistCommerceChanges[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiSpecialistCommerceChanges);
	}

	// Civ4 Reimagined
	if (m_ppaaiBonusHealthFromBuilding != NULL)
	{
		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiBonusHealthFromBuilding[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiBonusHealthFromBuilding);
	}

	if (m_ppaaiImprovementYieldChange != NULL)
	{
		for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiImprovementYieldChange[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiImprovementYieldChange);
	}

	// Civ4 Reimagined
	if (m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus != NULL)
	{
		for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus);
	}

	// Civ4 Reimagined
	if (m_ppaaiTerrainYieldChange != NULL)
	{
		for (int iI = 0; iI < GC.getNumTerrainInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiTerrainYieldChange[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiTerrainYieldChange);
	}
	
	// Civ4 Reimagined
	if (m_ppaaiRadiusImprovementCommerceChange != NULL)
	{
		for (int iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiRadiusImprovementCommerceChange[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiRadiusImprovementCommerceChange);
	}
	
	// Civ4 Reimagined
	if (m_ppaaiBuildingYieldChange != NULL)
	{
		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaaiBuildingYieldChange[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaaiBuildingYieldChange);
	}

	// Civ4 Reimagined
	if (m_ppaiFeatureCommerce != NULL)
	{
		for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaiFeatureCommerce[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaiFeatureCommerce);
	}

	// Civ4 Reimagined
	if (m_ppaiFeatureExtraYield != NULL)
	{
		for (int iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			SAFE_DELETE_ARRAY(m_ppaiFeatureExtraYield[iI]);
		}
		SAFE_DELETE_ARRAY(m_ppaiFeatureExtraYield);
	}

	m_groupCycle.clear();

	m_researchQueue.clear();

	m_cityNames.clear();


	m_plotGroups.uninit();

	m_cities.uninit();

	m_units.uninit();

	m_selectionGroups.uninit();

	m_eventsTriggered.uninit();

	clearMessages();

	clearPopups();

	clearDiplomacy();
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvPlayer::reset(PlayerTypes eID, bool bConstructorCall)
{
	int iI, iJ;

	//--------------------------------
	// Uninit class
	uninit();

	m_iStartingX = INVALID_PLOT_COORD;
	m_iStartingY = INVALID_PLOT_COORD;
	m_iTotalPopulation = 0;
	m_iTotalLand = 0;
	m_iTotalLandScored = 0;
	m_iGold = 0;
	m_iGoldPerTurn = 0;
	m_iAdvancedStartPoints = -1;
	m_iGoldenAgeTurns = 0;
	m_iNumUnitGoldenAges = 0;
	m_iStrikeTurns = 0;
	m_iAnarchyTurns = 0;
	m_iMaxAnarchyTurns = 0;
	m_iAnarchyModifier = 0;
	m_iGoldenAgeModifier = 0;
	m_iGlobalHurryModifier = 0;
	m_iGreatPeopleCreated = 0;
	m_iGreatGeneralsCreated = 0;
	m_iGreatPeopleThresholdModifier = 0;
	m_iGreatGeneralsThresholdModifier = 0;
	m_iGreatPeopleRateModifier = 0;
	m_iNoGreatPeopleCount = 0; // Civ4 Reimagined
	m_iNoCultureFlipCount = 0; // Civ4 Reimagined
	m_iNoCapitalCount = 0; // Civ4 Reimagined
	m_iGreatGeneralRateModifier = 0;
	m_iDomesticGreatGeneralRateModifier = 0;
	m_iStateReligionGreatPeopleRateModifier = 0;
	m_iMaxGlobalBuildingProductionModifier = 0;
	m_iMaxTeamBuildingProductionModifier = 0;
	m_iMaxPlayerBuildingProductionModifier = 0;
	m_iBuildingsProductionModifier = 0; // Civ4 Reimagined
	m_iFreeExperience = 0;
	m_iFeatureProductionModifier = 0;
	m_iWorkerSpeedModifier = 0;
	m_iImprovementUpgradeRateModifier = 0;
	m_iMilitaryProductionModifier = 0;
	m_iSpaceProductionModifier = 0;
	m_iCityDefenseModifier = 0;
	m_iNumNukeUnits = 0; 
	m_iNumOutsideUnits = 0; 
	m_iFreeWorkers = 0; // Civ4 Reimagined
	m_iBaseFreeUnits = 0;
	m_iBaseFreeMilitaryUnits = 0;
	m_iFreeUnitsPopulationPercent = 0;
	m_iFreeMilitaryUnitsPopulationPercent = 0;
	m_iGoldPerUnit = 0;
	m_iGoldPerMilitaryUnit = 0;
	m_iExtraUnitCost = 0;
	m_iNumMilitaryUnits = 0;
	m_iNoUnitSupplyCount = 0; // Civ4 Reimagined
	m_iHappyPerMilitaryUnit = 0;
	m_iMilitaryHappinessLimit = 0; //Leoreth
	m_iMilitaryFoodProductionCount = 0;
	m_iMeleeMilitaryFoodProductionCount = 0; // Civ4 Reimagind
	m_iEnableSlavesCount = 0; // Civ4 Reimagined
	m_iSlavesCount = 0; // Civ4 Reimagined
	m_iSlavePoints = 0; // Civ4 Reimagined
	m_iSlaveThreshold = 0; // Civ4 Reimagined
	m_iSlavePointsFromSacrificePopulation = 0; // Civ4 Reimagined
	//m_iNumColonies = 0; // Civ4 Reimagined
	m_iNoMilitaryProductionMaliCount = 0; // Civ4 Reimagined
	m_iConscriptCount = 0;
	m_iMaxConscript = 0;
	m_iHighestUnitLevel = 1;
	m_iHighestNavalUnitLevel = 1; // Civ4 Reimagined
	m_iLootingModifier = 0; // Civ4 Reimagined
	m_iBarbarianGreatGeneralCount = 0; // Civ4 Reimagined
	m_iUnlimitedBarbXPCount = 0; // Civ4 Reimagined
	m_iNoWarAgainstSameFaithUnhappinessCount = 0; // Civ4 Reimagined
	m_iUnlimitedAnimalXPCount = 0; // Civ4 Reimagined
	m_iOverflowResearch = 0;
	//m_iNoUnhealthyPopulationCount = 0;
	m_iUnhealthyPopulationModifier = 0; // K-Mod
	m_iExpInBorderModifier = 0;
	m_iBuildingOnlyHealthyCount = 0;
	m_iDistanceMaintenanceModifier = 0;
	m_iBonusValueModifier = 0; // Civ4 Reimagined
	m_iVassalCommerceGain = 0; // Civ4 Reimagined
	m_iIsNoPillageCount = 0; // Civ4 Reimagined
	m_iNumCitiesMaintenanceModifier = 0;
	m_iColonyMaintenanceModifier = 0; // Civ4 Reimagined
	m_iNoCapitalCount = 0; // Civ4 Reimagined
	m_iCorporationMaintenanceModifier = 0;
	m_iProcessModifier = 0; //Leoreth
	m_iTotalMaintenance = 0;
	m_iUpkeepModifier = 0;
	m_iLevelExperienceModifier = 0;
	m_iExtraHealth = 0;
	m_iPollutionModifier = 0; //Leoreth
	m_iPopulationUnhappinessModifier = 0; //Civ4 Reimagined
	m_iBuildingGoodHealth = 0;
	m_iBuildingBadHealth = 0;
	m_iExtraHappiness = 0;
	m_iCivicHappiness = 0; // Civ4 Reimagined
	m_iBuildingHappiness = 0;
	m_iLargestCityHappiness = 0;
	m_iGoldPerHappinessBonus = 0; // Civ4 Reimagined
	m_iWarWearinessPercentAnger = 0;
	m_iWarWearinessModifier = 0;
	m_iGwPercentAnger = 0; // K-Mod
	m_iFreeSpecialist = 0;
	m_iCoreFreeSpecialist = 0; //Leoreth
	m_iNoForeignTradeCount = 0;
	m_iNoCorporationsCount = 0;
	m_iNoForeignCorporationsCount = 0;
	m_iCoastalTradeRoutes = 0;
	m_iTradeRoutes = 0;
	m_iRevolutionTimer = 0;
	m_iConversionTimer = 0;
	m_iStateReligionCount = 0;
	m_iNoNonStateReligionSpreadCount = 0;
	m_iStateReligionHappiness = 0;
	m_iNonStateReligionHappiness = 0;
	m_iStateReligionUnitProductionModifier = 0;
	m_iVoteSourceStateReligionUnitProductionModifier = 0; // Civ4 Reimagined
	m_iStateReligionBuildingProductionModifier = 0;
	m_iStateReligionFreeExperience = 0;
	m_iSpecialistExtraYieldBaseThreshold = 0; //Leoreth
	m_iSpecialistExtraYieldEraThreshold = 0; //Leoreth
	m_iCapitalCityID = FFreeList::INVALID_INDEX;
	m_iCitiesLost = 0;
	m_iWinsVsBarbs = 0;
	m_iAssets = 0;
	m_iPower = 0;
	m_iPopulationScore = 0;
	m_iLandScore = 0;
	m_iTechScore = 0;
	m_iWondersScore = 0;
	m_iCombatExperience = 0;
	m_iPopRushHurryCount = 0;
	m_iInflationModifier = 0;
	m_iInflationRate = 0; // K-Mod
	m_uiStartTime = 0;

	m_bAlive = false;
	m_bEverAlive = false;
	m_bTurnActive = false;
	m_bAutoMoves = false;
	m_bEndTurn = false;
	m_bPbemNewTurn = false;
	m_bExtendedGame = false;
	m_bFoundedFirstCity = false;
	m_bStrike = false;
	m_bDisableHuman = false; // bbai
	m_iNoCapitalUnhappinessCount = 0; // Civ4 Reimagined
	m_iChoosingFreeTechCount = 0; // K-Mod
	m_iDomesticTradeModifier = 0; // Civ4 Reimagined
	m_iColonyTradeModifier = 0; // Civ4 Reimagined
	m_iCapitalTradeModifier = 0; // Civ4 Reimagined
	m_iUnitUpgradeCostModifier = 0; // Civ4 Reimagined
	m_iForeignUnitSupply = 100; // Civ4 Reimagined
	m_iPillageGainModifier = 0; // Civ4 Reimagined
	m_iTechValue = 0; // Civ4 Reimagined
	m_iBonusRatio = 100; // Civ4 Reimagined
	m_iResearchPerCulture = 0; // Civ4 Reimagined
	m_iFreePopulationInCapital = 0; // Civ4 Reimagined
	m_iFreeCivicEnabled = NO_CIVIC; // Civ4 Reimagined
	m_iLegacyCivic = NO_CIVIC; // Civ4 Reimagined
	m_iEarlyScientistBonusCommerce = 0; // Civ4 Reimagined
	m_iEarlyPriestExtraFood = 0; // Civ4 Reimagined
	m_iWorldWonderProductionModifier = 0; // Civ4 Reimagined
	m_iProductionNearRiver = 0; // Civ4 Reimagined
	m_iProductionPerPopulation = 0; // Civ4 Reimagined
	m_iHurryWithGreatPriestsRatio = 0; // Civ4 Reimagined
	m_iCoastalTradeRouteModifier = 0; // Civ4 Reimagined
	m_iOverseaTradeRouteModifier = 0; // Civ4 Reimagined
	m_iEspionagePointsOnConquestPerPopulation = 0; // Civ4 Reimagined
	m_iUniquePowerGreatPeopleModifier = 0; // Civ4 Reimagined
	m_iUniqueUnitFreeExperience = 0; // Civ4 Reimagined
	m_iReligionTechModifier = 0; // Civ4 Reimagined
	m_iFreeUnitsOnConquest = 0; // Civ4 Reimagined
	m_bImmigrants = false; // Civ4 Reimagined
	m_bCaptureSlaves = false; // Civ4 Reimagined
	m_bDesertGold = false; // Civ4 Reimagined
	m_iUniquePowerBuildingModifier = 0; // Civ4 Reimagined
	m_iEarlyWorkerSpeedModifier = 0; // Civ4 Reimagined
	m_iFatcrossPeakHappiness = 0; // Civ4 Reimagined
	m_iFatcrossPeakCulture = 0; // Civ4 Reimagined
	m_iNonStateReligionHappinessWithStateReligion = 0; // Civ4 Reimagined
	m_bExploreRivalSea = false; // Civ4 Reimagined
	m_bEnableFinancial = false; // Civ4 Reimagined
	m_iMercenaryCostModifier = 0; // Civ4 Reimagined
	m_iBuyBuildingCostModifier = 0; // Civ4 Reimagined
	m_iCanFarmHillsCount = 0; // Civ4 Reimagined
	m_bSpecialTradeRoutePerPlayer = false; // Civ4 Reimagined
	m_bExtraAvailableBonuses = false; // Civ4 Reimagined
	m_iCulturePerPopulationSacrified = 0; // Civ4 Reimagined
	m_bCanExploreSea = false; // Civ4 Reimagined
	m_iNoForeignCultureUnhappinessCount = 0; // Civ4 Reimagined
	m_iNoCityResistanceCount = 0; // Civ4 Reimagined
	m_iProductionPerSurplusHappiness = 0; // Civ4 Reimagined
	m_bUpdateBonusRatio = true; // Civ4 Reimagined
	m_bCivicEffect = false; // Civ4 Reimagined
	m_bFaithConquest = false; // Civ4 Reimagined
	m_iLiberatedColonyTradeRouteModifier = 0; // Civ4 Reimagined
	m_iCorporationTraderouteModifier = 0; // Civ4 Reimagined
	m_iGreatGeneralGoldenAgeLength = 0; // Civ4 Reimagined
	m_bConscriptInfidels = false; // Civ4 Reimagined
	m_iCatchUpTechModifier = 0; // Civ4 Reimagined
	m_iReligiousColonyMaintenanceModifier = 0; // Civ4 Reimagined
	m_iGreatMerchantPointsPerTrade = 0; // Civ4 Reimagined
	m_iGreatPeoplePointsPerTrade = 0; // Civ4 Reimagined
	m_iCapitalCultureAttitudeBonus = 0; // Civ4 Reimagined
	m_iAdditionalAncientEurekaBoost = 0; // Civ4 Reimagined
	m_iAdditionalFarmBonusYield = 0; // Civ4 Reimagined
	m_iAdditionalMineBonusCommerce = 0; // Civ4 Reimagined
	m_iPillageHeal = 0; // Civ4 Reimagined
	m_iGreatPeopleRatePerReligionModifier = 0; // Civ4 Reimagined
	m_eIdeology = IDEOLOGY_CONSERVATISM; // Civ4 Reimagined
	m_eUniquePowerBuilding = NO_BUILDING; // Civ4 Reimagined
	m_eUniquePowerPromotion = NO_PROMOTION; // Civ4 Reimagined
	m_bAlwaysFreshWater = false; // Civ4 Reimagined
	m_bCanRemoveFeatures = false; // Civ4 Reimagined
	m_bCityRevoltOnKill = false; // Civ4 Reimagined
	m_bNoReligionRemoval = false; // Civ4 Reimagined
	m_bCoastalRaid = false; // Civ4 Reimagined
	m_iTradeGoldModifierPerForeignResource = 0; // Civ4 Reimagined
	m_bGainGreatWorkGoldWithHitBonuses = false; // Civ4 Reimagined
	m_iReligiousVoteModifier = 0; // Civ4 Reimagined
	m_bCapitalAlwaysPerfectBonusValue = false; // Civ4 Reimagined
	m_bCanBuildWindmillsOnCoast = false; // Civ4 Reimagined
	m_iGreatEngineerPointsFromCathedrals = 0; // Civ4 Reimagined
	m_iCombatBonusOnHomeArea = 0; // Civ4 Reimagined
	m_iStrategicBonusYieldModifier = 0; // Civ4 Reimagined
	m_iBuildingProductionModifierFromCapital = 0; // Civ4 Reimagined
	m_iCultureResistanceModifier = 0; // Civ4 Reimagined
	m_iFreshWaterHealthModifier = 0; // Civ4 Reimagined
	m_iStateReligionShrineModifier = 0; // Civ4 Reimagined
	m_iLootCityModifier = 0; // Civ4 Reimagined
	m_iTechBulbModifier = 0; // Civ4 Reimagined
	m_bCityImprovesBonus = false; // Civ4 Reimagined
	m_bFreePillage = false; // Civ4 Reimagined
	m_bPirateGold = false; // Civ4 Reimagined
	m_bIsIgnoreEarlyWonderHurryCostModifier = false; // Civ4 Reimagined
	m_bIsIgnoreForeignTradeBan = false; // Civ4 Reimagined
	m_bAlwaysReceiveGreatPeopleLateTechs = false; // Civ4 Reimagined
	m_bCanGoldenAgeWithSamePeople = false; // Civ4 Reimagined
	m_bLegacyCivic = false; // Civ4 Reimagined
	m_bFrenchRevolution = false; // Civ4 Reimagined
	
	m_eID = eID;
	updateTeamType();
	updateHuman();

	if (m_eID != NO_PLAYER)
	{
		m_ePersonalityType = GC.getInitCore().getLeader(m_eID); //??? Is this repeated data???
	}
	else
	{
		m_ePersonalityType = NO_LEADER;
	}
	m_eCurrentEra = ((EraTypes)0);  //??? Is this repeated data???
	m_eLastStateReligion = NO_RELIGION;
	m_eParent = NO_PLAYER;

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		m_aiSeaPlotYield[iI] = 0;
		m_aiLakePlotYield[iI] = 0; // Civ4 Reimagined
		m_aiPeakYield[iI] = 0; // Civ4 Reimagined
		m_aiPeakYieldChangeAdjacentToTerrace[iI] = 0; // Civ4 Reimagined
		m_aiSpecialistThresholdExtraYield[iI] = 0; // Civ4 Reimagined
		m_aiYieldRateModifier[iI] = 0;
		m_aiExtraYield[iI] = 0; // Civ4 Reimagined
		m_aiPeakAdjacencyExtraYield[iI] = 0; // Civ4 Reimagined
		m_aiCityOnHillsExtraYield[iI] = 0; // Civ4 Reimagined
		m_aiTownAdjacencyBonus[iI] = 0; // Civ4 Reimagined
		m_aiCapitalExtraYieldFromCityPercent[iI] = 0; // Civ4 Reimagined
		m_aiExtraYieldPerReligion[iI] = 0; // Civ4 Reimagined
		m_aiCapitalYieldRateModifier[iI] = 0;
		m_aiExtraYieldThreshold[iI] = 0;
		m_aiTradeYieldModifier[iI] = 0;
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		m_aiFreeCityCommerce[iI] = 0;
		m_aiCommercePercent[iI] = 0;
		m_aiCommerceRate[iI] = 0;
		m_aiCommerceRateModifier[iI] = 0;
		m_aiCapitalCommerceRateModifier[iI] = 0;
		m_aiCapitalCommerceRateModifierPerHappinessSurplus[iI] = 0; // Civ4 Reimagined
		m_aiCommerceHappiness[iI] = 0; // Civ4 Reimagined
		m_aiStateReligionBuildingCommerce[iI] = 0;
		m_aiStateReligionCommercePerPopulationOverThreshold[iI] = 0; // Civ4 Reimagined
		m_aiSpecialistExtraCommerce[iI] = 0;
		m_aiGreatPeopleExtraCommerceInCapital[iI] = 0; // Civ4 Reimagined
		m_aiCommerceFlexibleCount[iI] = 0;
		m_aiInitCityCommerce[iI] = 0; // Civ4 Reimagined
		m_aiUniquePowerCommerceModifier[iI] = 0; // Civ4 Reimagined
	}
	
	// Leoreth
	for (iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		m_aiDomainProductionModifiers[iI] = 0;
		m_aiDomainExperienceModifiers[iI] = 0;
		m_aiDomainExperienceGainModifiers[iI] = 0; // Civ4 Reimagined
		m_aiMilitaryPower[iI] = 0; // Civ4 Reimagined
		m_aiBestUnitPower[iI] = 0; // Civ4 Reimagined
		m_aiExtraMovesInGoldenAge[iI] = 0; // Civ4 Reimagined
		m_aiDomainGreatGeneralRateModifier[iI] = 0; // Civ4 Reimagined
	}
	
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiGoldPerTurnByPlayer[iI] = 0;
		if (!bConstructorCall && getID() != NO_PLAYER)
		{
			GET_PLAYER((PlayerTypes) iI).m_aiGoldPerTurnByPlayer[getID()] = 0;
		}
	}

	for (iI = 0; iI < MAX_TEAMS; iI++)
	{
		m_aiEspionageSpendingWeightAgainstTeam[iI] = 1;

		if (!bConstructorCall && getTeam() != NO_TEAM)
		{
			for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes) iJ).getTeam() == iI)
				{
					GET_PLAYER((PlayerTypes) iJ).setEspionageSpendingWeightAgainstTeam(getTeam(), 1);
				}
			}
		}
	}

	// Civ4 Reimagind
	for (iI = 0; iI < NUM_IDEOLOGY_TYPES; iI++)
	{
		m_aiIdeologyInfluence[iI] = 0;
	}

	// Civ4 Reimagind
	for (iI = 0; iI < NUM_IDEOLOGY_TYPES; iI++)
	{
		m_aiForeignTradeIdeologyModifier[iI] = 0;
		m_aiBonusRatioModifierPerIdeologyCiv[iI] = 0;
		m_aiIdeologyCombatExperienceModifier[iI] = 0;
	}

	for (iI = 0; iI < NUM_FEAT_TYPES; iI++)
	{
		m_abFeatAccomplished[iI] = false;
	}

	for (iI = 0; iI < NUM_PLAYEROPTION_TYPES; iI++)
	{
		m_abOptions[iI] = false;
	}

	// Civ4 Reimagined
	for (iI = 0; iI < NUM_NATIVEBONUS_TYPES; iI++)
	{
		m_abNativeAmericanBonus[iI] = false;
	}

	m_szScriptData = "";

	if (!bConstructorCall)
	{
		FAssertMsg(0 < GC.getNumBonusInfos(), "GC.getNumBonusInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiBonusExport==NULL, "about to leak memory, CvPlayer::m_paiBonusExport");
		m_paiBonusExport = new int [GC.getNumBonusInfos()];
		FAssertMsg(m_paiBonusImport==NULL, "about to leak memory, CvPlayer::m_paiBonusImport");
		m_paiBonusImport = new int [GC.getNumBonusInfos()]; // Civ4 Reimagined
		FAssertMsg(m_paiPlayerExtraAvailableBonuses==NULL, "about to leak memory, CvPlayer::m_paiBonusImport"); // Civ4 Reimagined
		m_paiPlayerExtraAvailableBonuses = new int [GC.getNumBonusInfos()];
		for (iI = 0; iI < GC.getNumBonusInfos(); iI++)
		{
			m_paiBonusExport[iI] = 0;
			m_paiBonusImport[iI] = 0;
			m_paiPlayerExtraAvailableBonuses[iI] = 0; // Civ4 Reimagined
		}

		FAssertMsg(0 < GC.getNumImprovementInfos(), "GC.getNumImprovementInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiImprovementCount==NULL, "about to leak memory, CvPlayer::m_paiImprovementCount");
		m_paiImprovementCount = new int [GC.getNumImprovementInfos()];
		FAssertMsg(m_paiRadiusImprovementHappiness==NULL, "about to leak memory, CvPlayer::m_paiRadiusImprovementHappiness");
		m_paiRadiusImprovementHappiness = new int [GC.getNumImprovementInfos()];
		FAssertMsg(m_paiGreatSpyPointsFromImprovementInRadius==NULL, "about to leak memory, CvPlayer::m_paiGreatSpyPointsFromImprovementInRadius");
		m_paiGreatSpyPointsFromImprovementInRadius = new int [GC.getNumImprovementInfos()];
		for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_paiImprovementCount[iI] = 0;
			m_paiRadiusImprovementHappiness[iI] = 0;
			m_paiGreatSpyPointsFromImprovementInRadius[iI] = 0;
		}

		FAssertMsg(m_paiFreeBuildingCount==NULL, "about to leak memory, CvPlayer::m_paiFreeBuildingCount");
		m_paiFreeBuildingCount = new int [GC.getNumBuildingInfos()];
		FAssertMsg(m_paiExtraBuildingHappiness==NULL, "about to leak memory, CvPlayer::m_paiExtraBuildingHappiness");
		m_paiExtraBuildingHappiness = new int [GC.getNumBuildingInfos()];
		FAssertMsg(m_paiExtraBuildingHealth==NULL, "about to leak memory, CvPlayer::m_paiExtraBuildingHealth");
		m_paiExtraBuildingHealth = new int [GC.getNumBuildingInfos()];
		//Leoreth
		FAssertMsg(m_paiBuildingProductionModifiers==NULL, "about to leak memory, CvPlayer::m_paiBuildingProductionModifiers");
		m_paiBuildingProductionModifiers = new int[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_paiFreeBuildingCount[iI] = 0;
			m_paiExtraBuildingHappiness[iI] = 0;
			m_paiExtraBuildingHealth[iI] = 0;
			m_paiBuildingProductionModifiers[iI] = 0;
		}
		
		FAssertMsg(m_paiFeatureHappiness==NULL, "about to leak memory, CvPlayer::m_paiFeatureHappiness");
		m_paiFeatureHappiness = new int [GC.getNumFeatureInfos()];
		for (iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			m_paiFeatureHappiness[iI] = 0;
		}

		FAssertMsg(m_paiUnitClassCount==NULL, "about to leak memory, CvPlayer::m_paiUnitClassCount");
		m_paiUnitClassCount = new int [GC.getNumUnitClassInfos()];
		FAssertMsg(m_paiUnitClassMaking==NULL, "about to leak memory, CvPlayer::m_paiUnitClassMaking");
		m_paiUnitClassMaking = new int [GC.getNumUnitClassInfos()];
		FAssertMsg(m_paiUnitClassProductionModifier==NULL, "about to leak memory, CvPlayer::m_paiUnitClassProductionModifier");
		m_paiUnitClassProductionModifier = new int[GC.getNumUnitClassInfos()];
		FAssertMsg(m_paiReligiousUnitClassProductionModifier==NULL, "about to leak memory, CvPlayer::m_paiReligiousUnitClassProductionModifier");
		m_paiReligiousUnitClassProductionModifier = new int[GC.getNumUnitClassInfos()];
		for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			m_paiUnitClassCount[iI] = 0;
			m_paiUnitClassMaking[iI] = 0;
			m_paiUnitClassProductionModifier[iI] = 0; // Civ4 Reimagined
			m_paiReligiousUnitClassProductionModifier[iI] = 0; // Civ4 Reimagined
		}

		FAssertMsg(m_paiBuildingClassCount==NULL, "about to leak memory, CvPlayer::m_paiBuildingClassCount");
		m_paiBuildingClassCount = new int [GC.getNumBuildingClassInfos()];
		FAssertMsg(m_paiBuildingClassMaking==NULL, "about to leak memory, CvPlayer::m_paiBuildingClassMaking");
		m_paiBuildingClassMaking = new int [GC.getNumBuildingClassInfos()];
		for (iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			m_paiBuildingClassCount[iI] = 0;
			m_paiBuildingClassMaking[iI] = 0;
		}

		FAssertMsg(m_paiHurryCount==NULL, "about to leak memory, CvPlayer::m_paiHurryCount");
		m_paiHurryCount = new int [GC.getNumHurryInfos()];
		for (iI = 0; iI < GC.getNumHurryInfos(); iI++)
		{
			m_paiHurryCount[iI] = 0;
		}

		FAssertMsg(m_paiSpecialBuildingNotRequiredCount==NULL, "about to leak memory, CvPlayer::m_paiSpecialBuildingNotRequiredCount");
		m_paiSpecialBuildingNotRequiredCount = new int [GC.getNumSpecialBuildingInfos()];
		for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
		{
			m_paiSpecialBuildingNotRequiredCount[iI] = 0;
		}
	
		FAssertMsg(m_paiHasCivicOptionCount==NULL, "about to leak memory, CvPlayer::m_paiHasCivicOptionCount");
		m_paiHasCivicOptionCount = new int[GC.getNumCivicOptionInfos()];
		FAssertMsg(m_paiHasNoAnarchyCivicOptionCount==NULL, "about to leak memory, CvPlayer::m_paiHasNoAnarchyCivicOptionCount"); // Civ4 Reimagined
		m_paiHasNoAnarchyCivicOptionCount = new int[GC.getNumCivicOptionInfos()]; // Civ4 Reimagined
		FAssertMsg(m_paiNoCivicUpkeepCount==NULL, "about to leak memory, CvPlayer::m_paiNoCivicUpkeepCount");
		m_paiNoCivicUpkeepCount = new int[GC.getNumCivicOptionInfos()];
		FAssertMsg(m_paeCivics==NULL, "about to leak memory, CvPlayer::m_paeCivics");
		m_paeCivics = new CivicTypes [GC.getNumCivicOptionInfos()];
		for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			m_paiHasCivicOptionCount[iI] = 0;
			m_paiHasNoAnarchyCivicOptionCount[iI] = 0; // Civ4 Reimagined
			m_paiNoCivicUpkeepCount[iI] = 0;
			m_paeCivics[iI] = NO_CIVIC;
		}

		// Civ4 Reimagined
		FAssertMsg(m_paiCivicEffect==NULL, "about to leak memory, CvPlayer::m_paiCivicEffect");
		m_paiCivicEffect = new int[GC.getNumCivicInfos()];
		for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			m_paiCivicEffect[iI] = 0;
		}

		// Civ4 Reimagined
		FAssertMsg(m_paiRouteChange==NULL, "about to leak memory, CvPlayer::m_paiRouteChange");
		m_paiRouteChange = new int[GC.getNumRouteInfos()];
		for (iI = 0; iI < GC.getNumRouteInfos(); iI++)
		{
			m_paiRouteChange[iI] = 0;
		}
		
		FAssertMsg(m_paiHasReligionCount==NULL, "about to leak memory, CvPlayer::m_paiHasReligionCount");
		m_paiHasReligionCount = new int[GC.getNumReligionInfos()];
		for (iI = 0;iI < GC.getNumReligionInfos();iI++)
		{
			m_paiHasReligionCount[iI] = 0;
		}
	
		FAssertMsg(m_paiHasCorporationCount==NULL, "about to leak memory, CvPlayer::m_paiHasReligionCount");
		m_paiHasCorporationCount = new int[GC.getNumCorporationInfos()];
		for (iI = 0;iI < GC.getNumCorporationInfos();iI++)
		{
			m_paiHasCorporationCount[iI] = 0;
		}

		FAssertMsg(m_pabResearchingTech==NULL, "about to leak memory, CvPlayer::m_pabResearchingTech");
		m_pabResearchingTech = new bool[GC.getNumTechInfos()];
		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			m_pabResearchingTech[iI] = false;
		}

		FAssertMsg(m_pabLoyalMember==NULL, "about to leak memory, CvPlayer::m_pabLoyalMember");
		m_pabLoyalMember = new bool[GC.getNumVoteSourceInfos()];
		for (iI = 0; iI < GC.getNumVoteSourceInfos(); iI++)
		{
			m_pabLoyalMember[iI] = true;
		}

		// Civ4 Reimagined
		FAssertMsg(m_pabUnlimitedSpecialistsWithTemple==NULL, "about to leak memory, CvPlayer::m_pabUnlimitedSpecialistsWithTemple");
		m_pabUnlimitedSpecialistsWithTemple = new bool[GC.getNumSpecialistInfos()];
		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_pabUnlimitedSpecialistsWithTemple[iI] = false;
		}

		FAssertMsg(0 < GC.getNumUpkeepInfos(), "GC.getNumUpkeepInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiUpkeepCount==NULL, "about to leak memory, CvPlayer::m_paiUpkeepCount");
		m_paiUpkeepCount = new int[GC.getNumUpkeepInfos()];
		for (iI = 0; iI < GC.getNumUpkeepInfos(); iI++)
		{
			m_paiUpkeepCount[iI] = 0;
		}

		FAssertMsg(0 < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiSpecialistValidCount==NULL, "about to leak memory, CvPlayer::m_paiSpecialistValidCount");
		m_paiSpecialistValidCount = new int[GC.getNumSpecialistInfos()];
		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_paiSpecialistValidCount[iI] = 0;
		}
		
		// Civ4 Reimagined
		FAssertMsg(0 < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiExtraSpecialists==NULL, "about to leak memory, CvPlayer::m_paiExtraSpecialists");
		m_paiExtraSpecialists = new int[GC.getNumSpecialistInfos()];
		FAssertMsg(m_paiExtraSpecialistExperience==NULL, "about to leak memory, CvPlayer::m_paiExtraSpecialistExperience");
		m_paiExtraSpecialistExperience = new int[GC.getNumSpecialistInfos()];
		FAssertMsg(m_paiExtraSpecialistHappiness==NULL, "about to leak memory, CvPlayer::m_paiExtraSpecialistHappiness");
		m_paiExtraSpecialistHappiness = new int[GC.getNumSpecialistInfos()];
		FAssertMsg(m_paiGreatPeopleRateChangeModifier==NULL, "about to leak memory, CvPlayer::m_paiGreatPeopleRateChangeModifier");
		m_paiGreatPeopleRateChangeModifier = new int[GC.getNumSpecialistInfos()];
		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_paiExtraSpecialists[iI] = 0;
			m_paiExtraSpecialistExperience[iI] = 0;
			m_paiExtraSpecialistHappiness[iI] = 0;
			m_paiGreatPeopleRateChangeModifier[iI] = 0;
		}
		
		// Civ4 Reimagined
		FAssertMsg(0 < GC.getNumTerrainInfos(), "GC.getNumTerrainInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_paiFatcrossTerrainHappiness==NULL, "about to leak memory, CvPlayer::m_paiFatcrossTerrainHappiness");
		m_paiFatcrossTerrainHappiness = new int[GC.getNumTerrainInfos()];
		FAssertMsg(m_paiFatcrossTerrainCulture==NULL, "about to leak memory, CvPlayer::m_paiFatcrossTerrainCulture");
		m_paiFatcrossTerrainCulture = new int[GC.getNumTerrainInfos()];
		FAssertMsg(m_paiTerrainMovementCostModifier==NULL, "about to leak memory, CvPlayer::m_paiTerrainMovementCostModifier");
		m_paiTerrainMovementCostModifier = new int[GC.getNumTerrainInfos()];
		for (iI = 0; iI < GC.getNumTerrainInfos(); iI++)
		{
			m_paiFatcrossTerrainHappiness[iI] = 0;
			m_paiFatcrossTerrainCulture[iI] = 0;
			m_paiTerrainMovementCostModifier[iI] = 0;
		}
		
		// Civ4 Reimagined
		FAssertMsg(m_paiCapitalCommercePopulationThreshold==NULL, "about to leak memory, CvPlayer::m_paiCapitalCommercePopulationThreshold");
		m_paiCapitalCommercePopulationThreshold = new int[NUM_COMMERCE_TYPES];
		FAssertMsg(m_paiCapitalCommerceModifier==NULL, "about to leak memory, CvPlayer::m_paiCapitalCommerceModifier");
		m_paiCapitalCommerceModifier = new int[NUM_COMMERCE_TYPES];
		FAssertMsg(m_paiAveragePopCommerceModifier==NULL, "about to leak memory, CvPlayer::m_paiAveragePopCommerceModifier");
		m_paiAveragePopCommerceModifier = new int[NUM_COMMERCE_TYPES];
		FAssertMsg(m_paiAveragePopCommerceModifierMaxPop==NULL, "about to leak memory, CvPlayer::m_paiAveragePopCommerceModifierMaxPop");
		m_paiAveragePopCommerceModifierMaxPop = new int[NUM_COMMERCE_TYPES];
		FAssertMsg(m_paiAveragePopCommerceModifierMaxMod==NULL, "about to leak memory, CvPlayer::m_paiAveragePopCommerceModifierMaxMod");
		m_paiAveragePopCommerceModifierMaxMod = new int[NUM_COMMERCE_TYPES];
		
		for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			m_paiCapitalCommercePopulationThreshold[iI] = 0;
			m_paiCapitalCommerceModifier[iI] = 0;
			m_paiAveragePopCommerceModifier[iI] = 0;
			m_paiAveragePopCommerceModifierMaxPop[iI] = 0;
			m_paiAveragePopCommerceModifierMaxMod[iI] = 0;
		}
		
	
		FAssertMsg(0 < GC.getNumSpecialistInfos(), "GC.getNumSpecialistInfos() is not greater than zero but it is used to allocate memory in CvPlayer::reset");
		FAssertMsg(m_ppaaiSpecialistExtraYield==NULL, "about to leak memory, CvPlayer::m_ppaaiSpecialistExtraYield");
		m_ppaaiSpecialistExtraYield = new int*[GC.getNumSpecialistInfos()];
		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_ppaaiSpecialistExtraYield[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiSpecialistExtraYield[iI][iJ] = 0;
			}
		}

		// Civ4 Reimagined
		FAssertMsg(m_ppaaiSpecialistCommerceChanges==NULL, "about to leak memory, CvPlayer::m_ppaaiSpecialistCommerceChanges");
		m_ppaaiSpecialistCommerceChanges = new int*[GC.getNumSpecialistInfos()];
		for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
		{
			m_ppaaiSpecialistCommerceChanges[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppaaiSpecialistCommerceChanges[iI][iJ] = 0;
			}
		}

		// Civ4 Reimagined
		FAssertMsg(m_ppaaiBonusHealthFromBuilding==NULL, "about to leak memory, CvPlayer::m_ppaaiBonusHealthFromBuilding");
		m_ppaaiBonusHealthFromBuilding = new int*[GC.getNumBuildingInfos()];
		for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			m_ppaaiBonusHealthFromBuilding[iI] = new int[GC.getNumBonusInfos()];
			for (iJ = 0; iJ < GC.getNumBonusInfos(); iJ++)
			{
				m_ppaaiBonusHealthFromBuilding[iI][iJ] = 0;
			}
		}

		FAssertMsg(m_ppaaiImprovementYieldChange==NULL, "about to leak memory, CvPlayer::m_ppaaiImprovementYieldChange");
		FAssertMsg(m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus==NULL, "about to leak memory, CvPlayer::m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus");
		m_ppaaiImprovementYieldChange = new int*[GC.getNumImprovementInfos()];
		m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus = new int*[GC.getNumImprovementInfos()];
		for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_ppaaiImprovementYieldChange[iI] = new int[NUM_YIELD_TYPES];
			// Civ4 Reimagined
			m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiImprovementYieldChange[iI][iJ] = 0;
				// Civ4 Reimagined
				m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus[iI][iJ] = 0;
			}
		}

		// Civ4 Reimagined
		FAssertMsg(m_ppaaiTerrainYieldChange==NULL, "about to leak memory, CvPlayer::m_ppaaiTerrainYieldChange");
		m_ppaaiTerrainYieldChange = new int*[GC.getNumTerrainInfos()];
		for (iI = 0; iI < GC.getNumTerrainInfos(); iI++)
		{
			m_ppaaiTerrainYieldChange[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiTerrainYieldChange[iI][iJ] = 0;
			}
		}
		
		FAssertMsg(m_ppaaiRadiusImprovementCommerceChange==NULL, "about to leak memory, CvPlayer::m_ppaaiRadiusImprovementCommerceChange");
		m_ppaaiRadiusImprovementCommerceChange = new int*[GC.getNumImprovementInfos()];
		for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
		{
			m_ppaaiRadiusImprovementCommerceChange[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppaaiRadiusImprovementCommerceChange[iI][iJ] = 0;
			}
		}
		
		// Civ4 Reimagined
		FAssertMsg(m_ppaaiBuildingYieldChange==NULL, "about to leak memory, CvPlayer::m_ppaaiBuildingYieldChange");
		m_ppaaiBuildingYieldChange = new int*[GC.getNumBuildingClassInfos()];
		for (iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			m_ppaaiBuildingYieldChange[iI] = new int[NUM_YIELD_TYPES];
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				m_ppaaiBuildingYieldChange[iI][iJ] = 0;
			}
		}

		// Civ4 Reimagined
		FAssertMsg(m_ppaiFeatureCommerce==NULL, "about to leak memory, CvPlayer::m_ppaiFeatureCommerce");
		m_ppaiFeatureCommerce = new int*[GC.getNumFeatureInfos()];
		for (iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			m_ppaiFeatureCommerce[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppaiFeatureCommerce[iI][iJ] = 0;
			}
		}

		// Civ4 Reimagined
		FAssertMsg(m_ppaiFeatureExtraYield==NULL, "about to leak memory, CvPlayer::m_ppaiFeatureExtraYield");
		m_ppaiFeatureExtraYield = new int*[GC.getNumFeatureInfos()];
		for (iI = 0; iI < GC.getNumFeatureInfos(); iI++)
		{
			m_ppaiFeatureExtraYield[iI] = new int[NUM_COMMERCE_TYPES];
			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				m_ppaiFeatureExtraYield[iI][iJ] = 0;
			}
		}		

		m_mapEventsOccured.clear();
		m_mapEventCountdown.clear();
		m_aFreeUnitCombatPromotions.clear();
		m_aFreeUnitClassPromotions.clear();
		m_aVote.clear();
		m_aUnitExtraCosts.clear();
		m_triggersFired.clear();
	}

	m_plotGroups.removeAll();

	m_cities.removeAll();

	m_units.removeAll();

	m_selectionGroups.removeAll();

	m_eventsTriggered.removeAll();

	if (!bConstructorCall)
	{
		AI_reset(false);
	}
}

/************************************************************************************************/
/* CHANGE_PLAYER                          08/17/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
//
// for stripping obsolete trait bonuses
// for complete reset, use in conjunction with addTraitBonuses
//
void CvPlayer::clearTraitBonuses( )
{
	int iI, iJ;

	FAssertMsg((GC.getNumTraitInfos() > 0), "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::init");
	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			changeExtraHealth(-GC.getTraitInfo((TraitTypes)iI).getHealth());
			changeExtraHappiness(-GC.getTraitInfo((TraitTypes)iI).getHappiness());

			for (iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
			{
				changeExtraBuildingHappiness((BuildingTypes)iJ, -GC.getBuildingInfo((BuildingTypes)iJ).getHappinessTraits(iI));
			}

			changeUpkeepModifier(-GC.getTraitInfo((TraitTypes)iI).getUpkeepModifier());
			changeLevelExperienceModifier(-GC.getTraitInfo((TraitTypes)iI).getLevelExperienceModifier());
			changeGreatPeopleRateModifier(-GC.getTraitInfo((TraitTypes)iI).getGreatPeopleRateModifier());
			changeGreatGeneralRateModifier(-GC.getTraitInfo((TraitTypes)iI).getGreatGeneralRateModifier());
			changeDomesticGreatGeneralRateModifier(-GC.getTraitInfo((TraitTypes)iI).getDomesticGreatGeneralRateModifier());

			changeMaxGlobalBuildingProductionModifier(-GC.getTraitInfo((TraitTypes)iI).getMaxGlobalBuildingProductionModifier());
			changeMaxTeamBuildingProductionModifier(-GC.getTraitInfo((TraitTypes)iI).getMaxTeamBuildingProductionModifier());
			changeMaxPlayerBuildingProductionModifier(-GC.getTraitInfo((TraitTypes)iI).getMaxPlayerBuildingProductionModifier());
			changeBuildingsProductionModifier(-GC.getTraitInfo((TraitTypes)iI).getBuildingProductionModifier()); // Civ4 Reimagined
			changeDistanceMaintenanceModifier(-GC.getTraitInfo((TraitTypes)iI).getDistanceMaintenanceModifier()); // Civ4 Reimagined
			changeBonusValueModifier(-GC.getTraitInfo((TraitTypes)iI).getBonusValueModifier()); // Civ4 Reimagined
			changeVassalCommerceGain(-GC.getTraitInfo((TraitTypes)iI).getVassalCommerceGain()); // Civ4 Reimagined
			changeIsNoPillageCount((GC.getTraitInfo((TraitTypes)iI).isNoPillage() ? -1 : 0)); // Civ4 Reimagined

			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				changeTradeYieldModifier(((YieldTypes)iJ), -GC.getTraitInfo((TraitTypes)iI).getTradeYieldModifier(iJ));
			}

			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				changeFreeCityCommerce(((CommerceTypes)iJ), -GC.getTraitInfo((TraitTypes)iI).getCommerceChange(iJ));
				changeCommerceRateModifier(((CommerceTypes)iJ), -GC.getTraitInfo((TraitTypes)iI).getCommerceModifier(iJ));
				changeInitCityCommerce(((CommerceTypes)iJ), -GC.getTraitInfo((TraitTypes)iI).getFreeCommerce(iJ)); //Civ4 Reimagined
			}

			for (iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
			{
				if (GC.getCivicOptionInfo((CivicOptionTypes) iJ).getTraitNoUpkeep(iI))
				{
					changeNoCivicUpkeepCount(((CivicOptionTypes)iJ), -1);
				}
			}
		}
	}
}

//
// for adding new trait bonuses
//
void CvPlayer::addTraitBonuses( )
{
	int iI, iJ;

	FAssertMsg((GC.getNumTraitInfos() > 0), "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::init");
	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			changeExtraHealth(GC.getTraitInfo((TraitTypes)iI).getHealth());
			changeExtraHappiness(GC.getTraitInfo((TraitTypes)iI).getHappiness());

			for (iJ = 0; iJ < GC.getNumBuildingInfos(); iJ++)
			{
				changeExtraBuildingHappiness((BuildingTypes)iJ, GC.getBuildingInfo((BuildingTypes)iJ).getHappinessTraits(iI));
			}

			changeUpkeepModifier(GC.getTraitInfo((TraitTypes)iI).getUpkeepModifier());
			changeLevelExperienceModifier(GC.getTraitInfo((TraitTypes)iI).getLevelExperienceModifier());
			changeGreatPeopleRateModifier(GC.getTraitInfo((TraitTypes)iI).getGreatPeopleRateModifier());
			changeGreatGeneralRateModifier(GC.getTraitInfo((TraitTypes)iI).getGreatGeneralRateModifier());
			changeDomesticGreatGeneralRateModifier(GC.getTraitInfo((TraitTypes)iI).getDomesticGreatGeneralRateModifier());

			changeMaxGlobalBuildingProductionModifier(GC.getTraitInfo((TraitTypes)iI).getMaxGlobalBuildingProductionModifier());
			changeMaxTeamBuildingProductionModifier(GC.getTraitInfo((TraitTypes)iI).getMaxTeamBuildingProductionModifier());
			changeMaxPlayerBuildingProductionModifier(GC.getTraitInfo((TraitTypes)iI).getMaxPlayerBuildingProductionModifier());
			changeBuildingsProductionModifier(GC.getTraitInfo((TraitTypes)iI).getBuildingProductionModifier()); // Civ4 Reimagined
			changeDistanceMaintenanceModifier(GC.getTraitInfo((TraitTypes)iI).getDistanceMaintenanceModifier()); // Civ4 Reimagined
			changeBonusValueModifier(GC.getTraitInfo((TraitTypes)iI).getBonusValueModifier()); // Civ4 Reimagined
			changeVassalCommerceGain(GC.getTraitInfo((TraitTypes)iI).getVassalCommerceGain()); // Civ4 Reimagined
			changeIsNoPillageCount(GC.getTraitInfo((TraitTypes)iI).isNoPillage()); // Civ4 Reimagined

			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				changeTradeYieldModifier(((YieldTypes)iJ), GC.getTraitInfo((TraitTypes)iI).getTradeYieldModifier(iJ));
			}

			for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
			{
				changeFreeCityCommerce(((CommerceTypes)iJ), GC.getTraitInfo((TraitTypes)iI).getCommerceChange(iJ));
				changeCommerceRateModifier(((CommerceTypes)iJ), GC.getTraitInfo((TraitTypes)iI).getCommerceModifier(iJ));
				changeInitCityCommerce(((CommerceTypes)iJ), GC.getTraitInfo((TraitTypes)iI).getFreeCommerce(iJ)); //Civ4 Reimagined
			}

			for (iJ = 0; iJ < GC.getNumCivicOptionInfos(); iJ++)
			{
				if (GC.getCivicOptionInfo((CivicOptionTypes) iJ).getTraitNoUpkeep(iI))
				{
					changeNoCivicUpkeepCount(((CivicOptionTypes)iJ), 1);
				}
			}
		}
	}

	updateMaxAnarchyTurns();

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		updateExtraYieldThreshold((YieldTypes)iI);
	}
}
/************************************************************************************************/
/* CHANGE_PLAYER                           END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* CHANGE_PLAYER                          08/17/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
//
// for changing the personality of the player
//
void CvPlayer::changePersonalityType( )
{
	LeaderHeadTypes eBestPersonality;
	int iValue;
	int ipBestValue;
	int iI, iJ;

	if (GC.getGameINLINE().isOption(GAMEOPTION_RANDOM_PERSONALITIES))
	{
		if (!isBarbarian())
		{
			ipBestValue = 0;
			eBestPersonality = NO_LEADER;

			for (iI = 0; iI < GC.getNumLeaderHeadInfos(); iI++)
			{
				if (iI != GC.getDefineINT("BARBARIAN_LEADER")) // XXX minor civ???
				{
					iValue = (1 + GC.getGameINLINE().getSorenRandNum(10000, "Choosing Personality"));

					for (iJ = 0; iJ < MAX_CIV_PLAYERS; iJ++)
					{
						if (GET_PLAYER((PlayerTypes)iJ).isAlive())
						{
							if (GET_PLAYER((PlayerTypes)iJ).getPersonalityType() == ((LeaderHeadTypes)iI))
							{
								iValue /= 2;
							}
						}
					}

					if (iValue > ipBestValue)
					{
						ipBestValue = iValue;
						eBestPersonality = ((LeaderHeadTypes)iI);
					}
				}
			}

			if (eBestPersonality != NO_LEADER)
			{
				setPersonalityType(eBestPersonality);
			}
		}
	}
	else
	{
		setPersonalityType( getLeaderType() );
	}
}
/************************************************************************************************/
/* CHANGE_PLAYER                           END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* CHANGE_PLAYER                          08/17/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
//
// reset state of event logic, unit prices
//
void CvPlayer::resetCivTypeEffects( )
{
	int iI;

	if( !isAlive() ) 
	{
		for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			setCivics(((CivicOptionTypes)iI), ((CivicTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationInitialCivics(iI))));
		}

		for (iI = 0; iI < GC.getNumEventInfos(); iI++)
		{
			resetEventOccured((EventTypes)iI, false);
		}

		for (iI = 0; iI < GC.getNumEventTriggerInfos(); iI++)
		{
			if( (!GC.getEventTriggerInfo((EventTriggerTypes)iI).isGlobal()) && (!GC.getEventTriggerInfo((EventTriggerTypes)iI).isTeam() || GET_TEAM(getTeam()).getNumMembers() == 1) )
			{
				resetTriggerFired((EventTriggerTypes)iI);
			}
		}
	}

	for (iI = 0; iI < GC.getNumUnitClassInfos(); ++iI)
	{
		UnitTypes eUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

		if (NO_UNIT != eUnit)
		{
			if (GC.getUnitInfo(eUnit).isFound())
			{
				setUnitExtraCost((UnitClassTypes)iI, getNewCityProductionValue());
			}
		}
	}
}
/************************************************************************************************/
/* CHANGE_PLAYER                           END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* CHANGE_PLAYER                          08/17/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
//
// for switching the leaderhead of this player
//
void CvPlayer::changeLeader( LeaderHeadTypes eNewLeader )
{
	LeaderHeadTypes eOldLeader = getLeaderType();

	if( eOldLeader == eNewLeader )
		return;

	// Clear old traits
	clearTraitBonuses();

	GC.getInitCore().setLeader( getID(), eNewLeader );

	// Add new traits
	addTraitBonuses();

	// Set new personality
	changePersonalityType();

	if( isAlive() || isEverAlive() )
	{
		gDLL->getInterfaceIFace()->setDirty(HighlightPlot_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Foreign_Screen_DIRTY_BIT, true);
	}

	AI_init();
}
/************************************************************************************************/
/* CHANGE_PLAYER                           END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* CHANGE_PLAYER                          05/09/09                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
//
// for changing the civilization of this player
//
void CvPlayer::changeCiv( CivilizationTypes eNewCiv )
{
	CivilizationTypes eOldCiv = getCivilizationType();
	PlayerColorTypes eColor = (PlayerColorTypes)GC.getCivilizationInfo(eNewCiv).getDefaultPlayerColor();

	if( eOldCiv == eNewCiv )
		return;

	for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if (eColor == NO_PLAYERCOLOR || (GET_PLAYER((PlayerTypes)iI).getPlayerColor() == eColor && iI != getID()) )
		{
			for (int iK = 0; iK < GC.getNumPlayerColorInfos(); iK++)
			{
				if (iK != GC.getCivilizationInfo((CivilizationTypes)GC.getDefineINT("BARBARIAN_CIVILIZATION")).getDefaultPlayerColor())
				{
					bool bValid = true;

					for (int iL = 0; iL < MAX_CIV_PLAYERS; iL++)
					{
						if (GET_PLAYER((PlayerTypes)iL).getPlayerColor() == iK)
						{
							bValid = false;
							break;
						}
					}

					if (bValid)
					{
						eColor = (PlayerColorTypes)iK;
						iI = MAX_CIV_PLAYERS;
						break;
					}
				}
			}
		}
	}

	GC.getInitCore().setCiv( getID(), eNewCiv );
	GC.getInitCore().setColor( getID(), eColor );

	resetCivTypeEffects();

	if( isAlive() )
	{
		// if the player is alive and showing on scoreboard, etc
		// change colors, graphics, flags, units
		GC.getInitCore().setFlagDecal( getID(), (CvWString)GC.getCivilizationInfo(eNewCiv).getFlagTexture() );
		GC.getInitCore().setArtStyle( getID(), (ArtStyleTypes)GC.getCivilizationInfo(eNewCiv).getArtStyleType() );

		// Forces update of units flags
		EraTypes eEra = getCurrentEra();
		bool bAuto = m_bDisableHuman;
		m_bDisableHuman = true;
		//setCurrentEra((EraTypes)((eEra + 1)%GC.getNumEraInfos()));
		setCurrentEra((EraTypes)0);
		setCurrentEra((EraTypes)(GC.getNumEraInfos() - 1));

		setCurrentEra(eEra);
		m_bDisableHuman = bAuto;
		gDLL->getInterfaceIFace()->makeInterfaceDirty();
		

		int iLoop;
		CvCity* pLoopCity;
		// dirty all of this player's cities...
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			//if (pLoopCity->getOwnerINLINE() == getID())
			FAssert(pLoopCity->getOwnerINLINE() == getID()); // K-Mod
			{
				pLoopCity->setLayoutDirty(true);
			}
		}

		//update unit eras
		CvUnit* pLoopUnit;
		CvPlot* pLoopPlot;
		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			pLoopUnit->reloadEntity();
			pLoopPlot = pLoopUnit->plot();
/*
			if( pLoopPlot != NULL )
			{
				
				CvFlagEntity* pFlag = pLoopPlot->getFlagSymbol();

				if( pFlag != NULL )
				{

					if( gDLL->getFlagEntityIFace()->getPlayer(pFlag) == getID() )
					{
						gDLL->getFlagEntityIFace()->destroy(pFlag);
						CvFlagEntity* pNewFlag = gDLL->getFlagEntityIFace()->create(getID());
						if (pFlag != NULL)
						{
							gDLL->getFlagEntityIFace()->setPlot(pNewFlag, pLoopPlot, false);
						}

						gDLL->getFlagEntityIFace()->updateGraphicEra(pNewFlag);
					}

				}
				
				pLoopPlot->setFlagDirty(true);
				//pLoopPlot->updateGraphicEra();
			}
*/
		}

		//update flag eras
		gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Soundtrack_DIRTY_BIT, true);
		}

		gDLL->getInterfaceIFace()->makeInterfaceDirty();

		// Need to force redraw
		gDLL->getEngineIFace()->SetDirty(CultureBorders_DIRTY_BIT, true);
		gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
		gDLL->getEngineIFace()->SetDirty(GlobeTexture_DIRTY_BIT, true);
		gDLL->getEngineIFace()->SetDirty(GlobePartialTexture_DIRTY_BIT, true);

		gDLL->getInterfaceIFace()->setDirty(ColoredPlots_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(HighlightPlot_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GlobeLayer_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(MinimapSection_DIRTY_BIT, true);
		gDLL->getEngineIFace()->SetDirty(MinimapTexture_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Foreign_Screen_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionSound_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GlobeInfo_DIRTY_BIT, true);
	}
	else if( isEverAlive() )
	{
		// Not currently alive, but may show on some people's scoreboard
		// or graphs
		// change colors
		gDLL->getInterfaceIFace()->setDirty(InfoPane_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}

	setupGraphical();
}
/************************************************************************************************/
/* CHANGE_PLAYER                           END                                                  */
/************************************************************************************************/

/************************************************************************************************/
/* CHANGE_PLAYER                          08/17/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
//
// for changing whether this player is human or not
//
void CvPlayer::setIsHuman( bool bNewValue )
{
	if( bNewValue == isHuman() )
		return;

	if( bNewValue )
		GC.getInitCore().setSlotStatus( getID(), SS_TAKEN );
	else
		GC.getInitCore().setSlotStatus( getID(), SS_COMPUTER ); // or SS_OPEN for multiplayer?

}
/************************************************************************************************/
/* CHANGE_PLAYER                          END                                                  */
/************************************************************************************************/

//////////////////////////////////////
// graphical only setup
//////////////////////////////////////
void CvPlayer::setupGraphical()
{
	if (!GC.IsGraphicsInitialized())
		return;

	CvCity* pLoopCity;
	CvUnit* pLoopUnit;

	// Setup m_cities
	int iLoop;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->setupGraphical();
	}

	// Setup m_units
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->setupGraphical();
	}
}


void CvPlayer::initFreeState()
{
	setGold(0);
	changeGold(GC.getHandicapInfo(getHandicapType()).getStartingGold());
	changeGold(GC.getEraInfo(GC.getGameINLINE().getStartEra()).getStartingGold());

	clearResearchQueue();
}


void CvPlayer::initFreeUnits()
{
	UnitTypes eLoopUnit;
	int iFreeCount;
	int iI, iJ;

	if (GC.getGameINLINE().isOption(GAMEOPTION_ADVANCED_START) && !isBarbarian())
	{
		int iPoints = GC.getGameINLINE().getNumAdvancedStartPoints();

		iPoints *= GC.getHandicapInfo(getHandicapType()).getAdvancedStartPointsMod();
		iPoints /= 100;

		if (!isHuman())
		{
			iPoints *= GC.getHandicapInfo(getHandicapType()).getAIAdvancedStartPercent();
			iPoints /= 100;
		}

		setAdvancedStartPoints(iPoints);

		// Starting visibility
		CvPlot* pStartingPlot = getStartingPlot();
		if (NULL != pStartingPlot)
		{
			for (int iPlotLoop = 0; iPlotLoop < GC.getMapINLINE().numPlots(); ++iPlotLoop)
			{
				CvPlot* pPlot = GC.getMapINLINE().plotByIndex(iPlotLoop);

				if (plotDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE()) <= GC.getDefineINT("ADVANCED_START_SIGHT_RANGE"))
				{
					pPlot->setRevealed(getTeam(), true, false, NO_TEAM, false);
				}
			}
		}
	}
	else
	{
		for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
		{
			eLoopUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI);

			if (eLoopUnit != NO_UNIT)
			{
				iFreeCount = GC.getCivilizationInfo(getCivilizationType()).getCivilizationFreeUnitsClass(iI);

				iFreeCount *= (GC.getEraInfo(GC.getGameINLINE().getStartEra()).getStartingUnitMultiplier() + ((!isHuman()) ? GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIStartingUnitMultiplier() : 0));

				for (iJ = 0; iJ < iFreeCount; iJ++)
				{
					addFreeUnit(eLoopUnit);
				}
			}
		}

		iFreeCount = GC.getEraInfo(GC.getGameINLINE().getStartEra()).getStartingDefenseUnits();
		iFreeCount += GC.getHandicapInfo(getHandicapType()).getStartingDefenseUnits();

		if (!isHuman())
		{
			iFreeCount += GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIStartingDefenseUnits();
		}

		if (iFreeCount > 0)
		{
			addFreeUnitAI(UNITAI_CITY_DEFENSE, iFreeCount);
		}

		iFreeCount = GC.getEraInfo(GC.getGameINLINE().getStartEra()).getStartingWorkerUnits();
		iFreeCount += GC.getHandicapInfo(getHandicapType()).getStartingWorkerUnits();

		if (!isHuman())
		{
			iFreeCount += GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIStartingWorkerUnits();
		}

		if (iFreeCount > 0)
		{
			addFreeUnitAI(UNITAI_WORKER, iFreeCount);
		}

		iFreeCount = GC.getEraInfo(GC.getGameINLINE().getStartEra()).getStartingExploreUnits();
		iFreeCount += GC.getHandicapInfo(getHandicapType()).getStartingExploreUnits();

		if (!isHuman())
		{
			iFreeCount += GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIStartingExploreUnits();
		}

		if (iFreeCount > 0)
		{
			addFreeUnitAI(UNITAI_EXPLORE, iFreeCount);
		}

		// Civ4 Reimagined Starting visibility
		CvPlot* pStartingPlot = getStartingPlot();
		if (NULL != pStartingPlot)
		{
			for (int iPlotLoop = 0; iPlotLoop < GC.getMapINLINE().numPlots(); ++iPlotLoop)
			{
				CvPlot* pPlot = GC.getMapINLINE().plotByIndex(iPlotLoop);

				if (plotDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE()) <= GC.getDefineINT("START_SIGHT_RANGE"))
				{
					pPlot->setRevealed(getTeam(), true, false, NO_TEAM, false);
				}
			}
		}
	}
}


void CvPlayer::addFreeUnitAI(UnitAITypes eUnitAI, int iCount)
{
	UnitTypes eLoopUnit;
	UnitTypes eBestUnit;
	bool bValid;
	int iValue;
	int ipBestValue;
	int iI, iJ;

	eBestUnit = NO_UNIT;
	ipBestValue = 0;

	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		eLoopUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI);

		if (eLoopUnit != NO_UNIT)
		{
			if (canTrain(eLoopUnit))
			{
				bValid = true;

				if (GC.getUnitInfo(eLoopUnit).getPrereqAndBonus() != NO_BONUS)
				{
					bValid = false;
				}

				for (iJ = 0; iJ < GC.getNUM_UNIT_PREREQ_OR_BONUSES(); iJ++)
				{
					if (GC.getUnitInfo(eLoopUnit).getPrereqOrBonuses(iJ) != NO_BONUS)
					{
						bValid = false;
					}
				}

				if (bValid)
				{
					iValue = AI_unitValue(eLoopUnit, eUnitAI, NULL);

					if (iValue > ipBestValue)
					{
						eBestUnit = eLoopUnit;
						ipBestValue = iValue;
					}
				}
			}
		}
	}

	if (eBestUnit != NO_UNIT)
	{
		for (iI = 0; iI < iCount; iI++)
		{
			addFreeUnit(eBestUnit, eUnitAI);
		}
	}
}


void CvPlayer::addFreeUnit(UnitTypes eUnit, UnitAITypes eUnitAI)
{
	CvPlot* pStartingPlot;
	CvPlot* pBestPlot;
	CvPlot* pLoopPlot;
	int iRandOffset;
	int iI;

	if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		if ((eUnitAI == UNITAI_SETTLE) || (GC.getUnitInfo(eUnit).getDefaultUnitAIType() == UNITAI_SETTLE))
		{
			if (AI_getNumAIUnits(UNITAI_SETTLE) >= 1)
			{
				return;
			}
		}
	}

	pStartingPlot = getStartingPlot();

	if (pStartingPlot != NULL)
	{
		pBestPlot = NULL;

		if (isHuman())
		{
			long lResult=0;
			gDLL->getPythonIFace()->callFunction(gDLL->getPythonIFace()->getMapScriptModule(), "startHumansOnSameTile", NULL, &lResult);
			if (lResult == 0)
			{
				if (!(GC.getUnitInfo(eUnit).isFound()))
				{
					iRandOffset = GC.getGameINLINE().getSorenRandNum(NUM_CITY_PLOTS, "Place Units (Player)");

					for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
					{
						pLoopPlot = plotCity(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE(), ((iI + iRandOffset) % NUM_CITY_PLOTS));

						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->getArea() == pStartingPlot->getArea())
							{
								if (!(pLoopPlot->isImpassable()))
								{
									if (!(pLoopPlot->isUnit()))
									{
										if (!(pLoopPlot->isGoody()))
										{
											pBestPlot = pLoopPlot;
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

		if (pBestPlot == NULL)
		{
			pBestPlot = pStartingPlot;
		}

		initUnit(eUnit, pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), eUnitAI);
	}
}


int CvPlayer::startingPlotRange() const
{
	int iRange;

	iRange = (GC.getMapINLINE().maxStepDistance() + 10);

	iRange *= GC.getDefineINT("STARTING_DISTANCE_PERCENT");
	iRange /= 100;

	iRange *= (GC.getMapINLINE().getLandPlots() / (GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities() * GC.getGameINLINE().countCivPlayersAlive()));
	iRange /= NUM_CITY_PLOTS;

	iRange += std::min(((GC.getMapINLINE().getNumAreas() + 1) / 2), GC.getGameINLINE().countCivPlayersAlive());

	long lResult=0;
	if (gDLL->getPythonIFace()->callFunction(gDLL->getPythonIFace()->getMapScriptModule(), "minStartingDistanceModifier", NULL, &lResult))
	{
		iRange *= std::max<int>(0, (lResult + 100));
		iRange /= 100;
	}

	return std::max(iRange, GC.getDefineINT("MIN_CIV_STARTING_DISTANCE"));
}


bool CvPlayer::startingPlotWithinRange(CvPlot* pPlot, PlayerTypes ePlayer, int iRange, int iPass) const
{
	//PROFILE_FUNC();

	//XXX changes to AI_foundValue (which are far more flexible) make this function 
	//    redundant but it is still called from Python. 
	return false;
}

int CvPlayer::startingPlotDistanceFactor(CvPlot* pPlot, PlayerTypes ePlayer, int iRange) const
{
	PROFILE_FUNC();
	
	FAssert(ePlayer != getID());

	CvPlot* pStartingPlot;
	
	int iValue = 1000;

	pStartingPlot = getStartingPlot();

	if (pStartingPlot != NULL)
	{
		if (GC.getGameINLINE().isTeamGame())
		{
			if (GET_PLAYER(ePlayer).getTeam() == getTeam())
			{
				iRange *= GC.getDefineINT("OWN_TEAM_STARTING_MODIFIER");
				iRange /= 100;
			}
			else
			{
				iRange *= GC.getDefineINT("RIVAL_TEAM_STARTING_MODIFIER");
				iRange /= 100;
			}
		}

		int iDistance = stepDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE());
		if (pStartingPlot->getArea() != pPlot->getArea())
		{
			iDistance *= 4;            	
			iDistance /= 3;
		}

		iValue *= iDistance;
		iValue /= iRange ;

	}
    
	return std::max(1, iValue);

}


// Returns the id of the best area, or -1 if it doesn't matter:
int CvPlayer::findStartingArea() const
{
	PROFILE_FUNC();

	long result = -1;
	CyArgsList argsList;
	argsList.add(getID());		// pass in this players ID
	if (gDLL->getPythonIFace()->callFunction(gDLL->getPythonIFace()->getMapScriptModule(), "findStartingArea", argsList.makeFunctionArgs(), &result))
	{
		if (!gDLL->getPythonIFace()->pythonUsingDefaultImpl()) // Python override
		{
			if (result == -1 || GC.getMapINLINE().getArea(result) != NULL)
			{
				return result;
			}
			else
			{
				FAssertMsg(false, "python findStartingArea() must return -1 or the ID of a valid area");
			}
		}
	}

	int ipBestValue = 0;
	int iBestArea = -1;
	int iValue;
	int iLoop = 0;

	CvArea *pLoopArea = NULL;

	// find best land area
	for(pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		if (!(pLoopArea->isWater()))
		{
			// iNumPlayersOnArea is the number of players starting on the area, plus this player
			int iNumPlayersOnArea = (pLoopArea->getNumStartingPlots() + 1);
			int iTileValue = ((pLoopArea->calculateTotalBestNatureYield() + (pLoopArea->countCoastalLand() * 2) + pLoopArea->getNumRiverEdges() + (pLoopArea->getNumTiles())) + 1);
			iValue = iTileValue / iNumPlayersOnArea;

			iValue *= std::min(NUM_CITY_PLOTS + 1, pLoopArea->getNumTiles() + 1);
			iValue /= (NUM_CITY_PLOTS + 1);

			if (iNumPlayersOnArea <= 2)
			{
				iValue *= 4;
				iValue /= 3;
			}

			if (iValue > ipBestValue)
			{
				ipBestValue = iValue;
				iBestArea = pLoopArea->getID();
			}
		}
	}

	return iBestArea;
}


CvPlot* CvPlayer::findStartingPlot(bool bRandomize)
{
	PROFILE_FUNC();

	long result = -1;
	CyArgsList argsList;
	argsList.add(getID());		// pass in this players ID
	if (gDLL->getPythonIFace()->callFunction(gDLL->getPythonIFace()->getMapScriptModule(), "findStartingPlot", argsList.makeFunctionArgs(), &result))
	{
		if (!gDLL->getPythonIFace()->pythonUsingDefaultImpl()) // Python override
		{
			CvPlot *pPlot = GC.getMapINLINE().plotByIndexINLINE(result);
			if (pPlot != NULL)
			{
				return pPlot;
			}
			else
			{
				FAssertMsg(false, "python findStartingPlot() returned an invalid plot index!");
			}
		}
	}

	CvPlot* pLoopPlot;
	bool bValid;
	int iBestArea = -1;
	int iValue;
	int iRange;
	int iI;

	bool bNew = false;
	if (getStartingPlot() != NULL)
	{
		iBestArea = getStartingPlot()->getArea();
		setStartingPlot(NULL, true);
		bNew = true;
	}

	AI_updateFoundValues(true);//this sets all plots found values to -1
	
	if (!bNew)
	{
		iBestArea = findStartingArea();
	}

	iRange = startingPlotRange();
	for(int iPass = 0; iPass < GC.getMapINLINE().maxPlotDistance(); iPass++)
	{
		CvPlot *pBestPlot = NULL;
		int ipBestValue = 0;
		
		for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
		{
			pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

			if ((iBestArea == -1) || (pLoopPlot->getArea() == iBestArea))
			{
				//the distance factor is now done inside foundValue
				iValue = pLoopPlot->getFoundValue(getID());

				if (bRandomize && iValue > 0)
				{
					iValue += GC.getGameINLINE().getSorenRandNum(10000, "Randomize Starting Location");
				}

				if (iValue > ipBestValue)
				{
					bValid = true;

					if (bValid)
					{
						ipBestValue = iValue;
						pBestPlot = pLoopPlot;
					}
				}
			}
		}

		if (pBestPlot != NULL)
		{
			return pBestPlot;
		}

		FAssertMsg(iPass != 0, "CvPlayer::findStartingPlot - could not find starting plot in first pass.");
	}

	FAssertMsg(false, "Could not find starting plot.");
	return NULL;
}


CvPlotGroup* CvPlayer::initPlotGroup(CvPlot* pPlot)
{
	CvPlotGroup* pPlotGroup;

	pPlotGroup = addPlotGroup();

	FAssertMsg(pPlotGroup != NULL, "PlotGroup is not assigned a valid value");

	pPlotGroup->init(pPlotGroup->getID(), getID(), pPlot);

	return pPlotGroup;
}


CvCity* CvPlayer::initCity(int iX, int iY, bool bBumpUnits, bool bUpdatePlotGroups)
{
	PROFILE_FUNC();

	CvCity* pCity;

	pCity = addCity();

	FAssertMsg(pCity != NULL, "City is not assigned a valid value");
	FAssertMsg(!(GC.getMapINLINE().plotINLINE(iX, iY)->isCity()), "No city is expected at this plot when initializing new city");

	pCity->init(pCity->getID(), getID(), iX, iY, bBumpUnits, bUpdatePlotGroups);

	// Civ4 Reimagined
	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CALENDAR")))
	{
		if (pCity->plot()->isAdjacentToPeak())
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CALENDAR"), getID(), true);
		}
	}

	// Civ4 Reimagined
	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_ASTRONOMY")))
	{
		if (pCity->plot()->countAdjacentPeaks() > 2)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_ASTRONOMY"), getID(), true);
		}
	}

	return pCity;
}


void CvPlayer::acquireCity(CvCity* pOldCity, bool bConquest, bool bTrade, bool bUpdatePlotGroups)
{
	CLLNode<IDInfo>* pUnitNode;
	CvCity* pNewCity;
	CvUnit* pLoopUnit;
	CvPlot* pCityPlot;
	CvPlot* pLoopPlot;
	bool* pabHasReligion;
	bool* pabHolyCity;
	bool* pabHasCorporation;
	bool* pabHeadquarters;
	int* paiNumRealBuilding;
	int* paiBuildingOriginalOwner;
	int* paiBuildingOriginalTime;
	CvWString szBuffer;
	CvWString szName;
	bool abEverOwned[MAX_PLAYERS];
	int aiCulture[MAX_PLAYERS];
	PlayerTypes eOldOwner;
	PlayerTypes eOriginalOwner;
	PlayerTypes eHighestCulturePlayer;
	PlayerTypes eOldHighestCulturePlayer; // Civ4 Reimagined
	BuildingTypes eBuilding;
	bool bRecapture;
	bool bRaze;
	bool bGift;
	bool bOldEverOwned; // Civ4 Reimagined
	int iRange;
	int iCaptureGold;
	int iGameTurnFounded;
	int iPopulation;
	int iHighestPopulation;
	int iHurryAngerTimer;
	int iConscriptAngerTimer;
	int iDefyResolutionAngerTimer;
	int iOccupationTimer;
	int iTeamCulturePercent;
	int iDamage;
	int iDX, iDY;
	int iI;
	int iOldCultureLevel = (int)(pOldCity->getCultureLevel());
	CLinkList<IDInfo> oldUnits;

	pCityPlot = pOldCity->plot();

	pUnitNode = pCityPlot->headUnitNode();

	while (pUnitNode != NULL)
	{
		oldUnits.insertAtEnd(pUnitNode->m_data);
		pUnitNode = pCityPlot->nextUnitNode(pUnitNode);
	}

	pUnitNode = oldUnits.head();

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = oldUnits.next(pUnitNode);

		if (pLoopUnit && pLoopUnit->getTeam() != getTeam())
		{
			if (pLoopUnit->getDomainType() == DOMAIN_IMMOBILE)
			{
				pLoopUnit->kill(false, getID());
			}
		}
	}

	if (bConquest)
	{
		iRange = pOldCity->getCultureLevel();

		for (iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for (iDY = -(iRange); iDY <= iRange; iDY++)
			{
				if (pOldCity->cultureDistance(iDX, iDY) <= iRange)
				{
					pLoopPlot = plotXY(pOldCity->getX_INLINE(),pOldCity-> getY_INLINE(), iDX, iDY);

					if (pLoopPlot != NULL)
					{
						if (pLoopPlot->getOwnerINLINE() == pOldCity->getOwnerINLINE())	
						{
							if (pLoopPlot->getNumCultureRangeCities(pOldCity->getOwnerINLINE()) == 1)
							{
								bool bForceUnowned = false;

								for (iI = 0; iI < MAX_PLAYERS; iI++)
								{
									if (GET_PLAYER((PlayerTypes)iI).isAlive())
									{
										if ((GET_PLAYER((PlayerTypes)iI).getTeam() != getTeam()) && (GET_PLAYER((PlayerTypes)iI).getTeam() != pOldCity->getTeam()))
										{
											if (pLoopPlot->getNumCultureRangeCities((PlayerTypes)iI) > 0)
											{
												bForceUnowned = true;
												break;
											}
										}
									}
								}

								if (bForceUnowned)
								{
									pLoopPlot->setForceUnownedTimer(GC.getDefineINT("FORCE_UNOWNED_CITY_TIMER"));
								}
							}
						}
					}
				}
			}
		}
	}

	if (pOldCity->getOriginalOwner() == pOldCity->getOwnerINLINE())
	{
		GET_PLAYER(pOldCity->getOriginalOwner()).changeCitiesLost(1);
	}
	else if (pOldCity->getOriginalOwner() == getID())
	{
		GET_PLAYER(pOldCity->getOriginalOwner()).changeCitiesLost(-1);
	}

	if (bConquest)
	{
		szBuffer = gDLL->getText("TXT_KEY_MISC_CAPTURED_CITY", pOldCity->getNameKey()).GetCString();
		gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYCAPTURE", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pOldCity->getX_INLINE(), pOldCity->getY_INLINE(), true, true);

		szName.Format(L"%s (%s)", pOldCity->getName().GetCString(), GET_PLAYER(pOldCity->getOwnerINLINE()).getReplayName());

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (iI != getID())
				{
					if (pOldCity->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
					{
						szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_CAPTURED_BY", szName.GetCString(), getCivilizationDescriptionKey());
						gDLL->getInterfaceIFace()->addHumanMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYCAPTURED", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pOldCity->getX_INLINE(), pOldCity->getY_INLINE(), true, true);
					}
				}
			}
		}

		szBuffer = gDLL->getText("TXT_KEY_MISC_CITY_WAS_CAPTURED_BY", szName.GetCString(), getCivilizationDescriptionKey());
		GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer, pOldCity->getX_INLINE(), pOldCity->getY_INLINE(), (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
	}

	iCaptureGold = 0;

	if (bConquest)
	{
		long lCaptureGold;
		// Use python to determine city capture gold amounts...
		lCaptureGold = 0;

		CyCity* pyOldCity = new CyCity(pOldCity);

		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyOldCity));	// pass in plot class
		
		gDLL->getPythonIFace()->callFunction(PYGameModule, "doCityCaptureGold", argsList.makeFunctionArgs(),&lCaptureGold);

		delete pyOldCity;	// python fxn must not hold on to this pointer 

		iCaptureGold = (int)lCaptureGold;

		// Civ4 Reimagined: Celtic UP
		iCaptureGold *= 100 + getLootCityModifier();
		iCaptureGold /= 100;
	}

	changeGold(iCaptureGold);

	pabHasReligion = new bool[GC.getNumReligionInfos()];
	pabHolyCity = new bool[GC.getNumReligionInfos()];
	pabHasCorporation = new bool[GC.getNumCorporationInfos()];
	pabHeadquarters = new bool[GC.getNumCorporationInfos()];
	paiNumRealBuilding = new int[GC.getNumBuildingInfos()];
	paiBuildingOriginalOwner = new int[GC.getNumBuildingInfos()];
	paiBuildingOriginalTime = new int[GC.getNumBuildingInfos()];

	for (iI = 0; iI < GC.getNumVoteSourceInfos(); ++iI)
	{
		pOldCity->processVoteSourceBonus((VoteSourceTypes)iI, false);
	}

	eOldOwner = pOldCity->getOwnerINLINE();
	eOriginalOwner = pOldCity->getOriginalOwner();
	eHighestCulturePlayer = pOldCity->findHighestCulture();
	eOldHighestCulturePlayer = pOldCity->findHighestCulture();
	iGameTurnFounded = pOldCity->getGameTurnFounded();
	iPopulation = pOldCity->getPopulation();
	iHighestPopulation = pOldCity->getHighestPopulation();
	iHurryAngerTimer = pOldCity->getHurryAngerTimer();
	iConscriptAngerTimer = pOldCity->getConscriptAngerTimer();
	iDefyResolutionAngerTimer = pOldCity->getDefyResolutionAngerTimer();
	iOccupationTimer = pOldCity->getOccupationTimer();
	szName = pOldCity->getNameKey();
	iDamage = pOldCity->getDefenseDamage();
	int iOldCityId = pOldCity->getID();

	bool bOldCapitalBonus = false;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		abEverOwned[iI] = pOldCity->isEverOwned((PlayerTypes)iI);
		aiCulture[iI] = pOldCity->getCultureTimes100((PlayerTypes)iI);
	}

	bOldEverOwned = abEverOwned[getID()]; // Civ4 Reimagined
	abEverOwned[getID()] = true;
	

	for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		pabHasReligion[iI] = pOldCity->isHasReligion((ReligionTypes)iI);
		pabHolyCity[iI] = pOldCity->isHolyCity((ReligionTypes)iI);
	}

	for (iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		pabHasCorporation[iI] = pOldCity->isHasCorporation((CorporationTypes)iI);
		pabHeadquarters[iI] = pOldCity->isHeadquarters((CorporationTypes)iI);
	}

	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		paiNumRealBuilding[iI] = pOldCity->getNumRealBuilding((BuildingTypes)iI);
		paiBuildingOriginalOwner[iI] = pOldCity->getBuildingOriginalOwner((BuildingTypes)iI);
		paiBuildingOriginalTime[iI] = pOldCity->getBuildingOriginalTime((BuildingTypes)iI);
	}

	std::vector<BuildingYieldChange> aBuildingYieldChange;
	std::vector<BuildingCommerceChange> aBuildingCommerceChange;
	BuildingChangeArray aBuildingHappyChange;
	BuildingChangeArray aBuildingHealthChange;
	for (iI = 0; iI < GC.getNumBuildingClassInfos(); ++iI)
	{
		for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
		{
			BuildingYieldChange kChange;
			kChange.eBuildingClass = (BuildingClassTypes)iI;
			kChange.eYield = (YieldTypes)iYield;
			kChange.iChange = pOldCity->getBuildingYieldChange((BuildingClassTypes)iI, (YieldTypes)iYield);
			if (0 != kChange.iChange)
			{
				aBuildingYieldChange.push_back(kChange);
			}
		}

		for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
		{
			BuildingCommerceChange kChange;
			kChange.eBuildingClass = (BuildingClassTypes)iI;
			kChange.eCommerce = (CommerceTypes)iCommerce;
			kChange.iChange = pOldCity->getBuildingCommerceChange((BuildingClassTypes)iI, (CommerceTypes)iCommerce);
			if (0 != kChange.iChange)
			{
				aBuildingCommerceChange.push_back(kChange);
			}
		}

		int iChange = pOldCity->getBuildingHappyChange((BuildingClassTypes)iI);
		if (0 != iChange)
		{
			aBuildingHappyChange.push_back(std::make_pair((BuildingClassTypes)iI, iChange));
		}

		iChange = pOldCity->getBuildingHealthChange((BuildingClassTypes)iI);
		if (0 != iChange)
		{
			aBuildingHealthChange.push_back(std::make_pair((BuildingClassTypes)iI, iChange));
		}
	}

	bRecapture = ((eHighestCulturePlayer != NO_PLAYER) ? (GET_PLAYER(eHighestCulturePlayer).getTeam() == getTeam()) : false);

	// Civ4 Reimagined: Slaves on city conquest / raze
	if (bConquest && !bOldEverOwned && hasSlavery())
	{
		int iSlavePoints = std::max(0, iPopulation - getCurrentEra() * GC.getDefineINT("CONQUEST_LESS_SLAVE_POINTS_PER_ERA"));
		gainSlavePoints(iSlavePoints, pOldCity);
	}
	
	pOldCity->kill(false);

	if (bTrade)
	{
		for (iDX = -1; iDX <= 1; iDX++)
		{
			for (iDY = -1; iDY <= 1; iDY++)
			{
				pLoopPlot = plotXY(pCityPlot->getX_INLINE(), pCityPlot->getY_INLINE(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					pLoopPlot->setCulture(eOldOwner, 0, false, false);
				}
			}
		}
	}

	pNewCity = initCity(pCityPlot->getX_INLINE(), pCityPlot->getY_INLINE(), !bConquest, false);

	FAssertMsg(pNewCity != NULL, "NewCity is not assigned a valid value");

	pNewCity->setPreviousOwner(eOldOwner);
	pNewCity->setOriginalOwner(eOriginalOwner);
	pNewCity->setGameTurnFounded(iGameTurnFounded);
	pNewCity->setPopulation((bConquest && !bRecapture) ? std::max(1, (iPopulation - 1)) : iPopulation);
	pNewCity->setHighestPopulation(iHighestPopulation);
	pNewCity->setName(szName);
	pNewCity->setNeverLost(false);
	pNewCity->changeDefenseDamage(iDamage);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pNewCity->setEverOwned(((PlayerTypes)iI), abEverOwned[iI]);
		pNewCity->setCultureTimes100(((PlayerTypes)iI), aiCulture[iI], false, false);
	}
	
	for (iI = 0; iI < GC.getNumBuildingInfos(); iI++)
	{
		int iNum = 0;

		if (paiNumRealBuilding[iI] > 0)
		{
			BuildingClassTypes eBuildingClass = (BuildingClassTypes)GC.getBuildingInfo((BuildingTypes)iI).getBuildingClassType();
			if (::isWorldWonderClass(eBuildingClass))
			{
				eBuilding = (BuildingTypes)iI;
			}
			else
			{
				eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass);
			}

			if (eBuilding != NO_BUILDING)
			{
				if (bTrade || !(GC.getBuildingInfo((BuildingTypes)iI).isNeverCapture()))
				{
					if (!isProductionMaxedBuildingClass(((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType())), true))
					{
						// Civ4 Reimagined: Always destroy national wonders
						if (!::isNationalWonderClass(eBuildingClass) || ::isWorldWonderClass(eBuildingClass) || ::isTeamWonderClass(eBuildingClass))
						{
							if (pNewCity->isValidBuildingLocation(eBuilding))
							{
								if (!bConquest || bRecapture || GC.getGameINLINE().getSorenRandNum(100, "Capture Probability") < GC.getBuildingInfo((BuildingTypes)iI).getConquestProbability())
								{
									iNum += paiNumRealBuilding[iI];
								}
							}
						}
					}
				}

				pNewCity->setNumRealBuildingTimed(eBuilding, std::min(pNewCity->getNumRealBuilding(eBuilding) + iNum, GC.getCITY_MAX_NUM_BUILDINGS()), false, ((PlayerTypes)(paiBuildingOriginalOwner[iI])), paiBuildingOriginalTime[iI]);

			}
		}
	}

	for (std::vector<BuildingYieldChange>::iterator it = aBuildingYieldChange.begin(); it != aBuildingYieldChange.end(); ++it)
	{
		pNewCity->setBuildingYieldChange((*it).eBuildingClass, (*it).eYield, (*it).iChange);
	}

	for (std::vector<BuildingCommerceChange>::iterator it = aBuildingCommerceChange.begin(); it != aBuildingCommerceChange.end(); ++it)
	{
		pNewCity->setBuildingCommerceChange((*it).eBuildingClass, (*it).eCommerce, (*it).iChange);
	}

	for (BuildingChangeArray::iterator it = aBuildingHappyChange.begin(); it != aBuildingHappyChange.end(); ++it)
	{
		pNewCity->setBuildingHappyChange((*it).first, (*it).second);
	}

	for (BuildingChangeArray::iterator it = aBuildingHealthChange.begin(); it != aBuildingHealthChange.end(); ++it)
	{
		pNewCity->setBuildingHealthChange((*it).first, (*it).second);
	}

	for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (pabHasReligion[iI])
		{
			pNewCity->setHasReligion(((ReligionTypes)iI), true, false, true);
		}

		if (pabHolyCity[iI])
		{
			GC.getGameINLINE().setHolyCity(((ReligionTypes)iI), pNewCity, false);
		}
	}

	for (iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (pabHasCorporation[iI])
		{
			pNewCity->setHasCorporation(((CorporationTypes)iI), true, false);
		}

		if (pabHeadquarters[iI])
		{
			GC.getGameINLINE().setHeadquarters(((CorporationTypes)iI), pNewCity, false);
		}
	}

	if (bTrade)
	{
		if (isHuman() || (getTeam() == GET_PLAYER(eOldOwner).getTeam()))
		{
			pNewCity->changeHurryAngerTimer(iHurryAngerTimer);
			pNewCity->changeConscriptAngerTimer(iConscriptAngerTimer);
			pNewCity->changeDefyResolutionAngerTimer(iDefyResolutionAngerTimer);
		}

		if (!bRecapture)
		{
			if (iOccupationTimer > 0)
			{
				pNewCity->changeOccupationTimer(std::max(1, (iOccupationTimer * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getOccupyLengthPercent()) / 100)); // Civ4 Reimagined: Added gamespeed modifier
			}			
		}
	}
	

	if (bConquest)
	{
		iTeamCulturePercent = pNewCity->calculateTeamCulturePercent(getTeam());

		if (iTeamCulturePercent < GC.getDefineINT("OCCUPATION_CULTURE_PERCENT_THRESHOLD") && !GET_TEAM(getTeam()).isNoConquestResistance() && !isNoCityResistance() && !isConscriptInfidels())
		{
			int iOccupationTurns = ((GC.getDefineINT("BASE_OCCUPATION_TURNS") + ((pNewCity->getPopulation() * GC.getDefineINT("OCCUPATION_TURNS_POPULATION_PERCENT")) / 100)) * (100 - iTeamCulturePercent)) / 100;
			 // Civ4 Reimagined: Added game speed modifier
			iOccupationTurns *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getOccupyLengthPercent();
			iOccupationTurns /= 100;
			iOccupationTurns = std::max(1, iOccupationTurns);

			pNewCity->changeOccupationTimer(iOccupationTurns);
		}

		// Civ4 Reimagined
		if (isNoCityResistance() || GET_TEAM(getTeam()).isNoConquestResistance())
		{
			for (int iJ = 0; iJ < NUM_DIRECTION_TYPES; ++iJ)
			{
				CvPlot* pAdjacentPlot = plotDirection(pNewCity->getX_INLINE(), pNewCity->getY_INLINE(), ((DirectionTypes)iJ));

				if (pAdjacentPlot != NULL)
				{
					for (int iK = 0; iK < MAX_PLAYERS; ++iK)
					{
						if (iK != getID())
						{
							pAdjacentPlot->setCulture((PlayerTypes)iK, 0, false, false);
						}
					}
					pAdjacentPlot->updateCulture(false, false);
				}
			}		
		}

		// Civ4 Reimagined: Mongol UP
		if (getEspionagePointsOnConquestPerPopulation() > 0)
		{
			int iEspionagePointsOnConquest = getEspionagePointsOnConquestPerPopulation() * pNewCity->getPopulation();
			iEspionagePointsOnConquest *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getResearchPercent();
			iEspionagePointsOnConquest /= 100;

			GET_TEAM(getTeam()).changeEspionagePointsEver(iEspionagePointsOnConquest);
			GET_TEAM(getTeam()).changeEspionagePointsAgainstTeam(GET_PLAYER(eOldOwner).getTeam(), iEspionagePointsOnConquest);

			if (iEspionagePointsOnConquest > 0)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_PILLAGED_ESPIONAGE_CITY", iEspionagePointsOnConquest, pNewCity->getNameKey());
				gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYRAZE", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pNewCity->getX_INLINE(), pNewCity->getY_INLINE(), true, true);
			}
		}

		GC.getMapINLINE().verifyUnitValidPlot();
	}

	// Civ4 Reimagined: Unique power for Arabia
	if (isHasFaithConquest() && bConquest)
	{
		// Spread religion
		ReligionTypes eOwnStateReligion = getStateReligion();
		if (!pNewCity->isHasReligion(eOwnStateReligion))
		{
			pNewCity->setHasReligion(eOwnStateReligion, true, true, false);
		}

		// Build temples and monasteries
		for (int iI = 0; iI < GC.getNumBuildingInfos(); iI++)
		{
			const CvBuildingInfo& kBuilding = GC.getBuildingInfo((BuildingTypes)iI);

			if (kBuilding.getSpecialBuildingType() == (SpecialBuildingTypes)GC.getInfoTypeForString("SPECIALBUILDING_TEMPLE") 
		     || kBuilding.getSpecialBuildingType() == (SpecialBuildingTypes)GC.getInfoTypeForString("SPECIALBUILDING_MONASTERY"))
			{
				if (kBuilding.getReligionType() == eOwnStateReligion)
				{
					if (pNewCity->canConstruct((BuildingTypes)iI, false, false, true))
					{
						pNewCity->setNumRealBuilding((BuildingTypes)iI, 1);
					}
				}
			}
		}
	}

	pCityPlot->setRevealed(GET_PLAYER(eOldOwner).getTeam(), true, false, NO_TEAM, false);

	pNewCity->updateEspionageVisibility(false);

	if (bUpdatePlotGroups)
	{
		GC.getGameINLINE().updatePlotGroups();
	}

	CvEventReporter::getInstance().cityAcquired(eOldOwner, getID(), pNewCity, bConquest, bTrade);

	if( gPlayerLogLevel >= 1 )
	{
		logBBAI("  Player %d (%S) acquires city %S bConq %d bTrade %d", getID(), getCivilizationDescription(0), pNewCity->getName(0).GetCString(), bConquest, bTrade );
	}

	SAFE_DELETE_ARRAY(pabHasReligion);
	SAFE_DELETE_ARRAY(pabHolyCity);
	SAFE_DELETE_ARRAY(pabHasCorporation);
	SAFE_DELETE_ARRAY(pabHeadquarters);
	SAFE_DELETE_ARRAY(paiNumRealBuilding);
	SAFE_DELETE_ARRAY(paiBuildingOriginalOwner);
	SAFE_DELETE_ARRAY(paiBuildingOriginalTime);

	if (bConquest)
	{
		CyCity* pyCity = new CyCity(pNewCity);
		CyArgsList argsList;
		argsList.add(getID());	// Player ID
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "canRazeCity", argsList.makeFunctionArgs(), &lResult);
		delete pyCity;	// python fxn must not hold on to this pointer 

		if (lResult == 1)
		{
			//auto raze based on game rules
			if (pNewCity->isAutoRaze())
			{
				if (iCaptureGold > 0)
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_PILLAGED_CITY", iCaptureGold, pNewCity->getNameKey());
					gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYRAZE", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pNewCity->getX_INLINE(), pNewCity->getY_INLINE(), true, true);
				}

				pNewCity->doTask(TASK_RAZE);
			}
			else if (!isHuman())
			{
				AI_conquerCity(pNewCity); // could delete the pointer...
			}
			else
			{
				//popup raze option
				eHighestCulturePlayer = pNewCity->getLiberationPlayer(true);
				bRaze = canRaze(pNewCity);
				bGift = ((eHighestCulturePlayer != NO_PLAYER) 
						&& (eHighestCulturePlayer != getID()) 
						&& ((getTeam() == GET_PLAYER(eHighestCulturePlayer).getTeam()) 
							|| GET_TEAM(getTeam()).isOpenBorders(GET_PLAYER(eHighestCulturePlayer).getTeam()) 
							|| GET_TEAM(GET_PLAYER(eHighestCulturePlayer).getTeam()).isVassal(getTeam())));

				if (bRaze || bGift)
				{
					CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_RAZECITY);
					pInfo->setData1(pNewCity->getID());
					pInfo->setData2(eHighestCulturePlayer);
					pInfo->setData3(iCaptureGold);
					gDLL->getInterfaceIFace()->addPopup(pInfo, getID());
				}
				else
				{
					/* original bts code
					pNewCity->chooseProduction();
					CvEventReporter::getInstance().cityAcquiredAndKept(getID(), pNewCity); */
					lResult = 0; // K-Mod. (signal that we cannot raze the city.)
				}
			}
		}
		// K-Mod. properly handle the case where python says we can't raze the city
		// (note. this isn't in an 'else' because I overwrite lResult in the case above.)
		if (lResult == 0)
		{
			CvEventReporter::getInstance().cityAcquiredAndKept(getID(), pNewCity);

			if (isHuman())
				pNewCity->chooseProduction();
		}
		// K-Mod end
	}
	else if (!bTrade)
	{
		if (isHuman())
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_DISBANDCITY);
			pInfo->setData1(pNewCity->getID());
			gDLL->getInterfaceIFace()->addPopup(pInfo, getID());
		}
		else
		{
			CvEventReporter::getInstance().cityAcquiredAndKept(getID(), pNewCity);
		}
	}

	// Forcing events that deal with the old city not to expire just because we conquered that city
	for (CvEventMap::iterator it = m_mapEventsOccured.begin(); it != m_mapEventsOccured.end(); ++it)
	{
		EventTriggeredData &triggerData = it->second;
		if((triggerData.m_eOtherPlayer == eOldOwner) && (triggerData.m_iOtherPlayerCityId == iOldCityId))
		{
			triggerData.m_iOtherPlayerCityId = -1;
		}
	}
}


void CvPlayer::killCities()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->kill(false);
	}

	GC.getGameINLINE().updatePlotGroups();
}


CvWString CvPlayer::getNewCityName() const
{
	CLLNode<CvWString>* pNode;
	CvWString szName;
	int iI;

	/* original bts code
	for (pNode = headCityNameNode(); (pNode != NULL); pNode = nextCityNameNode(pNode))
	{
		szName = gDLL->getText(pNode->m_data);
		if (isCityNameValid(szName, true))
		{
			szName = pNode->m_data;
			break;
		}
	} */
	// K-Mod
	for (pNode = headCityNameNode(); pNode && szName.empty(); pNode = nextCityNameNode(pNode))
	{
		szName = gDLL->getText(pNode->m_data); // (temp use of the buffer)

		if (isCityNameValid(szName, true))
			szName = pNode->m_data;
		else
			szName.clear(); // clear the buffer if the name is not valid!
	}
	// Note: unfortunately, the name-skipping system in getCivilizationCityName does not apply here.
	// K-Mod end

	if (szName.empty())
	{
		getCivilizationCityName(szName, getCivilizationType());
	}

	if (szName.empty())
	{
		// Pick a name from another random civ
		int iRandOffset = GC.getGameINLINE().getSorenRandNum(GC.getNumCivilizationInfos(), "Place Units (Player)");
		for (iI = 0; iI < GC.getNumCivilizationInfos(); iI++)
		{
			int iLoopName = ((iI + iRandOffset) % GC.getNumCivilizationInfos());

			getCivilizationCityName(szName, ((CivilizationTypes)iLoopName));

			if (!szName.empty())
			{
				break;
			}
		}
	}

	if (szName.empty())
	{
		szName = "TXT_KEY_CITY";
	}

	return szName;
}


void CvPlayer::getCivilizationCityName(CvWString& szBuffer, CivilizationTypes eCivilization) const
{
	int iRandOffset;
	int iLoopName;
	int iI;

	/* original bts code
	if (isBarbarian() || isMinorCiv())
	{
		iRandOffset = GC.getGameINLINE().getSorenRandNum(GC.getCivilizationInfo(eCivilization).getNumCityNames(), "Place Units (Player)");
	}
	else
	{
		iRandOffset = 0;
	} */
	// K-Mod
	if (eCivilization != getCivilizationType() || isBarbarian() || isMinorCiv())
		iRandOffset = GC.getGameINLINE().getSorenRandNum(GC.getCivilizationInfo(eCivilization).getNumCityNames(), "City name offset");
	else
		iRandOffset = std::max(0, getPlayerRecord()->getNumCitiesBuilt() - getNumCityNames()); // note: the explicit city names list is checked before this function is called.
	// K-Mod end

	for (iI = 0; iI < GC.getCivilizationInfo(eCivilization).getNumCityNames(); iI++)
	{
		iLoopName = ((iI + iRandOffset) % GC.getCivilizationInfo(eCivilization).getNumCityNames());

		CvWString szName = gDLL->getText(GC.getCivilizationInfo(eCivilization).getCityNames(iLoopName));

		if (isCityNameValid(szName, true))
		{
			szBuffer = GC.getCivilizationInfo(eCivilization).getCityNames(iLoopName);
			break;
		}
	}
}


bool CvPlayer::isCityNameValid(CvWString& szName, bool bTestDestroyed) const
{
	CvCity* pLoopCity;
	int iLoop;

	if (bTestDestroyed)
	{
		if (GC.getGameINLINE().isDestroyedCityName(szName))
		{
			return false;
		}

		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			for (pLoopCity = kLoopPlayer.firstCity(&iLoop); pLoopCity != NULL; pLoopCity = kLoopPlayer.nextCity(&iLoop))
			{
				if (pLoopCity->getName() == szName)
				{
					return false;
				}
			}
		}
	}
	else
	{
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pLoopCity->getName() == szName)
			{
				return false;
			}
		}
	}

	return true;
}


CvUnit* CvPlayer::initUnit(UnitTypes eUnit, int iX, int iY, UnitAITypes eUnitAI, DirectionTypes eFacingDirection)
{
	PROFILE_FUNC();

	FAssertMsg(eUnit != NO_UNIT, "Unit is not assigned a valid value");

	CvUnit* pUnit = addUnit();
	FAssertMsg(pUnit != NULL, "Unit is not assigned a valid value");
	if (NULL != pUnit)
	{
		pUnit->init(pUnit->getID(), eUnit, ((eUnitAI == NO_UNITAI) ? ((UnitAITypes)(GC.getUnitInfo(eUnit).getDefaultUnitAIType())) : eUnitAI), getID(), iX, iY, eFacingDirection);
	}

	return pUnit;
}


void CvPlayer::initFreeWorkers(int iChange)
{
	CvCity* pCapitalCity = getCapitalCity();
	UnitTypes eUnit = (UnitTypes)GC.getInfoTypeForString("UNIT_WORKER");

	if (!pCapitalCity)
	{
		return;
	}

	for (int iI = 0; iI < iChange; ++iI)
	{
		CvUnit* pUnit = initUnit(eUnit, pCapitalCity->getX_INLINE(), pCapitalCity->getY_INLINE());
		pUnit->setIsFreeWorker(true);
	}
}


void CvPlayer::disbandUnit(bool bAnnounce)
{
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit;
	wchar szBuffer[1024];
	int iValue;
	int ipBestValue;
	int iLoop;

	ipBestValue = MAX_INT;
	pBestUnit = NULL;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (!(pLoopUnit->hasCargo()))
		{
			if (!(pLoopUnit->isGoldenAge()))
			{
				if (pLoopUnit->getUnitInfo().getProductionCost() > 0)
				{
					if (!(pLoopUnit->isMilitaryHappiness()) || !(pLoopUnit->plot()->isCity()) || (pLoopUnit->plot()->plotCount(PUF_isMilitaryHappiness, -1, -1, getID()) > 1))
					{
						iValue = (10000 + GC.getGameINLINE().getSorenRandNum(1000, "Disband Unit"));

						iValue += (pLoopUnit->getUnitInfo().getProductionCost() * 5);

						iValue += (pLoopUnit->getExperience() * 20);
						iValue += (pLoopUnit->getLevel() * 100);

						if (pLoopUnit->canDefend() && pLoopUnit->plot()->isCity())
						{
							iValue *= 2;
						}

						if (pLoopUnit->plot()->getTeam() == pLoopUnit->getTeam())
						{
							iValue *= 3;
						}

						switch (pLoopUnit->AI_getUnitAIType())
						{
						case UNITAI_UNKNOWN:
						case UNITAI_ANIMAL:
							break;

						case UNITAI_SETTLE:
							iValue *= 20;
							break;

						case UNITAI_WORKER:
							iValue *= 10;
							break;

						case UNITAI_ATTACK:
						case UNITAI_ATTACK_CITY:
						case UNITAI_COLLATERAL:
						case UNITAI_PILLAGE:
						case UNITAI_RESERVE:
						case UNITAI_COUNTER:
							iValue *= 2;
							break;

						case UNITAI_CITY_DEFENSE:
						case UNITAI_CITY_COUNTER:
						case UNITAI_CITY_SPECIAL:
						case UNITAI_PARADROP:
							iValue *= 6;
							break;

						case UNITAI_EXPLORE:
							iValue *= 15;
							break;

						case UNITAI_MISSIONARY:
							iValue *= 8;
							break;

						case UNITAI_PROPHET:
						case UNITAI_ARTIST:
						case UNITAI_SCIENTIST:
						case UNITAI_GENERAL:
						case UNITAI_MERCHANT:
						case UNITAI_ENGINEER:
						case UNITAI_GREAT_SPY: // K-Mod
							break;

						case UNITAI_SPY:
							iValue *= 12;
							break;

						case UNITAI_ICBM:
							iValue *= 4;
							break;

						case UNITAI_WORKER_SEA:
							iValue *= 18;
							break;

						case UNITAI_ATTACK_SEA:
						case UNITAI_RESERVE_SEA:
						case UNITAI_ESCORT_SEA:
							break;

						case UNITAI_EXPLORE_SEA:
							iValue *= 25;
							break;

						case UNITAI_ASSAULT_SEA:
						case UNITAI_SETTLER_SEA:
						case UNITAI_MISSIONARY_SEA:
						case UNITAI_SPY_SEA:
						case UNITAI_CARRIER_SEA:
						case UNITAI_MISSILE_CARRIER_SEA:
							iValue *= 5;
							break;

						case UNITAI_PIRATE_SEA:
						case UNITAI_ATTACK_AIR:
							break;

						case UNITAI_DEFENSE_AIR:
						case UNITAI_CARRIER_AIR:
						case UNITAI_MISSILE_AIR:
							iValue *= 3;
							break;

						default:
							FAssert(false);
							break;
						}

						if (pLoopUnit->getUnitInfo().getExtraCost() > 0)
						{
							iValue /= (pLoopUnit->getUnitInfo().getExtraCost() + 1);
						}

						if (iValue < ipBestValue)
						{
							ipBestValue = iValue;
							pBestUnit = pLoopUnit;
						}
					}
				}
			}
		}
	}

	if (pBestUnit != NULL)
	{
		swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_UNIT_DISBANDED_NO_MONEY", pBestUnit->getNameKey()).GetCString());
		gDLL->getInterfaceIFace()->addHumanMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_UNITDISBANDED", MESSAGE_TYPE_MINOR_EVENT, pBestUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pBestUnit->getX_INLINE(), pBestUnit->getY_INLINE(), true, true);

		FAssert(!(pBestUnit->isGoldenAge()));

		pBestUnit->kill(false);
	}
}


void CvPlayer::killUnits()
{
	CvUnit* pLoopUnit;
	int iLoop;

	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->kill(false);
	}
}


// XXX should pUnit be a CvSelectionGroup???
// Returns the next unit in the cycle...
CvSelectionGroup* CvPlayer::cycleSelectionGroups(CvUnit* pUnit, bool bForward, bool bWorkers, bool* pbWrap)
{
	CLLNode<int>* pSelectionGroupNode;
	CLLNode<int>* pFirstSelectionGroupNode;
	CvSelectionGroup* pLoopSelectionGroup;

	// K-Mod
	FAssert(GC.getGameINLINE().getActivePlayer() == getID());
	bool bDummy;
	bool &bWrap = pbWrap ? *pbWrap : bDummy; // this means we can just use bWrap directly and it will update *pbWrap if need be.
	std::set<int>& cycled_groups = GC.getGameINLINE().m_ActivePlayerCycledGroups; // for convenience
	// K-Mod end

	/* if (pbWrap != NULL)
	{
		*pbWrap = false;
	} */
	bWrap = false;

	pSelectionGroupNode = headGroupCycleNode();

	if (pUnit != NULL)
	{
		while (pSelectionGroupNode != NULL)
		{
			if (getSelectionGroup(pSelectionGroupNode->m_data) == pUnit->getGroup())
			{
				// K-Mod
				if (isTurnActive())
					cycled_groups.insert(pSelectionGroupNode->m_data);
				//
				if (bForward)
				{
					pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
				}
				else
				{
					pSelectionGroupNode = previousGroupCycleNode(pSelectionGroupNode);
				}
				break;
			}

			pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
		}
	}

	if (pSelectionGroupNode == NULL)
	{
		if (bForward)
		{
			pSelectionGroupNode = headGroupCycleNode();
		}
		else
		{
			pSelectionGroupNode = tailGroupCycleNode();
		}

		/* if (pbWrap != NULL)
		{
			*pbWrap = true;
		} */ // disabled by K-Mod
	}

	if (pSelectionGroupNode != NULL)
	{
		pFirstSelectionGroupNode = pSelectionGroupNode;

		while (true)
		{
			pLoopSelectionGroup = getSelectionGroup(pSelectionGroupNode->m_data);
			FAssertMsg(pLoopSelectionGroup != NULL, "LoopSelectionGroup is not assigned a valid value");

			//if (pLoopSelectionGroup->readyToSelect())
			if (pLoopSelectionGroup->readyToSelect() && cycled_groups.count(pSelectionGroupNode->m_data) == 0) // K-Mod
			{
				if (!bWorkers || pLoopSelectionGroup->hasWorker())
				{
					/* if (pUnit && pLoopSelectionGroup == pUnit->getGroup())
					{
						if (pbWrap != NULL)
						{
							*pbWrap = true;
						}
					} */

					return pLoopSelectionGroup;
				}
			}

			if (bForward)
			{
				pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);

				if (pSelectionGroupNode == NULL)
				{
					pSelectionGroupNode = headGroupCycleNode();

					/* if (pbWrap != NULL)
					{
						*pbWrap = true;
					} */
				}
			}
			else
			{
				pSelectionGroupNode = previousGroupCycleNode(pSelectionGroupNode);

				if (pSelectionGroupNode == NULL)
				{
					pSelectionGroupNode = tailGroupCycleNode();

					/* if (pbWrap != NULL)
					{
						*pbWrap = true;
					} */
				}
			}

			if (pSelectionGroupNode == pFirstSelectionGroupNode)
			{
				// break;
				// K-Mod
				if (bWrap)
					break;
				else
				{
					cycled_groups.clear();
					bWrap = true;
				}
				// K-Mod end
			}
		} //
	}

	return NULL;
}


bool CvPlayer::hasTrait(TraitTypes eTrait) const
{
	FAssertMsg((getLeaderType() >= 0), "getLeaderType() is less than zero");
	FAssertMsg((eTrait >= 0), "eTrait is less than zero");
	return GC.getLeaderHeadInfo(getLeaderType()).hasTrait(eTrait);
}

/************************************************************************************************/
/* AI_AUTO_PLAY_MOD                           07/09/08                            jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
void CvPlayer::setHumanDisabled( bool newVal )
{
	m_bDisableHuman = newVal;
	updateHuman();
}
bool CvPlayer::isHumanDisabled( )
{
	return m_bDisableHuman;
}
/************************************************************************************************/
/* AI_AUTO_PLAY_MOD                            END                                              */
/************************************************************************************************/

bool CvPlayer::isHuman() const
{
	return m_bHuman;
}

void CvPlayer::updateHuman()
{
	if (getID() == NO_PLAYER)
	{
		m_bHuman = false;
	}
	else
	{
/************************************************************************************************/
/* AI_AUTO_PLAY_MOD                            09/01/07                        MRGENIE          */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
/*
		m_bHuman = GC.getInitCore().getHuman(getID());
*/
		if( m_bDisableHuman )
		{
			m_bHuman = false;
		}
		else
		{
			m_bHuman = GC.getInitCore().getHuman(getID());
		}
/************************************************************************************************/
/* AI_AUTO_PLAY_MOD                            END                                              */
/************************************************************************************************/	
	}
}

bool CvPlayer::isBarbarian() const
{
	return (getID() == BARBARIAN_PLAYER);
}

static bool concealUnknownCivs() { return GC.getGameINLINE().getActiveTeam() != NO_TEAM && gDLL->getChtLvl() == 0 && !gDLL->GetWorldBuilderMode(); } // K-Mod

const wchar* CvPlayer::getName(uint uiForm) const
{
	if (GC.getInitCore().getLeaderName(getID(), uiForm).empty() || (GC.getGameINLINE().isMPOption(MPOPTION_ANONYMOUS) && isAlive() && GC.getGameINLINE().getGameState() == GAMESTATE_ON))
	{
		//return GC.getLeaderHeadInfo(getLeaderType()).getDescription(uiForm);
		// K-Mod. Conceal the leader name of unmet players.
		if (!concealUnknownCivs() || GET_TEAM(GC.getGameINLINE().getActiveTeam()).isHasSeen(getTeam()))
			return GC.getLeaderHeadInfo(getLeaderType()).getDescription(uiForm);
		else
		{
			static CvWString string = gDLL->getText("TXT_KEY_UNKNOWN"); // hack to stop the string from going out of scope.
			return string;
		}
		// K-Mod end
	}
	else
	{
		return GC.getInitCore().getLeaderName(getID(), uiForm);
	}
}

// K-Mod. Player name to be used in replay
const wchar* CvPlayer::getReplayName(uint uiForm) const
{
	if (GC.getInitCore().getLeaderName(getID(), uiForm).empty() || (GC.getGameINLINE().isMPOption(MPOPTION_ANONYMOUS) && isAlive() && GC.getGameINLINE().getGameState() == GAMESTATE_ON))
	{
		return GC.getLeaderHeadInfo(getLeaderType()).getDescription(uiForm);
	}
	else
	{
		return GC.getInitCore().getLeaderName(getID(), uiForm);
	}
}
// K-Mod end

const wchar* CvPlayer::getNameKey() const
{
	if (GC.getInitCore().getLeaderNameKey(getID()).empty() || (GC.getGameINLINE().isMPOption(MPOPTION_ANONYMOUS) && isAlive()))
	{
		return GC.getLeaderHeadInfo(getLeaderType()).getTextKeyWide();
	}
	else
	{
		return GC.getInitCore().getLeaderNameKey(getID());
	}
}


const wchar* CvPlayer::getCivilizationDescription(uint uiForm) const
{
	// K-Mod. Conceal the civilization of unmet players.
	if (concealUnknownCivs() && !GET_TEAM(GC.getGameINLINE().getActiveTeam()).isHasSeen(getTeam()))
	{
		static CvWString string = gDLL->getText("TXT_KEY_TOPCIVS_UNKNOWN"); // hack to stop the string from going out of scope.
		return string;
	}
	// K-Mod end

	if (GC.getInitCore().getCivDescription(getID(), uiForm).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getDescription(uiForm);
	}
	else
	{
		return GC.getInitCore().getCivDescription(getID(), uiForm);
	}
}


const wchar* CvPlayer::getCivilizationDescriptionKey() const
{
	if (GC.getInitCore().getCivDescriptionKey(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getTextKeyWide();
	}
	else
	{
		return GC.getInitCore().getCivDescriptionKey(getID());
	}
}


const wchar* CvPlayer::getCivilizationShortDescription(uint uiForm) const
{
	// K-Mod. Conceal the civilization of unmet players.
	if (concealUnknownCivs() && !GET_TEAM(GC.getGameINLINE().getActiveTeam()).isHasSeen(getTeam()))
	{
		static CvWString string = gDLL->getText("TXT_KEY_UNKNOWN"); // hack to stop the string from going out of scope.
		return string;
	}
	// K-Mod end

	if (GC.getInitCore().getCivShortDesc(getID(), uiForm).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getShortDescription(uiForm);
	}
	else
	{
		return GC.getInitCore().getCivShortDesc(getID(), uiForm);
	}
}


const wchar* CvPlayer::getCivilizationShortDescriptionKey() const
{
	if (GC.getInitCore().getCivShortDescKey(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getShortDescriptionKey();
	}
	else
	{
		return GC.getInitCore().getCivShortDescKey(getID());
	}
}


const wchar* CvPlayer::getCivilizationAdjective(uint uiForm) const
{
	if (GC.getInitCore().getCivAdjective(getID(), uiForm).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getAdjective(uiForm);
	}
	else
	{
		return GC.getInitCore().getCivAdjective(getID(), uiForm);
	}
}

const wchar* CvPlayer::getCivilizationAdjectiveKey() const
{
	if (GC.getInitCore().getCivAdjectiveKey(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getAdjectiveKey();
	}
	else
	{
		return GC.getInitCore().getCivAdjectiveKey(getID());
	}
}


CvWString CvPlayer::getFlagDecal() const
{
	if (GC.getInitCore().getFlagDecal(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getFlagTexture();
	}
	else
	{
		return GC.getInitCore().getFlagDecal(getID());
	}
}

bool CvPlayer::isWhiteFlag() const
{
	if (GC.getInitCore().getFlagDecal(getID()).empty())
	{
		return GC.getCivilizationInfo(getCivilizationType()).getArtInfo()->isWhiteFlag();
	}
	else
	{
		return GC.getInitCore().getWhiteFlag(getID());
	}
}


const wchar* CvPlayer::getStateReligionName(uint uiForm) const
{
	return GC.getReligionInfo(getStateReligion()).getDescription(uiForm);
}

const wchar* CvPlayer::getStateReligionKey() const
{
	if (getStateReligion() != NO_RELIGION)
	{
		return GC.getReligionInfo(getStateReligion()).getTextKeyWide();
	}

	return L"TXT_KEY_MISC_NO_STATE_RELIGION";
}


const CvWString CvPlayer::getBestAttackUnitName(uint uiForm) const
{
	return gDLL->getObjectText((CvString)getBestAttackUnitKey(), uiForm, true);
}


const CvWString CvPlayer::getWorstEnemyName() const
{
	TeamTypes eWorstEnemy;

	eWorstEnemy = GET_TEAM(getTeam()).AI_getWorstEnemy();

	if (eWorstEnemy != NO_TEAM)
	{
		return GET_TEAM(eWorstEnemy).getName();
	}

	return "";
}

const wchar* CvPlayer::getBestAttackUnitKey() const
{
	CvCity* pCapitalCity;
	CvCity* pLoopCity;
	UnitTypes eBestUnit;
	int iLoop;

	eBestUnit = NO_UNIT;

	pCapitalCity = getCapitalCity();

	if (pCapitalCity != NULL)
	{
		eBestUnit = pCapitalCity->AI_bestUnitAI(UNITAI_ATTACK, true);
	}

	if (eBestUnit == NO_UNIT)
	{
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			eBestUnit = pLoopCity->AI_bestUnitAI(UNITAI_ATTACK, true);

			if (eBestUnit != NO_UNIT)
			{
				break;
			}
		}
	}

	if (eBestUnit != NO_UNIT)
	{
		return GC.getUnitInfo(eBestUnit).getTextKeyWide();
	}

	return L"TXT_KEY_MISC_NO_UNIT";
}


ArtStyleTypes CvPlayer::getArtStyleType() const
{
	if (GC.getInitCore().getArtStyle(getID()) == NO_ARTSTYLE)
	{
		return ((ArtStyleTypes)(GC.getCivilizationInfo(getCivilizationType()).getArtStyleType()));
	}
	else
	{
		return GC.getInitCore().getArtStyle(getID());
	}
}

const TCHAR* CvPlayer::getUnitButton(UnitTypes eUnit) const
{
	return GC.getUnitInfo(eUnit).getArtInfo(0, getCurrentEra(), (UnitArtStyleTypes) GC.getCivilizationInfo(getCivilizationType()).getUnitArtStyleType())->getButton();
}

void CvPlayer::doTurn()
{
	PROFILE_FUNC();

	CvCity* pLoopCity;
	int iLoop;

	FAssertMsg(isAlive(), "isAlive is expected to be true");
	FAssertMsg(!hasBusyUnit() || GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS)  || GC.getGameINLINE().isSimultaneousTeamTurns(), "End of turn with busy units in a sequential-turn game");

	CvEventReporter::getInstance().beginPlayerTurn( GC.getGameINLINE().getGameTurn(),  getID());

	doUpdateCacheOnTurn();

	GC.getGameINLINE().verifyDeals();

	AI_doTurnPre();

	if (getRevolutionTimer() > 0)
	{
		changeRevolutionTimer(-1);
	}

	if (getConversionTimer() > 0)
	{
		changeConversionTimer(-1);
	}

	setConscriptCount(0);

	AI_assignWorkingPlots();

	//if (0 == GET_TEAM(getTeam()).getHasMetCivCount(true) || GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
	if (isCommerceFlexible(COMMERCE_ESPIONAGE) && (GET_TEAM(getTeam()).getHasMetCivCount(true) == 0 || GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))) // K-Mod
	{
		setCommercePercent(COMMERCE_ESPIONAGE, 0); // (note: not forced)
	}

	verifyGoldCommercePercent();

	doGold();

	doResearch();

	doEspionagePoints();
	
	//Civ4 Reimagined: Don't update bonus values before city growth is completed for all cities.
	m_bUpdateBonusRatio = false;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->doTurn();
	}
	
	// Civ4 Reimagined
	updateBonusRatio(true);

	if (getGoldenAgeTurns() > 0)
	{
		changeGoldenAgeTurns(-1);
	}

	if (getAnarchyTurns() > 0)
	{
		changeAnarchyTurns(-1);
	}

	verifyCivics();

	updateTradeRoutes();

	updateWarWearinessPercentAnger();

	updateInflationRate();

	doEvents();

	updateEconomyHistory(GC.getGameINLINE().getGameTurn(), calculateTotalCommerce());
	updateIndustryHistory(GC.getGameINLINE().getGameTurn(), calculateTotalYield(YIELD_PRODUCTION));
	updateAgricultureHistory(GC.getGameINLINE().getGameTurn(), calculateTotalYield(YIELD_FOOD));
	updatePowerHistory(GC.getGameINLINE().getGameTurn(), getPower());
	updateCultureHistory(GC.getGameINLINE().getGameTurn(), countTotalCulture());
	updateEspionageHistory(GC.getGameINLINE().getGameTurn(), GET_TEAM(getTeam()).getEspionagePointsEver());
	expireMessages();  // turn log
	
	updateEffectFromStayingAtCivic(); // Civ4 Reimagined
	
	gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);

	AI_doTurnPost();

    if( GC.getGameINLINE().isDebugMode() )
	{
		GC.getGameINLINE().updateColoredPlots();
	}

	CvEventReporter::getInstance().endPlayerTurn( GC.getGameINLINE().getGameTurn(),  getID());
}


void CvPlayer::doTurnUnits()
{
	PROFILE_FUNC();

	int iLoop;

	AI_doTurnUnitsPre();

	for(CvSelectionGroup* pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup != NULL; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		pLoopSelectionGroup->doDelayedDeath();
	}

	for (int iPass = 0; iPass < 4; iPass++)
	{
		for (CvSelectionGroup* pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup != NULL; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
		{
			switch (pLoopSelectionGroup->getDomainType())
			{
			case DOMAIN_AIR:
				if (iPass == 1)
				{
					pLoopSelectionGroup->doTurn();
				}
				break;
			case DOMAIN_SEA:
				if (iPass == 2)
				{
					pLoopSelectionGroup->doTurn();
				}
				break;
			case DOMAIN_LAND:
				if (iPass == 3)
				{
					pLoopSelectionGroup->doTurn();
				}
				break;
			case DOMAIN_IMMOBILE:
				if (iPass == 0)
				{
					pLoopSelectionGroup->doTurn();
				}
				break;
			case NO_DOMAIN:
				FAssertMsg(NULL == pLoopSelectionGroup->getHeadUnit(), "Unit with no Domain");
			default:
				if (iPass == 3)
				{
					pLoopSelectionGroup->doTurn();
				}
				break;
			}
		}
	}

	// K-Mod. (currently unused)
	/*for (CvSelectionGroup* pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup != NULL; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		updateGroupCycle(pLoopSelectionGroup);
	}*/
	// K-Mod end

	if (getID() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getFAStarIFace()->ForceReset(&GC.getInterfacePathFinder());

		gDLL->getInterfaceIFace()->setDirty(Waypoints_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
	}

	gDLL->getInterfaceIFace()->setDirty(UnitInfo_DIRTY_BIT, true);

	AI_doTurnUnitsPost();
}


void CvPlayer::verifyCivics()
{
	int iI, iJ;

	if (!isAnarchy())
	{
		for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			if (!canDoCivics(getCivics((CivicOptionTypes)iI)))
			{
				for (iJ = 0; iJ < GC.getNumCivicInfos(); iJ++)
				{
					if (GC.getCivicInfo((CivicTypes)iJ).getCivicOptionType() == iI)
					{
						if (canDoCivics((CivicTypes)iJ))
						{
							setCivics(((CivicOptionTypes)iI), ((CivicTypes)iJ));
							break;
						}
					}
				}
			}
		}
	}
}


void CvPlayer::updatePlotGroups()
{
	PROFILE_FUNC();

	CvPlotGroup* pLoopPlotGroup;
	int iLoop;
	int iI;

	if (!(GC.getGameINLINE().isFinalInitialized()))
	{
		return;
	}

	for(pLoopPlotGroup = firstPlotGroup(&iLoop); pLoopPlotGroup != NULL; pLoopPlotGroup = nextPlotGroup(&iLoop))
	{
		pLoopPlotGroup->recalculatePlots();
	}

	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		GC.getMapINLINE().plotByIndexINLINE(iI)->updatePlotGroup(getID(), false);
	}

	updateTradeRoutes();
}


void CvPlayer::updateYield()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateYield();
	}

	// Civ4 Reimagined
	GC.getMapINLINE().updateSymbols();
}


void CvPlayer::updateMaintenance()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateMaintenance();
	}
}


void CvPlayer::updatePowerHealth()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updatePowerHealth();
	}
}


void CvPlayer::updateExtraBuildingHappiness()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateExtraBuildingHappiness();
	}
}


void CvPlayer::updateExtraBuildingHealth()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateExtraBuildingHealth();
	}
}


void CvPlayer::updateFeatureHappiness()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateFeatureHappiness();
	}
}


void CvPlayer::updateReligionHappiness()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateReligionHappiness();
	}
}


// Civ4 Reimagined
void CvPlayer::updateStateReligionTempleCache()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateStateReligionTempleCache();
	}
}


void CvPlayer::updateExtraSpecialistYield()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateExtraSpecialistYield();
	}
}


void CvPlayer::updateCommerce(CommerceTypes eCommerce)
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateCommerce(eCommerce);
	}
}


void CvPlayer::updateCommerce()
{
	PROFILE_FUNC(); // K-Mod
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateCommerce();
	}
}


void CvPlayer::updateBuildingCommerce()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateBuildingCommerce();
	}
}


void CvPlayer::updateReligionCommerce()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateReligionCommerce();
	}
}


void CvPlayer::updateCorporation()
{
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateCorporation();
	}
}


void CvPlayer::updateCityPlotYield()
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->plot()->updateYield();
	}
}


void CvPlayer::updateCitySight(bool bIncrement, bool bUpdatePlotGroups)
{
	CvCity* pLoopCity;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->plot()->updateSight(bIncrement, bUpdatePlotGroups);
	}
}


void CvPlayer::updateTradeRoutes()
{
	PROFILE_FUNC();

	CLLNode<int>* pCityNode;
	CvCity* pLoopCity;
	CvCity* pListCity;
	CLinkList<int> cityList;
	int iTotalTradeModifier;
	int iLoop;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->clearTradeRoutes();
	}

	cityList.clear();

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalTradeModifier = pLoopCity->totalTradeModifier();

		pCityNode = cityList.head();

		while (pCityNode != NULL)
		{
			pListCity = getCity(pCityNode->m_data);

			if (iTotalTradeModifier > pListCity->totalTradeModifier())
			{
				cityList.insertBefore(pLoopCity->getID(), pCityNode);
				break;
			}
			else
			{
				pCityNode = cityList.next(pCityNode);
			}
		}

		if (pCityNode == NULL)
		{
			cityList.insertAtEnd(pLoopCity->getID());
		}
	}

	pCityNode = cityList.head();

	while (pCityNode != NULL)
	{
		getCity(pCityNode->m_data)->updateTradeRoutes();
		pCityNode = cityList.next(pCityNode);
	}
}

void CvPlayer::updatePlunder(int iChange, bool bUpdatePlotGroups)
{
	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->isBlockading())
		{
			pLoopUnit->updatePlunder(iChange, bUpdatePlotGroups);
		}
	}
}

void CvPlayer::updateTimers()
{
	CvSelectionGroup* pLoopSelectionGroup;
	int iLoop;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		pLoopSelectionGroup->updateTimers(); // could destroy the selection group...
	}

	// if a unit was busy, perhaps it was not quite deleted yet, give it one more try
	if (getNumSelectionGroups() > getNumUnits())
	{
		for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
		{
			pLoopSelectionGroup->doDelayedDeath(); // could destroy the selection group...
		}
	}

	FAssertMsg(getNumSelectionGroups() <= getNumUnits(), "The number of Units is expected not to exceed the number of Selection Groups");
}


bool CvPlayer::hasReadyUnit(bool bAny) const
{
	PROFILE_FUNC();

	CvSelectionGroup* pLoopSelectionGroup;
	int iLoop;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		//if (pLoopSelectionGroup->readyToMove(bAny))
		if (pLoopSelectionGroup->readyToMove(bAny) && !pLoopSelectionGroup->isAutomated()) // K-Mod
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::hasAutoUnit() const
{
	PROFILE_FUNC();

	CvSelectionGroup* pLoopSelectionGroup;
	int iLoop;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if (pLoopSelectionGroup->readyToAuto())
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::hasBusyUnit() const
{
	//PROFILE_FUNC();

	CvSelectionGroup* pLoopSelectionGroup;
	int iLoop;

	for(pLoopSelectionGroup = firstSelectionGroup(&iLoop); pLoopSelectionGroup; pLoopSelectionGroup = nextSelectionGroup(&iLoop))
	{
		if (pLoopSelectionGroup->isBusy())
		{
			/* original bts code
		    if (pLoopSelectionGroup->getNumUnits() == 0)
		    {
		        pLoopSelectionGroup->kill();
		        return false;
		    } */ // disabled by K-Mod. isBusy returns false if there are no units in the group.

			return true;
		}
	}

	return false;
}

void CvPlayer::chooseTech(int iDiscover, CvWString szText, bool bFront)
{
	// K-mod
	FAssert(isHuman());
	if (iDiscover > 0)
		changeChoosingFreeTechCount(1); // note: if iDiscover is > 1, this function will be called again with iDiscover-=1
	// K-Mod end

	CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHOOSETECH);
	if (NULL != pInfo)
	{
		pInfo->setData1(iDiscover);
		pInfo->setText(szText);
		gDLL->getInterfaceIFace()->addPopup(pInfo, getID(), false, bFront);
	}
}


int CvPlayer::calculateScore(bool bFinal, bool bVictory) const
{
	PROFILE_FUNC();

	if (!isAlive())
	{
		return 0;
	}

	if (GET_TEAM(getTeam()).getNumMembers() == 0)
	{
		return 0;
	}

	long lScore = 0;

	CyArgsList argsList;
	argsList.add((int) getID());
	argsList.add(bFinal);
	argsList.add(bVictory);
	gDLL->getPythonIFace()->callFunction(PYGameModule, "calculateScore", argsList.makeFunctionArgs(), &lScore);

	return ((int)lScore);
}


int CvPlayer::findBestFoundValue() const
{
	CvArea* pLoopArea;
	int iValue;
	int ipBestValue;
	int iLoop;

	ipBestValue = 0;

	for(pLoopArea = GC.getMapINLINE().firstArea(&iLoop); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoop))
	{
		iValue = pLoopArea->getBestFoundValue(getID());

		if (iValue > ipBestValue)
		{
			ipBestValue = iValue;
		}
	}

	return ipBestValue;
}


int CvPlayer::upgradeAllPrice(UnitTypes eUpgradeUnit, UnitTypes eFromUnit)
{
	CvUnit* pLoopUnit;
	int iPrice;
	int iLoop;

	iPrice = 0;

	// Loop through units and determine the total power of this player's military
	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->getUnitType() == eFromUnit)
		{
			if (pLoopUnit->canUpgrade(eUpgradeUnit, true))
			{
				iPrice += pLoopUnit->upgradePrice(eUpgradeUnit);
			}
		}
	}

	return iPrice;
}

int CvPlayer::countReligionSpreadUnits(CvArea* pArea, ReligionTypes eReligion, bool bIncludeTraining) const
{
	PROFILE_FUNC();

	CvUnit* pLoopUnit;
	int iCount;
	int iLoop;

	iCount = 0;

	for (pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->getArea() == pArea->getID())
		{
			if (pLoopUnit->getUnitInfo().getReligionSpreads(eReligion) > 0)
			{
				iCount++;
			}
		}
	}

	// bbai
	if( bIncludeTraining )
	{
		CvCity* pLoopCity;
		for( pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop) )
		{
			UnitTypes eUnit = pLoopCity->getProductionUnit();

			if( eUnit != NO_UNIT )
			{
				if(GC.getUnitInfo(eUnit).getReligionSpreads(eReligion) > 0)
				{
					iCount++;
				}
			}
		}
	}
	// bbai end

	return iCount;
}

int CvPlayer::countCorporationSpreadUnits(CvArea* pArea, CorporationTypes eCorporation, bool bIncludeTraining) const
{
	PROFILE_FUNC();

	int iCount = 0;
	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = nextUnit(&iLoop))
	{
		//if (pLoopUnit->area() == pArea)
		if (pArea == NULL || pLoopUnit->area() == pArea) // K-Mod
		{
			if (pLoopUnit->getUnitInfo().getCorporationSpreads(eCorporation) > 0)
			{
				++iCount;
			}
		}
	}

	// bbai
	if (bIncludeTraining)
	{
		for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pArea == NULL || pLoopCity->area() == pArea) // K-Mod
			{
				UnitTypes eUnit = pLoopCity->getProductionUnit();

				if (eUnit != NO_UNIT)
				{
					if(GC.getUnitInfo(eUnit).getCorporationSpreads(eCorporation) > 0)
					{
						iCount++;
					}
				}
			}
		}
	}
	// bbai end

	return iCount;
}

int CvPlayer::countNumCoastalCities() const
{
	CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			iCount++;
		}
	}

	return iCount;
}


int CvPlayer::countNumCoastalCitiesByArea(CvArea* pArea) const
{
	CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	int iAreaID = pArea->getID();
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
		{
			if ((pLoopCity->getArea() == iAreaID) || pLoopCity->plot()->isAdjacentToArea(iAreaID))
			{
				iCount++;
			}
		}
	}

	return iCount;
}


bool CvPlayer::hasCoastalCitiesByWaterArea(CvArea* pArea) const
{
	int iLoop;
	
	if (!pArea->isWater())
	{
		return false;
	}

	int iAreaID = pArea->getID();
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		CvArea* pWaterArea = pLoopCity->waterArea();
		if (pWaterArea && pWaterArea->getID() == iAreaID)
		{
			return true;
		}
	}

	return false;
}


int CvPlayer::countTotalCulture() const
{
	CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iCount += pLoopCity->getCultureTimes100(getID());
	}

	return iCount/100;
}


// Modified by Civ4 Reimagined: Now able to count all boni if eBonus = NO_BONUS.
int CvPlayer::countOwnedBonuses(BonusTypes eBonus) const
{
	PROFILE("CvPlayer::countOwnedBonuses");
    CvCity* pLoopCity;
	CvPlot* pLoopPlot;
	int iCount;
	int iI;
    int iLoop;
	
	bool bAdvancedStart = (getAdvancedStartPoints() >= 0) && (getCurrentEra() < 3);
	
	// Civ4 Reimagined: Count all boni
	if (eBonus == NO_BONUS)
	{
		iCount = 0;

		//count bonuses outside city radius
		for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
		{
			pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

			if ((pLoopPlot->getOwnerINLINE() == getID()) && !pLoopPlot->isCityRadius())
			{
				if (pLoopPlot->getBonusType(getTeam()) != NO_BONUS)
				{
					iCount++;
				}
			}
			else if (bAdvancedStart && pLoopPlot->isRevealed(getTeam(), false))
			{
				if (pLoopPlot->getBonusType(getTeam()) != NO_BONUS)
				{
					iCount++;
				}
			}
		}



		//count bonuses inside city radius or easily claimed
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			iCount += pLoopCity->AI_countNumBonuses(eBonus, true, pLoopCity->getCommerceRate(COMMERCE_CULTURE) > 0, -1);
		}

		return iCount;
	}

	// K-Mod. Shortcut.
	if (!GET_TEAM(getTeam()).isBonusRevealed(eBonus))
		return 0;
	// K-Mod end

	iCount = 0;

	//count bonuses outside city radius
	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if ((pLoopPlot->getOwnerINLINE() == getID()) && !pLoopPlot->isCityRadius())
		{
			if (pLoopPlot->getBonusType(getTeam()) == eBonus)
			{
				iCount++;
			}
		}
		else if (bAdvancedStart && pLoopPlot->isRevealed(getTeam(), false))
		{
			if (pLoopPlot->getBonusType(getTeam()) == eBonus)
			{
				iCount++;
			}
		}
	}



	//count bonuses inside city radius or easily claimed
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iCount += pLoopCity->AI_countNumBonuses(eBonus, true, pLoopCity->getCommerceRate(COMMERCE_CULTURE) > 0, -1);
	}

	return iCount;
}


// K-Mod. I've rearranged some stuff in this function to fix a couple of minor bugs; and to make the code neater and less error prone.
int CvPlayer::countUnimprovedBonuses(CvArea* pArea, CvPlot* pFromPlot) const
{
	PROFILE_FUNC();

	gDLL->getFAStarIFace()->ForceReset(&GC.getBorderFinder());

	int iCount = 0;

	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->getOwnerINLINE() == getID() && pLoopPlot->area() == pArea && !pLoopPlot->isCity())
		{
			BonusTypes eNonObsoleteBonus = pLoopPlot->getNonObsoleteBonusType(getTeam());

			if (eNonObsoleteBonus != NO_BONUS)
			{
				if (!doesImprovementConnectBonus(pLoopPlot->getImprovementType(), eNonObsoleteBonus))
				{
					if ((pFromPlot == NULL) || gDLL->getFAStarIFace()->GeneratePath(&GC.getBorderFinder(), pFromPlot->getX_INLINE(), pFromPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), false, getID(), true))
					{
						for (int iJ = 0; iJ < GC.getNumBuildInfos(); iJ++)
						{
							BuildTypes eBuild = ((BuildTypes)iJ);

							if (doesImprovementConnectBonus((ImprovementTypes)GC.getBuildInfo(eBuild).getImprovement(), eNonObsoleteBonus) && canBuild(pLoopPlot, eBuild))
							{
								iCount++;
								break; // K-Mod!
							}
						}
					}
				}
			}
		}
	}

	return iCount;
}


int CvPlayer::countCityFeatures(FeatureTypes eFeature) const
{
	PROFILE_FUNC();

	CvCity* pLoopCity;
	CvPlot* pLoopPlot;
	int iCount;
	int iLoop;
	int iI;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for (iI = 0; iI < NUM_CITY_PLOTS; iI++)
		{
			pLoopPlot = plotCity(pLoopCity->getX_INLINE(), pLoopCity->getY_INLINE(), iI);

			if (pLoopPlot != NULL)
			{
				if (pLoopPlot->getFeatureType() == eFeature)
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}


int CvPlayer::countNumBuildings(BuildingTypes eBuilding) const
{
	PROFILE_FUNC();

	CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->getNumBuilding(eBuilding) > 0)
		{
			iCount += pLoopCity->getNumBuilding(eBuilding);
		}
	}

	return iCount;
}


int CvPlayer::countNumCitiesConnectedToCapital() const
{
	CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isConnectedToCapital())
		{
			iCount++;
		}
	}

	return iCount;
}

// K-Mod
bool CvPlayer::doesImprovementConnectBonus(ImprovementTypes eImprovement, BonusTypes eBonus) const
{
	return GET_TEAM(getTeam()).doesImprovementConnectBonus(eImprovement, eBonus);
}
// K-Mod end


bool CvPlayer::canContact(PlayerTypes ePlayer) const
{
	if (ePlayer == getID())
	{
		return false;
	}

	if (!isAlive() || !(GET_PLAYER(ePlayer).isAlive()))
	{
		return false;
	}

	if (isBarbarian() || GET_PLAYER(ePlayer).isBarbarian())
	{
		return false;
	}

	if (isMinorCiv() || GET_PLAYER(ePlayer).isMinorCiv())
	{
		return false;
	}

	if (getTeam() != GET_PLAYER(ePlayer).getTeam())
	{
		if (!(GET_TEAM(getTeam()).isHasMet(GET_PLAYER(ePlayer).getTeam())))
		{
			return false;
		}

		if (atWar(getTeam(), GET_PLAYER(ePlayer).getTeam()))
		{
			if (!(GET_TEAM(getTeam()).canChangeWarPeace(GET_PLAYER(ePlayer).getTeam())))
			{
				return false;
			}
		}

		if (isHuman() || GET_PLAYER(ePlayer).isHuman())
		{
			if (GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_WAR))
			{
				return false;
			}
		}
	}

	return true;
}

// K-Mod
bool CvPlayer::canContactAndTalk(PlayerTypes ePlayer) const
{
	return canContact(ePlayer) && AI_isWillingToTalk(ePlayer) && GET_PLAYER(ePlayer).AI_isWillingToTalk(getID());
}
// K-Mod end

void CvPlayer::contact(PlayerTypes ePlayer)
{
	CvDiploParameters* pDiplo;

	if (!canContact(ePlayer) || isTurnDone())
	{
		return;
	}

	if (GET_PLAYER(ePlayer).isHuman())
	{
		if (GC.getGameINLINE().isPbem() || GC.getGameINLINE().isHotSeat() || (GC.getGameINLINE().isPitboss() && !gDLL->isConnected(GET_PLAYER(ePlayer).getNetID())))
		{
			if (gDLL->isMPDiplomacy())
			{
				gDLL->beginMPDiplomacy(ePlayer, false, false);
			}
		}
		else
		{
			if (gDLL->getInterfaceIFace()->isFlashing(ePlayer))
			{
				if (!gDLL->getInterfaceIFace()->isDiplomacyLocked())
				{
					gDLL->getInterfaceIFace()->setDiplomacyLocked(true);
					gDLL->sendContactCiv(NETCONTACT_RESPONSE, ePlayer);
				}
			}
			else
			{
				gDLL->sendContactCiv(NETCONTACT_INITIAL, ePlayer);
			}
		}
	}
	else
	{
		pDiplo = new CvDiploParameters(ePlayer);
		FAssert(pDiplo != NULL);
		if (GC.ctrlKey())
		{
			pDiplo->setDiploComment((DiploCommentTypes)GC.getInfoTypeForString("AI_DIPLOCOMMENT_TRADING"));
		}
		gDLL->getInterfaceIFace()->setDiploQueue(pDiplo, GC.getGameINLINE().getActivePlayer());
	}
}


void CvPlayer::handleDiploEvent(DiploEventTypes eDiploEvent, PlayerTypes ePlayer, int iData1, int iData2)
{
	CivicTypes* paeNewCivics;
	CvCity* pCity;
	int iI;

	FAssertMsg(ePlayer != getID(), "shouldn't call this function on ourselves");

	switch (eDiploEvent)
	{
	case DIPLOEVENT_CONTACT:
		AI_setFirstContact(ePlayer, true);
		GET_PLAYER(ePlayer).AI_setFirstContact(getID(), true);
		break;

	case DIPLOEVENT_AI_CONTACT:
		break;

	case DIPLOEVENT_FAILED_CONTACT:
		AI_setFirstContact(ePlayer, true);
		GET_PLAYER(ePlayer).AI_setFirstContact(getID(), true);
		break;

	case DIPLOEVENT_GIVE_HELP:
		AI_changeMemoryCount(ePlayer, MEMORY_GIVE_HELP, 1);
		forcePeace(ePlayer);
		break;

	case DIPLOEVENT_REFUSED_HELP:
		AI_changeMemoryCount(ePlayer, MEMORY_REFUSED_HELP, 1);
		break;

	case DIPLOEVENT_ACCEPT_DEMAND:
		AI_changeMemoryCount(ePlayer, MEMORY_ACCEPT_DEMAND, 1);
		forcePeace(ePlayer);
		break;

	case DIPLOEVENT_REJECTED_DEMAND:
		FAssertMsg(GET_PLAYER(ePlayer).getTeam() != getTeam(), "shouldn't call this function on our own team");

		AI_changeMemoryCount(ePlayer, MEMORY_REJECTED_DEMAND, 1);

		if (AI_demandRebukedSneak(ePlayer))
		{
			GET_TEAM(getTeam()).AI_setWarPlan(GET_PLAYER(ePlayer).getTeam(), WARPLAN_PREPARING_LIMITED);
		}
		break;

	case DIPLOEVENT_DEMAND_WAR:
		FAssertMsg(GET_PLAYER(ePlayer).getTeam() != getTeam(), "shouldn't call this function on our own team");
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
/*                                                                                              */
/* AI logging                                                                                   */
/************************************************************************************************/
		if( gTeamLogLevel >= 2 )
		{
			logBBAI("    Team %d (%S) declares war on team %d due to DIPLOEVENT", getTeam(), getCivilizationDescription(0), ePlayer );
		}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
		GET_TEAM(getTeam()).declareWar(GET_PLAYER(ePlayer).getTeam(), false, WARPLAN_LIMITED);
		break;

	case DIPLOEVENT_CONVERT:
		AI_changeMemoryCount(ePlayer, MEMORY_ACCEPTED_RELIGION, 1);
		GET_PLAYER(ePlayer).convert(getStateReligion());
		break;

	case DIPLOEVENT_NO_CONVERT:
		AI_changeMemoryCount(ePlayer, MEMORY_DENIED_RELIGION, 1);
		break;

	case DIPLOEVENT_REVOLUTION:
		AI_changeMemoryCount(ePlayer, MEMORY_ACCEPTED_CIVIC, 1);

		paeNewCivics = new CivicTypes[GC.getNumCivicOptionInfos()];

		for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
		{
			paeNewCivics[iI] = GET_PLAYER(ePlayer).getCivics((CivicOptionTypes)iI);
		}

		FAssertMsg(GC.getLeaderHeadInfo(getPersonalityType()).getFavoriteCivic() != NO_CIVIC, "getFavoriteCivic() must be valid");

		paeNewCivics[GC.getCivicInfo((CivicTypes)(GC.getLeaderHeadInfo(getPersonalityType())).getFavoriteCivic()).getCivicOptionType()] = ((CivicTypes)(GC.getLeaderHeadInfo(getPersonalityType()).getFavoriteCivic()));

		// Civ4 Reimagined: No anarchy
		GET_PLAYER(ePlayer).revolution(paeNewCivics, true, false);

		SAFE_DELETE_ARRAY(paeNewCivics);
		break;

	case DIPLOEVENT_NO_REVOLUTION:
		AI_changeMemoryCount(ePlayer, MEMORY_DENIED_CIVIC, 1);
		break;

	case DIPLOEVENT_JOIN_WAR:
		AI_changeMemoryCount(ePlayer, MEMORY_ACCEPTED_JOIN_WAR, 1);
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
/*                                                                                              */
/* AI logging                                                                                   */
/************************************************************************************************/
		if( gTeamLogLevel >= 2 )
		{
			logBBAI("    Team %d (%S) declares war on team %d due to DIPLOEVENT", getTeam(), getCivilizationDescription(0), ePlayer );
		}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
		GET_TEAM(GET_PLAYER(ePlayer).getTeam()).declareWar(((TeamTypes)iData1), false, WARPLAN_DOGPILE);

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == ((TeamTypes)iData1))
				{
					GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_HIRED_WAR_ALLY, 1);
				}
			}
		}
		break;

	case DIPLOEVENT_NO_JOIN_WAR:
		// Removed by Civ4 Reimagined
		//AI_changeMemoryCount(ePlayer, MEMORY_DENIED_JOIN_WAR, 1);
		break;

	case DIPLOEVENT_STOP_TRADING:
		AI_changeMemoryCount(ePlayer, MEMORY_ACCEPTED_STOP_TRADING, 1);
		GET_PLAYER(ePlayer).stopTradingWithTeam((TeamTypes)iData1);

		for (iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).isAlive())
			{
				if (GET_PLAYER((PlayerTypes)iI).getTeam() == ((TeamTypes)iData1))
				{
					GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_HIRED_TRADE_EMBARGO, 1);
				}
			}
		}
		break;

	case DIPLOEVENT_NO_STOP_TRADING:
		AI_changeMemoryCount(ePlayer, MEMORY_DENIED_STOP_TRADING, 2); //Civ4 Reimagined: Add two stacks to compensate for reduced amount of inquiries.
		break;

	case DIPLOEVENT_ASK_HELP:
		AI_changeMemoryCount(ePlayer, MEMORY_MADE_DEMAND_RECENT, 1);
		break;

	case DIPLOEVENT_MADE_DEMAND:
		if (AI_getMemoryCount(ePlayer, MEMORY_MADE_DEMAND) < 10)
		{
			AI_changeMemoryCount(ePlayer, MEMORY_MADE_DEMAND, 1);
		}
		AI_changeMemoryCount(ePlayer, MEMORY_MADE_DEMAND_RECENT, 1);
		break;

	case DIPLOEVENT_MADE_DEMAND_VASSAL:
		break;

	case DIPLOEVENT_RESEARCH_TECH:
		pushResearch(((TechTypes)iData1), true);
		break;

	case DIPLOEVENT_TARGET_CITY:
		pCity = GET_PLAYER((PlayerTypes)iData1).getCity(iData2);
		if (pCity != NULL)
		{
			pCity->area()->setTargetCity(getID(), pCity);
			GET_PLAYER(getID()).AI_setCityTargetTimer(GC.getGameINLINE().getPeaceDealLength()); // Civ4 Reimagined: Added gamespeed modifier // K-Mod. (I'd make it a virtual function, but that causes problems.)
		}
		break;
	// K-Mod
	case DIPLOEVENT_SET_WARPLAN:
	{
		CvTeamAI& kOurTeam = GET_TEAM(getTeam());
		FAssert(kOurTeam.getAtWarCount(true) == 0);
		if (iData1 == NO_TEAM)
		{
			FAssert(iData2 == NO_WARPLAN);
			for (TeamTypes i = (TeamTypes)0; i < MAX_CIV_TEAMS; i=(TeamTypes)(i+1))
			{
				if (!kOurTeam.isAtWar(i))
					kOurTeam.AI_setWarPlan(i, NO_WARPLAN, false);
			}
		}
		else
			kOurTeam.AI_setWarPlan((TeamTypes)iData1, (WarPlanTypes)iData2, false);
		break;
	}
	// K-Mod end

	default:
		FAssert(false);
		break;
	}
}


bool CvPlayer::canTradeWith(PlayerTypes eWhoTo) const
{
	if (atWar(getTeam(), GET_PLAYER(eWhoTo).getTeam()))
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isTechTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isTechTrading())
	{
		return true;
	}

	// Civ4 Reimagined: Was "OR".
	if (GET_TEAM(getTeam()).isGoldTrading() && GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isGoldTrading())
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isMapTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isMapTrading())
	{
		return true;
	}

	if (canTradeNetworkWith(eWhoTo))
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isOpenBordersTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isOpenBordersTrading())
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isDefensivePactTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isDefensivePactTrading())
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isPermanentAllianceTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isPermanentAllianceTrading())
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isVassalStateTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isVassalStateTrading())
	{
		return true;
	}

	return false;
}

bool CvPlayer::canReceiveTradeCity() const
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		return false;
	}

	return true;
}

bool CvPlayer::canTradeItem(PlayerTypes eWhoTo, TradeData item, bool bTestDenial) const
{
	CvCity *pOurCapitalCity;

	if (bTestDenial)
	{
		if (getTradeDenial(eWhoTo, item) != NO_DENIAL)
		{
			return false;
		}
	}

	switch (item.m_eItemType)
	{
	case TRADE_TECHNOLOGIES:
		if (!(GC.getGameINLINE().isOption(GAMEOPTION_NO_TECH_TRADING)))
		{
			if (GC.getTechInfo((TechTypes)(item.m_iData)).isTrade())
			{
				if (GET_TEAM(getTeam()).isHasTech((TechTypes)(item.m_iData)) && !(GET_TEAM(getTeam()).isNoTradeTech((TechTypes)(item.m_iData))))
				{
					if (!GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isHasTech((TechTypes)(item.m_iData)))
					{
						//if (GET_PLAYER(eWhoTo).isHuman() || (GET_PLAYER(eWhoTo).getCurrentResearch() != item.m_iData))
						{
							if (GET_TEAM(getTeam()).isTechTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isTechTrading())
							{
								FAssertMsg(item.m_iData >= 0, "item.m_iData is expected to be non-negative (invalid Index)");

								if (GET_PLAYER(eWhoTo).canResearch(((TechTypes)item.m_iData), true))
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
		break;

	case TRADE_RESOURCES:
		FAssertMsg(item.m_iData > -1, "iData is expected to be non-negative");

		if (canTradeNetworkWith(eWhoTo))
		{
			if (!GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isBonusObsolete((BonusTypes) item.m_iData) && !GET_TEAM(getTeam()).isBonusObsolete((BonusTypes) item.m_iData))
			{
				bool bCanTradeAll = (isHuman() || getTeam() == GET_PLAYER(eWhoTo).getTeam() || GET_TEAM(getTeam()).isVassal(GET_PLAYER(eWhoTo).getTeam()));
				if (getNumTradeableBonuses((BonusTypes) item.m_iData) > (bCanTradeAll ? 0 : 1))
				{
					return true;
				}
			}
		}
		break;

	case TRADE_CITIES:
		{
			CvCity* pCityTraded = getCity(item.m_iData);

			if (NULL != pCityTraded && pCityTraded->getLiberationPlayer(false) == eWhoTo)
			{
				return true;
			}

			if (GET_PLAYER(eWhoTo).canReceiveTradeCity())
			{
				if (0 == GC.getGameINLINE().getMaxCityElimination())
				{
					// Civ4 Reimagined: Can only trade cities when at war
					if (!GET_TEAM(getTeam()).isAVassal() && !GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isVassal(getTeam()) && GET_TEAM(getTeam()).isAtWar(GET_PLAYER(eWhoTo).getTeam()))
					{
						pOurCapitalCity = getCapitalCity();
						if (pOurCapitalCity != NULL)
						{
							if (pOurCapitalCity->getID() != item.m_iData)
							{
								return true;
							}
						}
					}
				}
			}
		}
		break;

	case TRADE_GOLD: // Civ4 Reimagined: Was "OR"
		if ((GET_TEAM(getTeam()).isGoldTrading() && GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isGoldTrading()))
		{
			if (getGold() >= item.m_iData)
			{
				return true;
			}
		}
		break;

	case TRADE_GOLD_PER_TURN: // Civ4 Reimagined: Was "OR"
		if (GET_TEAM(getTeam()).isGoldTrading() && GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isGoldTrading())
		{
			return true;
		}
		break;

	case TRADE_MAPS:
		if (getTeam() != GET_PLAYER(eWhoTo).getTeam())
		{
			if (GET_TEAM(getTeam()).isMapTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isMapTrading())
			{
				return true;
			}
		}
		break;

	case TRADE_VASSAL:
		// Civ4 Reimagined: Don't allow peace vassals
		return false;
	case TRADE_SURRENDER:
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      12/06/09                                jdog5000      */
/*                                                                                              */
/* Customization                                                                                */
/************************************************************************************************/
		if (!isHuman() || GET_PLAYER(eWhoTo).isHuman() || (GC.getBBAI_HUMAN_AS_VASSAL_OPTION()))
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
		{
			CvTeam& kVassalTeam = GET_TEAM(getTeam());
			CvTeam& kMasterTeam = GET_TEAM(GET_PLAYER(eWhoTo).getTeam());
			if (kMasterTeam.isVassalStateTrading()) // the master must possess the tech
			{
				if (!kVassalTeam.isAVassal() && !kMasterTeam.isAVassal() && getTeam() != GET_PLAYER(eWhoTo).getTeam())
				{
					if ((kMasterTeam.isAtWar(getTeam()) || item.m_iData == 1) && item.m_eItemType == TRADE_SURRENDER)
					{
						return true;
					}

					if (!kMasterTeam.isAtWar(getTeam()) && item.m_eItemType == TRADE_VASSAL)
					{
						return true;
					}
				}
			}
		}
		break;

	case TRADE_PEACE:
		if (!(GET_TEAM(getTeam()).isHuman()))
		{
			if (!(GET_TEAM(getTeam()).isAVassal()))
			{
				if (GET_TEAM(getTeam()).isHasMet((TeamTypes)(item.m_iData)) && GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isHasMet((TeamTypes)(item.m_iData)))
				{
					if (atWar(getTeam(), ((TeamTypes)(item.m_iData))))
					{
						return true;
					}
				}
			}
		}
		break;

	case TRADE_WAR:
		if (!(GET_TEAM(getTeam()).isHuman()))
		{
			if (!(GET_TEAM(getTeam()).isAVassal()))
			{
				if (!GET_TEAM((TeamTypes)item.m_iData).isAVassal())
				{
					if (GET_TEAM(getTeam()).isHasMet((TeamTypes)(item.m_iData)) && GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isHasMet((TeamTypes)(item.m_iData)))
					{
						if (GET_TEAM(getTeam()).canDeclareWar((TeamTypes)(item.m_iData)))
						{
							return true;
						}
					}
				}
			}
		}
		break;

	case TRADE_EMBARGO:
		if (!(GET_TEAM(getTeam()).isHuman()))
		{
			if (GET_TEAM(getTeam()).isHasMet((TeamTypes)(item.m_iData)) && GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isHasMet((TeamTypes)(item.m_iData)))
			{
				if (canStopTradingWithTeam((TeamTypes)(item.m_iData)))
				{
					return true;
				}
			}
		}
		break;

	case TRADE_CIVIC:
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       10/22/09                          denev & jdog5000    */
/*                                                                                              */
/* Diplomacy                                                                                    */
/************************************************************************************************/
/* original bts code
		if (!(GET_TEAM(getTeam()).isHuman()))
*/
		if (!(GET_TEAM(getTeam()).isHuman()) || (getTeam() == GET_PLAYER(eWhoTo).getTeam()))
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
		{
			if (GET_PLAYER(eWhoTo).isCivic((CivicTypes)(item.m_iData)))
			{
				if (canDoCivics((CivicTypes)(item.m_iData)) && !isCivic((CivicTypes)(item.m_iData)))
				{
					if (canRevolution(NULL))
					{
						return true;
					}
				}
			}
		}
		break;

	case TRADE_RELIGION:
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       10/22/09                          denev & jdog5000    */
/*                                                                                              */
/* Diplomacy                                                                                    */
/************************************************************************************************/
/* original bts code
		if (!(GET_TEAM(getTeam()).isHuman()))
*/
		if (!(GET_TEAM(getTeam()).isHuman()) || (getTeam() == GET_PLAYER(eWhoTo).getTeam()))
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
		{
			if (GET_PLAYER(eWhoTo).getStateReligion() == ((ReligionTypes)(item.m_iData)))
			{
				if (canConvert((ReligionTypes)(item.m_iData)))
				{
					return true;
				}
			}
		}
		break;

	case TRADE_OPEN_BORDERS:
		if (getTeam() != GET_PLAYER(eWhoTo).getTeam())
		{
			if (!atWar(getTeam(), GET_PLAYER(eWhoTo).getTeam()))
			{
				if (!(GET_TEAM(getTeam()).isOpenBorders(GET_PLAYER(eWhoTo).getTeam())))
				{
					if (GET_TEAM(getTeam()).isOpenBordersTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isOpenBordersTrading())
					{
						return true;
					}
				}
			}
		}
		break;

	case TRADE_DEFENSIVE_PACT:
		if (!(GET_TEAM(getTeam()).isAVassal()) && !(GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isAVassal()))
		{
			if (getTeam() != GET_PLAYER(eWhoTo).getTeam() && !GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isVassal(getTeam()))
			{
				if (!atWar(getTeam(), GET_PLAYER(eWhoTo).getTeam()))
				{
					if (!(GET_TEAM(getTeam()).isDefensivePact(GET_PLAYER(eWhoTo).getTeam())))
					{
						if (GET_TEAM(getTeam()).isDefensivePactTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isDefensivePactTrading())
						{
							if ((GET_TEAM(getTeam()).getAtWarCount(true) == 0) && (GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).getAtWarCount(true) == 0))
							{
								if (GET_TEAM(getTeam()).canSignDefensivePact(GET_PLAYER(eWhoTo).getTeam()))
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
		break;

	case TRADE_PERMANENT_ALLIANCE:
		if (!(GET_TEAM(getTeam()).isAVassal()) && !(GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isAVassal()))
		{
			if (getTeam() != GET_PLAYER(eWhoTo).getTeam() && !GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isVassal(getTeam()))
			{
				if (!atWar(getTeam(), GET_PLAYER(eWhoTo).getTeam()))
				{
					if (GET_TEAM(getTeam()).isPermanentAllianceTrading() || GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).isPermanentAllianceTrading())
					{
						if ((GET_TEAM(getTeam()).getNumMembers() == 1) && (GET_TEAM(GET_PLAYER(eWhoTo).getTeam()).getNumMembers() == 1))
						{
							return true;
						}
					}
				}
			}
		}
		break;

	case TRADE_PEACE_TREATY:
		return true;
		break;
	}

	return false;
}


DenialTypes CvPlayer::getTradeDenial(PlayerTypes eWhoTo, TradeData item) const
{
	CvCity* pCity;
	const CvTeamAI& kOurTeam = GET_TEAM(getTeam()); // K-Mod

	// K-Mod note: I've changed it so that AI players on human teams can be contacted when not at war.
	// So.. as a follow up on that change, I'm making the AI deny trades which affect the team, not just the player.
	switch (item.m_eItemType)
	{
	case TRADE_TECHNOLOGIES:
		// K-Mod
		if (!isHuman() && kOurTeam.isHuman())
			return DENIAL_MYSTERY;
		// K-Mod end
		return kOurTeam.AI_techTrade(((TechTypes)(item.m_iData)), GET_PLAYER(eWhoTo).getTeam());
		break;

	case TRADE_RESOURCES:
		return AI_bonusTrade(((BonusTypes)(item.m_iData)), eWhoTo);
		break;

	case TRADE_CITIES:
		pCity = getCity(item.m_iData);
		if (pCity != NULL)
		{
			return AI_cityTrade(pCity, eWhoTo);
		}
		break;

	case TRADE_GOLD:
	case TRADE_GOLD_PER_TURN:
		break;

	case TRADE_MAPS:
		return kOurTeam.AI_mapTrade(GET_PLAYER(eWhoTo).getTeam());
		break;

	case TRADE_SURRENDER:
		return kOurTeam.AI_surrenderTrade(GET_PLAYER(eWhoTo).getTeam(), 140);
		break;

	case TRADE_VASSAL:
		// K-Mod
		if (!isHuman() && kOurTeam.isHuman())
			return DENIAL_MYSTERY;
		// K-Mod end
		return kOurTeam.AI_vassalTrade(GET_PLAYER(eWhoTo).getTeam());
		break;

	case TRADE_PEACE:
		return kOurTeam.AI_makePeaceTrade(((TeamTypes)(item.m_iData)), GET_PLAYER(eWhoTo).getTeam());
		break;

	case TRADE_WAR:
		return kOurTeam.AI_declareWarTrade(((TeamTypes)(item.m_iData)), GET_PLAYER(eWhoTo).getTeam());
		break;

	case TRADE_EMBARGO:
		return AI_stopTradingTrade(((TeamTypes)(item.m_iData)), eWhoTo);
		break;

	case TRADE_CIVIC:
		return AI_civicTrade(((CivicTypes)(item.m_iData)), eWhoTo);
		break;

	case TRADE_RELIGION:
		return AI_religionTrade(((ReligionTypes)(item.m_iData)), eWhoTo);
		break;

	case TRADE_OPEN_BORDERS:
		return kOurTeam.AI_openBordersTrade(GET_PLAYER(eWhoTo).getTeam());
		break;

	case TRADE_DEFENSIVE_PACT:
		// K-Mod
		if (!isHuman() && kOurTeam.isHuman())
			return DENIAL_MYSTERY;
		// K-Mod end
		return kOurTeam.AI_defensivePactTrade(GET_PLAYER(eWhoTo).getTeam());
		break;

	case TRADE_PERMANENT_ALLIANCE:
		// K-Mod
		if (!isHuman() && kOurTeam.isHuman())
			return DENIAL_MYSTERY;
		// K-Mod end
		return kOurTeam.AI_permanentAllianceTrade(GET_PLAYER(eWhoTo).getTeam());
		break;

	case TRADE_PEACE_TREATY:
		// K-Mod
		if (kOurTeam.AI_refusePeace(GET_PLAYER(eWhoTo).getTeam()))
			return DENIAL_VICTORY;
		// K-Mod end
		break;
	}

	return NO_DENIAL;
}


bool CvPlayer::canTradeNetworkWith(PlayerTypes ePlayer) const
{
	CvCity* pOurCapitalCity;

	pOurCapitalCity = getCapitalCity();

	if (pOurCapitalCity != NULL)
	{
		if (pOurCapitalCity->isConnectedToCapital(ePlayer))
		{
			return true;
		}
	}

	return false;
}


int CvPlayer::getNumAvailableBonuses(BonusTypes eBonus) const
{
	CvPlotGroup* pPlotGroup;

	pPlotGroup = ((getCapitalCity() != NULL) ? getCapitalCity()->plot()->getOwnerPlotGroup() : NULL);

	if (pPlotGroup != NULL)
	{
		return pPlotGroup->getNumBonuses(eBonus) + getPlayerExtraAvailableBonuses(eBonus);  // Civ4 Reimagined
	}

	return 0;
}


int CvPlayer::getNumTradeableBonuses(BonusTypes eBonus) const
{
	return (getNumAvailableBonuses(eBonus) - getBonusImport(eBonus));
}

bool CvPlayer::hasBonus(BonusTypes eBonus) const
{
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->hasBonus(eBonus))
		{
			return true;
		}
	}

	return false;
}

int CvPlayer::getNumTradeBonusImports(PlayerTypes ePlayer) const
{
	CLLNode<TradeData>* pNode;
	CvDeal* pLoopDeal;
	int iCount;
	int iLoop;

	FAssert(ePlayer != getID());

	iCount = 0;

	for(pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
	{
		if ((pLoopDeal->getFirstPlayer() == getID()) && (pLoopDeal->getSecondPlayer() == ePlayer))
		{
			for (pNode = pLoopDeal->headSecondTradesNode(); (pNode != NULL); pNode = pLoopDeal->nextSecondTradesNode(pNode))
			{
				if (pNode->m_data.m_eItemType == TRADE_RESOURCES)
				{
					iCount++;
				}
			}
		}

		if ((pLoopDeal->getFirstPlayer() == ePlayer) && (pLoopDeal->getSecondPlayer() == getID()))
		{
			for (pNode = pLoopDeal->headFirstTradesNode(); (pNode != NULL); pNode = pLoopDeal->nextFirstTradesNode(pNode))
			{
				if (pNode->m_data.m_eItemType == TRADE_RESOURCES)
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}


bool CvPlayer::isTradingWithTeam(TeamTypes eTeam, bool bIncludeCancelable) const
{
	int iLoop;

	if (eTeam == getTeam())
	{
		return false;
	}

	for (CvDeal* pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
	{
		if (bIncludeCancelable || pLoopDeal->isCancelable(getID()))
		{
			if (!pLoopDeal->isPeaceDeal())
			{
				if ((pLoopDeal->getFirstPlayer() == getID()) && (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == eTeam))
				{
					if (pLoopDeal->getLengthFirstTrades() > 0)
					{
						return true;
					}
				}

				if ((pLoopDeal->getSecondPlayer() == getID()) && (GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == eTeam))
				{
					if (pLoopDeal->getLengthSecondTrades() > 0)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}


bool CvPlayer::canStopTradingWithTeam(TeamTypes eTeam, bool bContinueNotTrading) const
{
	if (eTeam == getTeam())
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isVassal(eTeam))
	{
		return false;
	}

	if (!isTradingWithTeam(eTeam, false))
	{
		if (bContinueNotTrading && !isTradingWithTeam(eTeam, true))
		{
			return true;
		}

		return false;
	}

	return true;
}


void CvPlayer::stopTradingWithTeam(TeamTypes eTeam)
{
	CvDeal* pLoopDeal;
	int iLoop;
	int iI;

	FAssert(eTeam != getTeam());

	for(pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
	{
		if (pLoopDeal->isCancelable(getID()) && !(pLoopDeal->isPeaceDeal()))
		{
			if (((pLoopDeal->getFirstPlayer() == getID()) && (GET_PLAYER(pLoopDeal->getSecondPlayer()).getTeam() == eTeam)) ||
				  ((pLoopDeal->getSecondPlayer() == getID()) && (GET_PLAYER(pLoopDeal->getFirstPlayer()).getTeam() == eTeam)))
			{
				pLoopDeal->kill();
			}
		}
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == eTeam)
			{
				GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_STOPPED_TRADING, 1);
				GET_PLAYER((PlayerTypes)iI).AI_changeMemoryCount(getID(), MEMORY_STOPPED_TRADING_RECENT, 1);
			}
		}
	}
}


void CvPlayer::killAllDeals()
{
	CvDeal* pLoopDeal;
	int iLoop;

	for(pLoopDeal = GC.getGameINLINE().firstDeal(&iLoop); pLoopDeal != NULL; pLoopDeal = GC.getGameINLINE().nextDeal(&iLoop))
	{
		if ((pLoopDeal->getFirstPlayer() == getID()) || (pLoopDeal->getSecondPlayer() == getID()))
		{
			pLoopDeal->kill();
		}
	}
}


void CvPlayer::findNewCapital()
{
	CvCity* pOldCapital;
	CvCity* pLoopCity;
	CvCity* ppBestCity;
	BuildingTypes eCapitalBuilding;
	int iValue;
	int ipBestValue;
	int iLoop;

	eCapitalBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(GC.getDefineINT("CAPITAL_BUILDINGCLASS"))));

	if (eCapitalBuilding == NO_BUILDING)
	{
		return;
	}

	pOldCapital = getCapitalCity();

	ipBestValue = 0;
	ppBestCity = NULL;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity != pOldCapital)
		{
			if (0 == pLoopCity->getNumRealBuilding(eCapitalBuilding))
			{
				iValue = (pLoopCity->getPopulation() * 4);

				iValue += pLoopCity->getYieldRate(YIELD_FOOD);
				iValue += (pLoopCity->getYieldRate(YIELD_PRODUCTION) * 3);
				iValue += (pLoopCity->getYieldRate(YIELD_COMMERCE) * 2);
				iValue += pLoopCity->getCultureLevel();
				iValue += pLoopCity->getReligionCount();
				iValue += pLoopCity->getCorporationCount();
				iValue += (pLoopCity->getNumGreatPeople() * 2);

				iValue *= (pLoopCity->calculateCulturePercent(getID()) + 100);
				iValue /= 100;

				if (iValue > ipBestValue)
				{
					ipBestValue = iValue;
					ppBestCity = pLoopCity;
				}
			}
		}
	}

	if (ppBestCity != NULL)
	{
		if (pOldCapital != NULL)
		{
			pOldCapital->setNumRealBuilding(eCapitalBuilding, 0);
		}
		FAssertMsg(!(ppBestCity->getNumRealBuilding(eCapitalBuilding)), "(ppBestCity->getNumRealBuilding(eCapitalBuilding)) did not return false as expected");
		ppBestCity->setNumRealBuilding(eCapitalBuilding, 1);
	}
}


int CvPlayer::getNumGovernmentCenters() const
{
	CvCity* pLoopCity;
	int iCount;
	int iLoop;

	iCount = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->isGovernmentCenter())
		{
			iCount++;
		}
	}

	return iCount;
}


bool CvPlayer::canRaze(CvCity* pCity) const
{
	if (!pCity->isAutoRaze())
	{
		if (GC.getGameINLINE().isOption(GAMEOPTION_NO_CITY_RAZING))
		{
			return false;
		}

		if (pCity->getOwnerINLINE() != getID())
		{
			return false;
		}

		if (pCity->calculateTeamCulturePercent(getTeam()) >= GC.getDefineINT("RAZING_CULTURAL_PERCENT_THRESHOLD"))
		{
			return false;
		}
		
		// Civ4 Reimagined
		if (pCity->getPopulation() > 4)
		{
			return false;
		}
	}

	CyCity* pyCity = new CyCity(pCity);
	CyArgsList argsList;
	argsList.add(getID());	// Player ID
	argsList.add(gDLL->getPythonIFace()->makePythonObject(pyCity));	// pass in city class
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "canRazeCity", argsList.makeFunctionArgs(), &lResult);
	delete pyCity;	// python fxn must not hold on to this pointer 
	if (lResult == 0)
	{
		return (false);
	}

	return true;
}


void CvPlayer::raze(CvCity* pCity)
{
	wchar szBuffer[1024];
	PlayerTypes eHighestCulturePlayer;
	int iI, iJ;

	if (!canRaze(pCity))
	{
		return;
	}

	FAssert(pCity->getOwnerINLINE() == getID());

	eHighestCulturePlayer = pCity->findHighestCulture();

	if (eHighestCulturePlayer != NO_PLAYER)
	{
		if (GET_PLAYER(eHighestCulturePlayer).getTeam() != getTeam())
		{
			GET_PLAYER(eHighestCulturePlayer).AI_changeMemoryCount(getID(), MEMORY_RAZED_CITY, 1);
		}
	}

	for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (pCity->isHolyCity((ReligionTypes)iI))
		{
			for (iJ = 0; iJ < MAX_PLAYERS; iJ++)
			{
				if (GET_PLAYER((PlayerTypes)iJ).isAlive())
				{
					if (iJ != getID())
					{
						if (GET_PLAYER((PlayerTypes)iJ).getStateReligion() == ((ReligionTypes)iI))
						{
							GET_PLAYER((PlayerTypes)iJ).AI_changeMemoryCount(getID(), MEMORY_RAZED_HOLY_CITY, 1);
						}
					}
				}
			}
		}
	}

	swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_DESTROYED_CITY", pCity->getNameKey()).GetCString());
	gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYRAZE", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (iI != getID())
			{
				if (pCity->isRevealed(GET_PLAYER((PlayerTypes)iI).getTeam(), false))
				{
					swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_CITY_HAS_BEEN_RAZED_BY", pCity->getNameKey(), getCivilizationDescriptionKey()).GetCString());
					gDLL->getInterfaceIFace()->addHumanMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CITYRAZED", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("WORLDBUILDER_CITY_EDIT")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pCity->getX_INLINE(), pCity->getY_INLINE(), true, true);
				}
			}
		}
	}

	swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_CITY_RAZED_BY", pCity->getNameKey(), getCivilizationDescriptionKey()).GetCString());
	GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer, pCity->getX_INLINE(), pCity->getY_INLINE(), (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));

	// Report this event
	CvEventReporter::getInstance().cityRazed(pCity, getID());

	disband(pCity);
}


void CvPlayer::disband(CvCity* pCity)
{
	if (getNumCities() == 1)
	{
		setFoundedFirstCity(false);
	}

	GC.getGameINLINE().addDestroyedCityName(pCity->getName());

	pCity->kill(true);
}


bool CvPlayer::canReceiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit) const
{
	CvCity* pCity;
	UnitTypes eUnit;
	bool bTechFound;
	int iI;

	if (GC.getGoodyInfo(eGoody).getExperience() > 0)
	{
		if ((pUnit == NULL) || !(pUnit->canAcquirePromotionAny()) || (GC.getGameINLINE().getElapsedGameTurns() < 10))
		{
			return false;
		}
	}

	if (GC.getGoodyInfo(eGoody).getDamagePrereq() > 0)
	{
		if ((pUnit == NULL) || (pUnit->getDamage() < ((pUnit->maxHitPoints() * GC.getGoodyInfo(eGoody).getDamagePrereq()) / 100)))
		{
			return false;
		}
	}

	if (GC.getGoodyInfo(eGoody).isTech())
	{
		bTechFound = false;

		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GC.getTechInfo((TechTypes) iI).isGoodyTech())
			{
				// Civ4 Reimagined
				if (canResearch((TechTypes)iI, false, true) && !GET_TEAM(getTeam()).isTechBoosted((TechTypes)iI))
				{
					bTechFound = true;
					break;
				}
			}
		}

		if (!bTechFound)
		{
			return false;
		}
	}

	if (GC.getGoodyInfo(eGoody).isBad())
	{
		if ((pUnit == NULL) || pUnit->isNoBadGoodies())
		{
			return false;
		}
	}

	if (GC.getGoodyInfo(eGoody).getUnitClassType() != NO_UNITCLASS)
	{
		eUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getGoodyInfo(eGoody).getUnitClassType())));

		if (eUnit == NO_UNIT)
		{
			return false;
		}
		
		if (GC.getGameINLINE().getElapsedGameTurns() < 20)
		{
			return false;
		}

		if ((GC.getUnitInfo(eUnit).getCombat() > 0) && !(GC.getUnitInfo(eUnit).isOnlyDefensive()))
		{
			if (GC.getGameINLINE().isGameMultiPlayer())
			{
				return false;
			}
		}

		if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
		{
			if (GC.getUnitInfo(eUnit).isFound())
			{
				return false;
			}
		}
	}

	if (GC.getGoodyInfo(eGoody).getBarbarianUnitClass() != NO_UNITCLASS)
	{
		//Civ4 Reimagined
		if (GC.getGameINLINE().getElapsedGameTurns() < 20)
		{
			return false;
		}
		
		if (GC.getGameINLINE().isOption(GAMEOPTION_NO_BARBARIANS))
		{
			return false;
		}

		if (getNumCities() == 0)
		{
			return false;
		}

		if (getNumCities() == 1)
		{
			pCity = GC.getMapINLINE().findCity(pPlot->getX_INLINE(), pPlot->getY_INLINE(), NO_PLAYER, getTeam());

			if (pCity != NULL)
			{
				if (plotDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pCity->getX_INLINE(), pCity->getY_INLINE()) <= (8 - getNumCities()))
				{
					return false;
				}
			}
		}
	}
	
	//Civ4 Reimagined goodies start
	if (GC.getGoodyInfo(eGoody).isUpgrade())
	{
		if ((pUnit == NULL) || (pUnit->getUnitInfo().getUnitClassType() != (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_WARRIOR")))
		{
			return false;
		}
	}
	
	if (GC.getGoodyInfo(eGoody).getProduction() > 0)
	{
		if (getNumCities() < 1) {
			return false;
		}
	}
	
	if (GC.getGoodyInfo(eGoody).getFood() > 0)
	{
		if (getNumCities() < 1) {
			return false;
		}
	}

	if (GC.getGoodyInfo(eGoody).getCulture() > 0)
	{
		if (getNumCities() < 1) {
			return false;
		}
	}
	
	if (GC.getGoodyInfo(eGoody).getGreatPersonPoints() > 0)
	{
		if (getNumCities() < 1) {
			return false;
		}
	}

	if (GC.getGoodyInfo(eGoody).isSpecialPromotion()) 
	{
		if (pUnit == NULL || (pUnit->getUnitCombatType() == NO_UNITCOMBAT))
		{
			return false;
		}
	}
	//Civ4 Reimagined goodies end
	
	return true;
}


void CvPlayer::receiveGoody(CvPlot* pPlot, GoodyTypes eGoody, CvUnit* pUnit)
{
	CvPlot* pLoopPlot;
	CvPlot* pBestPlot = NULL;
	CvWString szBuffer;
	CvWString szTempBuffer;
	TechTypes eBestTech;
	UnitTypes eUnit;
	int iGold;
	int iOffset;
	int iRange;
	int iBarbCount;
	int iValue;
	int ipBestValue;
	int iPass;
	int iDX, iDY;
	int iI;

	FAssertMsg(canReceiveGoody(pPlot, eGoody, pUnit), "Instance is expected to be able to recieve goody");

	szBuffer = GC.getGoodyInfo(eGoody).getDescription();

	// Civ4 Reimagined
	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_POLYTHEISM")) && pUnit != NULL)
	{
		GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_POLYTHEISM"), getID(), true);
	}

	// Civ4 Reimagined
	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_BIOLOGY")) && pUnit != NULL)
	{
		if (pUnit->getUnitClassType() == (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_EXPLORER"))
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_BIOLOGY"), getID(), true);
		}
	}

	iGold = GC.getGoodyInfo(eGoody).getGold() + GC.getGameINLINE().getSorenRandNum(GC.getGoodyInfo(eGoody).getGoldRand1(), "Goody Gold 1") + GC.getGameINLINE().getSorenRandNum(GC.getGoodyInfo(eGoody).getGoldRand2(), "Goody Gold 2");
	iGold  = (iGold * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent()) / 100;

	if (iGold != 0)
	{
		changeGold(iGold);

		szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_GOLD", iGold);
	}

	iRange = GC.getGoodyInfo(eGoody).getMapRange();

	if (iRange > 0)
	{
		iOffset = GC.getGoodyInfo(eGoody).getMapOffset();

		if (iOffset > 0)
		{
			ipBestValue = 0;
			pBestPlot = NULL;

			for (iDX = -(iOffset); iDX <= iOffset; iDX++)
			{
				for (iDY = -(iOffset); iDY <= iOffset; iDY++)
				{
					pLoopPlot = plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

					if (pLoopPlot != NULL)
					{
						if (!pLoopPlot->isWater()) // Civ4 Reimagined: Noone likes to see 20 tiles of ocean
						{
							if (!(pLoopPlot->isRevealed(getTeam(), false)))
							{
								iValue = (1 + GC.getGameINLINE().getSorenRandNum(10000, "Goody Map"));

								iValue *= plotDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());

								if (iValue > ipBestValue)
								{
									ipBestValue = iValue;
									pBestPlot = pLoopPlot;
								}
							}
						}
					}
				}
			}
		}

		if (pBestPlot == NULL)
		{
			pBestPlot = pPlot;
		}

		for (iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for (iDY = -(iRange); iDY <= iRange; iDY++)
			{
				pLoopPlot = plotXY(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					if (plotDistance(pBestPlot->getX_INLINE(), pBestPlot->getY_INLINE(), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE()) <= iRange)
					{
						if (GC.getGameINLINE().getSorenRandNum(100, "Goody Map") < GC.getGoodyInfo(eGoody).getMapProb())
						{
							pLoopPlot->setRevealed(getTeam(), true, false, NO_TEAM, true);
						}
					}
				}
			}
		}
	}

	if (pUnit != NULL)
	{
		pUnit->changeExperience(GC.getGoodyInfo(eGoody).getExperience());
	}

	if (pUnit != NULL)
	{
		pUnit->changeDamage(-(GC.getGoodyInfo(eGoody).getHealing()));
	}
	

	//Civ4 Reimagined goodies start
	if (pUnit != NULL && GC.getGoodyInfo(eGoody).isUpgrade())
	{
		eUnit = (UnitTypes)GC.getInfoTypeForString("UNIT_SPEARMAN");
		
		CvUnit* pUpgradeUnit;
		
		pUpgradeUnit = initUnit(eUnit, pUnit->getX_INLINE(), pUnit->getY_INLINE(), pUnit->AI_getUnitAIType());

		FAssertMsg(pUpgradeUnit != NULL, "UpgradeUnit is not assigned a valid value");
		
		pUpgradeUnit->convert(pUnit);
		pUpgradeUnit->joinGroup(pUnit->getGroup());
		pUpgradeUnit->finishMoves();
	}
	
	if (getNumCities() > 0) 
	{
		int iProduction = GC.getGoodyInfo(eGoody).getProduction();
		
		if (iProduction > 0)
		{
			CvCity* pClosestCity = GC.getMapINLINE().findCity(pPlot->getX_INLINE(), pPlot->getY_INLINE(), getID(), getTeam(), false, false);
			if (pClosestCity != NULL)
			{
				iProduction *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent();
				iProduction /= 100;
				
				iProduction = (iProduction+1)/2 + GC.getGameINLINE().getSorenRandNum(iProduction, "Good hut randomization");
				
				pClosestCity->changeOverflowProduction(iProduction, pClosestCity->getProductionModifier());
				//pClosestCity->changeProduction(iProduction);
				
				szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_PRODUCTION", iProduction, pClosestCity->getName().GetCString());
			} else
			{
				FAssertMsg(false, "Cannot find a city for goody hut.");
			}
		}	
	}
	
	if (GC.getGoodyInfo(eGoody).getGreatGeneralPoints() > 0)
	{
		int iGreatGeneralPoints = GC.getGoodyInfo(eGoody).getGreatGeneralPoints();
		
		iGreatGeneralPoints *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGreatPeoplePercent();
		iGreatGeneralPoints /= 100;
		
		iGreatGeneralPoints = (iGreatGeneralPoints+1)/2 + GC.getGameINLINE().getSorenRandNum(iGreatGeneralPoints, "Good hut randomization");
				
		changeCombatExperience(iGreatGeneralPoints);
		szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_GREAT_GENERAL_POINTS", iGreatGeneralPoints);
	}
	
	if (pUnit != NULL && GC.getGoodyInfo(eGoody).isSpecialPromotion())
	{
		PromotionTypes tacticsPromotion = (PromotionTypes)GC.getInfoTypeForString("PROMOTION_TACTICS");
		PromotionTypes leadershipPromotion = (PromotionTypes)GC.getInfoTypeForString("PROMOTION_LEADERSHIP");
		PromotionTypes moralePromotion = (PromotionTypes)GC.getInfoTypeForString("PROMOTION_MORALE");
		PromotionTypes commandoPromotion = (PromotionTypes)GC.getInfoTypeForString("PROMOTION_COMMANDO");
		std::vector<const PromotionTypes> tPromotion;
		PromotionTypes selectedPromotion;
	
		if (!pUnit->isHasPromotion(tacticsPromotion) 
			&& (pUnit->getUnitCombatType() != (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_RECON"))
			&& (pUnit->getUnitCombatType() != (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_ARCHER"))
			&& (pUnit->getUnitCombatType() != (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_SIEGE")))
		{
			tPromotion.push_back(tacticsPromotion);
		}
	
		if (!pUnit->isHasPromotion(leadershipPromotion)) 
		{
			tPromotion.push_back(leadershipPromotion);
		}
		
		if (!pUnit->isHasPromotion(moralePromotion)) 
		{
			tPromotion.push_back(moralePromotion);
		}
		
		if (!pUnit->isHasPromotion(commandoPromotion)
			&& (pUnit->getUnitCombatType() != (UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_RECON"))) 
		{
			tPromotion.push_back(commandoPromotion);
		}
		
		
		if (!tPromotion.empty()) 
		{
			int randomElement = GC.getGameINLINE().getSorenRandNum(tPromotion.size(), "Goody Special Promotion");
		
			selectedPromotion = tPromotion.at(randomElement);
			pUnit->setHasPromotion(selectedPromotion, true);
			
			if (selectedPromotion == tacticsPromotion) 
			{				
				szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_PROMOTION_TACTICS");
			}
			if (selectedPromotion == leadershipPromotion) 
			{				
				szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_PROMOTION_LEADERSHIP");
			}
			if (selectedPromotion == moralePromotion) 
			{				
				szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_PROMOTION_MORALE");
			}
			if (selectedPromotion == commandoPromotion) 
			{				
				szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_PROMOTION_COMMANDO");
			}

		} else
		{
			pUnit->changeExperience(10);
			szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_PROMOTION_NONE");
		}
		
	}
	
	if (getNumCities() > 0) 
	{
		int iFood = GC.getGoodyInfo(eGoody).getFood();
		
		if (iFood > 0)
		{
			CvCity* pClosestCity = GC.getMapINLINE().findCity(pPlot->getX_INLINE(), pPlot->getY_INLINE(), getID(), getTeam(), false, false);
			if (pClosestCity != NULL)
			{
				iFood *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
				iFood /= 100;
				
				iFood = (iFood+1)/2 + GC.getGameINLINE().getSorenRandNum(iFood, "Good hut randomization");
		
				pClosestCity->changeFood(iFood);
				CvWString szCityName = pClosestCity->getName();
			
				szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_FOOD", iFood, pClosestCity->getName().GetCString());
			} else
			{
				FAssertMsg(false, "Cannot find a city for goody hut.");
			}
		}	
	}
	
	if (getNumCities() > 0) 
	{
		int iCulture = GC.getGoodyInfo(eGoody).getCulture();
		
		if (iCulture > 0)
		{
			CvCity* pClosestCity = GC.getMapINLINE().findCity(pPlot->getX_INLINE(), pPlot->getY_INLINE(), getID(), getTeam(), false, false);
			if (pClosestCity != NULL)
			{		
				iCulture = (iCulture+1)/2 + GC.getGameINLINE().getSorenRandNum(iCulture, "Good hut randomization");
		
				pClosestCity->changeCultureTimes100(getID(), 100*iCulture, true, true);
			
				szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_CULTURE", iCulture, pClosestCity->getName().GetCString());
			} else
			{
				FAssertMsg(false, "Cannot find a city for goody hut.");
			}
		}	
	}
	
	if (getNumCities() > 0) 
	{
		int iGreatPersonPoints = GC.getGoodyInfo(eGoody).getGreatPersonPoints();
		
		if (iGreatPersonPoints > 0)
		{
			iGreatPersonPoints *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGreatPeoplePercent();
			iGreatPersonPoints /= 100;

			iGreatPersonPoints = (iGreatPersonPoints*3)/4 + GC.getGameINLINE().getSorenRandNum((iGreatPersonPoints+1)/2, "Good hut randomization");
			
			CvCity* pBestCity;
			int pBestValue = -1;
			
			CvCity* pLoopCity;
			int iLoop;
			
			for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				int greatPeopleProgress = pLoopCity->getGreatPeopleProgress();

				if (greatPeopleProgress > pBestValue)
				{
					pBestCity = pLoopCity;
					pBestValue = greatPeopleProgress;
				}
			}
			
			if (pBestCity != NULL)
			{
				pBestCity->changeGreatPeopleProgress(iGreatPersonPoints);
			
				szBuffer += gDLL->getText("TXT_KEY_MISC_RECEIVED_GREAT_PERSON_POINTS", iGreatPersonPoints, pBestCity->getName().GetCString());
			} else
			{
				FAssertMsg(false, "Cannot find a city for goody hut.");
			}
		}	
	}

	if (GC.getGoodyInfo(eGoody).isTech())
	{
		ipBestValue = 0;
		eBestTech = NO_TECH;

		for (iI = 0; iI < GC.getNumTechInfos(); iI++)
		{
			if (GC.getTechInfo((TechTypes) iI).isGoodyTech())
			{
				if (canResearch((TechTypes)iI, false, true) && !GET_TEAM(getTeam()).isTechBoosted((TechTypes)iI))
				{
					iValue = (1 + GC.getGameINLINE().getSorenRandNum(10000, "Goody Tech"));

					if (iValue > ipBestValue)
					{
						ipBestValue = iValue;
						eBestTech = ((TechTypes)iI);
					}
				}
			}
		}

		FAssertMsg(eBestTech != NO_TECH, "BestTech is not assigned a valid value");

		GET_TEAM(getTeam()).setTechBoosted(eBestTech, getID(), true);
	}

	//Civ4 Reimagined goodies end	

	if (GC.getGoodyInfo(eGoody).getUnitClassType() != NO_UNITCLASS)
	{
		eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getGoodyInfo(eGoody).getUnitClassType());

		if (eUnit != NO_UNIT)
		{
			initUnit(eUnit, pPlot->getX_INLINE(), pPlot->getY_INLINE());
		}
	}

	if (GC.getGoodyInfo(eGoody).getBarbarianUnitClass() != NO_UNITCLASS)
	{
		iBarbCount = 0;

		eUnit = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(BARBARIAN_PLAYER).getCivilizationType()).getCivilizationUnits(GC.getGoodyInfo(eGoody).getBarbarianUnitClass());

		if (eUnit != NO_UNIT)
		{
			for (iPass = 0; iPass < 2; iPass++)
			{
				if (iBarbCount < GC.getGoodyInfo(eGoody).getMinBarbarians())
				{
					for (iI = 0; iI < NUM_DIRECTION_TYPES; iI++)
					{
						pLoopPlot = plotDirection(pPlot->getX_INLINE(), pPlot->getY_INLINE(), ((DirectionTypes)iI));

						if (pLoopPlot != NULL)
						{
							if (pLoopPlot->getArea() == pPlot->getArea())
							{
								if (!(pLoopPlot->isImpassable()))
								{
									if (pLoopPlot->getNumUnits() == 0)
									{
										if ((iPass > 0) || (GC.getGameINLINE().getSorenRandNum(100, "Goody Barbs") < GC.getGoodyInfo(eGoody).getBarbarianUnitProb()))
										{
											GET_PLAYER(BARBARIAN_PLAYER).initUnit(eUnit, pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), ((pLoopPlot->isWater()) ? UNITAI_ATTACK_SEA : UNITAI_ATTACK));
											iBarbCount++;

											if ((iPass > 0) && (iBarbCount == GC.getGoodyInfo(eGoody).getMinBarbarians()))
											{
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
	}
	
	// Civ4 Reimagined: Moved this to the end.
	if (!szBuffer.empty())
	{
		gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, GC.getGoodyInfo(eGoody).getSound(), MESSAGE_TYPE_MINOR_EVENT, ARTFILEMGR.getImprovementArtInfo("ART_DEF_IMPROVEMENT_GOODY_HUT")->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pPlot->getX_INLINE(), pPlot->getY_INLINE());
	}
}


void CvPlayer::doGoody(CvPlot* pPlot, CvUnit* pUnit)
{
	CyPlot kGoodyPlot(pPlot);
	CyUnit kGoodyUnit(pUnit);
	CyArgsList argsList;
	argsList.add(getID());		// pass in this players ID
	argsList.add(gDLL->getPythonIFace()->makePythonObject(&kGoodyPlot));
	argsList.add(gDLL->getPythonIFace()->makePythonObject(&kGoodyUnit));

	long result=0;
	bool ok = gDLL->getPythonIFace()->callFunction(PYGameModule, "doGoody", argsList.makeFunctionArgs(), &result);
	if (ok && result)
	{
		return;
	}

	FAssertMsg(pPlot->isGoody(), "pPlot->isGoody is expected to be true");

	pPlot->removeGoody();
	if (!isBarbarian())
	{
		for (int iI = 0; iI < GC.getDefineINT("NUM_DO_GOODY_ATTEMPTS"); iI++)
		{
			if (GC.getHandicapInfo(getHandicapType()).getNumGoodies() > 0)
			{
				GoodyTypes eGoody = (GoodyTypes)GC.getHandicapInfo(getHandicapType()).getGoodies(GC.getGameINLINE().getSorenRandNum(GC.getHandicapInfo(getHandicapType()).getNumGoodies(), "Goodies"));
	
				FAssert(eGoody >= 0);
				FAssert(eGoody < GC.getNumGoodyInfos());

				if (canReceiveGoody(pPlot, eGoody, pUnit))
				{
					receiveGoody(pPlot, eGoody, pUnit);

					// Python Event
					CvEventReporter::getInstance().goodyReceived(getID(), pPlot, pUnit, eGoody);
					break;
				}
			}
		}
	}
}


bool CvPlayer::canFound(int iX, int iY, bool bTestVisible) const
{
	CvPlot* pPlot;
	CvPlot* pLoopPlot;
	bool bValid;
	int iRange;
	int iDX, iDY;

	pPlot = GC.getMapINLINE().plotINLINE(iX, iY);

	long lResult=0;
	if(GC.getUSE_CANNOT_FOUND_CITY_CALLBACK())
	{
		CyArgsList argsList;
		argsList.add((int)getID());
		argsList.add(iX);
		argsList.add(iY);
		gDLL->getPythonIFace()->callFunction(PYGameModule, "cannotFoundCity", argsList.makeFunctionArgs(), &lResult);

		if (lResult == 1)
		{
			return false;
		}
	}

	if (GC.getGameINLINE().isFinalInitialized())
	{
		if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
		{
			if (getNumCities() > 0)
			{
				return false;
			}
		}
	}

	if (pPlot->isImpassable())
	{
		return false;
	}

	if (pPlot->getFeatureType() != NO_FEATURE)
	{
		if (GC.getFeatureInfo(pPlot->getFeatureType()).isNoCity())
		{
			return false;
		}
	}

	if (pPlot->isOwned() && (pPlot->getOwnerINLINE() != getID()))
	{
		return false;
	}

	bValid = false;

	if (!bValid)
	{
		if (GC.getTerrainInfo(pPlot->getTerrainType()).isFound())
		{
			bValid = true;
		}
	}

	if (!bValid)
	{
		if (GC.getTerrainInfo(pPlot->getTerrainType()).isFoundCoast())
		{
			if (pPlot->isCoastalLand())
			{
				bValid = true;
			}
		}
	}

	if (!bValid)
	{
		if (GC.getTerrainInfo(pPlot->getTerrainType()).isFoundFreshWater())
		{
			if (pPlot->isFreshWater())
			{
				bValid = true;
			}
		}
	}

/************************************************************************************************/
/* UNOFFICIAL_PATCH                       02/16/10                    EmperorFool & jdog5000    */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
	// EF: canFoundCitiesOnWater callback handling was incorrect and ignored isWater() if it returned true
	if (pPlot->isWater())
	{
		if(GC.getUSE_CAN_FOUND_CITIES_ON_WATER_CALLBACK())
		{
			bValid = false;

			CyArgsList argsList2;
			argsList2.add(iX);
			argsList2.add(iY);
			lResult=0;
			gDLL->getPythonIFace()->callFunction(PYGameModule, "canFoundCitiesOnWater", argsList2.makeFunctionArgs(), &lResult);

			if (lResult == 1)
			{
				bValid = true;
			}
		}
	}
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/

	if (!bValid)
	{
		return false;
	}

	if (!bTestVisible)
	{
		iRange = GC.getMIN_CITY_RANGE();

		for (iDX = -(iRange); iDX <= iRange; iDX++)
		{
			for (iDY = -(iRange); iDY <= iRange; iDY++)
			{
				pLoopPlot	= plotXY(pPlot->getX_INLINE(), pPlot->getY_INLINE(), iDX, iDY);

				if (pLoopPlot != NULL)
				{
					if (pLoopPlot->isCity())
					{
						if (pLoopPlot->area() == pPlot->area())
						{
							return false;
						}
					}
				}
			}
		}
	}

	return true;
}


void CvPlayer::found(int iX, int iY)
{
	CvCity* pCity;
	BuildingTypes eLoopBuilding;
	UnitTypes eDefenderUnit;
	int iI;

	if (!canFound(iX, iY))
	{
		return;
	}

	// Civ4 Reimagined
	applyNativeAmericanBonus(iX, iY);

	pCity = initCity(iX, iY, true, true);
	FAssertMsg(pCity != NULL, "City is not assigned a valid value");

	if (isBarbarian())
	{
		eDefenderUnit = pCity->AI_bestUnitAI(UNITAI_CITY_DEFENSE);

		if (eDefenderUnit == NO_UNIT)
		{
			eDefenderUnit = pCity->AI_bestUnitAI(UNITAI_ATTACK);
		}

		if (eDefenderUnit != NO_UNIT)
		{
			for (iI = 0; iI < GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getBarbarianInitialDefenders(); iI++)
			{
				initUnit(eDefenderUnit, iX, iY, UNITAI_CITY_DEFENSE);
			}
		}
	}

	for (iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		eLoopBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI)));

		if (eLoopBuilding != NO_BUILDING)
		{
			if (GC.getBuildingInfo(eLoopBuilding).getFreeStartEra() != NO_ERA)
			{
				if (GC.getGameINLINE().getStartEra() >= GC.getBuildingInfo(eLoopBuilding).getFreeStartEra())
				{
					if (pCity->canConstruct(eLoopBuilding))
					{
						pCity->setNumRealBuilding(eLoopBuilding, 1);
					}
				}
			}
		}
	}

	if (getAdvancedStartPoints() >= 0)
	{
		// Free border expansion for Creative
		bool bCreative = false;
		for (iI = 0; iI < GC.getNumTraitInfos(); ++iI)
		{
			if (hasTrait((TraitTypes)iI))
			{
				if (GC.getTraitInfo((TraitTypes)iI).getCommerceChange(COMMERCE_CULTURE) > 0)
				{
					bCreative = true;
					break;
				}

			}
		}

		if (bCreative)
		{
			for (iI = 0; iI < GC.getNumCultureLevelInfos(); ++iI)
			{
				int iCulture = GC.getGameINLINE().getCultureThreshold((CultureLevelTypes)iI);
				if (iCulture > 0)
				{
					pCity->setCulture(getID(), iCulture, true, true);
					break;
				}
			}
		}
	}

	// Civ4 Reimagined: Slaves in colonies
	if (pCity->isColony())
	{
		if (hasSlavery() && GET_TEAM(getTeam()).isTerrainTrade((TerrainTypes)GC.getInfoTypeForString("TERRAIN_OCEAN")))
		{
			gainSlavePoints(GC.getDefineINT("NEW_COLONY_SLAVE_POINTS"), pCity);
		}
	}
	
	if (isHuman() && getAdvancedStartPoints() < 0)
	{
		pCity->chooseProduction();
	}
	else
	{
		pCity->doFoundMessage();
	}

	CvEventReporter::getInstance().cityBuilt(pCity);

	// Civ4 Reimagined
	if (pCity->isCoastal(GC.getMIN_WATER_SIZE_FOR_OCEAN()))
	{
		GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_SAILING"), getID(), true);
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
/*                                                                                              */
/* AI logging                                                                                   */
/************************************************************************************************/
	if( gPlayerLogLevel >= 1 )
	{
		logBBAI("  Player %d (%S) founds new city %S at %d, %d", getID(), getCivilizationDescription(0), pCity->getName(0).GetCString(), iX, iY );
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
}


bool CvPlayer::canTrain(UnitTypes eUnit, bool bContinue, bool bTestVisible, bool bIgnoreCost) const
{
	PROFILE_FUNC();

	UnitClassTypes eUnitClass;
	int iI;

	eUnitClass = ((UnitClassTypes)(GC.getUnitInfo(eUnit).getUnitClassType()));

	// K-Mod note. This assert can fail if team games when checking whether this city can upgrade a unit to one of our team member's UUs.
	//FAssert(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass) == eUnit);
	if (GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass) != eUnit)
	{
		return false;
	}

	if (!bIgnoreCost)
	{
		if (GC.getUnitInfo(eUnit).getProductionCost() == -1)
		{
			return false;
		}
	}

	if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		if (GC.getUnitInfo(eUnit).isFound())
		{
			return false;
		}
	}

	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		if (GC.getUnitInfo(eUnit).isSpy() || GC.getUnitInfo(eUnit).getEspionagePoints() > 0)
		{
			return false;
		}
	}

	if (!(GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getUnitInfo(eUnit).getPrereqAndTech()))))
	{
		return false;
	}

	for (iI = 0; iI < GC.getNUM_UNIT_AND_TECH_PREREQS(); iI++)
	{
		if (GC.getUnitInfo(eUnit).getPrereqAndTechs(iI) != NO_TECH)
		{
			if (!(GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getUnitInfo(eUnit).getPrereqAndTechs(iI)))))
			{
				return false;
			}
		}
	}

	if (GC.getUnitInfo(eUnit).getStateReligion() != NO_RELIGION)
	{
		if (getStateReligion() != GC.getUnitInfo(eUnit).getStateReligion())
		{
			return false;
		}
	}

	// Civ4 Reimagined
	if (GC.getGameINLINE().isUnitClassMaxedOut(eUnitClass) && !GC.getUnitInfo(eUnit).isIgnoreMaxInstances())
	{
		return false;
	}
	
	// Civ4 Reimagined
	if (GC.getUnitInfo(eUnit).getPrereqAndBonus() != NO_BONUS)
	{
		if (GET_TEAM(getTeam()).isBonusObsolete((BonusTypes)GC.getUnitInfo(eUnit).getPrereqAndBonus()))
		{
			return false;
		}
	}

	/* original bts code
	if (GET_TEAM(getTeam()).isUnitClassMaxedOut(eUnitClass))
	{
		return false;
	}

	if (isUnitClassMaxedOut(eUnitClass))
	{
		return false;
	} */  // disabled by K-Mod.
	// Note that unlike the global limit, these two limits apply to the number of units currently alive rather than the total ever trained.
	// Therefore these limits should be ignored for the visibility test.

	if (!bTestVisible)
	{
		if (GC.getGameINLINE().isUnitClassMaxedOut(eUnitClass, (GET_TEAM(getTeam()).getUnitClassMaking(eUnitClass) + ((bContinue) ? -1 : 0))) && !GC.getUnitInfo(eUnit).isIgnoreMaxInstances())
		{
			return false;
		}

		if (GET_TEAM(getTeam()).isUnitClassMaxedOut(eUnitClass, (GET_TEAM(getTeam()).getUnitClassMaking(eUnitClass) + ((bContinue) ? -1 : 0))) && !GC.getUnitInfo(eUnit).isIgnoreMaxInstances())
		{
			return false;
		}

		if (isUnitClassMaxedOut(eUnitClass, (getUnitClassMaking(eUnitClass) + ((bContinue) ? -1 : 0))) && !GC.getUnitInfo(eUnit).isIgnoreMaxInstances())
		{
			return false;
		}

		/* original bts code
		if (GC.getGameINLINE().isNoNukes() || !GC.getGameINLINE().isNukesValid())
		{
			if (GC.getUnitInfo(eUnit).getNukeRange() != -1)
			{
				return false;
			}
		} */ // disabled by Civ4 Reimagined
		// Nukes can now always be built

		if (GC.getUnitInfo(eUnit).getSpecialUnitType() != NO_SPECIALUNIT)
		{
			if (!(GC.getGameINLINE().isSpecialUnitValid((SpecialUnitTypes)(GC.getUnitInfo(eUnit).getSpecialUnitType()))))
			{
				return false;
			}
		}
	}

	return true;
}


bool CvPlayer::canConstruct(BuildingTypes eBuilding, bool bContinue, bool bTestVisible, bool bIgnoreCost, bool bIgnoreTech, bool bIgnoreCivic) const
{
	BuildingClassTypes eBuildingClass;
	int iI;
	CvTeamAI& currentTeam = GET_TEAM(getTeam());

	eBuildingClass = ((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType()));

	//FAssert(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass) == eBuilding);
	if (GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass) != eBuilding)
	{
		return false;
	}

	// Civ4 Reimagined
	if (GC.getBuildingInfo(eBuilding).isUniquePower() && getUniquePowerBuilding() != eBuilding)
	{
		return false;
	}

	if (!bIgnoreCost)
	{
		if (GC.getBuildingInfo(eBuilding).getProductionCost() == -1)
		{
			return false;
		}
	}

	if (!bIgnoreTech) // K-Mod
	{
		if (!(currentTeam.isHasTech((TechTypes)(GC.getBuildingInfo(eBuilding).getPrereqAndTech()))))
		{
			return false;
		}

		for (iI = 0; iI < GC.getNUM_BUILDING_AND_TECH_PREREQS(); iI++)
		{
			if (GC.getBuildingInfo(eBuilding).getPrereqAndTechs(iI) != NO_TECH)
			{
				if (!(currentTeam.isHasTech((TechTypes)(GC.getBuildingInfo(eBuilding).getPrereqAndTechs(iI)))))
				{
					return false;
				}
			}
		}
	}
	
	// Civ4 Reimagined
	if (!bIgnoreCivic)
	{
		for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			if (GC.getBuildingInfo(eBuilding).isPrereqCivic(iI))
			{
				if (!isCivic((CivicTypes)iI))
				{
					return false;
				}
			}
		}
	}

	if (currentTeam.isObsoleteBuilding(eBuilding))
	{
		return false;
	}

	if (GC.getBuildingInfo(eBuilding).getSpecialBuildingType() != NO_SPECIALBUILDING)
	{
		if (!(currentTeam.isHasTech((TechTypes)(GC.getSpecialBuildingInfo((SpecialBuildingTypes) GC.getBuildingInfo(eBuilding).getSpecialBuildingType()).getTechPrereq()))))
		{
			return false;
		}
	}

	if (GC.getBuildingInfo(eBuilding).getStateReligion() != NO_RELIGION)
	{
		if (getStateReligion() != GC.getBuildingInfo(eBuilding).getStateReligion())
		{
			return false;
		}
	}

	if (GC.getGameINLINE().countCivTeamsEverAlive() < GC.getBuildingInfo(eBuilding).getNumTeamsPrereq())
	{
		return false;
	}

	if (GC.getBuildingInfo(eBuilding).getVictoryPrereq() != NO_VICTORY)
	{
		if (!(GC.getGameINLINE().isVictoryValid((VictoryTypes)(GC.getBuildingInfo(eBuilding).getVictoryPrereq()))))
		{
			return false;
		}

		if (isMinorCiv())
		{
			return false;
		}

		if (currentTeam.getVictoryCountdown((VictoryTypes)GC.getBuildingInfo(eBuilding).getVictoryPrereq()) >= 0)
		{
			return false;
		}
	}

	if (GC.getBuildingInfo(eBuilding).getMaxStartEra() != NO_ERA)
	{
		if (GC.getGameINLINE().getStartEra() > GC.getBuildingInfo(eBuilding).getMaxStartEra())
		{
			return false;
		}
	}

	if (GC.getGameINLINE().isBuildingClassMaxedOut(eBuildingClass))
	{
		return false;
	}

	if (currentTeam.isBuildingClassMaxedOut(eBuildingClass))
	{
		return false;
	}

	if (isBuildingClassMaxedOut(eBuildingClass))
	{
		return false;
	}

	CvCivilizationInfo &civilizationInfo = GC.getCivilizationInfo(getCivilizationType());
	int numBuildingClassInfos = GC.getNumBuildingClassInfos();
	for (iI = 0; iI < numBuildingClassInfos; iI++)
	{
		BuildingTypes ePrereqBuilding = (BuildingTypes)civilizationInfo.getCivilizationBuildings(iI);

		if (NO_BUILDING != ePrereqBuilding && currentTeam.isObsoleteBuilding(ePrereqBuilding))
		{
			if (getBuildingClassCount((BuildingClassTypes)iI) < getBuildingClassPrereqBuilding(eBuilding, (BuildingClassTypes)iI, 0))
			{
				return false;
			}
		}
	}

	if (!bTestVisible)
	{
		if (GC.getGameINLINE().isBuildingClassMaxedOut(eBuildingClass, (currentTeam.getBuildingClassMaking(eBuildingClass) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}

		if (currentTeam.isBuildingClassMaxedOut(eBuildingClass, (currentTeam.getBuildingClassMaking(eBuildingClass) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}

		if (isBuildingClassMaxedOut(eBuildingClass, (getBuildingClassMaking(eBuildingClass) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}

		if (GC.getGameINLINE().isNoNukes())
		{
			if (GC.getBuildingInfo(eBuilding).isAllowsNukes())
			{
				for (iI = 0; iI < GC.getNumUnitInfos(); iI++)
				{
					if (GC.getUnitInfo((UnitTypes)iI).getNukeRange() != -1)
					{
						return false;
					}
				}
			}
		}

		if (GC.getBuildingInfo(eBuilding).getSpecialBuildingType() != NO_SPECIALBUILDING)
		{
			if (!(GC.getGameINLINE().isSpecialBuildingValid((SpecialBuildingTypes)(GC.getBuildingInfo(eBuilding).getSpecialBuildingType()))))
			{
				return false;
			}
		}

		if (getNumCities() < GC.getBuildingInfo(eBuilding).getNumCitiesPrereq())
		{
			return false;
		}

		if (getHighestUnitLevel() < GC.getBuildingInfo(eBuilding).getUnitLevelPrereq())
		{
			return false;
		}

		// Civ4 Reimagined
		if (getHighestNavalUnitLevel() < GC.getBuildingInfo(eBuilding).getNavalLevelPrereq())
		{
			return false;
		}

		// Civ4 Reimagined
		if (!bIgnoreCivic)
		{
			for (int iI = 0; iI < GC.getNumIdeologyInfos(); iI++)
			{
				if (GC.getBuildingInfo(eBuilding).getPrereqIdeology() == (IdeologyTypes)iI)
				{
					if (getIdeology() != (IdeologyTypes)iI)
					{
						return false;
					}
				}
			}
		}
		
		for (iI = 0; iI < numBuildingClassInfos; iI++)
		{
			if (getBuildingClassCount((BuildingClassTypes)iI) < getBuildingClassPrereqBuilding(eBuilding, ((BuildingClassTypes)iI), ((bContinue) ? 0 : getBuildingClassMaking(eBuildingClass))))
			{
				return false;
			}
		}
	}

	return true;
}


bool CvPlayer::canCreate(ProjectTypes eProject, bool bContinue, bool bTestVisible) const
{
	int iI;

	if (isBarbarian())
	{
		return false;
	}

	if (GC.getProjectInfo(eProject).getProductionCost() == -1)
	{
		return false;
	}

	if (!(GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getProjectInfo(eProject).getTechPrereq()))))
	{
		return false;
	}
	
	// Civ4 Reimagined
	if (!(GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getProjectInfo(eProject).getTechPrereq2()))))
	{
		return false;
	}

	if (GC.getProjectInfo(eProject).getVictoryPrereq() != NO_VICTORY)
	{
		if (!(GC.getGameINLINE().isVictoryValid((VictoryTypes)(GC.getProjectInfo(eProject).getVictoryPrereq()))))
		{
			return false;
		}

		if (isMinorCiv())
		{
			return false;
		}

		if (GET_TEAM(getTeam()).getVictoryCountdown((VictoryTypes)GC.getProjectInfo(eProject).getVictoryPrereq()) >= 0)
		{
			return false;
		}
	}

	if (GC.getGameINLINE().isProjectMaxedOut(eProject))
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject))
	{
		return false;
	}

	if (!bTestVisible)
	{
		if (GC.getGameINLINE().isProjectMaxedOut(eProject, (GET_TEAM(getTeam()).getProjectMaking(eProject) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}

		if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject, (GET_TEAM(getTeam()).getProjectMaking(eProject) + ((bContinue) ? -1 : 0))))
		{
			return false;
		}

		if (GC.getGameINLINE().isNoNukes())
		{
			if (GC.getProjectInfo(eProject).isAllowsNukes())
			{
				for (iI = 0; iI < GC.getNumUnitInfos(); iI++)
				{
					if (GC.getUnitInfo((UnitTypes)iI).getNukeRange() != -1)
					{
						return false;
					}
				}
			}
		}

		if (GC.getProjectInfo(eProject).getAnyoneProjectPrereq() != NO_PROJECT)
		{
			if (GC.getGameINLINE().getProjectCreatedCount((ProjectTypes)(GC.getProjectInfo(eProject).getAnyoneProjectPrereq())) == 0)
			{
				return false;
			}
		}

		// Civ4 Reimagined
		if ((IdeologyTypes)GC.getProjectInfo(eProject).getIdeologyPrereq() != NO_IDEOLOGY && getIdeology() != (IdeologyTypes)GC.getProjectInfo(eProject).getIdeologyPrereq())
		{
			return false;
		}

		for (iI = 0; iI < GC.getNumProjectInfos(); iI++)
		{
			if (GET_TEAM(getTeam()).getProjectCount((ProjectTypes)iI) < GC.getProjectInfo(eProject).getProjectsNeeded(iI))
			{
				return false;
			}
		}
	}

	return true;
}


bool CvPlayer::canMaintain(ProcessTypes eProcess, bool bContinue) const
{
	if (!(GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getProcessInfo(eProcess).getTechPrereq()))))
	{
		return false;
	}

	return true;
}


bool CvPlayer::isProductionMaxedUnitClass(UnitClassTypes eUnitClass) const
{
	if (eUnitClass == NO_UNITCLASS)
	{
		return false;
	}

	if (GC.getGameINLINE().isUnitClassMaxedOut(eUnitClass))
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isUnitClassMaxedOut(eUnitClass))
	{
		return true;
	}

	if (isUnitClassMaxedOut(eUnitClass))
	{
		return true;
	}

	return false;
}


bool CvPlayer::isProductionMaxedBuildingClass(BuildingClassTypes eBuildingClass, bool bAcquireCity) const
{
	if (eBuildingClass == NO_BUILDINGCLASS)
	{
		return false;
	}

	if (!bAcquireCity)
	{
		if (GC.getGameINLINE().isBuildingClassMaxedOut(eBuildingClass))
		{
			return true;
		}
	}

	if (GET_TEAM(getTeam()).isBuildingClassMaxedOut(eBuildingClass))
	{
		return true;
	}

	if (isBuildingClassMaxedOut(eBuildingClass, ((bAcquireCity) ? GC.getBuildingClassInfo(eBuildingClass).getExtraPlayerInstances() : 0)))
	{
		return true;
	}

	return false;
}


bool CvPlayer::isProductionMaxedProject(ProjectTypes eProject) const
{
	if (eProject == NO_PROJECT)
	{
		return false;
	}

	if (GC.getGameINLINE().isProjectMaxedOut(eProject))
	{
		return true;
	}

	if (GET_TEAM(getTeam()).isProjectMaxedOut(eProject))
	{
		return true;
	}

	return false;
}


int CvPlayer::getProductionNeeded(UnitTypes eUnit) const
{
	UnitClassTypes eUnitClass = (UnitClassTypes)GC.getUnitInfo(eUnit).getUnitClassType();
	FAssert(NO_UNITCLASS != eUnitClass);

	int iProductionNeeded = GC.getUnitInfo(eUnit).getProductionCost();

	iProductionNeeded *= 100 + getUnitClassCount(eUnitClass) * GC.getUnitClassInfo(eUnitClass).getInstanceCostModifier();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getDefineINT("UNIT_PRODUCTION_PERCENT");
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getTrainPercent();
	iProductionNeeded /= 100;

	// Civ4 Reimagind
	iProductionNeeded *= 100 + GC.getUnitInfo(eUnit).getEraCostModifier() * getCurrentEra();
	iProductionNeeded /= 100;

	if (!isHuman() && !isBarbarian())
	{
		if (isWorldUnitClass(eUnitClass))
		{
			iProductionNeeded *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIWorldTrainPercent();
			iProductionNeeded /= 100;
		}
		else
		{
			iProductionNeeded *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAITrainPercent();
			iProductionNeeded /= 100;
		}

		iProductionNeeded *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iProductionNeeded /= 100;
	}

	iProductionNeeded += getUnitExtraCost(eUnitClass);

	// Python cost modifier
	if(GC.getUSE_GET_UNIT_COST_MOD_CALLBACK())
	{
		CyArgsList argsList;
		argsList.add(getID());	// Player ID
		argsList.add((int)eUnit);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "getUnitCostMod", argsList.makeFunctionArgs(), &lResult);

		if (lResult > 1)
		{
			iProductionNeeded *= lResult;
			iProductionNeeded /= 100;
		}
	}

	return std::max(1, iProductionNeeded);
}


int CvPlayer::getProductionNeeded(BuildingTypes eBuilding) const
{
	int iProductionNeeded;

	iProductionNeeded = GC.getBuildingInfo(eBuilding).getProductionCost();

	iProductionNeeded *= GC.getDefineINT("BUILDING_PRODUCTION_PERCENT");
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getConstructPercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getConstructPercent();
	iProductionNeeded /= 100;

	if (!isHuman() && !isBarbarian())
	{
		if (isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType())))
		{
			iProductionNeeded *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIWorldConstructPercent();
			iProductionNeeded /= 100;
		}
		else
		{
			iProductionNeeded *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIConstructPercent();
			iProductionNeeded /= 100;
		}

		iProductionNeeded *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iProductionNeeded /= 100;
	}

	return std::max(1, iProductionNeeded);
}


int CvPlayer::getProductionNeeded(ProjectTypes eProject) const
{
	int iProductionNeeded;

	iProductionNeeded = GC.getProjectInfo(eProject).getProductionCost();

	iProductionNeeded *= GC.getDefineINT("PROJECT_PRODUCTION_PERCENT");
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getCreatePercent();
	iProductionNeeded /= 100;

	iProductionNeeded *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getCreatePercent();
	iProductionNeeded /= 100;

	if (!isHuman() && !isBarbarian())
	{
		if (isWorldProject(eProject))
		{
			iProductionNeeded *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIWorldCreatePercent();
			iProductionNeeded /= 100;
		}
		else
		{
			iProductionNeeded *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAICreatePercent();
			iProductionNeeded /= 100;
		}

		iProductionNeeded *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iProductionNeeded /= 100;
	}

	return std::max(1, iProductionNeeded);
}

int CvPlayer::getProductionModifier(UnitTypes eUnit) const
{
	int iMultiplier = 0;

	if (GC.getUnitInfo(eUnit).isMilitaryProduction())
	{
		iMultiplier += getMilitaryProductionModifier();
	}
	
	// Leoreth: civic modifier for specific domains
	if (GC.getUnitInfo(eUnit).getDomainType() != NO_DOMAIN)
	{
		iMultiplier += getDomainProductionModifier((DomainTypes)GC.getUnitInfo(eUnit).getDomainType());
	}

	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			iMultiplier += GC.getUnitInfo(eUnit).getProductionTraits(iI);

			if (GC.getUnitInfo(eUnit).getSpecialUnitType() != NO_SPECIALUNIT)
			{
				iMultiplier += GC.getSpecialUnitInfo((SpecialUnitTypes) GC.getUnitInfo(eUnit).getSpecialUnitType()).getProductionTraits(iI);
			}
		}
	}

	return iMultiplier;
}

int CvPlayer::getProductionModifier(BuildingTypes eBuilding) const
{
	int iMultiplier = 0;
	for (int iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			iMultiplier += GC.getBuildingInfo(eBuilding).getProductionTraits(iI);

			if (GC.getBuildingInfo(eBuilding).getSpecialBuildingType() != NO_SPECIALBUILDING)
			{
				iMultiplier += GC.getSpecialBuildingInfo((SpecialBuildingTypes) GC.getBuildingInfo(eBuilding).getSpecialBuildingType()).getProductionTraits(iI);
			}
		}
	}
	
	// Leoreth: civics
	iMultiplier += getBuildingProductionModifier(eBuilding);

	if (::isWorldWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType())))
	{
		iMultiplier += getMaxGlobalBuildingProductionModifier();
	}

	if (::isTeamWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType())))
	{
		iMultiplier += getMaxTeamBuildingProductionModifier();
	}

	if (::isNationalWonderClass((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType())))
	{
		iMultiplier += getMaxPlayerBuildingProductionModifier();
	}
	
	// Civ4 Reimagined
	iMultiplier += getBuildingsProductionModifier();

	return iMultiplier;
}

int CvPlayer::getProductionModifier(ProjectTypes eProject) const
{
	int iMultiplier = 0;

	if (GC.getProjectInfo(eProject).isSpaceship())
	{
		iMultiplier += getSpaceProductionModifier();
	}

	return iMultiplier;
}

int CvPlayer::getBuildingClassPrereqBuilding(BuildingTypes eBuilding, BuildingClassTypes ePrereqBuildingClass, int iExtra) const
{
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);

	int iPrereqs = kBuilding.getPrereqNumOfBuildingClass(ePrereqBuildingClass);
	// Civ4 Reimagined
	int iPercentPrereqs = kBuilding.getPercentPrereqNumOfBuildingClass(ePrereqBuildingClass) * getNumCities() / 100;
	
	// dont bother with the rest of the calcs if we have no prereqs
	if (iPrereqs < 1 && iPercentPrereqs < 1)
	{
		return 0;
	}
	
	// Civ4 Reimagined
	// If prereqBuilding is National Wonder you only need to have one.
	if (::isNationalWonderClass(ePrereqBuildingClass))
	{
		return 1;
	}

	BuildingClassTypes eBuildingClass = (BuildingClassTypes)kBuilding.getBuildingClassType();

	iPrereqs *= std::max(0, (GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getBuildingClassPrereqModifier() + 100));
	iPrereqs /= 100;

	if (!isLimitedWonderClass(eBuildingClass))
	{
		iPrereqs *= (getBuildingClassCount((BuildingClassTypes)(GC.getBuildingInfo(eBuilding).getBuildingClassType())) + iExtra + 1);
	}

	if (GC.getGameINLINE().isOption(GAMEOPTION_ONE_CITY_CHALLENGE) && isHuman())
	{
		iPrereqs = std::min(1, iPrereqs);
	}

	// Civ4 Reimagined
	return std::max(iPrereqs, iPercentPrereqs);
}


void CvPlayer::removeBuildingClass(BuildingClassTypes eBuildingClass)
{
	CvCity* pLoopCity;
	BuildingTypes eBuilding;
	int iLoop;

	eBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eBuildingClass)));

	if (eBuilding != NO_BUILDING)
	{
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			if (pLoopCity->getNumRealBuilding(eBuilding) > 0)
			{
/************************************************************************************************/
/* UNOFFICIAL_PATCH                       02/16/10                          EmperorFool         */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
/* original bts code
				pLoopCity->setNumRealBuilding(eBuilding, 0);
*/
				pLoopCity->setNumRealBuilding(eBuilding, pLoopCity->getNumRealBuilding(eBuilding) - 1);
/************************************************************************************************/
/* UNOFFICIAL_PATCH                        END                                                  */
/************************************************************************************************/
				break;
			}
		}
	}
}



// courtesy of the Gourd Bros...
void CvPlayer::processBuilding(BuildingTypes eBuilding, int iChange, CvArea* pArea)
{
	int iI, iJ;

	if (GC.getBuildingInfo(eBuilding).getFreeBuildingClass() != NO_BUILDINGCLASS)
	{
		BuildingTypes eFreeBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(GC.getBuildingInfo(eBuilding).getFreeBuildingClass());
		if (eFreeBuilding != NO_BUILDING)
		{
			changeFreeBuildingCount(eFreeBuilding, iChange);
		}
	}

	// Civ4 Reimagined
	if (GC.getBuildingInfo(eBuilding).getAreaFreeBuildingClass() != NO_BUILDINGCLASS)
	{
		BuildingTypes eFreeBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(GC.getBuildingInfo(eBuilding).getAreaFreeBuildingClass());
		if (eFreeBuilding != NO_BUILDING)
		{
			pArea->changeFreeBuildingCount(getID(), eFreeBuilding, iChange);
		}
	}

	if (GC.getBuildingInfo(eBuilding).getCivicOption() != NO_CIVICOPTION)
	{
		changeHasCivicOptionCount(((CivicOptionTypes)GC.getBuildingInfo(eBuilding).getCivicOption()), iChange);
	}
	
	// Civ4 Reimagined
	if (GC.getBuildingInfo(eBuilding).getNoAnarchyCivicOption() != NO_CIVICOPTION)
	{
		changeHasNoAnarchyCivicOptionCount(((CivicOptionTypes)GC.getBuildingInfo(eBuilding).getNoAnarchyCivicOption()), iChange);
	}

	changeGreatPeopleRateModifier(GC.getBuildingInfo(eBuilding).getGlobalGreatPeopleRateModifier() * iChange);
	changeGreatGeneralRateModifier(GC.getBuildingInfo(eBuilding).getGreatGeneralRateModifier() * iChange);
	changeDomesticGreatGeneralRateModifier(GC.getBuildingInfo(eBuilding).getDomesticGreatGeneralRateModifier() * iChange);
	changeAnarchyModifier(GC.getBuildingInfo(eBuilding).getAnarchyModifier() * iChange);
	changeGoldenAgeModifier(GC.getBuildingInfo(eBuilding).getGoldenAgeModifier() * iChange);
	changeHurryModifier(GC.getBuildingInfo(eBuilding).getGlobalHurryModifier() * iChange);
	changeFreeExperience(GC.getBuildingInfo(eBuilding).getGlobalFreeExperience() * iChange);
	changeStateReligionFreeExperience(GC.getBuildingInfo(eBuilding).getStateReligionFreeExperience() * iChange); // Civ4 Reimagined
	changeLootingModifier(GC.getBuildingInfo(eBuilding).getLootingModifier() * iChange); // Civ4 Reimagined
	changeBarbarianGreatGeneralCount((GC.getBuildingInfo(eBuilding).isBarbarianGreatGeneral()) ? iChange : 0); // Civ4 Reimagined
	changeUnlimitedBarbXPCount((GC.getBuildingInfo(eBuilding).isUnlimitedBarbXP()) ? iChange : 0); // Civ4 Reimagined
	changeNoWarAgainstSameFaithUnhappinessCount((GC.getBuildingInfo(eBuilding).isNoWarAgainstSameFaithUnhappiness()) ? iChange : 0); // Civ4 Reimagined
	changeWarWearinessModifier(GC.getBuildingInfo(eBuilding).getGlobalWarWearinessModifier() * iChange);
	pArea->changeFreeSpecialist(getID(), (GC.getBuildingInfo(eBuilding).getAreaFreeSpecialist() * iChange));
	changeFreeSpecialist(GC.getBuildingInfo(eBuilding).getGlobalFreeSpecialist() * iChange);
	changeCoastalTradeRoutes(GC.getBuildingInfo(eBuilding).getCoastalTradeRoutes() * iChange);
	changeTradeRoutes(GC.getBuildingInfo(eBuilding).getGlobalTradeRoutes() * iChange);
	pArea->changeDistanceMaintenanceModifier(getID(), GC.getBuildingInfo(eBuilding).getAreaDistanceMaintenanceModifier() * iChange); // Civ4 Reimagined
	pArea->changeCorporationMaintenanceModifier(getID(), GC.getBuildingInfo(eBuilding).getAreaCorporationMaintenanceModifier() * iChange); // Civ4 Reimagined
	if (GC.getBuildingInfo(eBuilding).getAreaHealth() > 0)
	{
		pArea->changeBuildingGoodHealth(getID(), (GC.getBuildingInfo(eBuilding).getAreaHealth() * iChange));
	}
	else
	{
		pArea->changeBuildingBadHealth(getID(), (GC.getBuildingInfo(eBuilding).getAreaHealth() * iChange));
	}
	if (GC.getBuildingInfo(eBuilding).getGlobalHealth() > 0)
	{
		changeBuildingGoodHealth(GC.getBuildingInfo(eBuilding).getGlobalHealth() * iChange);
	}
	else
	{
		changeBuildingBadHealth(GC.getBuildingInfo(eBuilding).getGlobalHealth() * iChange);
	}
	pArea->changeBuildingHappiness(getID(), (GC.getBuildingInfo(eBuilding).getAreaHappiness() * iChange));
	changeBuildingHappiness(GC.getBuildingInfo(eBuilding).getGlobalHappiness() * iChange);
	changeWorkerSpeedModifier(GC.getBuildingInfo(eBuilding).getWorkerSpeedModifier() * iChange);
	changeSpaceProductionModifier(GC.getBuildingInfo(eBuilding).getGlobalSpaceProductionModifier() * iChange);
	changeCityDefenseModifier(GC.getBuildingInfo(eBuilding).getAllCityDefenseModifier() * iChange);
	pArea->changeCleanPowerCount(getTeam(), ((GC.getBuildingInfo(eBuilding).isAreaCleanPower()) ? iChange : 0));
	pArea->changeBorderObstacleCount(getTeam(), ((GC.getBuildingInfo(eBuilding).isAreaBorderObstacle()) ? iChange : 0));
	changeNoNonStateReligionSpreadCount((GC.getBuildingInfo(eBuilding).isNoNonStateReligionSpread()) ? iChange : 0); // Civ4 Reimagined

	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		changeSeaPlotYield(((YieldTypes)iI), (GC.getBuildingInfo(eBuilding).getGlobalSeaPlotYieldChange(iI) * iChange));
		changeLakePlotYield(((YieldTypes)iI), (GC.getBuildingInfo(eBuilding).getGlobalLakePlotYieldChange(iI) * iChange)); // Civ4 Reimagined
		pArea->changeYieldRateModifier(getID(), ((YieldTypes)iI), (GC.getBuildingInfo(eBuilding).getAreaYieldModifier(iI) * iChange));
		pArea->changeTradeYieldModifier(getID(), ((YieldTypes)iI), (GC.getBuildingInfo(eBuilding).getAreaTradeYieldModifier(iI) * iChange)); // Civ4 Reimagined
		changeYieldRateModifier(((YieldTypes)iI), (GC.getBuildingInfo(eBuilding).getGlobalYieldModifier(iI) * iChange));
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		changeCommerceRateModifier(((CommerceTypes)iI), (GC.getBuildingInfo(eBuilding).getGlobalCommerceModifier(iI) * iChange));
		changeSpecialistExtraCommerce(((CommerceTypes)iI), (GC.getBuildingInfo(eBuilding).getSpecialistExtraCommerce(iI) * iChange));
		changeStateReligionBuildingCommerce(((CommerceTypes)iI), (GC.getBuildingInfo(eBuilding).getStateReligionCommerce(iI) * iChange));
		changeCommerceFlexibleCount(((CommerceTypes)iI), (GC.getBuildingInfo(eBuilding).isCommerceFlexible(iI)) ? iChange : 0);
	}

	for (iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingTypes eOurBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI);
		if (NO_BUILDING != eOurBuilding)
		{
			changeExtraBuildingHappiness(eOurBuilding, (GC.getBuildingInfo(eBuilding).getBuildingHappinessChanges(iI) * iChange));
		}
	}

	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeSpecialistExtraYield(((SpecialistTypes)iI), ((YieldTypes)iJ), (GC.getBuildingInfo(eBuilding).getSpecialistYieldChange(iI, iJ) * iChange));
		}
	}

	// Civ4 Reimagined
	for (iI = 0; iI < GC.getNumIdeologyInfos(); iI++)
	{
		changeForeignTradeIdeologyModifier((IdeologyTypes)iI, GC.getBuildingInfo(eBuilding).getForeignTradeIdeologyModifier(iI) * iChange);

		if (GC.getBuildingInfo(eBuilding).getIdeologyCombatExperience(iI) > 0)
		{
			changeIdeologyCombatExperienceModifier((IdeologyTypes)iI, GC.getBuildingInfo(eBuilding).getIdeologyCombatExperience(iI) * iChange);

			if (getIdeologyCombatExperienceModifier((IdeologyTypes)iI) > 0)
			{
				GC.getGameINLINE().setIdeologyCombatExperienceOwner((IdeologyTypes)iI, getID());
			}
			else
			{
				GC.getGameINLINE().setIdeologyCombatExperienceOwner((IdeologyTypes)iI, NO_PLAYER);
			}
		}
	}
}


bool CvPlayer::canBuild(const CvPlot* pPlot, BuildTypes eBuild, bool bTestEra, bool bTestVisible) const
{
	PROFILE_FUNC();

	if (!(pPlot->canBuild(eBuild, getID(), bTestVisible)))
	{
		return false;
	}

	if (GC.getBuildInfo(eBuild).getTechPrereq() != NO_TECH)
	{
		if (!(GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getBuildInfo(eBuild).getTechPrereq())))
		{
			if ((!bTestEra && !bTestVisible) || ((getCurrentEra() + 1) < GC.getTechInfo((TechTypes) GC.getBuildInfo(eBuild).getTechPrereq()).getEra()))
			{
				return false;
			}
		}
	}
	
	// Civ4 Reimagined
	if (GC.getBuildInfo(eBuild).getTechObsolete() != NO_TECH)
	{
		if ((GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getBuildInfo(eBuild).getTechObsolete())))
		{
			return false;
		}
	}
		
	if (!bTestVisible)
	{
		if (pPlot->getFeatureType() != NO_FEATURE)
		{
			// Civ4 Reimagined: Can remove Features without tech when there is a visible Bonus
			if (!isCanRemoveFeatures() && !(GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getBuildInfo(eBuild).getFeatureTech(pPlot->getFeatureType()))))
			{
				BonusTypes eBonus = pPlot->getBonusType();
				if (eBonus == NO_BONUS || (GC.getBonusInfo(eBonus).getTechReveal() > -1 && !(GET_TEAM(getTeam()).isBonusRevealed(eBonus))))
				{
					return false;
				}
			}
		}

		if (std::max(0, getGold()) < getBuildCost(pPlot, eBuild))
		{
			return false;
		}
	}

	return true;
}

// Returns the cost
int CvPlayer::getBuildCost(const CvPlot* pPlot, BuildTypes eBuild) const
{
	FAssert(eBuild >= 0 && eBuild < GC.getNumBuildInfos());

	if (pPlot->getBuildProgress(eBuild) > 0)
	{
		return 0;
	}

	return std::max(0, GC.getBuildInfo(eBuild).getCost() * (100 + getInflationRate())) / 100;
}




RouteTypes CvPlayer::getBestRoute(const CvPlot* pPlot) const
{
	PROFILE_FUNC();

	int ipBestValue = 0;
	RouteTypes eBestRoute = NO_ROUTE;

	// BBAI TODO: Efficiency: Could cache this, decent savings on large maps
	// Perhaps save best route type per player each turn, then just check that
	// one first and only check others if can't do best.

	// K-Mod: I've reversed the order of iteration because the best builds are usually at the end.
	for (int iI = GC.getNumBuildInfos()-1; iI >= 0 ; iI--)
	{
		RouteTypes eRoute = ((RouteTypes)(GC.getBuildInfo((BuildTypes)iI).getRoute()));

		if (eRoute != NO_ROUTE)
		{
			// K-Mod: I've swapped the order of the if statments, because the value check is much faster. (faster trumps convention)
			int iValue = GC.getRouteInfo(eRoute).getValue();
			if (iValue > ipBestValue)
			{
				if (pPlot != NULL ? (pPlot->getRouteType() == eRoute || canBuild(pPlot, (BuildTypes)iI)) : GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getBuildInfo((BuildTypes)iI).getTechPrereq())))
				{
					ipBestValue = iValue;
					eBestRoute = eRoute;
				}
			}
		}
	}

	return eBestRoute;
}


int CvPlayer::getImprovementUpgradeRate() const
{
	// Civ4 Reimagind: Before improvementUpgradeRateModifier was divided by 100 which resulted in rounding down all modifiers less than 100.
	int iRate = 100 + getImprovementUpgradeRateModifier();

	return std::max(0, iRate);
}


int CvPlayer::calculateTotalYield(YieldTypes eYield) const
{
	PROFILE_FUNC();
	CvCity* pLoopCity;
	int iTotalYield = 0;
	int iLoop = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalYield += pLoopCity->getYieldRate(eYield);
	}

	return iTotalYield;
}


int CvPlayer::calculateTotalCityHappiness() const
{
	CvCity* pLoopCity;
	int iTotalHappiness = 0;
	int iLoop = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalHappiness += pLoopCity->happyLevel();
	}

	return iTotalHappiness;
}


int CvPlayer::calculateTotalExports(YieldTypes eYield) const
{
	CvCity* pLoopCity;
	CvCity* pTradeCity;
	int iTotalExports = 0;
	int iLoop = 0, iTradeLoop = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for (iTradeLoop = 0; iTradeLoop < pLoopCity->getTradeRoutes(); iTradeLoop++)
		{
			pTradeCity = pLoopCity->getTradeCity(iTradeLoop);
			if (pTradeCity != NULL)
			{
				if (pTradeCity->getOwnerINLINE() != getID())
				{
					iTotalExports += pLoopCity->calculateTradeYield(eYield, pLoopCity->calculateTradeProfitTimes100(pTradeCity));
				}
			}
		}
	}

	return iTotalExports / 100;
}


int CvPlayer::calculateTotalImports(YieldTypes eYield) const
{
	CvCity* pLoopCity;
	CvCity* pTradeCity;
	int iTotalImports = 0;
	int iPlayerLoop = 0, iLoop = 0, iTradeLoop = 0;
	
	// Loop through players
	for (iPlayerLoop = 0; iPlayerLoop < MAX_CIV_PLAYERS; iPlayerLoop++)
	{
		if (iPlayerLoop != getID())
		{
			for (pLoopCity = GET_PLAYER((PlayerTypes) iPlayerLoop).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes) iPlayerLoop).nextCity(&iLoop))
			{
				for (iTradeLoop = 0; iTradeLoop < pLoopCity->getTradeRoutes(); iTradeLoop++)
				{
					pTradeCity = pLoopCity->getTradeCity(iTradeLoop);
					if (pTradeCity != NULL)
					{
						if (pTradeCity->getOwnerINLINE() == getID())
						{
							iTotalImports += pLoopCity->calculateTradeYield(eYield, pLoopCity->calculateTradeProfitTimes100(pTradeCity));
						}
					}
				}
			}
		}
	}
	return iTotalImports / 100;
}


int CvPlayer::calculateTotalCityUnhappiness() const
{
	CvCity* pLoopCity;
	int iTotalUnhappiness = 0;
	int iLoop = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalUnhappiness += pLoopCity->unhappyLevel();
	}

	return iTotalUnhappiness;
}


int CvPlayer::calculateTotalCityHealthiness() const
{
	CvCity* pLoopCity;
	int iTotalHealthiness = 0;
	int iLoop = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalHealthiness += pLoopCity->goodHealth();
	}

	return iTotalHealthiness;
}

int CvPlayer::calculateTotalCityUnhealthiness() const
{
	CvCity* pLoopCity;
	int iTotalUnhealthiness = 0;
	int iLoop = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalUnhealthiness += pLoopCity->badHealth();
	}

	return iTotalUnhealthiness;
}

/*
** K-Mod
** calculate the pollution output of a civ.
** iTypes is a bit-field whos members are POLLUTION_POPULTION, _BUILDINGS, _BONUS, _POWER. (and _ALL)
*/
int CvPlayer::calculatePollution(int iTypes) const
{
	int iTotal = 0;

	int iBuildingWeight = ((iTypes & POLLUTION_BUILDINGS) == 0)?0 :GC.getDefineINT("GLOBAL_WARMING_BUILDING_WEIGHT");
	int iBonusWeight = ((iTypes & POLLUTION_BONUSES) == 0)?0 :GC.getDefineINT("GLOBAL_WARMING_BONUS_WEIGHT");
	int iPowerWeight = ((iTypes & POLLUTION_POWER) == 0)?0 :GC.getDefineINT("GLOBAL_WARMING_POWER_WEIGHT");
	int iPopWeight = ((iTypes & POLLUTION_POPULATION) == 0)?0 :GC.getDefineINT("GLOBAL_WARMING_POPULATION_WEIGHT");

	int iLoop;
	for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		// note: "bad health" values are negative, except for population! (crazy, but true. Who writes this junk?)
		iTotal -=
			(pCity->totalBadBuildingHealth() * iBuildingWeight)
			+ (pCity->getBonusBadHealth() * iBonusWeight)
			+ (pCity->getPowerBadHealth() * iPowerWeight)
			- (pCity->unhealthyPopulation() * iPopWeight);
	}

	return iTotal;
}

int CvPlayer::getGwPercentAnger() const
{
	return m_iGwPercentAnger;
}

void CvPlayer::setGwPercentAnger(int iNewValue)
{
	if (iNewValue != m_iGwPercentAnger)
	{
		m_iGwPercentAnger = iNewValue;
		AI_makeAssignWorkDirty();
	}
}

/*
** K-Mod end
*/

// K-Mod
int CvPlayer::getUnitCostMultiplier() const
{
	int iMultiplier = 100;
	iMultiplier *= GC.getHandicapInfo(getHandicapType()).getUnitCostPercent();
	iMultiplier /= 100;

	if (!isHuman() && !isBarbarian())
	{
		iMultiplier *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIUnitCostPercent();
		iMultiplier /= 100;

		iMultiplier *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iMultiplier /= 100;
	}

	return iMultiplier;
}
// K-Mod end

int CvPlayer::calculateUnitCost(int& iFreeUnits, int& iFreeMilitaryUnits, int& iPaidUnits, int& iPaidMilitaryUnits, int& iUnitCost, int& iMilitaryCost, int& iExtraCost) const
{
	//int iSupport;

	iFreeUnits = GC.getHandicapInfo(getHandicapType()).getFreeUnits();

	iFreeUnits += getBaseFreeUnits();
	iFreeUnits += ((getTotalPopulation() * getFreeUnitsPopulationPercent()) / 100);

	iFreeMilitaryUnits = getBaseFreeMilitaryUnits();
	iFreeMilitaryUnits += ((getTotalPopulation() * getFreeMilitaryUnitsPopulationPercent()) / 100);

	/* original BTS code
	if (!isHuman())
	{
		if (GET_TEAM(getTeam()).hasMetHuman())
		{
			iFreeUnits += getNumCities(); // XXX
			iFreeMilitaryUnits += getNumCities(); // XXX
		}
	} */ // Hidden AI bonus removed by BBAI.

	iPaidUnits = std::max(0, getNumUnits() - iFreeUnits);
	iPaidMilitaryUnits = std::max(0, getNumMilitaryUnits() - iFreeMilitaryUnits);

	//iSupport = 0;

	/* original bts code
	iBaseUnitCost = iPaidUnits * getGoldPerUnit();
	iMilitaryCost = iPaidMilitaryUnits * getGoldPerMilitaryUnit();
	iExtraCost = getExtraUnitCost();

	iSupport = iMilitaryCost + iBaseUnitCost + iExtraCost;

	iSupport *= GC.getHandicapInfo(getHandicapType()).getUnitCostPercent();
	iSupport /= 100;

	if (!isHuman() && !isBarbarian())
	{
		iSupport *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIUnitCostPercent();
		iSupport /= 100;

		iSupport *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iSupport /= 100;
	}*/

	// K-Mod. GoldPerUnit, etc, are now done as percentages.
	// Also, "UnitCostPercent" handicap modifiers now apply directly to unit cost only, not military or extra cost.
	// (iBaseUnitCost is no longer fed back to the caller. Only the modified cost is.)
	iUnitCost = iPaidUnits * getGoldPerUnit() * getUnitCostMultiplier() / 10000;
	//Careful! GetGoldPerMilitaryUnit() is 0 per default!
	iMilitaryCost = iPaidMilitaryUnits * getGoldPerMilitaryUnit() / 100;
	// Civ4 Reimagined: /100 removed because of Mercenaries
	iExtraCost = getExtraUnitCost();

	int iSupport = iUnitCost + iMilitaryCost + iExtraCost;
	// K-Mod end

	FAssert(iSupport >= 0);

	return std::max(0, iSupport);
}


int CvPlayer::calculateUnitCost() const
{
	if (isAnarchy())
	{
		return 0;
	}

	int iFreeUnits;
	int iFreeMilitaryUnits;
	int iPaidUnits;
	int iPaidMilitaryUnits;
	int iMilitaryCost;
	int iBaseUnitCost;
	int iExtraCost;

	return calculateUnitCost(iFreeUnits, iFreeMilitaryUnits, iPaidUnits, iPaidMilitaryUnits, iBaseUnitCost, iMilitaryCost, iExtraCost);
}

int CvPlayer::calculateUnitSupply() const
{
	int iPaidUnits;
	int iBaseSupplyCost;

	// Civ4 Reimagined
	if (isAnarchy() || isNoUnitSupply())
	{
		return 0;
	}

	return calculateUnitSupply(iPaidUnits, iBaseSupplyCost);
}

int CvPlayer::calculateUnitSupply(int& iPaidUnits, int& iBaseSupplyCost, bool useForeignUnitSupplyModifier) const
{
	int iSupply;

	iPaidUnits = std::max(0, (getNumOutsideUnits() - GC.getDefineINT("INITIAL_FREE_OUTSIDE_UNITS")));
	
	iBaseSupplyCost = iPaidUnits * GC.getDefineINT("INITIAL_OUTSIDE_UNIT_GOLD_PERCENT");
	iBaseSupplyCost /= 100;

	if (useForeignUnitSupplyModifier)
	{
		//Civ4 Reimagined
		iBaseSupplyCost *= std::max(0,getForeignUnitSupply());
		iBaseSupplyCost /= 100;
	}
	
	iSupply = iBaseSupplyCost;

	if (!isHuman() && !isBarbarian())
	{
		iSupply *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIUnitSupplyPercent();
		iSupply /= 100;

		iSupply *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iSupply /= 100;
	}

	FAssert(iSupply >= 0);

	return iSupply;
}


int CvPlayer::calculatePreInflatedCosts() const
{
	CyArgsList argsList;
	argsList.add(getID());
	long lResult;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "getExtraCost", argsList.makeFunctionArgs(), &lResult);

	return (calculateUnitCost() + calculateUnitSupply() + getTotalMaintenance() + getCivicUpkeep() + (int)lResult);
}


//int CvPlayer::calculateInflationRate() const
void CvPlayer::updateInflationRate()
{
	int iTurns = ((GC.getGameINLINE().getGameTurn() + GC.getGameINLINE().getElapsedGameTurns()) / 2);

	if (GC.getGameINLINE().getMaxTurns() > 0)
	{
		iTurns = std::min(GC.getGameINLINE().getMaxTurns(), iTurns);
	}

	// K-Mod. Advance the effective turn proportionally with total techs discovered;
	// so that games which lots of tech trading will still have a reasonable inflation rate.
	// Note. This is an experimental change. I might have to revise it in the future.
	{
		PROFILE("tech inflation");
		long iPotentialTech = 0;
		long iCurrentTech = 0;
		for (TechTypes eTech = (TechTypes)0; eTech < GC.getNumTechInfos(); eTech=(TechTypes)(eTech+1))
		{
			for (PlayerTypes j = (PlayerTypes)0; j < MAX_CIV_PLAYERS; j=(PlayerTypes)(j+1))
			{
				const CvPlayer& kLoopPlayer = GET_PLAYER(j);
				if (kLoopPlayer.isAlive() &&
					!kLoopPlayer.isMinorCiv() &&
					kLoopPlayer.canEverResearch(eTech))
				{
					int iWeight = kLoopPlayer.getTotalPopulation() * (kLoopPlayer.getTeam() == getTeam() ? 2 : 1);
					iPotentialTech += iWeight; 
					if (GET_TEAM(kLoopPlayer.getTeam()).isHasTech(eTech))
						iCurrentTech += iWeight;
				}
			}
		}
		iTurns += iCurrentTech*GC.getGameINLINE().getEstimateEndTurn()*4/(3*std::max(1L, iPotentialTech)); // based on full tech at 3/4 of max time.
		iTurns /= 2;
	}
	// K-Mod end

	iTurns += GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getInflationOffset();

	if (iTurns <= 0)
	{
		//return 0;
		m_iInflationRate = 0;
		return;
	}
	/****************************/

	int iInflationPerTurnTimes10000 = GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getInflationPercent();
	iInflationPerTurnTimes10000 *= GC.getHandicapInfo(getHandicapType()).getInflationPercent();
	iInflationPerTurnTimes10000 /= 100;

	int iModifier = m_iInflationModifier;
	if (!isHuman() && !isBarbarian())
	{
		int iAIModifier = GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIInflationPercent();
		iAIModifier *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iAIModifier /= 100;

		iModifier += iAIModifier - 100;
	}

	iInflationPerTurnTimes10000 *= std::max(0, 100 + iModifier);
	iInflationPerTurnTimes10000 /= 100;

	// Keep up to second order terms in binomial series
	int iRatePercent = (iTurns * iInflationPerTurnTimes10000) / 100;
	iRatePercent += (iTurns * (iTurns - 1) * iInflationPerTurnTimes10000 * iInflationPerTurnTimes10000) / 2000000;

	FAssert(iRatePercent >= 0);

	//return iRatePercent;
	m_iInflationRate = iRatePercent;
}


int CvPlayer::calculateInflatedCosts() const
{
	PROFILE_FUNC();

	int iCosts;

	iCosts = calculatePreInflatedCosts();

	iCosts *= std::max(0, (getInflationRate() + 100));
	iCosts /= 100;

	return iCosts;
}


/* int CvPlayer::calculateBaseNetGold() const
{
	int iNetGold;

	iNetGold = (getCommerceRate(COMMERCE_GOLD) + getGoldPerTurn());

	iNetGold -= calculateInflatedCosts();
	
	return iNetGold;
} */

int CvPlayer::calculateResearchModifier(TechTypes eTech) const
{
	int iModifier = 100;

	if (NO_TECH == eTech)
	{
		return iModifier;
	}

	int iKnownCount = 0;
	int iPossibleKnownCount = 0;
	bool bDiscovered = false;

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      07/27/09                                jdog5000      */
/*                                                                                              */
/* Tech Diffusion                                                                               */
/************************************************************************************************/
	// Civ4 Reimagined: Only enable Tech Diffusion when there is no tech trading
	if( GC.getTECH_DIFFUSION_ENABLE() && GC.getGameINLINE().isOption(GAMEOPTION_NO_TECH_TRADING) )
	{
		double knownExp = 0.0;
		// Tech flows better through open borders
		for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GET_TEAM((TeamTypes)iI).isHasTech(eTech))
				{
					bDiscovered = true;
					if (GET_TEAM(getTeam()).isHasMet((TeamTypes)iI))
					{
						knownExp += 0.5;
						
						if (GET_TEAM((TeamTypes)iI).isVassal(getTeam())) {
							// Civ4 Reimagined
							knownExp += 4;
						}
						else if (GET_TEAM(getTeam()).isVassal((TeamTypes)iI))
						{
							knownExp += 1;
						}
						if (GET_TEAM(getTeam()).isOpenBorders((TeamTypes)iI))
						{
							knownExp += 1;
						}
					}
				}
			}
		}

		int techDiffMod = GC.getTECH_DIFFUSION_KNOWN_TEAM_MODIFIER();
		if (knownExp > 0.0)
		{
			iModifier += techDiffMod - (int)(techDiffMod * pow(0.85, knownExp) + 0.5);
			FAssert(iModifier >= 100);
		}
		
	
		// Civ4 Reimagined
		for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); ++iReligion)
		{
			if ((TechTypes)GC.getReligionInfo((ReligionTypes)iReligion).getTechPrereq() == eTech)
			{
				iModifier += getReligionTechModifier();
				
				// Cheaper religion techs when religion is discovered
				if (bDiscovered)
				{
					iModifier += GC.getDefineINT("TECH_COST_DISCOVERED_RELIGION_MODIFIER");
					break;
				}
			}
		}
	

		// Tech flows downhill to those who are far behind
		int iTechScorePercent = GET_TEAM(getTeam()).getBestKnownTechScorePercent();
		int iWelfareThreshold = GC.getTECH_DIFFUSION_WELFARE_THRESHOLD();
		if( iTechScorePercent < iWelfareThreshold )
		{
			if( knownExp > 0.0 )
			{
				iModifier += (GC.getTECH_DIFFUSION_WELFARE_MODIFIER() * GC.getGameINLINE().getCurrentEra() * (iWelfareThreshold - iTechScorePercent))/200;
				FAssert(iModifier >= 100);
			}
		}
	}
	else
	{
		// Default BTS code
		for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GET_TEAM(getTeam()).isHasMet((TeamTypes)iI))
				{
					if (GET_TEAM((TeamTypes)iI).isHasTech(eTech))
					{
						iKnownCount++;
					}
				}

				iPossibleKnownCount++;
			}
		}

		if (iPossibleKnownCount > 0)
		{
			iModifier += (GC.getDefineINT("TECH_COST_TOTAL_KNOWN_TEAM_MODIFIER") * iKnownCount) / iPossibleKnownCount;
		}
	}
	
	int iPossiblePaths = 0;
	int iUnknownPaths = 0;

	for (int iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
	{
		if (GC.getTechInfo(eTech).getPrereqOrTechs(iI) != NO_TECH)
		{
			if (!(GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getTechInfo(eTech).getPrereqOrTechs(iI)))))
			{
				iUnknownPaths++;
			}

			iPossiblePaths++;
		}
	}

	FAssertMsg(iPossiblePaths >= iUnknownPaths, "The number of possible paths is expected to match or exceed the number of unknown ones");

	if( iPossiblePaths > iUnknownPaths )
	{
		iModifier += GC.getTECH_COST_FIRST_KNOWN_PREREQ_MODIFIER();
		iPossiblePaths--;
		iModifier += (iPossiblePaths - iUnknownPaths) * GC.getTECH_COST_KNOWN_PREREQ_MODIFIER();
	}

	iModifier -= GC.getEraInfo((EraTypes)GC.getTechInfo(eTech).getEra()).getTechCostModifier();

	iModifier -= GC.getTECH_COST_MODIFIER();
	
	
	// Civ4 Reimagined: Unique power
	if (getCatchUpTechModifier() > 0)
	{
		// Tech flows better through open borders
		for (int iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			if (GET_TEAM((TeamTypes)iI).isAlive())
			{
				if (GET_TEAM((TeamTypes)iI).isHasTech(eTech))
				{
					if (GET_TEAM(getTeam()).isHasMet((TeamTypes)iI))
					{
						iModifier *= 100 + getCatchUpTechModifier();
						iModifier /= 100;
						break;
					}
				}
			}
		}
	}	

	FAssert(iModifier > 0);

	return iModifier;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
}

/* int CvPlayer::calculateBaseNetResearch(TechTypes eTech) const
{
	TechTypes eResearchTech;

	if (eTech != NO_TECH)
	{
		eResearchTech = eTech;
	}
	else
	{
		eResearchTech = getCurrentResearch();
	}

	return (((GC.getDefineINT("BASE_RESEARCH_RATE") + getCommerceRate(COMMERCE_RESEARCH)) * calculateResearchModifier(eResearchTech)) / 100);
} */


int CvPlayer::calculateGoldRate() const
{
	/* original bts code
	int iRate = 0;

	if (isCommerceFlexible(COMMERCE_RESEARCH))
	{
		iRate = calculateBaseNetGold();
	}
	else
	{
		iRate = std::min(0, (calculateBaseNetResearch() + calculateBaseNetGold()));
	}

	return iRate; */
	// K-Mod. (Just moved from calculateBaseNetGold.)
	int iNetGold;

	iNetGold = (getCommerceRate(COMMERCE_GOLD) + getGoldPerTurn());

	iNetGold -= calculateInflatedCosts();

	return iNetGold;
	// K-Mod end
}


int CvPlayer::calculateResearchRate(TechTypes eTech) const
{
	/* original bts code
	int iRate = 0;

	if (isCommerceFlexible(COMMERCE_RESEARCH))
	{
		iRate = calculateBaseNetResearch(eTech);
	}
	else
	{
		iRate = std::max(1, (calculateBaseNetResearch(eTech) + calculateBaseNetGold()));
	}

	return iRate; */
	// K-Mod. (Just moved from calculateBaseNetResearch.)
	// Note: the original code had a floor of 1. This version does not.
	TechTypes eResearchTech;

	if (eTech != NO_TECH)
	{
		eResearchTech = eTech;
	}
	else
	{
		eResearchTech = getCurrentResearch();
	}

	return (((GC.getDefineINT("BASE_RESEARCH_RATE") + getCommerceRate(COMMERCE_RESEARCH)) * calculateResearchModifier(eResearchTech)) / 100);
	// K-Mod end
}

int CvPlayer::calculateTotalCommerce() const
{
	//int iTotalCommerce = calculateBaseNetGold() + calculateBaseNetResearch();
	int iTotalCommerce = calculateGoldRate() + calculateResearchRate(); // K-Mod

	for (int i = 0; i < NUM_COMMERCE_TYPES; ++i)
	{
		if (COMMERCE_GOLD != i && COMMERCE_RESEARCH != i)
		{
			iTotalCommerce += getCommerceRate((CommerceTypes)i);
		}
	}

	return iTotalCommerce;
}

bool CvPlayer::isResearch() const
{
	if(GC.getUSE_IS_PLAYER_RESEARCH_CALLBACK())
	{
		CyArgsList argsList;
		long lResult;
		argsList.add(getID());
		lResult = 1;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "isPlayerResearch", argsList.makeFunctionArgs(), &lResult);
		if (lResult == 0)
		{
			return false;
		}
	}

	if (!isFoundedFirstCity())
	{
		return false;
	}

	return true;
}


bool CvPlayer::canEverResearch(TechTypes eTech) const
{
	if (GC.getTechInfo(eTech).isDisable())
	{
		return false;
	}

	if (GC.getCivilizationInfo(getCivilizationType()).isCivilizationDisableTechs(eTech))
	{
		return false;
	}

	if(GC.getUSE_CANNOT_RESEARCH_CALLBACK())
	{
		CyArgsList argsList;
		argsList.add(getID());
		argsList.add(eTech);
		argsList.add(false);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "cannotResearch", argsList.makeFunctionArgs(), &lResult);
		if (lResult == 1)
		{
			return false;
		}
	}

	return true;
}


bool CvPlayer::canResearch(TechTypes eTech, bool bTrade, bool bFree) const
{
	bool bFoundPossible;
	bool bFoundValid;
	int iI;

	if(GC.getUSE_CAN_RESEARCH_CALLBACK())
	{
		CyArgsList argsList;
		argsList.add(getID());
		argsList.add(eTech);
		argsList.add(bTrade);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "canResearch", argsList.makeFunctionArgs(), &lResult);
		if (lResult == 1)
		{
			return true;
		}
	}

	if (!isResearch() && !bFree && getAdvancedStartPoints() < 0)
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isHasTech(eTech))
	{
		return false;
	}

	bFoundPossible = false;
	bFoundValid = false;

	for (iI = 0; iI < GC.getNUM_OR_TECH_PREREQS(); iI++)
	{
		TechTypes ePrereq = (TechTypes)GC.getTechInfo(eTech).getPrereqOrTechs(iI);
		if (ePrereq != NO_TECH)
		{
			bFoundPossible = true;

			if (GET_TEAM(getTeam()).isHasTech(ePrereq))
			{
				if (!bTrade || GC.getGameINLINE().isOption(GAMEOPTION_NO_TECH_BROKERING) || !GET_TEAM(getTeam()).isNoTradeTech(ePrereq))
				{
					bFoundValid = true;
					break;
				}
			}
		}
	}

	if (bFoundPossible && !bFoundValid)
	{
		return false;
	}

	for (iI = 0; iI < GC.getNUM_AND_TECH_PREREQS(); iI++)
	{
		TechTypes ePrereq = (TechTypes)GC.getTechInfo(eTech).getPrereqAndTechs(iI);
		if (ePrereq != NO_TECH)
		{
			if (!GET_TEAM(getTeam()).isHasTech(ePrereq))
			{
				return false;
			}
			
			if (bTrade && !GC.getGameINLINE().isOption(GAMEOPTION_NO_TECH_BROKERING) && GET_TEAM(getTeam()).isNoTradeTech(ePrereq))
			{
				return false;
			}
		}
	}

	if (!canEverResearch(eTech))
	{
		return false;
	}

	return true;
}


TechTypes CvPlayer::getCurrentResearch() const
{
	CLLNode<TechTypes>* pResearchNode;

	pResearchNode = headResearchQueueNode();

	if (pResearchNode != NULL)
	{
		return pResearchNode->m_data;
	}
	else
	{
		return NO_TECH;
	}
}


bool CvPlayer::isCurrentResearchRepeat() const
{
	TechTypes eCurrentResearch;

	eCurrentResearch = getCurrentResearch();

	if (eCurrentResearch == NO_TECH)
	{
		return false;
	}

	return GC.getTechInfo(eCurrentResearch).isRepeat();
}


bool CvPlayer::isNoResearchAvailable() const
{
	int iI;

	if (getCurrentResearch() != NO_TECH)
	{
		return false;
	}

	for (iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		if (canResearch((TechTypes)iI))
		{
			return false;
		}
	}

	return true;
}


int CvPlayer::getResearchTurnsLeft(TechTypes eTech, bool bOverflow) const
{
	int iTurnsLeft = getResearchTurnsLeftTimes100(eTech, bOverflow);

	iTurnsLeft = (iTurnsLeft + 99) / 100; // round up

	return std::max(1, iTurnsLeft);
}

int CvPlayer::getResearchTurnsLeftTimes100(TechTypes eTech, bool bOverflow) const
{
    int iResearchRate;
	int iOverflow;
	int iResearchLeft;
	int iTurnsLeft;
	int iI;

	iResearchRate = 0;
	iOverflow = 0;

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
			{
				if ((iI == getID()) || (GET_PLAYER((PlayerTypes)iI).getCurrentResearch() == eTech))
				{
					//iResearchRate += GET_PLAYER((PlayerTypes)iI).calculateResearchRate(eTech);
					iResearchRate += std::max(1, GET_PLAYER((PlayerTypes)iI).calculateResearchRate(eTech)); // K-Mod (replacing the minimum which use to be in calculateResearchRate)
					iOverflow += (GET_PLAYER((PlayerTypes)iI).getOverflowResearch() * calculateResearchModifier(eTech)) / 100;
				}
			}
		}
	}

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/18/10                                jdog5000      */
/*                                                                                              */
/* Tech AI                                                                                      */
/************************************************************************************************/
	// Mainly just so debug display shows sensible value
	iResearchLeft = GET_TEAM(getTeam()).getResearchLeft(eTech);
	
	if (bOverflow)
	{
		iResearchLeft -= iOverflow;
	}
	
	iResearchLeft *= 100;

	if (iResearchRate == 0)
	{
		return iResearchLeft;
	}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

	iTurnsLeft = (iResearchLeft / iResearchRate);

	if (iTurnsLeft * iResearchRate < iResearchLeft)
	{
		++iTurnsLeft;
	}

	return std::max(1, iTurnsLeft);
    
    
}

// K-Mod. Return true if this player can see what ePlayer is researching
bool CvPlayer::canSeeResearch(PlayerTypes ePlayer) const
{
	FAssert(ePlayer >= 0 && ePlayer < MAX_PLAYERS);
	const CvPlayer& kOther = GET_PLAYER(ePlayer);

	if (kOther.getTeam() == getTeam() || GET_TEAM(kOther.getTeam()).isVassal(getTeam()))
		return true;

	if (!GET_TEAM(getTeam()).isHasMet(kOther.getTeam()))
		return false;

	if (!GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		for (EspionageMissionTypes i = (EspionageMissionTypes)0; i < GC.getNumEspionageMissionInfos(); i = (EspionageMissionTypes)(i+1))
		{
			CvEspionageMissionInfo& kMissionInfo = GC.getEspionageMissionInfo(i);

			if (kMissionInfo.isSeeResearch() && kMissionInfo.isPassive() && canDoEspionageMission(i, ePlayer, 0, 0, 0))
				return true;
		}
	}
	return false;
}

// return true if this player can see ePlayer's demographics (power graph, culture graph, etc.)
bool CvPlayer::canSeeDemographics(PlayerTypes ePlayer) const
{
	FAssert(ePlayer >= 0 && ePlayer < MAX_PLAYERS);
	const CvPlayer& kOther = GET_PLAYER(ePlayer);

	if (kOther.getTeam() == getTeam() || GET_TEAM(kOther.getTeam()).isVassal(getTeam()))
		return true;

	if (!GET_TEAM(getTeam()).isHasMet(kOther.getTeam()))
		return false;

	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
		return true;

	for (EspionageMissionTypes i = (EspionageMissionTypes)0; i < GC.getNumEspionageMissionInfos(); i = (EspionageMissionTypes)(i+1))
	{
		CvEspionageMissionInfo& kMissionInfo = GC.getEspionageMissionInfo(i);

		if (kMissionInfo.isSeeDemographics() && kMissionInfo.isPassive() && canDoEspionageMission(i, ePlayer, 0, 0, 0))
			return true;
	}
	return false;
}
// K-Mod end

bool CvPlayer::isCivic(CivicTypes eCivic) const
{
	int iI;

	for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		if (getCivics((CivicOptionTypes)iI) == eCivic)
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::canDoCivics(CivicTypes eCivic) const
{
	PROFILE_FUNC();

	// Circumvents second crash bug in simultaneous turns MP games
	if( eCivic == NO_CIVIC )
	{
		return true;
	}

	if (GC.getGameINLINE().isForceCivicOption((CivicOptionTypes)(GC.getCivicInfo(eCivic).getCivicOptionType())))
	{
		return GC.getGameINLINE().isForceCivic(eCivic);
	}

	if(GC.getUSE_CAN_DO_CIVIC_CALLBACK())
	{
		CyArgsList argsList;
		argsList.add(getID());
		argsList.add(eCivic);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "canDoCivic", argsList.makeFunctionArgs(), &lResult);
		if (lResult == 1)
		{
			return true;
		}
	}

	if (!isHasCivicOption((CivicOptionTypes)(GC.getCivicInfo(eCivic).getCivicOptionType())) && !(GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getCivicInfo(eCivic).getTechPrereq()))))
	{
		
		if (!(getFreeCivicEnabled() == eCivic)) // Civ4 Reimagined
		{
			return false;
		}
	}

	if(GC.getUSE_CANNOT_DO_CIVIC_CALLBACK())
	{
		CyArgsList argsList2; // XXX
		argsList2.add(getID());
		argsList2.add(eCivic);
		long lResult=0;
		gDLL->getPythonIFace()->callFunction(PYGameModule, "cannotDoCivic", argsList2.makeFunctionArgs(), &lResult);
		if (lResult == 1)
		{
			return false;
		}
	}

	return true;
}


bool CvPlayer::canRevolution(CivicTypes* paeNewCivics) const
{
	int iI;

	if (isAnarchy())
	{
		return false;
	}

	if (getRevolutionTimer() > 0)
	{
		return false;
	}

	if (paeNewCivics == NULL)
	{
		// XXX is this necessary?
		for (iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			if (canDoCivics((CivicTypes)iI))
			{
				if (getCivics((CivicOptionTypes)GC.getCivicInfo((CivicTypes) iI).getCivicOptionType()) != iI)
				{
					return true;
				}
			}
		}
	}
	else
	{
		for (iI = 0; iI < GC.getNumCivicOptionInfos(); ++iI)
		{
			if (GC.getGameINLINE().isForceCivicOption((CivicOptionTypes)iI))
			{
				if (!GC.getGameINLINE().isForceCivic(paeNewCivics[iI]))
				{
					return false;
				}
			}

			if (getCivics((CivicOptionTypes)iI) != paeNewCivics[iI])
			{
				return true;
			}
		}
	}

	return false;
}


// Civ4 Reimagined
void CvPlayer::revolution(CivicTypes* paeNewCivics, bool bForce, bool bAnarchy)
{
	int iAnarchyLength;
	int iI;

	if (!bForce && !canRevolution(paeNewCivics))
	{
		return;
	}

	// Civ4 Reimagined
	iAnarchyLength = !bAnarchy ? 0 : getCivicAnarchyLength(paeNewCivics);

	if (iAnarchyLength > 0)
	{
		changeAnarchyTurns(iAnarchyLength);
	}
	
	for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		setCivics(((CivicOptionTypes)iI), paeNewCivics[iI]);
	}

	setRevolutionTimer(std::max(1, ((100 + getAnarchyModifier()) * GC.getDefineINT("MIN_REVOLUTION_TURNS")) / 100) + iAnarchyLength);

	if (getID() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(Popup_DIRTY_BIT, true); // to force an update of the civic chooser popup
	}
}


int CvPlayer::getCivicPercentAnger(CivicTypes eCivic, bool bIgnore) const
{
	int iCount;
	int iPossibleCount;
	int iI;
	int iAnger;

	// Civ4 Reimagined: Additionally to enabling ideologies, civic anger (currently from Republic) is only active once the ideologies are enabled.
	if (!GC.getGameINLINE().areIdeologiesEnabled())
	{
		return 0;
	}
	
	if (GC.getCivicInfo(eCivic).getCivicPercentAnger() == 0)
	{
		return 0;
	}

	if (!bIgnore && (getCivics((CivicOptionTypes)(GC.getCivicInfo(eCivic).getCivicOptionType())) == eCivic))
	{
		return 0;
	}

	iCount = 0;
	iPossibleCount = 0;

	for (iI = 0; iI < MAX_CIV_PLAYERS; iI++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() != getTeam())
			{
				if (GET_PLAYER((PlayerTypes)iI).getCivics((CivicOptionTypes)(GC.getCivicInfo(eCivic).getCivicOptionType())) == eCivic)
				{
					//iCount += GET_PLAYER((PlayerTypes)iI).getNumCities();
					// Civ4 Reimagined
					iCount += GET_PLAYER((PlayerTypes)iI).getTotalPopulation();
				}

				// Civ4 Reimagined
				//iPossibleCount += GET_PLAYER((PlayerTypes)iI).getNumCities();
				iPossibleCount += GET_PLAYER((PlayerTypes)iI).getTotalPopulation();
			}
		}
	}

	if (iPossibleCount == 0 || iCount == 0)
	{
		return 0;
	}
	
	// Civ4 Reimagined
	iAnger = 500 + (GC.getCivicInfo(eCivic).getCivicPercentAnger() * iCount) / iPossibleCount;
	iAnger = std::min(iAnger, GC.getCivicInfo(eCivic).getCivicPercentAnger() / 4);

	return iAnger;
}


bool CvPlayer::canDoReligion(ReligionTypes eReligion) const
{
	if (GET_TEAM(getTeam()).getHasReligionCount(eReligion) == 0)
	{
		return false;
	}

	return true;
}


bool CvPlayer::canChangeReligion() const
{
	int iI;

	for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (canConvert((ReligionTypes)iI))
		{
			return true;
		}
	}

	return false;
}


bool CvPlayer::canConvert(ReligionTypes eReligion) const
{
	if (isBarbarian())
	{
		return false;
	}

	if (isAnarchy())
	{
		return false;
	}

	if (getConversionTimer() > 0)
	{
		return false;
	}

	if (!isStateReligion())
	{
		return false;
	}

	if (getLastStateReligion() == eReligion)
	{
		return false;
	}

	if (eReligion != NO_RELIGION)
	{
		if (!canDoReligion(eReligion))
		{
			return false;
		}
	}

	return true;
}


void CvPlayer::convert(ReligionTypes eReligion)
{
	if (!canConvert(eReligion))
	{
		return;
	}

	const int iAnarchyLength = getReligionAnarchyLength();

	changeAnarchyTurns(iAnarchyLength);

	setLastStateReligion(eReligion);

	setConversionTimer(std::max(1, ((100 + getAnarchyModifier()) * GC.getDefineINT("MIN_CONVERSION_TURNS")) / 100) + iAnarchyLength);

	// Civ4 Reimagined
	updateCommerce();
	AI_makeAssignWorkDirty();

	if (getTeam() == GC.getGameINLINE().getActiveTeam())
	{
		gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
	}
}


void CvPlayer::convertClassicalTemples(ReligionTypes eReligion)
{
	bool bConverted = false;

	int iLoop;
	for (CvCity* pCity = firstCity(&iLoop); pCity != NULL; pCity = nextCity(&iLoop))
	{
		bConverted = pCity->convertClassicalTemples(eReligion) || bConverted;
	}

	if (bConverted)
	{
		CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_CLASSICAL_TEMPLES_CONVERTED", GC.getReligionInfo(eReligion).getChar());
		gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GOLDAGESTART", MESSAGE_TYPE_MINOR_EVENT);
	}

}


bool CvPlayer::hasHolyCity(ReligionTypes eReligion) const
{
	CvCity* pHolyCity;

	FAssertMsg(eReligion != NO_RELIGION, "Religion is not assigned a valid value");

	pHolyCity = GC.getGameINLINE().getHolyCity(eReligion);

	if (pHolyCity != NULL)
	{
		return (pHolyCity->getOwnerINLINE() == getID());
	}

	return false;
}


int CvPlayer::countHolyCities() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		if (hasHolyCity((ReligionTypes)iI))
		{
			iCount++;
		}
	}

	return iCount;
}


void CvPlayer::foundReligion(ReligionTypes eReligion, ReligionTypes eSlotReligion, bool bAward)
{
	CvCity* pLoopCity;
	CvCity* ppBestCity;
	UnitTypes eFreeUnit;
	bool bStarting;
	int iValue;
	int ipBestValue;
	int iLoop;

	if (NO_RELIGION == eReligion)
	{
		return;
	}

	if (GC.getGameINLINE().isReligionFounded(eReligion))
	{
		if (isHuman())
		{
			CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_FOUND_RELIGION, eSlotReligion);
			if (NULL != pInfo)
			{
				gDLL->getInterfaceIFace()->addPopup(pInfo, getID());
			}
		}
		else
		{
			foundReligion(AI_chooseReligion(), eSlotReligion, bAward);
		}

		return;
	}

	GC.getGameINLINE().setReligionSlotTaken(eSlotReligion, true);

	bStarting = ((GC.getReligionInfo(eSlotReligion).getTechPrereq() == NO_TECH) || (GC.getTechInfo((TechTypes) GC.getReligionInfo(eSlotReligion).getTechPrereq()).getEra() < GC.getGameINLINE().getStartEra()));

	ipBestValue = 0;
	ppBestCity = NULL;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (!bStarting || !(pLoopCity->isHolyCity()))
		{
			iValue = 10;
			iValue += pLoopCity->getPopulation();
			iValue += GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("FOUND_RELIGION_CITY_RAND"), "Found Religion");

			iValue /= (pLoopCity->getReligionCount() + 1);

			if (pLoopCity->isCapital())
			{
				iValue /= 8;
			}

			iValue = std::max(1, iValue);

			if (iValue > ipBestValue)
			{
				ipBestValue = iValue;
				ppBestCity = pLoopCity;
			}
		}
	}

	if (ppBestCity != NULL)
	{
		GC.getGameINLINE().setHolyCity(eReligion, ppBestCity, true);

		if (bAward)
		{
			if (GC.getReligionInfo(eSlotReligion).getNumFreeUnits() > 0)
			{
				eFreeUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(GC.getReligionInfo(eReligion).getFreeUnitClass())));

				if (eFreeUnit != NO_UNIT)
				{
					for (int i = 0; i < GC.getReligionInfo(eSlotReligion).getNumFreeUnits(); ++i)
					{
						initUnit(eFreeUnit, ppBestCity->getX_INLINE(), ppBestCity->getY_INLINE());
					}
				}
			}
		}
	}
}


bool CvPlayer::hasHeadquarters(CorporationTypes eCorporation) const
{
	CvCity* pHeadquarters = GC.getGameINLINE().getHeadquarters(eCorporation);

	FAssert(eCorporation != NO_CORPORATION);

	if (pHeadquarters != NULL)
	{
		return (pHeadquarters->getOwnerINLINE() == getID());
	}

	return false;
}


int CvPlayer::countHeadquarters() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		if (hasHeadquarters((CorporationTypes)iI))
		{
			iCount++;
		}
	}

	return iCount;
}


//int CvPlayer::countCorporations(CorporationTypes eCorporation) const
int CvPlayer::countCorporations(CorporationTypes eCorporation, CvArea* pArea) const // K-Mod
{
	int iCount = 0;

	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (!pArea || pLoopCity->area() == pArea) // K-Mod
		{
			if (pLoopCity->isHasCorporation(eCorporation))
			{
				++iCount;
			}
		}
	}

	return iCount;
}


void CvPlayer::foundCorporation(CorporationTypes eCorporation)
{
	CvCity* pLoopCity;
	CvCity* ppBestCity;
	bool bStarting;
	int iValue;
	int ipBestValue;
	int iLoop;

	if (GC.getGameINLINE().isCorporationFounded(eCorporation))
	{
		return;
	}

	bStarting = ((GC.getCorporationInfo(eCorporation).getTechPrereq() == NO_TECH) || (GC.getTechInfo((TechTypes) GC.getCorporationInfo(eCorporation).getTechPrereq()).getEra() < GC.getGameINLINE().getStartEra()));

	ipBestValue = 0;
	ppBestCity = NULL;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (!bStarting || !(pLoopCity->isHeadquarters()))
		{
			iValue = 10;
			iValue += pLoopCity->getPopulation();

			for (int i = 0; i < GC.getNUM_CORPORATION_PREREQ_BONUSES(); ++i)
			{
				if (NO_BONUS != GC.getCorporationInfo(eCorporation).getPrereqBonus(i))
				{
					iValue += 10 * pLoopCity->getNumBonuses((BonusTypes)GC.getCorporationInfo(eCorporation).getPrereqBonus(i));
				}
			}

			iValue += GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("FOUND_CORPORATION_CITY_RAND"), "Found Corporation");

			iValue /= (pLoopCity->getCorporationCount() + 1);

			iValue = std::max(1, iValue);

			if (iValue > ipBestValue)
			{
				ipBestValue = iValue;
				ppBestCity = pLoopCity;
			}
		}
	}

	if (ppBestCity != NULL)
	{
		ppBestCity->setHeadquarters(eCorporation);
	}
}

/*
 *	How many civics can be changed for 1 turn of anarchy is currently defined with gamestartera in EraInfos.xml 
 *	-> Anarchy Turns = (iAnarchyPercent * CivicsChanged) / 100, rounded down (but min 1 unless player switches for free)
 */
int CvPlayer::getCivicAnarchyLength(CivicTypes* paeNewCivics) const
{
	bool bChange;
	int iAnarchyLength;
	int iI;

	if (getMaxAnarchyTurns() == 0)
	{
		return 0;
	}

	if (isGoldenAge())
	{
		return 0;
	}

	iAnarchyLength = 0;

	bChange = false;

	for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		if (paeNewCivics[iI] != getCivics((CivicOptionTypes)iI))
		{
			if (!isHasNoAnarchyCivicOption((CivicOptionTypes)iI)) // Civ4 Reimagined
			{
				iAnarchyLength += GC.getCivicInfo(paeNewCivics[iI]).getAnarchyLength();

				bChange = true;
			}
		}
	}

	if (bChange)
	{
		iAnarchyLength += GC.getDefineINT("BASE_CIVIC_ANARCHY_LENGTH");

		iAnarchyLength += ((getNumCities() * GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getNumCitiesAnarchyPercent()) / 100);
	}

	iAnarchyLength = ((iAnarchyLength * std::max(0, (getAnarchyModifier() + 100))) / 100);

	if (iAnarchyLength == 0)
	{
		return 0;
	}

	iAnarchyLength *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getAnarchyPercent();
	iAnarchyLength /= 100;

	iAnarchyLength *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getAnarchyPercent();
	iAnarchyLength /= 100;

	return range(iAnarchyLength, 1, getMaxAnarchyTurns());
}


int CvPlayer::getReligionAnarchyLength() const
{
	int iAnarchyLength;

	if (getMaxAnarchyTurns() == 0)
	{
		return 0;
	}

	if (isGoldenAge())
	{
		return 0;
	}

	iAnarchyLength = GC.getDefineINT("BASE_RELIGION_ANARCHY_LENGTH");

	//iAnarchyLength += ((getNumCities() * GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getNumCitiesAnarchyPercent()) / 100); //Civ4 Reimagined: Removed increased anarchy length for number of cities

	iAnarchyLength = ((iAnarchyLength * std::max(0, (getAnarchyModifier() + 100))) / 100);

	if (iAnarchyLength == 0)
	{
		return 0;
	}

	iAnarchyLength *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getAnarchyPercent();
	iAnarchyLength /= 100;

	iAnarchyLength *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getAnarchyPercent();
	iAnarchyLength /= 100;

	return range(iAnarchyLength, 1, getMaxAnarchyTurns());
}



int CvPlayer::unitsRequiredForGoldenAge() const
{
	return (GC.getDefineINT("BASE_GOLDEN_AGE_UNITS") + (getNumUnitGoldenAges() * GC.getDefineINT("GOLDEN_AGE_UNITS_MULTIPLIER")));
}


int CvPlayer::unitsGoldenAgeCapable() const
{
	CvUnit* pLoopUnit;
	int iCount;
	int iLoop;

	iCount = 0;

	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		if (pLoopUnit->isGoldenAge())
		{
			iCount++;
		}
	}

	return iCount;
}

// Rewriten for K-Mod. (The only functionality difference is that unit class is now used rather unit type. But this version is far more efficient.)
int CvPlayer::unitsGoldenAgeReady() const
{
	PROFILE_FUNC();

	std::set<UnitClassTypes> golden_age_units;
	int iGoldenAgeUnits = 0;

	int iLoop;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		// Civ4 Reimagined: Persia UP
		if (pLoopUnit->isGoldenAge() && (isCanGoldenAgeWithSamePeople() || golden_age_units.count(pLoopUnit->getUnitClassType()) == 0))
		{
			golden_age_units.insert(pLoopUnit->getUnitClassType());
			++iGoldenAgeUnits;
		}
	}

	return iGoldenAgeUnits;
}


void CvPlayer::killGoldenAgeUnits(CvUnit* pUnitAlive)
{
	CvUnit* pLoopUnit;
	CvUnit* pBestUnit;
	bool* pabUnitUsed;
	int iUnitsRequired;
	int iValue;
	int ipBestValue;
	int iLoop;
	int iI;

	pabUnitUsed = new bool[GC.getNumUnitInfos()];

	for (iI = 0; iI < GC.getNumUnitInfos(); iI++)
	{
		pabUnitUsed[iI] = false;
	}

	iUnitsRequired = unitsRequiredForGoldenAge();

	if (pUnitAlive != NULL)
	{
		pabUnitUsed[pUnitAlive->getUnitType()] = true;
		iUnitsRequired--;
	}

	for (iI = 0; iI < iUnitsRequired; iI++)
	{
		ipBestValue = 0;
		pBestUnit = NULL;

		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			if (pLoopUnit->isGoldenAge())
			{
				// Civ4 Reimagined: Persia UP
				if (!(pabUnitUsed[pLoopUnit->getUnitType()]) || (isCanGoldenAgeWithSamePeople() && pLoopUnit != pUnitAlive))
				{
					iValue = 10000;

					iValue /= (plotDistance(pLoopUnit->getX_INLINE(), pLoopUnit->getY_INLINE(), pUnitAlive->getX_INLINE(), pUnitAlive->getY_INLINE()) + 1);

					if (iValue > ipBestValue)
					{
						ipBestValue = iValue;
						pBestUnit = pLoopUnit;
					}
				}
			}
		}

		FAssert(pBestUnit != NULL);
		if (pBestUnit != NULL)
		{
			pabUnitUsed[pBestUnit->getUnitType()] = true;

			pBestUnit->kill(true);

			//play animations
			if (pBestUnit->plot()->isActiveVisible(false))
			{
				//kill removes bestUnit from any groups
				pBestUnit->getGroup()->pushMission(MISSION_GOLDEN_AGE, 0);
			}
		}
	}

	SAFE_DELETE_ARRAY(pabUnitUsed);
}


int CvPlayer::greatPeopleThreshold(bool bMilitary) const
{
	int iThreshold;

	if (bMilitary)
	{
		iThreshold = ((GC.getDefineINT("GREAT_GENERALS_THRESHOLD") * std::max(0, (getGreatGeneralsThresholdModifier() + 100))) / 100);
	}
	else
	{
		iThreshold = ((GC.getDefineINT("GREAT_PEOPLE_THRESHOLD") * std::max(0, (getGreatPeopleThresholdModifier() + 100))) / 100);
	}

	iThreshold *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGreatPeoplePercent();
	if (bMilitary)
	{
		iThreshold /= std::max(1, GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getTrainPercent());
	}
	else
	{
		iThreshold /= 100;
	}

	iThreshold *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getGreatPeoplePercent();
	iThreshold /= 100;


	return std::max(1, iThreshold);
}


int CvPlayer::specialistYield(SpecialistTypes eSpecialist, YieldTypes eYield) const
{
	int specialistYield = GC.getSpecialistInfo(eSpecialist).getYieldChange(eYield) + getSpecialistExtraYield(eSpecialist, eYield);
	
	// Civ4 Reimagined
	if (eYield == YIELD_FOOD)
	{
		if (eSpecialist == (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_PRIEST"))
		{
			specialistYield += getEarlyPriestExtraFood();
		}
	}
	
	return specialistYield;
}


int CvPlayer::specialistCommerce(SpecialistTypes eSpecialist, CommerceTypes eCommerce) const
{
	int specialistCommerce = 0; 
	specialistCommerce += GC.getSpecialistInfo(eSpecialist).getCommerceChange(eCommerce);
	specialistCommerce += getSpecialistExtraCommerce(eCommerce);

	// Civ4 Reimagined
	specialistCommerce += getSpecialistCommerceChange(eSpecialist, eCommerce);
	
	// Civ4 Reimagined
	if (eCommerce == COMMERCE_RESEARCH)
	{
		if (eSpecialist == (SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_SCIENTIST"))
		{
			specialistCommerce += getEarlyScientistBonusCommerce();
		}
	}

	return specialistCommerce;
}


CvPlot* CvPlayer::getStartingPlot() const
{
	return GC.getMapINLINE().plotSorenINLINE(m_iStartingX, m_iStartingY);
}


void CvPlayer::setStartingPlot(CvPlot* pNewValue, bool bUpdateStartDist)
{
	CvPlot* pOldStartingPlot;

	pOldStartingPlot = getStartingPlot();

	if (pOldStartingPlot != pNewValue)
	{
		if (pOldStartingPlot != NULL)
		{
			pOldStartingPlot->area()->changeNumStartingPlots(-1);

			if (bUpdateStartDist)
			{
				GC.getMapINLINE().updateMinOriginalStartDist(pOldStartingPlot->area());
			}
		}

		if (pNewValue == NULL)
		{
			m_iStartingX = INVALID_PLOT_COORD;
			m_iStartingY = INVALID_PLOT_COORD;
		}
		else
		{
			m_iStartingX = pNewValue->getX_INLINE();
			m_iStartingY = pNewValue->getY_INLINE();

			getStartingPlot()->area()->changeNumStartingPlots(1);

			if (bUpdateStartDist)
			{
				GC.getMapINLINE().updateMinOriginalStartDist(getStartingPlot()->area());
			}
		}
	}
}


int CvPlayer::getTotalPopulation() const
{
	return m_iTotalPopulation;
}


int CvPlayer::getAveragePopulation() const
{
	if (getNumCities() == 0)
	{
		return 0;
	}

	return ((getTotalPopulation() / getNumCities()) + 1);
}


void CvPlayer::changeTotalPopulation(int iChange)
{
	changeAssets(-(getPopulationAsset(getTotalPopulation())));
	changePower(-(getPopulationPower(getTotalPopulation())));
	changePopScore(-(getPopulationScore(getTotalPopulation())));

	m_iTotalPopulation = (m_iTotalPopulation + iChange);
	FAssert(getTotalPopulation() >= 0);

	changeAssets(getPopulationAsset(getTotalPopulation()));
	changePower(getPopulationPower(getTotalPopulation()));
	changePopScore(getPopulationScore(getTotalPopulation()));
	// Civ4 Reimagined
	updateBonusRatio();

	// Civ4 Reimagined
	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CODE_OF_LAWS")))
	{
		if (getTotalPopulation() > 14)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CODE_OF_LAWS"), getID(), true);
		}
	}
}


long CvPlayer::getRealPopulation() const
{
	CvCity* pLoopCity;
	__int64 iTotalPopulation = 0;
	int iLoop = 0;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		iTotalPopulation += pLoopCity->getRealPopulation();
	}

	if (iTotalPopulation > MAX_INT)
	{
		iTotalPopulation = MAX_INT;
	}

	return ((long)(iTotalPopulation));
}


int CvPlayer::getTotalLand() const
{
	return m_iTotalLand;
}


void CvPlayer::changeTotalLand(int iChange)
{
	m_iTotalLand = (m_iTotalLand + iChange);
	FAssert(getTotalLand() >= 0);
}


int CvPlayer::getTotalLandScored() const
{
	return m_iTotalLandScored;
}


void CvPlayer::changeTotalLandScored(int iChange)
{
	if (iChange != 0)
	{
		changeAssets(-(getLandPlotsAsset(getTotalLandScored())));
		changeLandScore(-(getLandPlotsScore(getTotalLandScored())));

		m_iTotalLandScored = (m_iTotalLandScored + iChange);
		FAssert(getTotalLandScored() >= 0);

		changeAssets(getLandPlotsAsset(getTotalLandScored()));
		changeLandScore(getLandPlotsScore(getTotalLandScored()));
	}
}


int CvPlayer::getGold() const
{
	return m_iGold;
}


void CvPlayer::setGold(int iNewValue)
{
	if (getGold() != iNewValue)
	{
		m_iGold = iNewValue;

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_BANKING")))
		{
			if (getGold() >= 500)
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_BANKING"), getID(), true);
			}
		}
	}
}

void CvPlayer::changeGold(int iChange)
{
	setGold(getGold() + iChange);
}

int CvPlayer::getGoldPerTurn() const
{
	return m_iGoldPerTurn;
}

int CvPlayer::getAdvancedStartPoints() const
{
	return m_iAdvancedStartPoints;
}


void CvPlayer::setAdvancedStartPoints(int iNewValue)
{
	if (getAdvancedStartPoints() != iNewValue)
	{
		m_iAdvancedStartPoints = iNewValue;

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(SelectionButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

void CvPlayer::changeAdvancedStartPoints(int iChange)
{
	setAdvancedStartPoints(getAdvancedStartPoints() + iChange);
}


int CvPlayer::getGoldenAgeTurns() const																		 
{
	return m_iGoldenAgeTurns;
}


bool CvPlayer::isGoldenAge() const																					 
{
	return (getGoldenAgeTurns() > 0);
}


void CvPlayer::changeGoldenAgeTurns(int iChange)
{
	CvWString szBuffer;
	bool bOldGoldenAge;
	int iI;

	if (iChange != 0)
	{
		bOldGoldenAge = isGoldenAge();

		m_iGoldenAgeTurns = (m_iGoldenAgeTurns + iChange);
		FAssert(getGoldenAgeTurns() >= 0);

		if (bOldGoldenAge != isGoldenAge())
		{
			if (isGoldenAge())
			{
				changeAnarchyTurns(-getAnarchyTurns());
				// K-Mod. Allow the AI to reconsider their civics. (a golden age is a good time for reform!)
				if (!isHuman() && getMaxAnarchyTurns() != 0 && getAnarchyModifier() + 100 > 0)
				{
					GET_PLAYER(getID()).AI_setCivicTimer(0); // silly, I know. But what else can I do?
				}
				// K-Mod end
			}

			updateYield();

			if (isGoldenAge())
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_GOLDEN_AGE_BEGINS", getNameKey());
				GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

				CvEventReporter::getInstance().goldenAge(getID());

				// Civ4 Reimagined
				if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_DEMOCRACY")))
				{
					if (getPlayerRecord()->getNumGoldenAges() > 1)
					{
						GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_DEMOCRACY"), getID(), true);
					}
				}
			}
			else
			{
				CvEventReporter::getInstance().endGoldenAge(getID());
			}

			for (iI = 0; iI < MAX_PLAYERS; iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive())
				{
					if (GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
					{
						if (isGoldenAge())
						{
							szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_GOLDEN_AGE_HAS_BEGUN", getNameKey());
							gDLL->getInterfaceIFace()->addHumanMessage(((PlayerTypes)iI), (((PlayerTypes)iI) == getID()), GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GOLDAGESTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
						}
						else
						{
							szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_GOLDEN_AGE_ENDED", getNameKey());
							gDLL->getInterfaceIFace()->addHumanMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GOLDAGEEND", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
						}
					}
				}
			}
		}

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

int CvPlayer::getGoldenAgeLength() const	
{
	return (GC.getGameINLINE().goldenAgeLength() * std::max(0, 100 + getGoldenAgeModifier())) / 100;
}

int CvPlayer::getNumUnitGoldenAges() const																			 
{
	return m_iNumUnitGoldenAges;
}


void CvPlayer::changeNumUnitGoldenAges(int iChange)												 
{
	m_iNumUnitGoldenAges = (m_iNumUnitGoldenAges + iChange);
	FAssert(getNumUnitGoldenAges() >= 0);
}


int CvPlayer::getStrikeTurns() const
{
	return m_iStrikeTurns;
}


void CvPlayer::changeStrikeTurns(int iChange)
{
	m_iStrikeTurns = (m_iStrikeTurns + iChange);
	FAssert(getStrikeTurns() >= 0);
}


int CvPlayer::getAnarchyTurns() const																			
{
	return m_iAnarchyTurns;
}


bool CvPlayer::isAnarchy() const																						
{
	return (getAnarchyTurns() > 0);
}


void CvPlayer::changeAnarchyTurns(int iChange)
{
	bool bOldAnarchy;

	if (iChange != 0)
	{
		bOldAnarchy = isAnarchy();

		m_iAnarchyTurns = (m_iAnarchyTurns + iChange);
		FAssert(getAnarchyTurns() >= 0);

		if (bOldAnarchy != isAnarchy())
		{
			updateCommerce();
			updateMaintenance();
			updateTradeRoutes();
			updateCorporation();

			AI_makeAssignWorkDirty();

			if (isAnarchy())
			{
				gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_REVOLUTION_HAS_BEGUN").GetCString(), "AS2D_REVOLTSTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
			}
			else
			{
				gDLL->getInterfaceIFace()->addHumanMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_REVOLUTION_OVER").GetCString(), "AS2D_REVOLTEND", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
				// K-Mod. trigger production/research popups that have been suppressed.
				if (isHuman())
				{
					if (isResearch() && getCurrentResearch() == NO_TECH)
					{
						chooseTech();
					}

					int iLoop;
					for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
					{
						if (pLoopCity->AI_isChooseProductionDirty() && !pLoopCity->isProduction() && !pLoopCity->isDisorder() && !pLoopCity->isProductionAutomated())
						{
							pLoopCity->chooseProduction();
						}
					}
				}
				// K-Mod end

				// Civ4 Reimagined: French UP
				CvCity* pCapitalCity = getCapitalCity();
				if (isFrenchRevolution() && pCapitalCity != NULL && getIdeology() == IDEOLOGY_LIBERALISM)
				{
					const UnitTypes eGeneral = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL");

					if (isGoldenAge())
					{
						pCapitalCity->createGreatPeople(eGeneral, false, false);
						setIsFrenchRevolution(false);
					}
					else
					{
						pCapitalCity->createGreatPeople(eGeneral, false, false);
						changeGoldenAgeTurns(getGoldenAgeLength() + 1);
						setIsFrenchRevolution(false);
					}
				}
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
			}

			if (getTeam() == GC.getGameINLINE().getActiveTeam())
			{
				gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
			}
		}

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getMaxAnarchyTurns() const																		 
{
	return m_iMaxAnarchyTurns;
}


void CvPlayer::updateMaxAnarchyTurns()
{
	int ipBestValue;
	int iI;

	ipBestValue = GC.getDefineINT("MAX_ANARCHY_TURNS");

	FAssertMsg((GC.getNumTraitInfos() > 0), "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::updateMaxAnarchyTurns");
	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			if (GC.getTraitInfo((TraitTypes)iI).getMaxAnarchy() >= 0)
			{
				if (GC.getTraitInfo((TraitTypes)iI).getMaxAnarchy() < ipBestValue)
				{
					ipBestValue = GC.getTraitInfo((TraitTypes)iI).getMaxAnarchy();
				}
			}
		}
	}

	m_iMaxAnarchyTurns = ipBestValue;
	FAssert(getMaxAnarchyTurns() >= 0);
}


int CvPlayer::getAnarchyModifier() const
{
	return m_iAnarchyModifier;
}


void CvPlayer::changeAnarchyModifier(int iChange)
{
	if (0 != iChange)
	{
		/* original bts code
		setRevolutionTimer(std::max(0, ((100 + iChange) * getRevolutionTimer()) / 100));
		setConversionTimer(std::max(0, ((100 + iChange) * getConversionTimer()) / 100)); */
		// K-Mod. The original code is wrong, and it is missing the anarchy length change.
		changeRevolutionTimer(getRevolutionTimer() * iChange / std::max(1, 100+getAnarchyModifier()));
		changeConversionTimer(getConversionTimer() * iChange / std::max(1, 100+getAnarchyModifier()));
		changeAnarchyTurns(getAnarchyTurns() * iChange / std::max(1, 100+getAnarchyModifier()));
		// K-Mod end

		m_iAnarchyModifier += iChange;
	}
}


int CvPlayer::getGoldenAgeModifier() const
{
	return m_iGoldenAgeModifier;
}


void CvPlayer::changeGoldenAgeModifier(int iChange)
{
	// K-Mod. If we are currently in a golden age, adjust its duration!
	changeGoldenAgeTurns(getGoldenAgeTurns() * iChange / std::max(1, 100 + getGoldenAgeModifier()));
	// K-Mod end
	m_iGoldenAgeModifier += iChange;
}


int CvPlayer::getHurryModifier() const
{
	return m_iGlobalHurryModifier;
}


void CvPlayer::changeHurryModifier(int iChange)
{
	m_iGlobalHurryModifier = (m_iGlobalHurryModifier + iChange);
}


int CvPlayer::getGreatPeopleCreated() const
{
	return m_iGreatPeopleCreated;
}


void CvPlayer::incrementGreatPeopleCreated()
{
	m_iGreatPeopleCreated++;
}

int CvPlayer::getGreatGeneralsCreated() const
{
	return m_iGreatGeneralsCreated;
}

void CvPlayer::incrementGreatGeneralsCreated()
{
	m_iGreatGeneralsCreated++;
}

int CvPlayer::getGreatPeopleThresholdModifier() const
{
	return m_iGreatPeopleThresholdModifier;
}


void CvPlayer::changeGreatPeopleThresholdModifier(int iChange)
{
	m_iGreatPeopleThresholdModifier = (m_iGreatPeopleThresholdModifier + iChange);
}


int CvPlayer::getGreatGeneralsThresholdModifier() const
{
	return m_iGreatGeneralsThresholdModifier;
}


void CvPlayer::changeGreatGeneralsThresholdModifier(int iChange)
{
	m_iGreatGeneralsThresholdModifier += iChange;
}


int CvPlayer::getGreatPeopleRateModifier() const
{
	return m_iGreatPeopleRateModifier;
}


void CvPlayer::changeGreatPeopleRateModifier(int iChange)													
{
	m_iGreatPeopleRateModifier = (m_iGreatPeopleRateModifier + iChange);
}


// Civ4 Reimagined
int CvPlayer::getNoGreatPeopleCount() const														
{
	return m_iNoGreatPeopleCount;
}


// Civ4 Reimagined
bool CvPlayer::isNoGreatPeople() const																
{
	return (getNoGreatPeopleCount() > 0);
}


// Civ4 Reimagined
void CvPlayer::changeNoGreatPeopleCount(int iChange)											
{
	if (iChange != 0)
	{
		m_iNoGreatPeopleCount = (m_iNoGreatPeopleCount + iChange);
		FAssert(m_iNoGreatPeopleCount >= 0);
	}
}


// Civ4 Reimagined
int CvPlayer::getNoCultureFlipCount() const														
{
	return m_iNoCultureFlipCount;
}


// Civ4 Reimagined
bool CvPlayer::isNoCultureFlip() const																
{
	return (getNoCultureFlipCount() > 0);
}


// Civ4 Reimagined
void CvPlayer::changeNoCultureFlipCount(int iChange)											
{
	if (iChange != 0)
	{
		m_iNoCultureFlipCount = (m_iNoCultureFlipCount + iChange);
		FAssert(m_iNoCultureFlipCount >= 0);
	}
}


int CvPlayer::getGreatGeneralRateModifier() const
{
	return m_iGreatGeneralRateModifier;
}


void CvPlayer::changeGreatGeneralRateModifier(int iChange)													
{
	m_iGreatGeneralRateModifier += iChange;
}


int CvPlayer::getDomesticGreatGeneralRateModifier() const
{
	return (GC.getDefineINT("COMBAT_EXPERIENCE_IN_BORDERS_PERCENT") + m_iDomesticGreatGeneralRateModifier);
}


void CvPlayer::changeDomesticGreatGeneralRateModifier(int iChange)													
{
	m_iDomesticGreatGeneralRateModifier += iChange;
}


int CvPlayer::getStateReligionGreatPeopleRateModifier() const
{
	return m_iStateReligionGreatPeopleRateModifier;
}


void CvPlayer::changeStateReligionGreatPeopleRateModifier(int iChange)													
{
	m_iStateReligionGreatPeopleRateModifier = (m_iStateReligionGreatPeopleRateModifier + iChange);
}


int CvPlayer::getMaxGlobalBuildingProductionModifier() const
{
	return m_iMaxGlobalBuildingProductionModifier;
}


void CvPlayer::changeMaxGlobalBuildingProductionModifier(int iChange)
{
	m_iMaxGlobalBuildingProductionModifier = (m_iMaxGlobalBuildingProductionModifier + iChange);
}


int CvPlayer::getMaxTeamBuildingProductionModifier() const												 
{
	return m_iMaxTeamBuildingProductionModifier;
}


void CvPlayer::changeMaxTeamBuildingProductionModifier(int iChange)
{
	m_iMaxTeamBuildingProductionModifier = (m_iMaxTeamBuildingProductionModifier + iChange);
}


int CvPlayer::getMaxPlayerBuildingProductionModifier() const												
{
	return m_iMaxPlayerBuildingProductionModifier;
}


void CvPlayer::changeMaxPlayerBuildingProductionModifier(int iChange)
{
	m_iMaxPlayerBuildingProductionModifier = (m_iMaxPlayerBuildingProductionModifier + iChange);
}

// Civ4 Reimagined
int CvPlayer::getBuildingsProductionModifier() const
{
	return m_iBuildingsProductionModifier;
}

// Civ4 Reimagined: To modify the production speed of a single building instead of all buildings, use CvPlayer::changeBuildingProductionModifier(BuildingTypes eIndex, int iChange).
void CvPlayer::changeBuildingsProductionModifier(int iChange)
{
	m_iBuildingsProductionModifier = (m_iBuildingsProductionModifier + iChange);
}


int CvPlayer::getFreeExperience() const																
{
	return m_iFreeExperience;
}


void CvPlayer::changeFreeExperience(int iChange)
{
	m_iFreeExperience = (m_iFreeExperience + iChange);
}


int CvPlayer::getFeatureProductionModifier() const
{
	return m_iFeatureProductionModifier;
}


void CvPlayer::changeFeatureProductionModifier(int iChange)
{
	m_iFeatureProductionModifier = (m_iFeatureProductionModifier + iChange);
}


int CvPlayer::getWorkerSpeedModifier() const
{
	return m_iWorkerSpeedModifier;
}


void CvPlayer::changeWorkerSpeedModifier(int iChange)
{
	m_iWorkerSpeedModifier = (m_iWorkerSpeedModifier + iChange);
}


int CvPlayer::getImprovementUpgradeRateModifier() const
{
	return m_iImprovementUpgradeRateModifier;
}


void CvPlayer::changeImprovementUpgradeRateModifier(int iChange)
{
	m_iImprovementUpgradeRateModifier = (m_iImprovementUpgradeRateModifier + iChange);
}


int CvPlayer::getMilitaryProductionModifier() const
{
	return m_iMilitaryProductionModifier;
}


void CvPlayer::changeMilitaryProductionModifier(int iChange)
{
	m_iMilitaryProductionModifier = (m_iMilitaryProductionModifier + iChange);
}


int CvPlayer::getSpaceProductionModifier() const													 
{
	return m_iSpaceProductionModifier;
}


void CvPlayer::changeSpaceProductionModifier(int iChange)
{
	m_iSpaceProductionModifier = (m_iSpaceProductionModifier + iChange);
}


int CvPlayer::getCityDefenseModifier() const
{
	return m_iCityDefenseModifier;
}


void CvPlayer::changeCityDefenseModifier(int iChange)
{
	m_iCityDefenseModifier = (m_iCityDefenseModifier + iChange);
}


int CvPlayer::getNumNukeUnits() const
{
	return m_iNumNukeUnits;
}


void CvPlayer::changeNumNukeUnits(int iChange)
{
	m_iNumNukeUnits = (m_iNumNukeUnits + iChange);
	FAssert(getNumNukeUnits() >= 0);
}


int CvPlayer::getNumOutsideUnits() const
{
	return m_iNumOutsideUnits;
}


void CvPlayer::changeNumOutsideUnits(int iChange)
{
	if (iChange != 0)
	{
		m_iNumOutsideUnits += iChange;
		FAssert(getNumOutsideUnits() >= 0);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getFreeWorkers() const																		
{
	return m_iFreeWorkers;
}


void CvPlayer::changeFreeWorkers(int iChange)														
{
	const UnitClassTypes eWorkerClass = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_WORKER");

	if (iChange != 0)
	{
		m_iFreeWorkers = (m_iFreeWorkers + iChange);

		if (iChange > 0)
		{
			initFreeWorkers(iChange);
		}
		else
		{
			int iLoop;
			int iKilledUnits = 0;
			for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL && iKilledUnits < -iChange; pLoopUnit = nextUnit(&iLoop))
			{
				if (pLoopUnit->isFreeWorker())
				{
					pLoopUnit->kill(false, getID());
					iKilledUnits++;
				}
			}

			if (isUnitClassMaxedOut(eWorkerClass, -1))
			{
				for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL && isUnitClassMaxedOut(eWorkerClass, -1); pLoopUnit = nextUnit(&iLoop))
				{
					if (pLoopUnit->getUnitClassType() == eWorkerClass)
					{
						pLoopUnit->kill(false, getID());
						iKilledUnits++;
					}
				}
			}
			
			if (iKilledUnits > 0)
			{
				gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_FREE_WORKERS_LOST").GetCString(), "AS2D_UNITDISBANDED", MESSAGE_TYPE_MINOR_EVENT);
			}
		}
	}
}


int CvPlayer::getBaseFreeUnits() const																		
{
	return m_iBaseFreeUnits;
}


void CvPlayer::changeBaseFreeUnits(int iChange)														
{
	if (iChange != 0)
	{
		m_iBaseFreeUnits = (m_iBaseFreeUnits + iChange);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getBaseFreeMilitaryUnits() const																		
{
	return m_iBaseFreeMilitaryUnits;
}


void CvPlayer::changeBaseFreeMilitaryUnits(int iChange)														
{
	if (iChange != 0)
	{
		m_iBaseFreeMilitaryUnits = (m_iBaseFreeMilitaryUnits + iChange);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getFreeUnitsPopulationPercent() const																	
{
	return m_iFreeUnitsPopulationPercent;
}


void CvPlayer::changeFreeUnitsPopulationPercent(int iChange)													
{
	if (iChange != 0)
	{
		m_iFreeUnitsPopulationPercent = (m_iFreeUnitsPopulationPercent + iChange);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getFreeMilitaryUnitsPopulationPercent() const															
{
	return m_iFreeMilitaryUnitsPopulationPercent;
}


void CvPlayer::changeFreeMilitaryUnitsPopulationPercent(int iChange)											
{
	if (iChange != 0)
	{
		m_iFreeMilitaryUnitsPopulationPercent = (m_iFreeMilitaryUnitsPopulationPercent + iChange);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

// K-Mod
int CvPlayer::getTypicalUnitValue(UnitAITypes eUnitAI, DomainTypes eDomain) const
{
	//UnitTypes eBestUnit = NO_UNIT;
	int iHighestValue = 0;

	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eLoopUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI)));

		if (eLoopUnit != NO_UNIT &&
			(eUnitAI == NO_UNITAI || GC.getUnitInfo(eLoopUnit).getUnitAIType(eUnitAI)) &&
			(eDomain == NO_DOMAIN || GC.getUnitInfo(eLoopUnit).getDomainType() == eDomain) &&
			canTrain(eLoopUnit))
		{
			// Note: currently the above checks do not consider any resource prerequites.
			int iValue = GC.getGameINLINE().AI_combatValue(eLoopUnit);
			if (iValue > iHighestValue)
			{
				iHighestValue = iValue;
				//eBestUnit = eLoopUnit;
			}
		}
		
		// Civ4 Reimagined: Quick and dirty adjustment for player traits.
		for (int iK = 0; iK < GC.getNumTraitInfos(); iK++)
		{
			if (hasTrait((TraitTypes)iK))
			{
				if (GC.getTraitInfo((TraitTypes)iK).getLevelExperienceModifier() != 0)
				{
					iHighestValue *= (100 - GC.getTraitInfo((TraitTypes)iK).getLevelExperienceModifier() / 3);
					iHighestValue /= 100;
				}
				
				for (int iJ = 0; iJ < GC.getNumPromotionInfos(); iJ++)
				{
					if (GC.getTraitInfo((TraitTypes)iK).isFreePromotion(iJ))
					{
						iHighestValue *= 6;
						iHighestValue /= 5;
						break; // Protective shall have the same value as aggressive.
					}
				}
			}
		}
	}

	return iHighestValue;
}

int CvPlayer::getGoldPerUnit() const																		
{
	return m_iGoldPerUnit;
}


void CvPlayer::changeGoldPerUnit(int iChange)															
{
	if (iChange != 0)
	{
		m_iGoldPerUnit = (m_iGoldPerUnit + iChange);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getGoldPerMilitaryUnit() const
{
	return m_iGoldPerMilitaryUnit;
}


void CvPlayer::changeGoldPerMilitaryUnit(int iChange)
{
	if (iChange != 0)
	{
		m_iGoldPerMilitaryUnit = (m_iGoldPerMilitaryUnit + iChange);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getExtraUnitCost() const
{
	return m_iExtraUnitCost;
}


void CvPlayer::changeExtraUnitCost(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraUnitCost = (m_iExtraUnitCost + iChange);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getNumMilitaryUnits() const
{
	return m_iNumMilitaryUnits;
}


void CvPlayer::changeNumMilitaryUnits(int iChange)													
{
	if (iChange != 0)
	{
		m_iNumMilitaryUnits = (m_iNumMilitaryUnits + iChange);
		FAssert(getNumMilitaryUnits() >= 0);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_HORSEBACK_RIDING")))
		{
			if (getUnitClassCount((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_CHARIOT")) > 0)
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_HORSEBACK_RIDING"), getID(), true);
			}
		}

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_COMPASS")))
		{
			if (getUnitClassCount((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_GALLEY")) > 1)
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_COMPASS"), getID(), true);
			}
		}

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_GUNPOWDER")))
		{
			if (getUnitClassCount((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_CROSSBOWMAN")) > 5)
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_GUNPOWDER"), getID(), true);
			}
		}

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_STEEL")))
		{
			if (getUnitClassCount((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_IRONCLAD")) > 2)
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_STEEL"), getID(), true);
			}
		}

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_ARTILLERY")))
		{
			if (getUnitClassCount((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_MACHINE_GUN")) > 2)
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_ARTILLERY"), getID(), true);
			}
		}

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_FLIGHT")))
		{
			if (getUnitClassCount((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_AIRSHIP")) > 2)
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_FLIGHT"), getID(), true);
			}
		}

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_PLASTICS")))
		{
			if (getUnitClassCount((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_TANK")) > 4)
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_PLASTICS"), getID(), true);
			}
		}

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_RADIO")))
		{
			if (getUnitClassCount((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_CRUISER")) > 4)
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_RADIO"), getID(), true);
			}
		}

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_SATELLITES")))
		{
			if (getPlayerRecord()->getNumUnitsBuilt((UnitTypes)GC.getInfoTypeForString("UNIT_GUIDED_MISSILE")) > 4)
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_SATELLITES"), getID(), true);
			}
		}
	}
}

// Civ4 Reimagined
int CvPlayer::getNoUnitSupplyCount() const														
{
	return m_iNoUnitSupplyCount;
}


// Civ4 Reimagined
bool CvPlayer::isNoUnitSupply() const																
{
	return (getNoUnitSupplyCount() > 0);
}


// Civ4 Reimagined
void CvPlayer::changeNoUnitSupplyCount(int iChange)											
{
	if (iChange != 0)
	{
		m_iNoUnitSupplyCount = (m_iNoUnitSupplyCount + iChange);
		FAssert(getNoUnitSupplyCount() >= 0);
	}
}


int CvPlayer::getHappyPerMilitaryUnit() const																
{
	return m_iHappyPerMilitaryUnit;
}


void CvPlayer::changeHappyPerMilitaryUnit(int iChange)
{
	if (iChange != 0)
	{
		m_iHappyPerMilitaryUnit = (m_iHappyPerMilitaryUnit + iChange);

		AI_makeAssignWorkDirty();
	}
}


// Leoreth
int CvPlayer::getMilitaryHappinessLimit() const
{
	return m_iMilitaryHappinessLimit;
}


// Leoreth
void CvPlayer::changeMilitaryHappinessLimit(int iChange)
{
	if (iChange != 0)
	{
		m_iMilitaryHappinessLimit = (m_iMilitaryHappinessLimit + iChange);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getMilitaryFoodProductionCount() const														
{
	return m_iMilitaryFoodProductionCount;
}


bool CvPlayer::isMilitaryFoodProduction() const																
{
	return (getMilitaryFoodProductionCount() > 0);
}


void CvPlayer::changeMilitaryFoodProductionCount(int iChange)											
{
	if (iChange != 0)
	{
		m_iMilitaryFoodProductionCount = (m_iMilitaryFoodProductionCount + iChange);
		FAssert(getMilitaryFoodProductionCount() >= 0);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


// Civ4 Reimagind
int CvPlayer::getMeleeMilitaryFoodProductionCount() const														
{
	return m_iMeleeMilitaryFoodProductionCount;
}


// Civ4 Reimagind
bool CvPlayer::isMeleeMilitaryFoodProduction() const																
{
	return (getMeleeMilitaryFoodProductionCount() > 0);
}


// Civ4 Reimagind
void CvPlayer::changeMeleeMilitaryFoodProductionCount(int iChange)											
{
	if (iChange != 0)
	{
		m_iMeleeMilitaryFoodProductionCount = (m_iMeleeMilitaryFoodProductionCount + iChange);
		FAssert(getMeleeMilitaryFoodProductionCount() >= 0);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


// Civ4 Reimagined
int CvPlayer::getEnableSlavesCount() const														
{
	return m_iEnableSlavesCount;
}


// Civ4 Reimagined
bool CvPlayer::hasSlavery() const																
{
	return (getEnableSlavesCount() > 0);
}


// Civ4 Reimagined
void CvPlayer::changeEnableSlavesCount(int iChange)											
{
	if (iChange != 0)
	{
		if (m_iEnableSlavesCount <= 0 && iChange > 0) // Slavery starts
		{
			setSlaveThreshold(GC.getDefineINT("SLAVE_THRESHOLD_START"));
		}
		
		m_iEnableSlavesCount = (m_iEnableSlavesCount + iChange);
		FAssert(m_iEnableSlavesCount >= 0);
		int iLoop;
		
		if (m_iEnableSlavesCount <= 0 && iChange < 0) // Slavery ends
		{
			if (getNumSlaveUnits() > 0)
			{
				for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
				{
					if ((UnitClassTypes)pLoopUnit->getUnitInfo().getUnitClassType() == (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_SLAVE"))
					{
						pLoopUnit->kill(false, getID());
					}
				}
				
				gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_SLAVES_LOST").GetCString(), "AS2D_UNITDISBANDED", MESSAGE_TYPE_MINOR_EVENT);

				// Civ4 Reimagined
				if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_LIBERALISM")))
				{
					GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_LIBERALISM"), getID(), true);
				}
			}
		}
	}
}


// Civ4 Reimagined
int CvPlayer::getNumSlaveUnits() const														
{
	return m_iSlavesCount;
}


// Civ4 Reimagined
void CvPlayer::changeNumSlaveUnits(int iChange)											
{
	if (iChange != 0)
	{
		m_iSlavesCount = (m_iSlavesCount + iChange);
		FAssert(m_iSlavesCount >= 0);

		updateCorporation();
	}
}


// Civ4 Reimagined
int CvPlayer::getSlavePoints() const														
{
	return m_iSlavePoints;
}


// Civ4 Reimagined
void CvPlayer::changeSlavePoints(int iChange)											
{
	if (iChange != 0)
	{
		m_iSlavePoints = (m_iSlavePoints + iChange);
		FAssert(m_iSlavePoints >= 0);
	}
}


// Civ4 Reimagined
int CvPlayer::getNewSlaveThreshold() const
{
	return m_iSlaveThreshold;
}


// Civ4 Reimagined
void CvPlayer::setSlaveThreshold(int iValue)
{
	m_iSlaveThreshold = iValue;
}


// Civ4 Reimagined
void CvPlayer::gainSlavePoints(int iChange, CvCity *pCity)
{
	changeSlavePoints(iChange);

	if (getSlavePoints() >= getNewSlaveThreshold())
	{
		if (pCity != NULL)
		{
			initSlave(pCity, true);
		}
		else
		{
			if (getCapitalCity() != NULL)
			{
				initSlave(getCapitalCity(), true);
			}
		}
	}
}


// Civ4 Reimagined
void CvPlayer::initSlave(CvCity* pCity, bool bIncreaseThreshold)
{
	const UnitClassTypes UNITCLASS_SLAVE = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_SLAVE");
	const UnitTypes eSlave = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(UNITCLASS_SLAVE);

	initUnit(eSlave, pCity->getX_INLINE(), pCity->getY_INLINE(), UNITAI_WORKER);
	gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_SLAVE_STARTED_WORKING", pCity->getNameKey()).GetCString(), "AS2D_UNIT_BUILD_UNIT", MESSAGE_TYPE_MINOR_EVENT);
	if (gPlayerLogLevel > 0) logBBAI("Slave starts working in %S", pCity->getName(0).GetCString());
	
	if (bIncreaseThreshold)
	{
		int iNewSlaveThreshold = getNewSlaveThreshold();
		changeSlavePoints(-1 * iNewSlaveThreshold);
		iNewSlaveThreshold *= 100 + GC.getDefineINT("SLAVE_THRESHOLD_INCREASE_PERCENT");
		iNewSlaveThreshold /= 100;
		setSlaveThreshold(iNewSlaveThreshold);
	}	
}


/*

// Civ4 Reimagined
int CvPlayer::getNumColonies() const														
{
	return m_iNumColonies;
}


// Civ4 Reimagined
void CvPlayer::changeNumColonies(int iChange)											
{
	if (iChange != 0)
	{
		m_iNumColonies = (m_iNumColonies + iChange);
		FAssert(m_iNumColonies >= 0);
	}
}


// Civ4 Reimagined
void CvPlayer::updateNumColonies()
{
	int iLoop;
	int iCount = 0;
	CvCity* pLoopCity;
	CvCity* pCapitalCity = getCapitalCity();
	
	if (pCapitalCity)
	{
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			
		}
	}
	m_iNumColonies = iCount;
}

*/


//Civ4 Reimagined
void CvPlayer::updateColonialCities()
{
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateColonialStatus();
		pLoopCity->updateDistanceToCapital();
	}
}


// Civ4 Reimagined
int CvPlayer::getNoMilitaryProductionMaliCount() const														
{
	return m_iNoMilitaryProductionMaliCount;
}


// Civ4 Reimagined
bool CvPlayer::isNoMilitaryProductionMali() const																
{
	return (getNoMilitaryProductionMaliCount() > 0);
}


// Civ4 Reimagined
void CvPlayer::changeNoMilitaryProductionMaliCount(int iChange)											
{
	if (iChange != 0)
	{
		m_iNoMilitaryProductionMaliCount = (m_iNoMilitaryProductionMaliCount + iChange);
		FAssert(getNoMilitaryProductionMaliCount() >= 0);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getHighestUnitLevel()	const																
{
	return m_iHighestUnitLevel;
}


void CvPlayer::setHighestUnitLevel(int iNewValue)
{
	m_iHighestUnitLevel = iNewValue;
	FAssert(getHighestUnitLevel() >= 0);
}

// Civ4 Reimagined
int CvPlayer::getHighestNavalUnitLevel() const
{
	return m_iHighestNavalUnitLevel;
}

// Civ4 Reimagined
void CvPlayer::setHighestNavalUnitLevel(int iNewValue)
{
	m_iHighestNavalUnitLevel = iNewValue;
	FAssert(getHighestNavalUnitLevel() >= 0);
}

// Civ4 Reimagined
int CvPlayer::getLootingModifier() const
{
	return m_iLootingModifier;
}

// Civ4 Reimagined
void CvPlayer::changeLootingModifier(int iChange)
{
	m_iLootingModifier = (m_iLootingModifier + iChange);
	FAssert(getLootingModifier() >= -100);
}

// Civ4 Reimagined
bool CvPlayer::isBarbarianGreatGeneral() const
{
	if (m_iBarbarianGreatGeneralCount > 0)
	{
		return true;
	} 
	else
	{
		return false;
	}
}

// Civ4 Reimagined
void CvPlayer::changeBarbarianGreatGeneralCount(int iChange)
{
	m_iBarbarianGreatGeneralCount = (m_iBarbarianGreatGeneralCount + iChange);
	FAssert(m_iBarbarianGreatGeneralCount >= 0);
}

// Civ4 Reimagined
bool CvPlayer::isUnlimitedBarbXP() const
{
	if (m_iUnlimitedBarbXPCount > 0)
	{
		return true;
	} 
	else
	{
		return false;
	}
}

// Civ4 Reimagined
void CvPlayer::changeUnlimitedBarbXPCount(int iChange)
{
	m_iUnlimitedBarbXPCount = (m_iUnlimitedBarbXPCount + iChange);
	FAssert(m_iUnlimitedBarbXPCount >= 0);
}

// Civ4 Reimagined
bool CvPlayer::isNoWarAgainstSameFaithUnhappiness() const
{
	if (m_iNoWarAgainstSameFaithUnhappinessCount > 0)
	{
		return true;
	} 
	else
	{
		return false;
	}
}

// Civ4 Reimagined
void CvPlayer::changeNoWarAgainstSameFaithUnhappinessCount(int iChange)
{
	m_iNoWarAgainstSameFaithUnhappinessCount += iChange;
	FAssert(m_iNoWarAgainstSameFaithUnhappinessCount >= 0);
}

// Civ4 Reimagined
bool CvPlayer::isUnlimitedAnimalXP() const
{
	if (m_iUnlimitedAnimalXPCount > 0)
	{
		return true;
	} 
	else
	{
		return false;
	}
}

// Civ4 Reimagined
void CvPlayer::changeUnlimitedAnimalXPCount(int iChange)
{
	m_iUnlimitedAnimalXPCount = (m_iUnlimitedAnimalXPCount + iChange);
	FAssert(m_iUnlimitedAnimalXPCount >= 0);
}

int CvPlayer::getMaxConscript() const
{
	return m_iMaxConscript;
}


void CvPlayer::changeMaxConscript(int iChange)
{
	m_iMaxConscript = (m_iMaxConscript + iChange);
	FAssert(getMaxConscript() >= 0);
}


int CvPlayer::getConscriptCount() const
{
	return m_iConscriptCount;
}


void CvPlayer::setConscriptCount(int iNewValue)
{
	m_iConscriptCount = iNewValue;
	FAssert(getConscriptCount() >= 0);
}


void CvPlayer::changeConscriptCount(int iChange)
{
	setConscriptCount(getConscriptCount() + iChange);
}


int CvPlayer::getOverflowResearch() const
{
	return m_iOverflowResearch;
}


void CvPlayer::setOverflowResearch(int iNewValue)
{
	m_iOverflowResearch = iNewValue;
	FAssert(getOverflowResearch() >= 0);
}


void CvPlayer::changeOverflowResearch(int iChange)
{
	setOverflowResearch(getOverflowResearch() + iChange);
}

/*
** K-Mod, 27/dec/10, karadoc
** replaced NoUnhealthyPopulation with UnhealthyPopulationModifier
*/
/* original bts code
int CvPlayer::getNoUnhealthyPopulationCount() const
{
	return m_iNoUnhealthyPopulationCount;
}

bool CvPlayer::isNoUnhealthyPopulation() const																		
{
	return (getNoUnhealthyPopulationCount() > 0);
}

void CvPlayer::changeNoUnhealthyPopulationCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoUnhealthyPopulationCount = (m_iNoUnhealthyPopulationCount + iChange);
		FAssert(getNoUnhealthyPopulationCount() >= 0);

		AI_makeAssignWorkDirty();
	}
}*/

int CvPlayer::getUnhealthyPopulationModifier() const
{
	return m_iUnhealthyPopulationModifier;
}


void CvPlayer::changeUnhealthyPopulationModifier(int iChange)
{
	m_iUnhealthyPopulationModifier += iChange;
}
/*
** K-Mod end
*/

int CvPlayer::getExpInBorderModifier() const
{
	return m_iExpInBorderModifier;
}


void CvPlayer::changeExpInBorderModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iExpInBorderModifier += iChange;
		FAssert(getExpInBorderModifier() >= 0);
	}
}


int CvPlayer::getBuildingOnlyHealthyCount() const
{
	return m_iBuildingOnlyHealthyCount;
}


bool CvPlayer::isBuildingOnlyHealthy() const																	
{
	return (getBuildingOnlyHealthyCount() > 0);
}


void CvPlayer::changeBuildingOnlyHealthyCount(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingOnlyHealthyCount = (m_iBuildingOnlyHealthyCount + iChange);
		FAssert(getBuildingOnlyHealthyCount() >= 0);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getDistanceMaintenanceModifier() const
{
	return m_iDistanceMaintenanceModifier;
}


void CvPlayer::changeDistanceMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iDistanceMaintenanceModifier += iChange;

		updateMaintenance();
	}
}

// Civ4 Reimagined
int CvPlayer::getBonusValueModifier() const
{
	return m_iBonusValueModifier;
}

// Civ4 Reimagined
void CvPlayer::changeBonusValueModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iBonusValueModifier += iChange;
		updateBonusRatio();
	}
}

// Civ4 Reimagined
int CvPlayer::getVassalCommerceGain() const
{
	return m_iVassalCommerceGain;
}

// Civ4 Reimagined
void CvPlayer::changeVassalCommerceGain(int iChange)
{
	if (iChange != 0)
	{
		m_iVassalCommerceGain += iChange;
	}
}


// Civ4 Reimagined
bool CvPlayer::isNoPillage() const
{
	return (m_iIsNoPillageCount > 0);
}

// Civ4 Reimagined
void CvPlayer::changeIsNoPillageCount(int iChange)
{
	if (iChange != 0)
	{
		m_iIsNoPillageCount += iChange;
	}
}

int CvPlayer::getNumCitiesMaintenanceModifier() const
{
	return m_iNumCitiesMaintenanceModifier;
}


void CvPlayer::changeNumCitiesMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iNumCitiesMaintenanceModifier += iChange;

		updateMaintenance();
	}
}


// Civ4 Reimagined
int CvPlayer::getColonyMaintenanceModifier() const
{
	return m_iColonyMaintenanceModifier;
}


// Civ4 Reimagined
void CvPlayer::changeColonyMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iColonyMaintenanceModifier += iChange;

		updateMaintenance();
	}
}


// Civ4 Reimagined
void CvPlayer::changeNoCapitalCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoCapitalCount += iChange;
	}
}


// Civ4 Reimagined: A player with isNoCapital() still receives full benefits from the palace building, but does not benefit from capital traderoute bonus and other effects that specifically affect the capital city.
bool CvPlayer::isNoCapital() const
{
	return (m_iNoCapitalCount > 0);
}


// Civ4 Reimagined
int CvPlayer::getNoCapitalCount() const
{
	return m_iNoCapitalCount;
}


int CvPlayer::getCorporationMaintenanceModifier() const
{
	return m_iCorporationMaintenanceModifier;
}


void CvPlayer::changeCorporationMaintenanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iCorporationMaintenanceModifier += iChange;

		updateMaintenance();
	}
}

//Leoreth
int CvPlayer::getProcessModifier() const
{
	return m_iProcessModifier;
}

//Leoreth
void CvPlayer::changeProcessModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iProcessModifier += iChange;

		updateProductionToCommerceModifier();

		updateCommerce();

		AI_makeAssignWorkDirty();

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}

//Leoreth
void CvPlayer::updateProductionToCommerceModifier()
{
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			pLoopCity->changeProductionToCommerceModifier((CommerceTypes)iI, pLoopCity->getProductionToCommerceModifier((CommerceTypes)iI) * (100 + getProcessModifier()) / 100 - pLoopCity->getProductionToCommerceModifier((CommerceTypes)iI));
		}
	}
}


int CvPlayer::getTotalMaintenance() const
{
	return m_iTotalMaintenance / 100;
}


void CvPlayer::changeTotalMaintenance(int iChange)
{
	m_iTotalMaintenance += iChange;
	FAssert(m_iTotalMaintenance >= 0);
}


int CvPlayer::getUpkeepModifier() const
{
	return m_iUpkeepModifier;
}


void CvPlayer::changeUpkeepModifier(int iChange)
{
	m_iUpkeepModifier = (m_iUpkeepModifier + iChange);
}


int CvPlayer::getLevelExperienceModifier() const
{
	return m_iLevelExperienceModifier;
}

void CvPlayer::changeLevelExperienceModifier(int iChange)
{
	m_iLevelExperienceModifier += iChange;
}



int CvPlayer::getExtraHealth() const
{
	return m_iExtraHealth;
}


void CvPlayer::changeExtraHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHealth = (m_iExtraHealth + iChange);

		AI_makeAssignWorkDirty();
	}
}

//Leoreth
int CvPlayer::getPollutionModifier() const
{
	return m_iPollutionModifier;
}

//Leoreth
void CvPlayer::changePollutionModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iPollutionModifier += iChange;

		AI_makeAssignWorkDirty();
	}
}

//Civ4 Reimagined
int CvPlayer::getPopulationUnhappinessModifier() const
{
	return m_iPopulationUnhappinessModifier;
}

//Civ4 Reimagined
void CvPlayer::changePopulationUnhappinessModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iPopulationUnhappinessModifier += iChange;

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getBuildingGoodHealth() const	
{
	return m_iBuildingGoodHealth;
}


void CvPlayer::changeBuildingGoodHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingGoodHealth = (m_iBuildingGoodHealth + iChange);
		FAssert(getBuildingGoodHealth() >= 0);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getBuildingBadHealth() const	
{
	return m_iBuildingBadHealth;
}


void CvPlayer::changeBuildingBadHealth(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingBadHealth = (m_iBuildingBadHealth + iChange);
		FAssert(getBuildingBadHealth() <= 0);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getExtraHappiness() const
{
	return m_iExtraHappiness;
}


void CvPlayer::changeExtraHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iExtraHappiness = (m_iExtraHappiness + iChange);

		AI_makeAssignWorkDirty();
	}
}

// Civ4 Reimagined
int CvPlayer::getCivicHappiness() const
{
	return m_iCivicHappiness;
}

// Civ4 Reimagined
void CvPlayer::changeCivicHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iCivicHappiness = (m_iCivicHappiness + iChange);

		AI_makeAssignWorkDirty();
	}
}

// Civ4 Reimagined
int CvPlayer::getNoCapitalUnhappinessCount() const
{
	return m_iNoCapitalUnhappinessCount;
}

// Civ4 Reimagined
void CvPlayer::changeNoCapitalUnhappinessCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoCapitalUnhappinessCount = (m_iNoCapitalUnhappinessCount + iChange);
		FAssert(getNoCapitalUnhappinessCount() >= 0);
		
		AI_makeAssignWorkDirty();
	}
}

// Civ4 Reimagined
bool CvPlayer::isNoCapitalUnhappiness() const
{
	return (getNoCapitalUnhappinessCount() > 0);
}


int CvPlayer::getBuildingHappiness() const
{
	return m_iBuildingHappiness;
}


void CvPlayer::changeBuildingHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingHappiness = (m_iBuildingHappiness + iChange);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getLargestCityHappiness() const
{
	return m_iLargestCityHappiness;
}


void CvPlayer::changeLargestCityHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iLargestCityHappiness = (m_iLargestCityHappiness + iChange);

		AI_makeAssignWorkDirty();
	}
}

// Civ4 Reimagined
int CvPlayer::getGoldPerHappinessBonus() const
{
	return m_iGoldPerHappinessBonus;
}

// Civ4 Reimagined
void CvPlayer::changeGoldPerHappinessBonus(int iChange, bool bRecalculate)
{
	if (iChange != 0 || bRecalculate)
	{
		m_iGoldPerHappinessBonus = (m_iGoldPerHappinessBonus + iChange);

		CvCity* pCapitalCity = getCapitalCity();
		if (pCapitalCity != NULL)
		{
			pCapitalCity->updateGoldForHappinessBonus();
		}
		AI_makeAssignWorkDirty();
	}
}

// Civ4 Reimagined: Clears the bonus from the old capital and gives the bonus to the new capital.
void CvPlayer::recalculateGoldPerHappinessBonusForNewCapital()
{
	// Clear bonus from all cities
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->setGoldForHappinessBonus(0);
	}
	
	// Recalculate bonus for current capital
	changeGoldPerHappinessBonus(0, true);
}

int CvPlayer::getWarWearinessPercentAnger() const																 
{
	return m_iWarWearinessPercentAnger;
}


void CvPlayer::updateWarWearinessPercentAnger()
{
	int iNewWarWearinessPercentAnger;
	int iI;

	iNewWarWearinessPercentAnger = 0;

	if (!isBarbarian() && !isMinorCiv())
	{
		for (iI = 0; iI < MAX_CIV_TEAMS; iI++)
		{
			CvTeam& kTeam = GET_TEAM((TeamTypes)iI);
			if (kTeam.isAlive() && !kTeam.isMinorCiv())
			{
				if (kTeam.isAtWar(getTeam()))
				{
					//iNewWarWearinessPercentAnger += (GET_TEAM(getTeam()).getWarWeariness((TeamTypes)iI) * std::max(0, 100 + kTeam.getEnemyWarWearinessModifier())) / 10000;
					iNewWarWearinessPercentAnger += GET_TEAM(getTeam()).getWarWeariness((TeamTypes)iI, true) / 100; // K-Mod
				}
			}
		}
	}

	iNewWarWearinessPercentAnger = getModifiedWarWearinessPercentAnger(iNewWarWearinessPercentAnger);

	if (getWarWearinessPercentAnger() != iNewWarWearinessPercentAnger)
	{
		m_iWarWearinessPercentAnger = iNewWarWearinessPercentAnger;

		AI_makeAssignWorkDirty();
	}
}

int CvPlayer::getModifiedWarWearinessPercentAnger(int iWarWearinessPercentAnger) const			
{
	iWarWearinessPercentAnger *= GC.getDefineINT("BASE_WAR_WEARINESS_MULTIPLIER");

	if (GC.getGameINLINE().isOption(GAMEOPTION_ALWAYS_WAR) || GC.getGameINLINE().isOption(GAMEOPTION_NO_CHANGING_WAR_PEACE))
	{
		iWarWearinessPercentAnger *= std::max(0, (GC.getDefineINT("FORCED_WAR_WAR_WEARINESS_MODIFIER") + 100));
		iWarWearinessPercentAnger /= 100;
	}

	if (GC.getGameINLINE().isGameMultiPlayer())
	{
		iWarWearinessPercentAnger *= std::max(0, (GC.getDefineINT("MULTIPLAYER_WAR_WEARINESS_MODIFIER") + 100));
		iWarWearinessPercentAnger /= 100;
	}

	iWarWearinessPercentAnger *= std::max(0, (GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getWarWearinessModifier() + 100));
	iWarWearinessPercentAnger /= 100;

	if (!isHuman() && !isBarbarian() && !isMinorCiv())
	{
		iWarWearinessPercentAnger *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIWarWearinessPercent();
		iWarWearinessPercentAnger /= 100;

		iWarWearinessPercentAnger *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iWarWearinessPercentAnger /= 100;
	}

	return iWarWearinessPercentAnger;
}


int CvPlayer::getWarWearinessModifier() const			
{
	return m_iWarWearinessModifier;
}


void CvPlayer::changeWarWearinessModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iWarWearinessModifier = (m_iWarWearinessModifier + iChange);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getFreeSpecialist() const	
{
	return m_iFreeSpecialist;
}


void CvPlayer::changeFreeSpecialist(int iChange)
{
	if (iChange != 0)
	{
		m_iFreeSpecialist = (m_iFreeSpecialist + iChange);
		FAssert(getFreeSpecialist() >= 0);

		AI_makeAssignWorkDirty();
	}
}

//Leoreth
int CvPlayer::getCoreFreeSpecialist() const
{
	return m_iCoreFreeSpecialist;
}

//Leoreth
void CvPlayer::changeCoreFreeSpecialist(int iChange)
{
	if (iChange != 0)
	{
		m_iCoreFreeSpecialist += iChange;
		FAssert(getCoreFreeSpecialist() >= 0);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getNoForeignTradeCount() const
{
	return m_iNoForeignTradeCount;
}


bool CvPlayer::isNoForeignTrade() const
{
	return (getNoForeignTradeCount() > 0);
}


void CvPlayer::changeNoForeignTradeCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoForeignTradeCount = (m_iNoForeignTradeCount + iChange);
		FAssert(getNoForeignTradeCount() >= 0);

		GC.getGameINLINE().updateTradeRoutes();
	}
}

// Civ4 Reimagined
int CvPlayer::getDomesticTradeModifier() const
{
	return m_iDomesticTradeModifier;
}

// Civ4 Reimagined
void CvPlayer::changeDomesticTradeModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iDomesticTradeModifier = (m_iDomesticTradeModifier + iChange);
		
		updateTradeRoutes();
	}
}

// Civ4 Reimagined
int CvPlayer::getColonyTradeModifier() const
{
	return m_iColonyTradeModifier;
}


// Civ4 Reimagined
void CvPlayer::changeColonyTradeModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iColonyTradeModifier = (m_iColonyTradeModifier + iChange);
		
		updateTradeRoutes();
	}
}


// Civ4 Reimagined
int CvPlayer::getCapitalTradeModifier() const
{
	return m_iCapitalTradeModifier;
}


// Civ4 Reimagined
void CvPlayer::changeCapitalTradeModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iCapitalTradeModifier = (m_iCapitalTradeModifier + iChange);
		
		updateTradeRoutes();
	}
}


// Civ4 Reimagined
bool CvPlayer::isColony(PlayerTypes eMaster) const
{
	CvTeam& kTeam = GET_TEAM(GET_PLAYER(eMaster).getTeam());
	CvCity* pCapital = getCapitalCity();
	CvCity* pMasterCapital = GET_PLAYER(eMaster).getCapitalCity();
	
	if (kTeam.isAlive() && !kTeam.isMinorCiv())
	{
		if (GET_TEAM(getTeam()).isVassal(GET_PLAYER(eMaster).getTeam()))
		{
			if (pCapital && pMasterCapital)
			{
				if (getCapitalCity()->area() != GET_PLAYER(eMaster).getCapitalCity()->area())
				{
					return true;
				}
			}
		}
	}
	return false;
}

int CvPlayer::getNoCorporationsCount() const
{
	return m_iNoCorporationsCount;
}


bool CvPlayer::isNoCorporations() const
{
	return (getNoCorporationsCount() > 0);
}


void CvPlayer::changeNoCorporationsCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoCorporationsCount += iChange;
		FAssert(getNoCorporationsCount() >= 0);

		updateCorporation();
	}
}


int CvPlayer::getNoForeignCorporationsCount() const
{
	return m_iNoForeignCorporationsCount;
}


bool CvPlayer::isNoForeignCorporations() const
{
	return (getNoForeignCorporationsCount() > 0);
}


void CvPlayer::changeNoForeignCorporationsCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoForeignCorporationsCount += iChange;
		FAssert(getNoForeignCorporationsCount() >= 0);

		updateCorporation();
	}
}


int CvPlayer::getCoastalTradeRoutes() const
{
	return m_iCoastalTradeRoutes;
}


void CvPlayer::changeCoastalTradeRoutes(int iChange)
{
	if (iChange != 0)
	{
		m_iCoastalTradeRoutes = (m_iCoastalTradeRoutes + iChange);
		FAssert(getCoastalTradeRoutes() >= 0);

		updateTradeRoutes();
	}
}


int CvPlayer::getTradeRoutes() const
{
	return m_iTradeRoutes;
}


void CvPlayer::changeTradeRoutes(int iChange)
{
	if (iChange != 0)
	{
		m_iTradeRoutes = (m_iTradeRoutes + iChange);
		FAssert(getTradeRoutes() >= 0);

		updateTradeRoutes();
	}
}


int CvPlayer::getRevolutionTimer() const	
{
	return m_iRevolutionTimer;
}


void CvPlayer::setRevolutionTimer(int iNewValue)
{
	if (getRevolutionTimer() != iNewValue)
	{
		m_iRevolutionTimer = iNewValue;
		FAssert(getRevolutionTimer() >= 0);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
		}
	}
}


void CvPlayer::changeRevolutionTimer(int iChange)
{
	setRevolutionTimer(getRevolutionTimer() + iChange);
}


int CvPlayer::getConversionTimer() const	
{
	return m_iConversionTimer;
}


void CvPlayer::setConversionTimer(int iNewValue)
{
	if (getConversionTimer() != iNewValue)
	{
		m_iConversionTimer = iNewValue;
		FAssert(getConversionTimer() >= 0);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(MiscButtons_DIRTY_BIT, true);
		}
	}
}


void CvPlayer::changeConversionTimer(int iChange)
{
	setConversionTimer(getConversionTimer() + iChange);
}


int CvPlayer::getStateReligionCount() const																	
{
	return m_iStateReligionCount;
}


bool CvPlayer::isStateReligion() const	 
{
	return (getStateReligionCount() > 0);
}


void CvPlayer::changeStateReligionCount(int iChange)
{
	if (iChange != 0)
	{
		// religion visibility now part of espionage
		//GC.getGameINLINE().updateCitySight(false, true);

		m_iStateReligionCount = (m_iStateReligionCount + iChange);
		FAssert(getStateReligionCount() >= 0);

		// religion visibility now part of espionage
		//GC.getGameINLINE().updateCitySight(true, true);

		updateMaintenance();
		updateReligionHappiness();
		updateReligionCommerce();
		updateBuildingCommerce();
		updateStateReligionTempleCache();

		GC.getGameINLINE().AI_makeAssignWorkDirty();

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


int CvPlayer::getNoNonStateReligionSpreadCount() const
{
	return m_iNoNonStateReligionSpreadCount;
}


bool CvPlayer::isNoNonStateReligionSpread() const
{
	return (getNoNonStateReligionSpreadCount() > 0);
}


void CvPlayer::changeNoNonStateReligionSpreadCount(int iChange)
{
	m_iNoNonStateReligionSpreadCount = (m_iNoNonStateReligionSpreadCount + iChange);
	FAssert(getNoNonStateReligionSpreadCount() >= 0);
}


int CvPlayer::getStateReligionHappiness() const								 
{
	return m_iStateReligionHappiness;
}


void CvPlayer::changeStateReligionHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iStateReligionHappiness = (m_iStateReligionHappiness + iChange);

		updateReligionHappiness();
	}
}


int CvPlayer::getNonStateReligionHappiness() const 
{
	return m_iNonStateReligionHappiness;
}


void CvPlayer::changeNonStateReligionHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iNonStateReligionHappiness = (m_iNonStateReligionHappiness + iChange);

		updateReligionHappiness();
	}
}


int CvPlayer::getStateReligionUnitProductionModifier() const	 
{
	return m_iStateReligionUnitProductionModifier;
}


void CvPlayer::changeStateReligionUnitProductionModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iStateReligionUnitProductionModifier = (m_iStateReligionUnitProductionModifier + iChange);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


// Civ4 Reimagined
int CvPlayer::getVoteSourceStateReligionUnitProductionModifier() const	 
{
	return m_iVoteSourceStateReligionUnitProductionModifier;
}


// Civ4 Reimagined
void CvPlayer::changeVoteSourceStateReligionUnitProductionModifier(int iChange)
{
	if (iChange != 0)
	{
		for (int iVoteSource = 0; iVoteSource < GC.getNumVoteSourceInfos(); ++iVoteSource)
		{
			processVoteSourceBonus((VoteSourceTypes)iVoteSource, false);
		}

		m_iVoteSourceStateReligionUnitProductionModifier = (m_iVoteSourceStateReligionUnitProductionModifier + iChange);

		for (int iVoteSource = 0; iVoteSource < GC.getNumVoteSourceInfos(); ++iVoteSource)
		{
			processVoteSourceBonus((VoteSourceTypes)iVoteSource, true);
		}

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getStateReligionBuildingProductionModifier() const
{
	return m_iStateReligionBuildingProductionModifier;
}


void CvPlayer::changeStateReligionBuildingProductionModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iStateReligionBuildingProductionModifier = (m_iStateReligionBuildingProductionModifier + iChange);

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getStateReligionFreeExperience() const
{
	return m_iStateReligionFreeExperience;
}


void CvPlayer::changeStateReligionFreeExperience(int iChange)
{
	m_iStateReligionFreeExperience = (m_iStateReligionFreeExperience + iChange);
}


//Leoreth
int CvPlayer::getSpecialistExtraYieldBaseThreshold() const
{
	return m_iSpecialistExtraYieldBaseThreshold;
}

//Leoreth
void CvPlayer::changeSpecialistExtraYieldBaseThreshold(int iChange)
{
	m_iSpecialistExtraYieldBaseThreshold = (m_iSpecialistExtraYieldBaseThreshold + iChange);
}

//Leoreth
int CvPlayer::getSpecialistExtraYieldEraThreshold() const
{
	return m_iSpecialistExtraYieldEraThreshold;
}

//Leoreth
void CvPlayer::changeSpecialistExtraYieldEraThreshold(int iChange)
{
	m_iSpecialistExtraYieldEraThreshold = (m_iSpecialistExtraYieldEraThreshold + iChange);
}


CvCity* CvPlayer::getCapitalCity() const	 
{
	return getCity(m_iCapitalCityID);
}	
	
void CvPlayer::setCapitalCity(CvCity* pNewCapitalCity)
{
	CvCity* pOldCapitalCity;
	bool bUpdatePlotGroups;

	pOldCapitalCity = getCapitalCity();
	
	if (pOldCapitalCity != pNewCapitalCity)
	{
		/*
		// Civ4 Reimagined
		if (isHasExtraAvailableBonuses())
		{
			for (int iI = 0; iI < GC.getNumBonusInfos(); iI++)
			{
				if (pOldCapitalCity != NULL)
				{
					pOldCapitalCity->changeNumBonuses((BonusTypes)iI, -getPlayerExtraAvailableBonuses((BonusTypes)iI));
				}
				if (pNewCapitalCity != NULL)
				{
					pNewCapitalCity->changeNumBonuses((BonusTypes)iI, getPlayerExtraAvailableBonuses((BonusTypes)iI));
				}
			}
		}
		*/
		
		bUpdatePlotGroups = ((pOldCapitalCity == NULL) || (pNewCapitalCity == NULL) || (pOldCapitalCity->plot()->getOwnerPlotGroup() != pNewCapitalCity->plot()->getOwnerPlotGroup()));

		if (bUpdatePlotGroups)
		{
			if (pOldCapitalCity != NULL)
			{
				pOldCapitalCity->plot()->updatePlotGroupBonus(false);
			}
			if (pNewCapitalCity != NULL)
			{
				pNewCapitalCity->plot()->updatePlotGroupBonus(false);
			}
		}

		if (pNewCapitalCity != NULL)
		{
			m_iCapitalCityID = pNewCapitalCity->getID();
		}
		else
		{
			m_iCapitalCityID = FFreeList::INVALID_INDEX;
		}

		if (bUpdatePlotGroups)
		{
			if (pOldCapitalCity != NULL)
			{
				pOldCapitalCity->plot()->updatePlotGroupBonus(true);
			}
			if (pNewCapitalCity != NULL)
			{
				pNewCapitalCity->plot()->updatePlotGroupBonus(true);
			}
		}

		updateMaintenance();
		updateTradeRoutes();
		// Civ4 Reimagined
		updateColonialCities();
		
		recalculateGoldPerHappinessBonusForNewCapital(); // Civ4 Reimagined

		if (pOldCapitalCity != NULL)
		{
			pOldCapitalCity->updateCommerce();

			pOldCapitalCity->setInfoDirty(true);
		}
		if (pNewCapitalCity != NULL)
		{
			pNewCapitalCity->updateCommerce();

			pNewCapitalCity->setInfoDirty(true);
		}
	}
}


int CvPlayer::getCitiesLost() const
{
	return m_iCitiesLost;
}


void CvPlayer::changeCitiesLost(int iChange)
{
	m_iCitiesLost = (m_iCitiesLost + iChange);
}


int CvPlayer::getWinsVsBarbs() const		 
{
	return m_iWinsVsBarbs;
}


void CvPlayer::changeWinsVsBarbs(int iChange)
{
	m_iWinsVsBarbs = (m_iWinsVsBarbs + iChange);
	FAssert(getWinsVsBarbs() >= 0);
}


int CvPlayer::getAssets() const
{
	return m_iAssets;
}


void CvPlayer::changeAssets(int iChange)
{
	m_iAssets = (m_iAssets + iChange);
	FAssert(getAssets() >= 0);
}


int CvPlayer::getPower() const
{
	if (isHuman()) // Civ4 Reimagined. AI should underestimate humans less.
	{
		int humanPower = m_iPower * 6;
		humanPower /= 5;
		return humanPower;
	}
	return m_iPower;
}


void CvPlayer::changePower(int iChange)
{
	m_iPower = (m_iPower + iChange);
	FAssert(getPower() >= 0);
}


int CvPlayer::getMilitaryPower(DomainTypes eDomain) const
{
	return m_aiMilitaryPower[eDomain];
}


void CvPlayer::changeMilitaryPower(DomainTypes eDomain, int iChange)
{
	m_aiMilitaryPower[eDomain] = (m_aiMilitaryPower[eDomain] + iChange);
	FAssert(getMilitaryPower(eDomain) >= 0);
}


int CvPlayer::getBestUnitPower(DomainTypes eDomain) const
{
	return m_aiBestUnitPower[eDomain];
}


void CvPlayer::setBestUnitPower(DomainTypes eDomain, int iPower)
{
	m_aiBestUnitPower[eDomain] = iPower;
	FAssert(getBestUnitPower(eDomain) >= 0);
}


int CvPlayer::getExtraMovesInGoldenAge(DomainTypes eDomain) const
{
	return m_aiExtraMovesInGoldenAge[eDomain];
}


void CvPlayer::changeExtraMovesInGoldenAge(DomainTypes eDomain, int iChange)
{
	m_aiExtraMovesInGoldenAge[eDomain] = (m_aiExtraMovesInGoldenAge[eDomain] + iChange);
	FAssert(getExtraMovesInGoldenAge(eDomain) >= 0);
}


// Civ4 Reimagined
int CvPlayer::getDomainGreatGeneralRateModifier(DomainTypes eDomain) const
{
	return m_aiDomainGreatGeneralRateModifier[eDomain];
}


// Civ4 Reimagined
void CvPlayer::changeDomainGreatGeneralRateModifier(DomainTypes eDomain, int iChange)
{
	m_aiDomainGreatGeneralRateModifier[eDomain] = (m_aiDomainGreatGeneralRateModifier[eDomain] + iChange);
}


int CvPlayer::getPopScore(bool bCheckVassal) const		 
{
	if (bCheckVassal && GET_TEAM(getTeam()).isAVassal())
	{
		return m_iPopulationScore / 2;
	}

	int iVassalScore = 0;

	if (bCheckVassal)
	{
		for (int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			if (i != getID())
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)i);
				if (kLoopPlayer.isAlive() && GET_TEAM(kLoopPlayer.getTeam()).isVassal(getTeam()))
				{
					iVassalScore += kLoopPlayer.getPopScore(false) / 2;
				}
			}
		}
	}

	return (m_iPopulationScore + iVassalScore / std::max(1, GET_TEAM(getTeam()).getNumMembers()));
}

void CvPlayer::changePopScore(int iChange)
{
	if (iChange != 0)
	{
		m_iPopulationScore += iChange;
		FAssert(getPopScore() >= 0);

		GC.getGameINLINE().setScoreDirty(true);
	}
}


int CvPlayer::getLandScore(bool bCheckVassal) const		 
{
	if (bCheckVassal && GET_TEAM(getTeam()).isAVassal())
	{
		return m_iLandScore / 2;
	}

	int iVassalScore = 0;

	if (bCheckVassal)
	{
		for (int i = 0; i < MAX_CIV_PLAYERS; i++)
		{
			if (i != getID())
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)i);
				if (kLoopPlayer.isAlive() && GET_TEAM(kLoopPlayer.getTeam()).isVassal(getTeam()))
				{
					iVassalScore += kLoopPlayer.getLandScore(false) / 2;
				}
			}
		}
	}

	return (m_iLandScore + iVassalScore  / std::max(1, GET_TEAM(getTeam()).getNumMembers()));
}


void CvPlayer::changeLandScore(int iChange)
{
	if (iChange != 0)
	{
		m_iLandScore += iChange;
		FAssert(getLandScore() >= 0);

		GC.getGameINLINE().setScoreDirty(true);
	}
}


int CvPlayer::getWondersScore() const		 
{
	return m_iWondersScore;
}


void CvPlayer::changeWondersScore(int iChange)
{
	if (iChange != 0)
	{
		m_iWondersScore += iChange;
		FAssert(getWondersScore() >= 0);

		GC.getGameINLINE().setScoreDirty(true);
	}
}


int CvPlayer::getTechScore() const		 
{
	return m_iTechScore;
}


void CvPlayer::changeTechScore(int iChange)
{
	if (iChange != 0)
	{
		m_iTechScore += iChange;
		FAssert(getTechScore() >= 0);

		GC.getGameINLINE().setScoreDirty(true);
	}
}

int CvPlayer::getCombatExperience() const
{
	return m_iCombatExperience;
}

void CvPlayer::setCombatExperience(int iExperience)
{
	FAssert(iExperience >= 0);

	if (iExperience != getCombatExperience())
	{
		m_iCombatExperience = iExperience;

		if (!isBarbarian())
		{
			int iExperienceThreshold = greatPeopleThreshold(true);
			if (m_iCombatExperience >= iExperienceThreshold && iExperienceThreshold > 0)
			{
				// create great person
				/*
				CvCity* ppBestCity = NULL;
				int ipBestValue = MAX_INT;
				int iLoop;
				for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
				{
					int iValue = 4 * GC.getGameINLINE().getSorenRandNum(getNumCities(), "Warlord City Selection");

					for (int i = 0; i < NUM_YIELD_TYPES; i++)
					{
						iValue += pLoopCity->findYieldRateRank((YieldTypes)i);
					}
					iValue += pLoopCity->findPopulationRank();

					if (iValue < ipBestValue)
					{
						ppBestCity = pLoopCity;
						ipBestValue = iValue;
					}
				}
				*/
				//Civ4 Reimagined: Great general always spawns in capital
				CvCity* ppBestCity = getCapitalCity();

				if (ppBestCity)
				{
					int iRandOffset = GC.getGameINLINE().getSorenRandNum(GC.getNumUnitInfos(), "Warlord Unit Generation");
					for (int iI = 0; iI < GC.getNumUnitInfos(); iI++)
					{
						UnitTypes eLoopUnit = (UnitTypes)((iI + iRandOffset) % GC.getNumUnitInfos());
						if (GC.getUnitInfo(eLoopUnit).getLeaderExperience() > 0 || GC.getUnitInfo(eLoopUnit).getLeaderPromotion() != NO_PROMOTION)
						{
							ppBestCity->createGreatPeople(eLoopUnit, false, true);
							setCombatExperience(getCombatExperience() - iExperienceThreshold);
							break;
						}
					}
				}
			}
		}
	}
}

void CvPlayer::changeCombatExperience(int iChange)
{
	setCombatExperience(getCombatExperience() + iChange);
}


bool CvPlayer::isConnected() const
{
	return gDLL->isConnected( getNetID() );
}

int CvPlayer::getNetID() const
{
	return GC.getInitCore().getNetID(getID());
}


void CvPlayer::setNetID(int iNetID)
{
	GC.getInitCore().setNetID(getID(), iNetID);
}

void CvPlayer::sendReminder()
{
	CvWString szYearStr;

	// Only perform this step if we have a valid email address on record,
	// and we have provided information about how to send emails
	if ( !getPbemEmailAddress().empty() && 
		!gDLL->GetPitbossSmtpHost().empty() )
	{
		GAMETEXT.setTimeStr(szYearStr, GC.getGameINLINE().getGameTurn(), true);

		// Generate our arguments
		CyArgsList argsList;
		argsList.add(getPbemEmailAddress());
		argsList.add(gDLL->GetPitbossSmtpHost());
		argsList.add(gDLL->GetPitbossSmtpLogin());
		argsList.add(gDLL->GetPitbossSmtpPassword());
		argsList.add(GC.getGameINLINE().getName());
		argsList.add(GC.getGameINLINE().isMPOption(MPOPTION_TURN_TIMER));
		argsList.add(GC.getGameINLINE().getPitbossTurnTime());
		argsList.add(gDLL->GetPitbossEmail());
		argsList.add(szYearStr);

		// Now send our email via Python
		long iResult;
		bool bOK = gDLL->getPythonIFace()->callFunction(PYPitBossModule, "sendEmail", argsList.makeFunctionArgs(), &iResult);

		FAssertMsg( bOK, "Pitboss Python call to onSendEmail failed!" );
		FAssertMsg( iResult == 0, "Pitboss Python fn onSendEmail encountered an error" );
	}
}

uint CvPlayer::getStartTime() const
{
	return m_uiStartTime;
}


void CvPlayer::setStartTime(uint uiStartTime)
{
	m_uiStartTime = uiStartTime;
}


uint CvPlayer::getTotalTimePlayed() const
{
	return ((timeGetTime() - m_uiStartTime)/1000);
}


bool CvPlayer::isMinorCiv() const
{
	return GC.getInitCore().getMinorNationCiv(m_eID);
}


bool CvPlayer::isAlive() const
{
	return m_bAlive;
}


bool CvPlayer::isEverAlive() const
{
	return m_bEverAlive;
}


void CvPlayer::setAlive(bool bNewValue)
{
	CvWString szBuffer;
	int iI;

	if (isAlive() != bNewValue)
	{
		m_bAlive = bNewValue;

		GET_TEAM(getTeam()).changeAliveCount((isAlive()) ? 1 : -1);

		// Report event to Python
		CvEventReporter::getInstance().setPlayerAlive(getID(), bNewValue);

		if (isAlive())
		{
			if (!isEverAlive())
			{
				m_bEverAlive = true;

				GET_TEAM(getTeam()).changeEverAliveCount(1);
			}

			if (getNumCities() == 0)
			{
				setFoundedFirstCity(false);
			}

			updatePlotGroups();

			if (GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS) || (GC.getGameINLINE().getNumGameTurnActive() == 0) || (GC.getGameINLINE().isSimultaneousTeamTurns() && GET_TEAM(getTeam()).isTurnActive()))
			{
				setTurnActive(true);
			}

			gDLL->openSlot(getID());

			// K-Mod. Attitude cache
			for (PlayerTypes i = (PlayerTypes)0; i < MAX_PLAYERS; i=(PlayerTypes)(i+1))
			{
				GET_PLAYER(i).AI_updateAttitudeCache(getID());
				GET_PLAYER(getID()).AI_updateAttitudeCache(i);
			}
			// K-Mod end
		}
		else
		{
			clearResearchQueue();
			killUnits();
			killCities();
			killAllDeals();

			// Civ4 Reimagined
			GC.getGameINLINE().updateIdeologyCount();

			setTurnActive(false);

			gDLL->endMPDiplomacy();
			gDLL->endDiplomacy();

			if (!isHuman())
			{
				gDLL->closeSlot(getID());
			}

			if (GC.getGameINLINE().getElapsedGameTurns() > 0)
			{
				if (!isBarbarian())
				{
					szBuffer = gDLL->getText("TXT_KEY_MISC_CIV_DESTROYED", getCivilizationAdjectiveKey());

					for (iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							gDLL->getInterfaceIFace()->addHumanMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVDESTROYED", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
						}
					}

					GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));
				}
			}
		}

		GC.getGameINLINE().setScoreDirty(true);
	}
}


void CvPlayer::verifyAlive()
{
	bool bKill;

	if (isAlive())
	{
		bKill = false;

		if (!bKill)
		{
			if (!isBarbarian())
			{
				if (getNumCities() == 0 && getAdvancedStartPoints() < 0)
				{
					if ((getNumUnits() == 0) || (!(GC.getGameINLINE().isOption(GAMEOPTION_COMPLETE_KILLS)) && isFoundedFirstCity()))
					{
						bKill = true;
					}
				}
			}
		}

		if (!bKill)
		{
			if (!isBarbarian())
			{
				if (GC.getGameINLINE().getMaxCityElimination() > 0)
				{
					if (getCitiesLost() >= GC.getGameINLINE().getMaxCityElimination())
					{
						bKill = true;
					}
				}
			}
		}

		if (bKill)
		{
			setAlive(false);
		}
	}
	else
	{
		if ((getNumCities() > 0) || (getNumUnits() > 0))
		{
			setAlive(true);
		}
	}
}


bool CvPlayer::isTurnActive() const																			
{
	return m_bTurnActive;
}

void CvPlayer::setTurnActiveForPbem(bool bActive)
{
	FAssertMsg(GC.getGameINLINE().isPbem(), "You are using setTurnActiveForPbem. Are you sure you know what you're doing?");

	// does nothing more than to set the member variable before saving the game
	// the rest of the turn will be performed upon loading the game
	// This allows the player to browse the game in paused mode after he has generated the save
	if (isTurnActive() != bActive)
	{
		m_bTurnActive = bActive;
		GC.getGameINLINE().changeNumGameTurnActive(isTurnActive() ? 1 : -1);

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/21/09                                jdog5000      */
/*                                                                                              */
/* Efficiency                                                                                   */
/************************************************************************************************/
		// Plot danger cache
		//if( GC.getGameINLINE().getNumGameTurnActive() != 1 )
		{
			GC.getMapINLINE().invalidateActivePlayerSafeRangeCache();
		}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	}
}

// This structure of this function has been changed for K-Mod.
void CvPlayer::setTurnActive(bool bNewValue, bool bDoTurn)
{
	if (isTurnActive() == bNewValue)
		return;

	m_bTurnActive = bNewValue;

	if (isTurnActive())
	{
		// K-Mod
		GET_PLAYER(getID()).updateCacheData();
		onTurnLogging(); // bbai logging
		// K-Mod end

		if (GC.getLogging())
		{
			if (gDLL->getChtLvl() > 0)
			{
				TCHAR szOut[1024];
				sprintf(szOut, "Player %d Turn ON\n", getID());
				gDLL->messageControlLog(szOut);
			}
		}

		FAssertMsg(isAlive(), "isAlive is expected to be true");

		setEndTurn(false);
		GC.getGameINLINE().resetTurnTimer();

		// If we are the Pitboss, send this player an email
		if ( gDLL->IsPitbossHost() )
		{
			// If this guy is not currently connected, try sending him an email
			if ( isHuman() && !isConnected() )
			{
				sendReminder();
			}
		}

		if ((GC.getGameINLINE().isHotSeat() || GC.getGameINLINE().isPbem()) && isHuman() && bDoTurn)
		{
			gDLL->getInterfaceIFace()->clearEventMessages();
			gDLL->getEngineIFace()->setResourceLayer(false);

			GC.getGameINLINE().setActivePlayer(getID());
		}

		GC.getGameINLINE().changeNumGameTurnActive(1);

		if (bDoTurn)
		{
			if (isAlive() && !isHuman() && !isBarbarian() && (getAdvancedStartPoints() >= 0))
			{
				AI_doAdvancedStart();
			}
					
			if (GC.getGameINLINE().getElapsedGameTurns() > 0)
			{
				if (isAlive())
				{
					if (GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
					{
						doTurn();
					}
					// K-Mod. Call CvTeam::doTurn at the start of this team's turn. ie. when the leader's turn is activated.
					// Note: in simultaneous turns mode this is called by CvGame::doTurn,
					//       because from here we can't tell which player in each team will be activated first.
					else
					{
						if (GET_TEAM(getTeam()).getLeaderID() == getID())
							GET_TEAM(getTeam()).doTurn();
					}
					// K-Mod end

					doTurnUnits();
				}
			}

			if ((getID() == GC.getGameINLINE().getActivePlayer()) && (GC.getGameINLINE().getElapsedGameTurns() > 0))
			{
				if (GC.getGameINLINE().isNetworkMultiPlayer())
				{
					gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_TURN_BEGINS").GetCString(), "AS2D_NEWTURN", MESSAGE_TYPE_DISPLAY_ONLY);
				}
				else
				{
					gDLL->getInterfaceIFace()->playGeneralSound("AS2D_NEWTURN");
				}
			}

			doWarnings();
		}

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			if (gDLL->getInterfaceIFace()->getLengthSelectionList() == 0)
			{
				GC.getGameINLINE().cycleSelectionGroups_delayed(1, false);
			}

			gDLL->getInterfaceIFace()->setDirty(SelectionCamera_DIRTY_BIT, true);
		}
	}
	else
	{
		if (GC.getLogging())
		{
			if (gDLL->getChtLvl() > 0)
			{
				TCHAR szOut[1024];
				sprintf(szOut, "Player %d Turn OFF\n", getID());
				gDLL->messageControlLog(szOut);
			}
		}

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setForcePopup(false);
			gDLL->getInterfaceIFace()->clearQueuedPopups();
			gDLL->getInterfaceIFace()->flushTalkingHeadMessages();
		}

		// start profiling DLL if desired
		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			startProfilingDLL(true);
		}

		GC.getGameINLINE().changeNumGameTurnActive(-1);

		if (bDoTurn)
		{
			if (!GC.getGameINLINE().isMPOption(MPOPTION_SIMULTANEOUS_TURNS))
			{
				if (isAlive())
				{
					doTurn();
				}

				if ((GC.getGameINLINE().isPbem() || GC.getGameINLINE().isHotSeat()) && isHuman() && GC.getGameINLINE().countHumanPlayersAlive() > 1)
				{
					GC.getGameINLINE().setHotPbemBetweenTurns(true);
				}

				if (GC.getGameINLINE().isSimultaneousTeamTurns())
				{
					if (!GET_TEAM(getTeam()).isTurnActive())
					{
						for (int iI = (getTeam() + 1); iI < MAX_TEAMS; iI++)
						{
							if (GET_TEAM((TeamTypes)iI).isAlive())
							{
								GET_TEAM((TeamTypes)iI).setTurnActive(true);
								break;
							}
						}
					}
				}
				else
				{
					for (int iI = (getID() + 1); iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							if (GC.getGameINLINE().isPbem() && GET_PLAYER((PlayerTypes)iI).isHuman())
							{
								if (!GC.getGameINLINE().getPbemTurnSent())
								{
									gDLL->sendPbemTurn((PlayerTypes)iI);
								}
							}
							else
							{
								GET_PLAYER((PlayerTypes)iI).setTurnActive(true);
							}
							break;
						}
					}
				}
			}
		}
	}

	gDLL->getInterfaceIFace()->updateCursorType();

	gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);

	GC.getMapINLINE().invalidateActivePlayerSafeRangeCache();
}

// K-Mod. The body of this function use to be part of setTurnActive.
// I've moved it here just to improve the readability of that function.
// (This logging is from bbai.)
void CvPlayer::onTurnLogging() const
{
	if( gPlayerLogLevel > 0 ) 
	{
		logBBAI("Player %d (%S) setTurnActive for turn %d (%d %s)", getID(), getCivilizationDescription(0), GC.getGameINLINE().getGameTurn(), std::abs(GC.getGameINLINE().getGameTurnYear()), GC.getGameINLINE().getGameTurnYear()>0 ? "AD" : "BC");

		// Civ4 Reimagined
		if( (GC.getGameINLINE().getGameTurn() == 360) && isBarbarian() )
		{
			logBBAI("----- STATISTICS -----");
			logBBAI("----- Corporations -----");
			
			for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
			{
				if (GC.getGameINLINE().isCorporationFounded((CorporationTypes)iI))
				{
					int iTurn = GC.getGameINLINE().getCorporationGameTurnFounded((CorporationTypes)iI);
					logBBAI("%c founded in turn %d (%d %s)", GC.getCorporationInfo((CorporationTypes)iI).getChar(), iTurn, std::abs(GC.getGameINLINE().getTurnYear(iTurn)), GC.getGameINLINE().getTurnYear(iTurn)>0 ? "AD" : "BC");
				}
				else
					logBBAI("%c not founded yet", GC.getCorporationInfo((CorporationTypes)iI).getChar());
			}
			
			logBBAI("----- Religions -----");
			
			for (int iI = 0; iI < GC.getNumReligionInfos(); iI++)
			{
				if (GC.getGameINLINE().isReligionFounded((ReligionTypes)iI))
				{
					int iTurn = GC.getGameINLINE().getReligionGameTurnFounded((ReligionTypes)iI);
					logBBAI("%c founded in turn %d (%d %s)", GC.getReligionInfo((ReligionTypes)iI).getChar(), iTurn, std::abs(GC.getGameINLINE().getTurnYear(iTurn)), GC.getGameINLINE().getTurnYear(iTurn)>0 ? "AD" : "BC");
					logBBAI("%c spread: %d", GC.getReligionInfo((ReligionTypes)iI).getChar(), GC.getGameINLINE().calculateReligionPercent((ReligionTypes)iI));
				}
				else
					logBBAI("%c not founded yet", GC.getReligionInfo((ReligionTypes)iI).getChar());
			}
		}
		
		
		if( GC.getGameINLINE().getGameTurn() > 0 && (GC.getGameINLINE().getGameTurn() % 25) == 0 && !isBarbarian() )
		{
			CvWStringBuffer szBuffer;
			GAMETEXT.setScoreHelp(szBuffer, getID());
			logBBAI("%S", szBuffer);

			int iGameTurn = GC.getGameINLINE().getGameTurn();
			logBBAI("  Total Score: %d, Population Score: %d (%d total pop), Land Score: %d, Tech Score: %d, Wonder Score: %d", calculateScore(), getPopScore(false), getTotalPopulation(), getLandScore(false), getTechScore(), getWondersScore());

			int iEconomy = 0;
			int iProduction = 0;
			int iAgri = 0;
			int iCount = 0;
			for( int iI = 1; iI <= 5; iI++ )
			{
				if( iGameTurn - iI >= 0 )
				{
					iEconomy += getEconomyHistory(iGameTurn - iI);
					iProduction += getIndustryHistory(iGameTurn - iI);
					iAgri += getAgricultureHistory(iGameTurn - iI);
					iCount++;
				}
			}
			iEconomy /= std::max(1, iCount);
			iProduction /= std::max(1, iCount);
			iAgri /= std::max(1, iCount);

			logBBAI("  Economy avg: %d,  Industry avg: %d,  Agriculture avg: %d", iEconomy, iProduction, iAgri);
		}
	}

	if( gPlayerLogLevel >= 2 )
	{
		CvWStringBuffer szBuffer;

		logBBAI("    Player %d (%S) has %d cities, %d pop, %d power, %d tech percent", getID(), getCivilizationDescription(0), getNumCities(), getTotalPopulation(), getPower(), GET_TEAM(getTeam()).getBestKnownTechScorePercent());

		if( GET_PLAYER(getID()).AI_isFinancialTrouble() )
		{
			logBBAI("    Financial trouble!");
		}

		szBuffer.append(CvWString::format(L"    Team %d has met: ", getTeam()));

		for( int iI = 0; iI < MAX_CIV_TEAMS; iI++ )
		{
			if( iI != getTeam() && GET_TEAM(getTeam()).isHasMet((TeamTypes)iI) )
			{
				if( GET_TEAM((TeamTypes)iI).isAlive() )
				{
					szBuffer.append(CvWString::format(L"%d,", iI));
				}
			}
		}

		if( GET_TEAM(getTeam()).getVassalCount() > 0 )
		{
			szBuffer.append(CvWString::format(L";  vassals: "));

			for( int iI = 0; iI < MAX_CIV_TEAMS; iI++ )
			{
				if( iI != getTeam() && GET_TEAM((TeamTypes)iI).isVassal(getTeam()) )
				{
					if( GET_TEAM((TeamTypes)iI).isAlive() )
					{
						szBuffer.append(CvWString::format(L"%d,", iI));
					}
				}
			}
		}

		if( GET_TEAM(getTeam()).getAtWarCount(false) > 0 )
		{
			szBuffer.append(CvWString::format(L";  at war with: "));

			for( int iI = 0; iI < MAX_CIV_TEAMS; iI++ )
			{
				if( iI != getTeam() && GET_TEAM(getTeam()).isAtWar((TeamTypes)iI) )
				{
					if( GET_TEAM((TeamTypes)iI).isAlive() )
					{
						szBuffer.append(CvWString::format(L"%d,", iI));
					}
				}
			}
		}

		if( GET_TEAM(getTeam()).getAnyWarPlanCount(true) > 0 )
		{
			szBuffer.append(CvWString::format(L";  planning war with: "));

			for( int iI = 0; iI < MAX_CIV_TEAMS; iI++ )
			{
				if( iI != getTeam() && !GET_TEAM(getTeam()).isAtWar((TeamTypes)iI) && GET_TEAM(getTeam()).AI_getWarPlan((TeamTypes)iI) != NO_WARPLAN )
				{
					if( GET_TEAM((TeamTypes)iI).isAlive() )
					{
						szBuffer.append(CvWString::format(L"%d,", iI));
					}
				}
			}
		}

		logBBAI("%S", szBuffer.getCString());

		szBuffer.clear();

		if( GET_TEAM(getTeam()).getAnyWarPlanCount(true) > 0 ) logBBAI("    Enemy power perc: %d (%d with others reduction)", GET_TEAM(getTeam()).AI_getEnemyPowerPercent(), GET_TEAM(getTeam()).AI_getEnemyPowerPercent(true));
	}
}
// K-Mod end

bool CvPlayer::isAutoMoves() const																			
{
	return m_bAutoMoves;
}


void CvPlayer::setAutoMoves(bool bNewValue)
{
	if (isAutoMoves() != bNewValue)
	{
		m_bAutoMoves = bNewValue;

		if (!isAutoMoves())
		{
			if (isEndTurn() || !isHuman())
			{
				setTurnActive(false);
			}
			else
			{
				if (getID() == GC.getGameINLINE().getActivePlayer())
				{
					gDLL->getInterfaceIFace()->setCycleSelectionCounter(1);
					//GC.getGameINLINE().cycleSelectionGroups_delayed(1, false, true); // this is a subtle case. I think it's best to just use the normal delay
				}
			}
		}
	}
}


bool CvPlayer::isEndTurn() const																			
{
	return m_bEndTurn;
}


void CvPlayer::setEndTurn(bool bNewValue)
{
	if (isEndTurn() != bNewValue)
	{
		FAssertMsg(isTurnActive(), "isTurnActive is expected to be true");

		m_bEndTurn = bNewValue;

		if (isEndTurn())
		{
			setAutoMoves(true);
		}
	}
}

bool CvPlayer::isTurnDone() const																			
{
	// if this returns true, popups and diplomacy will wait to appear until next turn
	if (!GC.getGameINLINE().isPbem() && !GC.getGameINLINE().isHotSeat())
	{
		return false;
	}
	if (!isHuman() )
	{
		return true;
	}
	if (!isEndTurn())
	{
		return false;
	}
	return (!isAutoMoves());
}

bool CvPlayer::isExtendedGame() const
{
	return m_bExtendedGame;
}


void CvPlayer::makeExtendedGame()
{
	m_bExtendedGame = true;
}


bool CvPlayer::isFoundedFirstCity() const
{
	return m_bFoundedFirstCity;
}


void CvPlayer::setFoundedFirstCity(bool bNewValue)
{
	if (isFoundedFirstCity() != bNewValue)
	{
		m_bFoundedFirstCity = bNewValue;

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(PercentButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
		}
	}
}


bool CvPlayer::isStrike() const																			
{
	return m_bStrike;
}


void CvPlayer::setStrike(bool bNewValue)
{
	if (isStrike() != bNewValue)
	{
		m_bStrike = bNewValue;

		if (isStrike())
		{
			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->addHumanMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_MISC_UNITS_ON_STRIKE").GetCString(), "AS2D_STRIKE", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WARNING_TEXT"));

				gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
			}
		}
	}
}

// Civ4 Reimagined
bool CvPlayer::hasNukes() const
{
	BuildingTypes eBuilding = (BuildingTypes)GC.getInfoTypeForString("BUILDING_NUCLEAR_RESEARCH_CENTER");
	return (countNumBuildings(eBuilding) > 0);
}

PlayerTypes CvPlayer::getID() const
{
	return m_eID;
}


HandicapTypes CvPlayer::getHandicapType() const																
{
	return GC.getInitCore().getHandicap(getID());
}


CivilizationTypes CvPlayer::getCivilizationType() const														
{
	return GC.getInitCore().getCiv(getID());
}


LeaderHeadTypes CvPlayer::getLeaderType() const
{
	return GC.getInitCore().getLeader(getID());
}


LeaderHeadTypes CvPlayer::getPersonalityType() const
{
	return m_ePersonalityType;
}


void CvPlayer::setPersonalityType(LeaderHeadTypes eNewValue)
{
	m_ePersonalityType = eNewValue;
}


EraTypes CvPlayer::getCurrentEra() const
{
	return m_eCurrentEra;
}



void CvPlayer::setCurrentEra(EraTypes eNewValue)
{
	CvCity* pLoopCity;
	CvUnit* pLoopUnit;
	CvPlot* pLoopPlot;
	int iLoop;
	int iI;
	
	if (getCurrentEra() != eNewValue)
	{

		EraTypes eOldEra = m_eCurrentEra;
		m_eCurrentEra = eNewValue;

		// Civ4 Reimagined
		if (eOldEra < eNewValue)
		{
			int iEra = (int)eOldEra;

			do {
				iEra++;
				updateUniquePowers((EraTypes)iEra);
			} while (iEra < (int)eNewValue);
		}
		
		if (GC.getGameINLINE().getActiveTeam() != NO_TEAM)
		{
			for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
			{
				pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
				pLoopPlot->updateGraphicEra();

				if (pLoopPlot->getRevealedImprovementType(GC.getGameINLINE().getActiveTeam(), true) != NO_IMPROVEMENT)
				{
					if ((pLoopPlot->getOwnerINLINE() == getID()) || (!(pLoopPlot->isOwned()) && (getID() == GC.getGameINLINE().getActivePlayer())))
					{
						pLoopPlot->setLayoutDirty(true);
					}
				}
			}
		}
		
		
		// dirty all of this player's cities... and check for military happiness. 
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			//if (pLoopCity->getOwnerINLINE() == getID())
			FAssert(pLoopCity->getOwnerINLINE() == getID()); // K-Mod
			{
				pLoopCity->setLayoutDirty(true);
				pLoopCity->changeMilitaryHappinessUnits(-pLoopCity->getMilitaryHappinessUnits()); //Civ4 Reimagined
				pLoopCity->changeMilitaryHappinessUnits(pLoopCity->plot()->plotCount(PUF_isMilitaryHappiness)); //Civ4 Reimagined
				pLoopCity->AI_setAssignWorkDirty(true);
			}
		}

		//update unit eras
		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			gDLL->getEntityIFace()->updateGraphicEra(pLoopUnit->getUnitEntity(), eOldEra);
		}

		//update flag eras
		gDLL->getInterfaceIFace()->setDirty(Flag_DIRTY_BIT, true);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Soundtrack_DIRTY_BIT, true);
		}

		if (isHuman() && (getCurrentEra() != GC.getGameINLINE().getStartEra()) && !GC.getGameINLINE().isNetworkMultiPlayer())
		{
			if (GC.getGameINLINE().isFinalInitialized() && !(gDLL->GetWorldBuilderMode()))
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
				if (NULL != pInfo)
				{
					pInfo->setData1(eNewValue);
					pInfo->setText(L"showEraMovie");
					addPopup(pInfo);
				}
			}
		}
	}
}


ReligionTypes CvPlayer::getLastStateReligion() const														
{
	return m_eLastStateReligion;
}


ReligionTypes CvPlayer::getStateReligion() const														
{
	return ((isStateReligion()) ? getLastStateReligion() : NO_RELIGION);
}


void CvPlayer::setLastStateReligion(ReligionTypes eNewValue)
{
	ReligionTypes eOldReligion;
	CvWString szBuffer;
	int iI;

	if (getLastStateReligion() != eNewValue)
	{
		// religion visibility now part of espionage
		//GC.getGameINLINE().updateCitySight(false, true);

		eOldReligion = getLastStateReligion();
		m_eLastStateReligion = eNewValue;

		// religion visibility now part of espionage
		//GC.getGameINLINE().updateCitySight(true, true);

		updateMaintenance();
		updateReligionHappiness();
		updateReligionCommerce();
		updateBuildingCommerce();

		GC.getGameINLINE().updateSecretaryGeneral();

		GC.getGameINLINE().AI_makeAssignWorkDirty();

		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MUSIC")))
		{
			if (getLastStateReligion() != NO_RELIGION)
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MUSIC"), getID(), true);
			}
		}

		if (GC.getGameINLINE().isFinalInitialized())
		{
			if (gDLL->isDiplomacy() && (gDLL->getDiplomacyPlayer() == getID()))
			{
				gDLL->updateDiplomacyAttitude(true);
			}

			if (!isBarbarian())
			{
				if (getLastStateReligion() != NO_RELIGION)
				{
					for (iI = 0; iI < MAX_PLAYERS; iI++)
					{
						if (GET_PLAYER((PlayerTypes)iI).isAlive())
						{
							// Civ4 Reimagined
							GC.getGameINLINE().checkWarPeaceEurekas();

							if (GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
							{
								szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_CONVERT_RELIGION", getNameKey(), GC.getReligionInfo(getLastStateReligion()).getTextKeyWide());
								gDLL->getInterfaceIFace()->addHumanMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_RELIGION_CONVERT", MESSAGE_TYPE_MAJOR_EVENT);
							}
						}
					}

					szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_CONVERT_RELIGION", getNameKey(), GC.getReligionInfo(getLastStateReligion()).getTextKeyWide());
					GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer);

					// Civ4 Reimagined
					if (eOldReligion == NO_RELIGION)
					{
						convertClassicalTemples(getLastStateReligion());
					}
				}
			}

			// Python Event
			CvEventReporter::getInstance().playerChangeStateReligion(getID(), eNewValue, eOldReligion);
			// K-Mod. Attitude cache
			for (int iI = 0; iI < GC.getMAX_PLAYERS(); iI++)
			{
				if (GET_PLAYER((PlayerTypes)iI).isAlive() && GET_PLAYER((PlayerTypes)iI).getStateReligion() != NO_RELIGION)
				{
					GET_PLAYER(getID()).AI_updateAttitudeCache((PlayerTypes)iI);
					GET_PLAYER((PlayerTypes)iI).AI_updateAttitudeCache(getID());
				}
			}
			// K-Mod end
		}
	}
}

PlayerTypes CvPlayer::getParent() const
{
	return m_eParent;
}

void CvPlayer::setParent(PlayerTypes eParent)
{
	m_eParent = eParent;
}

TeamTypes CvPlayer::getTeam() const
{
	return m_eTeamType;
}

void CvPlayer::updateTeamType()
{
	if(getID() == NO_PLAYER)
	{
		m_eTeamType = NO_TEAM;
	}
	else
	{
		m_eTeamType = GC.getInitCore().getTeam(getID());
	}
}

void CvPlayer::setTeam(TeamTypes eTeam)
{
	FAssert(eTeam != NO_TEAM);
	FAssert(getTeam() != NO_TEAM);

	GET_TEAM(getTeam()).changeNumMembers(-1);
	if (isAlive())
	{
		GET_TEAM(getTeam()).changeAliveCount(-1);
	}
	if (isEverAlive())
	{
		GET_TEAM(getTeam()).changeEverAliveCount(-1);
	}
	GET_TEAM(getTeam()).changeNumCities(-(getNumCities()));
	GET_TEAM(getTeam()).changeTotalPopulation(-(getTotalPopulation()));
	GET_TEAM(getTeam()).changeTotalLand(-(getTotalLand()));

	GC.getInitCore().setTeam(getID(), eTeam);

	GET_TEAM(getTeam()).changeNumMembers(1);
	if (isAlive())
	{
		GET_TEAM(getTeam()).changeAliveCount(1);
	}
	if (isEverAlive())
	{
		GET_TEAM(getTeam()).changeEverAliveCount(1);
	}
	GET_TEAM(getTeam()).changeNumCities(getNumCities());
	GET_TEAM(getTeam()).changeTotalPopulation(getTotalPopulation());
	GET_TEAM(getTeam()).changeTotalLand(getTotalLand());

	// K-Mod Attitude cache
	if (GC.getGameINLINE().isFinalInitialized())
	{
		for (PlayerTypes i = (PlayerTypes)0; i < MAX_PLAYERS; i=(PlayerTypes)(i+1))
		{
			GET_PLAYER(getID()).AI_updateAttitudeCache(i);
			GET_PLAYER(i).AI_updateAttitudeCache(getID());
		}
	}
	// K-Mod end
}


PlayerColorTypes CvPlayer::getPlayerColor() const
{
	//return GC.getInitCore().getColor(getID());
	// K-Mod. Conceal the player colour of unmet players.
	if (!concealUnknownCivs() || GET_TEAM(GC.getGameINLINE().getActiveTeam()).isHasSeen(getTeam()))
		return GC.getInitCore().getColor(getID());
	else
		return GC.getInitCore().getColor(BARBARIAN_PLAYER);
	// K-Mod end
}


int CvPlayer::getPlayerTextColorR() const
{
	FAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	return ((int)(GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(getPlayerColor()).getTextColorType()).getColor().r * 255));
}


int CvPlayer::getPlayerTextColorG() const
{
	FAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	return ((int)(GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(getPlayerColor()).getTextColorType()).getColor().g * 255));
}


int CvPlayer::getPlayerTextColorB() const
{
	FAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	return ((int)(GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(getPlayerColor()).getTextColorType()).getColor().b * 255));
}


int CvPlayer::getPlayerTextColorA() const
{
	FAssertMsg(getPlayerColor() != NO_PLAYERCOLOR, "getPlayerColor() is not expected to be equal with NO_PLAYERCOLOR");
	return ((int)(GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(getPlayerColor()).getTextColorType()).getColor().a * 255));
}


int CvPlayer::getSeaPlotYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiSeaPlotYield[eIndex];
}


void CvPlayer::changeSeaPlotYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiSeaPlotYield[eIndex] = (m_aiSeaPlotYield[eIndex] + iChange);

		updateYield();
	}
}


// Civ4 Reimagined
int CvPlayer::getLakePlotYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiLakePlotYield[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeLakePlotYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiLakePlotYield[eIndex] = (m_aiLakePlotYield[eIndex] + iChange);

		updateYield();
	}
}


int CvPlayer::getPeakYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiPeakYield[eIndex];
}


void CvPlayer::changePeakYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiPeakYield[eIndex] = (m_aiPeakYield[eIndex] + iChange);

		updateYield();
	}
}


// Civ4 Reimagined
int CvPlayer::getPeakYieldChangeAdjacentToTerrace(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiPeakYieldChangeAdjacentToTerrace[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changePeakYieldChangeAdjacentToTerrace(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiPeakYieldChangeAdjacentToTerrace[eIndex] = (m_aiPeakYieldChangeAdjacentToTerrace[eIndex] + iChange);

		updateYield();
	}
}


int CvPlayer::getYieldRateModifier(YieldTypes eIndex) const		 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiYieldRateModifier[eIndex];
}


void CvPlayer::changeYieldRateModifier(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiYieldRateModifier[eIndex] = (m_aiYieldRateModifier[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);
		
		if (eIndex == YIELD_COMMERCE)
		{
			updateCommerce();
		}

		AI_makeAssignWorkDirty();

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


// Civ4 Reimagined
int CvPlayer::getUnitClassProductionModifier(UnitClassTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitClassProductionModifier[eIndex];
}

// Civ4 Reimagined
void CvPlayer::changeUnitClassProductionModifier(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	
	if (iChange != 0)
	{
		m_paiUnitClassProductionModifier[eIndex] = (m_paiUnitClassProductionModifier[eIndex] + iChange);
	}
}

// Civ4 Reimagined
int CvPlayer::getReligiousUnitClassProductionModifier(UnitClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiReligiousUnitClassProductionModifier[eIndex];
}

// Civ4 Reimagined
void CvPlayer::changeReligiousUnitClassProductionModifier(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	
	if (iChange != 0)
	{
		m_paiReligiousUnitClassProductionModifier[eIndex] = (m_paiReligiousUnitClassProductionModifier[eIndex] + iChange);
	}
}

// Civ4 Reimagined
int CvPlayer::getGreatMerchantPointsPerTrade() const
{
	return m_iGreatMerchantPointsPerTrade;
}

// Civ4 Reimagined
void CvPlayer::changeGreatMerchantPointsPerTrade(int iChange)
{
	if (iChange != 0)
	{
		m_iGreatMerchantPointsPerTrade += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getGreatPeoplePointsPerTrade() const
{
	return m_iGreatPeoplePointsPerTrade;
}

// Civ4 Reimagined
void CvPlayer::changeGreatPeoplePointsPerTrade(int iChange)
{
	if (iChange != 0)
	{
		m_iGreatPeoplePointsPerTrade += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getCapitalCultureAttitudeBonus() const
{
	return m_iCapitalCultureAttitudeBonus;
}

// Civ4 Reimagined
void CvPlayer::changeCapitalCultureAttitudeBonus(int iChange)
{
	if (iChange != 0)
	{
		m_iCapitalCultureAttitudeBonus += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getExtraYield(YieldTypes eIndex) const		 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiExtraYield[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeExtraYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiExtraYield[eIndex] = (m_aiExtraYield[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);
		
		if (eIndex == YIELD_COMMERCE)
		{
			updateCommerce();
		}
		// Civ4 Reimagined
		else if (eIndex == YIELD_PRODUCTION)
		{
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
				{
					if (pLoopCity->getProductionToCommerceModifier((CommerceTypes)iCommerce) > 0)
					{
						pLoopCity->updateCommerce((CommerceTypes)iCommerce);
					}
				}
			}
		}

		AI_makeAssignWorkDirty();

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


// Civ4 Reimagined
int CvPlayer::getPeakAdjacencyExtraYield(YieldTypes eIndex) const		 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiPeakAdjacencyExtraYield[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changePeakAdjacencyExtraYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiPeakAdjacencyExtraYield[eIndex] = (m_aiPeakAdjacencyExtraYield[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);
		
		if (eIndex == YIELD_COMMERCE)
		{
			updateCommerce();
		}
		// Civ4 Reimagined
		else if (eIndex == YIELD_PRODUCTION)
		{
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
				{
					if (pLoopCity->getProductionToCommerceModifier((CommerceTypes)iCommerce) > 0)
					{
						pLoopCity->updateCommerce((CommerceTypes)iCommerce);
					}
				}
			}
		}

		AI_makeAssignWorkDirty();

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


// Civ4 Reimagined
int CvPlayer::getCityOnHillsExtraYield(YieldTypes eIndex) const		 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCityOnHillsExtraYield[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeCityOnHillsExtraYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCityOnHillsExtraYield[eIndex] = (m_aiCityOnHillsExtraYield[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);
		
		if (eIndex == YIELD_COMMERCE)
		{
			updateCommerce();
		}
		// Civ4 Reimagined
		else if (eIndex == YIELD_PRODUCTION)
		{
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
				{
					if (pLoopCity->getProductionToCommerceModifier((CommerceTypes)iCommerce) > 0)
					{
						pLoopCity->updateCommerce((CommerceTypes)iCommerce);
					}
				}
			}
		}

		AI_makeAssignWorkDirty();

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


// Civ4 Reimagined
int CvPlayer::getTownAdjacencyBonus(YieldTypes eIndex) const		 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiTownAdjacencyBonus[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeTownAdjacencyBonus(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiTownAdjacencyBonus[eIndex] = (m_aiTownAdjacencyBonus[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);
		
		if (eIndex == YIELD_COMMERCE)
		{
			updateCommerce();
		}
		// Civ4 Reimagined
		else if (eIndex == YIELD_PRODUCTION)
		{
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
				{
					if (pLoopCity->getProductionToCommerceModifier((CommerceTypes)iCommerce) > 0)
					{
						pLoopCity->updateCommerce((CommerceTypes)iCommerce);
					}
				}
			}
		}

		AI_makeAssignWorkDirty();

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


// Civ4 Reimagined
int CvPlayer::getCapitalExtraYieldFromCityPercent(YieldTypes eIndex) const		 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCapitalExtraYieldFromCityPercent[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeCapitalExtraYieldFromCityPercent(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCapitalExtraYieldFromCityPercent[eIndex] = (m_aiCapitalExtraYieldFromCityPercent[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);
		
		CvCity* pCapitalCity = getCapitalCity();
		if (pCapitalCity != NULL && eIndex == YIELD_COMMERCE)
		{
			pCapitalCity->updateCommerce();
		}
		else if (eIndex == YIELD_PRODUCTION)
		{
			for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
			{
				if (pCapitalCity->getProductionToCommerceModifier((CommerceTypes)iCommerce) > 0)
				{
					pCapitalCity->updateCommerce((CommerceTypes)iCommerce);
				}
			}
		}

		AI_makeAssignWorkDirty();

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


// Civ4 Reimagined
int CvPlayer::getExtraYieldPerReligion(YieldTypes eIndex) const		 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiExtraYieldPerReligion[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeExtraYieldPerReligion(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiExtraYieldPerReligion[eIndex] = (m_aiExtraYieldPerReligion[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);

		if (eIndex == YIELD_COMMERCE || eIndex == YIELD_PRODUCTION)
		{
			updateCommerce();
		}

		AI_makeAssignWorkDirty();

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getCapitalYieldRateModifier(YieldTypes eIndex) const		 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCapitalYieldRateModifier[eIndex];
}


void CvPlayer::changeCapitalYieldRateModifier(YieldTypes eIndex, int iChange)
{
	CvCity* pCapitalCity;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCapitalYieldRateModifier[eIndex] = (m_aiCapitalYieldRateModifier[eIndex] + iChange);

		invalidateYieldRankCache(eIndex);

		pCapitalCity = getCapitalCity();

		if (pCapitalCity != NULL)
		{
			if (eIndex == YIELD_COMMERCE)
			{
				pCapitalCity->updateCommerce();
			}
			// Civ4 Reimagined
			else if (eIndex == YIELD_PRODUCTION)
			{
				for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
				{
					if (pCapitalCity->getProductionToCommerceModifier((CommerceTypes)iCommerce) > 0)
					{
						pCapitalCity->updateCommerce((CommerceTypes)iCommerce);
					}
				}
			}

			pCapitalCity->AI_setAssignWorkDirty(true);

			if (pCapitalCity->getTeam() == GC.getGameINLINE().getActiveTeam())
			{
				pCapitalCity->setInfoDirty(true);
			}
		}
	}
}


int CvPlayer::getExtraYieldThreshold(YieldTypes eIndex) const	
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiExtraYieldThreshold[eIndex];
}


void CvPlayer::updateExtraYieldThreshold(YieldTypes eIndex)
{
	int ipBestValue;
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	ipBestValue = 0;

	FAssertMsg((GC.getNumTraitInfos() > 0), "GC.getNumTraitInfos() is less than or equal to zero but is expected to be larger than zero in CvPlayer::updateExtraYieldThreshold");
	for (iI = 0; iI < GC.getNumTraitInfos(); iI++)
	{
		if (hasTrait((TraitTypes)iI))
		{
			if (GC.getTraitInfo((TraitTypes) iI).getExtraYieldThreshold(eIndex) > 0)
			{
				if (((TechTypes)GC.getTraitInfo((TraitTypes) iI).getPrereqTech()) == NO_TECH || GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getTraitInfo((TraitTypes) iI).getPrereqTech()) || isEnableFinancial()) // Civ4 Reimagined
				{
					if ((ipBestValue == 0) || (GC.getTraitInfo((TraitTypes) iI).getExtraYieldThreshold(eIndex) < ipBestValue))
					{
						ipBestValue = GC.getTraitInfo((TraitTypes) iI).getExtraYieldThreshold(eIndex);
					}
				}
			}
		}
	}

	if (getExtraYieldThreshold(eIndex) != ipBestValue)
	{
		m_aiExtraYieldThreshold[eIndex] = ipBestValue;
		FAssert(getExtraYieldThreshold(eIndex) >= 0);

		updateYield();
	}
}


int CvPlayer::getTradeYieldModifier(YieldTypes eIndex) const	
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiTradeYieldModifier[eIndex];
}


void CvPlayer::changeTradeYieldModifier(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiTradeYieldModifier[eIndex] = (m_aiTradeYieldModifier[eIndex] + iChange);

		updateTradeRoutes();
	}
}


int CvPlayer::getFreeCityCommerce(CommerceTypes eIndex) const												
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiFreeCityCommerce[eIndex];
}


void CvPlayer::changeFreeCityCommerce(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiFreeCityCommerce[eIndex] = (m_aiFreeCityCommerce[eIndex] + iChange);
		FAssert(getFreeCityCommerce(eIndex) >= 0);

		updateCommerce(eIndex);
	}
}

// Civ4 Reimagined
int CvPlayer::getInitCityCommerce(CommerceTypes eIndex) const												
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiInitCityCommerce[eIndex];
}

// Civ4 Reimagined
void CvPlayer::changeInitCityCommerce(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiInitCityCommerce[eIndex] = (m_aiInitCityCommerce[eIndex] + iChange);
		FAssert(getInitCityCommerce(eIndex) >= 0);

		updateCommerce(eIndex);
	}
}

int CvPlayer::getCommercePercent(CommerceTypes eIndex) const												
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCommercePercent[eIndex];
}

// K-Mod. This function has been rewritten to enforce the rules of flexible / inflexible commerce types.
// (not all changes marked)
bool CvPlayer::setCommercePercent(CommerceTypes eIndex, int iNewValue, bool bForce)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!bForce && !isCommerceFlexible(eIndex))
	{
		FAssertMsg(false, "setCommercePercent called without permission.");
		return false; // can't change percent
	}

	int iOldValue = getCommercePercent(eIndex);

	iNewValue = range(iNewValue, 0, 100);

	if (iOldValue == iNewValue)
		return false;

	m_aiCommercePercent[eIndex] = iNewValue;

	int iTotalCommercePercent = 0;

	for (CommerceTypes i = (CommerceTypes)0; i < NUM_COMMERCE_TYPES; i = (CommerceTypes)(i+1))
	{
		iTotalCommercePercent += getCommercePercent(i);
	}

	for (CommerceTypes i = (CommerceTypes)0; iTotalCommercePercent != 100 && i < NUM_COMMERCE_TYPES; i = (CommerceTypes)(i+1))
	{
		if (i != eIndex && isCommerceFlexible(i))
		{
			FAssert(bForce || isCommerceFlexible(i));
			int iAdjustment = std::min(m_aiCommercePercent[i], iTotalCommercePercent - 100);
			m_aiCommercePercent[i] -= iAdjustment;
			iTotalCommercePercent -= iAdjustment;
		}
	}
	// if we couldn't balance the books, we need to do a second pass, with fewer restrictions
	if (iTotalCommercePercent != 100)
	{
		for (CommerceTypes i = (CommerceTypes)0; iTotalCommercePercent != 100 && i < NUM_COMMERCE_TYPES; i = (CommerceTypes)(i+1))
		{
			if (bForce ? i != eIndex : isCommerceFlexible(i))
			{
				FAssert(bForce || isCommerceFlexible(i));
				int iAdjustment = std::min(m_aiCommercePercent[i], iTotalCommercePercent - 100);
				m_aiCommercePercent[i] -= iAdjustment;
				iTotalCommercePercent -= iAdjustment;
			}
		}
	}
	FAssert(100 == iTotalCommercePercent);

	if (iOldValue == getCommercePercent(eIndex))
		return false;

	updateCommerce();
	// K-Mod. For human players, update commerce weight immediately so that they can see effects on working plots, etc.
	if (isHuman() && isTurnActive())
		GET_PLAYER(getID()).AI_updateCommerceWeights();
	// K-Mod end

	AI_makeAssignWorkDirty();

	/* original bts code
	if (getTeam() == GC.getGameINLINE().getActiveTeam())
	{
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(CityScreen_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Financial_Screen_DIRTY_BIT, true);
	} */
	// K-Mod
	if (getTeam() == GC.getGameINLINE().getActiveTeam())
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true); // research turns left?

	if (getID() == GC.getGameINLINE().getActivePlayer())
	{
		gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Financial_Screen_DIRTY_BIT, true);
	}
	// K-Mod end

	return true;
}


bool CvPlayer::changeCommercePercent(CommerceTypes eIndex, int iChange)
{
	return setCommercePercent(eIndex, (getCommercePercent(eIndex) + iChange));
}


int CvPlayer::getCommerceRate(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	int iRate = m_aiCommerceRate[eIndex];
	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		if (eIndex == COMMERCE_CULTURE)
		{
			iRate += m_aiCommerceRate[COMMERCE_ESPIONAGE];
		}
		else if (eIndex == COMMERCE_ESPIONAGE)
		{
			iRate = 0;
		}
	}
	
	// Civ4 Reimagined
	if (getVassalCommerceGain() != 0)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
			
			if (kLoopPlayer.isAlive() && GET_TEAM(kLoopPlayer.getTeam()).isVassal(getTeam()))
			{
				iRate += getVassalCommerceGain() * kLoopPlayer.getCommerceRate(eIndex);
			}
		}
	}

	return iRate / 100;  
}


void CvPlayer::changeCommerceRate(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCommerceRate[eIndex] += iChange;
		FAssert(getCommerceRate(eIndex) >= 0);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}

int CvPlayer::getCommerceRateModifier(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCommerceRateModifier[eIndex];
}


void CvPlayer::changeCommerceRateModifier(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCommerceRateModifier[eIndex] = (m_aiCommerceRateModifier[eIndex] + iChange);

		updateCommerce(eIndex);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getCapitalCommerceRateModifier(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCapitalCommerceRateModifier[eIndex];
}


void CvPlayer::changeCapitalCommerceRateModifier(CommerceTypes eIndex, int iChange)
{
	CvCity* pCapitalCity;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCapitalCommerceRateModifier[eIndex] = (m_aiCapitalCommerceRateModifier[eIndex] + iChange);

		pCapitalCity = getCapitalCity();

		if (pCapitalCity != NULL)
		{
			pCapitalCity->updateCommerce();

			pCapitalCity->AI_setAssignWorkDirty(true);
		}
	}
}

// Civ4 Reimagined: Grants the specified commerce modifier in capital for each surplus happiness in capital (i.e. 12 happiness and 7 anger with 2% gold per surplus happiness grants a (12-7)*2 = 10% gold modifier in capital). Has no effect if the city is unhappy. How often the effect applies is capped at the population in capital (in the example above, a population of 5 would be required for the full 10% gold modifier benefit, with each population less reducing the effect by 2%).
int CvPlayer::getCapitalCommerceRateModifierPerHappinessSurplus(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCapitalCommerceRateModifierPerHappinessSurplus[eIndex];
}

// Civ4 Reimagined
void CvPlayer::changeCapitalCommerceRateModifierPerHappinessSurplus(CommerceTypes eIndex, int iChange)
{
	CvCity* pCapitalCity;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCapitalCommerceRateModifierPerHappinessSurplus[eIndex] = (m_aiCapitalCommerceRateModifierPerHappinessSurplus[eIndex] + iChange);

		pCapitalCity = getCapitalCity();

		if (pCapitalCity != NULL)
		{
			pCapitalCity->updateCommerce(eIndex);

			pCapitalCity->AI_setAssignWorkDirty(true);
		}
	}
}

// Civ4 Reimagined
int CvPlayer::getCommerceHappiness(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCommerceHappiness[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeCommerceHappiness(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCommerceHappiness[eIndex] = (m_aiCommerceHappiness[eIndex] + iChange);
		FAssert(getCommerceHappiness(eIndex) >= 0);
		
		CvCity* pLoopCity;
		int iLoop;

		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->changeCommerceHappinessPer((eIndex), iChange);
		}
	}
}


int CvPlayer::getStateReligionBuildingCommerce(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiStateReligionBuildingCommerce[eIndex];
}


void CvPlayer::changeStateReligionBuildingCommerce(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiStateReligionBuildingCommerce[eIndex] = (m_aiStateReligionBuildingCommerce[eIndex] + iChange);
		FAssert(getStateReligionBuildingCommerce(eIndex) >= 0);

		updateCommerce(eIndex);
	}
}


// Civ4 Reimagined
int CvPlayer::getStateReligionCommercePerPopulationOverThreshold(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiStateReligionCommercePerPopulationOverThreshold[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeStateReligionCommercePerPopulationOverThreshold(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiStateReligionCommercePerPopulationOverThreshold[eIndex] = (m_aiStateReligionCommercePerPopulationOverThreshold[eIndex] + iChange);
		FAssert(getStateReligionCommercePerPopulationOverThreshold(eIndex) >= 0);

		updateCommerce(eIndex);
	}
}


int CvPlayer::getSpecialistExtraCommerce(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiSpecialistExtraCommerce[eIndex];
}


void CvPlayer::changeSpecialistExtraCommerce(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiSpecialistExtraCommerce[eIndex] = (m_aiSpecialistExtraCommerce[eIndex] + iChange);
		FAssert(getSpecialistExtraCommerce(eIndex) >= 0);

		updateCommerce(eIndex);

		AI_makeAssignWorkDirty();
	}
}


// Civ4 Reimagined
int CvPlayer::getGreatPeopleExtraCommerceInCapital(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiGreatPeopleExtraCommerceInCapital[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeGreatPeopleExtraCommerceInCapital(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiGreatPeopleExtraCommerceInCapital[eIndex] = (m_aiGreatPeopleExtraCommerceInCapital[eIndex] + iChange);
		FAssert(getGreatPeopleExtraCommerceInCapital(eIndex) >= 0);

		updateCommerce(eIndex);

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getCommerceFlexibleCount(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiCommerceFlexibleCount[eIndex];
}


bool CvPlayer::isCommerceFlexible(CommerceTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	/* if (!isFoundedFirstCity())
	{
		return false;
	}

	if (eIndex == COMMERCE_ESPIONAGE)
	{
		if (0 == GET_TEAM(getTeam()).getHasMetCivCount(true) || GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
		{
			return false;
		}
	} */ // Disabled by K-Mod. (I don't want to enforce these conditions in such a fundamental way.)

	return (GC.getCommerceInfo(eIndex).isFlexiblePercent() || (getCommerceFlexibleCount(eIndex) > 0) || GET_TEAM(getTeam()).isCommerceFlexible(eIndex));
}


void CvPlayer::changeCommerceFlexibleCount(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiCommerceFlexibleCount[eIndex] = (m_aiCommerceFlexibleCount[eIndex] + iChange);
		FAssert(getCommerceFlexibleCount(eIndex) >= 0);

		if (!isCommerceFlexible(eIndex))
		{
			//setCommercePercent(eIndex, 0);
			setCommercePercent(eIndex, GC.getCommerceInfo(eIndex).getInitialPercent(), true); // K-Mod
		}

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(PercentButtons_DIRTY_BIT, true);
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getGoldPerTurnByPlayer(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiGoldPerTurnByPlayer[eIndex];
}


void CvPlayer::changeGoldPerTurnByPlayer(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_iGoldPerTurn = (m_iGoldPerTurn + iChange);
		m_aiGoldPerTurnByPlayer[eIndex] = (m_aiGoldPerTurnByPlayer[eIndex] + iChange);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}

		if (!isHuman())
		{
			AI_doCommerce();
		}
	}
}


bool CvPlayer::isFeatAccomplished(FeatTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_FEAT_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abFeatAccomplished[eIndex];
}


void CvPlayer::setFeatAccomplished(FeatTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_FEAT_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_abFeatAccomplished[eIndex] = bNewValue;
}


bool CvPlayer::isOption(PlayerOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_PLAYEROPTION_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abOptions[eIndex];
}


void CvPlayer::setOption(PlayerOptionTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_PLAYEROPTION_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_abOptions[eIndex] = bNewValue;
}

// Civ4 Reimagined
bool CvPlayer::isNativeAmericanBonus(NativeBonusTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_NATIVEBONUS_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_abNativeAmericanBonus[eIndex];
}

// Civ4 Reimagined
void CvPlayer::setNativeAmericanBonus(NativeBonusTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_NATIVEBONUS_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	m_abNativeAmericanBonus[eIndex] = bNewValue;
}

bool CvPlayer::isPlayable() const
{
	return GC.getInitCore().getPlayableCiv(getID());
}

void CvPlayer::setPlayable(bool bNewValue)
{
	GC.getInitCore().setPlayableCiv(getID(), bNewValue);
}


int CvPlayer::getBonusExport(BonusTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBonusExport[eIndex];
}


void CvPlayer::changeBonusExport(BonusTypes eIndex, int iChange)
{
	CvCity* pCapitalCity;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		pCapitalCity = getCapitalCity();
		if (pCapitalCity != NULL)
		{
			pCapitalCity->plot()->updatePlotGroupBonus(false);
		}
		m_paiBonusExport[eIndex] = (m_paiBonusExport[eIndex] + iChange);
		FAssert(getBonusExport(eIndex) >= 0);
		if (pCapitalCity != NULL)
		{
			pCapitalCity->plot()->updatePlotGroupBonus(true);
		}
	}
}


int CvPlayer::getBonusImport(BonusTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBonusImport[eIndex];
}


void CvPlayer::changeBonusImport(BonusTypes eIndex, int iChange)
{
	CvCity* pCapitalCity;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		pCapitalCity = getCapitalCity();
		if (pCapitalCity != NULL)
		{
			pCapitalCity->plot()->updatePlotGroupBonus(false);
		}
		m_paiBonusImport[eIndex] = (m_paiBonusImport[eIndex] + iChange);
		FAssert(getBonusImport(eIndex) >= 0);
		if (pCapitalCity != NULL)
		{
			pCapitalCity->plot()->updatePlotGroupBonus(true);
		}
	}
}


int CvPlayer::getImprovementCount(ImprovementTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiImprovementCount[eIndex];
}


void CvPlayer::changeImprovementCount(ImprovementTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiImprovementCount[eIndex] = (m_paiImprovementCount[eIndex] + iChange);
	FAssert(getImprovementCount(eIndex) >= 0);

	checkImprovementEurekas();
}


int CvPlayer::getRadiusImprovementHappiness(ImprovementTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiRadiusImprovementHappiness[eIndex];
}


void CvPlayer::changeRadiusImprovementHappiness(ImprovementTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiRadiusImprovementHappiness[eIndex] = (m_paiRadiusImprovementHappiness[eIndex] + iChange);

	int iLoop;
	if (iChange != 0)
	{
		for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->updateFeatureHappiness();
		}
	}
}


// Civ4 Reimagined
int CvPlayer::getGreatSpyPointsFromImprovementInRadius(ImprovementTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiGreatSpyPointsFromImprovementInRadius[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeGreatSpyPointsFromImprovementInRadius(ImprovementTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumImprovementInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiGreatSpyPointsFromImprovementInRadius[eIndex] = (m_paiGreatSpyPointsFromImprovementInRadius[eIndex] + iChange);
}


int CvPlayer::getFreeBuildingCount(BuildingTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiFreeBuildingCount[eIndex];
}


bool CvPlayer::isBuildingFree(BuildingTypes eIndex)	const															
{
	return (getFreeBuildingCount(eIndex) > 0);
}


void CvPlayer::changeFreeBuildingCount(BuildingTypes eIndex, int iChange)
{
	CvCity* pLoopCity;
	int iOldFreeBuildingCount;
	int iLoop;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		iOldFreeBuildingCount = getFreeBuildingCount(eIndex);

		m_paiFreeBuildingCount[eIndex] = (m_paiFreeBuildingCount[eIndex] + iChange);
		FAssert(getFreeBuildingCount(eIndex) >= 0);

		if (iOldFreeBuildingCount == 0)
		{
			FAssertMsg(getFreeBuildingCount(eIndex) > 0, "getFreeBuildingCount(eIndex) is expected to be greater than 0");

			for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->setNumFreeBuilding(eIndex, 1);
			}
		}
		else if (getFreeBuildingCount(eIndex) == 0)
		{
			FAssertMsg(iOldFreeBuildingCount > 0, "iOldFreeBuildingCount is expected to be greater than 0");

			for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->setNumFreeBuilding(eIndex, 0);
			}
		}
	}
}


// Civ4 Reimagined
void CvPlayer::changeAreaFreeBuildings(BuildingTypes eIndex, int iAreaID, int iChange)
{
	CvCity* pLoopCity;
	int iOldFreeBuildingCount;
	int iLoop;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		iOldFreeBuildingCount = getFreeBuildingCount(eIndex);

		m_paiFreeBuildingCount[eIndex] = (m_paiFreeBuildingCount[eIndex] + iChange);
		FAssert(getFreeBuildingCount(eIndex) >= 0);

		if (iOldFreeBuildingCount == 0)
		{
			FAssertMsg(getFreeBuildingCount(eIndex) > 0, "getFreeBuildingCount(eIndex) is expected to be greater than 0");

			for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity->getArea() == iAreaID)
				{
					pLoopCity->setNumFreeBuilding(eIndex, 1);
				}
			}
		}
		else if (getFreeBuildingCount(eIndex) == 0)
		{
			FAssertMsg(iOldFreeBuildingCount > 0, "iOldFreeBuildingCount is expected to be greater than 0");

			for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity->getArea() == iAreaID)
				{
					pLoopCity->setNumFreeBuilding(eIndex, 0);
				}
			}
		}
	}
}


int CvPlayer::getExtraBuildingHappiness(BuildingTypes eIndex) const 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiExtraBuildingHappiness[eIndex];
}


void CvPlayer::changeExtraBuildingHappiness(BuildingTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiExtraBuildingHappiness[eIndex] += iChange;

		updateExtraBuildingHappiness();
	}
}

int CvPlayer::getExtraBuildingHealth(BuildingTypes eIndex) const 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiExtraBuildingHealth[eIndex];
}


void CvPlayer::changeExtraBuildingHealth(BuildingTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiExtraBuildingHealth[eIndex] += iChange;

		updateExtraBuildingHealth();
	}
}


// Leoreth
int CvPlayer::getBuildingProductionModifier(BuildingTypes eIndex) const
{
	return m_paiBuildingProductionModifiers[eIndex];
}


// Leoreth
// Note Civ4 Reimagined: To modify the production speed of all buildings instead of a single building, use CvPlayer::changeBuildingsProductionModifier(int iChange)
void CvPlayer::changeBuildingProductionModifier(BuildingTypes eIndex, int iChange)
{
	if (iChange != 0)
	{
		m_paiBuildingProductionModifiers[eIndex] += iChange;

		if (getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getFeatureHappiness(FeatureTypes eIndex) const 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiFeatureHappiness[eIndex];
}


void CvPlayer::changeFeatureHappiness(FeatureTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumFeatureInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiFeatureHappiness[eIndex] = (m_paiFeatureHappiness[eIndex] + iChange);

		updateFeatureHappiness();
	}
}


int CvPlayer::getUnitClassCount(UnitClassTypes eIndex) const 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitClassCount[eIndex];
}


bool CvPlayer::isUnitClassMaxedOut(UnitClassTypes eIndex, int iExtra) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!isNationalUnitClass(eIndex))
	{
		return false;
	}
	
	// Civ4 Reimagined
	if (GC.getGameINLINE().isOption(GAMEOPTION_UNLIMITED_WORKERS) && GC.getUnitClassInfo(eIndex).isUnlimitedWorkerGameOption())
	{
		return false;
	}

	int iMaxInstances = GC.getUnitClassInfo(eIndex).getMaxPlayerInstances();

	// Civ4 Reimagined
	if (GC.getUnitClassInfo(eIndex).isUnlimitedWorkerGameOption())
	{
		iMaxInstances += getFreeWorkers();
	}


	FAssertMsg(getUnitClassCount(eIndex) <= iMaxInstances, "getUnitClassCount is expected to be less than maximum bound of MaxPlayerInstances (invalid index)");

	return ((getUnitClassCount(eIndex) + iExtra) >= iMaxInstances);
}


void CvPlayer::changeUnitClassCount(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiUnitClassCount[eIndex] = (m_paiUnitClassCount[eIndex] + iChange);
	FAssert(getUnitClassCount(eIndex) >= 0);
}


int CvPlayer::getUnitClassMaking(UnitClassTypes eIndex) const									 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiUnitClassMaking[eIndex];
}


void CvPlayer::changeUnitClassMaking(UnitClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUnitClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiUnitClassMaking[eIndex] = (m_paiUnitClassMaking[eIndex] + iChange);
		FAssert(getUnitClassMaking(eIndex) >= 0);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Help_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getUnitClassCountPlusMaking(UnitClassTypes eIndex) const
{
	return (getUnitClassCount(eIndex) + getUnitClassMaking(eIndex));
}


int CvPlayer::getBuildingClassCount(BuildingClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingClassCount[eIndex];
}


bool CvPlayer::isBuildingClassMaxedOut(BuildingClassTypes eIndex, int iExtra) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (!isNationalWonderClass(eIndex))
	{
		return false;
	}

	FAssertMsg(getBuildingClassCount(eIndex) <= (GC.getBuildingClassInfo(eIndex).getMaxPlayerInstances() + GC.getBuildingClassInfo(eIndex).getExtraPlayerInstances()), "BuildingClassCount is expected to be less than or match the number of max player instances plus extra player instances");

	return ((getBuildingClassCount(eIndex) + iExtra) >= (GC.getBuildingClassInfo(eIndex).getMaxPlayerInstances() + GC.getBuildingClassInfo(eIndex).getExtraPlayerInstances()));
}


void CvPlayer::changeBuildingClassCount(BuildingClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiBuildingClassCount[eIndex] = (m_paiBuildingClassCount[eIndex] + iChange);
	FAssert(getBuildingClassCount(eIndex) >= 0);
}


int CvPlayer::getBuildingClassMaking(BuildingClassTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiBuildingClassMaking[eIndex];
}


void CvPlayer::changeBuildingClassMaking(BuildingClassTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingClassInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiBuildingClassMaking[eIndex] = (m_paiBuildingClassMaking[eIndex] + iChange);
		FAssert(getBuildingClassMaking(eIndex) >= 0);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Help_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getBuildingClassCountPlusMaking(BuildingClassTypes eIndex) const
{
	return (getBuildingClassCount(eIndex) + getBuildingClassMaking(eIndex));
}


int CvPlayer::getHurryCount(HurryTypes eIndex) const														
{
	//FAssert(eIndex >= 0);
	//FAssert(eIndex < GC.getNumHurryInfos());
	// Civ4 Reimagined
	if (eIndex >= 0 && eIndex < GC.getNumHurryInfos())
	{
		return m_paiHurryCount[eIndex];
	}
	else
	{
		return -1;
	}
}


bool CvPlayer::canHurry(HurryTypes eIndex) const
{	
	return (getHurryCount(eIndex) > 0);
}


bool CvPlayer::canPopRush() const
{
	return (m_iPopRushHurryCount > 0);
}

void CvPlayer::changeHurryCount(HurryTypes eIndex, int iChange)										
{
	FAssert(eIndex >= 0);
	FAssert(eIndex < GC.getNumHurryInfos());

	int oldHurryCount = m_paiHurryCount[eIndex];
	m_paiHurryCount[eIndex] = (m_paiHurryCount[eIndex] + iChange);
	FAssert(getHurryCount(eIndex) >= 0);
	
	// if we just went from 0 to 1 (or the reverse)
	if ((oldHurryCount > 0) != (m_paiHurryCount[eIndex] > 0))
	{
		// does this hurry reduce population?
		if (GC.getHurryInfo(eIndex).getProductionPerPopulation() > 0)
		{
			m_iPopRushHurryCount += iChange;
			FAssert(m_iPopRushHurryCount >= 0);
		}
	}
}

int CvPlayer::getSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex) const														
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiSpecialBuildingNotRequiredCount[eIndex];
}


bool CvPlayer::isSpecialBuildingNotRequired(SpecialBuildingTypes eIndex) const
{
	return (getSpecialBuildingNotRequiredCount(eIndex) > 0);
}


void CvPlayer::changeSpecialBuildingNotRequiredCount(SpecialBuildingTypes eIndex, int iChange)										
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiSpecialBuildingNotRequiredCount[eIndex] = (m_paiSpecialBuildingNotRequiredCount[eIndex] + iChange);
	FAssert(getSpecialBuildingNotRequiredCount(eIndex) >= 0);
}


int CvPlayer::getHasCivicOptionCount(CivicOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiHasCivicOptionCount[eIndex];
}


bool CvPlayer::isHasCivicOption(CivicOptionTypes eIndex) const
{
	return (getHasCivicOptionCount(eIndex) > 0);
}


void CvPlayer::changeHasCivicOptionCount(CivicOptionTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiHasCivicOptionCount[eIndex] = (m_paiHasCivicOptionCount[eIndex] + iChange);
	FAssert(getHasCivicOptionCount(eIndex) >= 0);
}

// Civ4 Reimagined
int CvPlayer::getHasNoAnarchyCivicOptionCount(CivicOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiHasNoAnarchyCivicOptionCount[eIndex];
}

// Civ4 Reimagined
bool CvPlayer::isHasNoAnarchyCivicOption(CivicOptionTypes eIndex) const
{
	return (getHasNoAnarchyCivicOptionCount(eIndex) > 0);
}

// Civ4 Reimagined
void CvPlayer::changeHasNoAnarchyCivicOptionCount(CivicOptionTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiHasNoAnarchyCivicOptionCount[eIndex] = (m_paiHasNoAnarchyCivicOptionCount[eIndex] + iChange);
	FAssert(getHasNoAnarchyCivicOptionCount(eIndex) >= 0);
}

int CvPlayer::getNoCivicUpkeepCount(CivicOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiNoCivicUpkeepCount[eIndex];
}


bool CvPlayer::isNoCivicUpkeep(CivicOptionTypes eIndex) const
{
	return (getNoCivicUpkeepCount(eIndex) > 0);
}


void CvPlayer::changeNoCivicUpkeepCount(CivicOptionTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiNoCivicUpkeepCount[eIndex] = (m_paiNoCivicUpkeepCount[eIndex] + iChange);
		FAssert(getNoCivicUpkeepCount(eIndex) >= 0);

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getHasReligionCount(ReligionTypes eIndex) const													
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiHasReligionCount[eIndex];
}


int CvPlayer::countTotalHasReligion() const
{
	int iCount;
	int iI;

	iCount = 0;

	for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		iCount += getHasReligionCount((ReligionTypes)iI);
	}

	return iCount;
}

int CvPlayer::getHasCorporationCount(CorporationTypes eIndex) const													
{
	if (!isActiveCorporation(eIndex))
	{
		return 0;
	}

	return m_paiHasCorporationCount[eIndex];
}


int CvPlayer::countTotalHasCorporation() const
{
	int iCount = 0;

	for (int iI = 0; iI < GC.getNumCorporationInfos(); iI++)
	{
		iCount += getHasCorporationCount((CorporationTypes)iI);
	}

	return iCount;
}

bool CvPlayer::isActiveCorporation(CorporationTypes eIndex) const
{
	if (isNoCorporations())
	{
		return false;
	}

	if (isNoForeignCorporations() && !hasHeadquarters(eIndex))
	{
		return false;
	}

	return true;
}


int CvPlayer::findHighestHasReligionCount() const
{
	int iValue;
	int ipBestValue;
	int iI;

	ipBestValue = 0;

	for (iI = 0; iI < GC.getNumReligionInfos(); iI++)
	{
		iValue = getHasReligionCount((ReligionTypes)iI);

		if (iValue > ipBestValue)
		{
			ipBestValue = iValue;
		}
	}

	return ipBestValue;
}


void CvPlayer::changeHasReligionCount(ReligionTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumReligionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiHasReligionCount[eIndex] = (m_paiHasReligionCount[eIndex] + iChange);
		FAssert(getHasReligionCount(eIndex) >= 0);

		GC.getGameINLINE().updateBuildingCommerce();

		GC.getGameINLINE().AI_makeAssignWorkDirty();

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_THEOLOGY")))
		{
			if (iChange > 0)
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_THEOLOGY"), getID(), true);
			}
		}
	}
}


void CvPlayer::changeHasCorporationCount(CorporationTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCorporationInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_paiHasCorporationCount[eIndex] += iChange;
		FAssert(getHasCorporationCount(eIndex) >= 0);

		GC.getGameINLINE().updateBuildingCommerce();

		GC.getGameINLINE().AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getUpkeepCount(UpkeepTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUpkeepInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_paiUpkeepCount != NULL, "m_paiUpkeepCount is not expected to be equal with NULL");
	return m_paiUpkeepCount[eIndex];
}


void CvPlayer::changeUpkeepCount(UpkeepTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumUpkeepInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		FAssertMsg(m_paiUpkeepCount != NULL, "m_paiUpkeepCount is not expected to be equal with NULL");
		m_paiUpkeepCount[eIndex] = (m_paiUpkeepCount[eIndex] + iChange);
		FAssertMsg(getUpkeepCount(eIndex) >= 0, "getUpkeepCount(eIndex) is expected to be non-negative (invalid Index)");

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		}
	}
}


int CvPlayer::getSpecialistValidCount(SpecialistTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_paiSpecialistValidCount != NULL, "m_paiSpecialistValidCount is not expected to be equal with NULL");
	return m_paiSpecialistValidCount[eIndex];
}


bool CvPlayer::isSpecialistValid(SpecialistTypes eIndex) const
{
	return (getSpecialistValidCount(eIndex) > 0);
}


void CvPlayer::changeSpecialistValidCount(SpecialistTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		FAssertMsg(m_paiSpecialistValidCount != NULL, "m_paiSpecialistValidCount is not expected to be equal with NULL");
		m_paiSpecialistValidCount[eIndex] = (m_paiSpecialistValidCount[eIndex] + iChange);
		FAssertMsg(getSpecialistValidCount(eIndex) >= 0, "getSpecialistValidCount(eIndex) is expected to be non-negative (invalid Index)");

		AI_makeAssignWorkDirty();
	}
}


// Civ4 Reimagined
int CvPlayer::getExtraSpecialists(SpecialistTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_paiExtraSpecialists != NULL, "m_paiExtraSpecialists is not expected to be equal with NULL");
	return m_paiExtraSpecialists[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeExtraSpecialists(SpecialistTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	
	CvCity* pLoopCity;
	int iLoop;

	if (iChange != 0)
	{
		FAssertMsg(m_paiExtraSpecialists != NULL, "m_paiExtraSpecialists is not expected to be equal with NULL");
		m_paiExtraSpecialists[eIndex] = (m_paiExtraSpecialists[eIndex] + iChange);
		FAssertMsg(m_paiExtraSpecialists[eIndex] >= 0, "m_paiExtraSpecialists(eIndex) is expected to be non-negative (invalid Index)");
		
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->changeFreeSpecialistCount(eIndex, iChange);
		}
	}
}


// Civ4 Reimagined
int CvPlayer::getExtraSpecialistExperience(SpecialistTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_paiExtraSpecialistExperience != NULL, "m_paiExtraSpecialistExperience is not expected to be equal with NULL");
	return m_paiExtraSpecialistExperience[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeExtraSpecialistExperience(SpecialistTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		FAssertMsg(m_paiExtraSpecialistExperience != NULL, "m_paiExtraSpecialistExperience is not expected to be equal with NULL");
		m_paiExtraSpecialistExperience[eIndex] = (m_paiExtraSpecialistExperience[eIndex] + iChange);
		FAssertMsg(m_paiExtraSpecialistExperience[eIndex] >= 0, "m_paiExtraSpecialistExperience(eIndex) is expected to be non-negative (invalid Index)");
	}
}


// Civ4 Reimagined
int CvPlayer::getExtraSpecialistHappiness(SpecialistTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_paiExtraSpecialistHappiness != NULL, "m_paiExtraSpecialistHappiness is not expected to be equal with NULL");
	return m_paiExtraSpecialistHappiness[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeExtraSpecialistHappiness(SpecialistTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		FAssertMsg(m_paiExtraSpecialistHappiness != NULL, "m_paiExtraSpecialistHappiness is not expected to be equal with NULL");
		m_paiExtraSpecialistHappiness[eIndex] = (m_paiExtraSpecialistHappiness[eIndex] + iChange);

		AI_makeAssignWorkDirty();
	}
}


// Civ4 Reimagined
int CvPlayer::getGreatPeopleRateChangeModifier(SpecialistTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(m_paiGreatPeopleRateChangeModifier != NULL, "m_paiGreatPeopleRateChangeModifier is not expected to be equal with NULL");
	return m_paiGreatPeopleRateChangeModifier[eIndex];
}


// Civ4 Reimagined
void CvPlayer::changeGreatPeopleRateChangeModifier(SpecialistTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		FAssertMsg(m_paiGreatPeopleRateChangeModifier != NULL, "m_paiGreatPeopleRateChangeModifier is not expected to be equal with NULL");
		m_paiGreatPeopleRateChangeModifier[eIndex] = (m_paiGreatPeopleRateChangeModifier[eIndex] + iChange);
	}
}


bool CvPlayer::isResearchingTech(TechTypes eIndex) const	
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabResearchingTech[eIndex];
}


void CvPlayer::setResearchingTech(TechTypes eIndex, bool bNewValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumTechInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isResearchingTech(eIndex) != bNewValue)
	{
		m_pabResearchingTech[eIndex] = bNewValue;

		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setDirty(Popup_DIRTY_BIT, true); // to check whether we still need the tech chooser popup
		}
	}
}

bool CvPlayer::isLoyalMember(VoteSourceTypes eVoteSource) const
{
	FAssertMsg(eVoteSource >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eVoteSource < GC.getNumVoteSourceInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabLoyalMember[eVoteSource];
}

void CvPlayer::setLoyalMember(VoteSourceTypes eVoteSource, bool bNewValue)
{
	FAssertMsg(eVoteSource >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eVoteSource < MAX_PLAYERS, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (isLoyalMember(eVoteSource) != bNewValue)
	{
		processVoteSourceBonus(eVoteSource, false);
		m_pabLoyalMember[eVoteSource] = bNewValue;
		processVoteSourceBonus(eVoteSource, true);

		GC.getGameINLINE().updateSecretaryGeneral();
	}
}

// Civ4 Reimagined
bool CvPlayer::isUnlimitedSpecialistsWithTemple(SpecialistTypes eSpecialist) const
{
	FAssertMsg(eSpecialist >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_pabUnlimitedSpecialistsWithTemple[eSpecialist];
}

// Civ4 Reimagined
void CvPlayer::setUnlimitedSpecialistsWithTemple(SpecialistTypes eSpecialist, bool bNewValue)
{
	FAssertMsg(eSpecialist >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	m_pabUnlimitedSpecialistsWithTemple[eSpecialist] = bNewValue;
}

CivicTypes CvPlayer::getCivics(CivicOptionTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paeCivics[eIndex];
}


int CvPlayer::getSingleCivicUpkeep(CivicTypes eCivic, bool bIgnoreAnarchy) const
{
	int iUpkeep;

	if (eCivic == NO_CIVIC)
	{
		return 0;
	}

	if (isNoCivicUpkeep((CivicOptionTypes)(GC.getCivicInfo(eCivic).getCivicOptionType())))
	{
		return 0;
	}

	if (GC.getCivicInfo(eCivic).getUpkeep() == NO_UPKEEP)
	{
		return 0;
	}

	if (!bIgnoreAnarchy)
	{
		if (isAnarchy())
		{
			return 0;
		}
	}

	iUpkeep = 0;

	iUpkeep += ((std::max(0, (getTotalPopulation() + GC.getDefineINT("UPKEEP_POPULATION_OFFSET") - GC.getCivicInfo(eCivic).getCivicOptionType())) * GC.getUpkeepInfo((UpkeepTypes)(GC.getCivicInfo(eCivic).getUpkeep())).getPopulationPercent()) / 100);
	iUpkeep += ((std::max(0, (getNumCities() + GC.getDefineINT("UPKEEP_CITY_OFFSET") + GC.getCivicInfo(eCivic).getCivicOptionType() - (GC.getNumCivicOptionInfos() / 2))) * GC.getUpkeepInfo((UpkeepTypes)(GC.getCivicInfo(eCivic).getUpkeep())).getCityPercent()) / 100);

	iUpkeep *= std::max(0, (getUpkeepModifier() + 100));
	iUpkeep /= 100;

	iUpkeep *= GC.getHandicapInfo(getHandicapType()).getCivicUpkeepPercent();
	iUpkeep /= 100;

	if (!isHuman() && !isBarbarian())
	{
		iUpkeep *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAICivicUpkeepPercent();
		iUpkeep /= 100;

		iUpkeep *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iUpkeep /= 100;
	}

	return std::max(0, iUpkeep);
}


int CvPlayer::getCivicUpkeep(CivicTypes* paeCivics, bool bIgnoreAnarchy) const
{
	int iTotalUpkeep;
	int iI;

	if (paeCivics == NULL)
	{
		paeCivics = m_paeCivics;
	}

	iTotalUpkeep = 0;

	for (iI = 0; iI < GC.getNumCivicOptionInfos(); iI++)
	{
		iTotalUpkeep += getSingleCivicUpkeep(paeCivics[iI], bIgnoreAnarchy);
	}

	return iTotalUpkeep;
}


void CvPlayer::setCivics(CivicOptionTypes eIndex, CivicTypes eNewValue)
{
	CvWString szBuffer;
	CivicTypes eOldCivic;
	int iI;

	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicOptionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eNewValue >= 0, "eNewValue is expected to be non-negative (invalid Index)");
	FAssertMsg(eNewValue < GC.getNumCivicInfos(), "eNewValue is expected to be within maximum bounds (invalid Index)");

	eOldCivic = getCivics(eIndex);

	if (eOldCivic != eNewValue)
	{
		m_paeCivics[eIndex] = eNewValue;

		if (eOldCivic != NO_CIVIC)
		{
			processCivics(eOldCivic, -1);

			// Civ4 Reimagined: Carthage UP
			if (isLegacyCivic() && getLegacyCivic() == eOldCivic)
			{
				szBuffer = gDLL->getText("TXT_KEY_MISC_LEGACY_CIVIC_EFFECT", GC.getCivicInfo(eOldCivic).getTextKeyWide());
				gDLL->getInterfaceIFace()->addHumanMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MAJOR_EVENT);
			}
		}

		if (getCivics(eIndex) != NO_CIVIC)
		{
			processCivics(getCivics(eIndex), 1);

			// Civ4 Reimagined: Carthage UP
			if (isLegacyCivic() && getLegacyCivic() == NO_CIVIC && eIndex == (CivicOptionTypes)GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))
			{
				setLegacyCivic(eNewValue);
			}

			// Civ4 Reimagined
			if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CONSTITUTION")))
			{
				if (getCivics(eIndex) == (CivicTypes)GC.getInfoTypeForString("CIVIC_SECULARISM"))
				{
					GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CONSTITUTION"), getID(), true);
				}
			}

			// Civ4 Reimagined
			if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_INDUSTRIALISM")))
			{
				if (getCivics(eIndex) == (CivicTypes)GC.getInfoTypeForString("CIVIC_INDUSTRIALISM"))
				{
					GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_INDUSTRIALISM"), getID(), true);
				}
			}
		}

		GC.getGameINLINE().updateSecretaryGeneral();

		GC.getGameINLINE().AI_makeAssignWorkDirty();

		if (GC.getGameINLINE().isFinalInitialized())
		{
			if (gDLL->isDiplomacy() && (gDLL->getDiplomacyPlayer() == getID()))
			{
				gDLL->updateDiplomacyAttitude(true);
			}

			if (!isBarbarian())
			{
				if (getCivics(eIndex) != NO_CIVIC)
				{
					/* original code (which erroneously blocked the message for certain civic switches)
					if (getCivics(eIndex) != GC.getCivilizationInfo(getCivilizationType()).getCivilizationInitialCivics(eIndex))*/
					// K-Mod
					if (eOldCivic != NO_CIVIC)
					// K-Mod end
					{
						for (iI = 0; iI < MAX_PLAYERS; iI++)
						{
							if (GET_PLAYER((PlayerTypes)iI).isAlive())
							{
								if (GET_TEAM(getTeam()).isHasMet(GET_PLAYER((PlayerTypes)iI).getTeam()))
								{
									szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_ADOPTED_CIVIC", getNameKey(), GC.getCivicInfo(getCivics(eIndex)).getTextKeyWide());
									gDLL->getInterfaceIFace()->addHumanMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MAJOR_EVENT);
								}
							}
						}

						szBuffer = gDLL->getText("TXT_KEY_MISC_PLAYER_ADOPTED_CIVIC", getNameKey(), GC.getCivicInfo(getCivics(eIndex)).getTextKeyWide());
						GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szBuffer);
					}
				}
			}
			GC.getGameINLINE().updateGwPercentAnger(); // K-Mod. (environmentalism can change this. It's nice to see the effects immediately.)
		}

		// K-Mod. Attitude cache.
		
		for (PlayerTypes i = (PlayerTypes)0; i < MAX_PLAYERS; i=(PlayerTypes)(i+1))
		{
			// Civ4 Reimagined: Update cache for all player combinations upon civic change (for favourite civic evaluation)
			for (PlayerTypes j = (PlayerTypes)0; j < MAX_PLAYERS; j=(PlayerTypes)(j+1))
			{
				GET_PLAYER(i).AI_updateAttitudeCache(j);
			}
			
			//GET_PLAYER(getID()).AI_updateAttitudeCache(i);
			//GET_PLAYER(i).AI_updateAttitudeCache(getID());
		}
		// K-Mod end
	}
}


int CvPlayer::getSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	FAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "eIndex1 expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eIndex2 >= 0, "eIndex2 expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 expected to be < NUM_YIELD_TYPES");
	return m_ppaaiSpecialistExtraYield[eIndex1][eIndex2];
}


void CvPlayer::changeSpecialistExtraYield(SpecialistTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	FAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "eIndex1 expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eIndex2 >= 0, "eIndex2 expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_ppaaiSpecialistExtraYield[eIndex1][eIndex2] = (m_ppaaiSpecialistExtraYield[eIndex1][eIndex2] + iChange);
		FAssert(getSpecialistExtraYield(eIndex1, eIndex2) >= 0);

		updateExtraSpecialistYield();

		AI_makeAssignWorkDirty();
	}
}

// Civ4 Reimagined
int CvPlayer::getSpecialistCommerceChange(SpecialistTypes eIndex1, CommerceTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	FAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "eIndex1 expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eIndex2 >= 0, "eIndex2 expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_COMMERCE_TYPES, "eIndex2 expected to be < NUM_COMMERCE_TYPES");
	return m_ppaaiSpecialistCommerceChanges[eIndex1][eIndex2];
}

// Civ4 Reimagined
void CvPlayer::changeSpecialistCommerceChange(SpecialistTypes eIndex1, CommerceTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	FAssertMsg(eIndex1 < GC.getNumSpecialistInfos(), "eIndex1 expected to be < GC.getNumSpecialistInfos()");
	FAssertMsg(eIndex2 >= 0, "eIndex2 expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_COMMERCE_TYPES, "eIndex2 expected to be < NUM_COMMERCE_TYPES");

	if (iChange != 0)
	{
		m_ppaaiSpecialistCommerceChanges[eIndex1][eIndex2] = (m_ppaaiSpecialistCommerceChanges[eIndex1][eIndex2] + iChange);
		FAssert(getSpecialistCommerceChange(eIndex1, eIndex2) >= 0);

		updateCommerce();

		AI_makeAssignWorkDirty();
	}
}

// Civ4 Reimagined
int CvPlayer::getBonusHealthFromBuilding(BuildingTypes eIndex1, BonusTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 expected to be < GC.getNumBuildingInfos()");
	FAssertMsg(eIndex2 >= 0, "eIndex2 expected to be >= 0");
	FAssertMsg(eIndex2 < GC.getNumBonusInfos(), "eIndex2 expected to be < GC.getNumBonusInfos()");
	return m_ppaaiBonusHealthFromBuilding[eIndex1][eIndex2];
}

// Civ4 Reimagined
void CvPlayer::changeBonusHealthFromBuilding(BuildingTypes eIndex1, BonusTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 expected to be >= 0");
	FAssertMsg(eIndex1 < GC.getNumBuildingInfos(), "eIndex1 expected to be < GC.getNumBuildingInfos()");
	FAssertMsg(eIndex2 >= 0, "eIndex2 expected to be >= 0");
	FAssertMsg(eIndex2 < GC.getNumBonusInfos(), "eIndex2 expected to be < GC.getNumBonusInfos()");

	if (iChange != 0)
	{
		m_ppaaiBonusHealthFromBuilding[eIndex1][eIndex2] = (m_ppaaiBonusHealthFromBuilding[eIndex1][eIndex2] + iChange);
		FAssert(getBonusHealthFromBuilding(eIndex1, eIndex2) >= 0);

		int iLoop;
		for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->updateResources();
		}

		AI_makeAssignWorkDirty();
	}
}

// Civ4 Reimagined
int CvPlayer::getSpecialistThresholdExtraYield(YieldTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex2 expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex2 expected to be < NUM_YIELD_TYPES");
	return m_aiSpecialistThresholdExtraYield[eIndex];
}

// Civ4 Reimagined
void CvPlayer::changeSpecialistThresholdExtraYield(YieldTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex2 expected to be >= 0");
	FAssertMsg(eIndex < NUM_YIELD_TYPES, "eIndex2 expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aiSpecialistThresholdExtraYield[eIndex] = (m_aiSpecialistThresholdExtraYield[eIndex] + iChange);
		FAssert(getSpecialistThresholdExtraYield(eIndex) >= 0);

		updateExtraSpecialistYield();

		AI_makeAssignWorkDirty();
	}
}


int CvPlayer::getImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiImprovementYieldChange[eIndex1][eIndex2];
}


void CvPlayer::changeImprovementYieldChange(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ppaaiImprovementYieldChange[eIndex1][eIndex2] = (m_ppaaiImprovementYieldChange[eIndex1][eIndex2] + iChange);
		// FAssert(getImprovementYieldChange(eIndex1, eIndex2) >= 0); // Towns in K-Mod get -1 commerce with serfdom.

		updateYield();
	}
}


// Civ4 Reimagined
int CvPlayer::getImprovementYieldChangeAdjacentToStrategicBonus(ImprovementTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus[eIndex1][eIndex2];
}


// Civ4 Reimagined
void CvPlayer::changeImprovementYieldChangeAdjacentToStrategicBonus(ImprovementTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus[eIndex1][eIndex2] = (m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus[eIndex1][eIndex2] + iChange);

		updateYield();
	}
}


// Civ4 Reimagined
int CvPlayer::getTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumTerrainInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiTerrainYieldChange[eIndex1][eIndex2];
}


// Civ4 Reimagined
void CvPlayer::changeTerrainYieldChange(TerrainTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumTerrainInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ppaaiTerrainYieldChange[eIndex1][eIndex2] = (m_ppaaiTerrainYieldChange[eIndex1][eIndex2] + iChange);

		updateYield();
	}
}


// Civ4 Reimagined
int CvPlayer::getRadiusImprovementCommerceChange(ImprovementTypes eIndex1, CommerceTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_COMMERCE_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiRadiusImprovementCommerceChange[eIndex1][eIndex2];
}


void CvPlayer::changeRadiusImrovementCommerceChange(ImprovementTypes eIndex1, CommerceTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumImprovementInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_COMMERCE_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ppaaiRadiusImprovementCommerceChange[eIndex1][eIndex2] = (m_ppaaiRadiusImprovementCommerceChange[eIndex1][eIndex2] + iChange);
		updateCommerce();
	}
}


// Civ4 Reimagined
int CvPlayer::getBuildingYieldChange(BuildingClassTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingClassInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");
	return m_ppaaiBuildingYieldChange[eIndex1][eIndex2];
}


// Civ4 Reimagined
void CvPlayer::changeBuildingYieldChange(BuildingClassTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex1 < GC.getNumBuildingClassInfos(), "eIndex1 is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ppaaiBuildingYieldChange[eIndex1][eIndex2] = (m_ppaaiBuildingYieldChange[eIndex1][eIndex2] + iChange);
		
		CvTeamAI& currentTeam = GET_TEAM(getTeam());
		
		BuildingTypes eOurBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(eIndex1);
		
		if (eOurBuilding != NO_BUILDING && !currentTeam.isObsoleteBuilding(eOurBuilding)) 
		{
			int iLoop;

			// Update yield in all cities with new values
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->changeBaseYieldRate(eIndex2, iChange * pLoopCity->getNumRealBuilding(eOurBuilding));
			}
		}
	}	
}


// Civ4 Reimagined
int CvPlayer::getFeatureCommerce(FeatureTypes eFeatureIndex, CommerceTypes eCommerceIndex) const
{
	FAssertMsg(eFeatureIndex >= 0, "eFeatureIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eFeatureIndex < GC.getNumFeatureInfos(), "eFeatureIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eCommerceIndex >= 0, "eCommerceIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eCommerceIndex < NUM_COMMERCE_TYPES, "eCommerceIndex is expected to be within maximum bounds (invalid Index)");
	return m_ppaiFeatureCommerce[eFeatureIndex][eCommerceIndex];
}


// Civ4 Reimagined
void CvPlayer::changeFeatureCommerce(FeatureTypes eFeatureIndex, CommerceTypes eCommerceIndex, int iChange)
{
	FAssertMsg(eFeatureIndex >= 0, "eFeatureIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eFeatureIndex < GC.getNumFeatureInfos(), "eFeatureIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eCommerceIndex >= 0, "eCommerceIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eCommerceIndex < NUM_COMMERCE_TYPES, "eCommerceIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ppaiFeatureCommerce[eFeatureIndex][eCommerceIndex] = (m_ppaiFeatureCommerce[eFeatureIndex][eCommerceIndex] + iChange);
		
		int iLoop;
		for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop) )
		{
			pLoopCity->updateFeatureCommerce();
		}
	}	
}


// Civ4 Reimagined
int CvPlayer::getFeatureExtraYield(FeatureTypes eFeatureIndex, YieldTypes eYieldIndex) const
{
	FAssertMsg(eFeatureIndex >= 0, "eFeatureIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eFeatureIndex < GC.getNumFeatureInfos(), "eFeatureIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eYieldIndex >= 0, "eYieldIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eYieldIndex < NUM_YIELD_TYPES, "eYieldIndex is expected to be within maximum bounds (invalid Index)");
	return m_ppaiFeatureExtraYield[eFeatureIndex][eYieldIndex];
}


// Civ4 Reimagined
void CvPlayer::changeFeatureExtraYield(FeatureTypes eFeatureIndex, YieldTypes eYieldIndex, int iChange)
{
	FAssertMsg(eFeatureIndex >= 0, "eFeatureIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eFeatureIndex < GC.getNumFeatureInfos(), "eFeatureIndex is expected to be within maximum bounds (invalid Index)");
	FAssertMsg(eYieldIndex >= 0, "eCommerceIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eYieldIndex < NUM_YIELD_TYPES, "eYieldIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_ppaiFeatureExtraYield[eFeatureIndex][eYieldIndex] = (m_ppaiFeatureExtraYield[eFeatureIndex][eYieldIndex] + iChange);
		
		updateYield();
	}	
}


// K-Mod. I've changed this function from using pUnit to using pGroup.
// I've also rewriten most of the code, to give more natural ordering, and to be more robust and readable code.
void CvPlayer::updateGroupCycle(CvSelectionGroup* pGroup)
{
	PROFILE_FUNC();
	FAssert(pGroup);

	CvPlot* pPlot = pGroup->plot();

	if (!pPlot || !isCycleGroup(pGroup))
		return;

	CvUnit* pUnit = pGroup->getHeadUnit();
	FAssert(pUnit);

	//removeGroupCycle(pGroup->getID()); // will be removed while we reposition it

	CLLNode<int>* pBestSelectionGroupNode = NULL;
	int iBestCost = MAX_INT;

	CvSelectionGroup* pPreviousGroup = NULL;

	CLLNode<int>* pSelectionGroupNode = headGroupCycleNode();

	while (pSelectionGroupNode)
	{
		CvSelectionGroup* pNextGroup = getSelectionGroup(pSelectionGroupNode->m_data);
		FAssert(pNextGroup);

		// if we find our group in the list, remove it.
		if (pNextGroup == pGroup)
		{
			pSelectionGroupNode = deleteGroupCycleNode(pSelectionGroupNode);
		}
		else if (isCycleGroup(pNextGroup) && pNextGroup->canAllMove())
		{
			//int iCost = groupCycleDistance(pPreviousGroup, pGroup) + groupCycleDistance(pGroup, pNextGroup) - groupCycleDistance(pPreviousGroup, pNextGroup);
			int iCost = groupCycleDistance(pGroup, pNextGroup) + (pPreviousGroup ? groupCycleDistance(pPreviousGroup, pGroup) - groupCycleDistance(pPreviousGroup, pNextGroup) : 3);
			if (iCost < iBestCost)
			{
				iBestCost = iCost;
				pBestSelectionGroupNode = pSelectionGroupNode;
			}
			pPreviousGroup = pNextGroup;
			pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
		}
		else
		{
			pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
		}
	}
	if (pPreviousGroup)
	{
		FAssert(isCycleGroup(pPreviousGroup) && pPreviousGroup->canAllMove());
		int iCost = groupCycleDistance(pPreviousGroup, pGroup) + 3; // cost for being at the end of the list.
		if (iCost < iBestCost)
		{
			pBestSelectionGroupNode = 0;
		}
	}

	if (pBestSelectionGroupNode)
	{
		m_groupCycle.insertBefore(pUnit->getGroupID(), pBestSelectionGroupNode);
	}
	else
	{
		m_groupCycle.insertAtEnd(pUnit->getGroupID());
	}
}


void CvPlayer::removeGroupCycle(int iID)
{
	CLLNode<int>* pSelectionGroupNode;

	pSelectionGroupNode = headGroupCycleNode();

	while (pSelectionGroupNode != NULL)
	{
		if (pSelectionGroupNode->m_data == iID)
		{
			pSelectionGroupNode = deleteGroupCycleNode(pSelectionGroupNode);
			break;
		}
		else
		{
			pSelectionGroupNode = nextGroupCycleNode(pSelectionGroupNode);
		}
	}
}

// K-Mod
void CvPlayer::refreshGroupCycleList()
{
	std::vector<CvSelectionGroup*> update_list;

	CLLNode<int>* pNode = headGroupCycleNode();
	while (pNode)
	{
		CvSelectionGroup* pLoopGroup = getSelectionGroup(pNode->m_data);
		CvUnit* pLoopHead = pLoopGroup->getHeadUnit();
		if (pLoopHead && isCycleGroup(pLoopGroup) && pLoopGroup->canAllMove() && (pLoopHead->hasMoved() || (pLoopHead->isCargo() && pLoopHead->getTransportUnit()->hasMoved())))
		{
			update_list.push_back(pLoopGroup);
			pNode = deleteGroupCycleNode(pNode);
		}
		else
			pNode = nextGroupCycleNode(pNode);
	}

	for (size_t i = 0; i < update_list.size(); i++)
	{
		updateGroupCycle(update_list[i]);
	}
}
// K-Mod end

CLLNode<int>* CvPlayer::deleteGroupCycleNode(CLLNode<int>* pNode)
{
	return m_groupCycle.deleteNode(pNode);
}


CLLNode<int>* CvPlayer::nextGroupCycleNode(CLLNode<int>* pNode) const
{
	return m_groupCycle.next(pNode);
}


CLLNode<int>* CvPlayer::previousGroupCycleNode(CLLNode<int>* pNode) const
{
	return m_groupCycle.prev(pNode);
}


CLLNode<int>* CvPlayer::headGroupCycleNode() const
{
	return m_groupCycle.head();
}


CLLNode<int>* CvPlayer::tailGroupCycleNode() const
{
	return m_groupCycle.tail();
}


//	Finds the path length from this tech type to one you already know
int CvPlayer::findPathLength(TechTypes eTech, bool bCost) const
{
	int i;
	int iNumSteps = 0;
	int iShortestPath = 0;
	int iPathLength = 0;
	TechTypes ePreReq;
	TechTypes eShortestOr;

	if (GET_TEAM(getTeam()).isHasTech(eTech) || isResearchingTech(eTech))
	{
		//	We have this tech, no reason to add this to the pre-reqs
		//	Base case return 0, we know it...
		return 0;
	}

	//	Cycle through the and paths and add up their tech lengths
	for (i = 0; i < GC.getNUM_AND_TECH_PREREQS(); i++)
	{
		ePreReq = (TechTypes)GC.getTechInfo(eTech).getPrereqAndTechs(i);

		if (ePreReq != NO_TECH)
		{
			iPathLength += findPathLength(ePreReq, bCost);
		}
	}

	eShortestOr = NO_TECH;
	iShortestPath = MAX_INT;
	//	Find the shortest OR tech
	for (i = 0; i < GC.getNUM_OR_TECH_PREREQS(); i++)
	{
		//	Grab the tech
		ePreReq = (TechTypes)GC.getTechInfo(eTech).getPrereqOrTechs(i);

		//	If this is a valid tech
		if (ePreReq != NO_TECH)
		{
			//	Recursively find the path length (takes into account all ANDs)
			iNumSteps = findPathLength(ePreReq, bCost);

			//	If the prereq is a valid tech and its the current shortest, mark it as such
			if (iNumSteps < iShortestPath)
			{
				eShortestOr = ePreReq;
				iShortestPath = iNumSteps;
			}
		}
	}

	//	If the shortest OR is a valid tech, add the steps to it...
	if (eShortestOr != NO_TECH)
	{
		iPathLength += iShortestPath;
	}

	return (iPathLength + ((bCost) ? GET_TEAM(getTeam()).getResearchCost(eTech) : 1));
}


//	Function specifically for python/tech chooser screen
int CvPlayer::getQueuePosition(TechTypes eTech) const
{
	int i = 1;
	CLLNode<TechTypes>* pResearchNode;

	for (pResearchNode = headResearchQueueNode(); pResearchNode; pResearchNode = nextResearchQueueNode(pResearchNode))
	{
		if (pResearchNode->m_data == eTech)
		{
			return i;
		}
		i++;
	}

	return -1;
}


void CvPlayer::clearResearchQueue()
{
	int iI;

	m_researchQueue.clear();

	for (iI = 0; iI < GC.getNumTechInfos(); iI++)
	{
		setResearchingTech(((TechTypes)iI), false);
	}

	if (getTeam() == GC.getGameINLINE().getActiveTeam())
	{
		gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


//	Pushes research onto the queue.  If it is an append if will put it
//	and its pre-reqs into the queue.  If it is not an append it will change
//	research immediately and should be used with clear.  Clear will clear the entire queue.
bool CvPlayer::pushResearch(TechTypes eTech, bool bClear)
{
	int i;
	int iNumSteps;
	int iShortestPath;
	bool bOrPrereqFound;
	TechTypes ePreReq;
  TechTypes eShortestOr;

	FAssertMsg(eTech != NO_TECH, "Tech is not assigned a valid value");

	if (GET_TEAM(getTeam()).isHasTech(eTech) || isResearchingTech(eTech))
	{
		//	We have this tech, no reason to add this to the pre-reqs
		return true;
	}

	if (!canEverResearch(eTech))
	{
		return false;
	}

	//	Pop the entire queue...
	if (bClear)
	{
		clearResearchQueue();
	}

	//	Add in all the pre-reqs for the and techs...
	for (i = 0; i < GC.getNUM_AND_TECH_PREREQS(); i++)
	{
		ePreReq = (TechTypes)GC.getTechInfo(eTech).getPrereqAndTechs(i);

		if (ePreReq != NO_TECH)
		{
			if (!pushResearch(ePreReq))
			{
				return false;
			}
		}
	}

	// Will return the shortest path of all the or techs.  Tie breaker goes to the first one...
	eShortestOr = NO_TECH;
	iShortestPath = MAX_INT;
	bOrPrereqFound = false;
	//	Cycle through all the OR techs
	for (i = 0; i < GC.getNUM_OR_TECH_PREREQS(); i++)
	{
		ePreReq = (TechTypes)GC.getTechInfo(eTech).getPrereqOrTechs(i);

		if (ePreReq != NO_TECH)
		{
			bOrPrereqFound = true;

			//	If the pre-req exists, and we have it, it is the shortest path, get out, we're done
			if (GET_TEAM(getTeam()).isHasTech(ePreReq))
			{
				eShortestOr = ePreReq;
				break;
			}

			if (canEverResearch(ePreReq))
			{
				//	Find the length of the path to this pre-req
				iNumSteps = findPathLength(ePreReq);

				//	If this pre-req is a valid tech, and its the shortest current path, set it as such
				if (iNumSteps < iShortestPath)
				{
					eShortestOr = ePreReq;
					iShortestPath = iNumSteps;
				}
			}
		}
	}

	//	If the shortest path tech is valid, push it (and its children) on to the research queue recursively
	if (eShortestOr != NO_TECH)
	{
		if (!pushResearch(eShortestOr))
		{
			return false;
		}
	}
	else if (bOrPrereqFound)
	{
		return false;
	}

	//	Insert this tech at the end of the queue
	m_researchQueue.insertAtEnd(eTech);

	setResearchingTech(eTech, true);

	//	Set the dirty bits
	if (getTeam() == GC.getGameINLINE().getActiveTeam())
	{
		gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}

	// ONEVENT - Tech selected (any)
	CvEventReporter::getInstance().techSelected(eTech, getID());

	return true;
}


//	If bHead is true we delete the entire queue...
void CvPlayer::popResearch(TechTypes eTech)
{
	CLLNode<TechTypes>* pResearchNode;

	for (pResearchNode = headResearchQueueNode(); pResearchNode; pResearchNode = nextResearchQueueNode(pResearchNode))
	{
		if (pResearchNode->m_data == eTech)
		{
			m_researchQueue.deleteNode(pResearchNode);
			break;
		}
	}

	setResearchingTech(eTech, false);

	if (getTeam() == GC.getGameINLINE().getActiveTeam())
	{
		gDLL->getInterfaceIFace()->setDirty(ResearchButtons_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(GameData_DIRTY_BIT, true);
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}
}


int CvPlayer::getLengthResearchQueue() const
{
	return m_researchQueue.getLength();
}


CLLNode<TechTypes>* CvPlayer::nextResearchQueueNode(CLLNode<TechTypes>* pNode) const						
{
	return m_researchQueue.next(pNode);
}


CLLNode<TechTypes>* CvPlayer::headResearchQueueNode() const													
{
	return m_researchQueue.head();
}


CLLNode<TechTypes>* CvPlayer::tailResearchQueueNode() const													 
{
	return m_researchQueue.tail();
}


void CvPlayer::addCityName(const CvWString& szName)
{
	m_cityNames.insertAtEnd(szName);
}


int CvPlayer::getNumCityNames() const
{
	return m_cityNames.getLength();
}


CvWString CvPlayer::getCityName(int iIndex) const
{
	CLLNode<CvWString>* pCityNameNode;

	pCityNameNode = m_cityNames.nodeNum(iIndex);

	if (pCityNameNode != NULL)
	{
		return pCityNameNode->m_data;
	}
	else
	{
		return L"";
	}
}


CLLNode<CvWString>* CvPlayer::nextCityNameNode(CLLNode<CvWString>* pNode) const
{
	return m_cityNames.next(pNode);
}


CLLNode<CvWString>* CvPlayer::headCityNameNode() const
{
	return m_cityNames.head();
}


CvPlotGroup* CvPlayer::firstPlotGroup(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_plotGroups.beginIter(pIterIdx) : m_plotGroups.endIter(pIterIdx);
}


CvPlotGroup* CvPlayer::nextPlotGroup(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_plotGroups.nextIter(pIterIdx) : m_plotGroups.prevIter(pIterIdx);
}


int CvPlayer::getNumPlotGroups() const																		
{
	return m_plotGroups.getCount();
}


CvPlotGroup* CvPlayer::getPlotGroup(int iID) const															
{
	return((CvPlotGroup *)(m_plotGroups.getAt(iID)));
}


CvPlotGroup* CvPlayer::addPlotGroup()																	
{
	return((CvPlotGroup *)(m_plotGroups.add()));
}


void CvPlayer::deletePlotGroup(int iID)																
{
	m_plotGroups.removeAt(iID);
}


CvCity* CvPlayer::firstCity(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_cities.beginIter(pIterIdx) : m_cities.endIter(pIterIdx);
}


CvCity* CvPlayer::nextCity(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_cities.nextIter(pIterIdx) : m_cities.prevIter(pIterIdx);
}


int CvPlayer::getNumCities() const																			
{
	return m_cities.getCount();
}


CvCity* CvPlayer::getCity(int iID) const
{
	return(m_cities.getAt(iID));
}


CvCity* CvPlayer::addCity()
{
	return(m_cities.add());
}


void CvPlayer::deleteCity(int iID)
{
	m_cities.removeAt(iID);
}


CvUnit* CvPlayer::firstUnit(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_units.beginIter(pIterIdx) : m_units.endIter(pIterIdx);
}


CvUnit* CvPlayer::nextUnit(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_units.nextIter(pIterIdx) : m_units.prevIter(pIterIdx);
}


int CvPlayer::getNumUnits() const
{
	return m_units.getCount();
}


CvUnit* CvPlayer::getUnit(int iID) const
{
	return (m_units.getAt(iID));
}


CvUnit* CvPlayer::addUnit()																			
{
	return (m_units.add());
}


void CvPlayer::deleteUnit(int iID)																	
{
	m_units.removeAt(iID);
}


CvSelectionGroup* CvPlayer::firstSelectionGroup(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_selectionGroups.beginIter(pIterIdx) : m_selectionGroups.endIter(pIterIdx);
}


CvSelectionGroup* CvPlayer::nextSelectionGroup(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_selectionGroups.nextIter(pIterIdx) : m_selectionGroups.prevIter(pIterIdx);
}


int CvPlayer::getNumSelectionGroups() const																
{
	return m_selectionGroups.getCount();
}


CvSelectionGroup* CvPlayer::getSelectionGroup(int iID) const												
{
	return ((CvSelectionGroup *)(m_selectionGroups.getAt(iID)));
}


CvSelectionGroup* CvPlayer::addSelectionGroup()
{
	//return ((CvSelectionGroup *)(m_selectionGroups.add()));
	// K-Mod. Make sure that every time we add a group, it also gets added to the group cycle list.
	// (we can update the specific position in the cycle list later; but it's important it gets into the list.)
	CvSelectionGroup* pGroup = m_selectionGroups.add();
	if (pGroup)
		m_groupCycle.insertAtEnd(pGroup->getID());
	return pGroup;
	// K-Mod end
}


void CvPlayer::deleteSelectionGroup(int iID)
{
	bool bRemoved = m_selectionGroups.removeAt(iID);
	
	FAssertMsg(bRemoved, "could not find group, delete failed");
}

EventTriggeredData* CvPlayer::firstEventTriggered(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_eventsTriggered.beginIter(pIterIdx) : m_eventsTriggered.endIter(pIterIdx);
}

EventTriggeredData* CvPlayer::nextEventTriggered(int *pIterIdx, bool bRev) const
{
	return !bRev ? m_eventsTriggered.nextIter(pIterIdx) : m_eventsTriggered.prevIter(pIterIdx);
}

int CvPlayer::getNumEventsTriggered() const
{
	return m_eventsTriggered.getCount();
}

EventTriggeredData* CvPlayer::getEventTriggered(int iID) const
{
	return ((EventTriggeredData*)(m_eventsTriggered.getAt(iID)));
}

EventTriggeredData* CvPlayer::addEventTriggered()
{
	return ((EventTriggeredData*)(m_eventsTriggered.add()));
}

void CvPlayer::deleteEventTriggered(int iID)
{
	m_eventsTriggered.removeAt(iID);
}


void CvPlayer::addMessage(const CvTalkingHeadMessage& message)
{
	m_listGameMessages.push_back(message);
}


void CvPlayer::clearMessages()
{
	m_listGameMessages.clear();
}


const CvMessageQueue& CvPlayer::getGameMessages() const
{
	return (m_listGameMessages);
}


void CvPlayer::expireMessages()
{
	CvMessageQueue::iterator it = m_listGameMessages.begin();
	bool bFoundExpired = false;
	while(it != m_listGameMessages.end())
	{
		CvTalkingHeadMessage& message = *it;
		if (GC.getGameINLINE().getGameTurn() >= message.getExpireTurn())
		{
			it = m_listGameMessages.erase(it);
			bFoundExpired = true;
		}
		else
		{
			++it;
		}
	}
	if (bFoundExpired)
	{
		gDLL->getInterfaceIFace()->dirtyTurnLog(getID());
	}
}


void CvPlayer::addPopup(CvPopupInfo* pInfo, bool bFront)
{
	if (isHuman())
	{
		if (bFront)
		{
			m_listPopups.push_front(pInfo);
		}
		else
		{
			m_listPopups.push_back(pInfo);
		}
	}
	else
	{
		SAFE_DELETE(pInfo);
	}
}


void CvPlayer::clearPopups()
{
	CvPopupQueue::iterator it;
	for (it = m_listPopups.begin(); it != m_listPopups.end(); ++it)
	{
		CvPopupInfo* pInfo = *it;
		if (NULL != pInfo)
		{
			delete pInfo;
		}
	}
	m_listPopups.clear();
}


CvPopupInfo* CvPlayer::popFrontPopup()
{
	CvPopupInfo* pInfo = NULL;
	if (!m_listPopups.empty())
	{
		pInfo = m_listPopups.front();
		m_listPopups.pop_front();
	}
	return pInfo;
}


const CvPopupQueue& CvPlayer::getPopups() const
{
	return (m_listPopups);
}


void CvPlayer::addDiplomacy(CvDiploParameters* pDiplo)
{
	if (NULL != pDiplo)
	{
		m_listDiplomacy.push_back(pDiplo);
	}
}


void CvPlayer::clearDiplomacy()
{
	CvDiploQueue::iterator it;
	for (it = m_listDiplomacy.begin(); it != m_listDiplomacy.end(); ++it)
	{
		CvDiploParameters* pDiplo = *it;
		if (NULL != pDiplo)
		{
			delete pDiplo;
		}
	}
	m_listDiplomacy.clear();
}


const CvDiploQueue& CvPlayer::getDiplomacy() const
{
	return (m_listDiplomacy);
}


CvDiploParameters* CvPlayer::popFrontDiplomacy()
{
	CvDiploParameters* pDiplo = NULL;
	if (!m_listDiplomacy.empty())
	{
		pDiplo = m_listDiplomacy.front();
		m_listDiplomacy.pop_front();
	}
	return pDiplo;
}


void CvPlayer::showSpaceShip()
{
	CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_PYTHON_SCREEN);
	pInfo->setData1(-1);
	pInfo->setText(L"showSpaceShip");
	addPopup(pInfo);
}

void CvPlayer::clearSpaceShipPopups()
{
	//clear all spaceship popups
	CvPopupQueue::iterator it;
	for (it = m_listPopups.begin(); it != m_listPopups.end(); )
	{
		CvPopupInfo* pInfo = *it;
		if (NULL != pInfo)
		{
			if(pInfo->getText().compare(L"showSpaceShip") == 0)
			{
				it = m_listPopups.erase(it);
				SAFE_DELETE(pInfo);
			}
			else
			{
				++it;
			}
		}
		else
		{
			++it;
		}
	}
}

int CvPlayer::getScoreHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapScoreHistory.find(iTurn);
	if (it != m_mapScoreHistory.end())
	{
		return it->second;
	}
	return 0;
}

void CvPlayer::updateScoreHistory(int iTurn, int iBestScore)
{
	m_mapScoreHistory[iTurn] = iBestScore;
}

int CvPlayer::getEconomyHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapEconomyHistory.find(iTurn);
	if (it != m_mapEconomyHistory.end())
	{
		return it->second;
	}
	return 0;
}

void CvPlayer::updateEconomyHistory(int iTurn, int iBestEconomy)
{
	m_mapEconomyHistory[iTurn] = iBestEconomy;
}

int CvPlayer::getIndustryHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapIndustryHistory.find(iTurn);
	if (it != m_mapIndustryHistory.end())
	{
		return it->second;
	}
	return 0;
}

void CvPlayer::updateIndustryHistory(int iTurn, int iBestIndustry)
{
	m_mapIndustryHistory[iTurn] = iBestIndustry;
}

int CvPlayer::getAgricultureHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapAgricultureHistory.find(iTurn);
	if (it != m_mapAgricultureHistory.end())
	{
		return it->second;
	}
	return 0;
}

void CvPlayer::updateAgricultureHistory(int iTurn, int iBestAgriculture)
{
	m_mapAgricultureHistory[iTurn] = iBestAgriculture;
}

int CvPlayer::getPowerHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapPowerHistory.find(iTurn);
	if (it != m_mapPowerHistory.end())
	{
		return it->second;
	}
	return 0;
}

void CvPlayer::updatePowerHistory(int iTurn, int iBestPower)
{
	m_mapPowerHistory[iTurn] = iBestPower;
}

int CvPlayer::getCultureHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapCultureHistory.find(iTurn);
	if (it != m_mapCultureHistory.end())
	{
		return it->second;
	}
	return 0;
}

void CvPlayer::updateCultureHistory(int iTurn, int iBestCulture)
{
	m_mapCultureHistory[iTurn] = iBestCulture;
}

int CvPlayer::getEspionageHistory(int iTurn) const
{
	CvTurnScoreMap::const_iterator it = m_mapEspionageHistory.find(iTurn);
	if (it != m_mapEspionageHistory.end())
	{
		return it->second;
	}
	return 0;
}

void CvPlayer::updateEspionageHistory(int iTurn, int iBestEspionage)
{
	m_mapEspionageHistory[iTurn] = iBestEspionage;
}

// K-Mod. Note, this function is a friend of CvEventReporter, so that it can access the data we need.
// (This saves us from having to use the built-in CyStatistics class)
const CvPlayerRecord* CvPlayer::getPlayerRecord() const
{
	return CvEventReporter::getInstance().m_kStatistics.getPlayerRecord(getID());
}
// K-Mod end

std::string CvPlayer::getScriptData() const
{
	return m_szScriptData;
}

void CvPlayer::setScriptData(std::string szNewValue)
{
	m_szScriptData = szNewValue;
}

const CvString CvPlayer::getPbemEmailAddress() const
{
	return GC.getInitCore().getEmail(getID());
}

void CvPlayer::setPbemEmailAddress(const char* szAddress)
{
	GC.getInitCore().setEmail(getID(), szAddress);
}

const CvString CvPlayer::getSmtpHost() const
{
	return GC.getInitCore().getSmtpHost(getID());
}

void CvPlayer::setSmtpHost(const char* szHost)
{
	GC.getInitCore().setSmtpHost(getID(), szHost);
}

// Protected Functions...

void CvPlayer::doGold()
{
	bool bStrike;
	int iGoldChange;
	int iDisbandUnit;
	int iI;

	CyArgsList argsList;
	argsList.add(getID());
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "doGold", argsList.makeFunctionArgs(), &lResult);
	if (lResult == 1)
	{
		return;
	}

	iGoldChange = calculateGoldRate();

	FAssert(isHuman() || isBarbarian() || ((getGold() + iGoldChange) >= 0) || isAnarchy());

	changeGold(iGoldChange);

	bStrike = false;

	if (getGold() < 0)
	{
		setGold(0);

		if (!isBarbarian() && (getNumCities() > 0))
		{
			bStrike = true;
		}
	}

	if (bStrike)
	{
		setStrike(true);
		changeStrikeTurns(1);

		if (getStrikeTurns() > 1)
		{
			iDisbandUnit = (getStrikeTurns() / 2); // XXX mod?

			for (iI = 0; iI < iDisbandUnit; iI++)
			{
				disbandUnit(true);

				if (calculateGoldRate() >= 0)
				{
					break;
				}
			}
		}
	}
	else
	{
		setStrike(false);
	}
}

void CvPlayer::doResearch()
{
	bool bForceResearchChoice;
	int iOverflowResearch;

	CyArgsList argsList;
	argsList.add(getID());
	long lResult=0;
	gDLL->getPythonIFace()->callFunction(PYGameModule, "doResearch", argsList.makeFunctionArgs(), &lResult);
	if (lResult == 1)
	{
		return;
	}

	if (isResearch() && !isAnarchy()) // K-Mod
	{
		bForceResearchChoice = false;

		if (getCurrentResearch() == NO_TECH && isHuman()) // K-Mod
		{
			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				chooseTech();
			}

			if (GC.getGameINLINE().getElapsedGameTurns() > 4)
			{
				AI_chooseResearch();

				bForceResearchChoice = true;
			}
		}

		TechTypes eCurrentTech = getCurrentResearch();
		if (eCurrentTech == NO_TECH)
		{
			int iOverflow = (100 * calculateResearchRate()) / std::max(1, calculateResearchModifier(eCurrentTech));
			changeOverflowResearch(iOverflow);
		}
		else
		{
			iOverflowResearch = (getOverflowResearch() * calculateResearchModifier(eCurrentTech)) / 100;
			setOverflowResearch(0);
			GET_TEAM(getTeam()).changeResearchProgress(eCurrentTech, std::max(1, calculateResearchRate()) + iOverflowResearch, getID()); // K-Mod (replacing the minimum which use to be in calculateResearchRate)
		}

		if (bForceResearchChoice)
		{
			clearResearchQueue();
		}
	}
}

void CvPlayer::doEspionagePoints()
{
	if (getCommerceRate(COMMERCE_ESPIONAGE) > 0)
	{
		GET_TEAM(getTeam()).changeEspionagePointsEver(getCommerceRate(COMMERCE_ESPIONAGE));

		int iSpending = 0;

		// Divide up Espionage between Teams
		for (int iLoop = 0; iLoop < MAX_CIV_TEAMS; iLoop++)
		{
			if (getTeam() != iLoop)
			{
				if (GET_TEAM((TeamTypes)iLoop).isAlive())
				{
					if (GET_TEAM(getTeam()).isHasMet((TeamTypes)iLoop))
					{
						iSpending = getEspionageSpending((TeamTypes)iLoop);

						if (iSpending > 0)
						{
							GET_TEAM(getTeam()).changeEspionagePointsAgainstTeam((TeamTypes)iLoop, iSpending);
						}
					}
				}
			}
		}
	}	
}

int CvPlayer::getEspionageSpending(TeamTypes eAgainstTeam) const
{
	int iSpendingValue = 0;

	int iTotalPoints = getCommerceRate(COMMERCE_ESPIONAGE);
	int iAvailablePoints = iTotalPoints;

	int iTotalWeight = 0;
	int iBestWeight = 0;

	bool bFoundTeam = false;

	int iLoop;

	// Get sum of all weights to be used later on
	for (iLoop = 0; iLoop < MAX_CIV_TEAMS; iLoop++)
	{
		if (getTeam() != iLoop)
		{
			if (GET_TEAM((TeamTypes)iLoop).isAlive())
			{
				if (GET_TEAM(getTeam()).isHasMet((TeamTypes)iLoop))
				{
					if (iLoop == int(eAgainstTeam))
					{
						bFoundTeam = true;
					}

					int iWeight = getEspionageSpendingWeightAgainstTeam((TeamTypes)iLoop);

					if (iWeight > iBestWeight)
					{
						iBestWeight = iWeight;
					}
					
					iTotalWeight += iWeight;
				}
			}
		}
	}

	// The player requested is not valid
	if (!bFoundTeam)
	{
		return -1;
	}

	// Split up Espionage Point budget based on weights (if any weights have been assigned)
	if (iTotalWeight > 0)
	{
		for (iLoop = 0; iLoop < MAX_CIV_TEAMS; iLoop++)
		{
			if (getTeam() != iLoop)
			{
				if (GET_TEAM((TeamTypes)iLoop).isAlive())
				{
					if (GET_TEAM(getTeam()).isHasMet((TeamTypes)iLoop))
					{
						int iChange = (iTotalPoints * getEspionageSpendingWeightAgainstTeam((TeamTypes)iLoop) / iTotalWeight);
						iAvailablePoints -= iChange;
						
						if (iLoop == int(eAgainstTeam))
						{
							iSpendingValue += iChange;
						}
					}
				}
			}
		}
	}

	// Divide remainder evenly among top Teams
	while (iAvailablePoints > 0)
	{
		for (iLoop = 0; iLoop < MAX_CIV_TEAMS; iLoop++)
		{
			if (getTeam() != iLoop)
			{
				if (GET_TEAM((TeamTypes)iLoop).isAlive())
				{
					if (GET_TEAM(getTeam()).isHasMet((TeamTypes)iLoop))
					{
						if (getEspionageSpendingWeightAgainstTeam((TeamTypes)iLoop) == iBestWeight)
						{
							if (iLoop == int(eAgainstTeam))
							{
								++iSpendingValue;
							}
							--iAvailablePoints;

							if (iAvailablePoints <= 0)
							{
								break;
							}
						}
					}
				}
			}
		}
	}
	
	return iSpendingValue;
}

bool CvPlayer::canDoEspionageMission(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pUnit) const
{
	if (getID() == eTargetPlayer || NO_PLAYER == eTargetPlayer)
	{
		return false;
	}

	if (!GET_PLAYER(eTargetPlayer).isAlive() || !GET_TEAM(getTeam()).isHasMet(GET_PLAYER(eTargetPlayer).getTeam()))
	{
		return false;
	}

	// K-Mod. Bugfix
	if (pUnit && pPlot && !pUnit->canEspionage(pPlot, false))
		return false;
	// K-Mod end

	CvEspionageMissionInfo& kMission = GC.getEspionageMissionInfo(eMission);

	// Need Tech Prereq, if applicable
	if (kMission.getTechPrereq() != NO_TECH)
	{
		if (!GET_TEAM(getTeam()).isHasTech((TechTypes)kMission.getTechPrereq()))
		{
			return false;
		}
	}

	int iCost = getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iExtraData, pUnit);
	if (iCost < 0)
	{
		return false;
	}

	if (NO_PLAYER != eTargetPlayer)
	{
		int iEspionagePoints = GET_TEAM(getTeam()).getEspionagePointsAgainstTeam(GET_PLAYER(eTargetPlayer).getTeam());

		if (iEspionagePoints < iCost)
		{
			return false;
		}

		if (iEspionagePoints <= 0)
		{
			return false;
		}
	}

	return true;
}

int CvPlayer::getEspionageMissionCost(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pSpyUnit) const
{
	int iMissionCost = getEspionageMissionBaseCost(eMission, eTargetPlayer, pPlot, iExtraData, pSpyUnit);

	if (-1 == iMissionCost)
	{
		return -1;
	}

	// Multiply cost of mission * number of team members
	iMissionCost *= GET_TEAM(getTeam()).getNumMembers();

	iMissionCost *= getEspionageMissionCostModifier(eMission, eTargetPlayer, pPlot, iExtraData, pSpyUnit);
	iMissionCost /= 100;

	return std::max(0, iMissionCost);
}

int CvPlayer::getEspionageMissionBaseCost(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pSpyUnit) const
{
	if (eMission == NO_ESPIONAGEMISSION) // K-Mod
		return -1;

	CvEspionageMissionInfo& kMission = GC.getEspionageMissionInfo(eMission);
	int iBaseMissionCost = kMission.getCost();

	// -1 means this mission is disabled
	if (iBaseMissionCost == -1)
	{
		return -1;
	}

	CvCity* pCity = NULL;
	if (NULL != pPlot)
	{
		pCity = pPlot->getPlotCity();
	}

	if (kMission.isSelectPlot())
	{
		if (NULL == pPlot)
		{
			return -1;
		}

		if (!pPlot->isRevealed(getTeam(), false))
		{
			return -1;
		}
	}

	if (NULL == pCity && kMission.isTargetsCity())
	{
		return -1;
	}

	int iMissionCost = -1;

	if (kMission.getStealTreasuryTypes() > 0)
	{
		// Steal Treasury
		//int iNumTotalGold = (GET_PLAYER(eTargetPlayer).getGold() * kMission.getStealTreasuryTypes()) / 100;
		int iNumTotalGold = 0;

		if (NULL != pCity)
		{
			/* iNumTotalGold *= pCity->getPopulation();
			iNumTotalGold /= std::max(1, GET_PLAYER(eTargetPlayer).getTotalPopulation()); */
			// K-Mod
			iNumTotalGold = getEspionageGoldQuantity(eMission, eTargetPlayer, pCity);
		}

		if (iNumTotalGold > 0)
		{
			iMissionCost = (iBaseMissionCost * iNumTotalGold) / 100;
		}
	}
	else if (kMission.getBuyTechCostFactor() > 0)
	{
		// Buy (Steal) Tech
		TechTypes eTech = (TechTypes)iExtraData;
		int iProdCost = MAX_INT;

		if (NO_TECH == eTech)
		{
			for (int iTech = 0; iTech < GC.getNumTechInfos(); ++iTech)
			{
				if (canStealTech(eTargetPlayer, (TechTypes)iTech))
				{
					int iCost = GET_TEAM(getTeam()).getResearchCost((TechTypes)iTech);
					// Civ4 Reimagined: Apply research modifier to tech stealing
					iCost *= 100;
					iCost /= calculateResearchModifier((TechTypes)iTech);

					if (!CvWString(GC.getTechInfo((TechTypes)iTech).getHelp()).empty())
					{
						iCost *= 100 - GC.getDefineINT("EUREKA_TECH_BOOST_PERCENTAGE");
						iCost /= 100;
					}

					if (iCost < iProdCost)
					{
						iProdCost = iCost;
						eTech = (TechTypes)iTech;
					}
				}
			}
		}
		else
		{
			iProdCost = GET_TEAM(getTeam()).getResearchCost(eTech);
			// Civ4 Reimagined: Apply research modifier to tech stealing
			iProdCost *= 100;
			iProdCost /= calculateResearchModifier(eTech);

			if (!CvWString(GC.getTechInfo(eTech).getHelp()).empty())
			{
				iProdCost *= 100 - GC.getDefineINT("EUREKA_TECH_BOOST_PERCENTAGE");
				iProdCost /= 100;
			}
		}

		if (NO_TECH != eTech)
		{
			if (canStealTech(eTargetPlayer, eTech))
			{
				iMissionCost = iBaseMissionCost + ((100 + kMission.getBuyTechCostFactor()) * iProdCost) / 100;
			}
		}
	}
	else if (kMission.getSwitchCivicCostFactor() > 0)
	{
		// Switch Civics
		CivicTypes eCivic = (CivicTypes)iExtraData;

		if (NO_CIVIC == eCivic)
		{
			for (int iCivic = 0; iCivic < GC.getNumCivicInfos(); ++iCivic)
			{
				if (canForceCivics(eTargetPlayer, (CivicTypes)iCivic))
				{
					eCivic = (CivicTypes)iCivic;
					break;
				}
			}
		}

		if (NO_CIVIC != eCivic)
		{
			if (canForceCivics(eTargetPlayer, eCivic))
			{
				//iMissionCost = iBaseMissionCost + (kMission.getSwitchCivicCostFactor() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getAnarchyPercent()) / 10000;
				// K-Mod
				iMissionCost = iBaseMissionCost + (kMission.getSwitchCivicCostFactor() * GET_PLAYER(eTargetPlayer).getTotalPopulation() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getAnarchyPercent()) / 10000;

				// Civ4 Reimagined
				iMissionCost *= GET_PLAYER(eTargetPlayer).getIdeologyChangeCivicModifier(eCivic);
				iMissionCost /= 100;
			}
		}
	}
	else if (kMission.getSwitchReligionCostFactor() > 0)
	{
		// Switch Religions
		ReligionTypes eReligion = (ReligionTypes)iExtraData;

		if (NO_RELIGION == eReligion)
		{
			for (int iReligion = 0; iReligion < GC.getNumReligionInfos(); ++iReligion)
			{
				if (canForceReligion(eTargetPlayer, (ReligionTypes)iReligion))
				{
					eReligion = (ReligionTypes)iReligion;
					break;
				}
			}
		}

		if (NO_RELIGION != eReligion)
		{
			if (canForceReligion(eTargetPlayer, eReligion))
			{
				//iMissionCost = iBaseMissionCost + (kMission.getSwitchReligionCostFactor() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getAnarchyPercent()) / 10000;
				// K-Mod
				iMissionCost = iBaseMissionCost + (kMission.getSwitchReligionCostFactor() * GET_PLAYER(eTargetPlayer).getTotalPopulation() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getAnarchyPercent()) / 10000;
				ReligionTypes eCurrentReligion = GET_PLAYER(eTargetPlayer).getStateReligion();
				// amplify the mission cost if we are trying to switch to a minority religion.
				if (eCurrentReligion != NO_RELIGION)
				{
					// maybe getReligionPopulation would be slightly better, but it's a bit slower.
					int iCurrent = GET_PLAYER(eTargetPlayer).getHasReligionCount(eCurrentReligion);
					int iNew = GET_PLAYER(eTargetPlayer).getHasReligionCount(eReligion);
					int iCitiesTarget = GC.getWorldInfo(GC.getMapINLINE().getWorldSize()).getTargetNumCities();
					FAssert(iCurrent > 0 && iNew > 0);
					iMissionCost *= std::max(iCurrent, iNew) + iCitiesTarget;
					iMissionCost /= iNew + iCitiesTarget;
				}
				// K-Mod end
			}
		}
	}
	else if (kMission.getDestroyUnitCostFactor() > 0)
	{
		// Destroys Unit
		CvUnit* pUnit = GET_PLAYER(eTargetPlayer).getUnit(iExtraData);
		int iCost = MAX_INT;

		if (NULL == pUnit)
		{
			if (NULL != pPlot)
			{
				CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = pPlot->nextUnitNode(pUnitNode);

					if (canSpyDestroyUnit(eTargetPlayer, *pLoopUnit))
					{
						int iValue = getProductionNeeded(pLoopUnit->getUnitType());
						if (iValue < iCost)
						{
							iCost = iValue;
							pUnit = pLoopUnit;
						}
					}
				}

			}
		}
		else
		{
			iCost = getProductionNeeded(pUnit->getUnitType());
		}

		if (NULL != pUnit)
		{
			if (canSpyDestroyUnit(eTargetPlayer, *pUnit))
			{
				iMissionCost = iBaseMissionCost + ((100 + kMission.getDestroyUnitCostFactor()) * iCost) / 100;
			}
		}
	}	
	else if (kMission.getDestroyProjectCostFactor() > 0)
	{
		ProjectTypes eProject = (ProjectTypes) iExtraData;
		int iCost = MAX_INT;

		if (NO_PROJECT == eProject)
		{
			for (int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
			{
				if (canSpyDestroyProject(eTargetPlayer, (ProjectTypes)iProject))
				{
					int iValue = getProductionNeeded((ProjectTypes)iProject);

					if (iValue < iCost)
					{
						iCost = iValue;
						eProject = (ProjectTypes)iProject;
					}
				}
			}
		}
		else
		{
			iCost = getProductionNeeded(eProject);
		}

		if (NO_PROJECT != eProject)
		{
			if (canSpyDestroyProject(eTargetPlayer, eProject))
			{
				iMissionCost = iBaseMissionCost + ((100 + kMission.getDestroyProjectCostFactor()) * iCost) / 100;
			}
		}
	}
	else if (kMission.getDestroyProductionCostFactor() > 0)
	{
		FAssert(NULL != pCity);
		if (NULL != pCity)
		{
			iMissionCost = iBaseMissionCost + ((100 + kMission.getDestroyProductionCostFactor()) * pCity->getProduction()) / 100;
		}
	}
	else if (kMission.getBuyUnitCostFactor() > 0)
	{
		// Buy Unit
		CvUnit* pUnit = GET_PLAYER(eTargetPlayer).getUnit(iExtraData);
		int iCost = MAX_INT;

		if (NULL == pUnit)
		{
			if (NULL != pPlot)
			{
				CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();

				while (pUnitNode != NULL)
				{
					CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
					pUnitNode = pPlot->nextUnitNode(pUnitNode);

					if (canSpyBribeUnit(eTargetPlayer, *pLoopUnit))
					{
						int iValue = getProductionNeeded(pLoopUnit->getUnitType());
						if (iValue < iCost)
						{
							iCost = iValue;
							pUnit = pLoopUnit;
						}
					}
				}

			}
		}
		else
		{
			iCost = getProductionNeeded(pUnit->getUnitType());
		}


		if (NULL != pUnit)
		{
			if (canSpyBribeUnit(eTargetPlayer, *pUnit))
			{
				iMissionCost = iBaseMissionCost + ((100 + kMission.getBuyUnitCostFactor()) * iCost) / 100;
			}
		}
	}
	else if (kMission.getDestroyBuildingCostFactor() > 0)
	{
		BuildingTypes eBuilding = (BuildingTypes) iExtraData;
		int iCost = MAX_INT;

		if (NO_BUILDING == eBuilding)
		{
			for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
			{
				// Civ4 Reimagined TODO: cannot destroy obsolete buildings?
				if (NULL != pCity && pCity->getNumRealBuilding((BuildingTypes)iBuilding) > 0)
				{
					if (canSpyDestroyBuilding(eTargetPlayer, (BuildingTypes)iBuilding))
					{
						int iValue = getProductionNeeded((BuildingTypes)iBuilding);

						if (iValue < iCost)
						{
							iCost = iValue;
							eBuilding = (BuildingTypes)iBuilding;
						}
					}
				}
			}
		}
		else
		{
			iCost = getProductionNeeded(eBuilding);
		}

		if (NO_BUILDING != eBuilding)
		{
			if (NULL != pCity && pCity->getNumRealBuilding(eBuilding) > 0)
			{
				if (canSpyDestroyBuilding(eTargetPlayer, eBuilding))
				{
					iMissionCost = iBaseMissionCost + ((100 + kMission.getDestroyBuildingCostFactor()) * iCost) / 100;
				}
			}
		}
	}
	else if (kMission.getBuyCityCostFactor() > 0)
	{
		// Buy City
		if (NULL != pCity)
		{
			iMissionCost = iBaseMissionCost + (kMission.getBuyCityCostFactor() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent()) / 10000;
		}
	}
	else if (kMission.getCityInsertCultureCostFactor() > 0)
	{
		// Insert Culture into City
		if (NULL != pPlot && pPlot->getCulture(getID()) > 0)
		{
			int iCultureAmount = kMission.getCityInsertCultureAmountFactor() *  pCity->countTotalCultureTimes100();
			iCultureAmount /= 10000;
			iCultureAmount = std::max(1, iCultureAmount);
			iMissionCost = iBaseMissionCost + (kMission.getCityInsertCultureCostFactor() * iCultureAmount) / 100;
		}
	}
	else if (kMission.isDestroyImprovement())
	{
		if (NULL != pPlot && !pPlot->isCity())
		{
			if (pPlot->getImprovementType() != NO_IMPROVEMENT || pPlot->getRouteType() != NO_ROUTE)
			{
				iMissionCost = (iBaseMissionCost * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getBuildPercent()) / 100;
			}
		}
	}
	else if (kMission.getCityPoisonWaterCounter() > 0)
	{
		FAssert(NULL != pCity);
		// Cannot poison a city's water supply if it's already poisoned (value is negative when active)
		if (NULL != pCity && pCity->getEspionageHealthCounter() <= 0)
		{
			iMissionCost = iBaseMissionCost;
		}
	}

	// Make city unhappy
	else if (kMission.getCityUnhappinessCounter() > 0)
	{
		FAssert(NULL != pCity);
		// Cannot make a city unhappy if you've already done it (value is negative when active)
		if (NULL != pCity && pCity->getEspionageHappinessCounter() <= 0)
		{
			iMissionCost = iBaseMissionCost;
		}
	}

	// Make city Revolt
	else if (kMission.getCityRevoltCounter() > 0)
	{
		FAssert(NULL != pCity);
		// Cannot make a city revolt if it's already revolting
		if (NULL != pCity && pCity->getOccupationTimer() == 0)
		{
			iMissionCost = iBaseMissionCost;
		}
	}
	else if (kMission.getCounterespionageMod() > 0)
	{
		if (GET_TEAM(getTeam()).getCounterespionageTurnsLeftAgainstTeam(GET_PLAYER(eTargetPlayer).getTeam()) <= 0)
		{
			iMissionCost = (iBaseMissionCost * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getResearchPercent()) / 100;
		}
	}
	else if (kMission.getPlayerAnarchyCounter() > 0)
	{
		// Player anarchy timer: can't add more turns of anarchy to player already in the midst of it
		if (!GET_PLAYER(eTargetPlayer).isAnarchy())
		{
			iMissionCost = (iBaseMissionCost * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getAnarchyPercent()) / 100;
		}
	}
	else if (kMission.isPassive())
	{
		iMissionCost = (iBaseMissionCost * (100 + GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()).getEspionagePointsAgainstTeam(getTeam()))) / 100;
	}
	else
	{
		iMissionCost = (iBaseMissionCost * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getResearchPercent()) / 100;
	}

	if (iMissionCost < 0)
	{
		return -1;
	}

	return iMissionCost;
}


int CvPlayer::getEspionageMissionCostModifier(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvPlot* pPlot, int iExtraData, const CvUnit* pSpyUnit) const
{
	// K-Mod. I've altered this function to give a generic answer when NO_ESPIONAGEMISSION is passed.

	//CvEspionageMissionInfo& kMission = GC.getEspionageMissionInfo(eMission);
	int iModifier = 100;

	CvCity* pCity = NULL;
	if (NULL != pPlot)
	{
		pCity = pPlot->getPlotCity();
	}

	if (NO_PLAYER == eTargetPlayer)
	{
		eTargetPlayer = getID();
	}

	const CvTeam& kTargetTeam = GET_TEAM(GET_PLAYER(eTargetPlayer).getTeam()); // (moved from the bottom of the function)

	//if (pCity != NULL && kMission.isTargetsCity())
	if (pCity != NULL && (eMission == NO_ESPIONAGEMISSION || GC.getEspionageMissionInfo(eMission).isTargetsCity()))
	{
		// City Population
		iModifier *= 100 + (GC.getDefineINT("ESPIONAGE_CITY_POP_EACH_MOD") * (pCity->getPopulation() - 1));
		iModifier /= 100;

		// Trade Route
		if (pCity->isTradeRoute(getID()))
		{
			iModifier *= 100 + GC.getDefineINT("ESPIONAGE_CITY_TRADE_ROUTE_MOD");
			iModifier /= 100;
		}

		ReligionTypes eReligion = getStateReligion();
		if (NO_RELIGION != eReligion)
		{
			int iReligionModifier = 0;

			// City has Your State Religion
			if (pCity->isHasReligion(eReligion))
			{
				// Changed by Civ4 Reimagined
				if (hasHolyCity(eReligion))
				{
					iReligionModifier += GC.getDefineINT("ESPIONAGE_CITY_HOLY_CITY_MOD");
				}
				else
				{
					iReligionModifier += GC.getDefineINT("ESPIONAGE_CITY_RELIGION_STATE_MOD");
				}
			}

			iModifier *= 100 + iReligionModifier;
			iModifier /= 100;

		}
		
		// Civ4 Reimagined
		// Corporation
		int iCorporationModifier = 0;
		
		for (int iI = 0; iI < GC.getNumCorporationInfos(); ++iI)
		{
			if (pCity->isHasCorporation((CorporationTypes)iI))
			{
				if (hasHeadquarters((CorporationTypes)iI))
				{
					iCorporationModifier += GC.getDefineINT("ESPIONAGE_CITY_CORPORATION_MOD");
				}
			}
		}
		
		iModifier *= 100 + iCorporationModifier;
		iModifier /= 100;

		// City's culture affects cost
		/* original bts code
		iModifier *= 100 - (pCity->getCultureTimes100(getID()) * GC.getDefineINT("ESPIONAGE_CULTURE_MULTIPLIER_MOD")) / std::max(1, pCity->getCultureTimes100(eTargetPlayer) + pCity->getCultureTimes100(getID()));
		iModifier /= 100; */

		iModifier *= 100 + pCity->getEspionageDefenseModifier();
		iModifier /= 100;
	}

	if (pPlot != NULL)
	{
		// K-Mod. Culture Mod. (Based on plot culture rather than city culture.)
		if (eMission == NO_ESPIONAGEMISSION || GC.getEspionageMissionInfo(eMission).isSelectPlot() || GC.getEspionageMissionInfo(eMission).isTargetsCity())
		{
			iModifier *= 100 - (pPlot->getCulture(getID()) * GC.getDefineINT("ESPIONAGE_CULTURE_MULTIPLIER_MOD")) / std::max(1, pPlot->getCulture(eTargetPlayer) + pPlot->getCulture(getID()));
			iModifier /= 100;
		}
		// K-Mod end

		// Removed by Civ4 Reimagined
		// Distance mod
		/*
		int iDistance = GC.getMap().maxPlotDistance();

		CvCity* pOurCapital = getCapitalCity();
		if (NULL != pOurCapital)
		{
			//if (kMission.isSelectPlot() || kMission.isTargetsCity())
			if (eMission == NO_ESPIONAGEMISSION || GC.getEspionageMissionInfo(eMission).isSelectPlot() || GC.getEspionageMissionInfo(eMission).isTargetsCity())
			{
				iDistance = plotDistance(pOurCapital->getX_INLINE(), pOurCapital->getY_INLINE(), pPlot->getX_INLINE(), pPlot->getY_INLINE());
			}
			else
			{
				CvCity* pTheirCapital = GET_PLAYER(eTargetPlayer).getCapitalCity();
				if (NULL != pTheirCapital)
				{
					iDistance = plotDistance(pOurCapital->getX_INLINE(), pOurCapital->getY_INLINE(), pTheirCapital->getX_INLINE(), pTheirCapital->getY_INLINE());
				}
			}
		}

		iModifier *= (iDistance + GC.getMapINLINE().maxPlotDistance()) * GC.getDefineINT("ESPIONAGE_DISTANCE_MULTIPLIER_MOD") / GC.getMapINLINE().maxPlotDistance();
		iModifier /= 100;
		*/
	}

	// Civ4 Reimagined
	if (GC.getGameINLINE().areIdeologiesEnabled() && getIdeology() != GET_PLAYER(eTargetPlayer).getIdeology())
	{
		const int iIdeologyInfluence = GET_PLAYER(eTargetPlayer).getIdeologyInfluence(getIdeology());
		if (iIdeologyInfluence > 0)
		{
			iModifier *= 100 + (iIdeologyInfluence * GC.getDefineINT("ESPIONAGE_IDEOLOGY_INFLUENCE_MOD"));
			iModifier /= 100;
		}
	}

	// Spy presence mission cost alteration
	if (NULL != pSpyUnit)
	{
		iModifier *= 100 - (pSpyUnit->getFortifyTurns() * GC.getDefineINT("ESPIONAGE_EACH_TURN_UNIT_COST_DECREASE"));
		iModifier /= 100;
	}

	// My points VS. Your points to mod cost
	/* original bts code
	int iTargetPoints = kTargetTeam.getEspionagePointsEver();
	int iOurPoints = GET_TEAM(getTeam()).getEspionagePointsEver();
		iModifier *= (GC.getDefineINT("ESPIONAGE_SPENDING_MULTIPLIER") * (2 * iTargetPoints + iOurPoints)) / std::max(1, iTargetPoints + 2 * iOurPoints);
		iModifier /= 100;
	} */
	// K-Mod. use the dedicated function that exists for this modifier, for consistency.
	iModifier *= ::getEspionageModifier(getTeam(), kTargetTeam.getID());
	iModifier /= 100;
	// K-Mod end
	
	// Counterespionage Mission Mod
	/* if (kTargetTeam.getCounterespionageModAgainstTeam(getTeam()) > 0)
	{
		iModifier *= kTargetTeam.getCounterespionageModAgainstTeam(getTeam());
		iModifier /= 100;
	} */
	// K-Mod
	iModifier *= 100 + std::max(-100, kTargetTeam.getCounterespionageModAgainstTeam(getTeam()));
	iModifier /= 100;
	// K-Mod end

	return iModifier;
}


bool CvPlayer::doEspionageMission(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, CvPlot* pPlot, int iExtraData, CvUnit* pSpyUnit)
{
	if (!canDoEspionageMission(eMission, eTargetPlayer, pPlot, iExtraData, pSpyUnit))
	{
		return false;
	}

	TeamTypes eTargetTeam = NO_TEAM;
	if (NO_PLAYER != eTargetPlayer)
	{
		eTargetTeam = GET_PLAYER(eTargetPlayer).getTeam();
	}

	CvEspionageMissionInfo& kMission = GC.getEspionageMissionInfo(eMission);

	bool bSomethingHappened = false;
	bool bShowExplosion = false;
	CvWString szBuffer;
	int iMissionCost = getEspionageMissionCost(eMission, eTargetPlayer, pPlot, iExtraData, pSpyUnit);

	
	//////////////////////////////
	// Destroy Improvement

	if (kMission.isDestroyImprovement())
	{
		if (NULL != pPlot)
		{
			// Blow it up
			if (pPlot->getImprovementType() != NO_IMPROVEMENT)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED", GC.getImprovementInfo(pPlot->getImprovementType()).getDescription()).GetCString();
				pPlot->setImprovementType((ImprovementTypes)(GC.getImprovementInfo(pPlot->getImprovementType()).getImprovementPillage()));
				bSomethingHappened = true;
			}
			else if (pPlot->getRouteType() != NO_ROUTE)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED", GC.getRouteInfo(pPlot->getRouteType()).getDescription()).GetCString();
				pPlot->setRouteType(NO_ROUTE, true);
				bSomethingHappened = true;
			}

			if (bSomethingHappened)
			{
				bShowExplosion = true;
			}
		}		
	}

	//////////////////////////////
	// Destroy Building

	if (kMission.getDestroyBuildingCostFactor() > 0)
	{
		BuildingTypes eTargetBuilding = (BuildingTypes)iExtraData;

		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED_IN", GC.getBuildingInfo(eTargetBuilding).getDescription(), pCity->getNameKey()).GetCString();
				pCity->setNumRealBuilding(eTargetBuilding, pCity->getNumRealBuilding(eTargetBuilding) - 1);

				bSomethingHappened = true;
				bShowExplosion = true;
				// K-Mod
				if (!isHuman() || pCity->isProductionAutomated())
					pCity->AI_setChooseProductionDirty(true);
				// K-Mod end
			}
		}
	}

	//////////////////////////////
	// Destroy Project

	if (kMission.getDestroyProjectCostFactor() > 0)
	{
		ProjectTypes eTargetProject = (ProjectTypes)iExtraData;

		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED_IN", GC.getProjectInfo(eTargetProject).getDescription(), pCity->getNameKey()).GetCString();
				GET_TEAM(eTargetTeam).changeProjectCount(eTargetProject, -1);

				bSomethingHappened = true;
				bShowExplosion = true;
			}
		}
	}

	//////////////////////////////
	// Destroy Production

	if (kMission.getDestroyProductionCostFactor() > 0)
	{
		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_PRODUCTION_DESTROYED_IN", pCity->getProductionName(), pCity->getNameKey());
				pCity->setProduction(0);

				bSomethingHappened = true;
				bShowExplosion = true;
				// K-Mod
				if (!isHuman()) // not for automated cities
					pCity->AI_setChooseProductionDirty(true);
				// K-Mod end
			}
		}
	}

	//////////////////////////////
	// Destroy Unit

	if (kMission.getDestroyUnitCostFactor() > 0)
	{
		if (NO_PLAYER != eTargetPlayer)
		{
			int iTargetUnitID = iExtraData;

			CvUnit* pUnit = GET_PLAYER(eTargetPlayer).getUnit(iTargetUnitID);

			if (NULL != pUnit)
			{
				FAssert(pUnit->plot() == pPlot);
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SOMETHING_DESTROYED", pUnit->getNameKey()).GetCString();
				pUnit->kill(false, getID());

				bSomethingHappened = true;
				bShowExplosion = true;
			}
		}
	}

	//////////////////////////////
	// Buy Unit

	if (kMission.getBuyUnitCostFactor() > 0)
	{
		if (NO_PLAYER != eTargetPlayer)
		{
			int iTargetUnitID = iExtraData;

			CvUnit* pUnit = GET_PLAYER(eTargetPlayer).getUnit(iTargetUnitID);

			if (NULL != pUnit)
			{
				FAssert(pUnit->plot() == pPlot);

				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_UNIT_BOUGHT", pUnit->getNameKey()).GetCString();

				UnitTypes eUnitType = pUnit->getUnitType();
				int iX = pUnit->getX_INLINE();
				int iY = pUnit->getY_INLINE();
				pUnit->kill(false, getID());
				initUnit(eUnitType, iX, iY, NO_UNITAI);

				bSomethingHappened = true;
			}
		}
	}

	//////////////////////////////
	// Buy City

	if (kMission.getBuyCityCostFactor() > 0)
	{
		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_BOUGHT", pCity->getNameKey()).GetCString();
				acquireCity(pCity, false, true, true);

				bSomethingHappened = true;
			}
		}
	}

	//////////////////////////////
	// Insert Culture into City

	if (kMission.getCityInsertCultureCostFactor() > 0)
	{
		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_CULTURE_INSERTED", pCity->getNameKey()).GetCString();				

				/* original bts code
				int iCultureAmount = kMission.getCityInsertCultureAmountFactor() * pCity->countTotalCultureTimes100();
				iCultureAmount /= 10000;
				iCultureAmount = std::max(1, iCultureAmount);

				int iNumTurnsApplied = (GC.getDefineINT("GREAT_WORKS_CULTURE_TURNS") * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getUnitGreatWorkPercent()) / 100;

				for (int i = 0; i < iNumTurnsApplied; ++i)
				{
					pCity->changeCulture(getID(), iCultureAmount / iNumTurnsApplied, true, true);
				}

				if (iNumTurnsApplied > 0)
				{
					pCity->changeCulture(getID(), iCultureAmount % iNumTurnsApplied, false, true);
				} */

				// K-Mod. apply culture in one hit. We don't need fake 'free city culture' anymore.
				int iCultureTimes100 = std::max(1, kMission.getCityInsertCultureAmountFactor() * pCity->countTotalCultureTimes100() / 100);
				
				iCultureTimes100 *= 2; //Civ4 Reimagined
				
				//pCity->changeCultureTimes100(getID(), iCultureTimes100, true, true);
				pCity->doPlotCultureTimes100(true, getID(), iCultureTimes100, false); // plot culture only.
				// K-Mod end

				bSomethingHappened = true;
			}

		}
	}

	//////////////////////////////
	// Poison City's Water Supply

	if (kMission.getCityPoisonWaterCounter() > 0)
	{
		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_POISONED", pCity->getNameKey()).GetCString();
				pCity->changeEspionageHealthCounter(kMission.getCityPoisonWaterCounter());

				// Civ4 Reimagined
				if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_PAPER")))
				{
					GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_PAPER"), getID(), true);
				}

				bShowExplosion = true;
				bSomethingHappened = true;
			}
		}
	}

	//////////////////////////////
	// Make city Unhappy

	if (kMission.getCityUnhappinessCounter() > 0)
	{
		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_UNHAPPY", pCity->getNameKey()).GetCString();
				pCity->changeEspionageHappinessCounter(kMission.getCityUnhappinessCounter());

				bShowExplosion = true;
				bSomethingHappened = true;
			}
		}
	}

	//////////////////////////////
	// Make city Revolt

	if (kMission.getCityRevoltCounter() > 0)
	{
		if (NULL != pPlot)
		{
			CvCity* pCity = pPlot->getPlotCity();

			if (NULL != pCity)
			{
				szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_CITY_REVOLT", pCity->getNameKey()).GetCString();
				pCity->changeCultureUpdateTimer(kMission.getCityRevoltCounter());
				pCity->changeOccupationTimer(kMission.getCityRevoltCounter());

				bSomethingHappened = true;
				bShowExplosion = true;

				if (gUnitLogLevel >= 2 && !isHuman())
				{
					logBBAI("      Spy for player %d (%S) causes revolt in %S, owned by %S (%d)", getID(), getCivilizationDescription(0), pCity->getName().GetCString(), GET_PLAYER(pCity->getOwner()).getCivilizationDescription(0), pCity->getOwner() );
				}
			}
		}
	}

	//////////////////////////////
	// Steal Treasury

	if (kMission.getStealTreasuryTypes() > 0)
	{
		if (NO_PLAYER != eTargetPlayer)
		{
			//int iNumTotalGold = (GET_PLAYER(eTargetPlayer).getGold() * kMission.getStealTreasuryTypes()) / 100;
			int iNumTotalGold = 0;

			if (NULL != pPlot)
			{
				CvCity* pCity = pPlot->getPlotCity();

				if (NULL != pCity)
				{
					/* iNumTotalGold *= pCity->getPopulation();
					iNumTotalGold /= std::max(1, GET_PLAYER(eTargetPlayer).getTotalPopulation()); */
					// K-Mod. Make stealing gold still worthwhile against large civs.
					iNumTotalGold = getEspionageGoldQuantity(eMission, eTargetPlayer, pCity);
				}
			}
			
			szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_STEAL_TREASURY", iNumTotalGold).GetCString();
			changeGold(iNumTotalGold);
			if (NO_PLAYER != eTargetPlayer)
			{
				GET_PLAYER(eTargetPlayer).changeGold(-iNumTotalGold);

				// Civ4 Reimagined
				if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_GUILDS")))
				{
					GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_GUILDS"), getID(), true);
				}
			}
			
			bSomethingHappened = true;
		}
	}

	//////////////////////////////
	// Buy (Steal) Tech

	if (kMission.getBuyTechCostFactor() > 0)
	{
		int iTech = iExtraData;

		szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_TECH_BOUGHT", GC.getTechInfo((TechTypes) iTech).getDescription()).GetCString();
		GET_TEAM(getTeam()).setHasTech((TechTypes) iTech, true, getID(), false, true);
		GET_TEAM(getTeam()).setNoTradeTech((TechTypes)iTech, true);
		
		if (gPlayerLogLevel > 2)
			logBBAI("Spy from player %d (%S) stole tech %S", getID(), getCivilizationDescription(0), GC.getTechInfo((TechTypes) iTech).getDescription());

		// Civ4 Reimagined
		if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_FIBER_OPTICS")))
		{
			if (GC.getTechInfo((TechTypes)iTech).getEra() == (EraTypes)GC.getInfoTypeForString("ERA_MODERN"))
			{
				GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_FIBER_OPTICS"), getID(), true);
			}
		}

		bSomethingHappened = true;
	}

	//////////////////////////////
	// Switch Civic

	if (kMission.getSwitchCivicCostFactor() > 0)
	{
		if (NO_PLAYER != eTargetPlayer)
		{
			int iCivic = iExtraData;

			szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SWITCH_CIVIC", GC.getCivicInfo((CivicTypes) iCivic).getDescription()).GetCString();
			GET_PLAYER(eTargetPlayer).setCivics((CivicOptionTypes) GC.getCivicInfo((CivicTypes) iCivic).getCivicOptionType(), (CivicTypes) iCivic);
			GET_PLAYER(eTargetPlayer).setRevolutionTimer(std::max(1, ((100 + GET_PLAYER(eTargetPlayer).getAnarchyModifier()) * GC.getDefineINT("MIN_REVOLUTION_TURNS")) / 100));
			bSomethingHappened = true;
		}
	}

	//////////////////////////////
	// Switch Religion

	if (kMission.getSwitchReligionCostFactor() > 0)
	{
		if (NO_PLAYER != eTargetPlayer)
		{
			int iReligion = iExtraData;

			szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_SWITCH_RELIGION", GC.getReligionInfo((ReligionTypes) iReligion).getDescription()).GetCString();
			GET_PLAYER(eTargetPlayer).setLastStateReligion((ReligionTypes) iReligion);
			GET_PLAYER(eTargetPlayer).setConversionTimer(std::max(1, ((100 + GET_PLAYER(eTargetPlayer).getAnarchyModifier()) * GC.getDefineINT("MIN_CONVERSION_TURNS")) / 100));
			bSomethingHappened = true;
		}
	}

	//////////////////////////////
	// Player Anarchy

	if (kMission.getPlayerAnarchyCounter() > 0)
	{
		if (NO_PLAYER != eTargetPlayer)
		{
			int iTurns = (kMission.getPlayerAnarchyCounter() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getAnarchyPercent()) / 100;
			szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_PLAYER_ANARCHY").GetCString();
			GET_PLAYER(eTargetPlayer).changeAnarchyTurns(iTurns);

			bSomethingHappened = true;
		}
	}

	//////////////////////////////
	// Counterespionage

	if (kMission.getCounterespionageNumTurns() > 0 && kMission.getCounterespionageMod() > 0)
	{
		szBuffer = gDLL->getText("TXT_KEY_ESPIONAGE_TARGET_COUNTERESPIONAGE").GetCString();

		if (NO_TEAM != eTargetTeam)
		{
			int iTurns = (kMission.getCounterespionageNumTurns() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getResearchPercent()) / 100;
			GET_TEAM(getTeam()).changeCounterespionageTurnsLeftAgainstTeam(eTargetTeam, iTurns);
			GET_TEAM(getTeam()).changeCounterespionageModAgainstTeam(eTargetTeam, kMission.getCounterespionageMod());
	
			bSomethingHappened = true;

		}		
	}

	int iHave = 0;
	if (NO_TEAM != eTargetTeam)
	{
		iHave = GET_TEAM(getTeam()).getEspionagePointsAgainstTeam(eTargetTeam);

		if (bSomethingHappened)
		{
			GET_TEAM(getTeam()).changeEspionagePointsAgainstTeam(eTargetTeam, -iMissionCost);
		}
	}

	if (bShowExplosion)
	{
		if (pPlot)
		{
			if (pPlot->isVisible(GC.getGame().getActiveTeam(), false))
			{
				EffectTypes eEffect = GC.getEntityEventInfo(GC.getMissionInfo(MISSION_BOMBARD).getEntityEvent()).getEffectType();
				gDLL->getEngineIFace()->TriggerEffect(eEffect, pPlot->getPoint(), (float)(GC.getASyncRand().get(360)));
				gDLL->getInterfaceIFace()->playGeneralSound("AS3D_UN_CITY_EXPLOSION", pPlot->getPoint());
			}
		}
	}

	if (bSomethingHappened)
	{
		int iX = -1;
		int iY = -1;
		if (NULL != pPlot)
		{
			iX = pPlot->getX_INLINE();
			iY = pPlot->getY_INLINE();
		}

		gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), gDLL->getText("TXT_KEY_ESPIONAGE_MISSION_PERFORMED"), "AS2D_POSITIVE_DINK", MESSAGE_TYPE_INFO, ARTFILEMGR.getInterfaceArtInfo("ESPIONAGE_BUTTON")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"), iX, iY, true, true);
	}
	else if (getID() == GC.getGameINLINE().getActivePlayer())
	{
		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_TEXT);
		if (iHave < iMissionCost)
		{
			pInfo->setText(gDLL->getText("TXT_KEY_ESPIONAGE_TOO_EXPENSIVE", iMissionCost, iHave));
		}
		else
		{
			pInfo->setText(gDLL->getText("TXT_KEY_ESPIONAGE_CANNOT_DO_MISSION"));
		}

		addPopup(pInfo);
	}

	if (bSomethingHappened && !szBuffer.empty())
	{
		int iX = -1;
		int iY = -1;
		if (NULL != pPlot)
		{
			iX = pPlot->getX_INLINE();
			iY = pPlot->getY_INLINE();
		}

		if (NO_PLAYER != eTargetPlayer)
		{
			gDLL->getInterfaceIFace()->addHumanMessage(eTargetPlayer, true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_DEAL_CANCELLED", MESSAGE_TYPE_INFO, ARTFILEMGR.getInterfaceArtInfo("ESPIONAGE_BUTTON")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), iX, iY, true, true);
		}
	}

	return bSomethingHappened;
}

int CvPlayer::getEspionageSpendingWeightAgainstTeam(TeamTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiEspionageSpendingWeightAgainstTeam[eIndex];
}

void CvPlayer::setEspionageSpendingWeightAgainstTeam(TeamTypes eIndex, int iValue)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");
	//FAssert(iValue >= 0);

	iValue = std::min(std::max(0, iValue), 99);

	if (iValue != getEspionageSpendingWeightAgainstTeam(eIndex))
	{
		m_aiEspionageSpendingWeightAgainstTeam[eIndex] = iValue;

		gDLL->getInterfaceIFace()->setDirty(Espionage_Advisor_DIRTY_BIT, true);
	}
}

void CvPlayer::changeEspionageSpendingWeightAgainstTeam(TeamTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < MAX_TEAMS, "eIndex is expected to be within maximum bounds (invalid Index)");

	setEspionageSpendingWeightAgainstTeam(eIndex, getEspionageSpendingWeightAgainstTeam(eIndex) + iChange);
}

void CvPlayer::doAdvancedStartAction(AdvancedStartActionTypes eAction, int iX, int iY, int iData, bool bAdd)
{
	if (getAdvancedStartPoints() < 0)
	{
		return;
	}

	CvPlot* pPlot = GC.getMap().plot(iX, iY);

	if (0 == getNumCities())
	{
		switch (eAction)
		{
		case ADVANCEDSTARTACTION_EXIT:
			//Try to build this player's empire
			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setBusy(true);
			}
			AI_doAdvancedStart(true);			
			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setBusy(false);
			}
			break;
		case ADVANCEDSTARTACTION_AUTOMATE:
		case ADVANCEDSTARTACTION_CITY:
			break;
		default:
			// The first action must be to place a city
			// so players can lose by spending everything
			return;
		}
	}

	switch (eAction)
	{
	case ADVANCEDSTARTACTION_EXIT:
		changeGold(getAdvancedStartPoints());
		setAdvancedStartPoints(-1);
		if (GC.getGameINLINE().getActivePlayer() == getID())
		{
			gDLL->getInterfaceIFace()->setInAdvancedStart(false);
		}

		if (isHuman())
		{
			int iLoop;
			for (CvCity* pCity = firstCity(&iLoop); NULL != pCity; pCity = nextCity(&iLoop))
			{
				pCity->chooseProduction();
			}

			chooseTech();

			if (canRevolution(NULL))
			{
				CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_CHANGECIVIC);
				if (NULL != pInfo)
				{
					gDLL->getInterfaceIFace()->addPopup(pInfo, getID());
				}
			}
		}
		break;
	case ADVANCEDSTARTACTION_AUTOMATE:
		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setBusy(true);
		}
		AI_doAdvancedStart(true);			
		if (getID() == GC.getGameINLINE().getActivePlayer())
		{
			gDLL->getInterfaceIFace()->setBusy(false);
		}
		break;
	case ADVANCEDSTARTACTION_UNIT:
		{
			if(pPlot == NULL)
				return;

			UnitTypes eUnit = (UnitTypes) iData;
			int iCost = getAdvancedStartUnitCost(eUnit, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add unit to the map
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					CvUnit* pUnit = initUnit(eUnit, iX, iY);
					if (NULL != pUnit)
					{
						pUnit->finishMoves();
						changeAdvancedStartPoints(-iCost);
					}
				}
			}

			// Remove unit from the map
			else
			{
				// If cost is -1 we already know this unit isn't present
				if (iCost != -1)
				{
					CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
					while (pUnitNode != NULL)
					{
						CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pPlot->nextUnitNode(pUnitNode);

						if (pLoopUnit->getUnitType() == eUnit)
						{
							pLoopUnit->kill(false);
							changeAdvancedStartPoints(iCost);
							return;
						}
					}
				}

				// Proper unit not found above, delete first found
				CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
				if (pUnitNode != NULL)
				{
					CvUnit* pUnit = ::getUnit(pUnitNode->m_data);

					iCost = getAdvancedStartUnitCost(pUnit->getUnitType(), false);
					FAssertMsg(iCost != -1, "If this is -1 then that means it's going to try to delete a unit which shouldn't exist");
					pUnit->kill(false);
					changeAdvancedStartPoints(iCost);
				}
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_CITY:
		{
			if(pPlot == NULL)
				return;

			int iCost = getAdvancedStartCityCost(bAdd, pPlot);

			if (iCost < 0)
			{
				return;
			}

			// Add City to the map
			if (bAdd)
			{
				if (0 == getNumCities())
				{
					PlayerTypes eClosestPlayer = NO_PLAYER;
					int iMinDistance = MAX_INT;
					for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
					{
						CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);
						if (kPlayer.isAlive())
						{
							if (kPlayer.getTeam() == getTeam())
							{
								if (0 == kPlayer.getNumCities())
								{
									FAssert(kPlayer.getStartingPlot() != NULL);
									int iDistance = plotDistance(iX, iY, kPlayer.getStartingPlot()->getX_INLINE(), kPlayer.getStartingPlot()->getY_INLINE());
									if (iDistance < iMinDistance)
									{
										eClosestPlayer = kPlayer.getID();
										iMinDistance = iDistance;
									}
								}
							}
						}
					}
					FAssertMsg(eClosestPlayer != NO_PLAYER, "Self at a minimum should always be valid");
					if (eClosestPlayer != getID())
					{
						CvPlot* pTempPlot = GET_PLAYER(eClosestPlayer).getStartingPlot();
						GET_PLAYER(eClosestPlayer).setStartingPlot(getStartingPlot(), false);
						setStartingPlot(pTempPlot, false);					
					}
				}
				if (getAdvancedStartPoints() >= iCost || 0 == getNumCities())
				{
					found(iX, iY);
					changeAdvancedStartPoints(-std::min(iCost, getAdvancedStartPoints()));
					GC.getGameINLINE().updateColoredPlots();
					CvCity* pCity = pPlot->getPlotCity();
					if (pCity != NULL)
					{
						if (pCity->getPopulation() > 1)
						{
							pCity->setFood(pCity->growthThreshold() / 2);
						}
					}
				}
			}

			// Remove City from the map
			else
			{
				pPlot->setRouteType(NO_ROUTE, true);
				pPlot->getPlotCity()->kill(true);
				pPlot->setImprovementType(NO_IMPROVEMENT);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_POP:
		{
			if(pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL)
			{
				int iCost = getAdvancedStartPopCost(bAdd, pCity);
				
				if (iCost < 0)
				{
					return;
				}

				bool bPopChanged = false;
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->changePopulation(1);
						changeAdvancedStartPoints(-iCost);
						bPopChanged = true;
					}
				}
				else
				{
					pCity->changePopulation(-1);
					changeAdvancedStartPoints(iCost);
					bPopChanged = true;
				}

				if (bPopChanged)
				{
					pCity->setHighestPopulation(pCity->getPopulation());
					if (pCity->getPopulation() == 1)
					{
						pCity->setFood(0);
						pCity->setFoodKept(0);
					}
					else if (pCity->getPopulation() > 1)
					{
						pCity->setFood(pCity->growthThreshold() / 2);
						pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
					}
				}
			}
		}
		break;
	case ADVANCEDSTARTACTION_CULTURE:
		{
			if(pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL)
			{
				int iCost = getAdvancedStartCultureCost(bAdd, pCity);

				if (iCost < 0)
				{
					return;
				}

				// Add Culture to the City
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->setCulture(getID(), pCity->getCultureThreshold(), true, true);
						changeAdvancedStartPoints(-iCost);
					}
				}

				// Remove Culture from the city
				else
				{
					CultureLevelTypes eLevel = (CultureLevelTypes)std::max(0, pCity->getCultureLevel() - 1);
					pCity->setCulture(getID(), CvCity::getCultureThreshold(eLevel), true, true);
					changeAdvancedStartPoints(iCost);
				}
			}
		}
		break;
	case ADVANCEDSTARTACTION_BUILDING:
		{
			if(pPlot == NULL)
				return;

			CvCity* pCity = pPlot->getPlotCity();

			if (pCity != NULL)
			{
				BuildingTypes eBuilding = (BuildingTypes) iData;
				int iCost = getAdvancedStartBuildingCost(eBuilding, bAdd, pCity);

				if (iCost < 0)
				{
					return;
				}

				// Add Building to the City
				if (bAdd)
				{
					if (getAdvancedStartPoints() >= iCost)
					{
						pCity->setNumRealBuilding(eBuilding, pCity->getNumRealBuilding(eBuilding)+1);
						changeAdvancedStartPoints(-iCost);
						if (GC.getBuildingInfo(eBuilding).getFoodKept() != 0)
						{
							pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
						}
					}
				}

				// Remove Building from the map
				else
				{
					pCity->setNumRealBuilding(eBuilding, pCity->getNumRealBuilding(eBuilding)-1);
					changeAdvancedStartPoints(iCost);
					if (GC.getBuildingInfo(eBuilding).getFoodKept() != 0)
					{
						pCity->setFoodKept((pCity->getFood() * pCity->getMaxFoodKeptPercent()) / 100);
					}
				}
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_ROUTE:
		{
			if(pPlot == NULL)
				return;

			RouteTypes eRoute = (RouteTypes) iData;
			int iCost = getAdvancedStartRouteCost(eRoute, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add Route to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					pPlot->setRouteType(eRoute, true);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Route from the Plot
			else
			{
				if (pPlot->getRouteType() != eRoute)
				{
					eRoute = pPlot->getRouteType();
					iCost = getAdvancedStartRouteCost(eRoute, bAdd);
				}
				
				if (iCost < 0)
				{
					return;
				}

				pPlot->setRouteType(NO_ROUTE, true);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_IMPROVEMENT:
		{
			if(pPlot == NULL)
				return;

			ImprovementTypes eImprovement = (ImprovementTypes) iData;
			int iCost = getAdvancedStartImprovementCost(eImprovement, bAdd, pPlot);

			if (bAdd && iCost < 0)
			{
				return;
			}

			// Add Improvement to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					if (pPlot->getFeatureType() != NO_FEATURE)
					{
						for (int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
						{
							ImprovementTypes eLoopImprovement = ((ImprovementTypes)(GC.getBuildInfo((BuildTypes)iI).getImprovement()));

							if (eImprovement == eLoopImprovement)
							{
								if (GC.getBuildInfo((BuildTypes)iI).isFeatureRemove(pPlot->getFeatureType()) && canBuild(pPlot, (BuildTypes)iI))
								{
									pPlot->setFeatureType(NO_FEATURE);
									break;
								}
							}
						}
					}

					pPlot->setImprovementType(eImprovement);

					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Improvement from the Plot
			else
			{
				if (pPlot->getImprovementType() != eImprovement)
				{
					eImprovement = pPlot->getImprovementType();
					iCost = getAdvancedStartImprovementCost(eImprovement, bAdd, pPlot);
				}

				if (iCost < 0)
				{
					return;
				}

				pPlot->setImprovementType(NO_IMPROVEMENT);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_TECH:
		{
			TechTypes eTech = (TechTypes) iData;
			int iCost = getAdvancedStartTechCost(eTech, bAdd);

			if (iCost < 0)
			{
				return;
			}

			// Add Tech to team
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					GET_TEAM(getTeam()).setHasTech(eTech, true, getID(), false, false);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Tech from the Team
			else
			{
				GET_TEAM(getTeam()).setHasTech(eTech, false, getID(), false, false);
				changeAdvancedStartPoints(iCost);
			}

			if (getID() == GC.getGameINLINE().getActivePlayer())
			{
				gDLL->getInterfaceIFace()->setDirty(Advanced_Start_DIRTY_BIT, true);
			}
		}
		break;
	case ADVANCEDSTARTACTION_VISIBILITY:
		{
			if(pPlot == NULL)
				return;

			int iCost = getAdvancedStartVisibilityCost(bAdd, pPlot);

			if (iCost < 0)
			{
				return;
			}

			// Add Visibility to the plot
			if (bAdd)
			{
				if (getAdvancedStartPoints() >= iCost)
				{
					pPlot->setRevealed(getTeam(), true, true, NO_TEAM, true);
					changeAdvancedStartPoints(-iCost);
				}
			}

			// Remove Visibility from the Plot
			else
			{
				pPlot->setRevealed(getTeam(), false, true, NO_TEAM, true);
				changeAdvancedStartPoints(iCost);
			}
		}
		break;
	default:
		FAssert(false);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing a unit
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartUnitCost(UnitTypes eUnit, bool bAdd, CvPlot* pPlot) const
{
	int iLoop;
	int iNumUnitType = 0;

	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = (getProductionNeeded(eUnit) * GC.getUnitInfo(eUnit).getAdvancedStartCost()) / 100;
	if (iCost < 0)
	{
		return -1;
	}

	if (NULL == pPlot)
	{
		if (bAdd)
		{
			bool bValid = false;
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity->canTrain(eUnit))
				{
					bValid = true;
					break;
				}
			}

			if (!bValid)
			{
				return -1;
			}
		}
	}
	else
	{
		CvCity* pCity = NULL;

		if (0 == GC.getDefineINT("ADVANCED_START_ALLOW_UNITS_OUTSIDE_CITIES"))
		{
			pCity = pPlot->getPlotCity();

			if (NULL == pCity || pCity->getOwnerINLINE() != getID())
			{
				return -1;
			}

			iCost *= 100;
			iCost /= std::max(1, 100 + pCity->getProductionModifier(eUnit));
		}
		else
		{
			if (pPlot->getOwnerINLINE() != getID())
			{
				return -1;
			}

			iCost *= 100;
			iCost /= std::max(1, 100 + getProductionModifier(eUnit));
		}


		if (bAdd)
		{
			int iMaxUnitsPerCity = GC.getDefineINT("ADVANCED_START_MAX_UNITS_PER_CITY");
			if (iMaxUnitsPerCity >= 0)
			{
				if (GC.getUnitInfo(eUnit).isMilitarySupport() && getNumMilitaryUnits() >= iMaxUnitsPerCity * getNumCities())
				{
					return -1;
				}
			}

			if (NULL != pCity)
			{
				if (!pCity->canTrain(eUnit))
				{
					return -1;
				}
			}
			else
			{
				if (!pPlot->canTrain(eUnit, false, false))
				{
					return -1;
				}

				if (pPlot->isImpassable() && !GC.getUnitInfo(eUnit).isCanMoveImpassable())
				{
					return -1;
				}

				if (pPlot->getFeatureType() != NO_FEATURE)
				{
					if (GC.getUnitInfo(eUnit).getFeatureImpassable(pPlot->getFeatureType()))
					{
						TechTypes eTech = (TechTypes)GC.getUnitInfo(eUnit).getFeaturePassableTech(pPlot->getFeatureType());
						if (NO_TECH == eTech || !GET_TEAM(getTeam()).isHasTech(eTech))
						{
							return -1;
						}
					}
				}
				else
				{
					if (GC.getUnitInfo(eUnit).getTerrainImpassable(pPlot->getTerrainType()))
					{
						TechTypes eTech = (TechTypes)GC.getUnitInfo(eUnit).getTerrainPassableTech(pPlot->getTerrainType());
						if (NO_TECH == eTech || !GET_TEAM(getTeam()).isHasTech(eTech))
						{
							return -1;
						}
					}
				}
			}
		}
		// Must be this unit at plot in order to remove
		else
		{
			bool bUnitFound = false;

			CLLNode<IDInfo>* pUnitNode = pPlot->headUnitNode();
			while (pUnitNode != NULL)
			{
				CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = pPlot->nextUnitNode(pUnitNode);

				if (pLoopUnit->getUnitType() == eUnit)
				{
					bUnitFound = true;
				}
			}

			if (!bUnitFound)
			{
				return -1;
			}
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getUnitInfo(eUnit).getAdvancedStartCostIncrease())
	{
		for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			if (pLoopUnit->getUnitType() == eUnit)
			{
				++iNumUnitType;
			}
		}

		if (!bAdd)
		{
			--iNumUnitType;
		}

		if (iNumUnitType > 0)
		{
			iCost *= 100 + GC.getUnitInfo(eUnit).getAdvancedStartCostIncrease() * iNumUnitType;
			iCost /= 100;
		}
	}
	
	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing a City
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartCityCost(bool bAdd, CvPlot* pPlot) const
{
	int iNumCities = getNumCities();

	int iCost = getNewCityProductionValue();

	if (iCost < 0)
	{
		return -1;
	}
	
	// Valid plot?
	if (pPlot != NULL)
	{
		// Need valid plot to found on if adding
		if (bAdd)
		{
			if (!canFound(pPlot->getX(), pPlot->getY(), false))
			{
				return -1;
			}
		}
		// Need your own city present to remove
		else
		{
			if (pPlot->isCity())
			{
				if (pPlot->getPlotCity()->getOwnerINLINE() != getID())
				{
					return -1;
				}
			}
			else
			{
				return -1;
			}
		}

		// Is there a distance limit on how far a city can be placed from a player's start/another city?
		if (GC.getDefineINT("ADVANCED_START_CITY_PLACEMENT_MAX_RANGE") > 0)
		{
			PlayerTypes eClosestPlayer = NO_PLAYER;
			int iClosestDistance = MAX_INT;

			for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
			{
				CvPlayer& kPlayer = GET_PLAYER((PlayerTypes)iPlayer);

				if (kPlayer.isAlive())
				{
					CvPlot* pStartingPlot = kPlayer.getStartingPlot();

					if (NULL != pStartingPlot)
					{
						int iDistance = ::plotDistance(pPlot->getX_INLINE(), pPlot->getY_INLINE(), pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE());
						if (iDistance <= GC.getDefineINT("ADVANCED_START_CITY_PLACEMENT_MAX_RANGE"))
						{
							if (iDistance < iClosestDistance || (iDistance == iClosestDistance && getTeam() != kPlayer.getTeam()))
							{
								iClosestDistance = iDistance;
								eClosestPlayer = kPlayer.getID();
							}
						}				
					}
				}
			}

			if (NO_PLAYER == eClosestPlayer || GET_PLAYER(eClosestPlayer).getTeam() != getTeam())
			{
				return -1;
			}
			//Only allow founding a city at someone elses start point if 
			//We have no cities and they have no cities.
			if ((getID() != eClosestPlayer) && ((getNumCities() > 0) || (GET_PLAYER(eClosestPlayer).getNumCities() > 0)))
			{
				return -1;
			}
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getDefineINT("ADVANCED_START_CITY_COST_INCREASE"))
	{
		if (!bAdd)
		{
			--iNumCities;
		}

		if (iNumCities > 0)
		{
			iCost *= 100 + GC.getDefineINT("ADVANCED_START_CITY_COST_INCREASE") * iNumCities;
			iCost /= 100;
		}
	}
	
	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Population
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartPopCost(bool bAdd, CvCity* pCity) const
{
	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = (getGrowthThreshold(1) * GC.getDefineINT("ADVANCED_START_POPULATION_COST")) / 100;

	if (NULL != pCity)
	{
		if (pCity->getOwnerINLINE() != getID())
		{
			return -1;
		}

		int iPopulation = pCity->getPopulation();

		// Need to have Population to remove it
		if (!bAdd)
		{
			--iPopulation;

			if (iPopulation < GC.getDefineINT("INITIAL_CITY_POPULATION") + GC.getEraInfo(GC.getGameINLINE().getStartEra()).getFreePopulation())
			{
				return -1;
			}
		}

		iCost = (getGrowthThreshold(iPopulation) * GC.getDefineINT("ADVANCED_START_POPULATION_COST")) / 100;

		// Increase cost if the XML defines that additional Pop will cost more
		if (0 != GC.getDefineINT("ADVANCED_START_POPULATION_COST_INCREASE"))
		{
			--iPopulation;

			if (iPopulation > 0)
			{
				iCost *= 100 + GC.getDefineINT("ADVANCED_START_POPULATION_COST_INCREASE") * iPopulation;
				iCost /= 100;
			}
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Culture
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartCultureCost(bool bAdd, CvCity* pCity) const
{
	if (0 == getNumCities())
	{
		return -1;
	}

	int iCost = GC.getDefineINT("ADVANCED_START_CULTURE_COST");
	if (iCost < 0)
	{
		return -1;
	}

	if (NULL != pCity)
	{
		if (pCity->getOwnerINLINE() != getID())
		{
			return -1;
		}

		// Need to have enough culture to remove it
		if (!bAdd)
		{
			if (pCity->getCultureLevel() <= 0)
			{
				return -1;
			}
		}

		int iCulture;
		if (bAdd)
		{
			iCulture = CvCity::getCultureThreshold((CultureLevelTypes)(pCity->getCultureLevel() + 1)) - pCity->getCulture(getID());
		}
		else
		{
			iCulture = pCity->getCulture(getID()) - CvCity::getCultureThreshold((CultureLevelTypes)(pCity->getCultureLevel() - 1));
		}

		iCost *= iCulture;
		iCost /= std::max(1, GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getHurryPercent());
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing a Building from a city
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartBuildingCost(BuildingTypes eBuilding, bool bAdd, CvCity* pCity) const
{
	if (0 == getNumCities())
	{
		return -1;
	}

	int iNumBuildingType = 0;

	int iCost = (getProductionNeeded(eBuilding) * GC.getBuildingInfo(eBuilding).getAdvancedStartCost()) / 100;

	if (iCost < 0)
	{
		return -1;
	}

	if (GC.getBuildingInfo(eBuilding).getFreeStartEra() != NO_ERA && GC.getGameINLINE().getStartEra() >=  GC.getBuildingInfo(eBuilding).getFreeStartEra())
	{
		// you get this building for free
		return -1;
	}

	if (NULL == pCity)
	{
		if (bAdd)
		{
			bool bValid = false;
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity->canConstruct(eBuilding))
				{
					bValid = true;
					break;
				}
			}

			if (!bValid)
			{
				return -1;
			}
		}
	}
	if (NULL != pCity)
	{
		if (pCity->getOwnerINLINE() != getID())
		{
			return -1;
		}

		iCost *= 100;
		iCost /= std::max(1, 100 + pCity->getProductionModifier(eBuilding));

		if (bAdd)
		{
			if (!pCity->canConstruct(eBuilding, true, false, false))
			{
				return -1;
			}
		}
		else
		{
			if (pCity->getNumRealBuilding(eBuilding) <= 0)
			{
				return -1;
			}

			// Check other buildings in this city and make sure none of them require this one

			// Loop through Buildings to see which are present
			for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
			{
				BuildingTypes eBuildingLoop = (BuildingTypes) iBuildingLoop;

				if (pCity->getNumBuilding(eBuildingLoop) > 0)
				{
					// Loop through present Building's requirements
					for (int iBuildingClassPrereqLoop = 0; iBuildingClassPrereqLoop < GC.getNumBuildingClassInfos(); iBuildingClassPrereqLoop++)
					{
						if (GC.getBuildingInfo(eBuildingLoop).isBuildingClassNeededInCity(iBuildingClassPrereqLoop))
						{
							if ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iBuildingClassPrereqLoop)) == eBuilding)
							{
								return -1;
							}
						}
					}
				}
			}
		}
	}

	// Increase cost if the XML defines that additional Buildings will cost more
	if (0 != GC.getBuildingInfo(eBuilding).getAdvancedStartCostIncrease())
	{
		iNumBuildingType = countNumBuildings(eBuilding);
		
		if (!bAdd)
		{
			--iNumBuildingType;
		}

		if (iNumBuildingType > 0)
		{
			iCost *= 100 + GC.getBuildingInfo(eBuilding).getAdvancedStartCostIncrease() * std::max(0, iNumBuildingType - getNumCities());
			iCost /= 100;
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Route
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartRouteCost(RouteTypes eRoute, bool bAdd, CvPlot* pPlot) const
{
	if (0 == getNumCities())
	{
		return -1;
	}

	if (eRoute == NO_ROUTE)
	{
		return -1;
	}

	int iNumRoutes = 0;

	int iCost = GC.getRouteInfo(eRoute).getAdvancedStartCost();

	// This denotes cities may not be purchased through Advanced Start
	if (iCost < 0)
	{
		return -1;
	}

	iCost *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getBuildPercent();
	iCost /= 100;

	// No invalid plots!
	if (pPlot != NULL)
	{
		if (pPlot->isCity())
		{
			return -1;
		}

		if (bAdd)
		{
			if (pPlot->isImpassable() || pPlot->isWater())
			{
				return -1;
			}
			// Can't place twice
			if (pPlot->getRouteType() == eRoute)
			{
				return -1;
			}
		}
		else
		{
			// Need Route to remove it
			if (pPlot->getRouteType() != eRoute)
			{
				return -1;
			}
		}

		// Must be owned by me
		if (pPlot->getOwnerINLINE() != getID())
		{
			return -1;
		}
	}

	// Tech requirement
	for (int iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
	{
		if (GC.getBuildInfo((BuildTypes) iBuildLoop).getRoute() == eRoute)
		{
			if (!(GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getBuildInfo((BuildTypes) iBuildLoop).getTechPrereq())))
			{
				return -1;
			}
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getRouteInfo(eRoute).getAdvancedStartCostIncrease())
	{
		int iPlotLoop = 0;
		CvPlot* pPlot;

		for (iPlotLoop = 0; iPlotLoop < GC.getMapINLINE().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMapINLINE().plotByIndex(iPlotLoop);

			if (pPlot->getRouteType() == eRoute)
			{
				++iNumRoutes;
			}
		}

		if (!bAdd)
		{
			--iNumRoutes;
		}

		if (iNumRoutes > 0)
		{
			iCost *= 100 + GC.getRouteInfo(eRoute).getAdvancedStartCostIncrease() * iNumRoutes;
			iCost /= 100;
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Improvement
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartImprovementCost(ImprovementTypes eImprovement, bool bAdd, CvPlot* pPlot) const
{
	if (eImprovement == NO_IMPROVEMENT)
	{
		return -1;
	}

	if (0 == getNumCities())
	{
		return -1;
	}

	int iNumImprovements = 0;
	int iCost = GC.getImprovementInfo(eImprovement).getAdvancedStartCost();

		// This denotes cities may not be purchased through Advanced Start
	if (iCost < 0)
	{
		return -1;
	}

	iCost *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getBuildPercent();
	iCost /= 100;

	// Can this Improvement be on our plot?
	if (pPlot != NULL)
	{
		if (bAdd)
		{
			// Valid Plot
			if (!pPlot->canHaveImprovement(eImprovement, getID(), false))
			{
				return -1;
			}

			bool bValid = false;

			for (int iI = 0; iI < GC.getNumBuildInfos(); ++iI)
			{
				CvBuildInfo& kBuild = GC.getBuildInfo((BuildTypes)iI);
				ImprovementTypes eLoopImprovement = ((ImprovementTypes)(kBuild.getImprovement()));

				if (eImprovement == eLoopImprovement && canBuild(pPlot, (BuildTypes)iI))
				{
					bValid = true;

					FeatureTypes eFeature = pPlot->getFeatureType();
					if (NO_FEATURE != eFeature && kBuild.isFeatureRemove(eFeature))
					{
						iCost += GC.getFeatureInfo(eFeature).getAdvancedStartRemoveCost();
					}

					break;
				}
			}

			if (!bValid)
			{
				return -1;
			}

			// Can't place twice
			if (pPlot->getImprovementType() == eImprovement)
			{
				return -1;
			}
		}
		else
		{
			// Need this improvement in order to remove it
			if (pPlot->getImprovementType() != eImprovement)
			{
				return -1;
			}
		}

		// Must be owned by me
		if (pPlot->getOwnerINLINE() != getID())
		{
			return -1;
		}
	}
	
	// Tech requirement
	for (int iBuildLoop = 0; iBuildLoop < GC.getNumBuildInfos(); iBuildLoop++)
	{
		if (GC.getBuildInfo((BuildTypes) iBuildLoop).getImprovement() == eImprovement)
		{
			if (!(GET_TEAM(getTeam()).isHasTech((TechTypes)GC.getBuildInfo((BuildTypes) iBuildLoop).getTechPrereq())))
			{
				return -1;
			}
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getImprovementInfo(eImprovement).getAdvancedStartCostIncrease())
	{
		int iPlotLoop = 0;
		CvPlot* pPlot;

		for (iPlotLoop = 0; iPlotLoop < GC.getMapINLINE().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMapINLINE().plotByIndex(iPlotLoop);
			
			if (pPlot->getImprovementType() == eImprovement)
			{
				++iNumImprovements;
			}
		}

		if (!bAdd)
		{
			--iNumImprovements;
		}

		if (iNumImprovements > 0)
		{
			iCost *= 100 + GC.getImprovementInfo(eImprovement).getAdvancedStartCostIncrease() * iNumImprovements;
			iCost /= 100;
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Tech
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartTechCost(TechTypes eTech, bool bAdd) const
{
	if (eTech == NO_TECH)
	{
		return -1;
	}

	if (0 == getNumCities())
	{
		return -1;
	}

	int iNumTechs = 0;
		
	int iCost = (GET_TEAM(getTeam()).getResearchCost(eTech) * GC.getTechInfo(eTech).getAdvancedStartCost()) / 100;
	if (iCost < 0)
	{
		return -1;
	}

	if (bAdd)
	{
		if (!canResearch(eTech, false))
		{
			return -1;
		}
	}
	else if (!bAdd)
	{
		if (!GET_TEAM(getTeam()).isHasTech(eTech))
		{
			return -1;
		}
		
		// Search through all techs to see if any of the currently owned ones requires this tech
		for (int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
		{
			TechTypes eTechLoop = (TechTypes) iTechLoop;

			if (GET_TEAM(getTeam()).isHasTech(eTechLoop))
			{
				int iPrereqLoop;

				// Or Prereqs
				for (iPrereqLoop = 0; iPrereqLoop < GC.getNUM_OR_TECH_PREREQS(); iPrereqLoop++)
				{
					if (GC.getTechInfo(eTechLoop).getPrereqOrTechs(iPrereqLoop) == eTech)
					{
						return -1;
					}
				}

				// And Prereqs
				for (iPrereqLoop = 0; iPrereqLoop < GC.getNUM_AND_TECH_PREREQS(); iPrereqLoop++)
				{
					if (GC.getTechInfo(eTechLoop).getPrereqAndTechs(iPrereqLoop) == eTech)
					{
						return -1;
					}
				}
			}
		}
		
		// If player has placed anything on the map which uses this tech then you cannot remove it
		int iLoop;

		// Units
		CvUnit* pLoopUnit;
		for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			if (pLoopUnit->getUnitInfo().getPrereqAndTech() == eTech)
			{
				return -1;
			}

			for (int iI = 0; iI < GC.getNUM_UNIT_AND_TECH_PREREQS(); iI++)
			{
				if (pLoopUnit->getUnitInfo().getPrereqAndTechs(iI) == eTech)
				{
					return -1;
				}
			}
		}

		// Cities
		CvCity* pLoopCity;
		for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			// All Buildings
			for (int iBuildingLoop = 0; iBuildingLoop < GC.getNumBuildingInfos(); iBuildingLoop++)
			{
				BuildingTypes eBuilding = (BuildingTypes) iBuildingLoop;

				if (pLoopCity->getNumRealBuilding(eBuilding) > 0)
				{
					if (GC.getBuildingInfo(eBuilding).getPrereqAndTech() == eTech)
					{
						return -1;
					}

					for (int iI = 0; iI < GC.getNUM_BUILDING_AND_TECH_PREREQS(); iI++)
					{
						if (GC.getBuildingInfo(eBuilding).getPrereqAndTechs(iI) == eTech)
						{
							return -1;
						}
					}
				}
			}
		}

	}

	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getTechInfo(eTech).getAdvancedStartCostIncrease())
	{
		for (int iTechLoop = 0; iTechLoop < GC.getNumTechInfos(); iTechLoop++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes) iTechLoop))
			{
				++iNumTechs;
			}
		}

		if (!bAdd)
		{
			--iNumTechs;
		}

		if (iNumTechs > 0)
		{
			iCost *= 100 + GC.getTechInfo(eTech).getAdvancedStartCostIncrease() * iNumTechs;
			iCost /= 100;
		}
	}

	return iCost;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Adding or removing Visibility
/////////////////////////////////////////////////////////////////////////////////////////////

int CvPlayer::getAdvancedStartVisibilityCost(bool bAdd, CvPlot* pPlot) const
{
	if (0 == getNumCities())
	{
		return -1;
	}

	int iNumVisiblePlots = 0;
	int iCost = GC.getDefineINT("ADVANCED_START_VISIBILITY_COST");

	// This denotes Visibility may not be purchased through Advanced Start
	if (iCost == -1)
	{
		return -1;
	}

	// Valid Plot?
	if (pPlot != NULL)
	{
		if (bAdd)
		{
			if (pPlot->isRevealed(getTeam(), false))
			{
				return -1;
			}
			if (!pPlot->isAdjacentRevealed(getTeam()))
			{
				return -1;
			}
		}
		else
		{
			if (!pPlot->isRevealed(getTeam(), false))
			{
				return -1;
			}
		}
	}

	// Increase cost if the XML defines that additional units will cost more
	if (0 != GC.getDefineINT("ADVANCED_START_VISIBILITY_COST_INCREASE"))
	{
		int iPlotLoop = 0;
		CvPlot* pPlot;

		for (iPlotLoop = 0; iPlotLoop < GC.getMapINLINE().numPlots(); iPlotLoop++)
		{
			pPlot = GC.getMapINLINE().plotByIndex(iPlotLoop);

			if (pPlot->isRevealed(getTeam(), false))
			{
				++iNumVisiblePlots;
			}
		}

		if (!bAdd)
		{
			--iNumVisiblePlots;
		}

		if (iNumVisiblePlots > 0)
		{
			iCost *= 100 + GC.getDefineINT("ADVANCED_START_VISIBILITY_COST_INCREASE") * iNumVisiblePlots;
			iCost /= 100;
		}
	}

	return iCost;
}

void CvPlayer::doWarnings()
{
	CvCity* pNearestCity;
	CvPlot* pLoopPlot;
	CvUnit* pLoopUnit;
	wchar szBuffer[1024];
	int iMaxCount;
	int iI;

	//update enemy unit in your territory glow
	int iLoop;
	for(pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		//update glow
		gDLL->getEntityIFace()->updateEnemyGlow(pLoopUnit->getUnitEntity());
	}

	//update enemy units close to your territory
	iMaxCount = range(((getNumCities() + 4) / 7), 2, 5);
	for (iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		if (iMaxCount == 0)
		{
			break;
		}

		pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

		if (pLoopPlot->isAdjacentPlayer(getID()))
		{
			if (!(pLoopPlot->isCity()))
			{
				if (pLoopPlot->isVisible(getTeam(), false))
				{
					CvUnit *pUnit = pLoopPlot->getVisibleEnemyDefender(getID());
					if (pUnit != NULL)
					{
						if (!pUnit->isAnimal())
						{
							pNearestCity = GC.getMapINLINE().findCity(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), getID(), NO_TEAM, !(pLoopPlot->isWater()));

							if (pNearestCity != NULL)
							{	
								swprintf(szBuffer, gDLL->getText("TXT_KEY_MISC_ENEMY_TROOPS_SPOTTED", pNearestCity->getNameKey()).GetCString());
								gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_ENEMY_TROOPS", MESSAGE_TYPE_INFO, pUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE(), true, true);

								iMaxCount--;
							}
						}
					}
				}
			}
		}
	}
}


void CvPlayer::verifyGoldCommercePercent()
{
	/* original bts code
	while ((getGold() + calculateGoldRate()) < 0)
	{
		changeCommercePercent(COMMERCE_GOLD, GC.getDefineINT("COMMERCE_PERCENT_CHANGE_INCREMENTS"));

		if (getCommercePercent(COMMERCE_GOLD) == 100)
		{
			break;
		}
	} */
	// K-Mod
	bool bValid = isCommerceFlexible(COMMERCE_GOLD);
	while (bValid && getCommercePercent(COMMERCE_GOLD) < 100 && getGold() + calculateGoldRate() < 0)
	{
		bValid = changeCommercePercent(COMMERCE_GOLD, GC.getDefineINT("COMMERCE_PERCENT_CHANGE_INCREMENTS"));
	}
	// K-Mod end
}


void CvPlayer::processCivics(CivicTypes eCivic, int iChange)
{
	int iI, iJ;
	int iLoop;

	changeGreatPeopleRateModifier(GC.getCivicInfo(eCivic).getGreatPeopleRateModifier() * iChange);
	changeNoGreatPeopleCount(GC.getCivicInfo(eCivic).isNoGreatPeople() * iChange); // Civ4 Reimagined
	changeNoCultureFlipCount(GC.getCivicInfo(eCivic).isNoCultureFlip() * iChange); // Civ4 Reimagined
	changeNoCapitalCount(GC.getCivicInfo(eCivic).isNoCapital() * iChange); // Civ4 Reimagined
	changeGreatGeneralRateModifier(GC.getCivicInfo(eCivic).getGreatGeneralRateModifier() * iChange);
	changeDomesticGreatGeneralRateModifier(GC.getCivicInfo(eCivic).getDomesticGreatGeneralRateModifier() * iChange);
	changeStateReligionGreatPeopleRateModifier(GC.getCivicInfo(eCivic).getStateReligionGreatPeopleRateModifier() * iChange);
	changeDistanceMaintenanceModifier(GC.getCivicInfo(eCivic).getDistanceMaintenanceModifier() * iChange);
	changeNumCitiesMaintenanceModifier(GC.getCivicInfo(eCivic).getNumCitiesMaintenanceModifier() * iChange);
	changeColonyMaintenanceModifier(GC.getCivicInfo(eCivic).getColonyMaintenanceModifier() * iChange); // Civ4 Reimagined
	changeCorporationMaintenanceModifier(GC.getCivicInfo(eCivic).getCorporationMaintenanceModifier() * iChange);
	changeProcessModifier(GC.getCivicInfo(eCivic).getProcessModifier() * iChange); //Leoreth
	changeExtraHealth(GC.getCivicInfo(eCivic).getExtraHealth() * iChange);
	changePollutionModifier(GC.getCivicInfo(eCivic).getPollutionModifier() * iChange); //Leoreth
	changePopulationUnhappinessModifier(GC.getCivicInfo(eCivic).getPopulationUnhappinessModifier() * iChange); // Civ4 Reimagined
	changeCivicHappiness(GC.getCivicInfo(eCivic).getExtraHappiness() * iChange); // K-Mod // Civ4 Reimagined
	changeNoCapitalUnhappinessCount(GC.getCivicInfo(eCivic).isNoCapitalUnhappiness() ? iChange : 0); // Civ4 Reimagined
	changeFreeExperience(GC.getCivicInfo(eCivic).getFreeExperience() * iChange);
	changeWorkerSpeedModifier(GC.getCivicInfo(eCivic).getWorkerSpeedModifier() * iChange);
	changeImprovementUpgradeRateModifier(GC.getCivicInfo(eCivic).getImprovementUpgradeRateModifier() * iChange);
	changeMilitaryProductionModifier(GC.getCivicInfo(eCivic).getMilitaryProductionModifier() * iChange);
	changeBaseFreeUnits(GC.getCivicInfo(eCivic).getBaseFreeUnits() * iChange);
	changeBaseFreeMilitaryUnits(GC.getCivicInfo(eCivic).getBaseFreeMilitaryUnits() * iChange);
	changeFreeUnitsPopulationPercent(GC.getCivicInfo(eCivic).getFreeUnitsPopulationPercent() * iChange);
	changeFreeMilitaryUnitsPopulationPercent(GC.getCivicInfo(eCivic).getFreeMilitaryUnitsPopulationPercent() * iChange);
	changeGoldPerUnit(GC.getCivicInfo(eCivic).getGoldPerUnit() * iChange);
	changeGoldPerMilitaryUnit(GC.getCivicInfo(eCivic).getGoldPerMilitaryUnit() * iChange);
	changeNoUnitSupplyCount((GC.getCivicInfo(eCivic).isNoUnitSupply()) ? iChange : 0); // Civ4 Reimagined
	changeHappyPerMilitaryUnit(GC.getCivicInfo(eCivic).getHappyPerMilitaryUnit() * iChange);
	changeMilitaryHappinessLimit(GC.getCivicInfo(eCivic).getMilitaryHappinessLimit() * iChange); //Leoreth
	changeMilitaryFoodProductionCount((GC.getCivicInfo(eCivic).isMilitaryFoodProduction()) ? iChange : 0);
	changeMeleeMilitaryFoodProductionCount((GC.getCivicInfo(eCivic).isMeleeMilitaryFoodProduction()) ? iChange : 0); // Civ4 Reimagined
	changeEnableSlavesCount((GC.getCivicInfo(eCivic).enablesSlaves()) ? iChange : 0); // Civ4 Reimagined
	changeNoMilitaryProductionMaliCount((GC.getCivicInfo(eCivic).isNoMilitaryProductionMali()) ? iChange : 0); // Civ4 Reimagined
	changeMaxConscript(getWorldSizeMaxConscript(eCivic) * iChange);
	//changeNoUnhealthyPopulationCount((GC.getCivicInfo(eCivic).isNoUnhealthyPopulation()) ? iChange : 0);
	changeUnhealthyPopulationModifier(GC.getCivicInfo(eCivic).getUnhealthyPopulationModifier() * iChange); // K-Mod
	changeBuildingOnlyHealthyCount((GC.getCivicInfo(eCivic).isBuildingOnlyHealthy()) ? iChange : 0);
	changeLargestCityHappiness(GC.getCivicInfo(eCivic).getLargestCityHappiness() * iChange);
	changeGoldPerHappinessBonus(GC.getCivicInfo(eCivic).getGoldPerHappinessBonus() * iChange); // Civ4 Reimagined
	changeWarWearinessModifier(GC.getCivicInfo(eCivic).getWarWearinessModifier() * iChange);
	changeFreeSpecialist(GC.getCivicInfo(eCivic).getFreeSpecialist() * iChange);
	changeCoreFreeSpecialist(GC.getCivicInfo(eCivic).getCoreFreeSpecialist() * iChange); //Leoreth
	changeTradeRoutes(GC.getCivicInfo(eCivic).getTradeRoutes() * iChange);
	changeNoForeignTradeCount(GC.getCivicInfo(eCivic).isNoForeignTrade() * iChange);
	changeDomesticTradeModifier(GC.getCivicInfo(eCivic).getDomesticTradeModifier() * iChange); // Civ4 Reimagined
	changeColonyTradeModifier(GC.getCivicInfo(eCivic).getColonyTradeModifier() * iChange); // Civ4 Reimagined
	changeCapitalTradeModifier(GC.getCivicInfo(eCivic).getCapitalTradeModifier() * iChange); // Civ4 Reimagined
	changeNoCorporationsCount(GC.getCivicInfo(eCivic).isNoCorporations() * iChange);
	changeNoForeignCorporationsCount(GC.getCivicInfo(eCivic).isNoForeignCorporations() * iChange);
	changeStateReligionCount((GC.getCivicInfo(eCivic).isStateReligion()) ? iChange : 0);
	changeNoNonStateReligionSpreadCount((GC.getCivicInfo(eCivic).isNoNonStateReligionSpread()) ? iChange : 0);
	changeStateReligionHappiness(GC.getCivicInfo(eCivic).getStateReligionHappiness() * iChange);
	changeNonStateReligionHappiness(GC.getCivicInfo(eCivic).getNonStateReligionHappiness() * iChange);
	changeStateReligionUnitProductionModifier(GC.getCivicInfo(eCivic).getStateReligionUnitProductionModifier() * iChange);
	changeStateReligionBuildingProductionModifier(GC.getCivicInfo(eCivic).getStateReligionBuildingProductionModifier() * iChange);
	changeStateReligionFreeExperience(GC.getCivicInfo(eCivic).getStateReligionFreeExperience() * iChange);
	changeExpInBorderModifier(GC.getCivicInfo(eCivic).getExpInBorderModifier() * iChange);
	changeSpecialistExtraYieldBaseThreshold(GC.getCivicInfo(eCivic).getSpecialistExtraYieldBaseThreshold() * iChange); //Leoreth
	changeSpecialistExtraYieldEraThreshold(GC.getCivicInfo(eCivic).getSpecialistExtraYieldEraThreshold() * iChange); //Leoreth
	changeNoForeignCultureUnhappinessCount(GC.getCivicInfo(eCivic).isNoForeignCultureUnhappiness() * iChange); //Civ4 Reimagined
	changeNoCityResistanceCount(GC.getCivicInfo(eCivic).isNoCityResistance() * iChange); //Civ4 Reimagined
	changeProductionPerSurplusHappiness(GC.getCivicInfo(eCivic).getProductionPerSurplusHappiness() * iChange); //Civ4 Reimagined
	changeMaxGlobalBuildingProductionModifier(GC.getCivicInfo(eCivic).getWonderProductionBonus() * iChange); //Civ4 Reimagined
	changeUnitUpgradeCostModifier(GC.getCivicInfo(eCivic).getUnitUpgradeCostModifier() * iChange); //Civ4 Reimagined
	changeForeignUnitSupply(GC.getCivicInfo(eCivic).getForeignUnitSupply() * iChange); //Civ4 Reimagined
	changePillageGainModifier(GC.getCivicInfo(eCivic).getPillageGainModifier() * iChange); //Civ4 Reimagined
	changeBonusValueModifier(GC.getCivicInfo(eCivic).getBonusRatioModifier() * iChange); //Civ4 Reimagined
	changeResearchPerCulture(GC.getCivicInfo(eCivic).getResearchPerCulture() * iChange); //Civ4 Reimagined
	changeUnlimitedAnimalXPCount((GC.getCivicInfo(eCivic).isUnlimitedAnimalXP()) ? iChange : 0); // Civ4 Reimagined
	changeFreeWorkers(GC.getCivicInfo(eCivic).getFreeWorkers() * iChange); //Civ4 Reimagined

	// Civ4 Reimagined: Carthage UP
	if (getLegacyCivic() == eCivic)
	{
		processLegacyCivicBonus(-iChange);
	}

	// Civ4 Reimagined: Ideologies
	const IdeologyTypes eCurrentIdeology = getIdeology();
	changeIdeologyInfluence(IDEOLOGY_CONSERVATISM, GC.getCivicInfo(eCivic).getConservative() * iChange);
	changeIdeologyInfluence(IDEOLOGY_LIBERALISM, GC.getCivicInfo(eCivic).getLiberal() * iChange);
	changeIdeologyInfluence(IDEOLOGY_COMMUNISM, GC.getCivicInfo(eCivic).getCommunist() * iChange);
	changeIdeologyInfluence(IDEOLOGY_FASCISM, GC.getCivicInfo(eCivic).getFascist() * iChange);
	updateIdeology();
	const IdeologyTypes eNewIdeology = getIdeology();
	const bool bIdeologyChange = eCurrentIdeology != eNewIdeology;

	if (bIdeologyChange)
	{
		gDLL->getInterfaceIFace()->setDirty(Score_DIRTY_BIT, true);
	}

	if ( gPlayerLogLevel >= 2 )
	{
		logBBAI("%S Conservative: %d, Liberal: %d, Communist: %d, Fascist: %d --> Ideology: %d", getCivilizationDescription(0), getIdeologyInfluence(IDEOLOGY_CONSERVATISM),
		getIdeologyInfluence(IDEOLOGY_LIBERALISM), getIdeologyInfluence(IDEOLOGY_COMMUNISM), getIdeologyInfluence(IDEOLOGY_FASCISM), (int)getIdeology());
	}
	
	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		changeUnitClassProductionModifier((UnitClassTypes)iI, (GC.getCivicInfo(eCivic).getUnitClassProductionModifier(iI) * iChange)); // Civ4 Reimagined
	}
		
	for (iI = 0; iI < NUM_YIELD_TYPES; iI++)
	{
		changeYieldRateModifier(((YieldTypes)iI), (GC.getCivicInfo(eCivic).getYieldModifier(iI) * iChange));
		changeExtraYield(((YieldTypes)iI), (GC.getCivicInfo(eCivic).getExtraYield(iI) * iChange)); // Civ4 Reimagined
		changeCapitalExtraYieldFromCityPercent(((YieldTypes)iI), (GC.getCivicInfo(eCivic).getCapitalExtraYieldFromCityPercent(iI) * iChange)); // Civ4 Reimagined
		changeCapitalYieldRateModifier(((YieldTypes)iI), (GC.getCivicInfo(eCivic).getCapitalYieldModifier(iI) * iChange));
		changeTradeYieldModifier(((YieldTypes)iI), (GC.getCivicInfo(eCivic).getTradeYieldModifier(iI) * iChange));
		changeSpecialistThresholdExtraYield(((YieldTypes)iI), (GC.getCivicInfo(eCivic).getSpecialistThresholdExtraYield(iI) * iChange)); //Civ4 Reimagined
		for (iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
		{
			changeSpecialistExtraYield(((SpecialistTypes)iJ), ((YieldTypes)iI), (GC.getCivicInfo(eCivic).getSpecialistYieldChanges(iJ, iI) * iChange)); //Civ4 Reimagined
		}
	}

	for (iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
	{
		changeCommerceRateModifier(((CommerceTypes)iI), (GC.getCivicInfo(eCivic).getCommerceModifier(iI) * iChange));
		changeCapitalCommerceRateModifier(((CommerceTypes)iI), (GC.getCivicInfo(eCivic).getCapitalCommerceModifier(iI) * iChange));
		changeCapitalCommerceRateModifierPerHappinessSurplus(((CommerceTypes)iI), (GC.getCivicInfo(eCivic).getCapitalCommerceModifierPerHappinessSurplus(iI) * iChange)); // Civ4 Reimagined
		changeSpecialistExtraCommerce(((CommerceTypes)iI), (GC.getCivicInfo(eCivic).getSpecialistExtraCommerce(iI) * iChange));
		changeStateReligionBuildingCommerce(((CommerceTypes)iI), (GC.getCivicInfo(eCivic).getStateReligionBuildingCommerce(iI) * iChange)); //Civ4 Reimagined
		changeCommerceHappiness(((CommerceTypes)iI), (GC.getCivicInfo(eCivic).getExtraCommerceHappiness(iI) * iChange)); //Civ4 Reimagined
		for (iJ = 0; iJ < GC.getNumSpecialistInfos(); iJ++)
		{
			changeSpecialistCommerceChange(((SpecialistTypes)iJ), ((CommerceTypes)iI), (GC.getCivicInfo(eCivic).getSpecialistCommerceChanges(iJ, iI) * iChange)); //Civ4 Reimagined
		}
	}

	for (iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
	{
		BuildingTypes eOurBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iI);
		CvTeamAI& currentTeam = GET_TEAM(getTeam());
		if (NO_BUILDING != eOurBuilding)
		{
			changeExtraBuildingHappiness(eOurBuilding, (GC.getCivicInfo(eCivic).getBuildingHappinessChanges(iI) * iChange));
			changeExtraBuildingHealth(eOurBuilding, (GC.getCivicInfo(eCivic).getBuildingHealthChanges(iI) * iChange));
			changeBuildingProductionModifier(eOurBuilding, (GC.getCivicInfo(eCivic).getBuildingProductionModifier(iI) * iChange)); // Leoreth
			
			// Civ4 Reimagined
			if (GC.getBuildingInfo(eOurBuilding).isPrereqCivic(eCivic))
			{
				if (iChange != 0 && !currentTeam.isObsoleteBuilding(eOurBuilding))
				{
					for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
					{
						if (pLoopCity->getNumBuilding(eOurBuilding) > 0)
						{
							pLoopCity->processBuilding(eOurBuilding, iChange * pLoopCity->getNumBuilding(eOurBuilding), true);
						}
					}
				}
			}

			// Civ4 Reimagined
			if (bIdeologyChange && !currentTeam.isObsoleteBuilding(eOurBuilding))
			{
				if (GC.getBuildingInfo(eOurBuilding).getPrereqIdeology() == eCurrentIdeology)
				{
					for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
					{
						if (pLoopCity->getNumBuilding(eOurBuilding) > 0)
						{
							pLoopCity->processBuilding(eOurBuilding, -pLoopCity->getNumBuilding(eOurBuilding), true);
						}
					}
				} else if (GC.getBuildingInfo(eOurBuilding).getPrereqIdeology() == eNewIdeology)
				{
					for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
					{
						if (pLoopCity->getNumBuilding(eOurBuilding) > 0)
						{
							pLoopCity->processBuilding(eOurBuilding, pLoopCity->getNumBuilding(eOurBuilding), true);
						}
					}
				}
			}
			
			// Civ4 Reimagined
			for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
			{
				changeBuildingYieldChange((BuildingClassTypes)iI, (YieldTypes)iJ, (GC.getCivicInfo(eCivic).getBuildingYieldChanges(iI, iJ) * iChange));
			}
		}
	}

	for (iI = 0; iI < GC.getNumFeatureInfos(); iI++)
	{
		changeFeatureHappiness(((FeatureTypes)iI), (GC.getCivicInfo(eCivic).getFeatureHappinessChanges(iI) * iChange));
	}

	for (iI = 0; iI < GC.getNumHurryInfos(); iI++)
	{
		changeHurryCount(((HurryTypes)iI), ((GC.getCivicInfo(eCivic).isHurry(iI)) ? iChange : 0));
	}

	for (iI = 0; iI < GC.getNumSpecialBuildingInfos(); iI++)
	{
		changeSpecialBuildingNotRequiredCount(((SpecialBuildingTypes)iI), ((GC.getCivicInfo(eCivic).isSpecialBuildingNotRequired(iI)) ? iChange : 0));
	}

	for (iI = 0; iI < GC.getNumSpecialistInfos(); iI++)
	{
		changeSpecialistValidCount(((SpecialistTypes)iI), ((GC.getCivicInfo(eCivic).isSpecialistValid(iI)) ? iChange : 0));
		// Civ4 Reimagined
		changeExtraSpecialists((SpecialistTypes)iI, GC.getCivicInfo(eCivic).getExtraSpecialists((SpecialistTypes)iI) * iChange);
	}

	for (iI = 0; iI < GC.getNumImprovementInfos(); iI++)
	{
		// Civ4 Reimagined
		changeRadiusImprovementHappiness((ImprovementTypes)iI, (GC.getCivicInfo(eCivic).getRadiusImprovementHappinessChanges(iI) * iChange));

		for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			changeImprovementYieldChange(((ImprovementTypes)iI), ((YieldTypes)iJ), (GC.getCivicInfo(eCivic).getImprovementYieldChanges(iI, iJ) * iChange));
		}
		// Civ4 Reimagined
		for (iJ = 0; iJ < NUM_COMMERCE_TYPES; iJ++)
		{
			changeRadiusImrovementCommerceChange(((ImprovementTypes)iI), ((CommerceTypes)iJ), (GC.getCivicInfo(eCivic).getRadiusImprovementCommerceChanges(iI, iJ) * iChange));
		}
	}
	
	// Leoreth
	for (iI = 0; iI < NUM_DOMAIN_TYPES; iI++)
	{
		changeDomainProductionModifier((DomainTypes)iI, GC.getCivicInfo(eCivic).getDomainProductionModifier(iI) * iChange);
		changeDomainExperienceModifier((DomainTypes)iI, GC.getCivicInfo(eCivic).getDomainExperienceModifier(iI) * iChange);
	}
	
}

void CvPlayer::showMissedMessages()
{
	CvMessageQueue::iterator it = m_listGameMessages.begin();
	while (it != m_listGameMessages.end())
	{
		CvTalkingHeadMessage& msg = *it;
		if (!msg.getShown())
		{
			msg.setShown(true);
			gDLL->getInterfaceIFace()->showMessage(msg);
		}
		++it;
	}
}

bool CvPlayer::isPbemNewTurn() const
{
	return m_bPbemNewTurn;
}

void CvPlayer::setPbemNewTurn(bool bNew)
{
	m_bPbemNewTurn = bNew;
}



//
// read object from a stream
// used during load
//
void CvPlayer::read(FDataStreamBase* pStream)
{
	int iI;

	// Init data before load
	reset();

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(&m_iStartingX);
	pStream->Read(&m_iStartingY);
	pStream->Read(&m_iTotalPopulation);
	pStream->Read(&m_iTotalLand);
	pStream->Read(&m_iTotalLandScored);
	pStream->Read(&m_iGold);
	pStream->Read(&m_iGoldPerTurn);
	pStream->Read(&m_iAdvancedStartPoints);
	pStream->Read(&m_iGoldenAgeTurns);
	pStream->Read(&m_iNumUnitGoldenAges);
	pStream->Read(&m_iStrikeTurns);
	pStream->Read(&m_iAnarchyTurns);
	pStream->Read(&m_iMaxAnarchyTurns);
	pStream->Read(&m_iAnarchyModifier);
	pStream->Read(&m_iGoldenAgeModifier);
	pStream->Read(&m_iGlobalHurryModifier);
	pStream->Read(&m_iGreatPeopleCreated);
	pStream->Read(&m_iGreatGeneralsCreated);
	pStream->Read(&m_iGreatPeopleThresholdModifier);
	pStream->Read(&m_iGreatGeneralsThresholdModifier);
	pStream->Read(&m_iGreatPeopleRateModifier);
	pStream->Read(&m_iNoGreatPeopleCount); // Civ4 Reimagined
	pStream->Read(&m_iNoCultureFlipCount); // Civ4 Reimagined
	pStream->Read(&m_iNoCapitalCount); // Civ4 Reimagined
	pStream->Read(&m_iGreatGeneralRateModifier);
	pStream->Read(&m_iDomesticGreatGeneralRateModifier);
	pStream->Read(&m_iStateReligionGreatPeopleRateModifier);
	pStream->Read(&m_iMaxGlobalBuildingProductionModifier);
	pStream->Read(&m_iMaxTeamBuildingProductionModifier);
	pStream->Read(&m_iMaxPlayerBuildingProductionModifier);
	pStream->Read(&m_iBuildingsProductionModifier); // Civ4 Reimagined
	pStream->Read(&m_iFreeExperience);
	pStream->Read(&m_iFeatureProductionModifier);
	pStream->Read(&m_iWorkerSpeedModifier);
	pStream->Read(&m_iImprovementUpgradeRateModifier);
	pStream->Read(&m_iMilitaryProductionModifier);
	pStream->Read(&m_iSpaceProductionModifier);
	pStream->Read(&m_iCityDefenseModifier);
	pStream->Read(&m_iNumNukeUnits);
	pStream->Read(&m_iNumOutsideUnits);
	pStream->Read(&m_iFreeWorkers); // Civ4 Reimagined
	pStream->Read(&m_iBaseFreeUnits);
	pStream->Read(&m_iBaseFreeMilitaryUnits);
	pStream->Read(&m_iFreeUnitsPopulationPercent);
	pStream->Read(&m_iFreeMilitaryUnitsPopulationPercent);
	pStream->Read(&m_iGoldPerUnit);
	pStream->Read(&m_iGoldPerMilitaryUnit);
	// K-Mod
	if (uiFlag < 3)
	{
		m_iGoldPerUnit *= 100;
		m_iGoldPerMilitaryUnit *= 100;
	}
	// K-Mod end
	pStream->Read(&m_iExtraUnitCost);
	pStream->Read(&m_iNumMilitaryUnits);
	pStream->Read(&m_iNoUnitSupplyCount); // Civ4 Reimagined
	pStream->Read(&m_iHappyPerMilitaryUnit);
	pStream->Read(&m_iMilitaryHappinessLimit); //Leoreth
	pStream->Read(&m_iMilitaryFoodProductionCount);
	pStream->Read(&m_iMeleeMilitaryFoodProductionCount); // Civ4 Reimagined
	pStream->Read(&m_iEnableSlavesCount); // Civ4 Reimagined
	pStream->Read(&m_iSlavesCount); // Civ4 Reimagined
	pStream->Read(&m_iSlavePoints); // Civ4 Reimagined
	pStream->Read(&m_iSlaveThreshold); // Civ4 Reimagined
	pStream->Read(&m_iSlavePointsFromSacrificePopulation); // Civ4 Reimagined
	//pStream->Read(&m_iNumColonies); // Civ4 Reimagined
	pStream->Read(&m_iNoMilitaryProductionMaliCount); // Civ4 Reimagined
	pStream->Read(&m_iConscriptCount);
	pStream->Read(&m_iMaxConscript);
	pStream->Read(&m_iHighestUnitLevel);
	pStream->Read(&m_iHighestNavalUnitLevel); // Civ4 Reimagined
	pStream->Read(&m_iLootingModifier); // Civ4 Reimagined
	pStream->Read(&m_iBarbarianGreatGeneralCount); // Civ4 Reimagined
	pStream->Read(&m_iUnlimitedBarbXPCount); // Civ4 Reimagined
	pStream->Read(&m_iNoWarAgainstSameFaithUnhappinessCount); // Civ4 Reimagined
	pStream->Read(&m_iUnlimitedAnimalXPCount); // Civ4 Reimagined
	pStream->Read(&m_iOverflowResearch);
	//pStream->Read(&m_iNoUnhealthyPopulationCount);
	pStream->Read(&m_iUnhealthyPopulationModifier); // K-Mod
	pStream->Read(&m_iExpInBorderModifier);
	pStream->Read(&m_iBuildingOnlyHealthyCount);
	pStream->Read(&m_iDistanceMaintenanceModifier);
	pStream->Read(&m_iBonusValueModifier); // Civ4 Reimagined
	pStream->Read(&m_iVassalCommerceGain); // Civ4 Reimagined
	pStream->Read(&m_iIsNoPillageCount); // Civ4 Reimagined
	pStream->Read(&m_iNumCitiesMaintenanceModifier);
	pStream->Read(&m_iColonyMaintenanceModifier); // Civ4 Reimagined
	pStream->Read(&m_iCorporationMaintenanceModifier);
	pStream->Read(&m_iProcessModifier); //Leoreth
	pStream->Read(&m_iTotalMaintenance);
	pStream->Read(&m_iUpkeepModifier);
	pStream->Read(&m_iLevelExperienceModifier);
	pStream->Read(&m_iExtraHealth);
	pStream->Read(&m_iPollutionModifier); //Leoreth
	pStream->Read(&m_iPopulationUnhappinessModifier); //Civ4 Reimagined
	pStream->Read(&m_iBuildingGoodHealth);
	pStream->Read(&m_iBuildingBadHealth);
	pStream->Read(&m_iExtraHappiness);
	pStream->Read(&m_iCivicHappiness); // Civ4 Reimagined
	pStream->Read(&m_iNoCapitalUnhappinessCount); //Civ4 Reimagined
	pStream->Read(&m_iBuildingHappiness);
	pStream->Read(&m_iLargestCityHappiness);
	pStream->Read(&m_iGoldPerHappinessBonus); // Civ4 Reimagined
	pStream->Read(&m_iWarWearinessPercentAnger);
	pStream->Read(&m_iWarWearinessModifier);
	pStream->Read(&m_iGwPercentAnger); // K-Mod
	pStream->Read(&m_iFreeSpecialist);
	pStream->Read(&m_iCoreFreeSpecialist); //Leoreth
	pStream->Read(&m_iNoForeignTradeCount);
	pStream->Read(&m_iNoCorporationsCount);
	pStream->Read(&m_iNoForeignCorporationsCount);
	pStream->Read(&m_iCoastalTradeRoutes);
	pStream->Read(&m_iTradeRoutes);
	pStream->Read(&m_iRevolutionTimer);
	pStream->Read(&m_iConversionTimer);
	pStream->Read(&m_iStateReligionCount);
	pStream->Read(&m_iNoNonStateReligionSpreadCount);
	pStream->Read(&m_iStateReligionHappiness);
	pStream->Read(&m_iNonStateReligionHappiness);
	pStream->Read(&m_iStateReligionUnitProductionModifier);
	pStream->Read(&m_iVoteSourceStateReligionUnitProductionModifier); // Civ4 Reimagined
	pStream->Read(&m_iStateReligionBuildingProductionModifier);
	pStream->Read(&m_iStateReligionFreeExperience);
	pStream->Read(&m_iSpecialistExtraYieldBaseThreshold); //Leoreth
	pStream->Read(&m_iSpecialistExtraYieldEraThreshold); //Leoreth
	pStream->Read(&m_iCapitalCityID);
	pStream->Read(&m_iCitiesLost);
	pStream->Read(&m_iWinsVsBarbs);
	pStream->Read(&m_iAssets);
	pStream->Read(&m_iPower);
	pStream->Read(&m_iPopulationScore);
	pStream->Read(&m_iLandScore);
	pStream->Read(&m_iWondersScore);
	pStream->Read(&m_iTechScore);
	pStream->Read(&m_iCombatExperience);
	pStream->Read(&m_iDomesticTradeModifier); // Civ4 Reimagined
	pStream->Read(&m_iColonyTradeModifier); // Civ4 Reimagined
	pStream->Read(&m_iCapitalTradeModifier); // Civ4 Reimagined
	pStream->Read(&m_iUnitUpgradeCostModifier);	 // Civ4 Reimagined
	pStream->Read(&m_iForeignUnitSupply);	 // Civ4 Reimagined
	pStream->Read(&m_iPillageGainModifier);	 // Civ4 Reimagined
	pStream->Read(&m_iTechValue); // Civ4 Reimagined
	pStream->Read(&m_iBonusRatio); // Civ4 Reimagined
	pStream->Read(&m_iResearchPerCulture); // Civ4 Reimagined
	pStream->Read(&m_iFreePopulationInCapital); // Civ4 Reimagined
	pStream->Read((int*)&m_iFreeCivicEnabled); // Civ4 Reimagined
	pStream->Read((int*)&m_iLegacyCivic); // Civ4 Reimagined
	pStream->Read(&m_iEarlyScientistBonusCommerce); // Civ4 Reimagined
	pStream->Read(&m_iEarlyPriestExtraFood); // Civ4 Reimagined
	pStream->Read(&m_iWorldWonderProductionModifier); // Civ4 Reimagined
	pStream->Read(&m_iProductionNearRiver); // Civ4 Reimagined
	pStream->Read(&m_iProductionPerPopulation); // Civ4 Reimagined
	pStream->Read(&m_iHurryWithGreatPriestsRatio); // Civ4 Reimagined
	pStream->Read(&m_iCoastalTradeRouteModifier); // Civ4 Reimagined
	pStream->Read(&m_iOverseaTradeRouteModifier); // Civ4 Reimagined
	pStream->Read(&m_iEspionagePointsOnConquestPerPopulation); // Civ4 Reimagined
	pStream->Read(&m_iUniquePowerGreatPeopleModifier); // Civ4 Reimagined
	pStream->Read(&m_iUniqueUnitFreeExperience); // Civ4 Reimagined
	pStream->Read(&m_iReligionTechModifier); // Civ4 Reimagined
	pStream->Read(&m_iFreeUnitsOnConquest); // Civ4 Reimagined
	pStream->Read(&m_bImmigrants); // Civ4 Reimagined
	pStream->Read(&m_bCaptureSlaves); // Civ4 Reimagined
	pStream->Read(&m_bDesertGold); // Civ4 Reimagined
	pStream->Read(&m_iUniquePowerBuildingModifier); // Civ4 Reimagined
	pStream->Read(&m_iEarlyWorkerSpeedModifier); // Civ4 Reimagined
	pStream->Read(&m_iFatcrossPeakHappiness); // Civ4 Reimagined
	pStream->Read(&m_iFatcrossPeakCulture); // Civ4 Reimagined
	pStream->Read(&m_iNonStateReligionHappinessWithStateReligion); // Civ4 Reimagined
	pStream->Read(&m_bExploreRivalSea); // Civ4 Reimagined
	pStream->Read(&m_bEnableFinancial); // Civ4 Reimagined
	pStream->Read(&m_iMercenaryCostModifier); // Civ4 Reimagined
	pStream->Read(&m_iBuyBuildingCostModifier); // Civ4 Reimagined
	pStream->Read(&m_iCanFarmHillsCount); // Civ4 Reimagined
	pStream->Read(&m_bSpecialTradeRoutePerPlayer); // Civ4 Reimagined
	pStream->Read(&m_bExtraAvailableBonuses); // Civ4 Reimagined
	pStream->Read(&m_iCulturePerPopulationSacrified); // Civ4 Reimagined
	pStream->Read(&m_bCanExploreSea); // Civ4 Reimagined
	pStream->Read(&m_iNoForeignCultureUnhappinessCount); // Civ4 Reimagined
	pStream->Read(&m_iNoCityResistanceCount); // Civ4 Reimagined
	pStream->Read(&m_iProductionPerSurplusHappiness); // Civ4 Reimagined
	pStream->Read(&m_bUpdateBonusRatio); // Civ4 Reimagined
	pStream->Read(&m_bCivicEffect); // Civ4 Reimagined
	pStream->Read(&m_bFaithConquest); // Civ4 Reimagined
	pStream->Read(&m_iLiberatedColonyTradeRouteModifier); // Civ4 Reimagined
	pStream->Read(&m_iCorporationTraderouteModifier); // Civ4 Reimagined
	pStream->Read(&m_iGreatGeneralGoldenAgeLength); // Civ4 Reimagined
	pStream->Read(&m_bConscriptInfidels); // Civ4 Reimagined
	pStream->Read(&m_iCatchUpTechModifier); // Civ4 Reimagined
	pStream->Read(&m_iReligiousColonyMaintenanceModifier); // Civ4 Reimagined
	pStream->Read(&m_iGreatMerchantPointsPerTrade); // Civ4 Reimagined
	pStream->Read(&m_iGreatPeoplePointsPerTrade); // Civ4 Reimagined
	pStream->Read(&m_iCapitalCultureAttitudeBonus); // Civ4 Reimagined
	pStream->Read(&m_iAdditionalAncientEurekaBoost); // Civ4 Reimagined
	pStream->Read(&m_iAdditionalFarmBonusYield); // Civ4 Reimagined
	pStream->Read(&m_iAdditionalMineBonusCommerce); // Civ4 Reimagined
	pStream->Read(&m_iPillageHeal); // Civ4 Reimagined
	pStream->Read(&m_iGreatPeopleRatePerReligionModifier); // Civ4 Reimagined
	pStream->Read(&m_bAlwaysFreshWater); // Civ4 Reimagined
	pStream->Read(&m_bCanRemoveFeatures); // Civ4 Reimagined
	pStream->Read(&m_bCityRevoltOnKill); // Civ4 Reimagined
	pStream->Read(&m_bNoReligionRemoval); // Civ4 Reimagined
	pStream->Read(&m_bCoastalRaid); // Civ4 Reimagined
	pStream->Read(&m_iTradeGoldModifierPerForeignResource); // Civ4 Reimagined
	pStream->Read(&m_bGainGreatWorkGoldWithHitBonuses); // Civ4 Reimagined
	pStream->Read(&m_iReligiousVoteModifier); // Civ4 Reimagined
	pStream->Read(&m_bCapitalAlwaysPerfectBonusValue); // Civ4 Reimagined
	pStream->Read(&m_bCanBuildWindmillsOnCoast); // Civ4 Reimagined
	pStream->Read(&m_iGreatEngineerPointsFromCathedrals); // Civ4 Reimagined
	pStream->Read(&m_iCombatBonusOnHomeArea); // Civ4 Reimagined
	pStream->Read(&m_iStrategicBonusYieldModifier); // Civ4 Reimagined
	pStream->Read(&m_iBuildingProductionModifierFromCapital); // Civ4 Reimagined
	pStream->Read(&m_iCultureResistanceModifier); // Civ4 Reimagined
	pStream->Read(&m_iFreshWaterHealthModifier); // Civ4 Reimagined
	pStream->Read(&m_iStateReligionShrineModifier); // Civ4 Reimagined
	pStream->Read(&m_iLootCityModifier); // Civ4 Reimagined
	pStream->Read(&m_iTechBulbModifier); // Civ4 Reimagined
	pStream->Read(&m_bCityImprovesBonus); // Civ4 Reimagined
	pStream->Read(&m_bFreePillage); // Civ4 Reimagined
	pStream->Read(&m_bPirateGold); // Civ4 Reimagined
	pStream->Read(&m_bIsIgnoreEarlyWonderHurryCostModifier); // Civ4 Reimagined
	pStream->Read(&m_bIsIgnoreForeignTradeBan); // Civ4 Reimagined
	pStream->Read(&m_bAlwaysReceiveGreatPeopleLateTechs); // Civ4 Reimagined
	pStream->Read(&m_bCanGoldenAgeWithSamePeople); // Civ4 Reimagined
	pStream->Read(&m_bLegacyCivic); // Civ4 Reimagined
	pStream->Read(&m_bFrenchRevolution); // Civ4 Reimagined
	
	pStream->Read(&m_bAlive);
	pStream->Read(&m_bEverAlive);
	pStream->Read(&m_bTurnActive);
	pStream->Read(&m_bAutoMoves);
	pStream->Read(&m_bEndTurn);
	pStream->Read(&m_bPbemNewTurn);
	pStream->Read(&m_bExtendedGame);
	pStream->Read(&m_bFoundedFirstCity);
	pStream->Read(&m_bStrike);
	// K-Mod
	if (uiFlag >= 4)
		pStream->Read(&m_iChoosingFreeTechCount);
	else if (uiFlag >= 2)
	{
		bool bFreeTech = false;
		pStream->Read(&bFreeTech);
		m_iChoosingFreeTechCount = bFreeTech ? 1 : 0;
	}
	// K-Mod end

	pStream->Read((int*)&m_eID);
	pStream->Read((int*)&m_ePersonalityType);
	pStream->Read((int*)&m_eCurrentEra);
	pStream->Read((int*)&m_eLastStateReligion);
	pStream->Read((int*)&m_eParent);
	pStream->Read((int*)&m_eIdeology); // Civ4 Reimagined
	pStream->Read((int*)&m_eUniquePowerBuilding); // Civ4 Reimagined
	pStream->Read((int*)&m_eUniquePowerPromotion); // Civ4 Reimagined
	updateTeamType(); //m_eTeamType not saved
	updateHuman();

	pStream->Read(NUM_YIELD_TYPES, m_aiSeaPlotYield);
	pStream->Read(NUM_YIELD_TYPES, m_aiLakePlotYield); // Civ4 Reimagined
	pStream->Read(NUM_YIELD_TYPES, m_aiPeakYield); // Civ4 Reimagined
	pStream->Read(NUM_YIELD_TYPES, m_aiPeakYieldChangeAdjacentToTerrace); // Civ4 Reimagined
	pStream->Read(NUM_YIELD_TYPES, m_aiSpecialistThresholdExtraYield); // Civ4 Reimagined
	pStream->Read(NUM_YIELD_TYPES, m_aiYieldRateModifier);
	pStream->Read(NUM_YIELD_TYPES, m_aiExtraYield); // Civ4 Reimagined
	pStream->Read(NUM_YIELD_TYPES, m_aiPeakAdjacencyExtraYield); // Civ4 Reimagined
	pStream->Read(NUM_YIELD_TYPES, m_aiCityOnHillsExtraYield); // Civ4 Reimagined
	pStream->Read(NUM_YIELD_TYPES, m_aiTownAdjacencyBonus); // Civ4 Reimagined
	pStream->Read(NUM_YIELD_TYPES, m_aiCapitalExtraYieldFromCityPercent); // Civ4 Reimagined
	pStream->Read(NUM_YIELD_TYPES, m_aiExtraYieldPerReligion); // Civ4 Reimagined
	pStream->Read(NUM_YIELD_TYPES, m_aiCapitalYieldRateModifier);
	pStream->Read(NUM_YIELD_TYPES, m_aiExtraYieldThreshold);
	pStream->Read(NUM_YIELD_TYPES, m_aiTradeYieldModifier);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiFreeCityCommerce);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiInitCityCommerce); // Civ4 Reimagined
	pStream->Read(NUM_COMMERCE_TYPES, m_aiUniquePowerCommerceModifier); // Civ4 Reimagined
	pStream->Read(NUM_COMMERCE_TYPES, m_aiCommercePercent);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiCommerceRate);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiCommerceRateModifier);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiCapitalCommerceRateModifier);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiCapitalCommerceRateModifierPerHappinessSurplus); // Civ4 Reimagined
	pStream->Read(NUM_COMMERCE_TYPES, m_aiCommerceHappiness); // Civ4 Reimagined
	pStream->Read(NUM_COMMERCE_TYPES, m_aiStateReligionBuildingCommerce);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiStateReligionCommercePerPopulationOverThreshold); // Civ4 Reimagined
	pStream->Read(NUM_COMMERCE_TYPES, m_aiSpecialistExtraCommerce);
	pStream->Read(NUM_COMMERCE_TYPES, m_aiGreatPeopleExtraCommerceInCapital); // Civ4 Reimagined
	pStream->Read(NUM_COMMERCE_TYPES, m_aiCommerceFlexibleCount);
	pStream->Read(MAX_PLAYERS, m_aiGoldPerTurnByPlayer);
	pStream->Read(MAX_TEAMS, m_aiEspionageSpendingWeightAgainstTeam);
	
	// Leoreth
	pStream->Read(NUM_DOMAIN_TYPES, m_aiDomainProductionModifiers);
	pStream->Read(NUM_DOMAIN_TYPES, m_aiDomainExperienceModifiers);

	pStream->Read(NUM_DOMAIN_TYPES, m_aiDomainExperienceGainModifiers); // Civ4 Reimagined
	pStream->Read(NUM_FEAT_TYPES, m_abFeatAccomplished);
	pStream->Read(NUM_PLAYEROPTION_TYPES, m_abOptions);
	pStream->Read(NUM_NATIVEBONUS_TYPES, m_abNativeAmericanBonus); // Civ4 Reimagined

	pStream->ReadString(m_szScriptData);

	FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but it is expected to be in CvPlayer::read");
	pStream->Read(GC.getNumBonusInfos(), m_paiBonusExport);
	pStream->Read(GC.getNumBonusInfos(), m_paiBonusImport);
	pStream->Read(GC.getNumImprovementInfos(), m_paiImprovementCount);
	pStream->Read(GC.getNumImprovementInfos(), m_paiRadiusImprovementHappiness); // Civ4 Reimagined
	pStream->Read(GC.getNumImprovementInfos(), m_paiGreatSpyPointsFromImprovementInRadius); // Civ4 Reimagined
	pStream->Read(GC.getNumBuildingInfos(), m_paiFreeBuildingCount);
	pStream->Read(GC.getNumBuildingInfos(), m_paiExtraBuildingHappiness);
	pStream->Read(GC.getNumBuildingInfos(), m_paiExtraBuildingHealth);
	pStream->Read(GC.getNumBuildingInfos(), m_paiBuildingProductionModifiers); // Leoreth
	pStream->Read(GC.getNumFeatureInfos(), m_paiFeatureHappiness);
	pStream->Read(GC.getNumUnitClassInfos(), m_paiUnitClassCount);
	pStream->Read(GC.getNumUnitClassInfos(), m_paiUnitClassMaking);
	pStream->Read(GC.getNumBuildingClassInfos(), m_paiBuildingClassCount);
	pStream->Read(GC.getNumBuildingClassInfos(), m_paiBuildingClassMaking);
	pStream->Read(GC.getNumHurryInfos(), m_paiHurryCount);
	pStream->Read(GC.getNumSpecialBuildingInfos(), m_paiSpecialBuildingNotRequiredCount);
	pStream->Read(GC.getNumCivicOptionInfos(), m_paiHasCivicOptionCount);
	pStream->Read(GC.getNumCivicOptionInfos(), m_paiHasNoAnarchyCivicOptionCount); // Civ4 Reimagined
	pStream->Read(GC.getNumCivicOptionInfos(), m_paiNoCivicUpkeepCount);
	pStream->Read(GC.getNumReligionInfos(), m_paiHasReligionCount);
	pStream->Read(GC.getNumCorporationInfos(), m_paiHasCorporationCount);
	pStream->Read(GC.getNumUpkeepInfos(), m_paiUpkeepCount);
	pStream->Read(GC.getNumSpecialistInfos(), m_paiSpecialistValidCount);
	pStream->Read(GC.getNumSpecialistInfos(), m_paiExtraSpecialists); // Civ4 Reimagined
	pStream->Read(GC.getNumSpecialistInfos(), m_paiExtraSpecialistExperience); // Civ4 Reimagined
	pStream->Read(GC.getNumSpecialistInfos(), m_paiExtraSpecialistHappiness); // Civ4 Reimagined
	pStream->Read(GC.getNumSpecialistInfos(), m_paiGreatPeopleRateChangeModifier); // Civ4 Reimagined
	pStream->Read(GC.getNumTerrainInfos(), m_paiFatcrossTerrainHappiness); // Civ4 Reimagined
	pStream->Read(GC.getNumTerrainInfos(), m_paiFatcrossTerrainCulture); // Civ4 Reimagined
	pStream->Read(GC.getNumTerrainInfos(), m_paiTerrainMovementCostModifier); // Civ4 Reimagined
	pStream->Read(NUM_COMMERCE_TYPES, m_paiCapitalCommercePopulationThreshold); // Civ4 Reimagined
	pStream->Read(NUM_COMMERCE_TYPES, m_paiCapitalCommerceModifier); // Civ4 Reimagined
	pStream->Read(NUM_COMMERCE_TYPES, m_paiAveragePopCommerceModifier); // Civ4 Reimagined
	pStream->Read(NUM_COMMERCE_TYPES, m_paiAveragePopCommerceModifierMaxPop); // Civ4 Reimagined
	pStream->Read(NUM_COMMERCE_TYPES, m_paiAveragePopCommerceModifierMaxMod); // Civ4 Reimagined
	pStream->Read(NUM_DOMAIN_TYPES, m_aiMilitaryPower); // Civ4 Reimagined
	pStream->Read(NUM_DOMAIN_TYPES, m_aiBestUnitPower); // Civ4 Reimagined
	pStream->Read(NUM_DOMAIN_TYPES, m_aiExtraMovesInGoldenAge); // Civ4 Reimagined
	pStream->Read(NUM_DOMAIN_TYPES, m_aiDomainGreatGeneralRateModifier); // Civ4 Reimagined
	pStream->Read(NUM_IDEOLOGY_TYPES, m_aiIdeologyInfluence); // Civ4 Reimagined
	pStream->Read(NUM_IDEOLOGY_TYPES, m_aiForeignTradeIdeologyModifier); // Civ4 Reimagined
	pStream->Read(NUM_IDEOLOGY_TYPES, m_aiBonusRatioModifierPerIdeologyCiv); // Civ4 Reimagined
	pStream->Read(NUM_IDEOLOGY_TYPES, m_aiIdeologyCombatExperienceModifier); // Civ4 Reimagined
	pStream->Read(GC.getNumBonusInfos(), m_paiPlayerExtraAvailableBonuses); // Civ4 Reimagined
	pStream->Read(GC.getNumUnitClassInfos(), m_paiUnitClassProductionModifier); // Civ4 Reimagined
	pStream->Read(GC.getNumUnitClassInfos(), m_paiReligiousUnitClassProductionModifier); // Civ4 Reimagined
	pStream->Read(GC.getNumCivicInfos(), m_paiCivicEffect); // Civ4 Reimagined
	pStream->Read(GC.getNumRouteInfos(), m_paiRouteChange); // Civ4 Reimagined

	FAssertMsg((0 < GC.getNumTechInfos()), "GC.getNumTechInfos() is not greater than zero but it is expected to be in CvPlayer::read");
	pStream->Read(GC.getNumTechInfos(), m_pabResearchingTech);

	pStream->Read(GC.getNumVoteSourceInfos(), m_pabLoyalMember);
	pStream->Read(GC.getNumSpecialistInfos(), m_pabUnlimitedSpecialistsWithTemple); // Civ4 Reimagined

	for (iI=0;iI<GC.getNumCivicOptionInfos();iI++)
	{
		pStream->Read((int*)&m_paeCivics[iI]);
	}

	for (iI=0;iI<GC.getNumSpecialistInfos();iI++)
	{
		pStream->Read(NUM_YIELD_TYPES, m_ppaaiSpecialistExtraYield[iI]);
		pStream->Read(NUM_COMMERCE_TYPES, m_ppaaiSpecialistCommerceChanges[iI]); // Civ4 Reimagined
	}

	for (iI=0;iI<GC.getNumImprovementInfos();iI++)
	{
		pStream->Read(NUM_YIELD_TYPES, m_ppaaiImprovementYieldChange[iI]);
		pStream->Read(NUM_YIELD_TYPES, m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus[iI]); // Civ4 Reimagined
		pStream->Read(NUM_COMMERCE_TYPES, m_ppaaiRadiusImprovementCommerceChange[iI]);
	}

	// Civ4 Reimagined
	for (iI=0;iI<GC.getNumTerrainInfos();iI++)
	{
		pStream->Read(NUM_YIELD_TYPES, m_ppaaiTerrainYieldChange[iI]);
	}
	
	// Civ4 Reimagined
	for (iI=0;iI<GC.getNumBuildingClassInfos();iI++)
	{
		pStream->Read(NUM_YIELD_TYPES, m_ppaaiBuildingYieldChange[iI]);
	}

	// Civ4 Reimagined
	for (iI=0;iI<GC.getNumBuildingInfos();iI++)
	{
		pStream->Read(GC.getNumBonusInfos(), m_ppaaiBonusHealthFromBuilding[iI]);
	}

	// Civ4 Reimagined
	for (iI=0;iI<GC.getNumFeatureInfos();iI++)
	{
		pStream->Read(NUM_COMMERCE_TYPES, m_ppaiFeatureCommerce[iI]);
		pStream->Read(NUM_YIELD_TYPES, m_ppaiFeatureExtraYield[iI]);
	}

	m_groupCycle.Read(pStream);
	m_researchQueue.Read(pStream);

	{
		m_cityNames.clear();
		CvWString szBuffer;
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			pStream->ReadString(szBuffer);
			m_cityNames.insertAtEnd(szBuffer);
		}
	}

	ReadStreamableFFreeListTrashArray(m_plotGroups, pStream);
	ReadStreamableFFreeListTrashArray(m_cities, pStream);
	ReadStreamableFFreeListTrashArray(m_units, pStream);
	ReadStreamableFFreeListTrashArray(m_selectionGroups, pStream);
	ReadStreamableFFreeListTrashArray(m_eventsTriggered, pStream);

	{
		CvMessageQueue::_Alloc::size_type iSize;
		pStream->Read(&iSize);
		for (CvMessageQueue::_Alloc::size_type i = 0; i < iSize; i++)
		{
			CvTalkingHeadMessage message;
			message.read(*pStream);
			m_listGameMessages.push_back(message);
		}
	}

	{
		clearPopups();
		CvPopupQueue::_Alloc::size_type iSize;
		pStream->Read(&iSize);
		for (CvPopupQueue::_Alloc::size_type i = 0; i < iSize; i++)
		{
			CvPopupInfo* pInfo = new CvPopupInfo();
			if (NULL != pInfo)
			{
				pInfo->read(*pStream);
				m_listPopups.push_back(pInfo);
			}
		}
	}

	{
		clearDiplomacy();
		CvDiploQueue::_Alloc::size_type iSize;
		pStream->Read(&iSize);
		for (CvDiploQueue::_Alloc::size_type i = 0; i < iSize; i++)
		{
			CvDiploParameters* pDiplo = new CvDiploParameters(NO_PLAYER);
			if (NULL != pDiplo)
			{
				pDiplo->read(*pStream);
				m_listDiplomacy.push_back(pDiplo);
			}
		}
	}

	{
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapScoreHistory[iTurn] = iScore;
		}
	}

	{
		m_mapEconomyHistory.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapEconomyHistory[iTurn] = iScore;
		}
	}

	{
		m_mapIndustryHistory.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapIndustryHistory[iTurn] = iScore;
		}
	}

	{
		m_mapAgricultureHistory.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapAgricultureHistory[iTurn] = iScore;
		}
	}

	{
		m_mapPowerHistory.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapPowerHistory[iTurn] = iScore;
		}
	}

	{
		m_mapCultureHistory.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapCultureHistory[iTurn] = iScore;
		}
	}

	{
		m_mapEspionageHistory.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTurn;
			int iScore;
			pStream->Read(&iTurn);
			pStream->Read(&iScore);
			m_mapEspionageHistory[iTurn] = iScore;
		}
	}

	{
		m_mapEventsOccured.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			EventTriggeredData kData;
			EventTypes eEvent;
			pStream->Read((int*)&eEvent);
			kData.read(pStream);
			m_mapEventsOccured[eEvent] = kData;
		}
	}

	{
		m_mapEventCountdown.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			EventTriggeredData kData;
			EventTypes eEvent;
			pStream->Read((int*)&eEvent);
			kData.read(pStream);
			m_mapEventCountdown[eEvent] = kData;
		}
	}

	{
		m_aFreeUnitCombatPromotions.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iUnitCombat;
			int iPromotion;
			pStream->Read(&iUnitCombat);
			pStream->Read(&iPromotion);
			m_aFreeUnitCombatPromotions.push_back(std::make_pair((UnitCombatTypes)iUnitCombat, (PromotionTypes)iPromotion));
		}
	}

	{
		m_aFreeUnitClassPromotions.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iUnitClass;
			int iPromotion;
			pStream->Read(&iUnitClass);
			pStream->Read(&iPromotion);
			m_aFreeUnitClassPromotions.push_back(std::make_pair((UnitClassTypes)iUnitClass, (PromotionTypes)iPromotion));
		}
	}

	{
		m_aVote.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iId;
			PlayerVoteTypes eVote;
			pStream->Read(&iId);
			pStream->Read((int*)&eVote);
			m_aVote.push_back(std::make_pair(iId, eVote));
		}
	}

	{
		m_aUnitExtraCosts.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iCost;
			UnitClassTypes eUnit;
			pStream->Read((int*)&eUnit);
			pStream->Read(&iCost);
			m_aUnitExtraCosts.push_back(std::make_pair(eUnit, iCost));
		}
	}

	if (uiFlag > 0)
	{
		m_triggersFired.clear();
		uint iSize;
		pStream->Read(&iSize);
		for (uint i = 0; i < iSize; i++)
		{
			int iTrigger;
			pStream->Read(&iTrigger);
			m_triggersFired.push_back((EventTriggerTypes)iTrigger);
		}
	}
	else
	{
		int iNumEventTriggers = std::min(176, GC.getNumEventTriggerInfos()); // yuck, hardcoded number of eventTriggers in the epic game in initial release
		for (iI=0; iI < iNumEventTriggers; iI++) 
		{
			bool bTriggered;
			pStream->Read(&bTriggered);
			if (bTriggered)
			{
				m_triggersFired.push_back((EventTriggerTypes)iI);
			}
		}
	}

	if (!isBarbarian())
	{
		// Get the NetID from the initialization structure
		setNetID(gDLL->getAssignedNetworkID(getID()));
	}

	pStream->Read(&m_iPopRushHurryCount);
	pStream->Read(&m_iInflationModifier);
	// K-Mod
	if (uiFlag >= 5)
		pStream->Read(&m_iInflationRate);
	else
		m_iInflationRate = 0; // We can't use updateInflationRate, because that relies on game-state which may not have been loaded yet.
	// K-Mod end
}

//
// save object to a stream
// used during save
//
void CvPlayer::write(FDataStreamBase* pStream)
{
	int iI;

	uint uiFlag = 5;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_iStartingX);
	pStream->Write(m_iStartingY);
	pStream->Write(m_iTotalPopulation);
	pStream->Write(m_iTotalLand);
	pStream->Write(m_iTotalLandScored);
	pStream->Write(m_iGold);
	pStream->Write(m_iGoldPerTurn);
	pStream->Write(m_iAdvancedStartPoints);
	pStream->Write(m_iGoldenAgeTurns);
	pStream->Write(m_iNumUnitGoldenAges);
	pStream->Write(m_iStrikeTurns);
	pStream->Write(m_iAnarchyTurns);
	pStream->Write(m_iMaxAnarchyTurns);
	pStream->Write(m_iAnarchyModifier);
	pStream->Write(m_iGoldenAgeModifier);
	pStream->Write(m_iGlobalHurryModifier);
	pStream->Write(m_iGreatPeopleCreated);
	pStream->Write(m_iGreatGeneralsCreated);
	pStream->Write(m_iGreatPeopleThresholdModifier);
	pStream->Write(m_iGreatGeneralsThresholdModifier);
	pStream->Write(m_iGreatPeopleRateModifier);
	pStream->Write(m_iNoGreatPeopleCount); // Civ4 Reimagined
	pStream->Write(m_iNoCultureFlipCount); // Civ4 Reimagined
	pStream->Write(m_iNoCapitalCount); // Civ4 Reimagined
	pStream->Write(m_iGreatGeneralRateModifier);
	pStream->Write(m_iDomesticGreatGeneralRateModifier);
	pStream->Write(m_iStateReligionGreatPeopleRateModifier);
	pStream->Write(m_iMaxGlobalBuildingProductionModifier);
	pStream->Write(m_iMaxTeamBuildingProductionModifier);
	pStream->Write(m_iMaxPlayerBuildingProductionModifier);
	pStream->Write(m_iBuildingsProductionModifier); // Civ4 Reimagined
	pStream->Write(m_iFreeExperience);
	pStream->Write(m_iFeatureProductionModifier);
	pStream->Write(m_iWorkerSpeedModifier);
	pStream->Write(m_iImprovementUpgradeRateModifier);
	pStream->Write(m_iMilitaryProductionModifier);
	pStream->Write(m_iSpaceProductionModifier);
	pStream->Write(m_iCityDefenseModifier);
	pStream->Write(m_iNumNukeUnits);
	pStream->Write(m_iNumOutsideUnits);
	pStream->Write(m_iFreeWorkers); // Civ4 Reimagined
	pStream->Write(m_iBaseFreeUnits);
	pStream->Write(m_iBaseFreeMilitaryUnits);
	pStream->Write(m_iFreeUnitsPopulationPercent);
	pStream->Write(m_iFreeMilitaryUnitsPopulationPercent);
	pStream->Write(m_iGoldPerUnit);
	pStream->Write(m_iGoldPerMilitaryUnit);
	pStream->Write(m_iExtraUnitCost);
	pStream->Write(m_iNumMilitaryUnits);
	pStream->Write(m_iNoUnitSupplyCount); // Civ4 Reimagined
	pStream->Write(m_iHappyPerMilitaryUnit);
	pStream->Write(m_iMilitaryHappinessLimit); //Leoreth
	pStream->Write(m_iMilitaryFoodProductionCount);
	pStream->Write(m_iMeleeMilitaryFoodProductionCount); // Civ4 Reimagined
	pStream->Write(m_iEnableSlavesCount); // Civ4 Reimagined
	pStream->Write(m_iSlavesCount); // Civ4 Reimagined
	pStream->Write(m_iSlavePoints); // Civ4 Reimagined
	pStream->Write(m_iSlaveThreshold); // Civ4 Reimagined
	pStream->Write(m_iSlavePointsFromSacrificePopulation); // Civ4 Reimagined
	//pStream->Write(m_iNumColonies); // Civ4 Reimagined
	pStream->Write(m_iNoMilitaryProductionMaliCount); // Civ4 Reimagined
	pStream->Write(m_iConscriptCount);
	pStream->Write(m_iMaxConscript);
	pStream->Write(m_iHighestUnitLevel);
	pStream->Write(m_iHighestNavalUnitLevel); // Civ4 Reimagined
	pStream->Write(m_iLootingModifier); // Civ4 Reimagined
	pStream->Write(m_iBarbarianGreatGeneralCount); // Civ4 Reimagined
	pStream->Write(m_iUnlimitedBarbXPCount); // Civ4 Reimagined
	pStream->Write(m_iNoWarAgainstSameFaithUnhappinessCount); // Civ4 Reimagined
	pStream->Write(m_iUnlimitedAnimalXPCount); // Civ4 Reimagined
	pStream->Write(m_iOverflowResearch);
	//pStream->Write(m_iNoUnhealthyPopulationCount);
	pStream->Write(m_iUnhealthyPopulationModifier); // K-Mod
	pStream->Write(m_iExpInBorderModifier);
	pStream->Write(m_iBuildingOnlyHealthyCount);
	pStream->Write(m_iDistanceMaintenanceModifier);
	pStream->Write(m_iBonusValueModifier); // Civ4 Reimagined
	pStream->Write(m_iVassalCommerceGain); // Civ4 Reimagined
	pStream->Write(m_iIsNoPillageCount); // Civ4 Reimagined
	pStream->Write(m_iNumCitiesMaintenanceModifier);
	pStream->Write(m_iColonyMaintenanceModifier); // Civ4 Reimagined
	pStream->Write(m_iCorporationMaintenanceModifier);
	pStream->Write(m_iProcessModifier); //Leoreth
	pStream->Write(m_iTotalMaintenance);
	pStream->Write(m_iUpkeepModifier);
	pStream->Write(m_iLevelExperienceModifier);
	pStream->Write(m_iExtraHealth);
	pStream->Write(m_iPollutionModifier); //Leoreth
	pStream->Write(m_iPopulationUnhappinessModifier); //Civ4 Reimagined
	pStream->Write(m_iBuildingGoodHealth);
	pStream->Write(m_iBuildingBadHealth);
	pStream->Write(m_iExtraHappiness);
	pStream->Write(m_iCivicHappiness); // Civ4 Reimagined
	pStream->Write(m_iNoCapitalUnhappinessCount); // Civ4 Reimagined
	pStream->Write(m_iBuildingHappiness);
	pStream->Write(m_iLargestCityHappiness);
	pStream->Write(m_iGoldPerHappinessBonus); // Civ4 Reimagined
	pStream->Write(m_iWarWearinessPercentAnger);
	pStream->Write(m_iWarWearinessModifier);
	pStream->Write(m_iGwPercentAnger); // K-Mod
	pStream->Write(m_iFreeSpecialist);
	pStream->Write(m_iCoreFreeSpecialist); //Leoreth
	pStream->Write(m_iNoForeignTradeCount);
	pStream->Write(m_iNoCorporationsCount);
	pStream->Write(m_iNoForeignCorporationsCount);
	pStream->Write(m_iCoastalTradeRoutes);
	pStream->Write(m_iTradeRoutes);
	pStream->Write(m_iRevolutionTimer);
	pStream->Write(m_iConversionTimer);
	pStream->Write(m_iStateReligionCount);
	pStream->Write(m_iNoNonStateReligionSpreadCount);
	pStream->Write(m_iStateReligionHappiness);
	pStream->Write(m_iNonStateReligionHappiness);
	pStream->Write(m_iStateReligionUnitProductionModifier);
	pStream->Write(m_iVoteSourceStateReligionUnitProductionModifier); // Civ4 Reimagined
	pStream->Write(m_iStateReligionBuildingProductionModifier);
	pStream->Write(m_iStateReligionFreeExperience);
	pStream->Write(m_iSpecialistExtraYieldBaseThreshold); //Leoreth
	pStream->Write(m_iSpecialistExtraYieldEraThreshold); //Leoreth
	pStream->Write(m_iCapitalCityID);
	pStream->Write(m_iCitiesLost);
	pStream->Write(m_iWinsVsBarbs);
	pStream->Write(m_iAssets);
	pStream->Write(m_iPower);
	pStream->Write(m_iPopulationScore);
	pStream->Write(m_iLandScore);
	pStream->Write(m_iWondersScore);
	pStream->Write(m_iTechScore);
	pStream->Write(m_iCombatExperience);
	pStream->Write(m_iDomesticTradeModifier); //Civ4 Reimagined
	pStream->Write(m_iColonyTradeModifier); //Civ4 Reimagined
	pStream->Write(m_iCapitalTradeModifier); //Civ4 Reimagined
	pStream->Write(m_iUnitUpgradeCostModifier); //Civ4 Reimagined
	pStream->Write(m_iForeignUnitSupply); //Civ4 Reimagined
	pStream->Write(m_iPillageGainModifier); //Civ4 Reimagined
	pStream->Write(m_iTechValue); //Civ4 Reimagined
	pStream->Write(m_iBonusRatio); //Civ4 Reimagined
	pStream->Write(m_iResearchPerCulture); // Civ4 Reimagined
	pStream->Write(m_iFreePopulationInCapital); // Civ4 Reimagined
	pStream->Write(m_iFreeCivicEnabled); // Civ4 Reimagined
	pStream->Write(m_iLegacyCivic); // Civ4 Reimagined
	pStream->Write(m_iEarlyScientistBonusCommerce); // Civ4 Reimagined
	pStream->Write(m_iEarlyPriestExtraFood); // Civ4 Reimagined
	pStream->Write(m_iWorldWonderProductionModifier); // Civ4 Reimagined
	pStream->Write(m_iProductionNearRiver); // Civ4 Reimagined
	pStream->Write(m_iProductionPerPopulation); // Civ4 Reimagined
	pStream->Write(m_iHurryWithGreatPriestsRatio); // Civ4 Reimagined
	pStream->Write(m_iCoastalTradeRouteModifier); // Civ4 Reimagined
	pStream->Write(m_iOverseaTradeRouteModifier); // Civ4 Reimagined
	pStream->Write(m_iEspionagePointsOnConquestPerPopulation); // Civ4 Reimagined
	pStream->Write(m_iUniquePowerGreatPeopleModifier); // Civ4 Reimagined
	pStream->Write(m_iUniqueUnitFreeExperience); // Civ4 Reimagined	
	pStream->Write(m_iReligionTechModifier); // Civ4 Reimagined	
	pStream->Write(m_iFreeUnitsOnConquest); // Civ4 Reimagined	
	pStream->Write(m_bImmigrants); // Civ4 Reimagined
	pStream->Write(m_bCaptureSlaves); // Civ4 Reimagined
	pStream->Write(m_bDesertGold); // Civ4 Reimagined
	pStream->Write(m_iUniquePowerBuildingModifier); // Civ4 Reimagined
	pStream->Write(m_iEarlyWorkerSpeedModifier); // Civ4 Reimagined
	pStream->Write(m_iFatcrossPeakHappiness); // Civ4 Reimagined
	pStream->Write(m_iFatcrossPeakCulture); // Civ4 Reimagined
	pStream->Write(m_iNonStateReligionHappinessWithStateReligion); // Civ4 Reimagined
	pStream->Write(m_bExploreRivalSea); // Civ4 Reimagined
	pStream->Write(m_bEnableFinancial); // Civ4 Reimagined
	pStream->Write(m_iMercenaryCostModifier); // Civ4 Reimagined
	pStream->Write(m_iBuyBuildingCostModifier); // Civ4 Reimagined
	pStream->Write(m_iCanFarmHillsCount); // Civ4 Reimagined
	pStream->Write(m_bSpecialTradeRoutePerPlayer); // Civ4 Reimagined
	pStream->Write(m_bExtraAvailableBonuses); // Civ4 Reimagined
	pStream->Write(m_iCulturePerPopulationSacrified); // Civ4 Reimagined
	pStream->Write(m_bCanExploreSea); // Civ4 Reimagined
	pStream->Write(m_iNoForeignCultureUnhappinessCount); // Civ4 Reimagined
	pStream->Write(m_iNoCityResistanceCount); // Civ4 Reimagined
	pStream->Write(m_iProductionPerSurplusHappiness); // Civ4 Reimagined
	pStream->Write(m_bUpdateBonusRatio); // Civ4 Reimagined
	pStream->Write(m_bCivicEffect); // Civ4 Reimagined
	pStream->Write(m_bFaithConquest); // Civ4 Reimagined
	pStream->Write(m_iLiberatedColonyTradeRouteModifier); // Civ4 Reimagined
	pStream->Write(m_iCorporationTraderouteModifier); // Civ4 Reimagined
	pStream->Write(m_iGreatGeneralGoldenAgeLength); // Civ4 Reimagined
	pStream->Write(m_bConscriptInfidels); // Civ4 Reimagined
	pStream->Write(m_iCatchUpTechModifier); // Civ4 Reimagined
	pStream->Write(m_iReligiousColonyMaintenanceModifier); // Civ4 Reimagined
	pStream->Write(m_iGreatMerchantPointsPerTrade); // Civ4 Reimagined
	pStream->Write(m_iGreatPeoplePointsPerTrade); // Civ4 Reimagined
	pStream->Write(m_iCapitalCultureAttitudeBonus); // Civ4 Reimagined
	pStream->Write(m_iAdditionalAncientEurekaBoost); // Civ4 Reimagined
	pStream->Write(m_iAdditionalFarmBonusYield); // Civ4 Reimagined
	pStream->Write(m_iAdditionalMineBonusCommerce); // Civ4 Reimagined
	pStream->Write(m_iPillageHeal); // Civ4 Reimagined
	pStream->Write(m_iGreatPeopleRatePerReligionModifier); // Civ4 Reimagined
	pStream->Write(m_bAlwaysFreshWater); // Civ4 Reimagined
	pStream->Write(m_bCanRemoveFeatures); // Civ4 Reimagined
	pStream->Write(m_bCityRevoltOnKill); // Civ4 Reimagined
	pStream->Write(m_bNoReligionRemoval); // Civ4 Reimagined
	pStream->Write(m_bCoastalRaid); // Civ4 Reimagined
	pStream->Write(m_iTradeGoldModifierPerForeignResource); // Civ4 Reimagined
	pStream->Write(m_bGainGreatWorkGoldWithHitBonuses); // Civ4 Reimagined
	pStream->Write(m_iReligiousVoteModifier); // Civ4 Reimagined
	pStream->Write(m_bCapitalAlwaysPerfectBonusValue); // Civ4 Reimagined
	pStream->Write(m_bCanBuildWindmillsOnCoast); // Civ4 Reimagined
	pStream->Write(m_iGreatEngineerPointsFromCathedrals); // Civ4 Reimagined
	pStream->Write(m_iCombatBonusOnHomeArea); // Civ4 Reimagined
	pStream->Write(m_iStrategicBonusYieldModifier); // Civ4 Reimagined
	pStream->Write(m_iBuildingProductionModifierFromCapital); // Civ4 Reimagined
	pStream->Write(m_iCultureResistanceModifier); // Civ4 Reimagined
	pStream->Write(m_iFreshWaterHealthModifier); // Civ4 Reimagined
	pStream->Write(m_iStateReligionShrineModifier); // Civ4 Reimagined
	pStream->Write(m_iLootCityModifier); // Civ4 Reimagined
	pStream->Write(m_iTechBulbModifier); // Civ4 Reimagined
	pStream->Write(m_bCityImprovesBonus); // Civ4 Reimagined
	pStream->Write(m_bFreePillage); // Civ4 Reimagined
	pStream->Write(m_bPirateGold); // Civ4 Reimagined
	pStream->Write(m_bIsIgnoreEarlyWonderHurryCostModifier); // Civ4 Reimagined
	pStream->Write(m_bIsIgnoreForeignTradeBan); // Civ4 Reimagined
	pStream->Write(m_bAlwaysReceiveGreatPeopleLateTechs); // Civ4 Reimagined
	pStream->Write(m_bCanGoldenAgeWithSamePeople); // Civ4 Reimagined
	pStream->Write(m_bLegacyCivic); // Civ4 Reimagined
	pStream->Write(m_bFrenchRevolution); // Civ4 Reimagined

	pStream->Write(m_bAlive);
	pStream->Write(m_bEverAlive);
	pStream->Write(m_bTurnActive);
	pStream->Write(m_bAutoMoves);
	pStream->Write(m_bEndTurn);
	pStream->Write(m_bPbemNewTurn && GC.getGameINLINE().isPbem());
	pStream->Write(m_bExtendedGame);
	pStream->Write(m_bFoundedFirstCity);
	pStream->Write(m_bStrike);
	pStream->Write(m_iChoosingFreeTechCount); // K-Mod (bool for 2 <= uiFlag < 4. then int.)

	pStream->Write(m_eID);
	pStream->Write(m_ePersonalityType);
	pStream->Write(m_eCurrentEra);
	pStream->Write(m_eLastStateReligion);
	pStream->Write(m_eParent);
	pStream->Write(m_eIdeology); // Civ4 Reimagined
	pStream->Write(m_eUniquePowerBuilding); // Civ4 Reimagined
	pStream->Write(m_eUniquePowerPromotion); // Civ4 Reimagined
	//m_eTeamType not saved

	pStream->Write(NUM_YIELD_TYPES, m_aiSeaPlotYield);
	pStream->Write(NUM_YIELD_TYPES, m_aiLakePlotYield); // Civ4 Reimagined
	pStream->Write(NUM_YIELD_TYPES, m_aiPeakYield); // Civ4 Reimagined
	pStream->Write(NUM_YIELD_TYPES, m_aiPeakYieldChangeAdjacentToTerrace); // Civ4 Reimagined
	pStream->Write(NUM_YIELD_TYPES, m_aiSpecialistThresholdExtraYield); // Civ4 Reimagined
	pStream->Write(NUM_YIELD_TYPES, m_aiYieldRateModifier);
	pStream->Write(NUM_YIELD_TYPES, m_aiExtraYield); // Civ4 Reimagined
	pStream->Write(NUM_YIELD_TYPES, m_aiPeakAdjacencyExtraYield); // Civ4 Reimagined
	pStream->Write(NUM_YIELD_TYPES, m_aiCityOnHillsExtraYield); // Civ4 Reimagined
	pStream->Write(NUM_YIELD_TYPES, m_aiTownAdjacencyBonus); // Civ4 Reimagined
	pStream->Write(NUM_YIELD_TYPES, m_aiCapitalExtraYieldFromCityPercent); // Civ4 Reimagined
	pStream->Write(NUM_YIELD_TYPES, m_aiExtraYieldPerReligion); // Civ4 Reimagined
	pStream->Write(NUM_YIELD_TYPES, m_aiCapitalYieldRateModifier);
	pStream->Write(NUM_YIELD_TYPES, m_aiExtraYieldThreshold);
	pStream->Write(NUM_YIELD_TYPES, m_aiTradeYieldModifier);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiFreeCityCommerce);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiInitCityCommerce); // Civ4 Reimagined
	pStream->Write(NUM_COMMERCE_TYPES, m_aiUniquePowerCommerceModifier); // Civ4 Reimagined
	pStream->Write(NUM_COMMERCE_TYPES, m_aiCommercePercent);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiCommerceRate);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiCommerceRateModifier);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiCapitalCommerceRateModifier);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiCapitalCommerceRateModifierPerHappinessSurplus); // Civ4 Reimagined
	pStream->Write(NUM_COMMERCE_TYPES, m_aiCommerceHappiness); // Civ4 Reimagined
	pStream->Write(NUM_COMMERCE_TYPES, m_aiStateReligionBuildingCommerce);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiStateReligionCommercePerPopulationOverThreshold); // Civ4 Reimagined
	pStream->Write(NUM_COMMERCE_TYPES, m_aiSpecialistExtraCommerce);
	pStream->Write(NUM_COMMERCE_TYPES, m_aiGreatPeopleExtraCommerceInCapital); // Civ4 Reimagined
	pStream->Write(NUM_COMMERCE_TYPES, m_aiCommerceFlexibleCount);
	pStream->Write(MAX_PLAYERS, m_aiGoldPerTurnByPlayer);
	pStream->Write(MAX_TEAMS, m_aiEspionageSpendingWeightAgainstTeam);
	
	// Leoreth
	pStream->Write(NUM_DOMAIN_TYPES, m_aiDomainProductionModifiers);
	pStream->Write(NUM_DOMAIN_TYPES, m_aiDomainExperienceModifiers);

	pStream->Write(NUM_DOMAIN_TYPES, m_aiDomainExperienceGainModifiers); // Civ4 Reimagined
	pStream->Write(NUM_FEAT_TYPES, m_abFeatAccomplished);
	pStream->Write(NUM_PLAYEROPTION_TYPES, m_abOptions);
	pStream->Write(NUM_NATIVEBONUS_TYPES, m_abNativeAmericanBonus); // Civ4 Reimagined

	pStream->WriteString(m_szScriptData);

	FAssertMsg((0 < GC.getNumBonusInfos()), "GC.getNumBonusInfos() is not greater than zero but an array is being allocated in CvPlayer::write");
	pStream->Write(GC.getNumBonusInfos(), m_paiBonusExport);
	pStream->Write(GC.getNumBonusInfos(), m_paiBonusImport);
	pStream->Write(GC.getNumImprovementInfos(), m_paiImprovementCount);
	pStream->Write(GC.getNumImprovementInfos(), m_paiRadiusImprovementHappiness); // Civ4 Reimagined
	pStream->Write(GC.getNumImprovementInfos(), m_paiGreatSpyPointsFromImprovementInRadius); // Civ4 Reimagined
	pStream->Write(GC.getNumBuildingInfos(), m_paiFreeBuildingCount);
	pStream->Write(GC.getNumBuildingInfos(), m_paiExtraBuildingHappiness);
	pStream->Write(GC.getNumBuildingInfos(), m_paiExtraBuildingHealth);
	pStream->Write(GC.getNumBuildingInfos(), m_paiBuildingProductionModifiers); // Leoreth
	pStream->Write(GC.getNumFeatureInfos(), m_paiFeatureHappiness);
	pStream->Write(GC.getNumUnitClassInfos(), m_paiUnitClassCount);
	pStream->Write(GC.getNumUnitClassInfos(), m_paiUnitClassMaking);
	pStream->Write(GC.getNumBuildingClassInfos(), m_paiBuildingClassCount);
	pStream->Write(GC.getNumBuildingClassInfos(), m_paiBuildingClassMaking);
	pStream->Write(GC.getNumHurryInfos(), m_paiHurryCount);
	pStream->Write(GC.getNumSpecialBuildingInfos(), m_paiSpecialBuildingNotRequiredCount);
	pStream->Write(GC.getNumCivicOptionInfos(), m_paiHasCivicOptionCount);
	pStream->Write(GC.getNumCivicOptionInfos(), m_paiHasNoAnarchyCivicOptionCount); // Civ4 Reimagined
	pStream->Write(GC.getNumCivicOptionInfos(), m_paiNoCivicUpkeepCount);
	pStream->Write(GC.getNumReligionInfos(), m_paiHasReligionCount);
	pStream->Write(GC.getNumCorporationInfos(), m_paiHasCorporationCount);
	pStream->Write(GC.getNumUpkeepInfos(), m_paiUpkeepCount);
	pStream->Write(GC.getNumSpecialistInfos(), m_paiSpecialistValidCount);
	pStream->Write(GC.getNumSpecialistInfos(), m_paiExtraSpecialists); // Civ4 Reimagined
	pStream->Write(GC.getNumSpecialistInfos(), m_paiExtraSpecialistExperience); // Civ4 Reimagined
	pStream->Write(GC.getNumSpecialistInfos(), m_paiExtraSpecialistHappiness); // Civ4 Reimagined
	pStream->Write(GC.getNumSpecialistInfos(), m_paiGreatPeopleRateChangeModifier); // Civ4 Reimagined
	pStream->Write(GC.getNumTerrainInfos(), m_paiFatcrossTerrainHappiness); // Civ4 Reimagined
	pStream->Write(GC.getNumTerrainInfos(), m_paiFatcrossTerrainCulture); // Civ4 Reimagined
	pStream->Write(GC.getNumTerrainInfos(), m_paiTerrainMovementCostModifier); // Civ4 Reimagined
	pStream->Write(NUM_COMMERCE_TYPES, m_paiCapitalCommercePopulationThreshold); // Civ4 Reimagined
	pStream->Write(NUM_COMMERCE_TYPES, m_paiCapitalCommerceModifier); // Civ4 Reimagined
	pStream->Write(NUM_COMMERCE_TYPES, m_paiAveragePopCommerceModifier); // Civ4 Reimagined
	pStream->Write(NUM_COMMERCE_TYPES, m_paiAveragePopCommerceModifierMaxPop); // Civ4 Reimagined
	pStream->Write(NUM_COMMERCE_TYPES, m_paiAveragePopCommerceModifierMaxMod); // Civ4 Reimagined
	pStream->Write(NUM_DOMAIN_TYPES, m_aiMilitaryPower); // Civ4 Reimagined
	pStream->Write(NUM_DOMAIN_TYPES, m_aiBestUnitPower); // Civ4 Reimagined
	pStream->Write(NUM_DOMAIN_TYPES, m_aiExtraMovesInGoldenAge); // Civ4 Reimagined
	pStream->Write(NUM_DOMAIN_TYPES, m_aiDomainGreatGeneralRateModifier); // Civ4 Reimagined
	pStream->Write(NUM_IDEOLOGY_TYPES, m_aiIdeologyInfluence); // Civ4 Reimagined
	pStream->Write(NUM_IDEOLOGY_TYPES, m_aiForeignTradeIdeologyModifier); // Civ4 Reimagined
	pStream->Write(NUM_IDEOLOGY_TYPES, m_aiBonusRatioModifierPerIdeologyCiv); // Civ4 Reimagined
	pStream->Write(NUM_IDEOLOGY_TYPES, m_aiIdeologyCombatExperienceModifier); // Civ4 Reimagined
	pStream->Write(GC.getNumBonusInfos(), m_paiPlayerExtraAvailableBonuses); // Civ4 Reimagined
	pStream->Write(GC.getNumUnitClassInfos(), m_paiUnitClassProductionModifier); // Civ4 Reimagined
	pStream->Write(GC.getNumUnitClassInfos(), m_paiReligiousUnitClassProductionModifier); // Civ4 Reimagined
	pStream->Write(GC.getNumCivicInfos(), m_paiCivicEffect); // Civ4 Reimagined
	pStream->Write(GC.getNumRouteInfos(), m_paiRouteChange); // Civ4 Reimagined
	
	FAssertMsg((0 < GC.getNumTechInfos()), "GC.getNumTechInfos() is not greater than zero but it is expected to be in CvPlayer::write");
	pStream->Write(GC.getNumTechInfos(), m_pabResearchingTech);

	pStream->Write(GC.getNumVoteSourceInfos(), m_pabLoyalMember);
	pStream->Write(GC.getNumSpecialistInfos(), m_pabUnlimitedSpecialistsWithTemple); // Civ4 Reimagined

	for (iI=0;iI<GC.getNumCivicOptionInfos();iI++)
	{
		pStream->Write(m_paeCivics[iI]);
	}

	for (iI=0;iI<GC.getNumSpecialistInfos();iI++)
	{
		pStream->Write(NUM_YIELD_TYPES, m_ppaaiSpecialistExtraYield[iI]);
		pStream->Write(NUM_COMMERCE_TYPES, m_ppaaiSpecialistCommerceChanges[iI]); // Civ4 Reimagined
	}

	for (iI=0;iI<GC.getNumImprovementInfos();iI++)
	{
		pStream->Write(NUM_YIELD_TYPES, m_ppaaiImprovementYieldChange[iI]);
		pStream->Write(NUM_YIELD_TYPES, m_ppaaiImprovementYieldChangeAdjacentToStrategicBonus[iI]); // Civ4 Reimagined
		pStream->Write(NUM_COMMERCE_TYPES, m_ppaaiRadiusImprovementCommerceChange[iI]);
	}

	// Civ4 Reimagined
	for (iI=0;iI<GC.getNumTerrainInfos();iI++)
	{
		pStream->Write(NUM_YIELD_TYPES, m_ppaaiTerrainYieldChange[iI]);
	}
	
	// Civ4 Reimagined
	for (iI=0;iI<GC.getNumBuildingClassInfos();iI++)
	{
		pStream->Write(NUM_YIELD_TYPES, m_ppaaiBuildingYieldChange[iI]);
	}

	// Civ4 Reimagined
	for (iI=0;iI<GC.getNumBuildingInfos();iI++)
	{
		pStream->Write(GC.getNumBonusInfos(), m_ppaaiBonusHealthFromBuilding[iI]);
	}

	// Civ4 Reimagined
	for (iI=0;iI<GC.getNumFeatureInfos();iI++)
	{
		pStream->Write(NUM_COMMERCE_TYPES, m_ppaiFeatureCommerce[iI]);
		pStream->Write(NUM_YIELD_TYPES, m_ppaiFeatureExtraYield[iI]);
	}

	m_groupCycle.Write(pStream);
	m_researchQueue.Write(pStream);

	{
		CLLNode<CvWString>* pNode;
		uint iSize = m_cityNames.getLength();
		pStream->Write(iSize);
		pNode = m_cityNames.head();
		while (pNode != NULL)
		{
			pStream->WriteString(pNode->m_data);
			pNode = m_cityNames.next(pNode);
		}
	}

	WriteStreamableFFreeListTrashArray(m_plotGroups, pStream);
	WriteStreamableFFreeListTrashArray(m_cities, pStream);
	WriteStreamableFFreeListTrashArray(m_units, pStream);
	WriteStreamableFFreeListTrashArray(m_selectionGroups, pStream);
	WriteStreamableFFreeListTrashArray(m_eventsTriggered, pStream);

	{
		CvMessageQueue::_Alloc::size_type iSize = m_listGameMessages.size();
		pStream->Write(iSize);
		CvMessageQueue::iterator it;
		for (it = m_listGameMessages.begin(); it != m_listGameMessages.end(); ++it)
		{
			CvTalkingHeadMessage& message = *it;
			message.write(*pStream);
		}
	}

	{
		CvPopupQueue currentPopups;
		if (GC.getGameINLINE().isNetworkMultiPlayer())
		{
			// don't save open popups in MP to avoid having different state on different machines
			currentPopups.clear();
		}
		else
		{
			gDLL->getInterfaceIFace()->getDisplayedButtonPopups(currentPopups);
		}
		CvPopupQueue::_Alloc::size_type iSize = m_listPopups.size() + currentPopups.size();
		pStream->Write(iSize);
		CvPopupQueue::iterator it;
		for (it = currentPopups.begin(); it != currentPopups.end(); ++it)
		{
			CvPopupInfo* pInfo = *it;
			if (NULL != pInfo)
			{
				pInfo->write(*pStream);
			}
		}
		for (it = m_listPopups.begin(); it != m_listPopups.end(); ++it)
		{
			CvPopupInfo* pInfo = *it;
			if (NULL != pInfo)
			{
				pInfo->write(*pStream);
			}
		}
	}

	{
		CvDiploQueue::_Alloc::size_type iSize = m_listDiplomacy.size();
		pStream->Write(iSize);
		CvDiploQueue::iterator it;
		for (it = m_listDiplomacy.begin(); it != m_listDiplomacy.end(); ++it)
		{
			CvDiploParameters* pDiplo = *it;
			if (NULL != pDiplo)
			{
				pDiplo->write(*pStream);
			}
		}
	}

	{
		uint iSize = m_mapScoreHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapScoreHistory.begin(); it != m_mapScoreHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapEconomyHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapEconomyHistory.begin(); it != m_mapEconomyHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapIndustryHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapIndustryHistory.begin(); it != m_mapIndustryHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapAgricultureHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapAgricultureHistory.begin(); it != m_mapAgricultureHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapPowerHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapPowerHistory.begin(); it != m_mapPowerHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapCultureHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapCultureHistory.begin(); it != m_mapCultureHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapEspionageHistory.size();
		pStream->Write(iSize);
		CvTurnScoreMap::iterator it;
		for (it = m_mapEspionageHistory.begin(); it != m_mapEspionageHistory.end(); ++it)
		{
			pStream->Write(it->first);
			pStream->Write(it->second);
		}
	}

	{
		uint iSize = m_mapEventsOccured.size();
		pStream->Write(iSize);
		CvEventMap::iterator it;
		for (it = m_mapEventsOccured.begin(); it != m_mapEventsOccured.end(); ++it)
		{
			pStream->Write(it->first);
			it->second.write(pStream);
		}
	}

	{
		uint iSize = m_mapEventCountdown.size();
		pStream->Write(iSize);
		CvEventMap::iterator it;
		for (it = m_mapEventCountdown.begin(); it != m_mapEventCountdown.end(); ++it)
		{
			pStream->Write(it->first);
			it->second.write(pStream);
		}
	}

	{
		uint iSize = m_aFreeUnitCombatPromotions.size();
		pStream->Write(iSize);
		UnitCombatPromotionArray::iterator it;
		for (it = m_aFreeUnitCombatPromotions.begin(); it != m_aFreeUnitCombatPromotions.end(); ++it)
		{
			pStream->Write((*it).first);
			pStream->Write((*it).second);
		}
	}

	{
		uint iSize = m_aFreeUnitClassPromotions.size();
		pStream->Write(iSize);
		UnitClassPromotionArray::iterator it;
		for (it = m_aFreeUnitClassPromotions.begin(); it != m_aFreeUnitClassPromotions.end(); ++it)
		{
			pStream->Write((*it).first);
			pStream->Write((*it).second);
		}
	}

	{
		uint iSize = m_aVote.size();
		pStream->Write(iSize);
		std::vector< std::pair<int, PlayerVoteTypes> >::iterator it;
		for (it = m_aVote.begin(); it != m_aVote.end(); ++it)
		{
			pStream->Write((*it).first);
			pStream->Write((*it).second);
		}
	}

	{
		uint iSize = m_aUnitExtraCosts.size();
		pStream->Write(iSize);
		std::vector< std::pair<UnitClassTypes, int> >::iterator it;
		for (it = m_aUnitExtraCosts.begin(); it != m_aUnitExtraCosts.end(); ++it)
		{
			pStream->Write((*it).first);
			pStream->Write((*it).second);
		}
	}

	{
		uint iSize = m_triggersFired.size();
		pStream->Write(iSize);
		std::vector<EventTriggerTypes>::iterator it;
		for (it = m_triggersFired.begin(); it != m_triggersFired.end(); ++it)
		{
			pStream->Write((*it));
		}
	}

	pStream->Write(m_iPopRushHurryCount);
	pStream->Write(m_iInflationModifier);
	pStream->Write(m_iInflationRate); // K-Mod, uiFlag >= 5
}

void CvPlayer::createGreatPeople(UnitTypes eGreatPersonUnit, bool bIncrementThreshold, bool bIncrementExperience, int iX, int iY)
{
	CvPlot* pPlot = GC.getMapINLINE().plot(iX, iY);
	if (pPlot == NULL)
	{
		FAssertMsg(false, "Invalid plot in createGreatPeople()");
		return;
	}

	CvUnit* pGreatPeopleUnit = initUnit(eGreatPersonUnit, iX, iY);
	if (NULL == pGreatPeopleUnit)
	{
		FAssert(false);
		return;
	}

	CvCity* pCity = pPlot->getPlotCity();

	if (bIncrementThreshold)
	{
		incrementGreatPeopleCreated();

		changeGreatPeopleThresholdModifier(GC.getDefineINT("GREAT_PEOPLE_THRESHOLD_INCREASE") * ((getGreatPeopleCreated() / 10) + 1));

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
			{
				GET_PLAYER((PlayerTypes)iI).changeGreatPeopleThresholdModifier(GC.getDefineINT("GREAT_PEOPLE_THRESHOLD_INCREASE_TEAM") * ((getGreatPeopleCreated() / 10) + 1));
			}
		}
	}

	if (bIncrementExperience)
	{
		incrementGreatGeneralsCreated();

		changeGreatGeneralsThresholdModifier(GC.getDefineINT("GREAT_GENERALS_THRESHOLD_INCREASE") * ((getGreatGeneralsCreated() / 10) + 1));

		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
			{
				GET_PLAYER((PlayerTypes)iI).changeGreatGeneralsThresholdModifier(GC.getDefineINT("GREAT_GENERALS_THRESHOLD_INCREASE_TEAM") * ((getGreatGeneralsCreated() / 10) + 1));
			}
		}
	}

	CvWString szReplayMessage;
	
	if (pCity)
	{
		CvWString szCity;
		szCity.Format(L"%s (%s)", pCity->getName().GetCString(), GET_PLAYER(pCity->getOwnerINLINE()).getReplayName());
		szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN", pGreatPeopleUnit->getName().GetCString(), szCity.GetCString());
	}
	else
	{
		szReplayMessage = gDLL->getText("TXT_KEY_MISC_GP_BORN_FIELD", pGreatPeopleUnit->getName().GetCString());
	}

	GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szReplayMessage, iX, iY, (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"));
	
	// Civ4 Reimagined: Messages for the owner itself only
	gDLL->getInterfaceIFace()->addHumanMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szReplayMessage, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_MAJOR_EVENT, pGreatPeopleUnit->getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), iX, iY, true, true);

	// Python Event
	if (pCity)
	{
		CvEventReporter::getInstance().greatPersonBorn(pGreatPeopleUnit, getID(), pCity);
	}

	// Civ4 Reimagined
	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MATHEMATICS")))
	{
		if (eGreatPersonUnit == (UnitTypes)GC.getInfoTypeForString("UNIT_SCIENTIST"))
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MATHEMATICS"), getID(), true);
		}
	}

	// Civ4 Reimagined
	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MEDITATION")))
	{
		if (eGreatPersonUnit == (UnitTypes)GC.getInfoTypeForString("UNIT_PROPHET"))
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MEDITATION"), getID(), true);
		}
	}

	// Civ4 Reimagined
	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CORPORATION")))
	{
		if (eGreatPersonUnit == (UnitTypes)GC.getInfoTypeForString("UNIT_MERCHANT"))
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CORPORATION"), getID(), true);
		}
	}

	// Civ4 Reimagined
	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MILITARY_TRADITION")))
	{
		if (getPlayerRecord()->getNumUnitsBuilt((UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL")) > 1)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MILITARY_TRADITION"), getID(), true);
		}
	}

	// Civ4 Reimagined
	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_ELECTRICITY")))
	{
		if (eGreatPersonUnit == (UnitTypes)GC.getInfoTypeForString("UNIT_ENGINEER") && getCurrentEra() >= (EraTypes)GC.getInfoTypeForString("ERA_RENAISSANCE"))
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_ELECTRICITY"), getID(), true);
		}
	}

	// Civ4 Reimagined
	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_COMPUTERS")))
	{
		if (getPlayerRecord()->getNumUnitsBuilt((UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_SPY")) > 1)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_COMPUTERS"), getID(), true);
		}
	}

	// Civ4 Reimagined
	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_SUPERCONDUCTORS")))
	{
		if (eGreatPersonUnit == (UnitTypes)GC.getInfoTypeForString("UNIT_SCIENTIST") && getCurrentEra() >= (EraTypes)GC.getInfoTypeForString("ERA_MODERN"))
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_SUPERCONDUCTORS"), getID(), true);
		}
	}

}


const EventTriggeredData* CvPlayer::getEventOccured(EventTypes eEvent) const
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	CvEventMap::const_iterator it = m_mapEventsOccured.find(eEvent);

	if (it == m_mapEventsOccured.end())
	{
		return NULL;
	}

	return &(it->second);
}

bool CvPlayer::isTriggerFired(EventTriggerTypes eEventTrigger) const
{
	return (std::find(m_triggersFired.begin(), m_triggersFired.end(), eEventTrigger) != m_triggersFired.end());
}

void CvPlayer::resetEventOccured(EventTypes eEvent, bool bAnnounce)
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	CvEventMap::iterator it = m_mapEventsOccured.find(eEvent);

	if (it != m_mapEventsOccured.end())
	{
		expireEvent(it->first, it->second, bAnnounce);
		m_mapEventsOccured.erase(it);
	}
}

void CvPlayer::setEventOccured(EventTypes eEvent, const EventTriggeredData& kEventTriggered, bool bOthers)
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	m_mapEventsOccured[eEvent] = kEventTriggered;

	if (GC.getEventInfo(eEvent).isQuest())
	{
		CvWStringBuffer szMessageBuffer;
		szMessageBuffer.append(GC.getEventInfo(eEvent).getDescription());
		GAMETEXT.setEventHelp(szMessageBuffer, eEvent, kEventTriggered.getID(), getID());
		gDLL->getInterfaceIFace()->addQuestMessage(getID(), szMessageBuffer.getCString(), kEventTriggered.getID());
	}

	if (bOthers)
	{
		if (GC.getEventInfo(eEvent).isGlobal())
		{
			for (int i = 0; i < MAX_CIV_PLAYERS; i++)
			{
				if (i != getID())
				{
					GET_PLAYER((PlayerTypes)i).setEventOccured(eEvent, kEventTriggered, false);
				}
			}
		}
		else if (GC.getEventInfo(eEvent).isTeam())
		{
			for (int i = 0; i < MAX_CIV_PLAYERS; i++)
			{
				if (i != getID() && getTeam() == GET_PLAYER((PlayerTypes)i).getTeam())
				{
					GET_PLAYER((PlayerTypes)i).setEventOccured(eEvent, kEventTriggered, false);
				}
			}
		}
	}
}


const EventTriggeredData* CvPlayer::getEventCountdown(EventTypes eEvent) const
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	CvEventMap::const_iterator it = m_mapEventCountdown.find(eEvent);

	if (it == m_mapEventCountdown.end())
	{
		return NULL;
	}

	return &(it->second);
}

void CvPlayer::setEventCountdown(EventTypes eEvent, const EventTriggeredData& kEventTriggered)
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	m_mapEventCountdown[eEvent] = kEventTriggered;
}

void CvPlayer::resetEventCountdown(EventTypes eEvent)
{
	FAssert(eEvent >= 0 && eEvent < GC.getNumEventInfos());

	CvEventMap::iterator it = m_mapEventCountdown.find(eEvent);

	if (it != m_mapEventCountdown.end())
	{
		m_mapEventCountdown.erase(it);
	}
}


void CvPlayer::resetTriggerFired(EventTriggerTypes eTrigger)
{
	std::vector<EventTriggerTypes>::iterator it = std::find(m_triggersFired.begin(), m_triggersFired.end(), eTrigger);

	if (it != m_triggersFired.end())
	{
		m_triggersFired.erase(it);
	}
}

void CvPlayer::setTriggerFired(const EventTriggeredData& kTriggeredData, bool bOthers, bool bAnnounce)
{
	FAssert(kTriggeredData.m_eTrigger >= 0 && kTriggeredData.m_eTrigger < GC.getNumEventTriggerInfos());

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(kTriggeredData.m_eTrigger);

	if (!isTriggerFired(kTriggeredData.m_eTrigger))
	{
		m_triggersFired.push_back(kTriggeredData.m_eTrigger);

		if (bOthers)
		{
			if (kTrigger.isGlobal())
			{
				for (int i = 0; i < MAX_CIV_PLAYERS; i++)
				{
					if (i != getID())
					{
						GET_PLAYER((PlayerTypes)i).setTriggerFired(kTriggeredData, false, false);
					}
				}
			}
			else if (kTrigger.isTeam())
			{
				for (int i = 0; i < MAX_CIV_PLAYERS; i++)
				{
					if (i != getID() && getTeam() == GET_PLAYER((PlayerTypes)i).getTeam())
					{
						GET_PLAYER((PlayerTypes)i).setTriggerFired(kTriggeredData, false, false);
					}
				}
			}
		}
	}

	if (!CvString(kTrigger.getPythonCallback()).empty())
	{
		long lResult;

		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(&kTriggeredData));

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kTrigger.getPythonCallback(), argsList.makeFunctionArgs(), &lResult);
	}

	if (bAnnounce)
	{
		CvPlot* pPlot = GC.getMapINLINE().plot(kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY);

		if (!kTriggeredData.m_szGlobalText.empty())
		{
			for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
			{
				CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

				if (kLoopPlayer.isAlive())
				{
					if (GET_TEAM(kLoopPlayer.getTeam()).isHasMet(getTeam()) && (NO_PLAYER == kTriggeredData.m_eOtherPlayer || GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).isHasMet(getTeam())))
					{
						bool bShowPlot = kTrigger.isShowPlot();

						if (bShowPlot)
						{
							if (kLoopPlayer.getTeam() != getTeam())
							{
								if (NULL == pPlot || !pPlot->isRevealed(kLoopPlayer.getTeam(), false))
								{
									bShowPlot = false;
								}
							}
						}

						if (bShowPlot)
						{
							gDLL->getInterfaceIFace()->addHumanMessage((PlayerTypes)iPlayer, false, GC.getEVENT_MESSAGE_TIME(), kTriggeredData.m_szGlobalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY, true, true);
						}
						else
						{
							gDLL->getInterfaceIFace()->addHumanMessage((PlayerTypes)iPlayer, false, GC.getEVENT_MESSAGE_TIME(), kTriggeredData.m_szGlobalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT);
						}
					}
				}
			}

			GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), kTriggeredData.m_szGlobalText, kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
		}
		else if (!kTriggeredData.m_szText.empty())
		{
			if (kTrigger.isShowPlot() && NULL != pPlot && pPlot->isRevealed(getTeam(), false))
			{
				gDLL->getInterfaceIFace()->addHumanMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), kTriggeredData.m_szText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY, true, true);
			}
			else
			{
				gDLL->getInterfaceIFace()->addHumanMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), kTriggeredData.m_szText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));
			}
		}
	}
}

EventTriggeredData* CvPlayer::initTriggeredData(EventTriggerTypes eEventTrigger, bool bFire, int iCityId, int iPlotX, int iPlotY, PlayerTypes eOtherPlayer, int iOtherPlayerCityId, ReligionTypes eReligion, CorporationTypes eCorporation, int iUnitId, BuildingTypes eBuilding)
{

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eEventTrigger);

	CvCity* pCity = getCity(iCityId);
	CvCity* pOtherPlayerCity = NULL;
	if (NO_PLAYER != eOtherPlayer)
	{
		pOtherPlayerCity = GET_PLAYER(eOtherPlayer).getCity(iOtherPlayerCityId);
	}
	CvPlot* pPlot = GC.getMapINLINE().plot(iPlotX, iPlotY);
	CvUnit* pUnit = getUnit(iUnitId);

	std::vector<CvPlot*> apPlots;
	bool bPickPlot = ::isPlotEventTrigger(eEventTrigger);
	if (kTrigger.isPickCity())
	{
		if (NULL == pCity)
		{
			pCity = pickTriggerCity(eEventTrigger);
		}

		if (NULL != pCity)
		{
			if (bPickPlot)
			{
				for (int iPlot = 0; iPlot < NUM_CITY_PLOTS; ++iPlot)
				{
					if (CITY_HOME_PLOT != iPlot)
					{
						CvPlot* pLoopPlot = pCity->getCityIndexPlot(iPlot);

						if (NULL != pLoopPlot)
						{
							if (pLoopPlot->canTrigger(eEventTrigger, getID()))
							{
								apPlots.push_back(pLoopPlot);
							}
						}
					}
				}
			}
		}
		else
		{
			return NULL;
		}
	}
	else 
	{
		if (kTrigger.getNumBuildings() > 0 && kTrigger.getNumBuildingsRequired() > 0)
		{
			int iFoundValid = 0;

			for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
			{
				if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
				{
					iFoundValid += getBuildingClassCount((BuildingClassTypes)kTrigger.getBuildingRequired(i));
				}
			}

			if (iFoundValid < kTrigger.getNumBuildings())
			{
				return NULL;
			}
		}

		if (kTrigger.getNumReligions() > 0)
		{
			int iFoundValid = 0;

			if (kTrigger.getNumReligionsRequired() > 0)
			{
				for (int i = 0; i < kTrigger.getNumReligionsRequired(); ++i)
				{
					if (kTrigger.getReligionRequired(i) != NO_RELIGION)
					{
						if (getHasReligionCount((ReligionTypes)kTrigger.getReligionRequired(i)) > 0)
						{
							++iFoundValid;
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < GC.getNumReligionInfos(); ++i)
				{
					if (getHasReligionCount((ReligionTypes)i) > 0)
					{
						++iFoundValid;
					}
				}
			}

			if (iFoundValid < kTrigger.getNumReligions())
			{
				return NULL;
			}
		}

		if (kTrigger.getNumCorporations() > 0)
		{
			int iFoundValid = 0;

			if (kTrigger.getNumCorporationsRequired() > 0)
			{
				for (int i = 0; i < kTrigger.getNumCorporationsRequired(); ++i)
				{
					if (kTrigger.getCorporationRequired(i) != NO_CORPORATION)
					{
						if (getHasCorporationCount((CorporationTypes)kTrigger.getCorporationRequired(i)) > 0)
						{
							++iFoundValid;
						}
					}
				}
			}
			else
			{
				for (int i = 0; i < GC.getNumCorporationInfos(); ++i)
				{
					if (getHasCorporationCount((CorporationTypes)i) > 0)
					{
						++iFoundValid;
					}
				}
			}

			if (iFoundValid < kTrigger.getNumCorporations())
			{
				return NULL;
			}
		}

		if (kTrigger.getMinPopulation() > 0)
		{
			if (getTotalPopulation() < kTrigger.getMinPopulation())
			{
				return NULL;
			}
		}

		if (kTrigger.getMaxPopulation() > 0)
		{
			if (getTotalPopulation() > kTrigger.getMaxPopulation())
			{
				return NULL;
			}
		}

		if (bPickPlot)
		{
			for (int iPlot = 0; iPlot < GC.getMapINLINE().numPlotsINLINE(); ++iPlot)
			{
				CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iPlot);

				if (pLoopPlot->canTrigger(eEventTrigger, getID()))
				{
					apPlots.push_back(pLoopPlot);
				}
			}
		}
	}

	if (kTrigger.isPickReligion())
	{
		if (NO_RELIGION == eReligion)
		{
			if (kTrigger.isStateReligion())
			{
				ReligionTypes eStateReligion = getStateReligion();
				if (NO_RELIGION != eStateReligion && isValidTriggerReligion(kTrigger, pCity, eStateReligion))
				{
					eReligion = getStateReligion();
				}
			}
			else
			{
				int iOffset = GC.getGameINLINE().getSorenRandNum(GC.getNumReligionInfos(), "Event pick religion");

				for (int i = 0; i < GC.getNumReligionInfos(); ++i)
				{
					int iReligion = (i + iOffset) % GC.getNumReligionInfos();

					if (isValidTriggerReligion(kTrigger, pCity, (ReligionTypes)iReligion))
					{
						eReligion = (ReligionTypes)iReligion;
						break;
					}
				}
			}
		}

		if (NO_RELIGION == eReligion)
		{
			return NULL;
		}
	}

	if (kTrigger.isPickCorporation())
	{
		if (NO_CORPORATION == eCorporation)
		{
			int iOffset = GC.getGameINLINE().getSorenRandNum(GC.getNumCorporationInfos(), "Event pick corporation");

			for (int i = 0; i < GC.getNumCorporationInfos(); ++i)
			{
				int iCorporation = (i + iOffset) % GC.getNumCorporationInfos();

				if (isValidTriggerCorporation(kTrigger, pCity, (CorporationTypes)iCorporation))
				{
					eCorporation = (CorporationTypes)iCorporation;
					break;
				}
			}
		}

		if (NO_CORPORATION == eCorporation)
		{
			return NULL;
		}
	}

	if (NULL == pPlot)
	{
		
		if (apPlots.size() > 0)
		{
			int iChosen = GC.getGameINLINE().getSorenRandNum(apPlots.size(), "Event pick plot");
			pPlot = apPlots[iChosen];

			if (NULL == pCity)
			{
				pCity = GC.getMapINLINE().findCity(pPlot->getX_INLINE(), pPlot->getY_INLINE(), getID(), NO_TEAM, false);
			}
		}
		else 
		{
			if (bPickPlot)
			{
				return NULL;
			}

			if (NULL != pCity)
			{
				pPlot = pCity->plot();
			}
		}
	}

	if (kTrigger.getNumBuildings() > 0)
	{
		if (NULL != pCity && NO_BUILDING == eBuilding)
		{
			std::vector<BuildingTypes> aeBuildings;
			for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
			{
				if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
				{
					BuildingTypes eTestBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(kTrigger.getBuildingRequired(i));
					if (NO_BUILDING != eTestBuilding && pCity->getNumRealBuilding(eTestBuilding) > 0)
					{
						aeBuildings.push_back(eTestBuilding);
					}
				}
			}

			if (aeBuildings.size() > 0)
			{
				int iChosen = GC.getGameINLINE().getSorenRandNum(aeBuildings.size(), "Event pick building");
				eBuilding = aeBuildings[iChosen];
			}
			else
			{
				return NULL;
			}
		}
	}	

	if (NULL == pUnit)
	{
		pUnit = pickTriggerUnit(eEventTrigger, pPlot, bPickPlot);
	}

	if (NULL == pUnit && kTrigger.getNumUnits() > 0)
	{
		return NULL;
	}

	if (NULL == pPlot && NULL != pUnit)
	{
		pPlot = pUnit->plot();
	}

	if (NULL == pPlot && bPickPlot)
	{
		return NULL;
	}

	if (kTrigger.getNumUnitsGlobal() > 0)
	{
		int iNumUnits = 0;
		for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kLoopPlayer.isAlive())
			{
				int iLoop;
				for (CvUnit* pLoopUnit = kLoopPlayer.firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = kLoopPlayer.nextUnit(&iLoop))
				{
					if (MIN_INT != pLoopUnit->getTriggerValue(eEventTrigger, pPlot, true))
					{
						++iNumUnits;
					}
				}
			}
		}

		if (iNumUnits < kTrigger.getNumUnitsGlobal())
		{
			return NULL;
		}
	}

	if (kTrigger.getNumBuildingsGlobal() > 0)
	{
		int iNumBuildings = 0;
		for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kLoopPlayer.isAlive())
			{
				for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
				{
					if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
					{
						iNumBuildings += getBuildingClassCount((BuildingClassTypes)kTrigger.getBuildingRequired(i));
					}
				}
			}
		}

		if (iNumBuildings < kTrigger.getNumBuildingsGlobal())
		{
			return NULL;
		}
	}

	if (kTrigger.isPickPlayer())
	{
		std::vector<PlayerTypes> aePlayers;
		std::vector<CvCity*> apCities;

		if (NO_PLAYER == eOtherPlayer)
		{
			for (int i = 0; i < MAX_CIV_PLAYERS; i++)
			{
				if (GET_PLAYER((PlayerTypes)i).canTrigger(eEventTrigger, getID(), eReligion))
				{
					if (kTrigger.isPickOtherPlayerCity())
					{
						CvCity* ppBestCity = NULL;

						if (NULL != pCity)
						{
							ppBestCity = GC.getMapINLINE().findCity(pCity->getX_INLINE(), pCity->getY_INLINE(), (PlayerTypes)i);
						}
						else
						{
							ppBestCity = GET_PLAYER((PlayerTypes)i).pickTriggerCity(eEventTrigger);
						}

						if (NULL != ppBestCity)
						{
							apCities.push_back(ppBestCity);
							aePlayers.push_back((PlayerTypes)i);
						}
					}
					else
					{
						apCities.push_back(NULL);
						aePlayers.push_back((PlayerTypes)i);
					}
				}
			}

			if (aePlayers.size() > 0)
			{
				int iChosen = GC.getGameINLINE().getSorenRandNum(aePlayers.size(), "Event pick player");
				eOtherPlayer = aePlayers[iChosen];
				pOtherPlayerCity = apCities[iChosen];
			}
			else
			{
				return NULL;
			}
		}
	}	

	EventTriggeredData* pTriggerData = addEventTriggered();

	if (NULL != pTriggerData)
	{
		pTriggerData->m_eTrigger = eEventTrigger;
		pTriggerData->m_ePlayer = getID();
		pTriggerData->m_iTurn = GC.getGameINLINE().getGameTurn();
		pTriggerData->m_iCityId = (NULL != pCity) ? pCity->getID() : -1;
		pTriggerData->m_iPlotX = (NULL != pPlot) ? pPlot->getX_INLINE() : INVALID_PLOT_COORD;
		pTriggerData->m_iPlotY = (NULL != pPlot) ? pPlot->getY_INLINE() : INVALID_PLOT_COORD;
		pTriggerData->m_eOtherPlayer = eOtherPlayer;
		pTriggerData->m_iOtherPlayerCityId = (NULL != pOtherPlayerCity) ? pOtherPlayerCity->getID() : -1;
		pTriggerData->m_eReligion = eReligion;
		pTriggerData->m_eCorporation = eCorporation;
		pTriggerData->m_iUnitId = (NULL != pUnit) ? pUnit->getID() : -1;
		pTriggerData->m_eBuilding = eBuilding;
	}
	else
	{
		return NULL;
	}

	if (!CvString(kTrigger.getPythonCanDo()).empty())
	{
		long lResult;

		CyArgsList argsList;
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pTriggerData));

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kTrigger.getPythonCanDo(), argsList.makeFunctionArgs(), &lResult);

		if (0 == lResult)
		{
			deleteEventTriggered(pTriggerData->getID());
			return NULL;
		}

		// python may change pTriggerData
		pCity = getCity(pTriggerData->m_iCityId);
		pPlot = GC.getMapINLINE().plot(pTriggerData->m_iPlotX, pTriggerData->m_iPlotY);
		pUnit = getUnit(pTriggerData->m_iUnitId);
		eOtherPlayer = pTriggerData->m_eOtherPlayer;
		if (NO_PLAYER != eOtherPlayer)
		{
			pOtherPlayerCity = GET_PLAYER(eOtherPlayer).getCity(pTriggerData->m_iOtherPlayerCityId);
		}
		eReligion = pTriggerData->m_eReligion;
		eCorporation = pTriggerData->m_eCorporation;
		eBuilding = pTriggerData->m_eBuilding;
	}

	std::vector<CvWString> aszTexts;
	for (int i = 0; i < kTrigger.getNumTexts(); ++i)
	{
		if (NO_ERA == kTrigger.getTextEra(i) || kTrigger.getTextEra(i) == getCurrentEra())
		{
			aszTexts.push_back(kTrigger.getText(i));
		}
	}

	if (aszTexts.size() > 0)
	{
		int iText = GC.getGameINLINE().getSorenRandNum(aszTexts.size(), "Event Text choice");

		pTriggerData->m_szText = gDLL->getText(aszTexts[iText].GetCString(), 
			eOtherPlayer != NO_PLAYER ? GET_PLAYER(eOtherPlayer).getCivilizationAdjectiveKey() : L"", 
			NULL != pCity ? pCity->getNameKey() : L"", 
			NULL != pUnit ? pUnit->getNameKey() : L"", 
			NO_RELIGION != eReligion ? GC.getReligionInfo(eReligion).getAdjectiveKey() : L"",
			NO_BUILDING != eBuilding ? GC.getBuildingInfo(eBuilding).getTextKeyWide() : L"",
			NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			NULL != pPlot && NO_TERRAIN != pPlot->getTerrainType() ? GC.getTerrainInfo(pPlot->getTerrainType()).getTextKeyWide() : L"",
			NULL != pPlot && NO_IMPROVEMENT != pPlot->getImprovementType() ? GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide() : L"",
			NULL != pPlot && NO_BONUS != pPlot->getBonusType() ? GC.getBonusInfo(pPlot->getBonusType()).getTextKeyWide() : L"",
			NULL != pPlot && NO_ROUTE != pPlot->getRouteType() ? GC.getRouteInfo(pPlot->getRouteType()).getTextKeyWide() : L"",
			NO_CORPORATION != eCorporation ? GC.getCorporationInfo(eCorporation).getTextKeyWide() : L""
			);

	}
	else
	{
		pTriggerData->m_szText = L"";
	}

	if (kTrigger.getNumWorldNews() > 0)
	{
		int iText = GC.getGameINLINE().getSorenRandNum(kTrigger.getNumWorldNews(), "Trigger World News choice");

		pTriggerData->m_szGlobalText = gDLL->getText(kTrigger.getWorldNews(iText).GetCString(),
			getCivilizationAdjectiveKey(),
			NULL != pCity ? pCity->getNameKey() : L"",
			pTriggerData->m_eReligion != NO_RELIGION ? GC.getReligionInfo(pTriggerData->m_eReligion).getAdjectiveKey() : L"",
			eOtherPlayer != NO_PLAYER ? GET_PLAYER(eOtherPlayer).getCivilizationAdjectiveKey() : L"",
			NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			pTriggerData->m_eCorporation != NO_CORPORATION ? GC.getCorporationInfo(pTriggerData->m_eCorporation).getTextKeyWide() : L""
			);
	}
	else
	{
		pTriggerData->m_szGlobalText.clear();
	}

	if (bFire)
	{
		trigger(*pTriggerData);
	}
	
	return pTriggerData;
}


bool CvPlayer::canDoEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const
{
	if (eEvent == NO_EVENT)
	{
		FAssert(false);
		return false;
	}

	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	int iGold = std::min(getEventCost(eEvent, kTriggeredData.m_eOtherPlayer, false), getEventCost(eEvent, kTriggeredData.m_eOtherPlayer, true));

	if (iGold != 0)
	{
		if (iGold > 0 && NO_PLAYER != kTriggeredData.m_eOtherPlayer && kEvent.isGoldToPlayer())
		{
			if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).getGold() < iGold)
			{
				return false;
			}
		}
		else if (iGold < 0)
		{
			if (getGold() < -iGold)
			{
				return false;
			}
		}
	}

	if (0 != kEvent.getSpaceProductionModifier())
	{
		bool bValid = false;
		for (int iProject = 0; iProject < GC.getNumProjectInfos(); ++iProject)
		{
			CvProjectInfo& kProject = GC.getProjectInfo((ProjectTypes)iProject);
			if (kProject.isSpaceship())
			{
				if (kProject.getVictoryPrereq() != NO_VICTORY)
				{
					if (GC.getGameINLINE().isVictoryValid((VictoryTypes)(kProject.getVictoryPrereq())))
					{
						bValid = true;
						break;
					}
				}
			}
		}

		if (!bValid)
		{
			return false;
		}
	}

	if (kEvent.getEspionagePoints() > 0 && GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		return false;
	}

	if (NO_PLAYER != kTriggeredData.m_eOtherPlayer)
	{
		if (kEvent.getEspionagePoints() + GET_TEAM(getTeam()).getEspionagePointsAgainstTeam(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()) < 0)
		{
			return false;
		}
	}

	if (0 != kEvent.getTechPercent() || 0 != kEvent.getTechCostPercent())
	{
		if (NO_TECH == getBestEventTech(eEvent, kTriggeredData.m_eOtherPlayer))
		{
			return false;
		}
	}

	if (NO_TECH != kEvent.getPrereqTech())
	{
		if (!GET_TEAM(getTeam()).isHasTech((TechTypes)kEvent.getPrereqTech()))
		{
			return false;
		}
	}

	if (NO_BONUS != kEvent.getBonusGift())
	{
		BonusTypes eBonus = (BonusTypes)kEvent.getBonusGift();
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		if (!canTradeNetworkWith(kTriggeredData.m_eOtherPlayer))
		{
			return false;
		}

		if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).getNumAvailableBonuses(eBonus) > 0)
		{
			return false;
		}

		if (getNumTradeableBonuses(eBonus) <= 1)
		{
			return false;
		}
	}

	if (kEvent.getUnitClass() != NO_UNITCLASS)
	{
		UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kEvent.getUnitClass());
		if (eUnit == NO_UNIT)
		{
			return false;
		}
	}

	if (kEvent.isCityEffect())
	{
		CvCity* pCity =	getCity(kTriggeredData.m_iCityId);
		if (NULL == pCity || !pCity->canApplyEvent(eEvent, kTriggeredData))
		{
			return false;
		}
	}
	else if (kEvent.isOtherPlayerCityEffect())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		CvCity* pCity = GET_PLAYER(kTriggeredData.m_eOtherPlayer).getCity(kTriggeredData.m_iOtherPlayerCityId);
		if (NULL == pCity || !pCity->canApplyEvent(eEvent, kTriggeredData))
		{
			return false;
		}
	}

	if (::isPlotEventTrigger(kTriggeredData.m_eTrigger))
	{
		CvPlot* pPlot = GC.getMapINLINE().plotINLINE(kTriggeredData.m_iPlotX, kTriggeredData.m_iPlotY);
		if (NULL != pPlot)
		{
			if (!pPlot->canApplyEvent(eEvent))
			{
				return false;
			}
		}
	}

	CvUnit* pUnit = getUnit(kTriggeredData.m_iUnitId);
	if (NULL != pUnit)
	{
		if (!pUnit->canApplyEvent(eEvent))
		{
			return false;
		}
	}

	if (NO_BONUS != kEvent.getBonusRevealed())
	{
		if (GET_TEAM(getTeam()).isBonusRevealed((BonusTypes)kEvent.getBonusRevealed()))
		{
			return false;
		}

		if (GET_TEAM(getTeam()).isForceRevealedBonus((BonusTypes)kEvent.getBonusRevealed()))
		{
			return false;
		}
	}

	if (kEvent.getConvertOwnCities() > 0)
	{
		bool bFoundValid = false;

		if (NO_RELIGION != kTriggeredData.m_eReligion)
		{
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				if (!pLoopCity->isHasReligion(kTriggeredData.m_eReligion))
				{
					if (-1 == kEvent.getMaxNumReligions() || pLoopCity->getReligionCount() <= kEvent.getMaxNumReligions())
					{
						bFoundValid = true;
						break;
					}
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (kEvent.getConvertOtherCities() > 0)
	{
		bool bFoundValid = false;

		if (NO_RELIGION != kTriggeredData.m_eReligion)
		{
			if (NO_PLAYER != kTriggeredData.m_eOtherPlayer)
			{
				int iLoop;
				for (CvCity* pLoopCity = GET_PLAYER(kTriggeredData.m_eOtherPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(kTriggeredData.m_eOtherPlayer).nextCity(&iLoop))
				{
					if (!pLoopCity->isHasReligion(kTriggeredData.m_eReligion))
					{
						if (-1 == kEvent.getMaxNumReligions() || pLoopCity->getReligionCount() <= kEvent.getMaxNumReligions())
						{
							bFoundValid = true;
							break;
						}
					}
				}
			}
		}

		if (!bFoundValid)
		{
			return false;
		}
	}

	if (0 != kEvent.getAttitudeModifier())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam() == getTeam())
		{
			return false;
		}

		if (GET_PLAYER(kTriggeredData.m_eOtherPlayer).isHuman())
		{
			if (0 == kEvent.getOurAttitudeModifier())
			{
				return false;
			}
		}
	}

	if (0 != kEvent.getTheirEnemyAttitudeModifier())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		TeamTypes eWorstEnemy = GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
		if (NO_TEAM == eWorstEnemy || eWorstEnemy == getTeam())
		{
			return false;
		}

		if (!GET_TEAM(eWorstEnemy).isAlive())
		{
			return false;
		}

		if (eWorstEnemy == getTeam())
		{
			return false;
		}
	}

	if (kEvent.isDeclareWar())
	{
		if (NO_PLAYER == kTriggeredData.m_eOtherPlayer)
		{
			return false;
		}

		if (!GET_TEAM(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()).canDeclareWar(getTeam()) || !GET_TEAM(getTeam()).canDeclareWar(GET_PLAYER(kTriggeredData.m_eOtherPlayer).getTeam()))
		{
			return false;
		}
	}

	if (kEvent.isQuest())
	{
		for (int iTrigger = 0; iTrigger < GC.getNumEventTriggerInfos(); ++iTrigger)
		{
			CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo((EventTriggerTypes)iTrigger);
			if (!kTrigger.isRecurring())
			{
				for (int i = 0; i < kTrigger.getNumPrereqEvents(); ++i)
				{
					if (kTrigger.getPrereqEvent(i) == eEvent)
					{
						if (isTriggerFired((EventTriggerTypes)iTrigger))
						{
							return false;
						}
					}
				}
			}
		}
	}

	if (!CvString(kEvent.getPythonCanDo()).empty())
	{
		long lResult;

		CyArgsList argsList;
		argsList.add(eEvent);
		argsList.add(gDLL->getPythonIFace()->makePythonObject(&kTriggeredData));

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kEvent.getPythonCanDo(), argsList.makeFunctionArgs(), &lResult);

		if (0 == lResult)
		{
			return false;
		}
	}

	return true;
}


void CvPlayer::applyEvent(EventTypes eEvent, int iEventTriggeredId, bool bUpdateTrigger)
{
	FAssert(eEvent != NO_EVENT);

	EventTriggeredData* pTriggeredData = getEventTriggered(iEventTriggeredId);

	if (NULL == pTriggeredData)
	{
		deleteEventTriggered(iEventTriggeredId);
		return;
	}

	if (bUpdateTrigger)
	{
		setTriggerFired(*pTriggeredData, true);
	}

	if (!canDoEvent(eEvent, *pTriggeredData))
	{
		if (bUpdateTrigger)
		{
			deleteEventTriggered(iEventTriggeredId);
		}
		return;
	}

	setEventOccured(eEvent, *pTriggeredData);

	CvEventInfo& kEvent = GC.getEventInfo(eEvent);
	CvCity* pCity =	getCity(pTriggeredData->m_iCityId);
	CvCity* pOtherPlayerCity = NULL;

	if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
	{
		pOtherPlayerCity = GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCity(pTriggeredData->m_iOtherPlayerCityId);
	}

	int iGold = getEventCost(eEvent, pTriggeredData->m_eOtherPlayer, false);
	int iRandomGold = getEventCost(eEvent, pTriggeredData->m_eOtherPlayer, true);

	iGold += GC.getGameINLINE().getSorenRandNum(iRandomGold - iGold + 1, "Event random gold");

	if (iGold != 0)
	{
		changeGold(iGold);

		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer && kEvent.isGoldToPlayer())
		{
			GET_PLAYER(pTriggeredData->m_eOtherPlayer).changeGold(-iGold);
		}
	}

	if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
	{
		if (kEvent.getEspionagePoints() != 0)
		{
			GET_TEAM(getTeam()).changeEspionagePointsAgainstTeam(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam(), kEvent.getEspionagePoints());
		}
	}

	if (0 != kEvent.getTechPercent())
	{
		TechTypes eBestTech = getBestEventTech(eEvent, pTriggeredData->m_eOtherPlayer);

		if (eBestTech != NO_TECH)
		{
			int iBeakers  = GET_TEAM(getTeam()).changeResearchProgressPercent(eBestTech, kEvent.getTechPercent(), getID());

			if (iBeakers > 0)
			{
				for (int iI = 0; iI < MAX_CIV_PLAYERS; iI++)
				{
					if (GET_PLAYER((PlayerTypes)iI).isAlive())
					{
						if (GET_PLAYER((PlayerTypes)iI).getTeam() == getTeam())
						{
							CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_PROGRESS_TOWARDS_TECH", iBeakers, GC.getTechInfo(eBestTech).getTextKeyWide());

							gDLL->getInterfaceIFace()->addHumanMessage(((PlayerTypes)iI), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, NULL, MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_TECH_TEXT"));
						}
					}
				}
			}
		}
	}

	if (kEvent.isGoldenAge())
	{
		changeGoldenAgeTurns(getGoldenAgeLength());
	}

	if (0 != kEvent.getInflationModifier())
	{
		m_iInflationModifier += kEvent.getInflationModifier();
	}

	if (0 != kEvent.getSpaceProductionModifier())
	{
		changeSpaceProductionModifier(kEvent.getSpaceProductionModifier());
	}

	if (0 != kEvent.getFreeUnitSupport())
	{
		changeBaseFreeUnits(kEvent.getFreeUnitSupport());
	}

	if (kEvent.isDeclareWar())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      10/02/09                                jdog5000      */
/*                                                                                              */
/* AI logging                                                                                   */
/************************************************************************************************/
		if( gTeamLogLevel >= 2 )
		{
			logBBAI("    Team %d (%S) declares war on team %d due to event", GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam(), GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationDescription(0), getTeam() );
		}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
			GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).declareWar(getTeam(), false, WARPLAN_LIMITED);
		}
	}

	if (NO_BONUS != kEvent.getBonusGift())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			CLinkList<TradeData> ourList;
			CLinkList<TradeData> theirList;
			TradeData kTradeData;
			setTradeItem(&kTradeData, TRADE_RESOURCES, kEvent.getBonusGift());
			ourList.insertAtEnd(kTradeData);
			GC.getGameINLINE().implementDeal(getID(), pTriggeredData->m_eOtherPlayer, &ourList, &theirList);			
		}
	}

	bool bClear = false;
	for (int iEvent = 0; iEvent < GC.getNumEventInfos(); ++iEvent)
	{
		if (kEvent.getClearEventChance(iEvent) > 0)
		{
			bClear = GC.getGameINLINE().getSorenRandNum(100, "Event Clear") < kEvent.getClearEventChance(iEvent);
			if (bClear)
			{
				if (kEvent.isGlobal())
				{
					for (int j = 0; j < MAX_CIV_PLAYERS; j++)
					{
						GET_PLAYER((PlayerTypes)j).resetEventOccured((EventTypes)iEvent, j != getID());
					}
				}
				else if (kEvent.isTeam())
				{
					for (int j = 0; j < MAX_CIV_PLAYERS; j++)
					{
						if (getTeam() == GET_PLAYER((PlayerTypes)j).getTeam())
						{
							GET_PLAYER((PlayerTypes)j).resetEventOccured((EventTypes)iEvent, j != getID());
						}
					}
				}
				else
				{
					resetEventOccured((EventTypes)iEvent, false);
				}
			}
		}
	}

	if (NULL != pCity && kEvent.isCityEffect())
	{
		pCity->applyEvent(eEvent, *pTriggeredData, bClear);
	}
	else if (NULL != pOtherPlayerCity && kEvent.isOtherPlayerCityEffect())
	{
		pOtherPlayerCity->applyEvent(eEvent, *pTriggeredData, bClear);
	}

	if (!kEvent.isCityEffect() && !kEvent.isOtherPlayerCityEffect())
	{
		if (kEvent.getHappy() != 0)
		{
			changeExtraHappiness(kEvent.getHappy());
		}

		if (kEvent.getHealth() != 0)
		{
			changeExtraHealth(kEvent.getHealth());
		}

		if (kEvent.getNumBuildingYieldChanges() > 0)
		{
			int iLoop;
			for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
			{
				for (int iYield = 0; iYield < NUM_YIELD_TYPES; ++iYield)
				{
					for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
					{
						pLoopCity->changeBuildingYieldChange((BuildingClassTypes)iBuildingClass, (YieldTypes)iYield, kEvent.getBuildingYieldChange(iBuildingClass, iYield));
					}					
				}
			}
		}

		if (kEvent.getNumBuildingCommerceChanges() > 0)
		{
			int iLoop;
			for (int iBuildingClass = 0; iBuildingClass < GC.getNumBuildingClassInfos(); ++iBuildingClass)
			{
				for (int iCommerce = 0; iCommerce < NUM_COMMERCE_TYPES; ++iCommerce)
				{
					for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
					{
						pLoopCity->changeBuildingCommerceChange((BuildingClassTypes)iBuildingClass, (CommerceTypes)iCommerce, kEvent.getBuildingCommerceChange(iBuildingClass, iCommerce));
					}					
				}
			}
		}

		if (kEvent.getNumBuildingHappyChanges() > 0)
		{
			for (int i = 0; i < GC.getNumBuildingClassInfos(); ++i)
			{
				if (0 != kEvent.getBuildingHappyChange(i))
				{
					BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(i);
					if (NO_BUILDING != eBuilding)
					{
						changeExtraBuildingHappiness(eBuilding, kEvent.getBuildingHappyChange(i));
					}
				}
			}
		}

		if (kEvent.getNumBuildingHealthChanges() > 0)
		{
			for (int i = 0; i < GC.getNumBuildingClassInfos(); ++i)
			{
				if (0 != kEvent.getBuildingHealthChange(i))
				{
					BuildingTypes eBuilding = (BuildingTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(i);
					if (NO_BUILDING != eBuilding)
					{
						changeExtraBuildingHealth(eBuilding, kEvent.getBuildingHealthChange(i));
					}
				}
			}
		}

		if (kEvent.getHurryAnger() != 0)
		{
			int iLoop;

			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->changeHurryAngerTimer(kEvent.getHurryAnger() * pLoopCity->flatHurryAngerLength());
			}
		}

		if (kEvent.getHappyTurns() > 0)
		{
			int iLoop;

			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->changeHappinessTimer(kEvent.getHappyTurns());
			}
		}

		if (kEvent.getMaxPillage() > 0)
		{
			FAssert(kEvent.getMaxPillage() >= kEvent.getMinPillage());
			int iNumPillage = kEvent.getMinPillage() + GC.getGameINLINE().getSorenRandNum(kEvent.getMaxPillage() - kEvent.getMinPillage(), "Pick number of event pillaged plots");

			int iNumPillaged = 0;
			for (int i = 0; i < iNumPillage; ++i)
			{
				int iRandOffset = GC.getGameINLINE().getSorenRandNum(GC.getMapINLINE().numPlotsINLINE(), "Pick event pillage plot (any city)");
				for (int j = 0; j < GC.getMapINLINE().numPlotsINLINE(); ++j)
				{
					int iPlot = (j + iRandOffset) % GC.getMapINLINE().numPlotsINLINE();
					CvPlot* pPlot = GC.getMapINLINE().plotByIndexINLINE(iPlot);
					if (NULL != pPlot && pPlot->getOwnerINLINE() == getID() && pPlot->isCity())
					{
						if (NO_IMPROVEMENT != pPlot->getImprovementType() && !GC.getImprovementInfo(pPlot->getImprovementType()).isPermanent())
						{
							CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_CITY_IMPROVEMENT_DESTROYED", GC.getImprovementInfo(pPlot->getImprovementType()).getTextKeyWide());
							gDLL->getInterfaceIFace()->addHumanMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO, GC.getImprovementInfo(pPlot->getImprovementType()).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_RED"), pPlot->getX_INLINE(), pPlot->getY_INLINE(), true, true);
							pPlot->setImprovementType(NO_IMPROVEMENT);
							++iNumPillaged;
							break;
						}
					}
				}
			}

			if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
			{
				CvWString szBuffer = gDLL->getText("TXT_KEY_EVENT_NUM_CITY_IMPROVEMENTS_DESTROYED", iNumPillaged, getCivilizationAdjectiveKey());
				gDLL->getInterfaceIFace()->addHumanMessage(pTriggeredData->m_eOtherPlayer, false, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_PILLAGED", MESSAGE_TYPE_INFO);
			}
		}

		if (kEvent.getFood() != 0)
		{
			int iLoop;

			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->changeFood(kEvent.getFood());
			}
		}

		if (kEvent.getFoodPercent() != 0)
		{
			int iLoop;

			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				pLoopCity->changeFood((pLoopCity->getFood() * kEvent.getFoodPercent()) / 100);
			}
		}

		if (kEvent.getPopulationChange() != 0)
		{
			int iLoop;

			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity->getPopulation() + kEvent.getPopulationChange() > 0)
				{
					pLoopCity->changePopulation(kEvent.getPopulationChange());
				}
			}
		}

		if (kEvent.getCulture() != 0)
		{
			int iLoop;

			for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
			{
				if (pLoopCity->getCultureTimes100(pLoopCity->getOwnerINLINE()) + 100 * kEvent.getCulture() > 0)
				{
					pLoopCity->changeCulture(pLoopCity->getOwnerINLINE(), kEvent.getCulture(), true, true);
				}
			}
		}

		if (kEvent.getUnitClass() != NO_UNITCLASS)
		{
			UnitTypes eUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(kEvent.getUnitClass());
			if (eUnit != NO_UNIT)
			{
				CvCity* pUnitCity = pCity;

				if (NULL == pUnitCity)
				{
					pUnitCity = getCapitalCity();
				}

				if (NULL != pUnitCity)
				{
					for (int i = 0; i < kEvent.getNumUnits(); ++i)
					{
						initUnit(eUnit, pUnitCity->getX_INLINE(), pUnitCity->getY_INLINE());
					}
				}
			}
		}
	}

	CvPlot* pPlot = GC.getMapINLINE().plotINLINE(pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY);
	if (NULL != pPlot)
	{
		if (::isPlotEventTrigger(pTriggeredData->m_eTrigger))
		{
			FAssert(pPlot->canApplyEvent(eEvent));
			pPlot->applyEvent(eEvent);
		}
	}

	CvUnit* pUnit = getUnit(pTriggeredData->m_iUnitId);
	if (NULL != pUnit)
	{
		FAssert(pUnit->canApplyEvent(eEvent));
		pUnit->applyEvent(eEvent);   // might kill the unit
	}

	for (int i = 0; i < GC.getNumUnitCombatInfos(); ++i)
	{
		if (NO_PROMOTION != kEvent.getUnitCombatPromotion(i))
		{
			int iLoop;
			for (CvUnit* pLoopUnit = firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = nextUnit(&iLoop))
			{
				if (pLoopUnit->getUnitCombatType() == i)
				{
					pLoopUnit->setHasPromotion((PromotionTypes)kEvent.getUnitCombatPromotion(i), true);
				}
			}

			setFreePromotion((UnitCombatTypes)i, (PromotionTypes)kEvent.getUnitCombatPromotion(i), true);
		}
	}

	for (int i = 0; i < GC.getNumUnitClassInfos(); ++i)
	{
		if (NO_PROMOTION != kEvent.getUnitClassPromotion(i))
		{
			int iLoop;
			for (CvUnit* pLoopUnit = firstUnit(&iLoop); NULL != pLoopUnit; pLoopUnit = nextUnit(&iLoop))
			{
				if (pLoopUnit->getUnitClassType() == i)
				{
					pLoopUnit->setHasPromotion((PromotionTypes)kEvent.getUnitClassPromotion(i), true);
				}
			}

			setFreePromotion((UnitClassTypes)i, (PromotionTypes)kEvent.getUnitClassPromotion(i), true);
		}
	}

	if (NO_BONUS != kEvent.getBonusRevealed())
	{
		GET_TEAM(getTeam()).setForceRevealedBonus((BonusTypes)kEvent.getBonusRevealed(), true);
	}

	std::vector<CvCity*> apSpreadReligionCities;

	if (kEvent.getConvertOwnCities() > 0)
	{
		if (NO_RELIGION != pTriggeredData->m_eReligion)
		{
			int iLoop;
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				if (!pLoopCity->isHasReligion(pTriggeredData->m_eReligion))
				{
					if (-1 == kEvent.getMaxNumReligions() || pLoopCity->getReligionCount() <= kEvent.getMaxNumReligions())
					{
						apSpreadReligionCities.push_back(pLoopCity);
					}
				}
			}
		}
	}

	while ((int)apSpreadReligionCities.size() > kEvent.getConvertOwnCities())
	{
		int iChosen = GC.getGameINLINE().getSorenRandNum(apSpreadReligionCities.size(), "Even Spread Religion (own)");

		int i = 0;
		for (std::vector<CvCity*>::iterator it = apSpreadReligionCities.begin(); it != apSpreadReligionCities.end(); ++it)
		{
			if (i == iChosen)
			{
				apSpreadReligionCities.erase(it);
				break;
			}
			++i;
		}
	}

	for (std::vector<CvCity*>::iterator it = apSpreadReligionCities.begin(); it != apSpreadReligionCities.end(); ++it)
	{
		(*it)->setHasReligion(pTriggeredData->m_eReligion, true, true, false);
	}

	apSpreadReligionCities.clear();

	if (kEvent.getConvertOtherCities() > 0)
	{
		if (NO_RELIGION != pTriggeredData->m_eReligion)
		{
			if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
			{
				int iLoop;
				for (CvCity* pLoopCity = GET_PLAYER(pTriggeredData->m_eOtherPlayer).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER(pTriggeredData->m_eOtherPlayer).nextCity(&iLoop))
				{
					if (!pLoopCity->isHasReligion(pTriggeredData->m_eReligion))
					{
						if (-1 == kEvent.getMaxNumReligions() || pLoopCity->getReligionCount() <= kEvent.getMaxNumReligions())
						{
							apSpreadReligionCities.push_back(pLoopCity);
						}
					}
				}
			}
		}
	}

	while ((int)apSpreadReligionCities.size() > kEvent.getConvertOtherCities())
	{
		int iChosen = GC.getGameINLINE().getSorenRandNum(apSpreadReligionCities.size(), "Even Spread Religion (other)");

		int i = 0;
		for (std::vector<CvCity*>::iterator it = apSpreadReligionCities.begin(); it != apSpreadReligionCities.end(); ++it)
		{
			if (i == iChosen)
			{
				apSpreadReligionCities.erase(it);
				break;
			}
			++i;
		}
	}

	for (std::vector<CvCity*>::iterator it = apSpreadReligionCities.begin(); it != apSpreadReligionCities.end(); ++it)
	{
		(*it)->setHasReligion(pTriggeredData->m_eReligion, true, true, false);
	}

	/* Civ4 Reimagined: Our own actions should not alter how we view others.
	if (0 != kEvent.getOurAttitudeModifier())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			if (kEvent.getOurAttitudeModifier() > 0)
			{
				AI_changeMemoryCount(pTriggeredData->m_eOtherPlayer, MEMORY_EVENT_GOOD_TO_US, kEvent.getOurAttitudeModifier());
			}
			else
			{
				AI_changeMemoryCount(pTriggeredData->m_eOtherPlayer, MEMORY_EVENT_BAD_TO_US, -kEvent.getOurAttitudeModifier());
			}
		}
	}
	*/

	if (0 != kEvent.getAttitudeModifier())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			if (kEvent.getAttitudeModifier() > 0)
			{
				GET_PLAYER(pTriggeredData->m_eOtherPlayer).AI_changeMemoryCount(getID(), MEMORY_EVENT_GOOD_TO_US, kEvent.getAttitudeModifier());
			}
			else
			{
				GET_PLAYER(pTriggeredData->m_eOtherPlayer).AI_changeMemoryCount(getID(), MEMORY_EVENT_BAD_TO_US, -kEvent.getAttitudeModifier());
			}
		}
	}

	if (0 != kEvent.getTheirEnemyAttitudeModifier())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			TeamTypes eWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
			if (NO_TEAM != eWorstEnemy)
			{
				for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
				{
					CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
					if (kLoopPlayer.isAlive() && kLoopPlayer.getTeam() == eWorstEnemy)
					{
						if (kEvent.getTheirEnemyAttitudeModifier() > 0)
						{
							kLoopPlayer.AI_changeMemoryCount(getID(), MEMORY_EVENT_GOOD_TO_US, kEvent.getTheirEnemyAttitudeModifier());
							AI_changeMemoryCount((PlayerTypes)iPlayer, MEMORY_EVENT_GOOD_TO_US, kEvent.getTheirEnemyAttitudeModifier());
						}
						else
						{
							kLoopPlayer.AI_changeMemoryCount(getID(), MEMORY_EVENT_BAD_TO_US, -kEvent.getTheirEnemyAttitudeModifier());
							AI_changeMemoryCount((PlayerTypes)iPlayer, MEMORY_EVENT_BAD_TO_US, -kEvent.getTheirEnemyAttitudeModifier());
						}
					}
				}
			}
		}
	}

	if (!CvString(kEvent.getPythonCallback()).empty())
	{
		long lResult;

		CyArgsList argsList;
		argsList.add(eEvent);
		argsList.add(gDLL->getPythonIFace()->makePythonObject(pTriggeredData));

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kEvent.getPythonCallback(), argsList.makeFunctionArgs(), &lResult);
	}

	if (kEvent.getNumWorldNews() > 0)
	{
		int iText = GC.getGameINLINE().getSorenRandNum(kEvent.getNumWorldNews(), "Event World News choice");

		CvWString szGlobalText;

		TeamTypes eTheirWorstEnemy = NO_TEAM;
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			eTheirWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
		}

		szGlobalText = gDLL->getText(kEvent.getWorldNews(iText).GetCString(),
			getCivilizationAdjectiveKey(),
			NULL != pCity ? pCity->getNameKey() : L"", 
			pTriggeredData->m_eOtherPlayer != NO_PLAYER ? GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey() : L"", 
			NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			NO_RELIGION != pTriggeredData->m_eReligion ? GC.getReligionInfo(pTriggeredData->m_eReligion).getAdjectiveKey() : L"",
			NO_TEAM != eTheirWorstEnemy ? GET_TEAM(eTheirWorstEnemy).getName().GetCString() : L"",
			NO_CORPORATION != pTriggeredData->m_eCorporation ? GC.getCorporationInfo(pTriggeredData->m_eCorporation).getTextKeyWide() : L""
			);

		for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
		{
			CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);

			if (kLoopPlayer.isAlive())
			{
				if (GET_TEAM(kLoopPlayer.getTeam()).isHasMet(getTeam()) && (NO_PLAYER == pTriggeredData->m_eOtherPlayer || GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).isHasMet(getTeam())))
				{
					bool bShowPlot = GC.getEventTriggerInfo(pTriggeredData->m_eTrigger).isShowPlot();

					if (bShowPlot)
					{
						if (kLoopPlayer.getTeam() != getTeam())
						{
							if (NULL == pPlot || !pPlot->isRevealed(kLoopPlayer.getTeam(), false))
							{
								bShowPlot = false;
							}
						}
					}

					if (bShowPlot)
					{
						gDLL->getInterfaceIFace()->addHumanMessage((PlayerTypes)iPlayer, false, GC.getEVENT_MESSAGE_TIME(), szGlobalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY, true, true);
					}
					else
					{
						gDLL->getInterfaceIFace()->addHumanMessage((PlayerTypes)iPlayer, false, GC.getEVENT_MESSAGE_TIME(), szGlobalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT);
					}
				}
			}
		}

		GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szGlobalText, pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));
	}

	if (!CvWString(kEvent.getLocalInfoTextKey()).empty())
	{
		CvWString szLocalText;

		TeamTypes eTheirWorstEnemy = NO_TEAM;
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			eTheirWorstEnemy = GET_TEAM(GET_PLAYER(pTriggeredData->m_eOtherPlayer).getTeam()).AI_getWorstEnemy();
		}

		szLocalText = gDLL->getText(kEvent.getLocalInfoTextKey(),
			getCivilizationAdjectiveKey(),
			NULL != pCity ? pCity->getNameKey() : L"", 
			pTriggeredData->m_eOtherPlayer != NO_PLAYER ? GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey() : L"", 
			NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
			NO_RELIGION != pTriggeredData->m_eReligion ? GC.getReligionInfo(pTriggeredData->m_eReligion).getAdjectiveKey() : L"",
			NO_TEAM != eTheirWorstEnemy ? GET_TEAM(eTheirWorstEnemy).getName().GetCString() : L"",
			NO_CORPORATION != pTriggeredData->m_eCorporation ? GC.getCorporationInfo(pTriggeredData->m_eCorporation).getTextKeyWide() : L""
			);

			if (GC.getEventTriggerInfo(pTriggeredData->m_eTrigger).isShowPlot())
			{
				gDLL->getInterfaceIFace()->addHumanMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szLocalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"), pTriggeredData->m_iPlotX, pTriggeredData->m_iPlotY, true, true);
			}
			else
			{
				gDLL->getInterfaceIFace()->addHumanMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szLocalText, "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_WHITE"));
			}

	}

	if (!CvWString(kEvent.getOtherPlayerPopup()).empty())
	{
		if (NO_PLAYER != pTriggeredData->m_eOtherPlayer)
		{
			CvWString szText = gDLL->getText(kEvent.getOtherPlayerPopup(),
				getCivilizationAdjectiveKey(),
				NULL != pCity ? pCity->getNameKey() : L"", 
				pTriggeredData->m_eOtherPlayer != NO_PLAYER ? GET_PLAYER(pTriggeredData->m_eOtherPlayer).getCivilizationAdjectiveKey() : L"", 
				NULL != pOtherPlayerCity ? pOtherPlayerCity->getNameKey() : L"",
				NO_RELIGION != pTriggeredData->m_eReligion ? GC.getReligionInfo(pTriggeredData->m_eReligion).getAdjectiveKey() : L"",
				NO_CORPORATION != pTriggeredData->m_eCorporation ? GC.getCorporationInfo(pTriggeredData->m_eCorporation).getTextKeyWide() : L""
				);

			CvPopupInfo* pInfo = new CvPopupInfo();

			if (NULL != pInfo)
			{
				pInfo->setText(szText);

				GET_PLAYER(pTriggeredData->m_eOtherPlayer).addPopup(pInfo);
			}
		}
	}

	bool bDeleteTrigger = bUpdateTrigger;

	for (int iEvent = 0; iEvent < GC.getNumEventInfos(); ++iEvent)
	{
		if (0 == kEvent.getAdditionalEventTime(iEvent))
		{
			if (kEvent.getAdditionalEventChance(iEvent) > 0)
			{
				if (canDoEvent((EventTypes)iEvent, *pTriggeredData))
				{
					if (GC.getGameINLINE().getSorenRandNum(100, "Additional Event") < kEvent.getAdditionalEventChance(iEvent))
					{
						applyEvent((EventTypes)iEvent, iEventTriggeredId, false);
					}
				}
			}
		}
		else
		{
			bool bSetTimer = true;
			if (kEvent.getAdditionalEventChance(iEvent) > 0)
			{
				if (GC.getGameINLINE().getSorenRandNum(100, "Additional Event 2") >= kEvent.getAdditionalEventChance(iEvent))
				{
					bSetTimer = false;
				}
			}

			if (bSetTimer)
			{
				EventTriggeredData kTriggered = *pTriggeredData;
				kTriggered.m_iTurn = (GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent() * kEvent.getAdditionalEventTime((EventTypes)iEvent)) / 100 + GC.getGameINLINE().getGameTurn();

				const EventTriggeredData* pExistingTriggered = getEventCountdown((EventTypes)iEvent);

				if (NULL != pExistingTriggered)
				{
					kTriggered.m_iTurn = std::min(kTriggered.m_iTurn, pExistingTriggered->m_iTurn); 
				}

				setEventCountdown((EventTypes)iEvent, kTriggered);
				bDeleteTrigger = false;
			}
		}
	}

	if (bDeleteTrigger)
	{
		deleteEventTriggered(iEventTriggeredId);
	}
}

bool CvPlayer::isValidEventTech(TechTypes eTech, EventTypes eEvent, PlayerTypes eOtherPlayer) const
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (0 == kEvent.getTechPercent() && 0 == kEvent.getTechCostPercent())
	{
		return false;
	}

	if (kEvent.getTechPercent() < 0 && GET_TEAM(getTeam()).getResearchProgress(eTech) <= 0)
	{
		return false;
	}

	if (!canResearch(eTech))
	{
		return false;
	}

	if (getResearchTurnsLeft(eTech, true) < kEvent.getTechMinTurnsLeft())
	{
		return false;
	}

	if (NO_PLAYER != eOtherPlayer && !GET_TEAM(GET_PLAYER(eOtherPlayer).getTeam()).isHasTech(eTech))
	{
		return false;
	}

	return true;
}


TechTypes CvPlayer::getBestEventTech(EventTypes eEvent, PlayerTypes eOtherPlayer) const
{
	TechTypes eBestTech = NO_TECH;
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (0 == kEvent.getTechPercent() && 0 == kEvent.getTechCostPercent())
	{
		return NO_TECH;
	}

	if (NO_TECH != kEvent.getTech())
	{
		eBestTech = (TechTypes)kEvent.getTech();
	}
	else
	{
		bool bFoundFlavor = false;
		for (int i = 0; i < GC.getNumFlavorTypes(); ++i)
		{
			if (0 != kEvent.getTechFlavorValue(i))
			{
				bFoundFlavor = true;
				break;
			}
		}

		if (!bFoundFlavor)
		{
			eBestTech = getCurrentResearch();
		}
	}

	if (NO_TECH != eBestTech)
	{
		if (!isValidEventTech(eBestTech, eEvent, eOtherPlayer))
		{
			eBestTech = NO_TECH;
		}
	}
	else
	{
		int ipBestValue = 0;
		for (int iTech = 0; iTech < GC.getNumTechInfos(); ++iTech)
		{
			if (isValidEventTech((TechTypes)iTech, eEvent, eOtherPlayer))
			{
				int iValue = 0;
				for (int i = 0; i < GC.getNumFlavorTypes(); ++i)
				{
					iValue += kEvent.getTechFlavorValue(i) * GC.getTechInfo((TechTypes)iTech).getFlavorValue(i);
				}

				if (iValue > ipBestValue)
				{
					eBestTech = (TechTypes)iTech;
					ipBestValue = iValue;
				}
			}
		}
	}

	return eBestTech;
}

int CvPlayer::getEventCost(EventTypes eEvent, PlayerTypes eOtherPlayer, bool bRandom) const
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	int iGold = kEvent.getGold();
	if (bRandom)
	{
		iGold += kEvent.getRandomGold();
	}
	
	iGold *= std::max(0, getInflationRate() + 100);
	iGold /= 100;

	TechTypes eBestTech = getBestEventTech(eEvent, eOtherPlayer);

	if (NO_TECH != eBestTech)
	{
		iGold -= (kEvent.getTechCostPercent() * GET_TEAM(getTeam()).getResearchCost(eBestTech)) / 100;
	}

	return iGold;
}


void CvPlayer::doEvents()
{
	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_EVENTS))
	{
		return;
	}

	if (isBarbarian() || isMinorCiv())
	{
		return;
	}

	CvEventMap::iterator it = m_mapEventsOccured.begin();
	while (it != m_mapEventsOccured.end())
	{
		if (checkExpireEvent(it->first, it->second))
		{
			expireEvent(it->first, it->second, true);
			it = m_mapEventsOccured.erase(it);
		}
		else
		{
			++it;
		}
	}

	bool bNewEventEligible = true;
	// Civ4 Reimagined: Adjust to game speed
	if (GC.getGameINLINE().getElapsedGameTurns() < GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getEventDelayTurns())
	{
		bNewEventEligible = false;
	}

	if (bNewEventEligible)
	{
		// Civ4 Reimagined: Adjust to game speed
		const int iEventChance = GC.getEraInfo(getCurrentEra()).getEventChancePerTurn() * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getEventProbabilityModifier() / 100;
		if (GC.getGameINLINE().getSorenRandNum(GC.getDefineINT("EVENT_PROBABILITY_ROLL_SIDES"), "Global event check") >= iEventChance)
		{
			bNewEventEligible = false;
		}
	}

	std::vector< std::pair<EventTriggeredData*, int> > aePossibleEventTriggerWeights;
	int iTotalWeight = 0;
	for (int i = 0; i < GC.getNumEventTriggerInfos(); ++i)
	{
		int iWeight = getEventTriggerWeight((EventTriggerTypes)i);
		if (iWeight == -1)
		{
			trigger((EventTriggerTypes)i);
		}
		else if (iWeight > 0 && bNewEventEligible)
		{
			EventTriggeredData* pTriggerData = initTriggeredData((EventTriggerTypes)i);
			if (NULL != pTriggerData)
			{
				iTotalWeight += iWeight;
				aePossibleEventTriggerWeights.push_back(std::make_pair(pTriggerData, iTotalWeight));
			}
		}
	}

	if (iTotalWeight > 0)
	{
		logBBAI("Total Trigger Weight: %d", iTotalWeight);
		bool bFired = false;
		int iValue = GC.getGameINLINE().getSorenRandNum(iTotalWeight, "Event trigger");
		logBBAI("Trigger Rand Number: %d", iValue);
		for (std::vector< std::pair<EventTriggeredData*, int> >::iterator it = aePossibleEventTriggerWeights.begin(); it != aePossibleEventTriggerWeights.end(); ++it)
		{
			EventTriggeredData* pTriggerData = (*it).first;
			if (NULL != pTriggerData)
			{
				if (iValue < (*it).second && !bFired)
				{
					trigger(*pTriggerData);
					bFired = true;
				}
				else
				{
					deleteEventTriggered(pTriggerData->getID());
				}
			}
		}
	}

	std::vector<int> aCleanup;
	for (int i = 0; i < GC.getNumEventInfos(); ++i)
	{
		const EventTriggeredData* pTriggeredData = getEventCountdown((EventTypes)i);
		if (NULL != pTriggeredData)
		{
			if (GC.getGameINLINE().getGameTurn() >= pTriggeredData->m_iTurn)
			{
				applyEvent((EventTypes)i, pTriggeredData->m_iId);
				resetEventCountdown((EventTypes)i);
				aCleanup.push_back(pTriggeredData->m_iId);
			}
		}
	}

	for (std::vector<int>::iterator it = aCleanup.begin(); it != aCleanup.end(); ++it)
	{
		bool bDelete = true;

		for (int i = 0; i < GC.getNumEventInfos(); ++i)
		{
			const EventTriggeredData* pTriggeredData = getEventCountdown((EventTypes)i);
			if (NULL != pTriggeredData)
			{
				if (pTriggeredData->m_iId == *it)
				{
					bDelete = false;
					break;
				}
			}
		}

		if (bDelete)
		{
			deleteEventTriggered(*it);
		}
	}
}


void CvPlayer::expireEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData, bool bFail)
{
	FAssert(getEventOccured(eEvent) == &kTriggeredData);
	FAssert(GC.getEventInfo(eEvent).isQuest() || GC.getGameINLINE().getGameTurn() - kTriggeredData.m_iTurn <= 4);

	if (GC.getEventInfo(eEvent).isQuest())
	{
		CvMessageQueue::iterator it;
		for (it = m_listGameMessages.begin(); it != m_listGameMessages.end(); ++it)
		{
			CvTalkingHeadMessage& message = *it;

			// the trigger ID is stored in the otherwise unused length field
			if (message.getLength() == kTriggeredData.getID()) 
			{
				m_listGameMessages.erase(it);
				gDLL->getInterfaceIFace()->dirtyTurnLog(getID());
				break;
			}
		}

		if (bFail)
		{
			gDLL->getInterfaceIFace()->addHumanMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), gDLL->getText(GC.getEventInfo(eEvent).getQuestFailTextKey()), "AS2D_CIVIC_ADOPT", MESSAGE_TYPE_MINOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"));
		}
	}
}

bool CvPlayer::checkExpireEvent(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const
{
	CvEventInfo& kEvent = GC.getEventInfo(eEvent);

	if (!CvString(kEvent.getPythonExpireCheck()).empty())
	{
		long lResult;

		CyArgsList argsList;
		argsList.add(eEvent);
		argsList.add(gDLL->getPythonIFace()->makePythonObject(&kTriggeredData));

		gDLL->getPythonIFace()->callFunction(PYRandomEventModule, kEvent.getPythonExpireCheck(), argsList.makeFunctionArgs(), &lResult);

		if (0 != lResult)
		{
			return true;
		}
	}

	if (!kEvent.isQuest())
	{
		if (GC.getGameINLINE().getGameTurn() - kTriggeredData.m_iTurn > 2)
		{
			return true;
		}

		return false;
	}

	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(kTriggeredData.m_eTrigger);

	FAssert(kTriggeredData.m_ePlayer != NO_PLAYER);

	CvPlayer& kPlayer = GET_PLAYER(kTriggeredData.m_ePlayer);

	if (kTrigger.isStateReligion() & kTrigger.isPickReligion())
	{
		if (kPlayer.getStateReligion() != kTriggeredData.m_eReligion)
		{
			return true;
		}
	}

	if (NO_CIVIC != kTrigger.getCivic())
	{
		if (!kPlayer.isCivic((CivicTypes)kTrigger.getCivic()))
		{
			return true;
		}
	}

	if (kTriggeredData.m_iCityId != -1)
	{
		if (NULL == kPlayer.getCity(kTriggeredData.m_iCityId))
		{
			return true;
		}
	}

	if (kTriggeredData.m_iUnitId != -1)
	{
		if (NULL == kPlayer.getUnit(kTriggeredData.m_iUnitId))
		{
			return true;
		}
	}

	if (NO_PLAYER != kTriggeredData.m_eOtherPlayer)
	{
		if (!GET_PLAYER(kTriggeredData.m_eOtherPlayer).isAlive())
		{
			return true;
		}

		if (kTriggeredData.m_iOtherPlayerCityId != -1)
		{
			if (NULL == GET_PLAYER(kTriggeredData.m_eOtherPlayer).getCity(kTriggeredData.m_iOtherPlayerCityId))
			{
				return true;
			}
		}
	}

	if (kTrigger.getNumObsoleteTechs() > 0)
	{
		for (int iI = 0; iI < kTrigger.getNumObsoleteTechs(); iI++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)(kTrigger.getObsoleteTech(iI))))
			{
				return true;
			}
		}
	}

	return false;
}


void CvPlayer::trigger(EventTriggerTypes eTrigger)
{
	initTriggeredData(eTrigger, true);
}

void CvPlayer::trigger(const EventTriggeredData& kData)
{
	// Civ4 Reimagined
	logBBAI("%S (Player %d): %S (Player %d) has triggered: %d", getCivilizationDescription(0), getID(), GET_PLAYER((PlayerTypes)kData.m_ePlayer).getCivilizationDescription(0), (int)kData.m_ePlayer, (int)kData.m_eTrigger);

	if (isHuman())
	{
		CvPopupInfo* pInfo = new CvPopupInfo(BUTTONPOPUP_EVENT, kData.getID());
		addPopup(pInfo);
	}
	else
	{
		EventTypes eEvent = AI_chooseEvent(kData.getID());
		if (NO_EVENT != eEvent)
		{
			applyEvent(eEvent, kData.getID());
		}
	}
}

bool CvPlayer::canTrigger(EventTriggerTypes eTrigger, PlayerTypes ePlayer, ReligionTypes eReligion) const
{
	if (!isAlive())
	{
		return false;
	}

	if (getID() == ePlayer)
	{
		return false;
	}

	CvPlayer& kPlayer = GET_PLAYER(ePlayer);
	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	if (getTeam() == kPlayer.getTeam())
	{
		return false;
	}

	if (!kTrigger.isPickPlayer())
	{
		return false;
	}

	if (!GET_TEAM(getTeam()).isHasMet(kPlayer.getTeam()))
	{
		return false;
	}

	if (isHuman() && kTrigger.isOtherPlayerAI())
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isAtWar(kPlayer.getTeam()) != kTrigger.isOtherPlayerWar())
	{
		return false;
	}

	if (NO_TECH != kTrigger.getOtherPlayerHasTech())
	{
		if (!GET_TEAM(getTeam()).isHasTech((TechTypes)kTrigger.getOtherPlayerHasTech()))
		{
			return false;
		}
	}

	if (kTrigger.getOtherPlayerShareBorders() > 0)
	{
		int iCount = 0;

		for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); ++iI)
		{
			CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);

			if (!pLoopPlot->isWater())
			{
				if ((pLoopPlot->getOwnerINLINE() == getID()) && pLoopPlot->isAdjacentPlayer(ePlayer, true))
				{
					++iCount;
				}
			}
		}

		if (iCount < kTrigger.getOtherPlayerShareBorders())
		{
			return false;
		}
	}

	if (NO_RELIGION != eReligion)
	{
		bool bHasReligion = kTrigger.isStateReligion() ? (getStateReligion() == eReligion) : (getHasReligionCount(eReligion) > 0);

		if (kTrigger.isOtherPlayerHasReligion())
		{
			if (!bHasReligion)
			{
				return false;
			}
		}

		if (kTrigger.isOtherPlayerHasOtherReligion())
		{
			if (bHasReligion)
			{
				return false;
			}

			if (kTrigger.isStateReligion() && getStateReligion() == NO_RELIGION)
			{
				return false;
			}

		}
	}

	return true;
}

CvCity* CvPlayer::pickTriggerCity(EventTriggerTypes eTrigger) const
{
	CvCity* pCity = NULL;
	std::vector<CvCity*> apCities;
	int iLoop;
	int ipBestValue = MIN_INT;
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		int iValue = pLoopCity->getTriggerValue(eTrigger);

		if (iValue >= ipBestValue && iValue != MIN_INT)
		{
			if (iValue > ipBestValue)
			{
				apCities.clear();
				ipBestValue = iValue;
			}

			apCities.push_back(pLoopCity);
		}
	}
	
	if (apCities.size() > 0)
	{
		int iChosen = GC.getGameINLINE().getSorenRandNum(apCities.size(), "Event pick city");
		pCity = apCities[iChosen];
	}

	return pCity;
}

CvUnit* CvPlayer::pickTriggerUnit(EventTriggerTypes eTrigger, CvPlot* pPlot, bool bPickPlot) const
{
	CvUnit* pUnit = NULL;
	std::vector<CvUnit*> apUnits;
	int iLoop;
	int ipBestValue = MIN_INT;
	for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		int iValue = pLoopUnit->getTriggerValue(eTrigger, pPlot, bPickPlot);

		if (iValue >= ipBestValue && iValue != MIN_INT)
		{
			if (iValue > ipBestValue)
			{
				apUnits.clear();
				ipBestValue = iValue;
			}
			apUnits.push_back(pLoopUnit);
		}
	}

	if (apUnits.size() > 0)
	{
		int iChosen = GC.getGameINLINE().getSorenRandNum(apUnits.size(), "Event pick unit");
		pUnit = apUnits[iChosen];
	}

	return pUnit;
}

int CvPlayer::getEventTriggerWeight(EventTriggerTypes eTrigger) const
{
	CvEventTriggerInfo& kTrigger = GC.getEventTriggerInfo(eTrigger);

	if (NO_HANDICAP != kTrigger.getMinDifficulty())
	{
		if (GC.getGameINLINE().getHandicapType() < kTrigger.getMinDifficulty())
		{
			return 0;
		}
	}

	if (kTrigger.isSinglePlayer() && GC.getGameINLINE().isGameMultiPlayer())
	{
		return 0;
	}

	if (!GC.getGameINLINE().isEventActive(eTrigger))
	{
		return 0;
	}

	if (kTrigger.getNumObsoleteTechs() > 0)
	{
		for (int iI = 0; iI < kTrigger.getNumObsoleteTechs(); iI++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)(kTrigger.getObsoleteTech(iI))))
			{
				return 0;
			}
		}
	}

	if (!kTrigger.isRecurring())
	{
		if (isTriggerFired(eTrigger))
		{
			return 0;
		}
	}

	if (kTrigger.getNumPrereqOrTechs() > 0)
	{
		bool bFoundValid = false;

		for (int iI = 0; iI < kTrigger.getNumPrereqOrTechs(); iI++)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)(kTrigger.getPrereqOrTechs(iI))))
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return 0;
		}
	}


	if (kTrigger.getNumPrereqAndTechs() > 0)
	{
		bool bFoundValid = true;

		for (int iI = 0; iI < kTrigger.getNumPrereqAndTechs(); iI++)
		{
			if (!GET_TEAM(getTeam()).isHasTech((TechTypes)(kTrigger.getPrereqAndTechs(iI))))
			{
				bFoundValid = false;
				break;
			}
		}

		if (!bFoundValid)
		{
			return 0;
		}
	}

	if (kTrigger.getNumPrereqEvents() > 0)
	{
		bool bFoundValid = true;
		for (int iI = 0; iI < kTrigger.getNumPrereqEvents(); iI++)
		{
			if (NULL == getEventOccured((EventTypes)kTrigger.getPrereqEvent(iI)))
			{
				bFoundValid = false;
				break;
			}
		}

		if (!bFoundValid)
		{
			return 0;
		}
	}

	if (NO_CIVIC != kTrigger.getCivic())
	{
		bool bFoundValid = false;

		for (int iI = 0; iI < GC.getNumCivicOptionInfos(); ++iI)
		{
			if (getCivics((CivicOptionTypes)iI) == kTrigger.getCivic())
			{
				bFoundValid = true;
				break;
			}
		}

		if (!bFoundValid)
		{
			return 0;
		}
	}

	if (kTrigger.getMinTreasury() > 0)
	{
		if (getGold() < kTrigger.getMinTreasury())
		{
			return 0;
		}
	}

	if (GC.getMapINLINE().getNumLandAreas() < kTrigger.getMinMapLandmass())
	{
		return 0;
	}

	if (kTrigger.getMinOurLandmass() > 0 || kTrigger.getMaxOurLandmass() != -1)
	{
		int iNumLandmass = 0;

		int iLoop;
		for (CvArea* pArea = GC.getMapINLINE().firstArea(&iLoop); NULL != pArea; pArea = GC.getMapINLINE().nextArea(&iLoop))
		{
			if (!pArea->isWater())
			{
				if (pArea->getCitiesPerPlayer(getID()) > 0)
				{
					++iNumLandmass;
				}
			}
		}

		if (iNumLandmass < kTrigger.getMinOurLandmass())
		{
			return 0;
		}

		if (kTrigger.getMaxOurLandmass() != -1 && iNumLandmass > kTrigger.getMaxOurLandmass())
		{
			return 0;
		}
	}

	if (kTrigger.getProbability() < 0)
	{
		return kTrigger.getProbability();
	}

	int iProbability = kTrigger.getProbability();

	if (kTrigger.isProbabilityUnitMultiply() && kTrigger.getNumUnits() > 0)
	{
		int iNumUnits = 0;
		int iLoop;
		for (CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
		{
			if (MIN_INT != pLoopUnit->getTriggerValue(eTrigger, NULL, true))
			{
				++iNumUnits;
			}
		}

		iProbability *= iNumUnits;
	}

	if (kTrigger.isProbabilityBuildingMultiply() && kTrigger.getNumBuildings() > 0)
	{
		int iNumBuildings = 0;
		for (int i = 0; i < kTrigger.getNumBuildingsRequired(); ++i)
		{
			if (kTrigger.getBuildingRequired(i) != NO_BUILDINGCLASS)
			{
				iNumBuildings += getBuildingClassCount((BuildingClassTypes)kTrigger.getBuildingRequired(i));
			}
		}

		iProbability *= iNumBuildings;
	}

	return iProbability;
}


PlayerTypes CvPlayer::getSplitEmpirePlayer(int iAreaId) const
{
	// can't create different derivative civs on the same continent
	for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; ++iPlayer)
	{
		CvPlayer& kLoopPlayer = GET_PLAYER((PlayerTypes)iPlayer);
		if (kLoopPlayer.isAlive() && kLoopPlayer.getParent() == getID())
		{
			CvCity* pLoopCapital = kLoopPlayer.getCapitalCity();
			if (NULL != pLoopCapital)
			{
				if (pLoopCapital->area()->getID() == iAreaId)
				{
					return NO_PLAYER;
				}
			}
		}
	}

	PlayerTypes eNewPlayer = NO_PLAYER;

	// Try to find a player who's never been in the game before
	for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
	{
		if (!GET_PLAYER((PlayerTypes)i).isEverAlive())
		{
			eNewPlayer = (PlayerTypes)i;
			break;
		}
	}

	if (eNewPlayer == NO_PLAYER)
	{
		// Try to recycle a dead player
		for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
		{
			if (!GET_PLAYER((PlayerTypes)i).isAlive())
			{
				eNewPlayer = (PlayerTypes)i;
				break;
			}
		}
	}

	return eNewPlayer;
}

bool CvPlayer::canSplitEmpire() const
{
	int iLoopArea;

	if (GC.getGameINLINE().isOption(GAMEOPTION_NO_VASSAL_STATES))
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isAVassal())
	{
		return false;
	}

	CivLeaderArray aLeaders;
	if (!getSplitEmpireLeaders(aLeaders))
	{
		return false;
	}

	bool bFoundArea = false;

	for (CvArea* pLoopArea = GC.getMapINLINE().firstArea(&iLoopArea); pLoopArea != NULL; pLoopArea = GC.getMapINLINE().nextArea(&iLoopArea))
	{
		if (canSplitArea(pLoopArea->getID()))
		{
			bFoundArea = true;
			break;
		}
	}

	if (!bFoundArea)
	{
		return false;
	}

	return true;
}

bool CvPlayer::canSplitArea(int iAreaId) const
{
	CvArea* pArea = GC.getMapINLINE().getArea(iAreaId);
	CvCity* pCapital = getCapitalCity();

	if (NULL == pCapital)
	{
		return false;
	}

	if (NULL == pArea || pArea == pCapital->area())
	{
		return false;
	}

	if (0 == pArea->getCitiesPerPlayer(getID()))
	{
		return false;
	}

	PlayerTypes ePlayer = getSplitEmpirePlayer(pArea->getID());
	if (NO_PLAYER == ePlayer)
	{
		return false;
	}

	if (!GET_PLAYER(ePlayer).isAlive())
	{
		if (pArea->getCitiesPerPlayer(getID()) <= 1)
		{
			return false;
		}
	}

	return true;
}

bool CvPlayer::getSplitEmpireLeaders(CivLeaderArray& aLeaders) const
{
	aLeaders.clear();

	for (int i = 0; i < GC.getNumCivilizationInfos(); ++i)
	{
		bool bValid = true;

		if (getCivilizationType() == i)
		{
			bValid = false;
		}

		if (bValid)
		{
			if (!GC.getCivilizationInfo((CivilizationTypes)i).isPlayable() || !GC.getCivilizationInfo((CivilizationTypes)i).isAIPlayable())
			{
				bValid = false;
			}
		}

		if (bValid)
		{
			for (int j = 0; j < MAX_CIV_PLAYERS; ++j)
			{
				if (getID() != j && GET_PLAYER((PlayerTypes)j).isEverAlive() && GET_PLAYER((PlayerTypes)j).getCivilizationType() == i)
				{
					bValid = false;
					break;
				}
			}
		}

		if (bValid)
		{
			for (int j = 0; j < GC.getNumLeaderHeadInfos(); ++j)
			{
				bool bLeaderValid = true;
				if (!GC.getCivilizationInfo((CivilizationTypes)i).isLeaders(j) && !GC.getGameINLINE().isOption(GAMEOPTION_LEAD_ANY_CIV))
				{
					bLeaderValid = false;
				}

				if (bLeaderValid)
				{
					for (int k = 0; k < MAX_CIV_PLAYERS; ++k)
					{
						if (GET_PLAYER((PlayerTypes)k).isEverAlive() && GET_PLAYER((PlayerTypes)k).getPersonalityType() == j)
						{
							bLeaderValid = false;
						}
					}
				}

				if (bLeaderValid)
				{
					aLeaders.push_back(std::make_pair((CivilizationTypes)i, (LeaderHeadTypes)j));
				}
			}
		}
	}

	return (aLeaders.size() > 0);
}

bool CvPlayer::splitEmpire(int iAreaId)
{
	PROFILE_FUNC();

	if (!canSplitEmpire())
	{
		return false;
	}

	if (!canSplitArea(iAreaId))
	{
		return false;
	}

	CvArea* pArea = GC.getMapINLINE().getArea(iAreaId);
	if (NULL == pArea)
	{
		return false;
	}

	PlayerTypes eNewPlayer = getSplitEmpirePlayer(iAreaId);
	if (eNewPlayer == NO_PLAYER)
	{
		return false;
	}

	bool bPlayerExists = GET_TEAM(GET_PLAYER(eNewPlayer).getTeam()).isAlive();
	FAssert(!bPlayerExists);
	if (!bPlayerExists)
	{
		int ipBestValue = -1;
		LeaderHeadTypes eBestLeader = NO_LEADER;
		CivilizationTypes eBestCiv = NO_CIVILIZATION;

		CivLeaderArray aLeaders;
		if (getSplitEmpireLeaders(aLeaders))
		{
			CivLeaderArray::iterator it;
			for (it = aLeaders.begin(); it != aLeaders.end(); ++it)
			{
				int iValue = (1 + GC.getGameINLINE().getSorenRandNum(100, "Choosing Split Personality"));

				if (GC.getCivilizationInfo(getCivilizationType()).getDerivativeCiv() == it->first)
				{
					iValue += 1000;
				}

				if (iValue > ipBestValue)
				{
					ipBestValue = iValue;
					eBestLeader = it->second;
					eBestCiv = it->first;
				}
			}
		}

		if (eBestLeader == NO_LEADER || eBestCiv == NO_CIVILIZATION)
		{
			return false;
		}

		CvWString szMessage = gDLL->getText("TXT_KEY_MISC_EMPIRE_SPLIT", getNameKey(), GC.getCivilizationInfo(eBestCiv).getShortDescriptionKey(), GC.getLeaderHeadInfo(eBestLeader).getTextKeyWide());
		for (int i = 0; i < MAX_CIV_PLAYERS; ++i)
		{
			if (GET_PLAYER((PlayerTypes)i).isAlive())
			{
				if (i == getID() || i == eNewPlayer || GET_TEAM(GET_PLAYER((PlayerTypes)i).getTeam()).isHasMet(getTeam()))
				{
					gDLL->getInterfaceIFace()->addHumanMessage((PlayerTypes)i, false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_REVOLTEND", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getInterfaceArtInfo("INTERFACE_CITY_BAR_CAPITAL_TEXTURE")->getPath());
				}
			}
		}
		GC.getGameINLINE().addReplayMessage(REPLAY_MESSAGE_MAJOR_EVENT, getID(), szMessage, -1, -1, (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"));

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      12/30/08                                jdog5000      */
/*                                                                                              */
/* Bugfix                                                                                       */
/************************************************************************************************/
/*
		// remove leftover culture from old recycled player
		for (int iPlot = 0; iPlot < GC.getMapINLINE().numPlotsINLINE(); ++iPlot)
		{
			CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iPlot);

			pLoopPlot->setCulture(eNewPlayer, 0, false, false);
		}
*/
		// Clearing plot culture along with many other bits of data now handled by CvGame::addPlayer
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/

		GC.getGameINLINE().addPlayer(eNewPlayer, eBestLeader, eBestCiv);
		GET_PLAYER(eNewPlayer).setParent(getID());
		GC.getInitCore().setLeaderName(eNewPlayer, GC.getLeaderHeadInfo(eBestLeader).getTextKeyWide());

		CvTeam& kNewTeam = GET_TEAM(GET_PLAYER(eNewPlayer).getTeam());
		for (int i = 0; i < GC.getNumTechInfos(); ++i)
		{
			if (GET_TEAM(getTeam()).isHasTech((TechTypes)i))
			{
				kNewTeam.setHasTech((TechTypes)i, true, eNewPlayer, false, false);
				if (GET_TEAM(getTeam()).isNoTradeTech((TechTypes)i) || GC.getGameINLINE().isOption(GAMEOPTION_NO_TECH_BROKERING))
				{
					kNewTeam.setNoTradeTech((TechTypes)i, true);
				}
			}
		}

		for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
		{
			CvTeam& kLoopTeam = GET_TEAM((TeamTypes)iTeam);

			if (kLoopTeam.isAlive())
			{
				kNewTeam.setEspionagePointsAgainstTeam((TeamTypes)iTeam, GET_TEAM(getTeam()).getEspionagePointsAgainstTeam((TeamTypes)iTeam));
				kLoopTeam.setEspionagePointsAgainstTeam(GET_PLAYER(eNewPlayer).getTeam(), kLoopTeam.getEspionagePointsAgainstTeam(getTeam()));
			}
		}
		kNewTeam.setEspionagePointsEver(GET_TEAM(getTeam()).getEspionagePointsEver());
		
		// Civ4 Reimagined
		GET_PLAYER(eNewPlayer).changeGoldenAgeTurns(GET_PLAYER(eNewPlayer).getGoldenAgeLength() + 1);
		GET_PLAYER(eNewPlayer).changeExtraHappiness(2);
		GET_PLAYER(eNewPlayer).changeExtraHealth(4);
		
		for (int iI = 0; iI < NUM_COMMERCE_TYPES; iI++)
		{
			GET_PLAYER(eNewPlayer).changeCommerceRateModifier((CommerceTypes)iI, 100);
		}

		GET_TEAM(getTeam()).assignVassal(GET_PLAYER(eNewPlayer).getTeam(), false);

		AI_updateBonusValue();
	}

	std::vector< std::pair<int, int> > aCultures;
	for (int iPlot = 0; iPlot < GC.getMapINLINE().numPlotsINLINE(); ++iPlot)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iPlot);

		bool bTranferPlot = false;

		if (!bTranferPlot && pLoopPlot->area() == pArea)
		{
			bTranferPlot = true;
		}

		if (!bTranferPlot)
		{
			CvCity* pWorkingCity = pLoopPlot->getWorkingCity();
			if (NULL != pWorkingCity && pWorkingCity->getOwnerINLINE() == getID() && pWorkingCity->area() == pArea)
			{
				bTranferPlot = true;
			}
		}

		if (!bTranferPlot && pLoopPlot->isWater() && pLoopPlot->isAdjacentToArea(pArea))
		{
			bTranferPlot = true;
		}

		if (bTranferPlot)
		{
			int iCulture = pLoopPlot->getCulture(getID());

			if (bPlayerExists)
			{
				iCulture = std::max(iCulture, pLoopPlot->getCulture(eNewPlayer));
			}

			aCultures.push_back(std::make_pair(iPlot, iCulture));
		}

		if (pLoopPlot->isRevealed(getTeam(), false))
		{
			pLoopPlot->setRevealed(GET_PLAYER(eNewPlayer).getTeam(), true, false, getTeam(), false);
		}
	}

	int iLoop;
	int iWorker = 0;
	UnitClassTypes eWorkerIndex = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_WORKER");
	UnitTypes eWorker = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(eNewPlayer).getCivilizationType()).getCivilizationUnits(eWorkerIndex);
	UnitTypes eSettler = (UnitTypes)GC.getCivilizationInfo(GET_PLAYER(eNewPlayer).getCivilizationType()).getCivilizationUnits((UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_SETTLER"));
	for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		if (pLoopCity->area() == pArea)
		{
			int iCulture = pLoopCity->getCultureTimes100(getID());
			CvPlot* pPlot = pLoopCity->plot();

			GET_PLAYER(eNewPlayer).acquireCity(pLoopCity, false, true, false);

			if (NULL != pPlot)
			{
				CvCity* pCity = pPlot->getPlotCity();
				if (NULL != pCity)
				{
					pCity->setCultureTimes100(eNewPlayer, iCulture, false, false);
					pCity->changePopulation(1); // Civ4 Reimagined

					// add settlers and worker
					if (pCity->isCapital())
					{
						GET_PLAYER(eNewPlayer).initUnit(eSettler, pCity->getX_INLINE(), pCity->getY_INLINE(), UNITAI_SETTLE);
						GET_PLAYER(eNewPlayer).initUnit(eSettler, pCity->getX_INLINE(), pCity->getY_INLINE(), UNITAI_SETTLE);
						GET_PLAYER(eNewPlayer).initUnit(eWorker, pCity->getX_INLINE(), pCity->getY_INLINE(), UNITAI_WORKER);
						GET_PLAYER(eNewPlayer).initUnit(eWorker, pCity->getX_INLINE(), pCity->getY_INLINE(), UNITAI_WORKER);
					}

					if (GET_PLAYER(eNewPlayer).getUnitClassCount(eWorkerIndex) < GC.getUnitClassInfo(eWorkerIndex).getMaxPlayerInstances())
					{
						GET_PLAYER(eNewPlayer).initUnit(eWorker, pCity->getX_INLINE(), pCity->getY_INLINE(), UNITAI_WORKER);
					}
					
				}

				for (int i = 0; i < GC.getDefineINT("COLONY_NUM_FREE_DEFENDERS"); ++i)
				{
					pCity->initConscriptedUnit();
				}
			}
		}
	}

	for (uint i = 0; i < aCultures.size(); ++i)
	{
		CvPlot* pPlot = GC.getMapINLINE().plotByIndexINLINE(aCultures[i].first);
		pPlot->setCulture(eNewPlayer, aCultures[i].second, true, false);
		pPlot->setCulture(getID(), 0, true, false);

		for (int iTeam = 0; iTeam < MAX_TEAMS; ++iTeam)
		{
			if (pPlot->getRevealedOwner((TeamTypes)iTeam, false) == getID())
			{
				pPlot->setRevealedOwner((TeamTypes)iTeam, eNewPlayer);
			}
		}
	}


	GC.getGameINLINE().updatePlotGroups();

	// K-Mod
	GET_PLAYER(eNewPlayer).AI_updateAttitudeCache(getID());
	GET_PLAYER(getID()).AI_updateAttitudeCache(eNewPlayer);
	// K-Mod end

	// Civ4 Reimagined
	GET_PLAYER(eNewPlayer).updateCacheData();

	return true;
}

bool CvPlayer::isValidTriggerReligion(const CvEventTriggerInfo& kTrigger, CvCity* pCity, ReligionTypes eReligion) const
{
	if (kTrigger.getNumReligionsRequired() > 0)
	{
		bool bFound = false;

		for (int i = 0; i < kTrigger.getNumReligionsRequired(); ++i)
		{
			if (eReligion == kTrigger.getReligionRequired(i))
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			return false;
		}
	}

	if (NULL != pCity)
	{
		if (!pCity->isHasReligion(eReligion))
		{
			return false;
		}

		if (kTrigger.isHolyCity())
		{
			if (!pCity->isHolyCity(eReligion))
			{
				return false;
			}
		}
	}
	else
	{
		if (0 == getHasReligionCount(eReligion))
		{
			return false;
		}

		if (kTrigger.isHolyCity())
		{
			CvCity* pHolyCity = GC.getGameINLINE().getHolyCity(eReligion);
			if (NULL == pHolyCity || pHolyCity->getOwnerINLINE() != getID())
			{
				return false;
			}
		}
	}

	return true;
}

bool CvPlayer::isValidTriggerCorporation(const CvEventTriggerInfo& kTrigger, CvCity* pCity, CorporationTypes eCorporation) const
{
	if (kTrigger.getNumCorporationsRequired() > 0)
	{
		bool bFound = false;

		for (int i = 0; i < kTrigger.getNumCorporationsRequired(); ++i)
		{
			if (eCorporation == kTrigger.getCorporationRequired(i))
			{
				bFound = true;
				break;
			}
		}

		if (!bFound)
		{
			return false;
		}
	}

	if (NULL != pCity)
	{
		if (!pCity->isHasCorporation(eCorporation))
		{
			return false;
		}

		if (kTrigger.isHeadquarters())
		{
			if (!pCity->isHeadquarters(eCorporation))
			{
				return false;
			}
		}
	}
	else
	{
		/* if (getHasCorporationCount(eCorporation) > 0)
		{
			return true;
		} */
		// K-Mod. (bugfix)
		if (getHasCorporationCount(eCorporation) == 0)
			return false;
		// K-Mod end

		if (kTrigger.isHeadquarters())
		{
			CvCity* pHeadquarters = GC.getGameINLINE().getHeadquarters(eCorporation);
			if (NULL == pHeadquarters || pHeadquarters->getOwnerINLINE() != getID())
			{
				return false;
			}
		}
	}

	//return false;
	return true; // K-Mod. (bugfix)
}

void CvPlayer::launch(VictoryTypes eVictory)
{
	CvTeam& kTeam = GET_TEAM(getTeam());

	if (!kTeam.canLaunch(eVictory))
	{
		return;
	}

	kTeam.finalizeProjectArtTypes();
	kTeam.setVictoryCountdown(eVictory, kTeam.getVictoryDelay(eVictory));

	//gDLL->getEngineIFace()->AddLaunch(getID());
	// K-Mod. The spaceship launch causes pitboss to crash
	if (GC.IsGraphicsInitialized())
		gDLL->getEngineIFace()->AddLaunch(getID());
	// K-Mod end.

	kTeam.setCanLaunch(eVictory, false);

	CvCity *capital = getCapitalCity();

	//message
	CvWString szBuffer;
	for(int i = 0; i < MAX_PLAYERS; ++i)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			int plotX = -1;
			int plotY = -1;
			if((capital != NULL) && capital->isRevealed(GET_PLAYER((PlayerTypes) i).getTeam(), false))
			{
				plotX = capital->getX();
				plotY = capital->getY();
			}

			if (GET_PLAYER((PlayerTypes)i).getTeam() == getTeam())
			{
				szBuffer = gDLL->getText("TXT_KEY_VICTORY_YOU_HAVE_LAUNCHED");
			}
			else
			{
				szBuffer = gDLL->getText("TXT_KEY_VICTORY_TEAM_HAS_LAUNCHED", GET_TEAM(getTeam()).getName().GetCString());
			}

			gDLL->getInterfaceIFace()->addHumanMessage(((PlayerTypes)i), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_CULTURELEVEL", MESSAGE_TYPE_MAJOR_EVENT, ARTFILEMGR.getMiscArtInfo("SPACE_SHIP_BUTTON")->getPath(), (ColorTypes)GC.getInfoTypeForString("COLOR_HIGHLIGHT_TEXT"), plotX, plotY, true, true);
		}
	}
}

bool CvPlayer::isFreePromotion(UnitCombatTypes eUnitCombat, PromotionTypes ePromotion) const
{
	for (UnitCombatPromotionArray::const_iterator it = m_aFreeUnitCombatPromotions.begin(); it != m_aFreeUnitCombatPromotions.end(); ++it)
	{
		if ((*it).first == eUnitCombat && (*it).second == ePromotion)
		{
			return true;
		}
	}

	return false;
}

void CvPlayer::setFreePromotion(UnitCombatTypes eUnitCombat, PromotionTypes ePromotion, bool bFree)
{
	for (UnitCombatPromotionArray::iterator it = m_aFreeUnitCombatPromotions.begin(); it != m_aFreeUnitCombatPromotions.end(); ++it)
	{
		if ((*it).first == eUnitCombat && (*it).second == ePromotion)
		{
			if (!bFree)
			{
				m_aFreeUnitCombatPromotions.erase(it);
			}
			return;
		}
	}

	if (bFree)
	{
		m_aFreeUnitCombatPromotions.push_back(std::make_pair(eUnitCombat, ePromotion));
	}
}

bool CvPlayer::isFreePromotion(UnitClassTypes eUnitClass, PromotionTypes ePromotion) const
{
	for (UnitClassPromotionArray::const_iterator it = m_aFreeUnitClassPromotions.begin(); it != m_aFreeUnitClassPromotions.end(); ++it)
	{
		if ((*it).first == eUnitClass && (*it).second == ePromotion)
		{
			return true;
		}
	}

	return false;
}

void CvPlayer::setFreePromotion(UnitClassTypes eUnitClass, PromotionTypes ePromotion, bool bFree)
{
	for (UnitClassPromotionArray::iterator it = m_aFreeUnitClassPromotions.begin(); it != m_aFreeUnitClassPromotions.end(); ++it)
	{
		if ((*it).first == eUnitClass && (*it).second == ePromotion)
		{
			if (!bFree)
			{
				m_aFreeUnitClassPromotions.erase(it);
			}
			return;
		}
	}

	if (bFree)
	{
		m_aFreeUnitClassPromotions.push_back(std::make_pair(eUnitClass, ePromotion));
	}
}

PlayerVoteTypes CvPlayer::getVote(int iId) const
{
	for (std::vector< std::pair<int, PlayerVoteTypes> >::const_iterator it = m_aVote.begin(); it != m_aVote.end(); ++it)
	{
		if ((*it).first == iId)
		{
			return ((*it).second);
		}
	}

	return NO_PLAYER_VOTE;
}

void CvPlayer::setVote(int iId, PlayerVoteTypes ePlayerVote)
{
	for (std::vector< std::pair<int, PlayerVoteTypes> >::iterator it = m_aVote.begin(); it != m_aVote.end(); ++it)
	{
		if ((*it).first == iId)
		{
			if (ePlayerVote == NO_PLAYER_VOTE)
			{
				m_aVote.erase(it);
			}
			else
			{
				(*it).second = ePlayerVote;
			}
			return;
		}
	}

	if (ePlayerVote != NO_PLAYER_VOTE)
	{
		m_aVote.push_back(std::make_pair(iId, ePlayerVote));
	}
}

int CvPlayer::getUnitExtraCost(UnitClassTypes eUnitClass) const
{
	for (std::vector< std::pair<UnitClassTypes, int> >::const_iterator it = m_aUnitExtraCosts.begin(); it != m_aUnitExtraCosts.end(); ++it)
	{
		if ((*it).first == eUnitClass)
		{
			return ((*it).second);
		}
	}

	return 0;
}

void CvPlayer::setUnitExtraCost(UnitClassTypes eUnitClass, int iCost)
{
	for (std::vector< std::pair<UnitClassTypes, int> >::iterator it = m_aUnitExtraCosts.begin(); it != m_aUnitExtraCosts.end(); ++it)
	{
		if ((*it).first == eUnitClass)
		{
			if (0 == iCost)
			{
				m_aUnitExtraCosts.erase(it);
			}
			else
			{
				(*it).second = iCost;
			}
			return;
		}
	}

	if (0 != iCost)
	{
		m_aUnitExtraCosts.push_back(std::make_pair(eUnitClass, iCost));
	}
}

// CACHE: cache frequently used values
///////////////////////////////////////
bool CvPlayer::hasShrine(ReligionTypes eReligion)
{
	bool bHasShrine = false;
	
	if (eReligion != NO_RELIGION)
	{
		CvCity* pHolyCity = GC.getGameINLINE().getHolyCity(eReligion);
		
		// if the holy city exists, and we own it
		if (pHolyCity != NULL && pHolyCity->getOwnerINLINE() == getID())
			bHasShrine = pHolyCity->hasShrine(eReligion);
	}
	return bHasShrine;
}

void CvPlayer::invalidatePopulationRankCache()
{
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->invalidatePopulationRankCache();
	}
}

void CvPlayer::invalidateYieldRankCache(YieldTypes eYield)
{
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->invalidateYieldRankCache();
	}
}

void CvPlayer::invalidateCommerceRankCache(CommerceTypes eCommerce)
{
	int iLoop;
	CvCity* pLoopCity;
	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->invalidateCommerceRankCache();
	}
}


void CvPlayer::doUpdateCacheOnTurn()
{
	updateCommerceAboveAveragePopulation(); // Civ4 Reimagined

	// add this back, after testing without it
	// invalidateYieldRankCache();
}

void CvPlayer::processVoteSourceBonus(VoteSourceTypes eVoteSource, bool bActive)
{
	int iLoop;
	for (CvCity* pCity = firstCity(&iLoop); NULL != pCity; pCity = nextCity(&iLoop))
	{
		pCity->processVoteSourceBonus(eVoteSource, bActive);
	}
}

int CvPlayer::getVotes(VoteTypes eVote, VoteSourceTypes eVoteSource) const
{
	int iVotes = 0;

	ReligionTypes eReligion = GC.getGameINLINE().getVoteSourceReligion(eVoteSource);

	if (NO_VOTE == eVote)
	{
		if (NO_RELIGION != eReligion)
		{
			iVotes = getReligionPopulation(eReligion);
		}
		else
		{
			iVotes = getTotalPopulation();
		}
	}
	else
	{
		if (!GC.getVoteInfo(eVote).isVoteSourceType(eVoteSource))
		{
			return 0;
		}

		if (GC.getVoteInfo(eVote).isCivVoting())
		{
			if (NO_RELIGION == eReligion || getHasReligionCount(eReligion) > 0)
			{
				iVotes = 1;
			}
		}
		else if (GC.getVoteInfo(eVote).isCityVoting())
		{
			if (NO_RELIGION != eReligion)
			{
				iVotes = getHasReligionCount(eReligion);
			}
			else
			{
				iVotes = getNumCities();
			}
		}
		else
		{
			if (NO_RELIGION == eReligion)
			{
				iVotes = getTotalPopulation();
			}
			else
			{
				iVotes = getReligionPopulation(eReligion);
			}
		}

		if (NO_RELIGION != eReligion && getStateReligion() == eReligion)
		{
			iVotes *= (100 + GC.getVoteInfo(eVote).getStateReligionVotePercent());
			iVotes /= 100;

			// Civ4 Reimagined
			if (hasGoodRelationsWithPope())
			{
				iVotes *= (100 + getReligiousVoteModifier());
				iVotes /= 100;
			}
		}
	}

	return iVotes;
}

bool CvPlayer::canDoResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const
{
	CvTeam& kOurTeam = GET_TEAM(getTeam());

	if (NO_PLAYER != kData.ePlayer)
	{
		if (!kOurTeam.isHasMet(GET_PLAYER(kData.ePlayer).getTeam()))
		{
			return false;
		}
	}

	if (GC.getVoteInfo(kData.eVote).isOpenBorders())
	{
		for (int iTeam2 = 0; iTeam2 < MAX_CIV_TEAMS; ++iTeam2)
		{
			if (GET_TEAM((TeamTypes)iTeam2).isVotingMember(eVoteSource))
			{
				if (!kOurTeam.isOpenBordersTrading() && !GET_TEAM((TeamTypes)iTeam2).isOpenBordersTrading())
				{
					return false;
				}

				if (kOurTeam.isAtWar((TeamTypes)iTeam2))
				{
					return false;
				}
			}
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isDefensivePact())
	{
		for (int iTeam2 = 0; iTeam2 < MAX_CIV_TEAMS; ++iTeam2)
		{
			if (GET_TEAM((TeamTypes)iTeam2).isVotingMember(eVoteSource))
			{
				if (!kOurTeam.isDefensivePactTrading() && !GET_TEAM((TeamTypes)iTeam2).isDefensivePactTrading())
				{
					return false;
				}

				if (kOurTeam.getAtWarCount(true) > 0 || GET_TEAM((TeamTypes)iTeam2).getAtWarCount(true) > 0)
				{
					return false;
				}

				if (!kOurTeam.canSignDefensivePact((TeamTypes)iTeam2))
				{
					return false;
				}
			}
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForcePeace())
	{
		FAssert(NO_PLAYER != kData.ePlayer);
		CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);

		if (kPlayer.getTeam() != getTeam())
		{
			if (kOurTeam.isAtWar(kPlayer.getTeam()))
			{
				TeamTypes eMaster = getTeam();
				for (int iMaster = 0; iMaster < MAX_CIV_TEAMS; ++iMaster)
				{
					if (iMaster != getID() && kOurTeam.isVassal((TeamTypes)iMaster))
					{
						if (GET_TEAM((TeamTypes)iMaster).isVotingMember(eVoteSource))
						{
							eMaster = (TeamTypes)iMaster;
							break;
						}
					}
				}

				if (!GET_TEAM(eMaster).canContact(kPlayer.getTeam()))
				{
					return false;
				}
			}
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForceWar())
	{
		FAssert(NO_PLAYER != kData.ePlayer);
		CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);

		if (!kOurTeam.isAtWar(kPlayer.getTeam()))
		{
			TeamTypes eMaster = getTeam();
			for (int iMaster = 0; iMaster < MAX_CIV_TEAMS; ++iMaster)
			{
				if (iMaster != getID() && kOurTeam.isVassal((TeamTypes)iMaster))
				{
					if (GET_TEAM((TeamTypes)iMaster).isVotingMember(eVoteSource))
					{
						eMaster = (TeamTypes)iMaster;
						break;
					}
				}
			}

			if (!GET_TEAM(eMaster).canDeclareWar(kPlayer.getTeam()))
			{
				return false;
			}
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForceNoTrade())
	{
		FAssert(NO_PLAYER != kData.ePlayer);
		CvPlayer& kPlayer = GET_PLAYER(kData.ePlayer);

		if (!canStopTradingWithTeam(kPlayer.getTeam(), true))
		{
			return false;
		}

	}
	else if (GC.getVoteInfo(kData.eVote).isAssignCity())
	{
		if (GET_TEAM(GET_PLAYER(kData.eOtherPlayer).getTeam()).isVassal(GET_PLAYER(kData.ePlayer).getTeam()))
		{
			return false;
		}
	}

	return true;
}

bool CvPlayer::canDefyResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData) const
{
	if (GC.getGameINLINE().getSecretaryGeneral(eVoteSource) == getTeam())
	{
		return false;
	}

	if (GC.getVoteInfo(kData.eVote).isOpenBorders())
	{
		for (int iTeam = 0; iTeam < MAX_CIV_TEAMS; ++iTeam)
		{
			CvTeam& kTeam = GET_TEAM((TeamTypes)iTeam);
			if ((TeamTypes)iTeam != getTeam())
			{
				if (kTeam.isVotingMember(eVoteSource))
				{
					if (!kTeam.isOpenBorders(getTeam()))
					{
						return true;
					}
				}
			}
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isDefensivePact())
	{
		for (int iTeam = 0; iTeam < MAX_CIV_TEAMS; ++iTeam)
		{
			CvTeam& kTeam = GET_TEAM((TeamTypes)iTeam);
			if ((TeamTypes)iTeam != getTeam())
			{
				if (kTeam.isVotingMember(eVoteSource))
				{
					if (!kTeam.isDefensivePact(getTeam()))
					{
						return true;
					}
				}
			}
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForceNoTrade())
	{
		return true;
	}
	else if (GC.getVoteInfo(kData.eVote).isForceWar())
	{
		if (!::atWar(getTeam(), GET_PLAYER(kData.ePlayer).getTeam()))
		{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      12/31/08                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
			// Vassals can't defy declarations of war
			if( !GET_TEAM(getTeam()).isAVassal() )
			{
				return true;
			}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isForcePeace())
	{
		if (GET_PLAYER(kData.ePlayer).getTeam() == getTeam())
		{
			return true;
		}

		if (::atWar(getTeam(), GET_PLAYER(kData.ePlayer).getTeam()))
		{
			return true;
		}
	}
	else if (GC.getVoteInfo(kData.eVote).isAssignCity())
	{
		if (kData.ePlayer == getID())
		{
			return true;
		}
	}
	else if (!GC.getGameINLINE().isTeamVote(kData.eVote))
	{
		return true;
	}

	return false;
}


void CvPlayer::setDefiedResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData)
{
	FAssert(canDefyResolution(eVoteSource, kData));

	// cities get unhappiness
	int iLoop;
	for (CvCity* pLoopCity = firstCity(&iLoop); NULL != pLoopCity; pLoopCity = nextCity(&iLoop))
	{
		ReligionTypes eReligion = GC.getGameINLINE().getVoteSourceReligion(eVoteSource);

		if (NO_RELIGION == eReligion || pLoopCity->isHasReligion(eReligion))
		{
			int iAngerLength = pLoopCity->flatDefyResolutionAngerLength();
			if (NO_RELIGION != eReligion && pLoopCity->isHasReligion(eReligion))
			{
				iAngerLength /= std::max(1, pLoopCity->getReligionCount());
			}

			pLoopCity->changeDefyResolutionAngerTimer(iAngerLength);
		}
	}

	setLoyalMember(eVoteSource, false);
}


void CvPlayer::setEndorsedResolution(VoteSourceTypes eVoteSource, const VoteSelectionSubData& kData)
{
	setLoyalMember(eVoteSource, true);
}

bool CvPlayer::isFullMember(VoteSourceTypes eVoteSource) const
{
	if (NO_RELIGION != GC.getGameINLINE().getVoteSourceReligion(eVoteSource))
	{
		if (getStateReligion() != GC.getGameINLINE().getVoteSourceReligion(eVoteSource))
		{
			return false;
		}
	}

	if (NO_CIVIC != GC.getVoteSourceInfo(eVoteSource).getCivic())
	{
		if (!isCivic((CivicTypes)GC.getVoteSourceInfo(eVoteSource).getCivic()))
		{
			return false;
		}
	}

	if (!isLoyalMember(eVoteSource))
	{
		return false;
	}

	return isVotingMember(eVoteSource);
}

bool CvPlayer::isVotingMember(VoteSourceTypes eVoteSource) const
{
	return (getVotes(NO_VOTE, eVoteSource) > 0);
}

PlayerTypes CvPlayer::pickConqueredCityOwner(const CvCity& kCity) const
{
	PlayerTypes eBestPlayer = kCity.getLiberationPlayer(true);

	if (NO_PLAYER != eBestPlayer)
	{
		if (GET_TEAM(getTeam()).isVassal(GET_PLAYER(eBestPlayer).getTeam()))
		{
			return eBestPlayer;
		}
	}

	return getID();
}

bool CvPlayer::canHaveTradeRoutesWith(PlayerTypes ePlayer) const
{
	CvPlayer& kOtherPlayer = GET_PLAYER(ePlayer);

	if (!kOtherPlayer.isAlive())
	{
		return false;
	}

	if (getTeam() == kOtherPlayer.getTeam())
	{
		return true;
	}
	
	// Civ4 Reimagined
	if (!(GET_TEAM(getTeam()).isHasMet(GET_PLAYER(ePlayer).getTeam())))
	{
		return false;
	}

	if (GET_TEAM(getTeam()).isFreeTrade(kOtherPlayer.getTeam()))
	{
		if (GET_TEAM(getTeam()).isVassal(kOtherPlayer.getTeam()))
		{
			return true;
		}

		if (GET_TEAM(kOtherPlayer.getTeam()).isVassal(getTeam()))
		{
			return true;
		}

		// Ottoman UP
		if (!isNoForeignTrade() && (!kOtherPlayer.isNoForeignTrade() || isIgnoreForeignTradeBan()))
		{
			return true;
		}
	}

	return false;
}

bool CvPlayer::canStealTech(PlayerTypes eTarget, TechTypes eTech) const
{
	if (GET_TEAM(GET_PLAYER(eTarget).getTeam()).isHasTech(eTech))
	{
		//if (canResearch(eTech))
		if (canResearch(eTech, false, true)) // K-Mod
		{
			return true;
		}
	}

	return false;
}

bool CvPlayer::canForceCivics(PlayerTypes eTarget, CivicTypes eCivic) const
{
	return (GET_PLAYER(eTarget).canDoCivics(eCivic) && !GET_PLAYER(eTarget).isCivic(eCivic) && isCivic(eCivic));
}

bool CvPlayer::canForceReligion(PlayerTypes eTarget, ReligionTypes eReligion) const
{
	//return (GET_PLAYER(eTarget).canDoReligion(eReligion) && GET_PLAYER(eTarget).getStateReligion() != eReligion && getStateReligion() == eReligion);
	// K-Mod - You shouldn't be able to force a relgion on an irreligious civ.
	return (GET_PLAYER(eTarget).isStateReligion() && GET_PLAYER(eTarget).canDoReligion(eReligion) && GET_PLAYER(eTarget).getStateReligion() != eReligion && getStateReligion() == eReligion);
}

bool CvPlayer::canSpyDestroyUnit(PlayerTypes eTarget, CvUnit& kUnit) const
{
	if (kUnit.getTeam() == getTeam())
	{
		return false;
	}

	if (kUnit.getUnitInfo().getProductionCost() <= 0)
	{
		return false;
	}

	if (!kUnit.plot()->isVisible(getTeam(), false))
	{
		return false;
	}

	return true;
}

bool CvPlayer::canSpyBribeUnit(PlayerTypes eTarget, CvUnit& kUnit) const
{
	if (!canSpyDestroyUnit(eTarget, kUnit))
	{
		return false;
	}

	// Can't buy units when at war
	if (kUnit.isEnemy(getTeam()))
	{
		return false;
	}

	// Can't buy units if they are not in a legal plot
	if (!GET_TEAM(getTeam()).isFriendlyTerritory(GET_PLAYER(eTarget).getTeam()) && !GET_TEAM(getTeam()).isOpenBorders(GET_PLAYER(eTarget).getTeam()))
	{
		return false;
	}

	CLLNode<IDInfo>* pUnitNode = kUnit.plot()->headUnitNode();

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = kUnit.plot()->nextUnitNode(pUnitNode);

		if (NULL != pLoopUnit && pLoopUnit != &kUnit)
		{
			if (pLoopUnit->isEnemy(getTeam()))
			{
				// If we buy the unit, we will be on the same plot as an enemy unit! Not good.
				return false;
			}
		}
	}

	return true;
}

bool CvPlayer::canSpyDestroyBuilding(PlayerTypes eTarget, BuildingTypes eBuilding) const
{
	CvBuildingInfo& kBuilding = GC.getBuildingInfo(eBuilding);
	if (kBuilding.getProductionCost() <= 0)
	{
		return false;
	}
	
	if (::isLimitedWonderClass((BuildingClassTypes)kBuilding.getBuildingClassType()))
	{
		return false;
	}

	return true;
}

bool CvPlayer::canSpyDestroyProject(PlayerTypes eTarget, ProjectTypes eProject) const
{
	CvProjectInfo& kProject = GC.getProjectInfo(eProject);
	if (kProject.getProductionCost() <= 0)
	{
		return false;
	}

	if (GET_TEAM(GET_PLAYER(eTarget).getTeam()).getProjectCount(eProject) <= 0)
	{
		return false;
	}

	if (::isWorldProject(eProject))
	{
		return false;
	}

	if (!kProject.isSpaceship())
	{
		return false;
	}
	else
	{
		VictoryTypes eVicotry = (VictoryTypes)kProject.getVictoryPrereq();
		if (NO_VICTORY != eVicotry)
		{
			// Can't destroy spaceship components if we have already launched
			if (GET_TEAM(GET_PLAYER(eTarget).getTeam()).getVictoryCountdown(eVicotry) >= 0)
			{
				return false;
			}
		}
	}	

	return true;
}

// K-Mod
int CvPlayer::getEspionageGoldQuantity(EspionageMissionTypes eMission, PlayerTypes eTargetPlayer, const CvCity* pCity) const
{
	if (!pCity || pCity->getOwnerINLINE() != eTargetPlayer)
		return 0;

	CvEspionageMissionInfo& kMission = GC.getEspionageMissionInfo(eMission);
	int iGoldStolen = (GET_PLAYER(eTargetPlayer).getGold() * kMission.getStealTreasuryTypes()) / 100;

	iGoldStolen *= 3*pCity->getPopulation();
	iGoldStolen /= std::max(1, pCity->getPopulation() + 2*GET_PLAYER(eTargetPlayer).getAveragePopulation());
	
	// Civ4 Reimagined
	iGoldStolen *= 100 + getLootingModifier();
	iGoldStolen /= 100;

	return std::min(iGoldStolen, GET_PLAYER(eTargetPlayer).getGold());
}

void CvPlayer::forcePeace(PlayerTypes ePlayer)
{
	/* original bts code
	if (!GET_TEAM(getTeam()).isAVassal())
	{
		FAssert(GET_TEAM(getTeam()).canChangeWarPeace(GET_PLAYER(ePlayer).getTeam())); */

	// K-Mod. "canChangeWarPeace" can return false here if the peace team vassalates after the vote is cast.
	if (GET_TEAM(getTeam()).canChangeWarPeace(GET_PLAYER(ePlayer).getTeam()))
	{
	// K-Mod end
		CLinkList<TradeData> playerList;
		CLinkList<TradeData> loopPlayerList;
		TradeData kTradeData;
		setTradeItem(&kTradeData, TRADE_PEACE_TREATY);
		playerList.insertAtEnd(kTradeData);
		loopPlayerList.insertAtEnd(kTradeData);

		GC.getGameINLINE().implementDeal(getID(), ePlayer, &playerList, &loopPlayerList);
	}
}

bool CvPlayer::canSpiesEnterBorders(PlayerTypes ePlayer) const
{
	for (int iMission = 0; iMission < GC.getNumEspionageMissionInfos(); ++iMission)
	{
		if (GC.getEspionageMissionInfo((EspionageMissionTypes)iMission).isNoActiveMissions() && GC.getEspionageMissionInfo((EspionageMissionTypes)iMission).isPassive())
		{
			if (GET_PLAYER(ePlayer).canDoEspionageMission((EspionageMissionTypes)iMission, getID(), NULL, -1, NULL))
			{
				return false;
			}
		}
	}

	return true;
}

int CvPlayer::getReligionPopulation(ReligionTypes eReligion) const
{
	int iPopulation = 0;
	int iLoop;
	for (CvCity* pCity = firstCity(&iLoop); NULL != pCity; pCity = nextCity(&iLoop))
	{
		if (pCity->isHasReligion(eReligion))
		{
			iPopulation += pCity->getPopulation();
		}
	}

	return iPopulation;
}

int CvPlayer::getNewCityProductionValue() const
{
	int iValue = 0;
	for (int iJ = 0; iJ < GC.getNumBuildingClassInfos(); iJ++)
	{
		BuildingTypes eBuilding = ((BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings(iJ)));

		if (NO_BUILDING != eBuilding)
		{
			if (GC.getBuildingInfo(eBuilding).getFreeStartEra() != NO_ERA)
			{
				if (GC.getGameINLINE().getStartEra() >= GC.getBuildingInfo(eBuilding).getFreeStartEra())
				{
					iValue += (100 * getProductionNeeded(eBuilding)) / std::max(1, 100 + getProductionModifier(eBuilding));
				}
			}
		}
	}

	iValue *= 100 + GC.getDefineINT("NEW_CITY_BUILDING_VALUE_MODIFIER");
	iValue /= 100;

	iValue += (GC.getDefineINT("ADVANCED_START_CITY_COST") * GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent()) / 100;

	int iPopulation = GC.getDefineINT("INITIAL_CITY_POPULATION") + GC.getEraInfo(GC.getGameINLINE().getStartEra()).getFreePopulation();
	for (int i = 1; i <= iPopulation; ++i)
	{
		iValue += (getGrowthThreshold(i) * GC.getDefineINT("ADVANCED_START_POPULATION_COST")) / 100;
	}

	return iValue;
}

int CvPlayer::getGrowthThreshold(int iPopulation) const
{
	int iThreshold;

	iThreshold = (GC.getDefineINT("BASE_CITY_GROWTH_THRESHOLD") + (iPopulation * GC.getDefineINT("CITY_GROWTH_MULTIPLIER")));

	iThreshold *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getGrowthPercent();
	iThreshold /= 100;

	iThreshold *= GC.getEraInfo(GC.getGameINLINE().getStartEra()).getGrowthPercent();
	iThreshold /= 100;

	if (!isHuman() && !isBarbarian())
	{
		iThreshold *= GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIGrowthPercent();
		iThreshold /= 100;

		iThreshold *= std::max(0, ((GC.getHandicapInfo(GC.getGameINLINE().getHandicapType()).getAIPerEraModifier() * getCurrentEra()) + 100));
		iThreshold /= 100;
	}

	return std::max(1, iThreshold);
}

void CvPlayer::verifyUnitStacksValid()
{
	int iLoop;
	for(CvUnit* pLoopUnit = firstUnit(&iLoop); pLoopUnit != NULL; pLoopUnit = nextUnit(&iLoop))
	{
		pLoopUnit->verifyStackValid();
	}
}

UnitTypes CvPlayer::getTechFreeUnit(TechTypes eTech) const
{
	UnitClassTypes eUnitClass = (UnitClassTypes) GC.getTechInfo(eTech).getFirstFreeUnitClass();
	if (eUnitClass == NO_UNITCLASS)
	{
		return NO_UNIT;
	}

	UnitTypes eUnit = ((UnitTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(eUnitClass)));
	if (eUnit == NO_UNIT)
	{
		return NO_UNIT;
	}

	if (GC.getUnitInfo(eUnit).getEspionagePoints() > 0 && GC.getGameINLINE().isOption(GAMEOPTION_NO_ESPIONAGE))
	{
		return NO_UNIT;
	}

	return eUnit;
}


void CvPlayer::buildTradeTable(PlayerTypes eOtherPlayer, CLinkList<TradeData>& ourList) const
{
	TradeData item;
	int iLoop;

	//	Put the gold and maps into the table
	setTradeItem(&item, TRADE_GOLD);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Gold per turn
	setTradeItem(&item, TRADE_GOLD_PER_TURN);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Maps
	setTradeItem(&item, TRADE_MAPS, 0);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Vassal
	setTradeItem(&item, TRADE_VASSAL, 0);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Open Borders
	setTradeItem(&item, TRADE_OPEN_BORDERS);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Defensive Pact
	setTradeItem(&item, TRADE_DEFENSIVE_PACT);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	//	Permanent Alliance
	setTradeItem(&item, TRADE_PERMANENT_ALLIANCE);
	if (canTradeItem(eOtherPlayer, item))
	{
		ourList.insertAtEnd(item);
	}

	if (::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam()))
	{
		//	We are at war, allow a peace treaty option
		setTradeItem(&item, TRADE_PEACE_TREATY);
		ourList.insertAtEnd(item);

		//	Capitulation
		setTradeItem(&item, TRADE_SURRENDER, 0);
		if (canTradeItem(eOtherPlayer, item))
		{
			ourList.insertAtEnd(item);
		}
	}

	//	Initial build of the inventory lists and buttons.
	//	Go through all the possible headings
	for (int i = NUM_BASIC_ITEMS; i < NUM_TRADEABLE_HEADINGS; i++)
	{
		bool bFoundItemUs = false;

		//	Build what we need to build for this item
		switch (i)
		{
		case TRADE_TECHNOLOGIES:
			for (int j = 0; j < GC.getNumTechInfos(); j++)
			{
				setTradeItem(&item, TRADE_TECHNOLOGIES, j);
				if (canTradeItem(eOtherPlayer, item))
				{
					bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_RESOURCES:
			for (int j = 0; j < GC.getNumBonusInfos(); j++)
			{
				setTradeItem(&item, TRADE_RESOURCES, j);
				if (canTradeItem(eOtherPlayer, item))
				{
					bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_CITIES:
			for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
			{
				// K-Mod
				if (AI_cityTrade(pLoopCity, eOtherPlayer) != DENIAL_NEVER)
				// K-Mod end
				{
					setTradeItem(&item, TRADE_CITIES, pLoopCity->getID());
					if (canTradeItem(eOtherPlayer, item))
					{
						bFoundItemUs = true;
						ourList.insertAtEnd(item);
					}
				}
			}
			break;

		case TRADE_PEACE:
			if (!isHuman())
			{
				for (int j = 0; j < MAX_CIV_TEAMS; j++)
				{
					if (GET_TEAM((TeamTypes)j).isAlive())
					{
						if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
						{
							setTradeItem(&item, TRADE_PEACE, j);
							if (canTradeItem(eOtherPlayer, item))
							{
								ourList.insertAtEnd(item);
								bFoundItemUs = true;
							}
						}
					}
				}
			}
			break;

		case TRADE_WAR:
			if (!isHuman())
			{
				for (int j = 0; j < MAX_CIV_TEAMS; j++)
				{
					if (GET_TEAM((TeamTypes)j).isAlive())
					{
						if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
						{
							setTradeItem(&item, TRADE_WAR, j);
							if (canTradeItem(eOtherPlayer, item))
							{
								ourList.insertAtEnd(item);
								bFoundItemUs = true;
							}
						}
					}
				}
			}
			break;

		case TRADE_EMBARGO:
			if (!isHuman())
			{
				for (int j = 0; j < MAX_CIV_TEAMS; j++)
				{
					if (GET_TEAM((TeamTypes)j).isAlive())
					{
						if (j != getTeam() && j != GET_PLAYER(eOtherPlayer).getTeam())
						{
							setTradeItem(&item, TRADE_EMBARGO, j);
							if (canTradeItem(eOtherPlayer, item))
							{
								ourList.insertAtEnd(item);
								bFoundItemUs = true;
							}
						}
					}
				}
			}
			break;

		case TRADE_CIVIC:
			for (int j = 0; j < GC.getNumCivicInfos(); j++)
			{
				setTradeItem(&item, TRADE_CIVIC, j);
				if (canTradeItem(eOtherPlayer, item))
				{
					bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;

		case TRADE_RELIGION:
			for (int j = 0; j < GC.getNumReligionInfos(); j++)
			{
				setTradeItem(&item, TRADE_RELIGION, j);
				if (canTradeItem(eOtherPlayer, item))
				{
					bFoundItemUs = true;
					ourList.insertAtEnd(item);
				}
			}
			break;
		}
	}
}

bool CvPlayer::getHeadingTradeString(PlayerTypes eOtherPlayer, TradeableItems eItem, CvWString& szString, CvString& szIcon) const
{
	szIcon.clear();

	switch ( eItem )
	{
	case TRADE_TECHNOLOGIES:
		szString = gDLL->getText("TXT_KEY_CONCEPT_TECHNOLOGY");
		break;

	case TRADE_RESOURCES:
		szString = gDLL->getText("TXT_KEY_TRADE_RESOURCES");
		break;

	case TRADE_CITIES:
		szString = gDLL->getText("TXT_KEY_TRADE_CITIES");
		break;

	case TRADE_PEACE:
		szString = gDLL->getText("TXT_KEY_TRADE_MAKE_PEACE_WITH");
		break;

	case TRADE_WAR:
		szString = gDLL->getText("TXT_KEY_TRADE_DECLARE_WAR_ON");
		break;

	case TRADE_EMBARGO:
		szString = gDLL->getText("TXT_KEY_TRADE_STOP_TRADING_WITH");
		break;

	case TRADE_CIVIC:
		szString = gDLL->getText("TXT_KEY_TRADE_ADOPT");
		break;

	case TRADE_RELIGION:
		szString = gDLL->getText("TXT_KEY_TRADE_CONVERT");
		break;
	default:
		szString.clear();
		return false;
		break;
	}

	return true;
}


bool CvPlayer::getItemTradeString(PlayerTypes eOtherPlayer, bool bOffer, bool bShowingCurrent, const TradeData& zTradeData, CvWString& szString, CvString& szIcon) const
{
	szIcon.clear();

	switch (zTradeData.m_eItemType)
	{
	case TRADE_GOLD:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_NUM", zTradeData.m_iData);
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_NUM", AI_maxGoldTrade(eOtherPlayer));
		}
		break;
	case TRADE_GOLD_PER_TURN:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_PER_TURN_NUM", zTradeData.m_iData);
		}
		else
		{
			szString = gDLL->getText("TXT_KEY_TRADE_GOLD_PER_TURN_NUM", AI_maxGoldPerTurnTrade(eOtherPlayer));
		}
		break;
	case TRADE_MAPS:
		szString = gDLL->getText("TXT_KEY_TRADE_WORLD_MAP_STRING");
		break;
	case TRADE_VASSAL:
		szString = gDLL->getText("TXT_KEY_TRADE_VASSAL_STRING");
		break;
	case TRADE_SURRENDER:
		szString = gDLL->getText("TXT_KEY_TRADE_CAPITULATE_STRING");
		break;
	case TRADE_OPEN_BORDERS:
		szString = gDLL->getText("TXT_KEY_TRADE_OPEN_BORDERS_STRING");
		break;
	case TRADE_DEFENSIVE_PACT:
		szString = gDLL->getText("TXT_KEY_TRADE_DEFENSIVE_PACT_STRING");
		break;
	case TRADE_PERMANENT_ALLIANCE:
		szString = gDLL->getText("TXT_KEY_TRADE_PERMANENT_ALLIANCE_STRING");
		break;
	case TRADE_PEACE_TREATY:
		szString = gDLL->getText("TXT_KEY_TRADE_PEACE_TREATY_STRING", GC.getGameINLINE().getPeaceDealLength()); // Civ4 Reimagined: Added gamespeed modifier
		break;
	case TRADE_TECHNOLOGIES:
		szString = GC.getTechInfo((TechTypes)zTradeData.m_iData).getDescription();
		szIcon = GC.getTechInfo((TechTypes)zTradeData.m_iData).getButton();
		break;
	case TRADE_RESOURCES:
		if (bOffer)
		{	
			int iNumResources = GET_PLAYER(eOtherPlayer).getNumTradeableBonuses((BonusTypes)zTradeData.m_iData) + getBonusImport((BonusTypes)zTradeData.m_iData); // Civ4 Reimagined
			if (bShowingCurrent)
			{
				++iNumResources;
			}
			szString = gDLL->getText("TXT_KEY_TRADE_RESOURCE", GC.getBonusInfo((BonusTypes)zTradeData.m_iData).getDescription(), iNumResources);

		}
		else
		{
			// Civ4 Reimagined
			szString.Format( L"%s (%d)", GC.getBonusInfo((BonusTypes)zTradeData.m_iData).getDescription(), getNumTradeableBonuses((BonusTypes)zTradeData.m_iData)+getBonusImport((BonusTypes)zTradeData.m_iData));
		}
		szIcon = GC.getBonusInfo((BonusTypes)zTradeData.m_iData).getButton();
		break;
	case TRADE_CITIES:
		{
			CvCity* pCity = NULL;
			if (bOffer)
			{
				pCity = GET_PLAYER(eOtherPlayer).getCity(zTradeData.m_iData);
			}
			else
			{
				pCity = getCity(zTradeData.m_iData);
			}
			if (NULL != pCity)
			{
				if (pCity->getLiberationPlayer(false) == eOtherPlayer)
				{
					szString.Format(L"%s (%s)", pCity->getName().GetCString(), gDLL->getText("TXT_KEY_LIBERATE_CITY").GetCString());
				}
				else
				{
					szString = gDLL->getText("TXT_KEY_CITY_OF", pCity->getNameKey());
				}
			}
		}
		break;
	case TRADE_PEACE:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_PEACE_WITH");
			szString += GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		else
		{
			szString = GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		break;
	case TRADE_WAR:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_WAR_WITH");
			szString += GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		else
		{
			szString = GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		break;
	case TRADE_EMBARGO:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_STOP_TRADING_WITH");
			szString += L" " + GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		else
		{
			szString = GET_TEAM((TeamTypes)zTradeData.m_iData).getName();
		}
		break;
	case TRADE_CIVIC:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_ADOPT");
			szString += GC.getCivicInfo((CivicTypes)zTradeData.m_iData).getDescription();
		}
		else
		{
			szString = GC.getCivicInfo((CivicTypes)zTradeData.m_iData).getDescription();
		}
		szIcon = GC.getCivicInfo((CivicTypes)zTradeData.m_iData).getButton();
		break;
	case TRADE_RELIGION:
		if (bOffer)
		{
			szString = gDLL->getText("TXT_KEY_TRADE_CONVERT");
			szString += GC.getReligionInfo((ReligionTypes)zTradeData.m_iData).getDescription();
		}
		else
		{
			szString = GC.getReligionInfo((ReligionTypes)zTradeData.m_iData).getDescription();
		}
		szIcon = GC.getReligionInfo((ReligionTypes)zTradeData.m_iData).getButton();
		break;
	default:
		szString.clear();
		return false;
	}

	return true;
}

void CvPlayer::updateTradeList(PlayerTypes eOtherPlayer, CLinkList<TradeData>& ourInventory, const CLinkList<TradeData>& ourOffer, const CLinkList<TradeData>& theirOffer) const
{
	for (CLLNode<TradeData>* pNode = ourInventory.head(); pNode != NULL; pNode = ourInventory.next(pNode))
	{
		pNode->m_data.m_bHidden = false;

		// Don't show peace treaties when not at war
		if (!::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam()))
		{
			if (pNode->m_data.m_eItemType == TRADE_PEACE_TREATY || pNode->m_data.m_eItemType == TRADE_SURRENDER)
			{
				pNode->m_data.m_bHidden = true;
			}
		}

		// Don't show technologies with no tech trading game option 
		if (GC.getGame().isOption(GAMEOPTION_NO_TECH_TRADING) && pNode->m_data.m_eItemType == TRADE_TECHNOLOGIES)
		{
			pNode->m_data.m_bHidden = true;
		}
	}

	for (CLLNode<TradeData>* pNode = ourInventory.head(); pNode != NULL; pNode = ourInventory.next(pNode))
	{
		switch (pNode->m_data.m_eItemType)
		{
		case TRADE_PEACE_TREATY:
			for (CLLNode<TradeData>* pOfferNode = ourOffer.head(); pOfferNode != NULL; pOfferNode = ourOffer.next(pOfferNode))
			{
				// Don't show vassal deals if peace treaty is already on the table
				if (CvDeal::isVassal(pOfferNode->m_data.m_eItemType))
				{
					pNode->m_data.m_bHidden = true;
					break;
				}
			}
			break;
		case TRADE_VASSAL:
		case TRADE_SURRENDER:
			for (CLLNode<TradeData>* pOfferNode = theirOffer.head(); pOfferNode != NULL; pOfferNode = theirOffer.next(pOfferNode))
			{
				// Don't show vassal deals if another type of vassal deal is on the table
				if (CvDeal::isVassal(pOfferNode->m_data.m_eItemType))
				{
					pNode->m_data.m_bHidden = true;
					break;
				}
			}

			if (!pNode->m_data.m_bHidden)
			{
				for (CLLNode<TradeData>* pOfferNode = ourOffer.head(); pOfferNode != NULL; pOfferNode = ourOffer.next(pOfferNode))
				{
					// Don't show peace deals if the other player is offering to be a vassal
					if (CvDeal::isEndWar(pOfferNode->m_data.m_eItemType))
					{
						pNode->m_data.m_bHidden = true;
						break;
					}
				}
			}
			break;
		default:
			break;
		}
	}

	if (!isHuman() || !GET_PLAYER(eOtherPlayer).isHuman())  // everything allowed in human-human trades
	{
		CLLNode<TradeData>* pFirstOffer = ourOffer.head();
		if (pFirstOffer == NULL)
		{
			pFirstOffer = theirOffer.head();
		}

		if (pFirstOffer != NULL)
		{
			//if (!CvDeal::isEndWar(pFirstOffer->m_data.m_eItemType) || !::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam()))
			if (!::atWar(getTeam(), GET_PLAYER(eOtherPlayer).getTeam())) // K-Mod
			{
				for (CLLNode<TradeData>* pNode = ourInventory.head(); pNode != NULL; pNode = ourInventory.next(pNode))
				{
					if (pFirstOffer->m_data.m_eItemType == TRADE_CITIES || pNode->m_data.m_eItemType == TRADE_CITIES)
					{
						pNode->m_data.m_bHidden = true;
					}
					else if (CvDeal::isAnnual(pFirstOffer->m_data.m_eItemType) != CvDeal::isAnnual(pNode->m_data.m_eItemType))
					{
						pNode->m_data.m_bHidden = true;
					}
				}
			}
		}
	}
}

// K-Mod. Find each item from the offer list in the inventory list - mark it as m_bOffering == true.
// (This is usually done somewhere in the game engine, or when the offer list is being generated or something.)
void CvPlayer::markTradeOffers(CLinkList<TradeData>& ourInventory, const CLinkList<TradeData>& ourOffer) const
{
	for (CLLNode<TradeData>* pOfferNode = ourOffer.head(); pOfferNode != NULL; pOfferNode = ourOffer.next(pOfferNode))
	{
		CLLNode<TradeData>* pInvNode; // (defined here just for the assertion at the end)
		for (pInvNode = ourInventory.head(); pInvNode != NULL; pInvNode = ourInventory.next(pInvNode))
		{
			if (pInvNode->m_data.m_eItemType == pOfferNode->m_data.m_eItemType &&
				pInvNode->m_data.m_iData == pOfferNode->m_data.m_iData)
			{
				pInvNode->m_data.m_bOffering = pOfferNode->m_data.m_bOffering = true;
				break;
			}
		}
		FAssertMsg(pInvNode, "failed to find offered item in inventory");
	}
}
// K-Mod end

int CvPlayer::getIntroMusicScriptId(PlayerTypes eForPlayer) const
{
	CvPlayer& kForPlayer = GET_PLAYER(eForPlayer);
	EraTypes eEra = kForPlayer.getCurrentEra();
	CvLeaderHeadInfo& kLeader = GC.getLeaderHeadInfo(getLeaderType());
	if (GET_TEAM(kForPlayer.getTeam()).isAtWar(getTeam()))
	{
		return kLeader.getDiploWarIntroMusicScriptIds(eEra);
	}
	else
	{
		return kLeader.getDiploPeaceIntroMusicScriptIds(eEra);
	}
}

int CvPlayer::getMusicScriptId(PlayerTypes eForPlayer) const
{
	CvPlayer& kForPlayer = GET_PLAYER(eForPlayer);
	EraTypes eEra = kForPlayer.getCurrentEra();
	CvLeaderHeadInfo& kLeader = GC.getLeaderHeadInfo(getLeaderType());
	if (GET_TEAM(kForPlayer.getTeam()).isAtWar(getTeam()))
	{
		return kLeader.getDiploWarMusicScriptIds(eEra);
	}
	else
	{
		return kLeader.getDiploPeaceMusicScriptIds(eEra);
	}
}


void CvPlayer::getGlobeLayerColors(GlobeLayerTypes eGlobeLayerType, int iOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	switch (eGlobeLayerType)
	{
	case GLOBE_LAYER_TRADE:
		getTradeLayerColors(aColors, aIndicators);
		break;
	case GLOBE_LAYER_UNIT:
		getUnitLayerColors((GlobeLayerUnitOptionTypes) iOption, aColors, aIndicators);
		break;
	case GLOBE_LAYER_RESOURCE:
		getResourceLayerColors((GlobeLayerResourceOptionTypes) iOption, aColors, aIndicators);
		break;
	case GLOBE_LAYER_RELIGION:
		getReligionLayerColors((ReligionTypes) iOption, aColors, aIndicators);
		break;
	case GLOBE_LAYER_CULTURE:
		getCultureLayerColors(aColors, aIndicators);
		break;
	default:
		FAssertMsg(false, "Unknown globe layer type");
		break;
	}
}

void CvPlayer::getTradeLayerColors(std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	aColors.resize(GC.getMapINLINE().numPlotsINLINE(), NiColorA(0, 0, 0, 0));
	aIndicators.clear();

	typedef std::map< int, std::vector<int> > PlotGroupMap;
	PlotGroupMap mapPlotGroups;
	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); ++iI)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		CvPlotGroup* pPlotGroup = pLoopPlot->getPlotGroup(getID());
		if (pPlotGroup != NULL && pLoopPlot->isRevealed(getTeam(), true) && pLoopPlot->getTeam() == getTeam())
		{
			mapPlotGroups[pPlotGroup->getID()].push_back(iI);
		}
	}

	CvRandom kRandom;
	kRandom.init(42);
	for (PlotGroupMap::iterator it = mapPlotGroups.begin(); it != mapPlotGroups.end(); ++it)
	{
		NiColorA kColor(kRandom.getFloat(), kRandom.getFloat(), kRandom.getFloat(), 0.8f);
		std::vector<int>& aPlots = it->second;
		for (size_t i = 0; i < aPlots.size(); ++i)
		{
			aColors[aPlots[i]] = kColor;
		}
	}
}

// Civ4 Reimagined
bool CvPlayer::canExploreSea() const
{
	return m_bCanExploreSea;
}

// Civ4 Reimagined
void CvPlayer::setCanExploreSea(bool bNewValue)
{
	if (m_bCanExploreSea != bNewValue)
	{
		m_bCanExploreSea = bNewValue;
	}
}

void CvPlayer::getUnitLayerColors(GlobeLayerUnitOptionTypes eOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	aColors.resize(GC.getMapINLINE().numPlotsINLINE(), NiColorA(0, 0, 0, 0));
	aIndicators.clear();

	std::vector< std::vector<float> > aafPlayerPlotStrength(MAX_PLAYERS);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GET_PLAYER((PlayerTypes)i).isAlive())
		{
			aafPlayerPlotStrength[i].resize(GC.getMapINLINE().numPlotsINLINE());
		}
	}

	float fMaxPlotStrength = 0.0f;

	// create unit plot indicators...
	// build the trade group texture
	typedef std::map<int, NiColor> GroupMap;
	GroupMap mapColors;

	// Loop through all the players
	CvWStringBuffer szBuffer;
	for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
	{
		if (GET_PLAYER((PlayerTypes)iPlayer).isAlive())
		{
			for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); ++iI)
			{
				CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
				int iNumUnits = pLoopPlot->getNumUnits();
				float fPlotStrength = 0.0f;

				if (iNumUnits > 0 && pLoopPlot->isVisible(getTeam(), true))
				{
					bool bShowIndicator = false;

					CLLNode<IDInfo>* pUnitNode = pLoopPlot->headUnitNode();
					while (pUnitNode != NULL)
					{
						CvUnit* pUnit = ::getUnit(pUnitNode->m_data);
						pUnitNode = pLoopPlot->nextUnitNode(pUnitNode);

						if (pUnit->getVisualOwner() == iPlayer && !pUnit->isInvisible(getTeam(), true))
						{
							// now, is this unit of interest?
							bool bIsMilitary = pUnit->baseCombatStr() > 0;
							bool bIsEnemy = pUnit->isEnemy(getTeam());
							bool bIsOnOurTeam = pUnit->getTeam() == getTeam();
							bool bOfInterest = false;

							switch (eOption)
							{
							case SHOW_ALL_MILITARY:
								{
									bOfInterest = bIsMilitary;
									if (bOfInterest)
									{
										fPlotStrength += ((float) pUnit->currHitPoints() / (float) pUnit->maxHitPoints() * (float) pUnit->baseCombatStr());
									}
									break;
								}
							case SHOW_TEAM_MILITARY:
								{
									bOfInterest = bIsMilitary && bIsOnOurTeam;
									if (bOfInterest)
										fPlotStrength += ((float) pUnit->currHitPoints() / (float) pUnit->maxHitPoints() * (float) pUnit->baseCombatStr());
									break;
								}
							case SHOW_ENEMIES:
								{
									bOfInterest = bIsMilitary && bIsEnemy;
									if (bOfInterest)
										fPlotStrength += ((float) pUnit->currHitPoints() / (float) pUnit->maxHitPoints() * (float) pUnit->baseCombatStr());
									break;
								}

							case SHOW_ENEMIES_IN_TERRITORY:
								{
									bOfInterest = bIsMilitary;
									break;
								}

							case SHOW_PLAYER_DOMESTICS:
								{
									bOfInterest = !bIsMilitary;// && (pUnit->getVisualOwner() == eCurPlayer);
									break;
								}
							default:
								bOfInterest = false; 
								break;
							}

							// create the indicator
							if (bOfInterest)
							{
								bShowIndicator = true;
							}

							fMaxPlotStrength = std::max(fPlotStrength, fMaxPlotStrength);
							aafPlayerPlotStrength[iPlayer][iI] = fPlotStrength;
						}
					}

					if (bShowIndicator)
					{
						CvUnit* pUnit = pLoopPlot->getBestDefender(NO_PLAYER);
						if (pUnit != NULL)
						{
							PlayerColorTypes eUnitColor = GET_PLAYER(pUnit->getVisualOwner()).getPlayerColor();
							const NiColorA& kColor = GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(eUnitColor).getColorTypePrimary()).getColor();

							szBuffer.clear();
							GAMETEXT.setPlotListHelp(szBuffer, pLoopPlot, true, true);

							CvPlotIndicatorData kIndicator;
							kIndicator.m_pUnit = pUnit;
							kIndicator.m_strLabel = "UNITS";
							kIndicator.m_strIcon = pUnit->getButton();

							if (eOption == SHOW_ENEMIES_IN_TERRITORY)
							{
								kIndicator.m_kColor.r = 1;
								kIndicator.m_kColor.r = 0;
								kIndicator.m_kColor.r = 0;
							}
							else
							{
								kIndicator.m_kColor.r = kColor.r;
								kIndicator.m_kColor.g = kColor.g;
								kIndicator.m_kColor.b = kColor.b;
							}
							kIndicator.m_strHelpText = szBuffer.getCString();

							//setup visibility
							switch (eOption)
							{
							case SHOW_ENEMIES_IN_TERRITORY:
								kIndicator.m_bTestEnemyVisibility = true;
								kIndicator.m_eVisibility = PLOT_INDICATOR_VISIBLE_ALWAYS;
								break;
							case SHOW_ENEMIES:
								kIndicator.m_eVisibility = PLOT_INDICATOR_VISIBLE_ALWAYS;
								break;
							default:
								kIndicator.m_eVisibility = PLOT_INDICATOR_VISIBLE_ONSCREEN_ONLY;
								break;
							}
							aIndicators.push_back(kIndicator);
						}
					}
				}
			}
		}
	}

	if (fMaxPlotStrength > 0)
	{
		for (int iPlayer = 0; iPlayer < MAX_PLAYERS; iPlayer++)
		{
			if (GET_PLAYER((PlayerTypes)iPlayer).isAlive())
			{
				PlayerColorTypes eCurPlayerColor = GET_PLAYER((PlayerTypes) iPlayer).getPlayerColor();
				const NiColorA& kColor = GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(eCurPlayerColor).getColorTypePrimary()).getColor();

				for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
				{
					CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
					if (pLoopPlot->isVisible(getTeam(), true))
					{
						float fPlotStrength = aafPlayerPlotStrength[iPlayer][iI];
						if (fPlotStrength > 0)
						{
							float fAlpha = (fPlotStrength / fMaxPlotStrength * 0.75f + 0.25f) * 0.8f;
							if (fAlpha > aColors[iI].a)
							{
								aColors[iI] = kColor;
								aColors[iI].a = fAlpha;
							}
						}
					}
				}
			}
		}
	}
}

void CvPlayer::getResourceLayerColors(GlobeLayerResourceOptionTypes eOption, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	aColors.clear();
	aIndicators.clear();

	PlayerColorTypes ePlayerColor = getPlayerColor();

	CvWStringBuffer szBuffer;
	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		PlayerTypes eOwner = pLoopPlot->getRevealedOwner(getTeam(), true); 

		if (pLoopPlot->isRevealed(getTeam(), true))
		{
			BonusTypes eCurType = pLoopPlot->getBonusType((GC.getGame().isDebugMode()) ? NO_TEAM : getTeam());
			if (eCurType != NO_BONUS)
			{
				CvBonusInfo& kBonusInfo = GC.getBonusInfo(eCurType);

				bool bOfInterest = false;
				switch (eOption)
				{
				case SHOW_ALL_RESOURCES:
					bOfInterest = true;
					break;
				case SHOW_STRATEGIC_RESOURCES:
					bOfInterest = (kBonusInfo.getHappiness() == 0) && (kBonusInfo.getHealth() == 0);
					break;
				case SHOW_HAPPY_RESOURCES:
					bOfInterest = (kBonusInfo.getHappiness() != 0 ) && (kBonusInfo.getHealth() == 0);
					break;
				case SHOW_HEALTH_RESOURCES:
					bOfInterest = (kBonusInfo.getHappiness() == 0) && (kBonusInfo.getHealth() != 0);
					break;

				}

				if (bOfInterest)
				{
					CvPlotIndicatorData kData;
					kData.m_strLabel = "RESOURCES";
					kData.m_eVisibility = PLOT_INDICATOR_VISIBLE_ONSCREEN_ONLY;
					kData.m_strIcon = GC.getBonusInfo(eCurType).getButton();

					int x = pLoopPlot->getX();
					int y = pLoopPlot->getY();
					kData.m_Target = NiPoint2(GC.getMapINLINE().plotXToPointX(x), GC.getMapINLINE().plotYToPointY(y));

					if (eOwner == NO_PLAYER)
					{
						kData.m_kColor.r = 0.8f;
						kData.m_kColor.g = 0.8f;
						kData.m_kColor.b = 0.8f;
					}
					else
					{
						PlayerColorTypes eCurPlayerColor = GET_PLAYER(eOwner).getPlayerColor();
						const NiColorA& kColor = GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(eCurPlayerColor).getColorTypePrimary()).getColor();
						kData.m_kColor.r = kColor.r;
						kData.m_kColor.g = kColor.g;
						kData.m_kColor.b = kColor.b;
					}

					szBuffer.clear();
					GAMETEXT.setBonusHelp(szBuffer, eCurType, false);
					kData.m_strHelpText = szBuffer.getCString();

					aIndicators.push_back(kData);
				}
			}
		}
	}
}

void CvPlayer::getReligionLayerColors(ReligionTypes eSelectedReligion, std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	aColors.resize(GC.getMapINLINE().numPlotsINLINE(), NiColorA(0, 0, 0, 0));
	aIndicators.clear();

	CvRandom kRandom;
	kRandom.init(42 * eSelectedReligion);
	const NiColorA kBaseColor(kRandom.getFloat(), kRandom.getFloat(), kRandom.getFloat(), 1.0f);

	for (int iI = 0; iI  < MAX_PLAYERS; iI ++)
	{
		if (GET_PLAYER((PlayerTypes)iI).isAlive())
		{
			int iLoop;
			for (CvCity* pLoopCity = GET_PLAYER((PlayerTypes)iI).firstCity(&iLoop); pLoopCity != NULL; pLoopCity = GET_PLAYER((PlayerTypes)iI).nextCity(&iLoop))
			{
				if (pLoopCity->isRevealed(getTeam(), true))
				{
					if (pLoopCity->isHasReligion(eSelectedReligion))
					{
						float fAlpha = 0.8f;
						if (!pLoopCity->isHolyCity(eSelectedReligion))
						{
							fAlpha *= 0.5f;
						}

						// loop through the city's plots
						for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
						{
							CvPlot* pLoopPlot = plotCity(pLoopCity->getX(), pLoopCity->getY(), iJ);
							if (pLoopPlot != NULL)
							{
								// visibility query
								if (pLoopPlot->isRevealed(getTeam(), true))
								{
									int iIndex = GC.getMapINLINE().plotNumINLINE(pLoopPlot->getX_INLINE(), pLoopPlot->getY_INLINE());
									if (fAlpha > aColors[iIndex].a)
									{
										aColors[iIndex] = kBaseColor;
										aColors[iIndex].a = fAlpha;
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

void CvPlayer::getCultureLayerColors(std::vector<NiColorA>& aColors, std::vector<CvPlotIndicatorData>& aIndicators) const
{
	const int iColorsPerPlot = 4;
	aColors.resize(GC.getMapINLINE().numPlotsINLINE() * iColorsPerPlot, NiColorA(0, 0, 0, 0));
	aIndicators.clear();

	// find maximum total culture
	int iMaxTotalCulture = INT_MIN;
	int iMinTotalCulture = INT_MAX;
	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		int iTotalCulture = pLoopPlot->countTotalCulture();
		if (iTotalCulture > iMaxTotalCulture)
		{
			iMaxTotalCulture = iTotalCulture;
		}
		if (iTotalCulture < iMinTotalCulture && iTotalCulture > 0)
		{
			iMinTotalCulture = iTotalCulture;
		}
	}
	iMinTotalCulture = 0;

	// find culture percentages
	for (int iI = 0; iI < GC.getMapINLINE().numPlotsINLINE(); iI++)
	{
		CvPlot* pLoopPlot = GC.getMapINLINE().plotByIndexINLINE(iI);
		PlayerTypes eOwner = pLoopPlot->getRevealedOwner(getTeam(), true); 

		// how many people own this plot?
		std::vector < std::pair<int,int> > plot_owners;
		//int iNumNonzeroOwners = 0;
		// K-Mod
		int iTotalCulture = pLoopPlot->countTotalCulture();
		if (iTotalCulture == 0)
			continue;
		// K-Mod end
		for (int iPlayer = 0; iPlayer < MAX_CIV_PLAYERS; iPlayer++)
		{
			if (GET_PLAYER((PlayerTypes)iPlayer).isAlive())
			{
				int iCurCultureAmount = pLoopPlot->getCulture((PlayerTypes)iPlayer);
				//if (iCurCultureAmount != 0)
				if (iCurCultureAmount * 100 / iTotalCulture >= 20) // K-Mod (to reduce visual spam from small amounts of culture)
				{
					//iNumNonzeroOwners ++;
					plot_owners.push_back(std::pair<int,int>(iCurCultureAmount, iPlayer));
				}
			}
		}

		// ensure that it is revealed
		if (!plot_owners.empty() && pLoopPlot->getRevealedOwner(getTeam(), true) != NO_PLAYER)
		{
			for (int i = 0; i < iColorsPerPlot; ++i)
			{
				int iCurOwnerIdx = i % plot_owners.size();
				PlayerTypes eCurOwnerID = (PlayerTypes) plot_owners[iCurOwnerIdx].second;
				int iCurCulture = plot_owners[iCurOwnerIdx].first;
				const NiColorA& kCurColor = GC.getColorInfo((ColorTypes) GC.getPlayerColorInfo(GET_PLAYER(eCurOwnerID).getPlayerColor()).getColorTypePrimary()).getColor();

				// damp the color by the value...
				aColors[iI * iColorsPerPlot + i] = kCurColor;
				float blend_factor = 0.5f * std::min(1.0f, std::max(0.0f, (float)(iCurCulture - iMinTotalCulture) / iMaxTotalCulture));
				aColors[iI * iColorsPerPlot + i].a = std::min(0.8f * blend_factor + 0.5f, 1.0f);
			}
		}
	}
}

void CvPlayer::cheat(bool bCtrl, bool bAlt, bool bShift)
{
	if (gDLL->getChtLvl() > 0)
	{
		GET_TEAM(getTeam()).setHasTech(getCurrentResearch(), true, getID(), true, false);
	}
}

const CvArtInfoUnit* CvPlayer::getUnitArtInfo(UnitTypes eUnit, int iMeshGroup) const
{
	CivilizationTypes eCivilization = getCivilizationType();
	if (eCivilization == NO_CIVILIZATION)
	{
		eCivilization = (CivilizationTypes) GC.getDefineINT("BARBARIAN_CIVILIZATION");
	}
	UnitArtStyleTypes eStyle = (UnitArtStyleTypes) GC.getCivilizationInfo(eCivilization).getUnitArtStyleType();
	EraTypes eEra = getCurrentEra();
	if (eEra == NO_ERA)
	{
		eEra = (EraTypes) 0;
	}
	return GC.getUnitInfo(eUnit).getArtInfo(iMeshGroup, eEra, eStyle);
}

// K-Mod. I've moved the original code to a new function: CvTeam::hasSpaceshipArrived
bool CvPlayer::hasSpaceshipArrived() const
{
	return GET_TEAM(getTeam()).hasSpaceshipArrived();
}

// Leoreth
int CvPlayer::getDomainProductionModifier(DomainTypes eDomainType) const
{
	return m_aiDomainProductionModifiers[(int)eDomainType];
}

// Leoreth
void CvPlayer::changeDomainProductionModifier(DomainTypes eDomainType, int iChange)
{
	m_aiDomainProductionModifiers[(int)eDomainType] += iChange;
}

// Leoreth
int CvPlayer::getDomainExperienceModifier(DomainTypes eDomainType) const
{
	return m_aiDomainExperienceModifiers[(int)eDomainType];
}

// Leoreth
void CvPlayer::changeDomainExperienceModifier(DomainTypes eDomainType, int iChange)
{
	m_aiDomainExperienceModifiers[(int)eDomainType] += iChange;
}

// Civ4 Reimagined
int CvPlayer::getDomainExperienceGainModifier(DomainTypes eDomainType) const
{
	return m_aiDomainExperienceGainModifiers[(int)eDomainType];
}

// Civ4 Reimagined
void CvPlayer::changeDomainExperienceGainModifier(DomainTypes eDomainType, int iChange)
{
	m_aiDomainExperienceGainModifiers[(int)eDomainType] += iChange;
}

// Civ4 Reimagined
int CvPlayer::getUnitUpgradeCostModifier() const
{
	return m_iUnitUpgradeCostModifier;
}

// Civ4 Reimagined
void CvPlayer::changeUnitUpgradeCostModifier(int iChange)
{
	if (iChange != 0)
		m_iUnitUpgradeCostModifier = m_iUnitUpgradeCostModifier + iChange;
}

// Civ4 Reimagined
bool CvPlayer::isNoForeignCultureUnhappiness() const
{
	int iNoForeignCultureUnhappinessCount = m_iNoForeignCultureUnhappinessCount;
	return (iNoForeignCultureUnhappinessCount > 0);
}

// Civ4 Reimagined
void CvPlayer::changeNoForeignCultureUnhappinessCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoForeignCultureUnhappinessCount = m_iNoForeignCultureUnhappinessCount + iChange;
		FAssert(m_iNoForeignCultureUnhappinessCount >= 0);
	}
}

// Civ4 Reimagined
bool CvPlayer::isNoCityResistance() const
{
	int iNoCityResistanceCount = m_iNoCityResistanceCount;
	return (iNoCityResistanceCount > 0);
}

// Civ4 Reimagined
void CvPlayer::changeNoCityResistanceCount(int iChange)
{
	if (iChange != 0)
	{
		m_iNoCityResistanceCount = m_iNoCityResistanceCount + iChange;
		FAssert(m_iNoCityResistanceCount >= 0);
	}
}

// Civ4 Reimagined
int CvPlayer::getProductionPerSurplusHappiness() const
{
	return m_iProductionPerSurplusHappiness;
}

// Civ4 Reimagined
void CvPlayer::changeProductionPerSurplusHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iProductionPerSurplusHappiness = m_iProductionPerSurplusHappiness + iChange;
		FAssert(m_iProductionPerSurplusHappiness >= 0);
		
		updateYield();
	}
}

// Civ4 Reimagined
int CvPlayer::getForeignUnitSupply() const
{
	return m_iForeignUnitSupply;
}

// Civ4 Reimagined
void CvPlayer::changeForeignUnitSupply(int iChange)
{
	if (iChange != 0)
		m_iForeignUnitSupply = m_iForeignUnitSupply + iChange;
}

// Civ4 Reimagined
int CvPlayer::getPillageGainModifier() const
{
	return m_iPillageGainModifier;
}

// Civ4 Reimagined
void CvPlayer::changePillageGainModifier(int iChange)
{
	if (iChange != 0)
		m_iPillageGainModifier = m_iPillageGainModifier + iChange;
}

// Civ4 Reimagined: Quantifiable Resource System
// Each bonus resource can supply X population of your realm
// X corresponds to TechValue/100
int CvPlayer::getTechValue() const
{
	return m_iTechValue;
}

// Civ4 Reimagined: Quantifiable Resource System
void CvPlayer::changeTechValue(int iChange)
{
	if (iChange != 0)
		m_iTechValue = m_iTechValue + iChange;
	if ( gPlayerLogLevel >= 1 )
		logBBAI("Tech value: %d", m_iTechValue);
}

// Civ4 Reimagined: Quantifiable Resource System
int CvPlayer::getRealNumBonusNeededTimes100() const
{
	long lPopulation = getTotalPopulation() * 10000;
	if (getTechValue() == 0)
		return 1000;
	
	return ( lPopulation / getTechValue() );
} 

// Civ4 Reimagined: Quantifiable Resource System
int CvPlayer::getBonusValueTimes100(int iBonusCount) const
{
	if (iBonusCount <= 0)
	{
		return 0;
	}

	return std::min(100, getBonusRatio() * iBonusCount);
}

// Civ4 Reimagined: Quantifiable Resource System
int CvPlayer::calculateBonusRatioModifier() const
{
	int iModifier = getBonusValueModifier();

	if (GC.getGameINLINE().areIdeologiesEnabled())
	{
		iModifier += GC.getGameINLINE().getIdeologyCount(getIdeology()) * getBonusRatioModifierPerIdeologyCiv(getIdeology());
	}

	return iModifier;
}


// Civ4 Reimagined: Quantifiable Resource System
int CvPlayer::getBonusRatio() const
{
	return m_iBonusRatio;
}

// Civ4 Reimagined: Quantifiable Resource System
void CvPlayer::updateBonusRatio(bool bAlwaysUpdate)
{
	if (!bAlwaysUpdate && !m_bUpdateBonusRatio)
	{
		return;
	}
	
	const int iTotalPop = getTotalPopulation();
	bool bChange = false;
	
	if (iTotalPop > 0)
	{
		const int iModifier = 100 + calculateBonusRatioModifier();

		int iValue = getTechValue() / iTotalPop;
		iValue *= iModifier;
		iValue /= 100;
		
		iValue = std::min(100, iValue);
		
		if (iValue != m_iBonusRatio)
		{
			bChange = true;
			m_iBonusRatio = iValue;
		}
	}
	
	if (bChange)
	{
		int iLoop;
		for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->updateResources();

			if (pLoopCity->isCapital())
			{
				pLoopCity->updateCommerce();
			}
		}
	}
	m_bUpdateBonusRatio = true;
}

// Civ4 Reimagined
int CvPlayer::getResearchPerCulture() const
{
	return m_iResearchPerCulture;
}

// Civ4 Reimagined
void CvPlayer::changeResearchPerCulture(int iChange)
{
	if (iChange != 0)
	{
		m_iResearchPerCulture = m_iResearchPerCulture + iChange;
		updateCommerce();
	}
		
}

// Civ4 Reimagined
bool CvPlayer::isWrongCivicBuilding(BuildingTypes eBuilding) const
{
	for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
	{
		if (GC.getBuildingInfo(eBuilding).isPrereqCivic(iI))
		{
			if (!isCivic((CivicTypes)iI))
			{
				return true;
			}
		}
	}

	for (int iI = 0; iI < GC.getNumIdeologyInfos(); iI++)
	{
		if (GC.getBuildingInfo(eBuilding).getPrereqIdeology() == (IdeologyTypes)iI)
		{
			if (getIdeology() != (IdeologyTypes)iI)
			{
				return true;
			}
		}
	}
	
	return false;
}

/************************************************************************************************/
/* Civ4 Reimagined Ideologies                   START                                           */
/************************************************************************************************/

int CvPlayer::getIdeologyInfluence(IdeologyTypes eIdeology) const
{
	FAssertMsg(eIdeology >= 0, "eIdeology is expected to be non-negative (invalid Index)");
	FAssertMsg(eIdeology < NUM_IDEOLOGY_TYPES, "eIdeology is expected to be within maximum bounds (invalid Index)");
	return m_aiIdeologyInfluence[eIdeology];
}

void CvPlayer::changeIdeologyInfluence(IdeologyTypes eIdeology, int iChange)
{
	FAssertMsg(eIdeology >= 0, "eIdeology is expected to be non-negative (invalid Index)");
	FAssertMsg(eIdeology < NUM_IDEOLOGY_TYPES, "eIdeology is expected to be within maximum bounds (invalid Index)");
	if (iChange != 0)
	{
		m_aiIdeologyInfluence[eIdeology] = m_aiIdeologyInfluence[eIdeology] + iChange;
	}	
}

IdeologyTypes CvPlayer::getIdeology() const
{
	return m_eIdeology;
}

void CvPlayer::updateIdeology()
{
	if (isBarbarian())
	{
		return;
	}

	if (!GC.getGameINLINE().areIdeologiesEnabled())
	{
		return;
	}

	IdeologyTypes eBestIdeology = IDEOLOGY_CONSERVATISM;
	int iBestValue = INT_MIN;

	for (int iI = 0; iI < NUM_IDEOLOGY_TYPES; ++iI)
	{
		if (!GET_TEAM(getTeam()).isHasTech((TechTypes)(GC.getIdeologyInfo((IdeologyTypes)iI).getTechPrereq())))
		{
			continue;
		}

		if (getIdeologyInfluence((IdeologyTypes)iI) > iBestValue)
		{
			eBestIdeology = (IdeologyTypes)iI;
			iBestValue = getIdeologyInfluence((IdeologyTypes)iI);
		}
	}

	if (getIdeology() != eBestIdeology)
	{
		m_eIdeology = eBestIdeology;
		GC.getGameINLINE().updateIdeologyCount();
		logBBAI("new ideology: %d", (int)eBestIdeology);

		GC.getGameINLINE().checkWarPeaceEurekas();

		// Civ4 Reimagined: French UP
		CvCity* pCapitalCity = getCapitalCity();
		if (isFrenchRevolution() && pCapitalCity != NULL && eBestIdeology == IDEOLOGY_LIBERALISM)
		{
			const UnitTypes eGeneral = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL");

			if (isGoldenAge())
			{
				pCapitalCity->createGreatPeople(eGeneral, false, false);
				setIsFrenchRevolution(false);
			}
			else if (!isAnarchy())
			{
				pCapitalCity->createGreatPeople(eGeneral, false, false);
				changeGoldenAgeTurns(getGoldenAgeLength() + 1);
				setIsFrenchRevolution(false);
			}

			// otherwise, we wait until anarchy is over
		}
	}
}

BuildingTypes CvPlayer::getUniquePowerBuilding() const
{
	return m_eUniquePowerBuilding;
}

void CvPlayer::setUniquePowerBuilding(BuildingTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_eUniquePowerBuilding = eIndex;
}

PromotionTypes CvPlayer::getUniquePowerPromotion() const
{
	return m_eUniquePowerPromotion;
}

void CvPlayer::setUniquePowerPromotion(PromotionTypes eIndex)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumPromotionInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_eUniquePowerPromotion = eIndex;
}

int CvPlayer::getForeignTradeIdeologyModifier(IdeologyTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumIdeologyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiForeignTradeIdeologyModifier[eIndex];
}

void CvPlayer::changeForeignTradeIdeologyModifier(IdeologyTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumIdeologyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiForeignTradeIdeologyModifier[eIndex] = (m_aiForeignTradeIdeologyModifier[eIndex] + iChange);

		updateTradeRoutes();
	}
}

int CvPlayer::getBonusRatioModifierPerIdeologyCiv(IdeologyTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumIdeologyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiBonusRatioModifierPerIdeologyCiv[eIndex];
}

void CvPlayer::changeBonusRatioModifierPerIdeologyCiv(IdeologyTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumIdeologyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiBonusRatioModifierPerIdeologyCiv[eIndex] = (m_aiBonusRatioModifierPerIdeologyCiv[eIndex] + iChange);

		updateBonusRatio();
	}
}

int CvPlayer::getIdeologyCombatExperienceModifier(IdeologyTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumIdeologyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiIdeologyCombatExperienceModifier[eIndex];
}

void CvPlayer::changeIdeologyCombatExperienceModifier(IdeologyTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumIdeologyInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiIdeologyCombatExperienceModifier[eIndex] = (m_aiIdeologyCombatExperienceModifier[eIndex] + iChange);
	}
}

// Civ4 Reimagined: Modifier to adjust costs for changing this player's civic by espionage
int CvPlayer::getIdeologyChangeCivicModifier(CivicTypes eCivic) const
{
	int iModifier = 100;

	switch(getIdeology()) {
		case IDEOLOGY_CONSERVATISM:
			iModifier += (GC.getCivicInfo(eCivic).getConservative() < 0) ? -GC.getCivicInfo(eCivic).getConservative() * 10 : 0;
			break;

		case IDEOLOGY_LIBERALISM:
			iModifier += (GC.getCivicInfo(eCivic).getLiberal() < 0) ? -GC.getCivicInfo(eCivic).getLiberal() * 10 : 0;
			break;

		case IDEOLOGY_COMMUNISM:
			iModifier += (GC.getCivicInfo(eCivic).getCommunist() < 0) ? -GC.getCivicInfo(eCivic).getCommunist() * 10 : 0;
			break;

		case IDEOLOGY_FASCISM:
			iModifier += (GC.getCivicInfo(eCivic).getFascist() < 0) ? -GC.getCivicInfo(eCivic).getFascist() * 10 : 0;
			break;

		default: break;
	}

	return iModifier;
}


// Civ4 Reimagined
void CvPlayer::checkImprovementEurekas()
{
	if (GC.getDefineINT("EUREKA_TECH_BOOST_PERCENTAGE") <= 0)
	{
		return;
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_POTTERY")))
	{
		if (getImprovementCount((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FARM")) > 0 || getImprovementCount((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FISHING_BOATS")) > 0)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_POTTERY"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_LEATHER_WORKING")))
	{
		if (getImprovementCount((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_CAMP")) > 0)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_LEATHER_WORKING"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_METAL_CASTING")))
	{
		if (getImprovementCount((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_MINE")) > 5)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_METAL_CASTING"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CROP_ROTATION")))
	{
		if (getImprovementCount((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FARM")) > 9)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CROP_ROTATION"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_ENGINEERING")))
	{
		if (getImprovementCount((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_WATERMILL")) > 1)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_ENGINEERING"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_FEUDALISM")))
	{
		if (getImprovementCount((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FORT")) > 1)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_FEUDALISM"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_LIBERALISM")))
	{
		if (getImprovementCount((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_TOWN")) > 9)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_LIBERALISM"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_REPLACEABLE_PARTS")))
	{
		if (getImprovementCount((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_WORKSHOP")) > 3)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_REPLACEABLE_PARTS"), getID(), true);
		}
	}
}

// Civ4 Reimagined
void CvPlayer::checkBuildingEurekas()
{
	if (GC.getDefineINT("EUREKA_TECH_BOOST_PERCENTAGE") <= 0)
	{
		return;
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_PRIESTHOOD")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_OBELISK")) > 1)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_PRIESTHOOD"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MONOTHEISM")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_CLASSICAL_TEMPLE")) > 2)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MONOTHEISM"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_LITERATURE")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_LIBRARY")) > 2)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_LITERATURE"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CONSTRUCTION")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_WALLS")) > 1)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CONSTRUCTION"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MACHINERY")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_FORGE")) > 0)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MACHINERY"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CIVIL_SERVICE")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_COURTHOUSE")) > 3)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_CIVIL_SERVICE"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_EDUCATION")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_ACADEMY")) > 0)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_EDUCATION"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_SCIENTIFIC_METHOD")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_OXFORD_UNIVERSITY")) > 0)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_SCIENTIFIC_METHOD"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_ECONOMICS")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_BANK")) > 5)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_ECONOMICS"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_COMBUSTION")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_FACTORY")) > 0)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_COMBUSTION"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MASS_MEDIA")))
	{
		int iBuildingCount = getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_BROADCAST_TOWER"));
		if (iBuildingCount > 0 && iBuildingCount >= 2*getNumCities()/3)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_MASS_MEDIA"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_ROBOTICS")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_INDUSTRIAL_PARK")) > 3)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_ROBOTICS"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_FUSION")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_NUCLEAR_PLANT")) > 3)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_FUSION"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_GENETICS")))
	{
		if (getBuildingClassCount((BuildingClassTypes)GC.getInfoTypeForString("BUILDINGCLASS_HOSPITAL")) >= getNumCities())
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_GENETICS"), getID(), true);
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_PRINTING_PRESS")))
	{
		int iCount = 0;

		for (int iI = 0; iI < GC.getNumBuildingClassInfos(); iI++)
		{
			BuildingTypes eBuilding = (BuildingTypes)(GC.getCivilizationInfo(getCivilizationType()).getCivilizationBuildings((BuildingClassTypes)iI));
			if (eBuilding != NO_BUILDING && GC.getBuildingInfo(eBuilding).getSpecialBuildingType() == (SpecialBuildingTypes)GC.getInfoTypeForString("SPECIALBUILDING_MONASTERY"))
			{
				iCount += getBuildingClassCount((BuildingClassTypes)iI);
			}
		}

		if (iCount > 5)
		{
			GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_PRINTING_PRESS"), getID(), true);
		}
	}
}


void CvPlayer::checkWarPeaceEurekas()
{
	if (GC.getDefineINT("EUREKA_TECH_BOOST_PERCENTAGE") <= 0)
	{
		return;
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_DIVINE_RIGHT")))
	{
		const ReligionTypes ourStateReligion = getStateReligion();
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (!GET_PLAYER((PlayerTypes)iI).isBarbarian())
			{
				if (GET_TEAM(getTeam()).isAtWar(GET_PLAYER((PlayerTypes)iI).getTeam()))
				{
					ReligionTypes theirStateReligion = GET_PLAYER((PlayerTypes)iI).getStateReligion();

					if (ourStateReligion != NO_RELIGION && theirStateReligion != NO_RELIGION && ourStateReligion != theirStateReligion)
					{
						GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_DIVINE_RIGHT"), getID(), true);
						break;
					}
				}
			}
		}
	}

	if (! GET_TEAM(getTeam()).isTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_FASCISM")))
	{
		const IdeologyTypes ourIdeology = getIdeology();
		for (int iI = 0; iI < MAX_PLAYERS; iI++)
		{
			if (!GET_PLAYER((PlayerTypes)iI).isBarbarian())
			{
				if (GET_TEAM(getTeam()).isAtWar(GET_PLAYER((PlayerTypes)iI).getTeam()))
				{
					IdeologyTypes theirIdeology = GET_PLAYER((PlayerTypes)iI).getIdeology();

					if (ourIdeology != theirIdeology)
					{
						GET_TEAM(getTeam()).setTechBoosted((TechTypes)GC.getInfoTypeForString("TECH_FASCISM"), getID(), true);
						break;
					}
				}
			}
		}
	}
}

/************************************************************************************************/
/* Civ4 Reimagined Ideologies                   END                                           */
/************************************************************************************************/

/************************************************************************************************/
/* Civ4 Reimagined Unique Powers                   START                                                 */
/************************************************************************************************/
	
// Civ4 Reimagined
int CvPlayer::getFreePopulationInCapital() const
{
	return m_iFreePopulationInCapital;
}

// Civ4 Reimagined
void CvPlayer::changeFreePopulationInCapital(int iChange)
{
	if (iChange != 0)
		m_iFreePopulationInCapital = m_iFreePopulationInCapital + iChange;
	
	FAssertMsg(getFreePopulationInCapital() >= 0, "Free Population is not supposed to be negative");
}

// Civ4 Reimagined
int CvPlayer::getAdditionalAncientEurekaBoost() const
{
	return m_iAdditionalAncientEurekaBoost;
}

// Civ4 Reimagined
void CvPlayer::changeAdditionalAncientEurekaBoost(int iChange)
{
	if (iChange != 0)
	{
		m_iAdditionalAncientEurekaBoost += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getAdditionalFarmBonusYield() const
{
	return m_iAdditionalFarmBonusYield;
}

// Civ4 Reimagined
void CvPlayer::changeAdditionalFarmBonusYield(int iChange)
{
	if (iChange != 0)
	{
		m_iAdditionalFarmBonusYield += iChange;

		updateYield();
	}
}

// Civ4 Reimagined
int CvPlayer::getAdditionalMineBonusCommerce() const
{
	return m_iAdditionalMineBonusCommerce;
}

// Civ4 Reimagined
void CvPlayer::changeAdditionalMineBonusCommerce(int iChange)
{
	if (iChange != 0)
	{
		m_iAdditionalMineBonusCommerce += iChange;

		updateYield();
	}
}

// Civ4 Reimagined
int CvPlayer::getPillageHeal() const
{
	return m_iPillageHeal;
}

// Civ4 Reimagined
void CvPlayer::changePillageHeal(int iChange)
{
	if (iChange != 0)
	{
		m_iPillageHeal += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getGreatPeopleRatePerReligionModifier() const
{
	return m_iGreatPeopleRatePerReligionModifier;
}

// Civ4 Reimagined
void CvPlayer::changeGreatPeopleRatePerReligionModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iGreatPeopleRatePerReligionModifier += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getTradeGoldModifierPerForeignResource() const
{
	return m_iTradeGoldModifierPerForeignResource;
}

// Civ4 Reimagined
void CvPlayer::changeTradeGoldModifierPerForeignResource(int iChange)
{
	if (iChange != 0)
	{
		m_iTradeGoldModifierPerForeignResource += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getReligiousVoteModifier() const
{
	return m_iReligiousVoteModifier;
}

// Civ4 Reimagined
void CvPlayer::changeReligiousVoteModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iReligiousVoteModifier += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getGreatEngineerPointsFromCathedrals() const
{
	return m_iGreatEngineerPointsFromCathedrals;
}

// Civ4 Reimagined
void CvPlayer::changeGreatEngineerPointsFromCathedrals(int iChange)
{
	if (iChange != 0)
	{
		m_iGreatEngineerPointsFromCathedrals += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getCombatBonusOnHomeArea() const
{
	return m_iCombatBonusOnHomeArea;
}

// Civ4 Reimagined
void CvPlayer::changeCombatBonusOnHomeArea(int iChange)
{
	if (iChange != 0)
	{
		m_iCombatBonusOnHomeArea += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getStrategicBonusYieldModifier() const
{
	return m_iStrategicBonusYieldModifier;
}

// Civ4 Reimagined
void CvPlayer::changeStrategicBonusYieldModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iStrategicBonusYieldModifier += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getBuildingProductionModifierFromCapital() const
{
	return m_iBuildingProductionModifierFromCapital;
}

// Civ4 Reimagined
void CvPlayer::changeBuildingProductionModifierFromCapital(int iChange)
{
	if (iChange != 0)
	{
		m_iBuildingProductionModifierFromCapital = m_iBuildingProductionModifierFromCapital + iChange;
		FAssert(m_iBuildingProductionModifierFromCapital >= 0);
		
		updateYield();
	}
}

// Civ4 Reimagined
int CvPlayer::getCultureResistanceModifier() const
{
	return m_iCultureResistanceModifier;
}

// Civ4 Reimagined
void CvPlayer::changeCultureResistanceModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iCultureResistanceModifier = m_iCultureResistanceModifier + iChange;
		FAssert(m_iCultureResistanceModifier >= 0);
	}
}

// Civ4 Reimagined
int CvPlayer::getFreshWaterHealthModifier() const
{
	return m_iFreshWaterHealthModifier;
}

// Civ4 Reimagined
void CvPlayer::changeFreshWaterHealthModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iFreshWaterHealthModifier = m_iFreshWaterHealthModifier + iChange;
		FAssert(m_iFreshWaterHealthModifier >= 0);

		int iLoop;
		for (CvCity* pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
		{
			pLoopCity->updateFreshWaterHealth();
		}
	}
}

// Civ4 Reimagined
int CvPlayer::getStateReligionShrineModifier() const
{
	return m_iStateReligionShrineModifier;
}

// Civ4 Reimagined
void CvPlayer::changeStateReligionShrineModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iStateReligionShrineModifier = m_iStateReligionShrineModifier + iChange;

		updateBuildingCommerce();
	}
}

// Civ4 Reimagined
int CvPlayer::getLootCityModifier() const
{
	return m_iLootCityModifier;
}

// Civ4 Reimagined
void CvPlayer::changeLootCityModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iLootCityModifier = m_iLootCityModifier + iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getTechBulbModifier() const
{
	return m_iTechBulbModifier;
}

// Civ4 Reimagined
void CvPlayer::changeTechBulbModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iTechBulbModifier = m_iTechBulbModifier + iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getRouteChange(RouteTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumRouteInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_paiRouteChange[eIndex];
}

// Civ4 Reimagined
void CvPlayer::changeRouteChange(RouteTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumRouteInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	m_paiRouteChange[eIndex] = (m_paiRouteChange[eIndex] + iChange);
}

// Civ4 Reimagined
int CvPlayer::getShrineCommerceIncome(CommerceTypes eIndex, ReligionTypes eReligion) const
{
	const int iCap = GC.getDefineINT("COMMERCE_CAP_FOR_SHRINES_AND_HEADQUARTERS");

	int iShrineCommerce = (GC.getReligionInfo((ReligionTypes)(eReligion)).getGlobalReligionCommerce(eIndex) * ::restrictedGrowth(GC.getGameINLINE().countReligionLevels(eReligion), iCap));

	if (getStateReligion() == eReligion)
	{
		iShrineCommerce *= 100 + getStateReligionShrineModifier();
		iShrineCommerce /= 100;
	}

	return iShrineCommerce;
}

// Civ4 Reimagined
CivicTypes CvPlayer::getFreeCivicEnabled() const
{
	return m_iFreeCivicEnabled;
}

// Civ4 Reimagined
void CvPlayer::setFreeCivicEnabled(CivicTypes eCivic)
{
	m_iFreeCivicEnabled = eCivic;
}

// Civ4 Reimagined
CivicTypes CvPlayer::getLegacyCivic() const
{
	return m_iLegacyCivic;
}

// Civ4 Reimagined
void CvPlayer::setLegacyCivic(CivicTypes eCivic)
{
	m_iLegacyCivic = eCivic;
}

// Civ4 Reimagined
int CvPlayer::getEarlyScientistBonusCommerce() const
{
	return m_iEarlyScientistBonusCommerce;
}

// Civ4 Reimagined
void CvPlayer::changeEarlyScientistBonusCommerce(int iChange)
{
	if (iChange != 0)
		m_iEarlyScientistBonusCommerce = m_iEarlyScientistBonusCommerce + iChange;
}

// Civ4 Reimagined
int CvPlayer::getEarlyPriestExtraFood() const
{
	return m_iEarlyPriestExtraFood;
}

// Civ4 Reimagined
void CvPlayer::changeEarlyPriestExtraFood(int iChange)
{
	if (iChange != 0)
		m_iEarlyPriestExtraFood = m_iEarlyPriestExtraFood + iChange;
}

// Civ4 Reimagined
int CvPlayer::getUniquePowerCommerceModifier(CommerceTypes eIndex) const												
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aiUniquePowerCommerceModifier[eIndex];
}

// Civ4 Reimagined
void CvPlayer::changeUniquePowerCommerceModifier(CommerceTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < NUM_COMMERCE_TYPES, "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aiUniquePowerCommerceModifier[eIndex] = (m_aiUniquePowerCommerceModifier[eIndex] + iChange);
		FAssert(getUniquePowerCommerceModifier(eIndex) >= 0);
		
		changeCommerceRateModifier(eIndex, iChange);
		AI_makeAssignWorkDirty();
	}
}

// Civ4 Reimagined
int CvPlayer::getWorldWonderProductionModifier() const
{
	return m_iWorldWonderProductionModifier;
}

// Civ4 Reimagined
void CvPlayer::changeWorldWonderProductionModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iWorldWonderProductionModifier = m_iWorldWonderProductionModifier + iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getProductionNearRiver() const
{
	return m_iProductionNearRiver;
}

// Civ4 Reimagined
void CvPlayer::changeProductionNearRiver(int iChange)
{
	if (iChange != 0)
	{
		m_iProductionNearRiver = m_iProductionNearRiver + iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getProductionPerPopulation() const
{
	return m_iProductionPerPopulation;
}

// Civ4 Reimagined
void CvPlayer::changeProductionPerPopulation(int iChange)
{
	if (iChange != 0)
	{
		m_iProductionPerPopulation = m_iProductionPerPopulation + iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getHurryWithGreatPriestsRatio() const
{
	return m_iHurryWithGreatPriestsRatio;
}

// Civ4 Reimagined
void CvPlayer::changeHurryWithGreatPriestsRatio(int iChange)
{
	if (iChange != 0)
	{
		m_iHurryWithGreatPriestsRatio = m_iHurryWithGreatPriestsRatio + iChange;
	}
}

// Civ4 Reimagined
void CvPlayer::placeBonusAtCapitalFatCross(BonusTypes eBonus)
{
	CvPlot* pLoopPlot;
	CvPlot* pStartingPlot = getStartingPlot();
	std::set<CvPlot*> validPlots;
	
	if (pStartingPlot == NULL)
	{
		return;
	}
	
	int aiShuffle[NUM_CITY_PLOTS];
	shuffleArray(aiShuffle, NUM_CITY_PLOTS, GC.getGameINLINE().getMapRand());
	
	for (int iPass = 0; iPass < 2; iPass++)
	{
		for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
		{
			pLoopPlot = plotCity(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE(), aiShuffle[iJ]);
			if (pLoopPlot != NULL && pLoopPlot != pStartingPlot)
			{
				if (pLoopPlot->getBonusType() == NO_BONUS)
				{
					if (iPass > 0 || !pLoopPlot->isRiver())
					{
						if (pLoopPlot->canHaveBonus(eBonus, false))
						{
						pLoopPlot->setBonusType(eBonus);
						return;
						}
					}
				}
			}
		}
	}
	
	shuffleArray(aiShuffle, NUM_CITY_PLOTS, GC.getGameINLINE().getMapRand());
	for (int iPass = 0; iPass < 2; iPass++)
	{
		for (int iJ = 0; iJ < NUM_CITY_PLOTS; iJ++)
		{
			CvPlot* pLoopPlot = plotCity(pStartingPlot->getX_INLINE(), pStartingPlot->getY_INLINE(), aiShuffle[iJ]);

			if ((pLoopPlot != NULL) && (pLoopPlot != pStartingPlot) && (pLoopPlot->getBonusType() == NO_BONUS) && (!pLoopPlot->isWater()) && (!pLoopPlot->isImpassable()))
			{
				if (iPass > 0 || !pLoopPlot->isRiver())
				{
					if (gMapLogLevel > 0)
						logBBAI("    Adding %S and terrain for player %d for unique power.", GC.getBonusInfo(eBonus).getDescription(), getID());
					
					if (!(GC.getBonusInfo(eBonus).isFeature(pLoopPlot->getFeatureType())))
					{
						pLoopPlot->setFeatureType(NO_FEATURE);
					}
					
					if (pLoopPlot->getFeatureType() != NO_FEATURE)
					{
						if (!(GC.getBonusInfo(eBonus).isFeatureTerrain(pLoopPlot->getTerrainType())))
						{
							for (int iK = 0; iK < GC.getNumTerrainInfos(); iK++)
							{
								if ((GC.getBonusInfo(eBonus).isFeatureTerrain((TerrainTypes)iK)))
								{
									pLoopPlot->setTerrainType((TerrainTypes)iK);
									break;
								}
							}
						}
					} else if (!(GC.getBonusInfo(eBonus).isTerrain(pLoopPlot->getTerrainType())))
					{
						for (int iK = 0; iK < GC.getNumTerrainInfos(); iK++)
						{
							if ((GC.getBonusInfo(eBonus).isTerrain((TerrainTypes)iK)))
							{
								pLoopPlot->setTerrainType((TerrainTypes)iK);
								break;
							}
						}
					}
					if ((GC.getBonusInfo(eBonus).isHills()) && !pLoopPlot->isHills())
					{
						pLoopPlot->setPlotType(PLOT_HILLS, false, true);
					}
					pLoopPlot->setBonusType(eBonus);
					return;
				}
			}
		}
	}
}

// Civ4 Reimagined
int CvPlayer::getCoastalTradeRouteModifier() const
{
	return m_iCoastalTradeRouteModifier;
}

// Civ4 Reimagined
void CvPlayer::changeCoastalTradeRouteModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iCoastalTradeRouteModifier = m_iCoastalTradeRouteModifier + iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getOverseaTradeRouteModifier() const
{
	return m_iOverseaTradeRouteModifier;
}

// Civ4 Reimagined
void CvPlayer::changeOverseaTradeRouteModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iOverseaTradeRouteModifier = m_iOverseaTradeRouteModifier + iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getEspionagePointsOnConquestPerPopulation() const
{
	return m_iEspionagePointsOnConquestPerPopulation;
}

// Civ4 Reimagined
void CvPlayer::changeEspionagePointsOnConquestPerPopulation(int iChange)
{
	if (iChange != 0)
	{
		m_iEspionagePointsOnConquestPerPopulation = m_iEspionagePointsOnConquestPerPopulation + iChange;
	}
}

// Civ4 Reimagined - Only for interface
int CvPlayer::getUniquePowerGreatPeopleModifier() const
{
	return m_iUniquePowerGreatPeopleModifier;
}

// Civ4 Reimagined
void CvPlayer::changeUniquePowerGreatPeopleModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iUniquePowerGreatPeopleModifier = m_iUniquePowerGreatPeopleModifier + iChange;
		
		changeGreatPeopleRateModifier(iChange);
	}
}

// Civ4 Reimagined
int CvPlayer::getUniqueUnitFreeExperience() const
{
	return m_iUniqueUnitFreeExperience;
}

// Civ4 Reimagined
void CvPlayer::changeUniqueUnitFreeExperience(int iChange)
{
	if (iChange != 0)
	{
		m_iUniqueUnitFreeExperience = m_iUniqueUnitFreeExperience + iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getFreeUnitsOnConquest() const
{
	return m_iFreeUnitsOnConquest;
}

// Civ4 Reimagined
void CvPlayer::changeFreeUnitsOnConquest(int iChange)
{
	if (iChange != 0)
	{
		m_iFreeUnitsOnConquest = m_iFreeUnitsOnConquest + iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getReligionTechModifier() const
{
	return m_iReligionTechModifier;
}

// Civ4 Reimagined
void CvPlayer::changeReligionTechModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iReligionTechModifier = m_iReligionTechModifier + iChange;
	}
}

// Civ4 Reimagined
void CvPlayer::setHasImmigrants(bool bNewValue)
{
	m_bImmigrants = bNewValue;
}

// Civ4 Reimagined
bool CvPlayer::getImmigrants() const
{
	return m_bImmigrants;
}

// Civ4 Reimagined
void CvPlayer::setIsCaptureSlaves(bool bNewValue)
{
	m_bCaptureSlaves = bNewValue;
}

// Civ4 Reimagined
bool CvPlayer::isCaptureSlaves() const
{
	return m_bCaptureSlaves;
}

// Civ4 Reimagined
void CvPlayer::setIsDesertGold(bool bNewValue)
{
	m_bDesertGold = bNewValue;
}

// Civ4 Reimagined
bool CvPlayer::isDesertGold() const
{
	return m_bDesertGold;
}

// Civ4 Reimagined
int CvPlayer::getUniquePowerBuildingModifier() const
{
	return m_iUniquePowerBuildingModifier;
}

// Civ4 Reimagined
void CvPlayer::changeUniquePowerBuildingModifier(int iChange)
{
	m_iUniquePowerBuildingModifier += iChange;
}

// Civ4 Reimagined
int CvPlayer::getEarlyWorkerSpeedModifier() const
{
	return m_iEarlyWorkerSpeedModifier;
}

// Civ4 Reimagined
void CvPlayer::changeEarlyWorkerSpeedModifier(int iChange)
{
	m_iEarlyWorkerSpeedModifier += iChange;
}

// Civ4 Reimagined
int CvPlayer::getFatcrossTerrainHappiness(TerrainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex expected to be < GC.getNumTerrainInfos()");

	return m_paiFatcrossTerrainHappiness[eIndex];
}

// Civ4 Reimagined
void CvPlayer::changeFatcrossTerrainHappiness(TerrainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex expected to be < GC.getNumTerrainInfos()");

	if (iChange != 0)
	{
		m_paiFatcrossTerrainHappiness[eIndex] += iChange;
	}
}

// Civ4 Reimagined: Still missing: Implementation in CvCity & Gametextmanager.
int CvPlayer::getFatcrossTerrainCulture(TerrainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex expected to be < GC.getNumTerrainInfos()");

	return m_paiFatcrossTerrainCulture[eIndex];
}

// Civ4 Reimagined
void CvPlayer::changeFatcrossTerrainCulture(TerrainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex expected to be < GC.getNumTerrainInfos()");

	if (iChange != 0)
	{
		m_paiFatcrossTerrainCulture[eIndex] += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getTerrainMovementCostModifier(TerrainTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex expected to be < GC.getNumTerrainInfos()");

	return m_paiTerrainMovementCostModifier[eIndex];
}

// Civ4 Reimagined
void CvPlayer::changeTerrainMovementCostModifier(TerrainTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex expected to be >= 0");
	FAssertMsg(eIndex < GC.getNumTerrainInfos(), "eIndex expected to be < GC.getNumTerrainInfos()");

	if (iChange != 0)
	{
		m_paiTerrainMovementCostModifier[eIndex] += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getFatcrossPeakCulture() const
{
	return m_iFatcrossPeakCulture;
}

// Civ4 Reimagined
void CvPlayer::changeFatcrossPeakCulture(int iChange)
{
	if (iChange != 0)
	{
		m_iFatcrossPeakCulture += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getFatcrossPeakHappiness() const
{
	return m_iFatcrossPeakHappiness;
}

// Civ4 Reimagined
void CvPlayer::changeFatcrossPeakHappiness(int iChange)
{
	if (iChange != 0)
	{
		m_iFatcrossPeakHappiness += iChange;
	}
}

// Civ4 Reimagined
void CvPlayer::setCapitalCommercePerPopulation(CommerceTypes eIndex, int iCommerceModifierPerPopulation, int iTargetPopulation)
{
	m_paiCapitalCommercePopulationThreshold[eIndex] = iTargetPopulation;
	m_paiCapitalCommerceModifier[eIndex] = iCommerceModifierPerPopulation;
	
	CvCity* pCapitalCity = getCapitalCity();

	if (pCapitalCity != NULL)
	{
		pCapitalCity->updateCommerce();

		pCapitalCity->AI_setAssignWorkDirty(true);
	}
}

// Civ4 Reimagined
int CvPlayer::getCapitalCommercePerPopulation(CommerceTypes eIndex, int iPopulation) const
{
	int iCommerceModifierPerPopulation = m_paiCapitalCommerceModifier[eIndex];

	if (iCommerceModifierPerPopulation > 0)
	{
		int iTargetPopulation = m_paiCapitalCommercePopulationThreshold[eIndex];
		if (iPopulation > iTargetPopulation)
		{
			return iCommerceModifierPerPopulation * (iPopulation - iTargetPopulation);
		}
	}
	
	return 0;
}

// Civ4 Reimagined
// iPopulationAverageEnd: Max effect if population reaches this % of average population
// iCommerceModifierMax: At aveage population, the effect is 0% and scales linearly until it reaches iCommerceModifierMax at iPopulationAverageEnd.
void CvPlayer::setCommerceAboveAveragePopulation(CommerceTypes eIndex, int iPopulationAverageMax, int iCommerceModifierMax)
{
	m_paiAveragePopCommerceModifierMaxPop[eIndex] = iPopulationAverageMax;
	m_paiAveragePopCommerceModifierMaxMod[eIndex] = iCommerceModifierMax;
	
	// Shortcut: Check once if effect has been removed here, instead of once per turn in updateCommerceAboveAveragePopulation
	if (iPopulationAverageMax == 0 || iCommerceModifierMax == 0)
	{
		m_paiAveragePopCommerceModifier[eIndex] = 0;
		updateCommerce(eIndex);
	}
	else
	{
		updateCommerceAboveAveragePopulation();
	}
}

// Civ4 Reimagined
int CvPlayer::getCommerceAboveAveragePopulation(CommerceTypes eIndex) const
{
	return m_paiAveragePopCommerceModifier[eIndex];
}

// Civ4 Reimagined
void CvPlayer::updateCommerceAboveAveragePopulation()
{
	for (int iIndex = 0; iIndex < NUM_COMMERCE_TYPES; iIndex++)
	{
		if (m_paiAveragePopCommerceModifierMaxMod[iIndex] > 0 && m_paiAveragePopCommerceModifierMaxPop[iIndex] > 0)
		{
			int iOldCommerce = m_paiAveragePopCommerceModifier[iIndex];
			int iOwnPop = getTotalPopulation();

			// Don't include self in comparison to make effect more consistent with low number of players
			int iOthersTotalPop = GC.getGameINLINE().getTotalPopulation() - iOwnPop;
			int iOthersNumCivs = GC.getGameINLINE().countCivPlayersEverAlive() - 1;
			double dAveragePopOthers = std::max(1,iOthersTotalPop) * 1.0 / std::max(1,iOthersNumCivs);
	
			double dPopDifferencePercent = (iOwnPop * 100 / dAveragePopOthers) - 100; // e.g. 13 pop own vs 10 average => 30%
			if (dPopDifferencePercent > 0)
			{
				int iPopDifferencePercentForMaxEffect = m_paiAveragePopCommerceModifierMaxPop[iIndex] - 100;
				int iMaxEffect = m_paiAveragePopCommerceModifierMaxMod[iIndex];

				// Scale down if required pop difference not reached. Example: 140% is required for max effect, but population is only 130% of average => 75% effective
				int iResult = static_cast<int>(iMaxEffect * (dPopDifferencePercent / std::max(1,iPopDifferencePercentForMaxEffect)));

				// If population is above maximum required, still only add iMaxEffect
				m_paiAveragePopCommerceModifier[iIndex] = std::min(iResult, iMaxEffect);
			}
			else
			{
				m_paiAveragePopCommerceModifier[iIndex] = 0;
			}

			// Update all citie?s
			if (iOldCommerce != m_paiAveragePopCommerceModifier[iIndex])
			{
				updateCommerce((CommerceTypes)iIndex);
			}
		}
	}
}

// Civ4 Reimagined
int CvPlayer::getNonStateReligionHappinessWithStateReligion() const
{
	return m_iNonStateReligionHappinessWithStateReligion;
}

// Civ4 Reimagined
void CvPlayer::changeNonStateReligionHappinessWithStateReligion(int iChange)
{
	if (iChange != 0)
	{
		m_iNonStateReligionHappinessWithStateReligion += iChange;
	}
}

// Civ4 Reimagined
bool CvPlayer::isExploreRivalSea() const
{
	return m_bExploreRivalSea;
}

// Civ4 Reimagined
void CvPlayer::setExploreRivalSea(bool bNewValue)
{
	m_bExploreRivalSea = bNewValue;
}

// Civ4 Reimagined
bool CvPlayer::isEnableFinancial() const
{
	return m_bEnableFinancial;
}

// Civ4 Reimagined
void CvPlayer::setEnableFinancial(bool bNewValue)
{
	m_bEnableFinancial = bNewValue;
	updateExtraYieldThreshold(YIELD_COMMERCE);
}

// Civ4 Reimagined
int CvPlayer::getMercenaryCostModifier() const
{
	return m_iMercenaryCostModifier;
}

// Civ4 Reimagined
void CvPlayer::changeMercenaryCostModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iMercenaryCostModifier += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getBuyBuildingCostModifier() const
{
	return m_iBuyBuildingCostModifier;
}

// Civ4 Reimagined
void CvPlayer::changeBuyBuildingCostModifier(int iChange)
{
	if (iChange != 0)
	{
		m_iBuyBuildingCostModifier += iChange;
	}
}

// Civ4 Reimagined
int CvPlayer::getCanFarmHillsCount() const
{
	return m_iCanFarmHillsCount;
}

// Civ4 Reimagined
void CvPlayer::changeCanFarmHillsCount(int iChange)
{
	if (iChange != 0)
	{
		m_iCanFarmHillsCount += iChange;
	}
}

// Civ4 Reimagined
bool CvPlayer::isCanFarmHills() const
{
	return (getCanFarmHillsCount() > 0);
}

// Civ4 Reimagined
bool CvPlayer::isSpecialTradeRoutePerPlayer() const
{
	return m_bSpecialTradeRoutePerPlayer;
}

// Civ4 Reimagined
void CvPlayer::setSpecialTradeRoutePerPlayer(bool bNewValue)
{
	m_bSpecialTradeRoutePerPlayer = bNewValue;
	updateTradeRoutes();
}

// Civ4 Reimagined
int CvPlayer::getPlayerExtraAvailableBonuses(BonusTypes eBonus) const
{
	FAssertMsg(eBonus >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eBonus < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	
	return m_paiPlayerExtraAvailableBonuses[eBonus];
}

// Civ4 Reimagined
void CvPlayer::changePlayerExtraAvailableBonuses(BonusTypes eBonus, int iChange)
{
	FAssertMsg(eBonus >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eBonus < GC.getNumBonusInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	
	if (iChange != 0)
	{
		m_paiPlayerExtraAvailableBonuses[eBonus] += iChange;
	}
}

// Civ4 Reimagined
bool CvPlayer::isHasExtraAvailableBonuses() const
{
	return m_bExtraAvailableBonuses;
}

// Civ4 Reimagined
void CvPlayer::setHasExtraAvailableBonuses(bool bNewValue)
{
	m_bExtraAvailableBonuses = bNewValue;
}

// Civ4 Reimagined
int CvPlayer::getCulturePerPopulationSacrified() const
{
	return m_iCulturePerPopulationSacrified;
}

// Civ4 Reimagined
void CvPlayer::changeCulturePerPopulationSacrified(int iChange)
{
	if (iChange != 0)
	{
		m_iCulturePerPopulationSacrified += iChange;
	}
}

// Civ4 Reimagined
void CvPlayer::changeSlavePointsPerPopulationSacrificed(int iChange)
{
	if (iChange != 0)
	{
		m_iSlavePointsFromSacrificePopulation += iChange; 
	}
}

// Civ4 Reimagined
int CvPlayer::getSlavePointsPerPopulationSacrificed() const
{
	return m_iSlavePointsFromSacrificePopulation;
}

//Civ4 Reimagined
bool CvPlayer::isHasCivicEffect() const
{
	return m_bCivicEffect;
}

//Civ4Reimagined
void CvPlayer::setHasCivicEffect(bool bEnabled)
{
	m_bCivicEffect = bEnabled;
}

int CvPlayer::getRemainingTurnsForCivicEffect(CivicTypes eIndex) const
{
	return m_paiCivicEffect[eIndex];
}

//Civ4Reimagined
void CvPlayer::updateEffectFromStayingAtCivic()
{
	// Do we even have this unique power?
	if (isHasCivicEffect())
	{
		// Get our current government civic
		const CivicOptionTypes governmentCivicOption = (CivicOptionTypes)GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT");
		CivicTypes eCivic = getCivics(governmentCivicOption);
		
		if (eCivic != NO_CIVIC)
		{
			changeTurnsToEffectFromStayingAtCivic(eCivic, -1);
		}
	}	
}

//Civ4 Reimagined
void CvPlayer::changeTurnsToEffectFromStayingAtCivic(CivicTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be non-negative (invalid Index)");
	FAssertMsg(eIndex < GC.getNumCivicInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");	
	
	m_paiCivicEffect[eIndex] = (m_paiCivicEffect[eIndex] + iChange);

	logBBAI("Remaining turns for civic %S: %d ", GC.getCivicInfo(eIndex).getDescription(0), m_paiCivicEffect[eIndex]);
	
	// Trigger effect when the player stayed on that civic for the specified number of turns
	if(iChange < 0 && m_paiCivicEffect[eIndex] == 0)
	{
		UnitTypes eUnit = getCivicEffectGreatPerson(eIndex);

		CvCity* pCapitalCity = getCapitalCity();
		if (eUnit != NO_UNIT && pCapitalCity != NULL)
		{
			CvWString szMessage = gDLL->getText("TXT_KEY_GREEK_GREAT_PERSON_BORN", GC.getCivicInfo(eIndex).getTextKeyWide(), GC.getUnitInfo(eUnit).getTextKeyWide());

			gDLL->getInterfaceIFace()->addHumanMessage(getID(), false, GC.getEVENT_MESSAGE_TIME(), szMessage, "AS2D_UNIT_GREATPEOPLE", MESSAGE_TYPE_MAJOR_EVENT, GC.getUnitInfo(eUnit).getButton(), (ColorTypes)GC.getInfoTypeForString("COLOR_UNIT_TEXT"), pCapitalCity->getX_INLINE(), pCapitalCity->getY_INLINE(), true, true);

			pCapitalCity->createGreatPeople(eUnit, false, false);
		}

		int iCount = 0;

		for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
		{
			if (GC.getCivicInfo((CivicTypes)iI).getCivicOptionType() != (CivicOptionTypes)GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))
			{
				continue;
			}

			if (m_paiCivicEffect[(CivicTypes)iI] == 0)
			{
				iCount++;
			}
		}

		if (iCount >= 4)
		{
			setHasCivicEffect(false);
			notifyUniquePowersChanged(false);
		}
	}
}

// Civ4 Reimagined
UnitTypes CvPlayer::getCivicEffectGreatPerson(CivicTypes eIndex) const
{
	UnitTypes eUnit = NO_UNIT;

	if (eIndex == (CivicTypes)GC.getInfoTypeForString("CIVIC_AUTOCRACY"))
	{
		eUnit = (UnitTypes)GC.getInfoTypeForString("UNIT_GREAT_GENERAL");
	} 
	else if (eIndex == (CivicTypes)GC.getInfoTypeForString("CIVIC_CITY_STATES"))
	{
		eUnit = (UnitTypes)GC.getInfoTypeForString("UNIT_MERCHANT");
	} 
	else if (eIndex == (CivicTypes)GC.getInfoTypeForString("CIVIC_DYNASTICISM"))
	{
		eUnit = (UnitTypes)GC.getInfoTypeForString("UNIT_ARTIST");
	} 
	else if (eIndex == (CivicTypes)GC.getInfoTypeForString("CIVIC_THEOCRACY"))
	{
		eUnit = (UnitTypes)GC.getInfoTypeForString("UNIT_PROPHET");
	} 
	else if (eIndex == (CivicTypes)GC.getInfoTypeForString("CIVIC_REPUBLIC"))
	{
		eUnit = (UnitTypes)GC.getInfoTypeForString("UNIT_SCIENTIST");
	}
	else if (eIndex == (CivicTypes)GC.getInfoTypeForString("CIVIC_TYRANNY"))
	{
		eUnit = (UnitTypes)GC.getInfoTypeForString("UNIT_ENGINEER");
	}

	return eUnit;
}

//Civ4 Reimagined
void CvPlayer::setFaithConquest(bool bNewValue)
{
	m_bFaithConquest = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isHasFaithConquest() const
{
	return m_bFaithConquest && (getStateReligion() != NO_RELIGION);
}

//Civ4 Reimagined
void CvPlayer::changeLiberatedColonyTradeRouteModifier(int iChange)
{
	m_iLiberatedColonyTradeRouteModifier += iChange;
	
	updateTradeRoutes();
}

//Civ4 Reimagined
int CvPlayer::getLiberatedColonyTradeRouteModifier() const
{
	return m_iLiberatedColonyTradeRouteModifier;
}

//Civ4 Reimagined
void CvPlayer::changeCorporationTraderouteModifier(int iChange)
{
	m_iCorporationTraderouteModifier += iChange;
	
	updateTradeRoutes();
}

//Civ4 Reimagined
int CvPlayer::getCorporationTraderouteModifier() const
{
	return m_iCorporationTraderouteModifier;
}

//Civ4 Reimagined
void CvPlayer::changeGreatGeneralGoldenAgeLength(int iChange)
{
	m_iGreatGeneralGoldenAgeLength += iChange;
}

//Civ4 Reimagined
int CvPlayer::getGreatGeneralGoldenAgeLength() const
{
	return m_iGreatGeneralGoldenAgeLength;
}

//Civ4 Reimagined
void CvPlayer::setConscriptInfidels(bool bNewValue)
{
	m_bConscriptInfidels = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isConscriptInfidels() const
{
	return m_bConscriptInfidels;
}

//Civ4 Reimagined
void CvPlayer::changeCatchUpTechModifier(int iChange)
{
	m_iCatchUpTechModifier += iChange;
}

// Civ4 Reimagined
int CvPlayer::getCatchUpTechModifier() const
{
	return m_iCatchUpTechModifier;
}

//Civ4 Reimagined
void CvPlayer::changeReligiousColonyMaintenanceModifier(int iChange)
{
	m_iReligiousColonyMaintenanceModifier += iChange;
}

// Civ4 Reimagined
int CvPlayer::getReligiousColonyMaintenanceModifier() const
{
	return m_iReligiousColonyMaintenanceModifier;
}

//Civ4 Reimagined
void CvPlayer::setAlwaysFreshWater(bool bNewValue)
{
	CvCity* pLoopCity;
	int iLoop = 0;

	m_bAlwaysFreshWater = bNewValue;

	for (pLoopCity = firstCity(&iLoop); pLoopCity != NULL; pLoopCity = nextCity(&iLoop))
	{
		pLoopCity->updateFreshWaterHealth();
	}
}

//Civ4 Reimagined
bool CvPlayer::isAlwaysFreshWater() const
{
	return m_bAlwaysFreshWater;
}

//Civ4 Reimagined
void CvPlayer::setCanRemoveFeatures(bool bNewValue)
{
	m_bCanRemoveFeatures = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isCanRemoveFeatures() const
{
	return m_bCanRemoveFeatures;
}


//Civ4 Reimagined
void CvPlayer::setCityRevoltOnKill(bool bNewValue)
{
	m_bCityRevoltOnKill = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isCityRevoltOnKill() const
{
	return m_bCityRevoltOnKill;
}


//Civ4 Reimagined
void CvPlayer::setNoReligionRemoval(bool bNewValue)
{
	m_bNoReligionRemoval = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isNoReligionRemoval() const
{
	return m_bNoReligionRemoval;
}


//Civ4 Reimagined
void CvPlayer::setCanCoastalRaid(bool bNewValue)
{
	m_bCoastalRaid = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::canCoastalRaid() const
{
	return m_bCoastalRaid;
}


//Civ4 Reimagined
void CvPlayer::setGainGreatWorkGoldWithHitBonuses(bool bNewValue)
{
	m_bGainGreatWorkGoldWithHitBonuses = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isGainGreatWorkGoldWithHitBonuses() const
{
	return m_bGainGreatWorkGoldWithHitBonuses;
}


//Civ4 Reimagined
void CvPlayer::setCapitalAlwaysPerfectBonusValue(bool bNewValue)
{
	m_bCapitalAlwaysPerfectBonusValue = bNewValue;

	CvCity* pCapital = getCapitalCity();

	if (pCapital != NULL)
	{
		pCapital->updateResources();
	}
}

//Civ4 Reimagined
bool CvPlayer::isCapitalAlwaysPerfectBonusValue() const
{
	return m_bCapitalAlwaysPerfectBonusValue;
}


//Civ4 Reimagined
void CvPlayer::setCanBuildWindmillsOnCoast(bool bNewValue)
{
	m_bCanBuildWindmillsOnCoast = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::canBuildWindmillsOnCoast() const
{
	return m_bCanBuildWindmillsOnCoast;
}

//Civ4 Reimagined
void CvPlayer::setIsCityImprovesBonus(bool bNewValue)
{
	m_bCityImprovesBonus = bNewValue;

	updateYield();
}

//Civ4 Reimagined
bool CvPlayer::isCityImprovesBonus() const
{
	return m_bCityImprovesBonus;
}

//Civ4 Reimagined
void CvPlayer::setIsFreePillage(bool bNewValue)
{
	m_bFreePillage = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isFreePillage() const
{
	return m_bFreePillage;
}

//Civ4 Reimagined
void CvPlayer::setIsPirateGold(bool bNewValue)
{
	m_bPirateGold = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isPirateGold() const
{
	return m_bPirateGold;
}

//Civ4 Reimagined
void CvPlayer::setIsIgnoreEarlyWonderHurryCostModifier(bool bNewValue)
{
	m_bIsIgnoreEarlyWonderHurryCostModifier = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isIgnoreEarlyWonderHurryCostModifier() const
{
	return m_bIsIgnoreEarlyWonderHurryCostModifier;
}

//Civ4 Reimagined
void CvPlayer::setIsIgnoreForeignTradeBan(bool bNewValue)
{
	m_bIsIgnoreForeignTradeBan = bNewValue;

	GC.getGameINLINE().updateTradeRoutes();
}

//Civ4 Reimagined
bool CvPlayer::isIgnoreForeignTradeBan() const
{
	return m_bIsIgnoreForeignTradeBan;
}

//Civ4 Reimagined
void CvPlayer::setIsAlwaysReceiveGreatPeopleLateTechs(bool bNewValue)
{
	m_bAlwaysReceiveGreatPeopleLateTechs = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isAlwaysReceiveGreatPeopleLateTechs() const
{
	return m_bAlwaysReceiveGreatPeopleLateTechs;
}

//Civ4 Reimagined
void CvPlayer::setIsCanGoldenAgeWithSamePeople(bool bNewValue)
{
	m_bCanGoldenAgeWithSamePeople = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isCanGoldenAgeWithSamePeople() const
{
	return m_bCanGoldenAgeWithSamePeople;
}

//Civ4 Reimagined
void CvPlayer::setIsLegacyCivic(bool bNewValue)
{
	m_bLegacyCivic = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isLegacyCivic() const
{
	return m_bLegacyCivic;
}

//Civ4 Reimagined
void CvPlayer::setIsFrenchRevolution(bool bNewValue)
{
	m_bFrenchRevolution = bNewValue;
}

//Civ4 Reimagined
bool CvPlayer::isFrenchRevolution() const
{
	return m_bFrenchRevolution;
}

// Civ4 Reimagined
bool CvPlayer::hasGoodRelationsWithPope() const
{
	for (int iI = 0; iI < GC.getNumVoteSourceInfos(); ++iI)
	{
		if (GC.getGameINLINE().getVoteSourceReligion((VoteSourceTypes)iI) != NO_RELIGION)
		{
			const TeamTypes eTeam = GC.getGameINLINE().getSecretaryGeneral((VoteSourceTypes)iI);

			if (eTeam != NO_TEAM)
			{
				const PlayerTypes ePope = GET_TEAM(eTeam).getSecretaryID();
				if (ePope != NO_PLAYER && ePope != getID() && !GET_TEAM(eTeam).isAtWar(getTeam()))
				{
					if (GET_PLAYER(ePope).AI_getAttitude(getID()) >= ATTITUDE_PLEASED || GET_PLAYER(ePope).isHuman())
					{
						return true;
					}
				}
				break;
			}
		}
	}

	return false;
}


// Civ4 Reimagined
UnitTypes CvPlayer::getStrongestPossibleLandUnit() const
{
	UnitTypes eLoopUnit;
	UnitTypes eBestUnit;
	int iValue;
	int iBestValue;
	int iI;
	
	iBestValue = 0;
	eBestUnit = NO_UNIT;

	CvCity* pCapitalCity = getCapitalCity();

	if (pCapitalCity == NULL)
	{
		return NO_UNIT;
	}

	for (iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		eLoopUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI);

		if (eLoopUnit != NO_UNIT && GC.getUnitInfo(eLoopUnit).getDomainType() == DOMAIN_LAND && !isNationalUnitClass((UnitClassTypes)iI))
		{
			if (pCapitalCity->canTrain(eLoopUnit))
			{
				iValue = GC.getUnitInfo(eLoopUnit).getCombat();

				if (iValue > iBestValue)
				{
					iBestValue = iValue;
					eBestUnit = eLoopUnit;
				}
			}
		}
	}

	return eBestUnit;
}


/*
 * Returns the additional great people rate that changing the number of the given specialist will provide/remove.
 */
int CvPlayer::getAdditionalBaseGreatPeopleRateBySpecialist(SpecialistTypes eSpecialist, int iChange) const
{
	FAssertMsg(eSpecialist >= 0, "eSpecialist expected to be >= 0");
	FAssertMsg(eSpecialist < GC.getNumSpecialistInfos(), "eSpecialist expected to be < GC.getNumSpecialistInfos()");

	// Civ4 Reimagined: Persia UP
	int iGreatPeopleRateChange = GC.getSpecialistInfo(eSpecialist).getGreatPeopleRateChange();
	iGreatPeopleRateChange *= 100 + getGreatPeopleRateChangeModifier(eSpecialist);
	iGreatPeopleRateChange /= 100;

	return iChange * iGreatPeopleRateChange;
}


// Civ4 Reimagined
EraTypes CvPlayer::getUniqueUnitEra() const
{
	for (int iI = 0; iI < GC.getNumUnitClassInfos(); iI++)
	{
		UnitTypes eCivUnit = (UnitTypes)GC.getCivilizationInfo(getCivilizationType()).getCivilizationUnits(iI);
		UnitTypes eDefaultUnit = (UnitTypes)GC.getUnitClassInfo((UnitClassTypes)iI).getDefaultUnitIndex();

		if (eCivUnit != NO_UNIT && eCivUnit != eDefaultUnit)
		{
			TechTypes ePrereqTech = (TechTypes)GC.getUnitInfo(eCivUnit).getPrereqAndTech();

			if (ePrereqTech != NO_TECH)
			{
				return (EraTypes)GC.getTechInfo(ePrereqTech).getEra();
			}
		}
	}

	return NO_ERA;
}



// Civ4 Reimagined
void CvPlayer::applyNativeAmericanBonus(int iX, int iY)
{
	CvPlot* pPlot = GC.getMapINLINE().plotINLINE(iX, iY);

	if (pPlot == NULL)
	{
		return;
	}

	if (isNativeAmericanBonus(NATIVEBONUS_LAKE) && pPlot->isAdjacentToLake())
	{
		changeDistanceMaintenanceModifier(-25);
		setNativeAmericanBonus(NATIVEBONUS_LAKE, false);
		notifyNativeAmericanBonus(NATIVEBONUS_LAKE);
	}

	if (isNativeAmericanBonus(NATIVEBONUS_TUNDRA) && pPlot->getTerrainType() == (TerrainTypes)GC.getInfoTypeForString("TERRAIN_TUNDRA"))
	{
		changeImprovementYieldChange((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FISHING_BOATS"), YIELD_FOOD, 1);
		setNativeAmericanBonus(NATIVEBONUS_TUNDRA, false);
		notifyNativeAmericanBonus(NATIVEBONUS_TUNDRA);
	}

	if (isNativeAmericanBonus(NATIVEBONUS_DESERT) && pPlot->getTerrainType() == (TerrainTypes)GC.getInfoTypeForString("TERRAIN_DESERT"))
	{
		changeSpecialistCommerceChange((SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_ARTIST"), COMMERCE_CULTURE, 1);
		setNativeAmericanBonus(NATIVEBONUS_DESERT, false);
		notifyNativeAmericanBonus(NATIVEBONUS_DESERT);
	}

	BonusTypes eBonus = pPlot->getBonusType();

	if (isNativeAmericanBonus(NATIVEBONUS_HUNTING))
	{
		if (eBonus == (BonusTypes)GC.getInfoTypeForString("BONUS_DEER") || eBonus == (BonusTypes)GC.getInfoTypeForString("BONUS_HORSE") || eBonus == (BonusTypes)GC.getInfoTypeForString("BONUS_IVORY"))
		{
			changeDomainExperienceGainModifier(DOMAIN_LAND, 50);
			setNativeAmericanBonus(NATIVEBONUS_HUNTING, false);
			notifyNativeAmericanBonus(NATIVEBONUS_HUNTING);
		}
	}

	FeatureTypes eFeature = pPlot->getFeatureType();

	if (isNativeAmericanBonus(NATIVEBONUS_FOREST) && eFeature == (FeatureTypes)GC.getInfoTypeForString("FEATURE_FOREST"))
	{
		setUniquePowerBuilding((BuildingTypes)GC.getInfoTypeForString("BUILDING_NATIVE_AMERICA_TOTEM"));
		setNativeAmericanBonus(NATIVEBONUS_FOREST, false);
		notifyNativeAmericanBonus(NATIVEBONUS_FOREST);
	}

	if (isNativeAmericanBonus(NATIVEBONUS_JUNGLE) && eFeature == (FeatureTypes)GC.getInfoTypeForString("FEATURE_JUNGLE"))
	{
		changeCityDefenseModifier(5);
		setNativeAmericanBonus(NATIVEBONUS_JUNGLE, false);
		notifyNativeAmericanBonus(NATIVEBONUS_JUNGLE);
	}
}


// Civ4 Raimagined
void CvPlayer::notifyNativeAmericanBonus(NativeBonusTypes eNativeBonus)
{
	if (!isHuman())
	{
		return;
	}

	CvWString szBuffer;

	switch(eNativeBonus)
	{
		case NATIVEBONUS_LAKE:
			szBuffer = gDLL->getText("TXT_KEY_MISC_NATIVE_BONUS_LAKE");
			break;
		case NATIVEBONUS_TUNDRA: 
			szBuffer = gDLL->getText("TXT_KEY_MISC_NATIVE_BONUS_TUNDRA");
			break;
		case NATIVEBONUS_DESERT: 
			szBuffer = gDLL->getText("TXT_KEY_MISC_NATIVE_BONUS_DESERT");
			break;
		case NATIVEBONUS_HUNTING: 
			szBuffer = gDLL->getText("TXT_KEY_MISC_NATIVE_BONUS_HUNTING");
			break;
		case NATIVEBONUS_FOREST: 
			szBuffer = gDLL->getText("TXT_KEY_MISC_NATIVE_BONUS_FOREST");
			break;
		case NATIVEBONUS_JUNGLE: 
			szBuffer = gDLL->getText("TXT_KEY_MISC_NATIVE_BONUS_JUNGLE");
			break;
		default:
			return;
	}
	
	gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GOLDAGESTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"));
}


void CvPlayer::processLegacyCivicBonus(int iChange)
{
	if (getLegacyCivic() == (CivicTypes)GC.getInfoTypeForString("CIVIC_AUTOCRACY"))
	{
		changeMilitaryProductionModifier(25 * iChange);
	} 
	else if (getLegacyCivic() == (CivicTypes)GC.getInfoTypeForString("CIVIC_CITY_STATES"))
	{
		changeSpecialistCommerceChange((SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_MERCHANT"), COMMERCE_GOLD, iChange);
	} 
	else if (getLegacyCivic() == (CivicTypes)GC.getInfoTypeForString("CIVIC_DYNASTICISM"))
	{
		changeCapitalExtraYieldFromCityPercent(YIELD_FOOD, 40 * iChange);
	} 
	else if (getLegacyCivic() == (CivicTypes)GC.getInfoTypeForString("CIVIC_THEOCRACY"))
	{
		changeSpecialistExtraYield((SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_PRIEST"), YIELD_PRODUCTION, iChange);
	} 
	else if (getLegacyCivic() == (CivicTypes)GC.getInfoTypeForString("CIVIC_REPUBLIC"))
	{
		changeImprovementUpgradeRateModifier(50 * iChange);
	}
}


//Civ4 Reimagined
void CvPlayer::updateUniquePowers(TechTypes eTech)
{
	if (getID() == NO_PLAYER)
	{
		return;
	}
	
	if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_ARABIA")
		&& eTech == (TechTypes)GC.getInfoTypeForString("TECH_DIVINE_RIGHT"))
	{
		setFaithConquest(true);
		changeStateReligionShrineModifier(50);
		notifyUniquePowersChanged(true);
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_GREECE")
		&& eTech == (TechTypes)GC.getInfoTypeForString("TECH_WRITING"))
	{
		changeHasCivicOptionCount((CivicOptionTypes)GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"), 1);
		notifyUniquePowersChanged(true);
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_PERSIA")
		&& eTech == (TechTypes)GC.getInfoTypeForString("TECH_LITERATURE"))
	{
		changeGreatPeopleRateChangeModifier((SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_ARTIST"), 100);
		notifyUniquePowersChanged(true);
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_OTTOMAN")
		&& eTech == (TechTypes)GC.getInfoTypeForString("TECH_GUNPOWDER"))
	{
		setConscriptInfidels(true);
		GET_TEAM(getTeam()).changeNoConscriptUnhappinessCount(1);
		notifyUniquePowersChanged(true);
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_NETHERLANDS")
	    && eTech == (TechTypes)GC.getInfoTypeForString("TECH_CORPORATION"))
	{
		GET_TEAM(getTeam()).setAdditionalPlantationBonus(1);
		changeGreatMerchantPointsPerTrade(20);
		notifyUniquePowersChanged(true);
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_ROME"))
	{
		if (eTech == (TechTypes)GC.getInfoTypeForString("TECH_ENGINEERING"))
		{
			changeRouteChange((RouteTypes)GC.getInfoTypeForString("ROUTE_ROAD"), 10);
		}

		if (eTech == (TechTypes)GC.getInfoTypeForString("TECH_PRINTING_PRESS"))
		{
			changeImprovementYieldChange((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_VILLAGE"), YIELD_COMMERCE, -1);
			changeImprovementYieldChange((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_TOWN"), YIELD_COMMERCE, -1);
		}
	}
}

//Civ4 Reimagined
void CvPlayer::notifyUniquePowersChanged(bool bGained) const
{
	if (isHuman())
	{
		if (bGained)
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_UNIQUE_POWER");
			gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GOLDAGESTART", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_GREEN"));
		}
		else
		{
			CvWString szBuffer = gDLL->getText("TXT_KEY_MISC_UNIQUE_POWER_LOST");
			gDLL->getInterfaceIFace()->addHumanMessage(getID(), true, GC.getEVENT_MESSAGE_TIME(), szBuffer, "AS2D_GOLDAGEEND", MESSAGE_TYPE_MAJOR_EVENT, NULL, (ColorTypes)GC.getInfoTypeForString("COLOR_RED"));
		}

	}
	
}

// Remember to adjust CvPlayerAI::uniquePowerAIEraValueMult and CvPlayerAI::uniquePowerAITechValueMult accordingly
void CvPlayer::updateUniquePowers(EraTypes eEra)
{
	if (getID() == NO_PLAYER)
	{
		return;		
	}

	logBBAI("Update Unique Powers for %d and era %d", getID(), (int)eEra);

	if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_AMERICA"))
	{
		if (eEra == ERA_INDUSTRIAL)
		{
			changeCorporationTraderouteModifier(GC.getDefineINT("UNIQUE_POWER_AMERICA"));
			changeGreatPeoplePointsPerTrade(40);
			
			UnitClassTypes UNITCLASS_EXECUTIVE_4 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_EXECUTIVE_4");
			UnitClassTypes UNITCLASS_EXECUTIVE_5 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_EXECUTIVE_5");
			UnitClassTypes UNITCLASS_EXECUTIVE_6 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_EXECUTIVE_6");
			UnitClassTypes UNITCLASS_EXECUTIVE_7 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_EXECUTIVE_7");
			UnitClassTypes UNITCLASS_EXECUTIVE_8 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_EXECUTIVE_8");
			UnitClassTypes UNITCLASS_EXECUTIVE_9 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_EXECUTIVE_9");
			UnitClassTypes UNITCLASS_EXECUTIVE_10 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_EXECUTIVE_10");
			UnitClassTypes UNITCLASS_EXECUTIVE_11 = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_EXECUTIVE_11");
			changeUnitClassProductionModifier(UNITCLASS_EXECUTIVE_4, 100);
			changeUnitClassProductionModifier(UNITCLASS_EXECUTIVE_5, 100);
			changeUnitClassProductionModifier(UNITCLASS_EXECUTIVE_6, 100);
			changeUnitClassProductionModifier(UNITCLASS_EXECUTIVE_7, 100);
			changeUnitClassProductionModifier(UNITCLASS_EXECUTIVE_8, 100);
			changeUnitClassProductionModifier(UNITCLASS_EXECUTIVE_9, 100);
			changeUnitClassProductionModifier(UNITCLASS_EXECUTIVE_10, 100);
			changeUnitClassProductionModifier(UNITCLASS_EXECUTIVE_11, 100);
			changeWorldWonderProductionModifier(100);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_AZTEC"))
	{
		if (eEra == ERA_ANCIENT)
		{
			setIsCaptureSlaves(true);
			changePillageGainModifier(100);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_BABYLON"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changeNoCapitalUnhappinessCount(1);
			setCapitalCommercePerPopulation(COMMERCE_GOLD,      GC.getDefineINT("UNIQUE_POWER_BABYLON"), GC.getDefineINT("UNIQUE_POWER_BABYLON_2"));
			setCapitalCommercePerPopulation(COMMERCE_RESEARCH,  GC.getDefineINT("UNIQUE_POWER_BABYLON"), GC.getDefineINT("UNIQUE_POWER_BABYLON_2"));
			setCapitalCommercePerPopulation(COMMERCE_CULTURE,   GC.getDefineINT("UNIQUE_POWER_BABYLON"), GC.getDefineINT("UNIQUE_POWER_BABYLON_2"));
			setCapitalCommercePerPopulation(COMMERCE_ESPIONAGE, GC.getDefineINT("UNIQUE_POWER_BABYLON"), GC.getDefineINT("UNIQUE_POWER_BABYLON_2"));
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_BYZANTIUM"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changeCapitalCultureAttitudeBonus(2);
			notifyUniquePowersChanged(true);
		}
		else if (eEra == ERA_MEDIEVAL)
		{
			changeStateReligionBuildingCommerce(COMMERCE_ESPIONAGE, 2);
			setUniquePowerPromotion((PromotionTypes)GC.getInfoTypeForString("PROMOTION_GREEK_FIRE"));
			changeGreatPeopleRateChangeModifier((SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_SPY"), 100);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_CARTHAGE"))
	{
		if (eEra == ERA_CLASSICAL)
		{
			setSpecialTradeRoutePerPlayer(true);
			changeMercenaryCostModifier(-50);
			setIsLegacyCivic(true);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_CELT"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changeFeatureCommerce((FeatureTypes)GC.getInfoTypeForString("FEATURE_FOREST"), COMMERCE_CULTURE, GC.getDefineINT("UNIQUE_POWER_CELT_CULTURE"));
			GET_TEAM(getTeam()).setForceRevealedBonus((BonusTypes)GC.getInfoTypeForString("BONUS_IRON"), true);
			setIsFreePillage(true);
			changeLootCityModifier(100);
			changeAdditionalMineBonusCommerce(2);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_CHINA"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changeUnhealthyPopulationModifier(-25);
			changeAdditionalFarmBonusYield(2);
			changeTechBulbModifier(50);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_EGYPT"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changeSlavePointsPerPopulationSacrificed(GC.getDefineINT("UNIQUE_POWER_EGYPT"));
			changeProductionPerPopulation(6);
			setIsIgnoreEarlyWonderHurryCostModifier(true);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_ENGLAND"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changeDomainGreatGeneralRateModifier(DOMAIN_SEA, 200);
			notifyUniquePowersChanged(true);
		}
		else if (eEra == ERA_RENAISSANCE)
		{
			setIsPirateGold(true);
			changeCorporationMaintenanceModifier(-50);
			notifyUniquePowersChanged(true);
		}
		else if (eEra == ERA_INDUSTRIAL)
		{
			changeLiberatedColonyTradeRouteModifier(GC.getDefineINT("UNIQUE_POWER_ENGLAND"));
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_ETHIOPIA"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changePeakAdjacencyExtraYield(YIELD_COMMERCE, 1);
			changeCityOnHillsExtraYield(YIELD_FOOD, 1);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_FRANCE"))
	{
		if (eEra == ERA_ANCIENT)
		{
			setCapitalAlwaysPerfectBonusValue(true);
			changeGreatEngineerPointsFromCathedrals(6);
			notifyUniquePowersChanged(true);
		}
		else if (eEra == ERA_MEDIEVAL)
		{
			changeGreatPeopleExtraCommerceInCapital(COMMERCE_GOLD, 3);
			setIsFrenchRevolution(true);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_GERMANY"))
	{
		if (eEra == ERA_ANCIENT)
		{
			setIsAlwaysReceiveGreatPeopleLateTechs(true);
		}
		if (eEra == ERA_INDUSTRIAL)
		{
			changeGreatGeneralGoldenAgeLength(GC.getDefineINT("UNIQUE_POWER_GERMANY"));
			changeBuyBuildingCostModifier(-33);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_GREECE"))
	{
		if (eEra == ERA_ANCIENT)
		{
			setHasCivicEffect(true); // Activate before calling changeTurnsToEffectFromStayingAtCivic
			changeCoastalTradeRouteModifier(100);

			for (int iI = 0; iI < GC.getNumCivicInfos(); iI++)
			{
				// Exclude non-government civics
				if (GC.getCivicInfo((CivicTypes)iI).getCivicOptionType() == (CivicOptionTypes)GC.getInfoTypeForString("CIVICOPTION_GOVERNMENT"))
				{
					int iTurnsPerCivic = GC.getDefineINT("UNIQUE_POWER_GREECE");
						
					iTurnsPerCivic *= GC.getGameSpeedInfo(GC.getGameINLINE().getGameSpeedType()).getResearchPercent();
					iTurnsPerCivic /= 100;

					m_paiCivicEffect[iI] = iTurnsPerCivic;
				}
			}
			
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_HOLY_ROMAN"))
	{
		if (eEra == ERA_MEDIEVAL) 
		{
			changeReligiousVoteModifier(100);
			changeVoteSourceStateReligionUnitProductionModifier(75);
			changeGreatSpyPointsFromImprovementInRadius((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_FORT"), 10);
			changeAnarchyModifier(100);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_INDIA"))
	{
		if (eEra == ERA_ANCIENT) 
		{
			changeGreatPeopleRatePerReligionModifier(35);
			changeExtraYieldPerReligion(YIELD_FOOD, 1);
			setNoReligionRemoval(true);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_INCA"))
	{
		if (eEra == ERA_ANCIENT) 
		{
			changeCanFarmHillsCount(1);
			changePeakYield(YIELD_PRODUCTION, 3);
			changePeakYield(YIELD_COMMERCE, 1);
			changePeakYieldChangeAdjacentToTerrace(YIELD_FOOD, 1);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_JAPAN"))
	{
		if (eEra == ERA_ANCIENT)
		{
			BuildingTypes eHarbor = (BuildingTypes)GC.getInfoTypeForString("BUILDING_HARBOR");
			changeCultureResistanceModifier(75);
			changeBonusHealthFromBuilding(eHarbor, (BonusTypes)GC.getInfoTypeForString("BONUS_CLAM"), 1);
			changeBonusHealthFromBuilding(eHarbor, (BonusTypes)GC.getInfoTypeForString("BONUS_CRAB"), 1);
			changeBonusHealthFromBuilding(eHarbor, (BonusTypes)GC.getInfoTypeForString("BONUS_FISH"), 1);
			notifyUniquePowersChanged(true);
		}
		else if (eEra == ERA_INDUSTRIAL)
		{
			changeCatchUpTechModifier(GC.getDefineINT("UNIQUE_POWER_JAPAN"));
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_KHMER"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changeFreshWaterHealthModifier(100);
			notifyUniquePowersChanged(true);
		}
		else if (eEra == ERA_MEDIEVAL)
		{
			changeStateReligionCommercePerPopulationOverThreshold(COMMERCE_GOLD, GC.getDefineINT("UNIQUE_POWER_KHMER"));
			changeSpecialistCommerceChange((SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_PRIEST"), COMMERCE_GOLD, 1);
			setUnlimitedSpecialistsWithTemple((SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_PRIEST"), true);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_KOREA"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changeDomesticGreatGeneralRateModifier(100);
			changeStateReligionBuildingCommerce(COMMERCE_CULTURE, 2);
			notifyUniquePowersChanged(true);
		}
		else if (eEra == ERA_INDUSTRIAL)
		{
			changeTownAdjacencyBonus(YIELD_COMMERCE, 3);
			setGainGreatWorkGoldWithHitBonuses(true);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_MAYA"))
	{
		if (eEra == ERA_ANCIENT) 
		{
			GET_TEAM(getTeam()).changeIrrigationCount(1);
			GET_TEAM(getTeam()).changeIgnoreIrrigationCount(1);
			setAlwaysFreshWater(true);
			setCanRemoveFeatures(true);
			setIsCityImprovesBonus(true);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_MALI"))
	{
		if (eEra == ERA_CLASSICAL) 
		{
			changeSpecialistExtraYield((SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_MERCHANT"), YIELD_FOOD, 1);
			changeFeatureExtraYield((FeatureTypes)GC.getInfoTypeForString("FEATURE_OASIS"), YIELD_FOOD, 2);
			setIsDesertGold(true);
			notifyUniquePowersChanged(true);
		}			
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_MONGOL"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changePillageHeal(GC.getDefineINT("UNIQUE_POWER_MONGOL_PILLAGE_HEAL"));
			notifyUniquePowersChanged(true);
		}
		else if (eEra == ERA_MEDIEVAL)
		{
			setCityRevoltOnKill(true);
			changeDomesticTradeModifier(100);
			changeEspionagePointsOnConquestPerPopulation(25);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_NATIVE_AMERICA"))
	{
		if (eEra == ERA_ANCIENT)
		{
			for (int iI = 0; iI < NUM_NATIVEBONUS_TYPES; iI++)
			{
				setNativeAmericanBonus((NativeBonusTypes)iI, true);
			}
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_NETHERLANDS"))
	{
		if (eEra == ERA_ANCIENT) 
		{
			setCanBuildWindmillsOnCoast(true);
			changeImprovementYieldChange((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_WINDMILL"), YIELD_COMMERCE, 1);
			notifyUniquePowersChanged(true);
		}			
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_OTTOMAN"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changeUpkeepModifier(-50);
			setIsIgnoreForeignTradeBan(true);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_PERSIA"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changeExtraMovesInGoldenAge(DOMAIN_LAND, 1);
			changeGoldenAgeModifier(100);
			setIsCanGoldenAgeWithSamePeople(true);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_PORTUGAL"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changeTerrainMovementCostModifier((TerrainTypes)GC.getInfoTypeForString("TERRAIN_COAST"), -25);
		}
		else if (eEra == ERA_MEDIEVAL)
		{
			changeTradeGoldModifierPerForeignResource(GC.getDefineINT("UNIQUE_POWER_PORTUGAL"));
			changeOverseaTradeRouteModifier(200);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_ROME"))
	{
		if (eEra == ERA_ANCIENT)
		{
			changeBuildingProductionModifierFromCapital(50);
			changeImprovementYieldChange((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_VILLAGE"), YIELD_COMMERCE, 1);
			changeImprovementYieldChange((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_TOWN"), YIELD_COMMERCE, 1);
			changeFreeUnitsOnConquest(GC.getDefineINT("UNIQUE_POWER_ROME"));
			changeRouteChange((RouteTypes)GC.getInfoTypeForString("ROUTE_ROAD"), -10);
			notifyUniquePowersChanged(true);
		}			
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_RUSSIA"))
	{
		if (eEra == ERA_MEDIEVAL) 
		{
			changeTerrainYieldChange((TerrainTypes)GC.getInfoTypeForString("TERRAIN_TUNDRA"), YIELD_PRODUCTION, 1);
			changeTerrainYieldChange((TerrainTypes)GC.getInfoTypeForString("TERRAIN_SNOW"), YIELD_PRODUCTION, 1);
			changeCombatBonusOnHomeArea(10);
			changeStrategicBonusYieldModifier(100);
			changeImprovementYieldChangeAdjacentToStrategicBonus((ImprovementTypes)GC.getInfoTypeForString("IMPROVEMENT_WORKSHOP"), YIELD_COMMERCE, 2);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_SPAIN"))
	{
		if (eEra == ERA_MEDIEVAL)
		{
			UnitClassTypes UNITCLASS_SETTLER = (UnitClassTypes)GC.getInfoTypeForString("UNITCLASS_SETTLER");
			changeReligiousUnitClassProductionModifier(UNITCLASS_SETTLER, GC.getDefineINT("UNIQUE_POWER_SPAIN_RELIGIOUS_SETTLER"));
			changeReligiousColonyMaintenanceModifier(GC.getDefineINT("UNIQUE_POWER_SPAIN_RELIGIOUS_MAINTENANCE"));
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_SUMERIA"))
	{
		if (eEra == ERA_ANCIENT) 
		{
			changeAdditionalAncientEurekaBoost(GC.getDefineINT("UNIQUE_POWER_SUMERIA"));
			changeExtraSpecialistHappiness((SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_GREAT_PRIEST"), 1);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_VIKING"))
	{
		if (eEra == ERA_ANCIENT) 
		{
			setCanCoastalRaid(true);
			changeTerrainYieldChange((TerrainTypes)GC.getInfoTypeForString("TERRAIN_TUNDRA"), YIELD_COMMERCE, 1);
			setFreePromotion((UnitCombatTypes)GC.getInfoTypeForString("UNITCOMBAT_MELEE"), (PromotionTypes)GC.getInfoTypeForString("PROMOTION_AMPHIBIOUS"), true);
			changeDomainProductionModifier(DOMAIN_SEA, 50);
			notifyUniquePowersChanged(true);
		}
	}
	else if (getCivilizationType() == (CivilizationTypes)GC.getInfoTypeForString("CIVILIZATION_ZULU"))
	{
		if (eEra == ERA_ANCIENT) 
		{
			changeHurryCount((HurryTypes)GC.getInfoTypeForString("HURRY_POPULATION_UNITS"), 1);
			changeExtraSpecialistExperience((SpecialistTypes)GC.getInfoTypeForString("SPECIALIST_GREAT_GENERAL"), 2);
			notifyUniquePowersChanged(true);
		}
	}
}

/************************************************************************************************/
/* Civ4 Reimagined Unique Powers                    END                                         */
/************************************************************************************************/


