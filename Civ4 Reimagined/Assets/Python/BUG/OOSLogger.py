import os
from CvPythonExtensions import *

gc = CyGlobalContext()

szFilename = "OOSLog.txt"
iMaxFilenameTries = 100

bWroteLog = False

SEPERATOR = "-----------------------------------------------------------------\n"


# Simply checks every game turn for OOS. If it finds it, writes the
# info contained in the sync checksum to a log file, then sets the bWroteLog
# variable so that it only happens once.
def doGameUpdate():
    global bWroteLog
    bOOS = CyInterface().isOOSVisible()

    if (bOOS and not bWroteLog):
        writeLog()
        bWroteLog = True

def writeLog():
    pFile = open(szFilename, "w")

    #
    # Global data
    #
    pFile.write(SEPERATOR)
    pFile.write(SEPERATOR)

    pFile.write("  GLOBALS  \n")

    pFile.write(SEPERATOR)
    pFile.write(SEPERATOR)
    pFile.write("\n\n")

    pFile.write("Next Map Rand Value: %d\n" % CyGame().getMapRand().get(10000, "OOS Log"))
    pFile.write("Next Soren Rand Value: %d\n" % CyGame().getSorenRand().get(10000, "OOS Log"))

    pFile.write("Total num cities: %d\n" % CyGame().getNumCities() )
    pFile.write("Total population: %d\n" % CyGame().getTotalPopulation() )
    pFile.write("Total Deals: %d\n" % CyGame().getNumDeals() )

    pFile.write("Total owned plots: %d\n" % CyMap().getOwnedPlots() )
    pFile.write("Total num areas: %d\n" % CyMap().getNumAreas() )

    pFile.write("\n\n")

    #
    # Player data
    #
    iPlayer = 0
    for iPlayer in range(gc.getMAX_PLAYERS()):
        pPlayer = gc.getPlayer(iPlayer)
        if (pPlayer.isEverAlive()):
            pFile.write(SEPERATOR)
            pFile.write(SEPERATOR)

            pFile.write("  PLAYER %d  \n" % iPlayer)

            pFile.write(SEPERATOR)
            pFile.write(SEPERATOR)
            pFile.write("\n\n")

            pFile.write("Basic data:\n")
            pFile.write("-----------\n")
            pFile.write("Player %d Score: %d\n" % (iPlayer, gc.getGame().getPlayerScore(iPlayer) ))

            pFile.write("Player %d Population: %d\n" % (iPlayer, pPlayer.getTotalPopulation() ) )
            pFile.write("Player %d Total Land: %d\n" % (iPlayer, pPlayer.getTotalLand() ) )
            pFile.write("Player %d Gold: %d\n" % (iPlayer, pPlayer.getGold() ) )
            pFile.write("Player %d Assets: %d\n" % (iPlayer, pPlayer.getAssets() ) )
            pFile.write("Player %d Power: %d\n" % (iPlayer, pPlayer.getPower() ) )
            pFile.write("Player %d Num Cities: %d\n" % (iPlayer, pPlayer.getNumCities() ) )
            pFile.write("Player %d Num Units: %d\n" % (iPlayer, pPlayer.getNumUnits() ) )
            pFile.write("Player %d Num Selection Groups: %d\n" % (iPlayer, pPlayer.getNumSelectionGroups() ) )

            pFile.write("\n\n")

            pFile.write("Yields:\n")
            pFile.write("-------\n")
            for iYield in range( int(YieldTypes.NUM_YIELD_TYPES) ):
                pFile.write("Player %d %s Total Yield: %d\n" % (iPlayer, gc.getYieldInfo(iYield).getDescription(), pPlayer.calculateTotalYield(iYield) ))

            pFile.write("\n\n")

            pFile.write("Commerce:\n")
            pFile.write("---------\n")
            for iCommerce in range( int(CommerceTypes.NUM_COMMERCE_TYPES) ):
                pFile.write("Player %d %s Total Commerce: %d\n" % (iPlayer, gc.getCommerceInfo(iCommerce).getDescription(), pPlayer.getCommerceRate(CommerceTypes(iCommerce)) ))

            pFile.write("\n\n")

            pFile.write("Bonus Info:\n")
            pFile.write("-----------\n")
            for iBonus in range(gc.getNumBonusInfos()):
                pFile.write("Player %d, %s, Number Available: %d\n" % (iPlayer, gc.getBonusInfo(iBonus).getDescription(), pPlayer.getNumAvailableBonuses(iBonus) ))
                pFile.write("Player %d, %s, Import: %d\n" % (iPlayer, gc.getBonusInfo(iBonus).getDescription(), pPlayer.getBonusImport(iBonus) ))
                pFile.write("Player %d, %s, Export: %d\n" % (iPlayer, gc.getBonusInfo(iBonus).getDescription(), pPlayer.getBonusExport(iBonus) ))
                pFile.write("\n")

            pFile.write("\n\n")

            pFile.write("Improvement Info:\n")
            pFile.write("-----------------\n")
            for iImprovement in range(gc.getNumImprovementInfos()):
                pFile.write("Player %d, %s, Improvement count: %d\n" % (iPlayer, gc.getImprovementInfo(iImprovement).getDescription(), pPlayer.getImprovementCount(iImprovement) ))

            pFile.write("\n\n")

            pFile.write("Building Class Info:\n")
            pFile.write("--------------------\n")
            for iBuildingClass in range(gc.getNumBuildingClassInfos()):
                pFile.write("Player %d, %s, Building class count plus building: %d\n" % (iPlayer, gc.getBuildingClassInfo(iBuildingClass).getDescription(), pPlayer.getBuildingClassCountPlusMaking(iBuildingClass) ))

            pFile.write("\n\n")

            pFile.write("Unit Class Info:\n")
            pFile.write("--------------------\n")
            for iUnitClass in range(gc.getNumUnitClassInfos()):
                pFile.write("Player %d, %s, Unit class count plus training: %d\n" % (iPlayer, gc.getUnitClassInfo(iUnitClass).getDescription(), pPlayer.getUnitClassCountPlusMaking(iUnitClass) ))

            pFile.write("\n\n")

            pFile.write("UnitAI Types Info:\n")
            pFile.write("------------------\n")
            for iUnitAIType in range(int(UnitAITypes.NUM_UNITAI_TYPES)):
                pFile.write("Player %d, %s, Unit AI Type count: %d\n" % (iPlayer, gc.getUnitAIInfo(iUnitAIType).getDescription(), pPlayer.AI_totalUnitAIs(UnitAITypes(iUnitAIType)) ))
            

            pFile.write("\n\n")

            pFile.write("Unit Info:\n")
            pFile.write("----------\n")
            iNumUnits = pPlayer.getNumUnits()

            if (iNumUnits == 0):
                pFile.write("No Units")
            else:
                pLoopUnitTuple = pPlayer.firstUnit(False)
                while (pLoopUnitTuple[0] != None):
                    pUnit = pLoopUnitTuple[0]
                    pFile.write("Player %d, Unit ID: %d, %s\n" % (iPlayer, pUnit.getID(), pUnit.getName() ))
                    pFile.write("X: %d, Y: %d\n" % (pUnit.getX(), pUnit.getY()) )
                    pFile.write("Damage: %d\n" % pUnit.getDamage() )
                    pFile.write("Experience: %d\n" % pUnit.getExperience() )
                    pFile.write("Level: %d\n" % pUnit.getLevel() )

                    pLoopUnitTuple = pPlayer.nextUnit(pLoopUnitTuple[1], False)
                    pFile.write("\n")
                

            # Space at end of player's info
            pFile.write("\n\n")
        
    # Close file

    pFile.close()
