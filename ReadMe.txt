Project plan

Short Description:
	I am attempting to recreate "Chronicles of Crime" by David Cicurel but as a video game. Instead of having several evidence categories to choose from, I'd like to learn how to create a dialog system so that the player can type in what they see. The evidence board, in the board game, is used as the player's hub to interact with all pieces of the game. I am turning it into a journal that the player has access to anytime during the game. I intend the journal to be the global blackboard for the entire game, showing active states the player set during their playthrough. The journal servers only a reminder of the evidence, locations, and character the player has found. For the player to investigate an item or a character, they must first type in the respective object's name. From there, they can type a different item or character to see if they go together. For instance, if I am interrogating Mary and I type in Knife, I am asking Mary if she knows anything about the knife. Like finding clues, or interrogating a person, each time the player types in a response, they are performing that response which takes up in-game time. If the player missed spelled an object, I would consider that as the player's character fumbling around or flipping through their notes. For a player to examine a location, they would type in investigate, after which the location's background will change from a landscape picture to a detailed subsection of the picture. In this picture, there will be multiple clues embedded in the picture using photoshop. The player can type in those items to see if they are relevant clues. Each inspection costs the same amount of in-game time, and some clues are decoys. It is up to the player to pick clues that are important to the case. My version will be a text-based visual novel, with an XML level editor, so that other people can create mysteries and quickly iterate them. 


Goals through the semester:
    - Goals mid Semester      
        - Goal        Setting up data structures exactly like the community editor
        - Goal        Able to set entities like the editor, only through xml
        - Goal        Able to parse player text, as well as read in files with variables
        - Goal        Have moment to moment be a text adventure
        - Stretch     Use Imgui to set up visual aids.
      
    - Goal final turn in     
        - Goal        Have prototype asset images as visual aids for the player
        - Goal        Have the tutorial from the board game playable in the project
        - Goal         Have the AI run a scheduled life 
        - Stretch     Have a custom mystery for players to solve
        - Stretch    Tie emotional models with the AI


Brake down of what goal entails:
    Setting up data structures exactly like the community editor     
        Generic Blackboard - General Settings, Ending Configurations, and Variables
        Generic Entity - ID Number, Name, Starting Status, Scanning in a character, Scanning in an item
        Entity::Location - room configuration with images
        Entity::Card (character or item) - Image
        Event manager - Handling the scanning options
        Event - Call back functions to perform intended results
      
    Able to set entities like the editor, only through xml: 
        Working in tandem with above     
         
    Able to parse player text, as well as read in files with variables:
        Player Input - Only one-word responses
        Data base - Quiring Entity blackboard for anything relating to player input
        Designer input - printing dialog, as well as variable names from entity blackboard
         
    Have moment to moment be a text adventure:     
        Debug log - printing entity states via debug log
        Player input - via Debug log to traverse and interrogate
         
    Use Imgui to set up visual aids:
        Start mobile - This will hold the evidence board, location board, characters, and evidence 
        Eyes of the investigator - Seeing the locations and the rooms
         
    Have the tutorial from the board game playable in the project:
        run the game - exhaust the game session, find every dialog and asset used
        recreate - recreate the tutorial using my xml editor
        tweak - check game systems to make sure I can run the game like the board game
         
    Have prototype asset images as visual aids for the player:
        modeling - creating apartment rooms, as well as creating images to support the location
         
    AI overhaul:
        Scheduler - Have plans for the NPC in the story so that they are not staying in one location
        Emotion    - look into tying in my thesis, to give replayability
         
    Custom story:
        run the game - exhaust a different game session, find every dialog and asset used
        recreate - recreate the game using my xml editor
        tweak - check game systems to make sure I can run the game like the board game


