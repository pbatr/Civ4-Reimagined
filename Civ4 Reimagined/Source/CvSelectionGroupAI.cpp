// selectionGroupAI.cpp

#include "CvGameCoreDLL.h"
#include "CvSelectionGroupAI.h"
#include "CvPlayerAI.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvTeamAI.h"
#include "CvDLLEntityIFaceBase.h"
#include "CvGameCoreUtils.h"
#include "FProfiler.h"
#include "CVInfos.h"
// Civ4 Reimagined
#include "BetterBTSAI.h"

// Public Functions...

CvSelectionGroupAI::CvSelectionGroupAI()
{
	AI_reset();
}


CvSelectionGroupAI::~CvSelectionGroupAI()
{
	AI_uninit();
}


void CvSelectionGroupAI::AI_init()
{
	AI_reset();

	//--------------------------------
	// Init other game data
}


void CvSelectionGroupAI::AI_uninit()
{
}


void CvSelectionGroupAI::AI_reset()
{
	AI_uninit();

	m_iMissionAIX = INVALID_PLOT_COORD;
	m_iMissionAIY = INVALID_PLOT_COORD;

	m_bForceSeparate = false;

	m_eMissionAIType = NO_MISSIONAI;

	m_missionAIUnit.reset();

	m_bGroupAttack = false;
	m_iGroupAttackX = -1;
	m_iGroupAttackY = -1;
}

// these separate function have been tweaked by K-Mod and bbai.
void CvSelectionGroupAI::AI_separate()
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;

	pEntityNode = headUnitNode();

	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);

		pLoopUnit->joinGroup(NULL);
	}
}

void CvSelectionGroupAI::AI_separateNonAI(UnitAITypes eUnitAI)
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;

	pEntityNode = headUnitNode();

	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		if (pLoopUnit->AI_getUnitAIType() != eUnitAI)
		{
			pLoopUnit->joinGroup(NULL);
		}
	}
}

void CvSelectionGroupAI::AI_separateAI(UnitAITypes eUnitAI)
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;

	pEntityNode = headUnitNode();

	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
		{
			pLoopUnit->joinGroup(NULL);
		}
	}
}

bool CvSelectionGroupAI::AI_separateImpassable()
{
	CvPlayerAI& kPlayer = GET_PLAYER(getOwner());
	bool bSeparated = false;

	CLLNode<IDInfo>* pEntityNode = headUnitNode();

	while (pEntityNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		if (kPlayer.AI_unitImpassableCount(pLoopUnit->getUnitType()) > 0)
		{
			pLoopUnit->joinGroup(NULL);
			bSeparated = true;
		}
	}
	return bSeparated;
}

bool CvSelectionGroupAI::AI_separateEmptyTransports()
{
	bool bSeparated = false;

	CLLNode<IDInfo>* pEntityNode = headUnitNode();

	while (pEntityNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		if ((pLoopUnit->AI_getUnitAIType() == UNITAI_ASSAULT_SEA) && (pLoopUnit->getCargo() == 0))
		{
			pLoopUnit->joinGroup(NULL);
			bSeparated = true;
		}
	}
	return bSeparated;
}
// bbai / K-Mod


