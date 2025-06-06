#pragma once

// playerAI.h

#ifndef CIV4_PLAYER_AI_H
#define CIV4_PLAYER_AI_H

#include "CvPlayer.h"
#include "AI_defines.h"

class CvEventTriggerInfo;

class CvPlayerAI : public CvPlayer
{

public:

	CvPlayerAI();
	virtual ~CvPlayerAI();

  // inlined for performance reasons
#ifdef _USRDLL
	static CvPlayerAI& getPlayer(PlayerTypes ePlayer)
	{
		FAssertMsg(ePlayer != NO_PLAYER, "Player is not assigned a valid value");
		FAssertMsg(ePlayer < MAX_PLAYERS, "Player is not assigned a valid value");
		return m_aPlayers[ePlayer];
	}
#endif
	DllExport static CvPlayerAI& getPlayerNonInl(PlayerTypes ePlayer);

	static void initStatics();
	static void freeStatics();
	DllExport static bool areStaticsInitialized();

	void AI_init();
	void AI_uninit();
	void AI_reset(bool bConstructor);

	int AI_getFlavorValue(FlavorTypes eFlavor) const;

	void updateCacheData(); // K-Mod

	void AI_doTurnPre();
	void AI_doTurnPost();
	void AI_doTurnUnitsPre();
	void AI_doTurnUnitsPost();

	void AI_doPeace();

	void AI_updateFoundValues(bool bStartingLoc = false);
	void AI_updateAreaTargets();

	int AI_movementPriority(CvSelectionGroup* pGroup) const;
	void AI_unitUpdate();

	void AI_makeAssignWorkDirty();
	void AI_assignWorkingPlots();
	void AI_updateAssignWork();

	void AI_makeProductionDirty();

	void AI_doCentralizedProduction(); // K-Mod. (not used)

	void AI_conquerCity(CvCity* pCity);

	bool AI_acceptUnit(CvUnit* pUnit) const;
	bool AI_captureUnit(UnitTypes eUnit, CvPlot* pPlot) const;

	DomainTypes AI_unitAIDomainType(UnitAITypes eUnitAI) const;

	int AI_yieldWeight(YieldTypes eYield, const CvCity* pCity = 0) const; // K-Mod added city argument
	int AI_commerceWeight(CommerceTypes eCommerce, const CvCity* pCity = NULL) const;
	void AI_updateCommerceWeights(); // K-Mod

	short AI_foundValue(int iX, int iY, int iMinRivalRange = -1, bool bStartingLoc = false) const;
	// K-Mod. (note, I also changed AI_foundValue to return short instead of int)
	struct CvFoundSettings
	{
		CvFoundSettings(const CvPlayerAI& kPlayer, bool bStartingLoc);
		int iMinRivalRange;
		bool bStartingLoc;
		int iClaimThreshold; // culture required to pop the 2nd borders. (from original bts)

		// some trait information that will influence where we settle
		int iGreed; // a number from the original bts code.
		bool bEasyCulture; // easy for us to pop the culture to the 2nd border
		bool bAmbitious; // expectation of taking foreign land, either by culture or by force
		bool bFinancial; // more value for rivers
		bool bDefensive; // more value for settlings on hills
		bool bSeafaring; // special affection for coast cities due to unique building or unit.
		bool bExpansive; // willing to place cities further apart. (not based on the expansive trait)
		bool bAllSeeing; // doesn't need vision of a plot to know what's there.
	};
	short AI_foundValue_bulk(int iX, int iY, const CvFoundSettings& kSet) const;
	// K-Mod end

	bool AI_isAreaAlone(CvArea* pArea) const;
	bool AI_isCapitalAreaAlone() const;
	bool AI_isPrimaryArea(CvArea* pArea) const;

	int AI_militaryWeight(CvArea* pArea) const;

	int AI_targetCityValue(CvCity* pCity, bool bRandomize, bool bIgnoreAttackers = false) const;
	CvCity* AI_findTargetCity(CvArea* pArea) const;

