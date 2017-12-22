// Civ4 Reimagined
// continent.cpp

#include <algorithm>
#include <set>
#include <iterator>

#include "CvGameCoreDLL.h"
#include "CvArea.h"
#include "CvContinent.h"
#include "CvMap.h"
#include "CvPlot.h"
#include "CvGlobals.h"
#include "CvGameCoreUtils.h"
#include "CvInfos.h"

#include "CvDLLInterfaceIFaceBase.h"

// Public Functions...

CvArea::CvContinent()
{
	m_aiBuildingGoodHealth = new int[MAX_PLAYERS];
	m_aiBuildingBadHealth = new int[MAX_PLAYERS];
	m_aiBuildingHappiness = new int[MAX_PLAYERS];
	m_aiFreeSpecialist = new int[MAX_PLAYERS];
	m_aiDistanceMaintenanceModifier = new int[MAX_PLAYERS];
	m_aiCorporationMaintenanceModifier = new int[MAX_PLAYERS];
	m_aiPower = new int[MAX_PLAYERS];

	m_aaiYieldRateModifier = new int*[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_aaiYieldRateModifier[i] = new int[NUM_YIELD_TYPES];
	}
	m_aaiFreeBuilding = new int*[MAX_PLAYERS];
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		m_aaiFreeBuilding[i] = new int[GC.getNumBuildingInfos()];
	}

	CvWString m_szName = "";

	reset(0);
}


CvArea::~CvContinent()
{
	SAFE_DELETE_ARRAY(m_aiBuildingGoodHealth);
	SAFE_DELETE_ARRAY(m_aiBuildingBadHealth);
	SAFE_DELETE_ARRAY(m_aiBuildingHappiness);
	SAFE_DELETE_ARRAY(m_aiFreeSpecialist);
	SAFE_DELETE_ARRAY(m_aiDistanceMaintenanceModifier);
	SAFE_DELETE_ARRAY(m_aiCorporationMaintenanceModifier);
	SAFE_DELETE_ARRAY(m_aiPower);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		SAFE_DELETE_ARRAY(m_aaiYieldRateModifier[i]);
	}
	SAFE_DELETE_ARRAY(m_aaiYieldRateModifier);
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		SAFE_DELETE_ARRAY(m_aaiFreeBuilding[i]);
	}
	SAFE_DELETE_ARRAY(m_aaiFreeBuilding);
}


void CvContinent::init(int iID)
{
	//--------------------------------
	// Init saved data
	reset(iID);

	//--------------------------------
	// Init non-saved data

	//--------------------------------
	// Init other game data
}


// FUNCTION: reset()
// Initializes data members that are serialized.
void CvContinent::reset(int iID)
{
	int iI, iJ;

	m_iID = iID;
	m_iNumTiles = 0;
	CvWString m_szName = "";

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		m_aiBuildingGoodHealth[iI] = 0;
		m_aiBuildingBadHealth[iI] = 0;
		m_aiBuildingHappiness[iI] = 0;
		m_aiFreeSpecialist[iI] = 0;
		m_aiDistanceMaintenanceModifier[iI] = 0;
		m_aiCorporationMaintenanceModifier[iI] = 0;
		m_aiPower[iI] = 0;
	}

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		for (iJ = 0; iJ < NUM_YIELD_TYPES; iJ++)
		{
			m_aaiYieldRateModifier[iI][iJ] = 0;
		}

		for (int iBuilding = 0; iBuilding < GC.getNumBuildingInfos(); ++iBuilding)
		{
			m_aaiFreeBuilding[iI][iBuilding] = 0;
		}
	}
}


void CvContinent::addArea(int iAreaID)
{
	FAssert(!m_areas.contains(iAreaID));

	m_areas.insert(iAreaID);
}


void CvContinent::removeArea(int iAreaID)
{
	FAssert(m_areas.contains(iAreaID));

	m_areas.erase(iAreaID);
}


std::set<int> CvContinent::getAreas() const						
{
	return m_areas;
}


int CvContinent::getID() const						
{
	return m_iID;
}


void CvContinent::setID(int iID)														
{
	m_iID = iID;
}


CvWString CvContinent::getName() const
{
	return m_szName;
}


void CvContinent::setName(const CvWString &szNewValue)
{
	m_szName = szNewValue;
}