// Returns true if the group has become busy...
bool CvSelectionGroupAI::AI_update()
{
	PROFILE_FUNC();

	FAssert(getOwnerINLINE() != NO_PLAYER);

	if (!AI_isControlled())
	{
		return false;
	}

	if (getNumUnits() == 0)
	{
		return false;
	}

	// K-Mod. (replacing the original "isForceUpdate" stuff.)
	if (isForceUpdate())
		AI_cancelGroupAttack(); // note: we haven't toggled the update flag, nor woken the group from sleep.
	// K-Mod end

	//FAssert(!(GET_PLAYER(getOwnerINLINE()).isAutoMoves())); // (no longer true in K-Mod)

	int iTempHack = 0; // XXX

	bool bDead = false;
	
	bool bFailedAlreadyFighting = false;
	//while ((m_bGroupAttack && !bFailedAlreadyFighting) || readyToMove())
	while ((AI_isGroupAttack() && !isBusy()) || readyToMove()) // K-Mod
	{
		setForceUpdate(false); // K-Mod. Force update just means we should get into this loop at least once.

		iTempHack++;
		if (iTempHack > 100)
		{
			FAssertMsg(false, "unit stuck in a loop");
			CvUnit* pHeadUnit = getHeadUnit();
			if (NULL != pHeadUnit)
			{
				if (gUnitLogLevel >= 1) {
					// Civ4 Reimagined
					logBBAI("Unit stuck in loop: %S(%S)[%d, %d]\n", pHeadUnit->getName().GetCString(), GET_PLAYER(pHeadUnit->getOwnerINLINE()).getName(),
						pHeadUnit->getX_INLINE(), pHeadUnit->getY_INLINE());
				}
				if (GC.getLogging())
				{
					TCHAR szOut[1024];
					CvWString szTempString;
					getUnitAIString(szTempString, pHeadUnit->AI_getUnitAIType());
					sprintf(szOut, "Unit stuck in loop: %S(%S)[%d, %d] (%S)\n", pHeadUnit->getName().GetCString(), GET_PLAYER(pHeadUnit->getOwnerINLINE()).getName(),
						pHeadUnit->getX_INLINE(), pHeadUnit->getY_INLINE(), szTempString.GetCString());
					gDLL->messageControlLog(szOut);
				}
				
				pHeadUnit->finishMoves();
			}
			break;
		}

		// if we want to force the group to attack, force another attack
		if (AI_isGroupAttack())
		{			
			AI_cancelGroupAttack();

			groupAttack(m_iGroupAttackX, m_iGroupAttackY, MOVE_DIRECT_ATTACK, bFailedAlreadyFighting);
		}
		// else pick AI action
		else
		{
			CvUnit* pHeadUnit = getHeadUnit();

			//if (pHeadUnit == NULL || pHeadUnit->isDelayedDeath())
			if (pHeadUnit == NULL || pHeadUnit->doDelayedDeath()) // K-Mod
			{
				break;
			}

			//resetPath();

			if (pHeadUnit->AI_update())
			{
				// AI_update returns true when we should abort the loop and wait until next slice
				FAssert(!pHeadUnit->isDelayedDeath());
				break;
			}
		}

		if (doDelayedDeath())
		{
			bDead = true;
			break;
		}

		// if no longer group attacking, and force separate is true, then bail, decide what to do after group is split up
		// (UnitAI of head unit may have changed)
		if (!AI_isGroupAttack() && AI_isForceSeparate())
		{
			AI_separate();	// pointers could become invalid...
			//return true;
			return false; // K-Mod
		}
	}

	if (!bDead)
	{
		// K-Mod. this is how we deal with force update when some group memebers can't move.
		if (isForceUpdate())
		{
			setForceUpdate(false);
			AI_cancelGroupAttack();
			setActivityType(ACTIVITY_AWAKE);
		}
		// K-Mod end
		if (!isHuman())
		{
			bool bFollow = false;

			// if we not group attacking, then check for follow action
			if (!AI_isGroupAttack())
			{
				CLLNode<IDInfo>* pEntityNode = headUnitNode();
				// K-Mod note: I've rearranged a few things below, and added 'bFirst'.
				bool bFirst = true;

				while ((pEntityNode != NULL) && readyToMove(true))
				{
					CvUnit* pLoopUnit = ::getUnit(pEntityNode->m_data);
					pEntityNode = nextUnitNode(pEntityNode);

					if (bFirst)
						path_finder.Reset();

					if (pLoopUnit->canMove())
					{
						if (pLoopUnit->AI_follow(bFirst))
						{
							bFollow = true;
							bFirst = true; // let the next unit start fresh.
						}
						else
							bFirst = false;
					}
				}
				// K-Mod end
			}

			if (doDelayedDeath())
			{
				bDead = true;
			}

			if (!bDead)
			{
				if (!bFollow && readyToMove(true))
				{
					pushMission(MISSION_SKIP);
				}
			}
		}
	}

	if (bDead)
	{
		//return true;
		return false; // K-Mod
	}

	return (isBusy() || isCargoBusy());
}


