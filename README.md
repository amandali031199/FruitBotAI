# FruitBotAI
A Bot AI in a virtual village competing with other Bots for highest profit through buying and selling fruits whilst being conscious of the amount of fuel used to travel. 

# Introduction
Bots have been in the news for doing negative things such as spreading fake news.

Your task in this assignment is to write a bot that makes the world a better place.

Your bot will pilot a drone which ferries fruit to UNSW students so they stop eating junk food, have a healthy diet.

Your bot must successfully buy, transport and sell fruit in a simulated world.

You will write a C program to implement this bot.

# Virtual World
The virtual world in which your bot will operate has locations spread around a ring.

It is possible to move in only two directions in the virtual world: east and west. You can not move north or south.

The word is circular so if you keep moving east or west you eventually return to where you started.

The details of the virtual world are randomly generated for each simulation. An example follows.

Locations are listed going east. As the world is circular if you go east from Kora Lab you go to CSE. Similarly if you go west from CSE you go to Kora Lab.

There is a diagram here that may help you to better understand how the world fits together. To obtain a copy you can edit on draw.io, copy the contents of this txt file into your browser's address bar.

Note most locations sell or buy a particular fruit - for example Clovelly Watermelons has 85 kg of watermelons for sale at $14/kg.

# What A Fruit Bot Should Do
Your bot has a fixed number of turns in the Fruit Bot world. Its only goal is to have as much cash as possible after the last turn of the simulation.

Your program is run once per turn and given a description of the world on stdin and is asked to choose a single action for your bot by printing a single line of output.

There are three types of actions: Move, Buy and Sell.

A bot makes money by buying fruit from one location and selling to another location at a higher price.

For example, it might buy Bananas from Bondi Banana Growers at $32/kg each and sell them to Physics Theatre at $111/kg.

A bot must move to a location before it can buy or sell from that location.

A bot's only goal is to maximize the cash it has at the end of the simulation. A bot makes cash by buying fruit at one location, and then moving to a location where it can sell the fruit.

There is a maximum weight of fruit that a bot can carry. This limit does not change during the simulation.

Each location buys or sells at most one type of fruit (exception below). No location both buys and sells fruit. Some locations do not buy or sell any fruit.

# Move
A bot making a Move action specifies the number of locations they wish to move. If the number is positive the bot moves East. If the number is negative the bot moves West. It is not possible to move North or South.

The Fruit Bot world is circular - you can move from the last location to the first location and vice-versa. So if a bot keeps moving East or West it will eventually return to where it started.

For example, in the world above a bot at Physics Theatre which makes a Move 3 action would go to Quadrangle. If instead a bot at Physics Theatre made a Move -5 action it would go to Sitar Lab.

There is a limit on the number of locations a bot can move in one turn. This limit does not change during the simulation.

Each location a bot moves will use up 1 kJ of electricity from its battery. A bot with battery level 0 can not move - it can still buy or sell at its current location, but unless it can buy Electricity at this location, it will not be able to move again during the simulation.

A good bot will likely need to charge its battery by buying Electricity

A bot attempting to make a move that would use up more than its available battery or exceed the move limit is not penalised. It is moved the maximum possible distance.

# Buy
A bot making a Buy action must specify how many kg of fruit they wish to buy. A bot may receive less kg than it requests. This will occur if:

the quantity exceeds the kg of fruit the seller has
the bot has insufficient cash
the kg would exceed the bot's weight limit
the bot already has a different type of fruit on board - a bot can only carry one sort of fruit
other bots simultaneously try to buy the fruit from the seller and the requests can't be fairly satisfied (see Multi-bot Worlds description)
A bot's cash is reduced by the total price of the fruit (cost/kg * kg) it receives in a Buy action.

A bot will usually then use one or more Move actions to go to a location which buys this fruit. A bot may however choose to buy more fruit of the same type.

# Sell
A bot making a Sell indicates how many kg of fruit they wish to sell. A bot making a Sell action must be on a location which buys this type of fruit. The amount that is actually sold may be limited by the amount the buyer wishes to buy. This and the price the buyer will pay is indicated in the location's description.

A bot's cash will be increased by the total price of the fruit (cost/kg * kg) it sells in a Sell action.

Bots are not penalised for attempting to buy or sell more fruit than is possible. The transaction will be carried out as much as is possible. For example, if a bot attempts to buy 10000 kg of fruit from a location which only has 25 kg, they will be sold 25 kg (other restrictions permitting).

The initial state of the world is randomly determined at the beginning of each simulation. The details of the world including prices do not change during the simulation. The only exception is that the amount of fruit buyers and sellers are willing to trade reduces as bots buy and sell fruit.

You can not assume a location of a particular name will be in the world. For example, you can not assume a location named "Randwick Apple Orchard" will be present in the world.

If a location of a particular name is present, you can not assume it it is a buyer or a seller, or that it buys or sells a particular fruit.

For example, if "Randwick Apple Orchard" is present you can not assume it is a seller and you can assume it sells or buys fruit.

You can not assume a location of a particular fruit will be in the world. For example, you can not assume "Apples" will be present in the world.

Fruit Bot worlds will always contain one or more locations which will buy any fruit. These locations indicate they buy "Anything".

Fruit Bot worlds will always contain one or more locations which sell "Electricity". Buying electricity charges a bot's battery. It does not affect a bot's weight limit. A bot can buy electricity when carrying fruit. Electricity cannot be sold by a bot. Bots always start with a full battery.

# Multi-bot Worlds
Your bot will be tested in Fruit Bot worlds where it is the only bot present.

It will also be tested in Fruit Bot worlds where other bots are present.

All bots start at the same location, with the full batteries of the same size & cash and have the same number of turns. All bots have the same limits on how far they can move and the maximum fruit weight they can carry.

When multiple bots are present in a world each turn all bots are simultaneously asked for their action. The actions then occur simultaneously.

Multiple bots may be present on one location. If multiple bots on the one location make buy or sell requests which cannot all be satisfied the trade is divided fairly between the bots. Fruit will be left unsold if it is not possible to divide trade fairly.

If, for example, there is 7 kg of fruit for sale at a location, and 3 bots simultaneously request to buy, respectively 7kg, 18kg and 2kg, each bot would be sold 2 kg and 1 kg would be left unsold.

If instead the 3 bots simultaneously requested to buy, respectively 18kg, 7kg and 1kg, the bots would sold 3 kg, 3 kg and 1kg respectively.