int CvContinent::getNumTiles() const
{
	return m_iNumTiles;
}


void CvContinent::changeNumTiles(int iChange)
{
	if (iChange != 0)
	{
		m_iNumTiles = (m_iNumTiles + iChange);
		FAssert(getNumTiles() >= 0);
	}
}


int CvContinent::getBuildingGoodHealth(PlayerTypes eIndex) const 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiBuildingGoodHealth[eIndex];
}


void CvContinent::changeBuildingGoodHealth(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");

	if (iChange != 0)
	{
		m_aiBuildingGoodHealth[eIndex] = (m_aiBuildingGoodHealth[eIndex] + iChange);
		FAssert(getBuildingGoodHealth(eIndex) >= 0);

		GET_PLAYER(eIndex).AI_makeAssignWorkDirty();
	}
}


int CvContinent::getBuildingBadHealth(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiBuildingBadHealth[eIndex];
}


void CvContinent::changeBuildingBadHealth(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");

	if (iChange != 0)
	{
		m_aiBuildingBadHealth[eIndex] = (m_aiBuildingBadHealth[eIndex] + iChange);
		FAssert(getBuildingBadHealth(eIndex) >= 0);

		GET_PLAYER(eIndex).AI_makeAssignWorkDirty();
	}
}


int CvContinent::getBuildingHappiness(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiBuildingHappiness[eIndex];
}


void CvContinent::changeBuildingHappiness(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");

	if (iChange != 0)
	{
		m_aiBuildingHappiness[eIndex] = (m_aiBuildingHappiness[eIndex] + iChange);

		GET_PLAYER(eIndex).AI_makeAssignWorkDirty();
	}
}


int CvContinent::getFreeSpecialist(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiFreeSpecialist[eIndex];
}


void CvContinent::changeFreeSpecialist(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");

	if (iChange != 0)
	{
		m_aiFreeSpecialist[eIndex] = (m_aiFreeSpecialist[eIndex] + iChange);
		FAssert(getFreeSpecialist(eIndex) >= 0);

		GET_PLAYER(eIndex).AI_makeAssignWorkDirty();
	}
}


int CvContinent::getDistanceMaintenanceModifier(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiDistanceMaintenanceModifier[eIndex];
}


void CvContinent::changeDistanceMaintenanceModifier(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");

	if (iChange != 0)
	{
		m_aiDistanceMaintenanceModifier[eIndex] = (m_aiDistanceMaintenanceModifier[eIndex] + iChange);
	}
}


int CvContinent::getCorporationMaintenanceModifier(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiCorporationMaintenanceModifier[eIndex];
}


void CvContinent::changeCorporationMaintenanceModifier(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");

	if (iChange != 0)
	{
		m_aiCorporationMaintenanceModifier[eIndex] = (m_aiCorporationMaintenanceModifier[eIndex] + iChange);
	}
}


int CvContinent::getFreeBuildingCount(PlayerTypes eIndex, BuildingTypes eBuilding) const 
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	FAssertMsg(eBuilding >= 0, "eBuilding is expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");
	return m_aaiFreeBuilding[eIndex][eBuilding];
}


void CvContinent::changeFreeBuildingCount(PlayerTypes eIndex, BuildingTypes eBuilding, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	FAssertMsg(eBuilding >= 0, "eBuilding is expected to be >= 0");
	FAssertMsg(eBuilding < GC.getNumBuildingInfos(), "eIndex is expected to be within maximum bounds (invalid Index)");

	if (iChange != 0)
	{
		m_aaiFreeBuilding[eIndex][eBuilding] = (m_aaiFreeBuilding[eIndex][eBuilding] + iChange);
		FAssert(getFreeBuildingCount(eIndex, eBuilding) >= 0);

		GET_PLAYER(eIndex).changeAreaFreeBuildings(eBuilding, getID(), iChange);
	}
}


int CvContinent::getPower(PlayerTypes eIndex) const
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	return m_aiPower[eIndex];
}


void CvContinent::changePower(PlayerTypes eIndex, int iChange)
{
	FAssertMsg(eIndex >= 0, "eIndex is expected to be >= 0");
	FAssertMsg(eIndex < MAX_PLAYERS, "eIndex is expected to be < MAX_PLAYERS");
	m_aiPower[eIndex] = (m_aiPower[eIndex] + iChange);
	FAssert(getPower(eIndex) >= 0);
}


