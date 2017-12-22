#pragma once

// Civ4 Reimagined
// continent.h

#ifndef CIV4_CONTINENT_H
#define CIV4_CONTINENT_H

#include <set>

class CvContinent
{

public:
	CvContinent();
	virtual ~CvContinent();

	void init(int iID);
	void reset(int iID = 0);

	void addArea(int iAreaID);
	void removeArea(int iAreaID);
	std::set<int> getAreas() const;

	int getID() const;
	void setID(int iID);

	CvWString getName() const;
	void setName(const CvWString &szNewValue);

	int getNumTiles() const;

	int getBuildingGoodHealth(PlayerTypes eIndex) const;
	void changeBuildingGoodHealth(PlayerTypes eIndex, int iChange);

	int getBuildingBadHealth(PlayerTypes eIndex) const;
	void changeBuildingBadHealth(PlayerTypes eIndex, int iChange);

	int getBuildingHappiness(PlayerTypes eIndex) const;
	void changeBuildingHappiness(PlayerTypes eIndex, int iChange);

	int getFreeSpecialist(PlayerTypes eIndex) const;
	void changeFreeSpecialist(PlayerTypes eIndex, int iChange);

	int getDistanceMaintenanceModifier(PlayerTypes eIndex) const;
	void changeDistanceMaintenanceModifier(PlayerTypes eIndex, int iChange);
	int getCorporationMaintenanceModifier(PlayerTypes eIndex) const;
	void changeCorporationMaintenanceModifier(PlayerTypes eIndex, int iChange);

	int getFreeBuildingCount(PlayerTypes eIndex, BuildingTypes eBuilding) const;
	void changeFreeBuildingCount(PlayerTypes eIndex, BuildingTypes eBuilding, int iChange);

	int getPower(PlayerTypes eIndex) const;
	void changePower(PlayerTypes eIndex, int iChange);

	int getYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2) const;
	void changeYieldRateModifier(PlayerTypes eIndex1, YieldTypes eIndex2, int iChange);

protected:

	int m_iID;
	int m_iNumTiles;

	CvWString m_szName;

	int* m_aiBuildingGoodHealth;
	int* m_aiBuildingBadHealth;
	int* m_aiBuildingHappiness;
	int* m_aiFreeSpecialist;
	int* m_aiDistanceMaintenanceModifier;
	int* m_aiCorporationMaintenanceModifier;
	int* m_aiPower;

	int** m_aaiYieldRateModifier;
	int** m_aaiFreeBuilding;

	std::set<int> m_areas;

public:

	// for serialization
	virtual void read(FDataStreamBase* pStream);
	virtual void write(FDataStreamBase* pStream);

};

#endif
