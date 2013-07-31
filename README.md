zanrun
======

By Alexmax and Hypnotoad
What is this?
Remember strun? It was a custom gametype for Skulltag in the spirit of Jumpmaze and Q3 Defrag.
Zanrun is an enhanced version of this, adding new features such as a leaderboard to show player's best times, and LUK support (an rcon utility that saves and retrieves serverside record times to display to people).

What's the difference between this and Jumpmaze?
ZanRun is not a mapset.  It is an engine that you run with other maps that support ZanRun.

ZanRun features:

    Resettable race times.  Your time starts as soon as you cross the START line, not at the beginning of the map.  If you want to give the map another shot, you can either kill yourself or teleport back to the start and try again.

    Advanced split times.  Want to have a checkpoint-by-checkpoint view of how much faster your current run is than your last best run?  The split times will let you know.
  
	Luk support: stores the record time on that map, and the author, and is saved in a database so it can be retrieved later on even after server restart.
	
    Serverside logging.  Every completed run is written to the Skulltag server's logfiles in a way that server administrators can easily use regular expressions to extract them.  This opens the door for things like web-based leaderboards.

    An easy way to make your own ZanRun maps.  We have gone out of our way to making sure that making your own ZanRun is seriously simple.  All you need to know is five script numbers specific to ZanRun, and you don't have to write a single line of ACS.

    A CTF mode where you can practice flag-running on almost any CTF map with the ZanRun engine.

This is our TODO list:

    A working player speedometer.

Sounds cool.  How do I make a map for ZanRun?
A map consists of five types of lines.  A start line, a checkpoint line, an end line, an "ENTER" line and an exit line.

    Start line: Line Action 226, arg1 of 219, arg3 is equal to the number of checkpoints you have in your map (not counting the start and finish line)

    Checkpoint: Line Action 226, arg1 of 220, arg3 is a unique identifier for that checkpoint that is between 0 and the number of total checkpoints minus one and goes up for every subsequent checkpoint.  So if you have three checkpoints in your map, the first one would have an arg3 of 0, the second one would have an arg3 of 1 and the third would have an arg3 of 2. and so on...

    Finishing Line: Line Action 226, arg1 of 221, arg3 is equal to the number of checkpoints you have in your map (not counting the start and finish line).

    "ENTER" Line: Line Action 226, arg1 of 100.  This line is not quite self explanatory, it's a line that resets the player back to a pristine state that he was when he entered the level.  I would suggest putting this at the end of the level, before the player gets teleported back...
	
	Exit line: Line action 80, arg1 of 401, when player pressues use (recommended). This will initiate the end of the race, and display a high score table, showing the top 5 player's best times in order from 1st to 5th. 

For more information, load up the ZanRun in Doom Builder and take a gander at the SAMPLE map.  Everything you can do with ZanRun is laid out there, nice and easy for you.
