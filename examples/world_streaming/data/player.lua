local playerGlobal = ungod.newGlobalBehavior("playerGlobal")


function playerGlobal.onStaticConstr(static)
  ungod.getInputManager():addBinding("left", ungod.InputType.KeyboardInput, ungod.Keys.A)
  ungod.getInputManager():addBinding("up", ungod.InputType.KeyboardInput, ungod.Keys.W)
  ungod.getInputManager():addBinding("down", ungod.InputType.KeyboardInput, ungod.Keys.S)
  ungod.getInputManager():addBinding("right", ungod.InputType.KeyboardInput, ungod.Keys.D)
end


function playerGlobal.onCreation(static, player)
  player.entity:add():visuals()
  player.entity:add():sprite()
  player.entity:add():spriteMetadata()
  player.entity:add():animation()
  player.entity:add():updateTimer()
  player.entity:add():movement()

  player.entity:visuals():loadTexture("data/sprites/ondafella.png")
  player.entity:visuals():loadMetadata("data/sprites/ondafella.xml")

  player.baseSpeed = 0.20
  player.maxVel = 1.6
  player.entity:movement():setBaseSpeed(player.baseSpeed)
  player.entity:movement():setMaximumVelocity(player.maxVel)

  player.entity:add():movementRigidbody()
end


function playerGlobal.onInit(static, player)
  player.entity:visuals():bindSpriteToAnimation()
  --player.entity:visuals():setAnimationState("idle")
end


function playerGlobal.onButtonDown(static, player, binding)
    if binding == "left" then
        player.entity:movement():directMovement(Vec2f.new(-0.25,0))
    elseif binding == "up" then
        player.entity:movement():directMovement(Vec2f.new(0,-0.25))
    elseif binding == "down" then
        player.entity:movement():directMovement(Vec2f.new(0,0.25))
    elseif binding == "right" then
        player.entity:movement():directMovement(Vec2f.new(0.25,0))
    end
end


function playerGlobal.onButtonReleased(static, player, binding)
    if binding == "left" then
        player.entity:movement():stop()
    elseif binding == "up" then
        player.entity:movement():stop()
    elseif binding == "down" then
        player.entity:movement():stop()
    elseif binding == "right" then
        player.entity:movement():stop()
    end
end


function playerGlobal.onDirectionChanged(static, player, old, current)
    if current == ungod.Direction.idle then
        player.entity:visuals():setAnimationState("idle")
    elseif current == ungod.Direction.left then
        player.entity:visuals():setAnimationState("left")
    elseif current == ungod.Direction.right then
        player.entity:visuals():setAnimationState("right")
    elseif current == ungod.Direction.up then
        player.entity:visuals():setAnimationState("up")
    elseif current == ungod.Direction.down then
        player.entity:visuals():setAnimationState("down")
    end
    player.entity:movementRigidbody():clearCollider()
    local upleft = Vec2f.new(player.entity:transform():getSize().x/2-30,
                              player.entity:transform():getSize().y-60)
    local downright = Vec2f.new(player.entity:transform():getSize().x/2+30,
                                player.entity:transform():getSize().y)
    player.entity:movementRigidbody():addRotatedRect(upleft, downright, 0)
end


function playerGlobal.onEnteredNewNode(static, player, worldGraph, oldNode, newNode)

  if player.transferTimer == nil or player.transferTimer:elapsedSeconds() > 2 then

    print("Transfer from " .. oldNode:getIdentifier() .. " to " .. newNode:getIdentifier())

    --should return immediately
    --if not, this may hint a design problem of the world graph
    --or a problem with the pacing of the game agents
    newNode:wait()

    --timer to prevent immediante retransfer
    player.transferTimer = ungod.makeClock()

    --copy the entity to the new node
    --note that after calling, player.entity will point to the newly created entity
    local playerCpy = newNode:getWorld(1):accomodateForeign(player.entity)
    newNode:getWorld(1):add(playerCpy)
    global.player = playerCpy
    worldGraph:getCamera():lockToEntity(playerCpy)

  end
end


local playerState = ungod.newStateBehavior("playerState")
