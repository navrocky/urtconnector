# What's new in UrTConnector #

### Version 0.9.0 "Noobs Hope" (September 12, 2012) ###
  * added support for the new UrbanTerror 4.2
  * added native server info updater (quick and stable), so qstat dropped from dependencies
  * added minimum empty slots filter
  * added game type filter
  * added waiting for someone function when connecting to the empty server
  * added coloring of players nicks
  * added ability to launch multiple instances of the game
  * added function to hide or close application before game launch
  * and, of course, were fixed some bugs and added a portion of fresh bugs :)

### Version 0.8.0 "Hilarious Headshot" (January 29, 2012) ###
  * added universal event tracking subsystem
  * added wait for friend action
  * new player nick name filter
  * new filter list look
  * added css skin support
  * create "wait for empty slots" task if server is full during connection
  * updating speedup and detailed update settings (200 parallel queries)
  * dont clear offline servers if the user cancels updating
  * now updating jobs performed in parallel
  * fixed mumble overlay option
  * fixed several crashes during the info update extending from v0.7
  * fixed memory leaks

### Version 0.7.0 "Fast & Foolish" (June 25, 2011) ###
  * tabs updating improvement and refactoring
  * history tab improved
  * added new friend list tab
  * bookmarking friends from server info
  * ability to kick players and change map from server info panel (RCON password needed)
  * added interface language select menu
  * added server checking before connect
  * added new filters: ping, weapon, custom
  * rcon improvements and fixes
  * added information on the allowed weapon in the tabs and server info panel
  * added launch with mumble-overlay option (in linux/unix)
  * added copy server info to a clipboard action
  * many small fixes

### Version 0.6.0 "Christmas Edition" (Dec 31, 2010) ###
  * added a history widget (endenis, kinnalru)
  * new sexy preferences dialogs (kinnalru)
  * added the anticheat prototype
  * a server list update optimizations
  * added warnings about empty and full server before connect
  * storing server list into sqlite db, this cause a launch/shutdown speedup (KJChernov)
  * great rcon autocompletion ipmrovement. It now complements a players and maps. (kinnalru)
  * rcon fixes (kinnalru)
  * added toolbar to filter panel
  * filter window is done now as dock window
  * added master server setting
  * christmas icon theme
  * many small fixes

### Version 0.5.0 "Enemy spotted" (Aug 29, 2010) ###
  * added simple Remote Console(Rcon)
  * added ability to start UrbanTerror on new free X-session
  * added coloring of server name in the info panel
  * added bookmark button in quick launch panel
  * added buttons to full/partial clear of server lists
  * added info about public slots on servers
  * offline servers moved to list bottom
  * added a powerfull filter system with a some number of useful filters (full, empty, game mode, online clans, quick, protected)
  * fixed bugs as usual

### Version 0.4.0 "Snotty noob" (Jun 04, 2010) ###
  * added server country information in info and lists
  * added ability to view progress executing update jobs in percent and cancel them
  * added server lists state saving
  * added passworded server indicator and password request when connecting
  * added clipboard watching for server and password info
  * added button to query server name from "add favorite" dialog
  * added mechanism to avoid urtconnector re-run
  * fixed game launch on windows from "program files"
  * fixed bugs in favorites
  * fixed columns order and sizes by default
  * fixed config system
  * fixed many other bugs

### Version 0.3.0 "Great meat" (Mar 29, 2010) ###
  * added tray icon and option "start hidden"
  * added Debian, ArchLinux packages and Windows installer
  * added translate ability and russian translation
  * added menu entry on linuxes
  * added launch game by double click on server list
  * fixed favorites deletion
  * fixed about dialog
  * fixed server list and server info panel
  * fixed config store method
  * fixed launcher, now it can run a game on windows
  * fixed closing game due close UrTConnector

### Version 0.2.0 (Mar 17, 2010) ###
  * initial public release, totally rewritten from previous version