	bool AI_isCommercePlot(CvPlot* pPlot) const;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      08/20/09                                jdog5000      */
/*                                                                                              */
/*                                                                                              */
/************************************************************************************************/
	bool isSafeRangeCacheValid() const; // K-Mod
	bool AI_getAnyPlotDanger(CvPlot* pPlot, int iRange = -1, bool bTestMoves = true, bool bCheckBorder = true) const; // K-Mod added bCheckBorder
	int AI_getPlotDanger(CvPlot* pPlot, int iRange = -1, bool bTestMoves = true) const;
	//int AI_getUnitDanger(CvUnit* pUnit, int iRange = -1, bool bTestMoves = true, bool bAnyDanger = true) const;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	int AI_getWaterDanger(CvPlot* pPlot, int iRange, bool bTestMoves = true) const;

	bool AI_avoidScience() const;
	bool AI_isFinancialTrouble() const;
	//int AI_goldTarget() const;
	int AI_goldTarget(bool bUpgradeBudgetOnly = false) const; // K-Mod

	TechTypes AI_bestTech(int iMaxPathLength = 1, bool bIgnoreCost = false, bool bAsync = false, TechTypes eIgnoreTech = NO_TECH, AdvisorTypes eIgnoreAdvisor = NO_ADVISOR, EraTypes eMaxEra = NO_ERA) const; //Civ4 Reimagined
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/18/10                                jdog5000      */
/*                                                                                              */
/* Tech AI                                                                                      */
/************************************************************************************************/
	int AI_techValue(TechTypes eTech, int iPathLength, bool bIgnoreCost, bool bAsync, const std::vector<int>& viBonusClassRevealed, const std::vector<int>& viBonusClassUnrevealed, const std::vector<int>& viBonusClassHave) const;
	int uniquePowerAIEraValueMult(EraTypes eEra) const; // Civ4 Reimagined
	int uniquePowerAITechValueMult(TechTypes eTech) const; // Civ4 Reimagined
	int AI_obsoleteBuildingPenalty(TechTypes eTech, bool bConstCache) const; // K-Mod
	int AI_techBuildingValue(TechTypes eTech, bool bConstCache, bool& bEnablesWonder) const; // Rewritten for K-Mod
	int AI_techProjectValue(TechTypes eTech, bool& bEnablesWonder) const; // Civ4 Reimagined
	int AI_techUnitValue( TechTypes eTech, int iPathLength, bool &bEnablesUnitWonder, CivicTypes eCivic ) const; //Civ4 Reimagined
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	int AI_cultureVictoryTechValue(TechTypes eTech) const;

	void AI_chooseFreeTech(EraTypes eMaxEra = NO_ERA); //Civ4 Reimagined
	void AI_chooseResearch();

	DllExport DiploCommentTypes AI_getGreeting(PlayerTypes ePlayer) const;
	bool AI_isWillingToTalk(PlayerTypes ePlayer) const; // Exposed to Python
	bool AI_demandRebukedSneak(PlayerTypes ePlayer) const;
	bool AI_demandRebukedWar(PlayerTypes ePlayer) const;
	bool AI_hasTradedWithTeam(TeamTypes eTeam) const;

	void AI_updateAttitudeCache(); // K-Mod (for all players)
	void AI_updateAttitudeCache(PlayerTypes ePlayer); // K-Mod
	void AI_changeCachedAttitude(PlayerTypes ePlayer, int iChange); // K-Mod
	AttitudeTypes AI_getAttitude(PlayerTypes ePlayer, bool bForced = true) const;
	int AI_getAttitudeVal(PlayerTypes ePlayer, bool bForced = true) const;
	static AttitudeTypes AI_getAttitudeFromValue(int iAttitudeVal);

