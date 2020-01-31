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
    Due Date	Hours	Actual		Task    
    31-Jan		5		---			Generic Blackboard: General Settings, Ending Configurations, and Variables
				4		---			Generic Entity: ID Number, Name, Starting Status, scanning in a character, Scanning in an item
    
    7-Feb		6		---			Entity::Location: room configuration with images
				3		--- 		Entity::Card (character or item): Image and other metadata
    
    14-Feb		6		---			Event manager: Handling the "scanning" options
				3		---			Event: Call back functions to perform intended results
    
    21-Feb		1		---			Generic Blackboard xml reader    
			    1		---			Generic Entity xml reader
			    1       ---			Entity::Location xml reader
			    1       ---			Entity::Card (character or item) xml reader
			    1       ---			Event manager xml reader
			    1		---			Event xml reader
    
    28-Feb		9		---			Player Input: Only one-word responses
    
    6-Mar		9		---			Data base: Quiring Entity blackboard for anything relating to player input
    
    13-Mar		5		---			Designer input: printing dialog, as well as variable names from entity blackboard
			    1		---			Debug log: printing entity states via debug log    
			    3		---			Player input: via Debug log to traverse and interrogate
    
    20-Mar		---		---			GDC    
    
    27-Mar		9		---			Mobile: This will hold the evidence board, location board, characters, and evidence
    
    3-Apr		3		---			Seeing the locations and the rooms
			    6		---			Have plans for the NPC in the story so that they are not staying in one location
			    
    10-Apr		2		---			exhaust the game session, find every dialog and asset used
			    2		---			recreate the tutorial using my xml editor
			    5		---			check game systems to make sure I can run the game like the board game
			    
    17-Apr		9		---			creating apartment rooms, as well as creating images to support the location
    
    24-Apr		2		---			exhaust a different game session, find every dialog and asset used    
			    2		---			recreate the game using my xml editor
			    5		---			check game systems to make sure I can run the game like the board game
			    
    1-May		9		---			look into tying in thesis, to give replayability