// Returns attack odds out of 100 (the higher, the better...)
int CvSelectionGroupAI::AI_attackOdds(const CvPlot* pPlot, bool bPotentialEnemy) const
{
	PROFILE_FUNC();

	CvUnit* pAttacker;

	FAssert(getOwnerINLINE() != NO_PLAYER);

/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      02/21/10                                jdog5000      */
/*                                                                                              */
/* Efficiency, Lead From Behind                                                                 */
/************************************************************************************************/
	// From Lead From Behind by UncutDragon
	// original
	//if (pPlot->getBestDefender(NO_PLAYER, getOwnerINLINE(), NULL, !bPotentialEnemy, bPotentialEnemy) == NULL)
	// modified
	if (!pPlot->hasDefender(false, NO_PLAYER, getOwnerINLINE(), NULL, !bPotentialEnemy, bPotentialEnemy))
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
	{
		return 100;
	}

	int iOdds = 0;
	pAttacker = AI_getBestGroupAttacker(pPlot, bPotentialEnemy, iOdds);

	if (pAttacker == NULL)
	{
		return 0;
	}

	return iOdds;
}


CvUnit* CvSelectionGroupAI::AI_getBestGroupAttacker(const CvPlot* pPlot, bool bPotentialEnemy, int& iUnitOdds, bool bForce, bool bNoBlitz) const
{
	PROFILE_FUNC();

	int iBestValue = 0;
	int iBestOdds = 0;
	CvUnit* pBestUnit = NULL;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();

	bool bIsHuman = (pUnitNode != NULL) ? GET_PLAYER(::getUnit(pUnitNode->m_data)->getOwnerINLINE()).isHuman() : true;

	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!pLoopUnit->isDead())
		{
			bool bCanAttack = false;
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				bCanAttack = pLoopUnit->canAirAttack();
			}
			else
			{
				bCanAttack = pLoopUnit->canAttack();

				if (bCanAttack && bNoBlitz && pLoopUnit->isBlitz() && pLoopUnit->isMadeAttack())
				{
					bCanAttack = false;
				}
			}

			if (bCanAttack)
			{
				if (bForce || pLoopUnit->canMove())
				{
					if (bForce || pLoopUnit->canMoveInto(pPlot, /*bAttack*/ true, /*bDeclareWar*/ bPotentialEnemy))
					{
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                      02/21/10                                jdog5000      */
/*                                                                                              */
/* Lead From Behind                                                                             */
/************************************************************************************************/
						// From Lead From Behind by UncutDragon
						if (GC.getLFBEnable() && GC.getLFBUseCombatOdds())
						{
							//pLoopUnit->LFBgetBetterAttacker(&pBestUnit, pPlot, bPotentialEnemy, iBestOdds, iValue);
							pLoopUnit->LFBgetBetterAttacker(&pBestUnit, pPlot, bPotentialEnemy, iBestOdds, iBestValue); // K-Mod.
						} 
						else 
						{
							int iOdds = pLoopUnit->AI_attackOdds(pPlot, bPotentialEnemy);

							int iValue = iOdds;
							FAssertMsg(iValue > 0, "iValue is expected to be greater than 0");

							if (pLoopUnit->collateralDamage() > 0)
							{
								int iPossibleTargets = std::min((pPlot->getNumVisibleEnemyDefenders(pLoopUnit) - 1), pLoopUnit->collateralDamageMaxUnits());

								if (iPossibleTargets > 0)
								{
									iValue *= (100 + ((pLoopUnit->collateralDamage() * iPossibleTargets) / 5));
									iValue /= 100;
								}
							}

							// if non-human, prefer the last unit that has the best value (so as to avoid splitting the group)
							if (iValue > iBestValue || (!bIsHuman && iValue > 0 && iValue == iBestValue))
							{
								iBestValue = iValue;
								iBestOdds = iOdds;
								pBestUnit = pLoopUnit;
							}
						}
/************************************************************************************************/
/* BETTER_BTS_AI_MOD                       END                                                  */
/************************************************************************************************/
					}
				}
			}
		}
	}

	iUnitOdds = iBestOdds;
	return pBestUnit;
}