	int AI_calculateStolenCityRadiusPlots(PlayerTypes ePlayer) const;
	void AI_updateCloseBorderAttitudeCache(); // K-Mod
	void AI_updateCloseBorderAttitudeCache(PlayerTypes ePlayer); // K-Mod
	int AI_getCloseBordersAttitude(PlayerTypes ePlayer) const;

	int AI_getWarAttitude(PlayerTypes ePlayer) const;
	int AI_getPeaceAttitude(PlayerTypes ePlayer) const;
	int AI_getSameReligionAttitude(PlayerTypes ePlayer) const;
	int AI_getDifferentReligionAttitude(PlayerTypes ePlayer) const;
	int AI_getBonusTradeAttitude(PlayerTypes ePlayer) const;
	int AI_getOpenBordersAttitude(PlayerTypes ePlayer) const;
	int AI_getDefensivePactAttitude(PlayerTypes ePlayer) const;
	int AI_getRivalDefensivePactAttitude(PlayerTypes ePlayer) const;
	int AI_getRivalVassalAttitude(PlayerTypes ePlayer) const;
	int AI_getShareWarAttitude(PlayerTypes ePlayer) const;
	int AI_getCivicsAttitude(PlayerTypes ePlayer) const; // Civ4 Reimagined
	int AI_getFavoriteCivicAttitude(PlayerTypes ePlayer) const;
	int AI_getTradeAttitude(PlayerTypes ePlayer) const;
	int AI_getRivalTradeAttitude(PlayerTypes ePlayer) const;
	int AI_getMemoryAttitude(PlayerTypes ePlayer, MemoryTypes eMemory) const;
	int AI_getColonyAttitude(PlayerTypes ePlayer) const;
	int AI_getNeedOpenBordersAttitude(PlayerTypes ePlayer) const; // Civ4 Reimagined
	int AI_updateAttitudeCache(PlayerTypes ePlayer) const; // Civ4 Reimagined
	int AI_getSameIdeologyAttitude(PlayerTypes ePlayer) const; // Civ4 Reimagined
	int AI_getDifferentIdeologyAttitude(PlayerTypes ePlayer) const; // Civ4 Reimagined
	int AI_getCapitalCultureAttitude(PlayerTypes ePlayer) const; // Civ4 Reimagined

	PlayerVoteTypes AI_diploVote(const VoteSelectionSubData& kVoteData, VoteSourceTypes eVoteSource, bool bPropose);

	int AI_dealVal(PlayerTypes ePlayer, const CLinkList<TradeData>* pList, bool bIgnoreAnnual = false, int iExtra = 1) const;
	bool AI_goldDeal(const CLinkList<TradeData>* pList) const;
	bool AI_considerOffer(PlayerTypes ePlayer, const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList, int iChange = 1) const;
	bool AI_counterPropose(PlayerTypes ePlayer, const CLinkList<TradeData>* pTheirList, const CLinkList<TradeData>* pOurList, CLinkList<TradeData>* pTheirInventory, CLinkList<TradeData>* pOurInventory, CLinkList<TradeData>* pTheirCounter, CLinkList<TradeData>* pOurCounter) const;
	int AI_tradeAcceptabilityThreshold(PlayerTypes eTrader) const; // K-Mod

	DllExport int AI_maxGoldTrade(PlayerTypes ePlayer) const;

	DllExport int AI_maxGoldPerTurnTrade(PlayerTypes ePlayer) const;
	int AI_goldPerTurnTradeVal(int iGoldPerTurn) const;

	int AI_bonusVal(BonusTypes eBonus, int iChange, bool bAssumeEnabled = false) const; // K-Mod added bAssumeEnabled
	int AI_baseBonusVal(BonusTypes eBonus, bool bAdditional = false) const;
	int AI_bonusTradeVal(BonusTypes eBonus, PlayerTypes ePlayer, int iChange) const;
	DenialTypes AI_bonusTrade(BonusTypes eBonus, PlayerTypes ePlayer) const;
	int AI_corporationBonusVal(BonusTypes eBonus) const;

