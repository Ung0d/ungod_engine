
local DIM = 10
local CHUNK_SIZE = 2560
local ENTITIES_PER_CHUNK = 20

global = {}

local GRAPH_UPD_INTERVAL = 3
local graphUpdateTimer = ungod.makeClock()

function gamestate.onInit(static, state)

    print("Init World Streaming state...")

    --genWorld(state)

    state:behavior():loadBehaviorScript("data/player.lua")

    local initPos = Vec2f.new(CHUNK_SIZE/2, CHUNK_SIZE/2)

    state:load("data/state_sav")
    state:worldGraph():updateReferencePosition(initPos)
    state:worldGraph():setDistance(1)

    local initNode = state:worldGraph():getNodeByPosition(initPos)
    if initNode then
      initNode:wait()
      local world = initNode:getWorld(1)
      local player = world:createEntity("player")
      player:transform():setPosition(initPos)
      world:add(player)
      state:worldGraph():getCamera():lockToEntity(player)
      global.player = player
    else
      ungod.error("Can not get the initial world node.")
    end

    state:toggleDebugmode(true)

    state:worldGraph():getCamera():setZoom(1)

end

function gamestate.onUpdate(static, state)

  if graphUpdateTimer:elapsedSeconds() > GRAPH_UPD_INTERVAL then
    state:updateWorldGraph()
    graphUpdateTimer:restart()
  end

end

function genWorld(state)

  local chunk_grid = {}

  for x=0,DIM-1 do
    chunk_grid[x] = {}
    for y=0,DIM-1 do
      local name = "node" .. x .. "_" .. y
      local node = state:worldGraph():createNode(state, name, "data/nodes/" .. name .. ".world")
      node:setSize(Vec2f.new(CHUNK_SIZE, CHUNK_SIZE))
      node:setPosition(Vec2f.new(x*CHUNK_SIZE, y*CHUNK_SIZE))
      --activate the current node
      --since we have no edges yet, this will unload the last activated node automatically
      state:worldGraph():updateReferencePosition(Vec2f.new(CHUNK_SIZE/2 + x*CHUNK_SIZE, CHUNK_SIZE/2 + y*CHUNK_SIZE))

      local ground = node:addWorld()
      local tilemap = ground:createWorldObject()
      tilemap:add():spriteMetadata()
      tilemap:add():tilemap()
      tilemap:visuals():loadTexture("data/sprites/ground.png")
      tilemap:visuals():loadMetadata("data/sprites/ground.xml")
      tilemap:tilemap():setTileDims(128, 128, {"beach_dirt", "beach_dirt2", "beach_dirt3"}) --define a keymap that maps indices to metadata keys
      tilemap:tilemap():setZeroTiles(20,20) --allocates a 10x10 tilemap filled with zeros
      tilemap:tilemap():floodfill(0, 0, {0,1,2}) --fill the tilemap with tiles choosing randomly from 0,1,2
      ground:add(tilemap)

      local world = node:addWorld()
      local wos = world:createWorldObjects(ENTITIES_PER_CHUNK)
      for _, e in pairs(wos) do
        e:add():sprite()
        e:add():spriteMetadata()
        e:visuals():loadTexture("data/sprites/pale_trees.png")
        e:visuals():loadMetadata("data/sprites/pale_trees.xml")
        local treeID = ungod.randInt(1,7)
        e:visuals():setSpriteTextureRect("tree_"..treeID)
        e:transform():setPosition(Vec2f.new(CHUNK_SIZE*ungod.randFloat(0,1), CHUNK_SIZE*ungod.randFloat(0,1)))
        e:add():movementRigidbody()
        local upleft = Vec2f.new(e:transform():getSize().x/2-30,e:transform():getSize().y-60)
        local downright = Vec2f.new(e:transform():getSize().x/2+30,e:transform():getSize().y)
        e:movementRigidbody():addRotatedRect(upleft, downright, 0)
        world:add(e)
      end
      chunk_grid[x][y] = node
      node:save()
    end
  end

  -- set connetions between adjacent nodes
  for x=0,DIM-1 do
    for y=0,DIM-1 do
      if y > 0 then
        if x > 0 then state:worldGraph():connect(chunk_grid[x-1][y-1], chunk_grid[x][y]) end
        state:worldGraph():connect(chunk_grid[x][y-1], chunk_grid[x][y])
        if x < DIM-1 then state:worldGraph():connect(chunk_grid[x+1][y-1], chunk_grid[x][y]) end
      end
      if x > 0 then state:worldGraph():connect(chunk_grid[x-1][y], chunk_grid[x][y]) end
    end
  end

  state:save("data/state_sav", true)
end

function gamestate.onClose(static, state)

    --global.player:getWorld():remove(global.player)
    --state:save("data/state_sav", true)
    print("... closing World Streaming state.")

end
