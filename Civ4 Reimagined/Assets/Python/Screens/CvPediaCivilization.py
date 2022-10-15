## Sid Meier's Civilization 4
## Copyright Firaxis Games 2005
from CvPythonExtensions import *
import CvUtil
import ScreenInput
import CvScreenEnums

# globals
gc = CyGlobalContext()
ArtFileMgr = CyArtFileMgr()
localText = CyTranslator()

class CvPediaCivilization:
	"Civilopedia Screen for Civilizations"

	def __init__(self, main):
		self.iCivilization = -1
		self.top = main
		
		self.X_MAIN_PANE = 50
		self.Y_MAIN_PANE = 90
		self.W_MAIN_PANE = 250
		self.H_MAIN_PANE = 210

		self.X_ICON = 98
		self.Y_ICON = 125
		self.W_ICON = 150
		self.H_ICON = 150
		self.ICON_SIZE = 64

		self.X_TECH = 330
		self.Y_TECH = 70
		self.W_TECH = 200
		self.H_TECH = 110

		self.X_BUILDING = 555
		self.Y_BUILDING = 190
		self.W_BUILDING = 200
		self.H_BUILDING = 110

		self.X_UNIT = 330
		self.Y_UNIT = 190
		self.W_UNIT = 200
		self.H_UNIT = 110

		self.X_LEADER = 555
		self.Y_LEADER = 70
		self.W_LEADER = 200
		self.H_LEADER = 110

		self.X_TEXT = self.X_MAIN_PANE
		self.Y_TEXT = self.Y_MAIN_PANE + self.H_MAIN_PANE + 20
		self.W_TEXT = 705
		self.H_TEXT = 350
		
	# Screen construction function
	def interfaceScreen(self, iCivilization):	
			
		self.iCivilization = iCivilization
	
		self.top.deleteAllWidgets()						
							
		screen = self.top.getScreen()
		
		bNotActive = (not screen.isActive())
		if bNotActive:
			self.top.setPediaCommonWidgets()

		# Header...
		szHeader = u"<font=4b>" + gc.getCivilizationInfo(self.iCivilization).getDescription().upper() + u"</font>"
		szHeaderId = self.top.getNextWidgetName()
		screen.setLabel(szHeaderId, "Background", szHeader, CvUtil.FONT_CENTER_JUSTIFY, self.top.X_SCREEN, self.top.Y_TITLE, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_GENERAL, -1, -1)
		
		# Top
		screen.setText(self.top.getNextWidgetName(), "Background", self.top.MENU_TEXT, CvUtil.FONT_LEFT_JUSTIFY, self.top.X_MENU, self.top.Y_MENU, 0, FontTypes.TITLE_FONT, WidgetTypes.WIDGET_PEDIA_MAIN, CivilopediaPageTypes.CIVILOPEDIA_PAGE_CIV, -1)

		if self.top.iLastScreen	!= CvScreenEnums.PEDIA_CIVILIZATION or bNotActive:		
			self.placeLinks(true)
			self.top.iLastScreen = CvScreenEnums.PEDIA_CIVILIZATION
		else:
			self.placeLinks(false)
			
		# Icon
		screen.addPanel( self.top.getNextWidgetName(), "", "", False, False,
		    self.X_MAIN_PANE, self.Y_MAIN_PANE, self.W_MAIN_PANE, self.H_MAIN_PANE, PanelStyles.PANEL_STYLE_BLUE50)
		screen.addPanel(self.top.getNextWidgetName(), "", "", false, false,
		    self.X_ICON, self.Y_ICON, self.W_ICON, self.H_ICON, PanelStyles.PANEL_STYLE_MAIN)
		screen.addDDSGFC(self.top.getNextWidgetName(), ArtFileMgr.getCivilizationArtInfo(gc.getCivilizationInfo(self.iCivilization).getArtDefineTag()).getButton(),
		    self.X_ICON + self.W_ICON/2 - self.ICON_SIZE/2, self.Y_ICON + self.H_ICON/2 - self.ICON_SIZE/2, self.ICON_SIZE, self.ICON_SIZE, WidgetTypes.WIDGET_GENERAL, -1, -1 )

		self.placeTech()
		self.placeBuilding()
		self.placeUnit()
		self.placeLeader()
		self.placeText()

		return

	def placeTech(self):
		
		screen = self.top.getScreen()
		
		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_FREE_TECHS", ()), "", false, true,
				 self.X_TECH, self.Y_TECH, self.W_TECH, self.H_TECH, PanelStyles.PANEL_STYLE_BLUE50 )
		screen.attachLabel(panelName, "", "  ")
		
		for iTech in range(gc.getNumTechInfos()):
			if (gc.getCivilizationInfo(self.iCivilization).isCivilizationFreeTechs(iTech)):
				screen.attachImageButton( panelName, "", gc.getTechInfo(iTech).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_TECH, iTech, 1, False )
	
	def placeBuilding(self):
		
		screen = self.top.getScreen()
		
		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_UNIQUE_BUILDINGS", ()), "", false, true,
				self.X_BUILDING, self.Y_BUILDING, self.W_BUILDING, self.H_BUILDING, PanelStyles.PANEL_STYLE_BLUE50 )
		screen.attachLabel(panelName, "", "  ")
		
		for iBuilding in range(gc.getNumBuildingClassInfos()):
			iUniqueBuilding = gc.getCivilizationInfo(self.iCivilization).getCivilizationBuildings(iBuilding);
			iDefaultBuilding = gc.getBuildingClassInfo(iBuilding).getDefaultBuildingIndex();
			if (iUniqueBuilding > -1 and iDefaultBuilding != iUniqueBuilding):
				screen.attachImageButton( panelName, "", gc.getBuildingInfo(iUniqueBuilding).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_BUILDING, iUniqueBuilding, 1, False )
				
	def placeUnit(self):
		
		screen = self.top.getScreen()
		
		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_FREE_UNITS", ()), "", false, true,
				 self.X_UNIT, self.Y_UNIT, self.W_UNIT, self.H_UNIT, PanelStyles.PANEL_STYLE_BLUE50 )
		screen.attachLabel(panelName, "", "  ")
					
		for iUnit in range(gc.getNumUnitClassInfos()):
			iUniqueUnit = gc.getCivilizationInfo(self.iCivilization).getCivilizationUnits(iUnit);
			iDefaultUnit = gc.getUnitClassInfo(iUnit).getDefaultUnitIndex();
			if (iDefaultUnit > -1 and iUniqueUnit > -1 and iDefaultUnit != iUniqueUnit):
				szButton = gc.getUnitInfo(iUniqueUnit).getButton()
				if self.top.iActivePlayer != -1:
					szButton = gc.getPlayer(self.top.iActivePlayer).getUnitButton(iUniqueUnit)
				screen.attachImageButton( panelName, "", szButton, GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_UNIT, iUniqueUnit, 1, False )
		
	def placeLeader(self):
		
		screen = self.top.getScreen()
		
		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, localText.getText("TXT_KEY_CONCEPT_LEADERS", ()), "", false, true,
				 self.X_LEADER, self.Y_LEADER, self.W_LEADER, self.H_LEADER, PanelStyles.PANEL_STYLE_BLUE50 )
		screen.attachLabel(panelName, "", "  ")

		for iLeader in range(gc.getNumLeaderHeadInfos()):
			civ = gc.getCivilizationInfo(self.iCivilization)
			if civ.isLeaders(iLeader):
				screen.attachImageButton( panelName, "", gc.getLeaderHeadInfo(iLeader).getButton(), GenericButtonSizes.BUTTON_SIZE_CUSTOM, WidgetTypes.WIDGET_PEDIA_JUMP_TO_LEADER, iLeader, self.iCivilization, False )
		
	def placeText(self):
		
		screen = self.top.getScreen()
		
		panelName = self.top.getNextWidgetName()
		screen.addPanel( panelName, "", "", true, true,
				 self.X_TEXT, self.Y_TEXT, self.W_TEXT, self.H_TEXT, PanelStyles.PANEL_STYLE_BLUE50 )

		szText = u"<color=231,219,159,255>" + "Unique Power: \n" + gc.getCivilizationInfo(self.iCivilization).getCivilizationUniquePowerText() + u"</color>"
		szText += "\n\n" + gc.getCivilizationInfo(self.iCivilization).getCivilopedia()
		screen.attachMultilineText( panelName, "Text", szText, WidgetTypes.WIDGET_GENERAL, -1, -1, CvUtil.FONT_LEFT_JUSTIFY)
													
	def placeLinks(self, bRedraw):

		screen = self.top.getScreen()

		if bRedraw:	
			screen.clearListBoxGFC(self.top.LIST_ID)
		
		# sort Improvements alphabetically
		listSorted=[(0,0)]*gc.getNumCivilizationInfos()
		for j in range(gc.getNumCivilizationInfos()):
			listSorted[j] = (gc.getCivilizationInfo(j).getDescription(), j)
		listSorted.sort()	
			
		iSelected = 0
		i = 0
		for iI in range(gc.getNumCivilizationInfos()):
			if (gc.getCivilizationInfo(listSorted[iI][1]).isPlayable() and not gc.getCivilizationInfo(listSorted[iI][1]).isGraphicalOnly()):
				if (not gc.getDefineINT("CIVILOPEDIA_SHOW_ACTIVE_CIVS_ONLY") or not gc.getGame().isFinalInitialized() or gc.getGame().isCivEverActive(listSorted[iI][1])):
					if bRedraw:
						screen.appendListBoxStringNoUpdate(self.top.LIST_ID, listSorted[iI][0], WidgetTypes.WIDGET_PEDIA_JUMP_TO_CIV, listSorted[iI][1], 0, CvUtil.FONT_LEFT_JUSTIFY )
					if listSorted[iI][1] == self.iCivilization:
						iSelected = i
					i += 1
					
		if bRedraw:
			screen.updateListBox(self.top.LIST_ID)
					
		screen.setSelectedListBoxStringGFC(self.top.LIST_ID, iSelected)
			

	# Will handle the input for this screen...
	def handleInput (self, inputClass):
		return 0