	int AI_cityTradeVal(CvCity* pCity) const;
	DenialTypes AI_cityTrade(CvCity* pCity, PlayerTypes ePlayer) const;

	int AI_stopTradingTradeVal(TeamTypes eTradeTeam, PlayerTypes ePlayer) const;
	DenialTypes AI_stopTradingTrade(TeamTypes eTradeTeam, PlayerTypes ePlayer) const;

	int AI_civicTradeVal(CivicTypes eCivic, PlayerTypes ePlayer) const;
	DenialTypes AI_civicTrade(CivicTypes eCivic, PlayerTypes ePlayer) const;

	int AI_religionTradeVal(ReligionTypes eReligion, PlayerTypes ePlayer) const;
	DenialTypes AI_religionTrade(ReligionTypes eReligion, PlayerTypes ePlayer) const;

	int AI_unitImpassableCount(UnitTypes eUnit) const;
	int AI_unitValue(UnitTypes eUnit, UnitAITypes eUnitAI, CvArea* pArea) const;
	int AI_totalUnitAIs(UnitAITypes eUnitAI) const;
	int AI_totalAreaUnitAIs(CvArea* pArea, UnitAITypes eUnitAI) const;
	int AI_totalWaterAreaUnitAIs(CvArea* pArea, UnitAITypes eUnitAI) const;
	int AI_countCargoSpace(UnitAITypes eUnitAI) const;

	int AI_neededExplorers(CvArea* pArea) const;
	int AI_neededWorkers(CvArea* pArea) const;
	int AI_neededMissionaries(CvArea* pArea, ReligionTypes eReligion) const;
	int AI_neededExecutives(CvArea* pArea, CorporationTypes eCorporation) const;
	int AI_unitCostPerMil() const; // K-Mod
	int AI_maxUnitCostPerMil(CvArea* pArea = 0, int iBuildProb = -1) const; // K-Mod
	int AI_nukeWeight() const; // K-Mod
	bool AI_isLandWar(CvArea* pArea) const; // K-Mod

	int AI_missionaryValue(CvArea* pArea, ReligionTypes eReligion, PlayerTypes* peBestPlayer = NULL) const;
	int AI_executiveValue(CvArea* pArea, CorporationTypes eCorporation, PlayerTypes* peBestPlayer = NULL, bool bSpreadOnly = false) const;

	int AI_corporationValue(CorporationTypes eCorporation, const CvCity* pCity = NULL) const;