CvUnit* CvSelectionGroupAI::AI_getBestGroupSacrifice(const CvPlot* pPlot, bool bPotentialEnemy, bool bForce, bool bNoBlitz) const
{
	int iBestValue = 0;
	CvUnit* pBestUnit = NULL;

	CLLNode<IDInfo>* pUnitNode = headUnitNode();
	while (pUnitNode != NULL)
	{
		CvUnit* pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!pLoopUnit->isDead())
		{
			bool bCanAttack = false;
			if (pLoopUnit->getDomainType() == DOMAIN_AIR)
			{
				bCanAttack = pLoopUnit->canAirAttack();
			}
			else
			{
				bCanAttack = pLoopUnit->canAttack();

				if (bCanAttack && bNoBlitz && pLoopUnit->isBlitz() && pLoopUnit->isMadeAttack())
				{
					bCanAttack = false;
				}
			}

			if (bCanAttack)
			{
				if (bForce || pLoopUnit->canMove())
				{
					if (bForce || pLoopUnit->canMoveInto(pPlot, true))
					{
                        int iValue = pLoopUnit->AI_sacrificeValue(pPlot);
						FAssertMsg(iValue > 0, "iValue is expected to be greater than 0");

						// we want to pick the last unit of highest value, so pick the last unit with a good value
						if (iValue >= iBestValue)
						{
							iBestValue = iValue;
							pBestUnit = pLoopUnit;
						}
					}
				}
			}
		}
	}

	return pBestUnit;
}

// Returns ratio of strengths of stacks times 100
// (so 100 is even ratio, numbers over 100 mean this group is more powerful than the stack on a plot)
int CvSelectionGroupAI::AI_compareStacks(const CvPlot* pPlot, bool bCheckCanAttack) const
{
	FAssert(pPlot != NULL);

	int	compareRatio;
	DomainTypes eDomainType = getDomainType();

	// if not aircraft, then choose based on the plot, not the head unit (mainly for transport carried units)
	if (eDomainType != DOMAIN_AIR)
	{
		if (pPlot->isWater())
			eDomainType = DOMAIN_SEA;
		else
			eDomainType = DOMAIN_LAND;
	}

	compareRatio = AI_sumStrength(pPlot, eDomainType, bCheckCanAttack);
	compareRatio *= 100;

	PlayerTypes eOwner = getOwnerINLINE();
	if (eOwner == NO_PLAYER)
	{
		eOwner = getHeadOwner();
	}
	FAssert(eOwner != NO_PLAYER);
	
	// K-Mod. Note. This function currently does not support bPotentialEnemy == false.
	//FAssert(bPotentialEnemy);
	int defenderSum = pPlot->isVisible(getHeadTeam(), false)
		? GET_PLAYER(eOwner).AI_localDefenceStrength(pPlot, NO_TEAM, eDomainType, 0)
		: GET_TEAM(getHeadTeam()).AI_getStrengthMemory(pPlot);

	// Civ4 Reimagined: Mongol UP
	CvCity* pDefenderCity = pPlot->getPlotCity();
	if (pDefenderCity != NULL && GET_PLAYER(eOwner).isCityRevoltOnKill() && pDefenderCity->getDefenseModifier(false) > 0 && pDefenderCity->getCultureLevel() < (CultureLevelTypes)5)
	{
		defenderSum *= 100 - pDefenderCity->getDefenseModifier(false) / 2;
		defenderSum /= 100;
	}

	// K-Mod end
	compareRatio /= std::max(1, defenderSum);

	// K-Mod. If there are more defenders than we have attacks, but yet the ratio is still greater than 100,
	// then inflate the ratio futher to account for the fact that we are going to do significantly more damage to them than they to us.
	// The purpose of this is to give the AI extra encouragement to attack when its units are better than the defender's units.
	/* if (compareRatio > 100)
	{
		FAssert(getHeadUnit() && getNumUnits() > 0);
		int iDefenders = pPlot->getNumVisibleEnemyDefenders(getHeadUnit());
		if (iDefenders > getNumUnits())
		{
			compareRatio += (compareRatio - 100) * (iDefenders - getNumUnits()) / getNumUnits();
		}
	} */ // (currently disabled)
	// K-Mod end

	return compareRatio;
}

