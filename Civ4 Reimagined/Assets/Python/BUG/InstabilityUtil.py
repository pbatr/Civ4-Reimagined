## InstabilityUtil
##
## Utilities for dealing with Instability and Crisis system.
##
## MODDERS
##
##   This utility handles the instability progress bar display for the three crisis types:
##   - Political Crisis
##   - Economic Crisis  
##   - Health Crisis
##
## Notes
##   - Must be initialized externally by calling init()

from CvPythonExtensions import *
import BugUtil
import FontUtil
import PlayerUtil

gc = CyGlobalContext()

# Crisis types
NUM_CRISIS_TYPES = 3
(
	CRISIS_POLITICAL,
	CRISIS_ECONOMIC,
	CRISIS_HEALTH,
) = range(NUM_CRISIS_TYPES)

# Crisis type names
g_crisisNames = (
	"TXT_KEY_CRISIS_POLITICAL",
	"TXT_KEY_CRISIS_ECONOMIC", 
	"TXT_KEY_CRISIS_HEALTH",
)

# Crisis colors
g_crisisColors = None

# Crisis icons
g_crisisIcons = None

# Information

def init():
	global g_crisisColors
	g_crisisColors = [None] * NUM_CRISIS_TYPES
	g_crisisColors[CRISIS_POLITICAL] = gc.getInfoTypeForString("COLOR_RED")
	g_crisisColors[CRISIS_ECONOMIC] = gc.getInfoTypeForString("COLOR_YELLOW")
	g_crisisColors[CRISIS_HEALTH] = gc.getInfoTypeForString("COLOR_GREEN")
	
	global g_crisisIcons
	g_crisisIcons = [None] * NUM_CRISIS_TYPES
	g_crisisIcons[CRISIS_POLITICAL] = FontUtil.getChar(FontSymbols.GREAT_GENERAL_CHAR)  # Use general icon for political
	g_crisisIcons[CRISIS_ECONOMIC] = FontUtil.getChar(FontSymbols.COMMERCE_GOLD_CHAR)   # Use gold icon for economic
	g_crisisIcons[CRISIS_HEALTH] = FontUtil.getChar(FontSymbols.HEALTHY_CHAR)          # Use health icon for health

def getCrisisName(crisisType):
	return g_crisisNames[crisisType]

def getCrisisColor(crisisType):
	return g_crisisColors[crisisType]

def getCrisisIcon(crisisType):
	return g_crisisIcons[crisisType]

# Getting Progress

def getPlayerInstabilityData():
	"""
	Returns the current player's instability data for display.
	"""
	player = gc.getPlayer(gc.getGame().getActivePlayer())
	
	iTotalInstability = player.getInstabilityProgress()
	iPoliticalInstability = player.getPoliticalInstabilityProgress()
	iEconomicInstability = player.getEconomicInstabilityProgress()
	iHealthInstability = player.getHealthInstabilityProgress()
	
	# Calculate threshold based on game speed and era
	iThreshold = player.getInstabilityThreshold()
	
	return (iTotalInstability, iPoliticalInstability, iEconomicInstability, iHealthInstability, iThreshold)

def calcInstabilityPercentages():
	"""
	Calculate the percentages for each crisis type based on current instability progress.
	"""
	player = gc.getPlayer(gc.getGame().getActivePlayer())
	
	iPolitical = player.getPoliticalInstabilityProgress()
	iEconomic = player.getEconomicInstabilityProgress()
	iHealth = player.getHealthInstabilityProgress()
	
	iTotal = iPolitical + iEconomic + iHealth
	
	percents = []
	if (iTotal > 0):
		iLeftover = 100
		crisisTypes = [CRISIS_POLITICAL, CRISIS_ECONOMIC, CRISIS_HEALTH]
		crisisValues = [iPolitical, iEconomic, iHealth]
		
		for i, crisisType in enumerate(crisisTypes):
			if (crisisValues[i] > 0):
				iPercent = 100 * crisisValues[i] / iTotal
				iLeftover -= iPercent
				percents.append((iPercent, crisisType))
		
		# Add remaining from 100 to first in list to match Civ4 pattern
		if (iLeftover > 0 and percents):
			percents[0] = (percents[0][0] + iLeftover, percents[0][1])
	
	return percents

# Displaying Progress

def getInstabilityText(iInstabilityBarWidth, bIncludePlayerName):
	"""
	Generate the text to display in the instability progress bar.
	"""
	player = gc.getPlayer(gc.getGame().getActivePlayer())
	
	iTotalInstability, iPolitical, iEconomic, iHealth, iThreshold = getPlayerInstabilityData()
	
	if (iThreshold <= 0):
		return u"<font=2>%s</font>" % (localText.getText("TXT_KEY_INSTABILITY_NO_THRESHOLD", ()))
	
	# Build the text
	szText = u""
	
	if (bIncludePlayerName):
		szText += u"%s: " % (player.getName())
	
	szText += u"%s" % (localText.getText("TXT_KEY_INSTABILITY_PROGRESS", (iTotalInstability, iThreshold)))
	
	# Add crisis type breakdown if there's instability
	if (iTotalInstability > 0):
		percents = calcInstabilityPercentages()
		if (percents):
			szText += u" ["
			for i, (iPercent, crisisType) in enumerate(percents):
				if (i > 0):
					szText += u", "
				szText += u"%s: %d%%" % (localText.getText(getCrisisName(crisisType), ()), iPercent)
			szText += u"]"
	
	return u"<font=2>%s</font>" % (szText)

def getHoverText(eWidgetType, iData1, iData2, bOption):
	"""
	Generate hover text for the instability progress bar.
	"""
	player = gc.getPlayer(gc.getGame().getActivePlayer())
	
	iTotalInstability, iPolitical, iEconomic, iHealth, iThreshold = getPlayerInstabilityData()
	
	szText = u"<font=3b>%s</font>\n" % (localText.getText("TXT_KEY_INSTABILITY_TITLE", ()))
	
	szText += u"%s: %d/%d\n" % (localText.getText("TXT_KEY_INSTABILITY_TOTAL", ()), iTotalInstability, iThreshold)
	szText += u"%s: %d\n" % (localText.getText("TXT_KEY_INSTABILITY_POLITICAL", ()), iPolitical)
	szText += u"%s: %d\n" % (localText.getText("TXT_KEY_INSTABILITY_ECONOMIC", ()), iEconomic)
	szText += u"%s: %d\n" % (localText.getText("TXT_KEY_INSTABILITY_HEALTH", ()), iHealth)
	
	if (iThreshold > 0):
		fPercent = float(iTotalInstability) / float(iThreshold) * 100.0
		szText += u"%s: %.1f%%" % (localText.getText("TXT_KEY_INSTABILITY_PERCENT", ()), fPercent)
	
	return szText 