	int AI_adjacentPotentialAttackers(CvPlot* pPlot, bool bTestCanMove = false) const;
	int AI_totalMissionAIs(MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_areaMissionAIs(CvArea* pArea, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0) const;
	int AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, int& iClosestTargetRange, CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0) const;
	int AI_plotTargetMissionAIs(CvPlot* pPlot, MissionAITypes* aeMissionAI, int iMissionAICount, int& iClosestTargetRange, CvSelectionGroup* pSkipSelectionGroup = NULL, int iRange = 0) const;
	int AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_enemyTargetMissionAIs(MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_enemyTargetMissionAIs(MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_wakePlotTargetMissionAIs(CvPlot* pPlot, MissionAITypes eMissionAI, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
// K-Mod
	int AI_localDefenceStrength(const CvPlot* pDefencePlot, TeamTypes eDefenceTeam, DomainTypes eDomainType = DOMAIN_LAND, int iRange = 0, bool bAtTarget = true, bool bCheckMoves = false, bool bNoCache = false) const;
	int AI_localAttackStrength(const CvPlot* pTargetPlot, TeamTypes eAttackTeam, DomainTypes eDomainType = DOMAIN_LAND, int iRange = 2, bool bUseTarget = true, bool bCheckMoves = false, bool bCheckCanAttack = false) const;
	int AI_cityTargetStrengthByPath(CvCity* pCity, CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns) const;
// K-Mod end
// BBAI
	int AI_enemyTargetMissions(TeamTypes eTargetTeam, CvSelectionGroup* pSkipSelectionGroup = NULL) const;
	int AI_unitTargetMissionAIs(CvUnit* pUnit, MissionAITypes* aeMissionAI, int iMissionAICount, CvSelectionGroup* pSkipSelectionGroup, int iMaxPathTurns) const;
// BBAI end

	// Civ4 Reimagined
	CivicTypes AI_bestCivic(CivicOptionTypes eCivicOption, int* iBestValue = 0, bool bNoWarWeariness = false, bool bStateReligion = true, int iHappy = 1, IdeologyTypes eBestIdeology = NO_IDEOLOGY) const;
	int AI_civicValue(CivicTypes eCivic, bool bNoWarWeariness = false, bool bStateReligion = true, int iHappy = 1, IdeologyTypes eBestIdeology = NO_IDEOLOGY) const;
	int AI_getBonusRatioModfierValue(const int iModifier) const;

	ReligionTypes AI_bestReligion() const;
	int AI_religionValue(ReligionTypes eReligion) const;

	//EspionageMissionTypes AI_bestPlotEspionage(CvPlot* pSpyPlot, PlayerTypes& eTargetPlayer, CvPlot*& pPlot, int& iData) const;
	// K-Mod has moved AI_bestPlotEspionage to CvUnitAI::
	int AI_espionageVal(PlayerTypes eTargetPlayer, EspionageMissionTypes eMission, CvPlot* pPlot, int iData) const;
	bool AI_isMaliciousEspionageTarget(PlayerTypes eTarget) const; // K-Mod

	int AI_getPeaceWeight() const;
	void AI_setPeaceWeight(int iNewValue);

	int AI_getEspionageWeight() const;
	void AI_setEspionageWeight(int iNewValue);

	int AI_getAttackOddsChange() const;
	void AI_setAttackOddsChange(int iNewValue);

	int AI_getCivicTimer() const;
	void AI_setCivicTimer(int iNewValue);
	void AI_changeCivicTimer(int iChange);

	int AI_getReligionTimer() const;
	void AI_setReligionTimer(int iNewValue);
	void AI_changeReligionTimer(int iChange);

	int AI_getExtraGoldTarget() const;
	void AI_setExtraGoldTarget(int iNewValue);

	int AI_getNumTrainAIUnits(UnitAITypes eIndex) const;
	void AI_changeNumTrainAIUnits(UnitAITypes eIndex, int iChange);

	int AI_getNumAIUnits(UnitAITypes eIndex) const;
	void AI_changeNumAIUnits(UnitAITypes eIndex, int iChange);

	int AI_getSameReligionCounter(PlayerTypes eIndex) const;
	void AI_changeSameReligionCounter(PlayerTypes eIndex, int iChange);

	int AI_getDifferentReligionCounter(PlayerTypes eIndex) const;
	void AI_changeDifferentReligionCounter(PlayerTypes eIndex, int iChange);

	int AI_getFavoriteCivicCounter(PlayerTypes eIndex) const;
	void AI_changeFavoriteCivicCounter(PlayerTypes eIndex, int iChange);

	int AI_getBonusTradeCounter(PlayerTypes eIndex) const;
	void AI_changeBonusTradeCounter(PlayerTypes eIndex, int iChange);

	int AI_getPeacetimeTradeValue(PlayerTypes eIndex) const;
	void AI_changePeacetimeTradeValue(PlayerTypes eIndex, int iChange);

	int AI_getPeacetimeGrantValue(PlayerTypes eIndex) const;
	void AI_changePeacetimeGrantValue(PlayerTypes eIndex, int iChange);

	int AI_getGoldTradedTo(PlayerTypes eIndex) const;
	void AI_changeGoldTradedTo(PlayerTypes eIndex, int iChange);

	int AI_getAttitudeExtra(PlayerTypes eIndex) const;
	void AI_setAttitudeExtra(PlayerTypes eIndex, int iNewValue);
	void AI_changeAttitudeExtra(PlayerTypes eIndex, int iChange);

	bool AI_isFirstContact(PlayerTypes eIndex) const;
	void AI_setFirstContact(PlayerTypes eIndex, bool bNewValue);

	int AI_getContactTimer(PlayerTypes eIndex1, ContactTypes eIndex2) const;
	void AI_changeContactTimer(PlayerTypes eIndex1, ContactTypes eIndex2, int iChange);

	int AI_getMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2) const;
	void AI_changeMemoryCount(PlayerTypes eIndex1, MemoryTypes eIndex2, int iChange);

	// K-Mod
	int AI_getCityTargetTimer() const;
	void AI_setCityTargetTimer(int iTurns);
	// K-Mod end

	int AI_calculateGoldenAgeValue(bool bConsiderRevolution = true) const;

	void AI_doCommerce();

	EventTypes AI_chooseEvent(int iTriggeredId) const;
	virtual void AI_launch(VictoryTypes eVictory);

	int AI_calculateCultureVictoryStage() const;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      03/17/10                                jdog5000      */
/*                                                                                              */
/* Victory Strategy AI       (functions renamed and edited for K-Mod)                           */
/************************************************************************************************/
	int AI_calculateSpaceVictoryStage() const;
	int AI_calculateConquestVictoryStage() const;
	int AI_calculateDominationVictoryStage() const;
	int AI_calculateDiplomacyVictoryStage() const;
	bool AI_isDoVictoryStrategy(int iVictoryStrategy) const;
	bool AI_isDoVictoryStrategyLevel4() const;
	bool AI_isDoVictoryStrategyLevel3() const;
	inline int AI_getVictoryStrategyHash() const { return m_iVictoryStrategyHash; }
	void AI_updateVictoryStrategyHash(); // K-Mod
	void AI_initStrategyRand(); // K-Mod
	int AI_getStrategyRand(int iShift) const;
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/		

	bool AI_isDoStrategy(int iStrategy) const;

	void AI_updateGreatPersonWeights(); // K-Mod
	int AI_getGreatPersonWeight(UnitClassTypes eGreatPerson) const; // K-Mod

	void AI_nowHasTech(TechTypes eTech);

	int AI_countDeadlockedBonuses(CvPlot* pPlot) const;

	//int AI_goldToUpgradeAllUnits(int iExpThreshold = 0) const;
	// K-Mod
	inline int AI_getGoldToUpgradeAllUnits() const { return m_iUpgradeUnitsCachedGold; }
	void AI_updateGoldToUpgradeAllUnits();
	inline int AI_getAvailableIncome() const { return m_iAvailableIncome; }
	void AI_updateAvailableIncome();
	int AI_estimateBreakEvenGoldPercent() const;
	// K-Mod end

	int AI_goldTradeValuePercent() const;

	int AI_averageYieldMultiplier(YieldTypes eYield) const;
	int AI_averageCommerceMultiplier(CommerceTypes eCommerce) const;
	int AI_averageGreatPeopleMultiplier() const;
	int AI_averageCulturePressure() const; // K-Mod
	int AI_averageCommerceExchange(CommerceTypes eCommerce) const;

	int AI_playerCloseness(PlayerTypes eIndex, int iMaxDistance) const;

	int AI_getTotalCityThreat() const;
	int AI_getTotalFloatingDefenseNeeded() const;


	int AI_getTotalAreaCityThreat(CvArea* pArea) const;
	int AI_countNumAreaHostileUnits(CvArea* pArea, bool bPlayer, bool bTeam, bool bNeutral, bool bHostile) const;
	int AI_getTotalFloatingDefendersNeeded(CvArea* pArea) const;
	int AI_getTotalFloatingDefenders(CvArea* pArea) const;
	int AI_getTotalAirDefendersNeeded() const; // K-Mod

	RouteTypes AI_bestAdvancedStartRoute(CvPlot* pPlot, int* piYieldValue = NULL) const;
	UnitTypes AI_bestAdvancedStartUnitAI(CvPlot* pPlot, UnitAITypes eUnitAI) const;
	CvPlot* AI_advancedStartFindCapitalPlot();

	bool AI_advancedStartPlaceExploreUnits(bool bLand);
	void AI_advancedStartRevealRadius(CvPlot* pPlot, int iRadius);
	bool AI_advancedStartPlaceCity(CvPlot* pPlot);
	bool AI_advancedStartDoRoute(CvPlot* pFromPlot, CvPlot* pToPlot);
	void AI_advancedStartRouteTerritory();
	void AI_doAdvancedStart(bool bNoExit = false);

	int AI_getMinFoundValue() const;

	void AI_recalculateFoundValues(int iX, int iY, int iInnerRadius, int iOuterRadius) const;

	void AI_updateCitySites(int iMinFoundValueThreshold, int iMaxSites);
	void AI_invalidateCitySites(int iMinFoundValueThreshold);

	int AI_getNumCitySites() const;
	bool AI_isPlotCitySite(CvPlot* pPlot) const;
	int AI_getNumAreaCitySites(int iAreaID, int& iBestValue) const;
	int AI_getNumAdjacentAreaCitySites(int iWaterAreaID, int iExcludeArea, int& iBestValue) const;
	int AI_getNumPrimaryAreaCitySites(int iMinimumValue = 0) const; // K-Mod
	CvPlot* AI_getCitySite(int iIndex) const;

	bool AI_deduceCitySite(const CvCity* pCity) const; // K-Mod
	int AI_countPotentialForeignTradeCities(bool bCheckConnected = true, bool bCheckForeignTradePolicy = true, CvArea* pIgnoreArea = 0) const; // K-Mod

	int AI_bestAreaUnitAIValue(UnitAITypes eUnitAI, CvArea* pArea, UnitTypes* peBestUnitType = NULL) const;
	int AI_bestCityUnitAIValue(UnitAITypes eUnitAI, CvCity* pCity, UnitTypes* peBestUnitType = NULL) const;

	int AI_calculateTotalBombard(DomainTypes eDomain) const;

	void AI_updateBonusValue(BonusTypes eBonus);
	void AI_updateBonusValue();

	int AI_getUnitClassWeight(UnitClassTypes eUnitClass) const;
	int AI_getUnitCombatWeight(UnitCombatTypes eUnitCombat) const;
	int AI_calculateUnitAIViability(UnitAITypes eUnitAI, DomainTypes eDomain) const;

	int AI_disbandValue(const CvUnit* pUnit, bool bMilitaryOnly = true) const; // K-Mod

	int AI_getAttitudeWeight(PlayerTypes ePlayer) const;

	ReligionTypes AI_chooseReligion();

	int AI_getPlotAirbaseValue(CvPlot* pPlot) const;
	int AI_getPlotCanalValue(CvPlot* pPlot) const;

	// Civ4 Reimagined: SubtractHappiness added
	int AI_getHappinessWeight(int iHappy, int iExtraPop, int iSubtractHappiness=0, bool bPercent=false) const;
	int AI_getHealthWeight(int iHealth, int iExtraPop, bool bPercent=false) const;

	bool AI_isPlotThreatened(CvPlot* pPlot, int iRange = -1, bool bTestMoves = true) const;

	bool AI_isFirstTech(TechTypes eTech) const;

	void AI_ClearConstructionValueCache(); // K-Mod

	// Civ4 Reimagined
	IdeologyTypes AI_bestIdeology(CivicTypes* paeCivics) const;

	// for serialization
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

protected:

	static CvPlayerAI* m_aPlayers;

	int m_iPeaceWeight;
	int m_iEspionageWeight;
	int m_iAttackOddsChange;
	int m_iCivicTimer;
	int m_iReligionTimer;
	int m_iExtraGoldTarget;
	int m_iCityTargetTimer; // K-Mod

	/* original bts code
	mutable int m_iStrategyHash;
	mutable int m_iStrategyHashCacheTurn;

	mutable int m_iAveragesCacheTurn;

	mutable int m_iAverageGreatPeopleMultiplier;

	mutable int *m_aiAverageYieldMultiplier;
	mutable int *m_aiAverageCommerceMultiplier;
	mutable int *m_aiAverageCommerceExchange;

	mutable int m_iUpgradeUnitsCacheTurn;
	mutable int m_iUpgradeUnitsCachedExpThreshold;
	mutable int m_iUpgradeUnitsCachedGold; */

	// K-Mod. The original caching method was just begging for OOS bugs.
	int m_iStrategyHash;
	// BBAI variables (adjusted for K-Mod)
	unsigned m_iStrategyRand;
	int m_iVictoryStrategyHash;
	// end BBAI

	int m_iAverageGreatPeopleMultiplier;

	int m_iAverageCulturePressure; // K-Mod culture pressure

	int *m_aiAverageYieldMultiplier;
	int *m_aiAverageCommerceMultiplier;
	int *m_aiAverageCommerceExchange;

	int m_iUpgradeUnitsCachedGold;
	int m_iAvailableIncome;
	// K-Mod end

	int *m_aiNumTrainAIUnits;
	int *m_aiNumAIUnits;
	int* m_aiSameReligionCounter;
	int* m_aiDifferentReligionCounter;
	int* m_aiFavoriteCivicCounter;
	int* m_aiBonusTradeCounter;
	int* m_aiPeacetimeTradeValue;
	int* m_aiPeacetimeGrantValue;
	int* m_aiGoldTradedTo;
	int* m_aiAttitudeExtra;
	int* m_aiBonusValue;
	int* m_aiAdditionalBonusValue; // Civ4 Reimagined
	int* m_aiUnitClassWeights;
	int* m_aiUnitCombatWeights;
	std::map<UnitClassTypes, int> m_GreatPersonWeights; // K-Mod

	//mutable int* m_aiCloseBordersAttitudeCache;
	std::vector<int> m_aiCloseBordersAttitudeCache; // K-Mod. (the original system was prone to mistakes.)

	std::vector<int> m_aiAttitudeCache; // K-Mod

	bool* m_abFirstContact;

	int** m_aaiContactTimer;
	int** m_aaiMemoryCount;

	std::vector<int> m_aiAICitySites;

	bool m_bWasFinancialTrouble;
	int m_iTurnLastProductionDirty;

	void AI_doCounter();
	void AI_doMilitary();
	void AI_doResearch();
	void AI_doCivics();
	void AI_doReligion();
	void AI_doDiplo();
	void AI_doSplit();
	void AI_doCheckFinancialTrouble();

	// K-Mod. I've moved the bulk of AI_getStrategyHash into a new function: AI_updateStrategyHash.
	inline int AI_getStrategyHash() const { return m_iStrategyHash; }
	void AI_updateStrategyHash();
	void AI_calculateAverages();

	void AI_convertUnitAITypesForCrush();
	int AI_eventValue(EventTypes eEvent, const EventTriggeredData& kTriggeredData) const;

	void AI_doEnemyUnitData();
	//void AI_invalidateCloseBordersAttitudeCache(); // disabled by K-Mod

	// Civ4 Reimagined
	int AI_unitDomainDistribution(const DomainTypes domain, const EraTypes era) const;

	friend class CvGameTextMgr;
};

// helper for accessing static functions
#ifdef _USRDLL
#define GET_PLAYER CvPlayerAI::getPlayer
#else
#define GET_PLAYER CvPlayerAI::getPlayerNonInl
#endif

#endif