// K-Mod. I've removed bCheckMove, and changed bCheckCanAttack to include checks for moves, and for hasAlreadyAttacked / blitz
int CvSelectionGroupAI::AI_sumStrength(const CvPlot* pAttackedPlot, DomainTypes eDomainType, bool bCheckCanAttack) const
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;
	int	strSum = 0;
	bool bDefenders = pAttackedPlot ? pAttackedPlot->isVisibleEnemyUnit(getHeadOwner()) : false; // K-Mod
	bool bCountCollateral = pAttackedPlot && pAttackedPlot != plot(); // K-Mod

	pUnitNode = headUnitNode();

	int iBaseCollateral = bCountCollateral
		? estimateCollateralWeight(pAttackedPlot, getTeam())
		: 0;

	while (pUnitNode != NULL)
	{
		pLoopUnit = ::getUnit(pUnitNode->m_data);
		pUnitNode = nextUnitNode(pUnitNode);

		if (!pLoopUnit->isDead())
		{
			// K-Mod. (original checks deleted.)
			if (bCheckCanAttack)
			{
				if (pLoopUnit->getDomainType() == DOMAIN_AIR)
				{
					if (!pLoopUnit->canAirAttack() || !pLoopUnit->canMove() || (pAttackedPlot && bDefenders && !pLoopUnit->canMoveInto(pAttackedPlot, true, true)))
						continue; // can't attack.
				}
				else
				{
					if (!pLoopUnit->canAttack() || !pLoopUnit->canMove()
						|| (pAttackedPlot && bDefenders && !pLoopUnit->canMoveInto(pAttackedPlot, true, true))
						|| (!pLoopUnit->isBlitz() && pLoopUnit->isMadeAttack()))
						continue; // can't attack.
				}
			}
			// K-Mod end

			if (eDomainType == NO_DOMAIN || pLoopUnit->getDomainType() == eDomainType)
			{
				// strSum += pLoopUnit->currEffectiveStr(pAttackedPlot, pLoopUnit);

				// K-Mod estimate the value of first strike, and the attack power of collateral units.
				// (cf with calculation in CvPlayerAI::AI_localAttackStrength)
				int iUnitStr = pLoopUnit->currEffectiveStr(pAttackedPlot, pLoopUnit);
				iUnitStr *= 100 + 4 * pLoopUnit->firstStrikes() + 2 * pLoopUnit->chanceFirstStrikes();
				iUnitStr /= 100;

				if (bCountCollateral && pLoopUnit->collateralDamage() > 0)
				{
					int iPossibleTargets = pLoopUnit->collateralDamageMaxUnits();
					// If !bCheckCanAttack, then lets not assume pAttackPlot won't get more units on it.
					if (bCheckCanAttack && pAttackedPlot->isVisible(getTeam(), false))
						iPossibleTargets = std::min(iPossibleTargets, pAttackedPlot->getNumVisibleEnemyDefenders(pLoopUnit) - 1);

					if (iPossibleTargets > 0)
					{
						// collateral damage is not trivial to calculate. This estimate is pretty rough.
						// (Note: collateralDamage() and iBaseCollateral both include factors of 100.)
						iUnitStr += pLoopUnit->baseCombatStr() * iBaseCollateral * pLoopUnit->collateralDamage() * iPossibleTargets / 10000;
					}
				}
				strSum += iUnitStr;
				// K-Mod end
			}
		}
	}

	return strSum;
}

void CvSelectionGroupAI::AI_queueGroupAttack(int iX, int iY)
{
	m_bGroupAttack = true;

	m_iGroupAttackX = iX;
	m_iGroupAttackY = iY;
}

bool CvSelectionGroupAI::AI_isControlled()
{
	return (!isHuman() || isAutomated());
}


