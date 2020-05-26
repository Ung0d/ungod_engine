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
end


function playerGlobal.onInit(static, player)

  player.entity:visuals():loadTexture("data/sprites/ondafella.png")
  player.entity:visuals():loadMetadata("data/sprites/ondafella.xml")
  player.entity:visuals():bindSpriteToAnimation()
  player.entity:visuals():setAnimationState("idle")

  player.entity:add():movement()
  player.baseSpeed = 0.20
  player.maxVel = 1.6
  player.entity:movement():setBaseSpeed(player.baseSpeed)
  player.entity:movement():setMaximumVelocity(player.maxVel)

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


local playerState = ungod.newStateBehavior("playerState")
