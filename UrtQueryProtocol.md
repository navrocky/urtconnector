# Common principles #

  * UDP protocol
  * all queries and replies starts with marker: 0xFFFFFFFF

# Query master #

We need query master server to get global server ip:port list.

Default master UDP port: 27950

Known masters:
  * master.urbanterror.info
  * master2.urbanterror.info

### Query: ###

> "getservers N (full) (empty)"

Where N - protocol number. "full" and "empty" is optional, it is mean to include full and empty servers to the list.

Example: **getservers 68 full empty**

### Reply: ###

> "getserversResponse\xxxxxx\xxxxxx\..."

Where "xxxxxx" - 6 bytes (4 bytes is ip address and 2 bytes - port)

Hint: For one query we can get more than one reply packet. End of list we can detect by some timeout.

# Query game server #

**Getting info query (protocol 68)**

Query:
> "getinfo"

Reply:
> "infoResponse\x0a\\game\\q3ut4\\pure\\1\\gametype\\8\\sv\_maxclients\\14\\clients\\6\\mapname\\ut4\_turnpike\\hostname\\TinyBomb #1 Classic\\protocol\\68"

**Getting info query (new for urt 4.2, protocol 70)**


Query:
> "getinfo xxx"

Reply (c-string with escape symbols "\"):
> "infoResponse\x0a\\game\\q3ut4\\auth\_enable\\1\\pure\\1\\gametype\\4\\sv\_maxclients\\14\\clients\\0\\mapname\\ut4\_turnpike\\hostname\\(LT) CLAN LUTE \\protocol\\70\\challenge\\xxx"

**Request server status**

Query:
> "getstatus"

Reply:
> "statusResponse\x0a\\sv\_allowdownload\\0\\g\_matchmode\\0\\g\_gametype\\8\\sv\_maxclients\\16\\sv\_floodprotect\\1\\g\_warmup\\15\\capturelimit\\10\\sv\_hostname\\TinyBomb #1 Classic\\g\_followstrict\\1\\fraglimit\\15\\timelimit\\0\\g\_cahtime\\60\\g\_swaproles\\1\\g\_roundtime\\1.6\\g\_bombexplodetime\\39\\g\_bombdefusetime\\9\\g\_hotpotato\\2\\g\_waverespawns\\1\\g\_redwave\\15\\g\_bluewave\\15\\g\_respawndelay\\8\\g\_suddendeath\\1\\g\_maxrounds\\10\\g\_friendlyfire\\1\\g\_allowvote\\536871000\\g\_armbands\\0\\g\_enablePrecip\\0\\g\_survivorrule\\0\\g\_teamnameblue\\BBQ'ed\\g\_teamnamered\\Bombers\\g\_gear\\0\\g\_deadchat\\2\\g\_maxGameClients\\0\\dmflags\\0\\sv\_minRate\\0\\sv\_maxRate\\0\\sv\_minPing\\0\\sv\_maxPing\\0\\sv\_dlURL\\http://ut.s.finestway.com\\version\\ioq3 1.35urt linux-i386 Jan 28 2009\\protocol\\68\\mapname\\ut4\_turnpike\\sv\_privateClients\\2\\ Admin\\ [aG-rum] Team !\\ Email\\agrum.team@gmail.com\\gamename\\q3ut4\\g\_needpass\\0\\g\_enableDust\\0\\g\_enableBreath\\0\\g\_antilagvis\\0\\g\_survivor\\1\\g\_modversion\\4.1\x0a0 270 \"Psicologica\"\x0a3 75 \"TheProdigy.pt\"\x0a0 50 \"You\"\x0a7 23 \"cub\"\x0a2 37 \"Mr.BEAN\"\x0a1 16 \"Wapaz\"\x0a"

As we can see here is some strings delimited by "\n":
  * "statusResponse"
  * cvar:value delimited by "\"
  * Score Ping "Player name"
  * Score Ping "Player name"
  * etc