bool CvSelectionGroupAI::AI_isDeclareWar(const CvPlot* pPlot)
{
	FAssert(getHeadUnit() != NULL);

	if (isHuman())
	{
		return false;
	}
	else
	{
		// K-Mod
		if (AI_getMissionAIType() == MISSIONAI_EXPLORE)
			return false;
		// K-Mod end

		bool bLimitedWar = false;
		if (pPlot != NULL)
		{
			TeamTypes ePlotTeam = pPlot->getTeam();
			if (ePlotTeam != NO_TEAM)
			{
				WarPlanTypes eWarplan = GET_TEAM(getTeam()).AI_getWarPlan(ePlotTeam);
				if (eWarplan == WARPLAN_LIMITED)
				{
					bLimitedWar = true;
				}
			}
		}

		CvUnit* pHeadUnit = getHeadUnit();

		if (pHeadUnit != NULL)
		{
			switch (pHeadUnit->AI_getUnitAIType())
			{
			case UNITAI_UNKNOWN:
			case UNITAI_ANIMAL:
			case UNITAI_SETTLE:
			case UNITAI_WORKER:
				break;
			case UNITAI_ATTACK_CITY:
			case UNITAI_ATTACK_CITY_LEMMING:
				return true;
				break;

			case UNITAI_ATTACK:
			case UNITAI_COLLATERAL:
			case UNITAI_PILLAGE:
				if (bLimitedWar)
				{
					return true;
				}
				break;
				
			case UNITAI_PARADROP:
			case UNITAI_RESERVE:
			case UNITAI_COUNTER:
			case UNITAI_CITY_DEFENSE:
			case UNITAI_CITY_COUNTER:
			case UNITAI_CITY_SPECIAL:
			case UNITAI_EXPLORE:
			case UNITAI_MISSIONARY:
			case UNITAI_PROPHET:
			case UNITAI_ARTIST:
			case UNITAI_SCIENTIST:
			case UNITAI_GENERAL:
			case UNITAI_MERCHANT:
			case UNITAI_ENGINEER:
			case UNITAI_GREAT_SPY: // K-Mod
			case UNITAI_SPY:
			case UNITAI_ICBM:
			case UNITAI_WORKER_SEA:
				break;

			case UNITAI_ATTACK_SEA:
			case UNITAI_RESERVE_SEA:
			case UNITAI_ESCORT_SEA:
				if (bLimitedWar)
				{
					return true;
				}
				break;
			case UNITAI_EXPLORE_SEA:
				break;

			case UNITAI_ASSAULT_SEA:
				if (hasCargo())
				{
					return true;
				}
				break;

			case UNITAI_SETTLER_SEA:
			case UNITAI_MISSIONARY_SEA:
			case UNITAI_SPY_SEA:
			case UNITAI_CARRIER_SEA:
			case UNITAI_MISSILE_CARRIER_SEA:
			case UNITAI_PIRATE_SEA:
			case UNITAI_ATTACK_AIR:
			case UNITAI_DEFENSE_AIR:
			case UNITAI_CARRIER_AIR:
			case UNITAI_MISSILE_AIR:
				break;

			default:
				FAssert(false);
				break;
			}
		}
	}

	return false;
}


CvPlot* CvSelectionGroupAI::AI_getMissionAIPlot()
{
	return GC.getMapINLINE().plotSorenINLINE(m_iMissionAIX, m_iMissionAIY);
}


bool CvSelectionGroupAI::AI_isForceSeparate()
{
	return m_bForceSeparate;
}


/* void CvSelectionGroupAI::AI_makeForceSeparate()
{
	m_bForceSeparate = true;
} */


MissionAITypes CvSelectionGroupAI::AI_getMissionAIType() const
{
	return m_eMissionAIType;
}


void CvSelectionGroupAI::AI_setMissionAI(MissionAITypes eNewMissionAI, CvPlot* pNewPlot, CvUnit* pNewUnit)
{
	//PROFILE_FUNC();

	m_eMissionAIType = eNewMissionAI;

	if (pNewPlot != NULL)
	{
		m_iMissionAIX = pNewPlot->getX_INLINE();
		m_iMissionAIY = pNewPlot->getY_INLINE();
	}
	else
	{
		m_iMissionAIX = INVALID_PLOT_COORD;
		m_iMissionAIY = INVALID_PLOT_COORD;
	}

	if (pNewUnit != NULL)
	{
		m_missionAIUnit = pNewUnit->getIDInfo();
	}
	else
	{
		m_missionAIUnit.reset();
	}
}


CvUnit* CvSelectionGroupAI::AI_getMissionAIUnit()
{
	return getUnit(m_missionAIUnit);
}