int CvContinent::getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be < NUM_YIELD_TYPES");
	return m_aaiYieldRateModifier[eIndex1][eIndex2];
}


void CvContinent::changeYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2, int iChange)
{
	FAssertMsg(eIndex1 >= 0, "eIndex1 is expected to be >= 0");
	FAssertMsg(eIndex1 < MAX_PLAYERS, "eIndex1 is expected to be < MAX_PLAYERS");
	FAssertMsg(eIndex2 >= 0, "eIndex2 is expected to be >= 0");
	FAssertMsg(eIndex2 < NUM_YIELD_TYPES, "eIndex2 is expected to be < NUM_YIELD_TYPES");

	if (iChange != 0)
	{
		m_aaiYieldRateModifier[eIndex1][eIndex2] = (m_aaiYieldRateModifier[eIndex1][eIndex2] + iChange);
		
		GET_PLAYER(eIndex1).invalidateYieldRankCache(eIndex2);

		if (eIndex2 == YIELD_COMMERCE)
		{
			GET_PLAYER(eIndex1).updateCommerce();
		}

		GET_PLAYER(eIndex1).AI_makeAssignWorkDirty();

		if (GET_PLAYER(eIndex1).getTeam() == GC.getGameINLINE().getActiveTeam())
		{
			gDLL->getInterfaceIFace()->setDirty(CityInfo_DIRTY_BIT, true);
		}
	}
}


void CvArea::read(FDataStreamBase* pStream)
{
	int iI;
	int iNumAreas;

	// Init saved data
	reset();

	uint uiFlag=0;
	pStream->Read(&uiFlag);	// flags for expansion

	pStream->Read(&m_iID);
	pStream->Read(&m_iNumTiles);

	pStream->ReadString(m_szName);

	pStream->Read(MAX_PLAYERS, m_aiBuildingGoodHealth);
	pStream->Read(MAX_PLAYERS, m_aiBuildingBadHealth);
	pStream->Read(MAX_PLAYERS, m_aiBuildingHappiness);
	pStream->Read(MAX_PLAYERS, m_aiFreeSpecialist);
	pStream->Read(MAX_PLAYERS, m_aiDistanceMaintenanceModifier);
	pStream->Read(MAX_PLAYERS, m_aiCorporationMaintenanceModifier);
	pStream->Read(MAX_PLAYERS, m_aiPower);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pStream->Read(NUM_YIELD_TYPES, m_aaiYieldRateModifier[iI]);
	}
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pStream->Read(GC.getNumBuildingInfos(), m_aaiFreeBuilding[iI]);
	}

	pStream->Read(&iNumAreas);
	m_areas.clear();
	for (int i = 0; i < iNumAreas; ++i)
	{
		int iAreaID;
		pStream->Read((int*)&iAreaID);
		m_areas.insert(iAreaID);
	}
}


void CvArea::write(FDataStreamBase* pStream)
{
	int iI;

	uint uiFlag=0;
	pStream->Write(uiFlag);		// flag for expansion

	pStream->Write(m_iID);
	pStream->Write(m_iNumTiles);

	pStream->WriteString(m_szName);

	pStream->Write(MAX_PLAYERS, m_aiBuildingGoodHealth);
	pStream->Write(MAX_PLAYERS, m_aiBuildingBadHealth);
	pStream->Write(MAX_PLAYERS, m_aiBuildingHappiness);
	pStream->Write(MAX_PLAYERS, m_aiFreeSpecialist);
	pStream->Write(MAX_PLAYERS, m_aiDistanceMaintenanceModifier);
	pStream->Write(MAX_PLAYERS, m_aiCorporationMaintenanceModifier);
	pStream->Write(MAX_PLAYERS, m_aiPower);

	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pStream->Write(NUM_YIELD_TYPES, m_aaiYieldRateModifier[iI]);
	}
	for (iI = 0; iI < MAX_PLAYERS; iI++)
	{
		pStream->Write(GC.getNumBuildingInfos(), m_aaiFreeBuilding[iI]);
	}

	pStream->Write(m_areas.size());
	for (std::set<int>::iterator it = m_areas.begin(); it != m_areas.end(); ++it)
	{
		pStream->Write(*it);
	}
}

// Protected Functions...
