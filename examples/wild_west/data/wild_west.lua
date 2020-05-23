--arguments:
-- static is a table shared between all gamestates (but not entities)
-- state is the underlying gamestate object with methods to access e.g. the world graph
---
-- the application can be accessed e.g. via ungod.addGameState(id, scriptfile) to add a new gamestate or ungod.quitApplication()
-- the ungod table is shared between all gamestates and entities and can be used for global communication
function wild_west.onInit(static, state)

    --create a global table
    --all entities and other gamestates can access this
    ungod.wild_west = {}
    local w = ungod.wild_west

    print("hello wild west world...")

   state:behavior():loadBehaviorScript("data/miner.lua")
   state:behavior():loadBehaviorScript("data/wife.lua")

    --create a new node of size 1000x1000 in the world graph and add a world to it
    w.node = state:worldGraph():createNode(state, "wild_west", "data/wild_west.world")

    --uncomment for reset when wild_west.world file is missing
    -- w.node:setSize(Vec2f.new(1000,1000))
    -- w.world = w.node:addWorld()
    -- w.miner = w.world:createEntity("miner", {name = "Bob"})
    -- w.miner:add():updateTimer()
    -- w.world:add(w.miner)
    -- w.wife = w.world:createEntity("wife", {name = "Elsa", husband = w.miner})
    -- w.wife:add():updateTimer()
    -- w.world:add(w.wife)

end

function wild_west.onClose(static, state)

    print("... and goodbye wild west world")

end