bool CvSelectionGroupAI::AI_isFull()
{
	CLLNode<IDInfo>* pUnitNode;
	CvUnit* pLoopUnit;

	if (getNumUnits() > 0)
	{
		UnitAITypes eUnitAI = getHeadUnitAI();
		// do two passes, the first pass, we ignore units with speical cargo
		int iSpecialCargoCount = 0;
		int iCargoCount = 0;
		
		// first pass, count but ignore special cargo units
		pUnitNode = headUnitNode();

		while (pUnitNode != NULL)
		{
			pLoopUnit = ::getUnit(pUnitNode->m_data);
			pUnitNode = nextUnitNode(pUnitNode);
			if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
			{
				if (pLoopUnit->cargoSpace() > 0)
				{
					iCargoCount++;
				}

				if (pLoopUnit->specialCargo() != NO_SPECIALUNIT)
				{
					iSpecialCargoCount++;
				}
				else if (!(pLoopUnit->isFull()))
				{
					return false;
				}
			}
		}
		
		// if every unit in the group has special cargo, then check those, otherwise, consider ourselves full
		if (iSpecialCargoCount >= iCargoCount)
		{
			pUnitNode = headUnitNode();
			while (pUnitNode != NULL)
			{
				pLoopUnit = ::getUnit(pUnitNode->m_data);
				pUnitNode = nextUnitNode(pUnitNode);
				
				if (pLoopUnit->AI_getUnitAIType() == eUnitAI)
				{
					if (!(pLoopUnit->isFull()))
					{
						return false;
					}
				}
			}
		}

		return true;
	}

	return false;	
}


CvUnit* CvSelectionGroupAI::AI_ejectBestDefender(CvPlot* pDefendPlot)
{
	CLLNode<IDInfo>* pEntityNode;
	CvUnit* pLoopUnit;

	pEntityNode = headUnitNode();
	
	CvUnit* pBestUnit = NULL;
	int iBestUnitValue = 0;

	while (pEntityNode != NULL)
	{
		pLoopUnit = ::getUnit(pEntityNode->m_data);
		pEntityNode = nextUnitNode(pEntityNode);
		
		//if (!pLoopUnit->noDefensiveBonus())
		// commented out by K-Mod. The noDefBonus thing is already taken into account.
		{
			int iValue = pLoopUnit->currEffectiveStr(pDefendPlot, NULL) * 100;
			
			if (pDefendPlot->isCity(true, getTeam()))
			{
				iValue *= 100 + pLoopUnit->cityDefenseModifier();
				iValue /= 100;
			}

			iValue *= 100;
			iValue /= (100 + pLoopUnit->cityAttackModifier() + pLoopUnit->getExtraCityAttackPercent());
			
			iValue /= 2 + pLoopUnit->getLevel();
			
			if (iValue > iBestUnitValue)
			{
				iBestUnitValue = iValue;
				pBestUnit = pLoopUnit;
			}
		}
	}
	
	if (NULL != pBestUnit && getNumUnits() > 1)
	{
		pBestUnit->joinGroup(NULL);
	}
	
	return pBestUnit;
}


// Protected Functions...

void CvSelectionGroupAI::read(FDataStreamBase* pStream)
{
	CvSelectionGroup::read(pStream);

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(&m_iMissionAIX);
	pStream->Read(&m_iMissionAIY);

	pStream->Read(&m_bForceSeparate);

	pStream->Read((int*)&m_eMissionAIType);

	pStream->Read((int*)&m_missionAIUnit.eOwner);
	pStream->Read(&m_missionAIUnit.iID);

	pStream->Read(&m_bGroupAttack);
	pStream->Read(&m_iGroupAttackX);
	pStream->Read(&m_iGroupAttackY);
}


void CvSelectionGroupAI::write(FDataStreamBase* pStream)
{
	CvSelectionGroup::write(pStream);

	uint uiFlag=0;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_iMissionAIX);
	pStream->Write(m_iMissionAIY);

	pStream->Write(m_bForceSeparate);

	pStream->Write(m_eMissionAIType);

	pStream->Write(m_missionAIUnit.eOwner);
	pStream->Write(m_missionAIUnit.iID);

	pStream->Write(m_bGroupAttack);
	pStream->Write(m_iGroupAttackX);
	pStream->Write(m_iGroupAttackY);
}

// Private Functions...