Semester Schedule
	Due Date	Hours	Task    																								Actual Hours	Actual Task	
	31-Jan		5		Generic Blackboard: General Settings, Ending Configurations, and Variables								4				Using ImGui to make a simple command prompt to read in the players input.	
				4		Generic Entity: ID Number, Name, Starting Status, scanning in a character, Scanning in an item			5				Going back and forth, and now hardcoading the different card types as classes. Make it clear as to what the game objs are.
    					
	7-Feb		6		Entity::Location: room configuration with images														9				This class was made already, so I ended up moving the XML reading to this week	
				3		Entity::Card (character or item): Image and other metadata												--- 		
    					
	14-Feb		6		Event manager: Handling the "scanning" options															2				Simple Dump items command and looking into 	
				3		Event: Call back functions to perform intended results													5				Using the event system to call back functions that interact with the cards
																																2				Had to prep for SAC event
    					
	21-Feb		1		Generic Blackboard xml reader    																		
				1		Generic Entity xml reader																				4				Get Tutorial Entities: Dialogue and states
				1		Entity::Location xml reader																				5				Picking best dialogue in base card class			
				1		Entity::Card (character or item) xml reader																---	
				1		Event manager xml reader																				---		
				1		Event xml reader																						---		
    					
	28-Feb		9		Player Input: Only one-word responses																	4				SICK DAY
																																4				Understanding CoC level editor Knowledge Base
																																1				Had to prep for SAC event
																																
	6-Mar		9		Data base: Quiring Entity blackboard for anything relating to player input								4				Move Dialogue selection from base to each inherited card type		
																																2				Picking the best Dialogue for a location
																																2				Saving card images to file
																																
	10-Mar	5.75
			0.25		Show the current location from image	
			0.25		Print the Location that the player is at
			0.25		Print the Item the player is interacting with
			0.25		Print the Character the player is talking to
			0.25		Print the Subject the player asked the character
			0.25		Print the current game time	
			0.25		Print the number of game days if the player goes over	
			0.5			Action Class: Display Text
			0.5			Action Class: Set Context
			0.5			Action Class: Enable/Disable Event
			0.5			Action Class: Set object state
			0.5			Action Class: change game time
			0.5			Action Class: Trigger ending
			0.5			Action Class: Set variable value
			0.5			Action Class: set intro text

	13-Mar	5.25
			0.75		Condition Class: Time passed
			0.75		Condition Class: in/notin location
			0.75		Condition Class: Object state check
			0.75		Condition Class: Object scanned
			0.75		Condition Class: Check current context
			0.75		Condition Class: Check a variable value
			0.75		Condition Class: in/out interrogation mode

	17-Mar	4
			0.25		Trigger class: Set name
			0.25		Trigger class: List of conditions
			0.25		Trigger class: List of Actions
			0.25		Event class: Set Name
			0.5			Event class: a one shot event
			0.5			Event class: event run multiple times
			0.25		Event class: Enabaling or disabling an event
			0.25		Event class: List of Triggers
			0.5			When changing locations, change the background to the location image
			0.25		Add 20 in-game minutes when going to a different location
			0.25		When talking to a character, present the text as if reading from a script - Chief: “Glad you can make it…”
			0.25		When interacting with an item, italicizes the text as if perform and thinking	
			0.25		When the computer is talking to the player, use arrowhead - > Talking to Chief

	20-Mar	4
			1			Read in Event from XML
			1			Read in Trigger from XML
			1			Read in Conditions from XML
			1			Read In Actions from XML

	24-Mar	4				
			0.25		Ask for a character the player knows about In a location the character is currently in
			0.25		Ask for a character the player knows about In a location the character is not in
			0.25		Ask for a character the player does not know about In a location the character is currently in
			0.25		Ask for a character the player does not know about In a location the character is not in
			0.25		Ask for a character that the game does not know about	
			0.25		Present an image of the character	
			0.25		Ask for an item the player knows about In a location the item is currently in
			0.25		Ask for an item the player knows about In a location the item is not in
			0.25		Ask for an item the player does not know about In a location the character is currently in
			0.25		Ask for an item the player does not know about In a location the character is not in
			0.25		Ask for an item that the game does not know about	
			0.25		Present an image of the item	
			0.25		When investigating a room, Change the background to the room image
			1			When investigating a room, Change the “state” of the dialog box to ask for evidence in the location
			0.25		When investigating a room, Every question to the room will take 5 in game minutes to “investigate”
			0.5			When investigating a room, the player is Able to leave, and change the room back to the current location

	27-Mar	2.75
			1			When interogating, Change the “state” of the dialog box to ask about entities (characters or items)
			0.25		When interogating, Every question to the character will take 5 in-game minutes to “interrogate”
			0.5			When interogating, Ask them about an item
			0.5			When interogating, Ask them about another character
			0.25		When interogating, Leave the interrogation mode by saying goodbye, and going back to the location
			0.25		Player Investigate an item, just display the text the item's current state is in

	31-Mar	4
			0.25		Journal class, press tab to display the journal
			1			Journal class, Show all know locations
			1			Journal class, Have characters close to the location if the player last seen them there. 
			1			Journal class, Have Characters in the middle of the journal if their location is unknown.
			1			Journal class, Have a section for clues the player has found

	3-Apr	3.5	
			0.5			Highlight the title of the location in the dialog box
			0.5			add the location to the journal
			0.5			Highlight the character’s name in the dialog box
			0.5			Just a character, then add to the unknown location of the journal
			0.5			If we mention a character and location, then add the location to the journal, and the character into the location
			0.5			Highlight the Item’s name as evidence in the dialog box
			0.5			Add the item to the evidence section of the journal

	7-Apr	4.5
			0.25		from xml, Set Starting Location
			0.25		from xml, SSet Scenario Name
			0.25		from xml, SSet startup event
			0.25		from xml, SSet starting time
			0.5			from xml, SIntro message
			0.25		from xml, Sclosed location default message
			0.25		from xml, Ssame location message
			0.25		from xml, Sunknown command message
			0.25		from xml, Stime cost move to location
			0.25		from xml, Stime cost to investigate a room
			0.25		from xml, Stime cost to investigate an item
			0.25		from xml, Stime cost to interagate character
			0.25		from xml, Stime cost for unknown command
			0.5			from xml, Sstart scoring for 1 to 5 stars
			0.5			from xml, Sgame time scoring bonus

	10-Apr	5.75	
			0.25		List of Endings
			0.25		Ending class, Final Solution Text		
			0.25		Ending class, Response to continue the investigation		
			1			Optional  scoring for object in a specific state		
			0.5			Optional variable scoring, add this value to the final score		
			0.25		List of Questions		
			0.5			Question class, Setting question as mandatory or optional	
			0.5			Question class, Score penalty	
			0.25		Question class, text to display	
			0.5			Question class, valid answers with scores	
			0.25		List of ending text		
			0.5			Conditions to test based on The answers from questions
			0.5			Conditions to test based on a List of object states

	14-Apr	4.75
			0.25		Read in name
			0.25		Read in solution
			0.25		Read in mandatory continue
			1			Read in list of optional scoring
			1			Read in list optional variable scoring
			1			Read in list of questions
			1			Read in list of ending text

	17-Apr	2
			1		List of custom variables to track
			1		Read in custome variables from xml

	21-Apr	4		Polish
	24-Apr	4		Polish
	28-Apr	4		Polish
	1-Mar 	4		Polish
	5-Mar	4		Polish

	8-Mar	4		Present final project													---		


