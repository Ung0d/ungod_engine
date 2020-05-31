wife_global = ungod.newGlobalBehavior("wife_global")

function wife_global.onInit(static, wife)
    ungod.listen(wife.entity, "hiHoneyAmHome")
    ungod.listen(wife.entity, "stewReady")
end

function wife_global.onUpdate(static, wife, delta)
  if ungod.randProb() < 0.1 then
    wife.states:switchTo("visit_bathroom")
  end
end

function wife_global.onSerialize(static, wife, context)
  context:serializeString("name", wife)
  context:serializeEntity("husband", wife)
end

function wife_global.onDeserialize(static, wife, context)
  context:deserializeString("name", wife)
  context:deserializeEntity("husband", wife)
end

function wife_global.onCustomEvent(static, wife, event)
  if event.type == "hiHoneyAmHome" then
    if event.data.sender == wife.husband then
      wife.states:switchTo("cook_stew")
    end
  elseif event.type == "stewReady" then
    if event.data.sender == wife.entity then
      print(wife.name .. ": Stew ready! let's eat")
      ungod.emit("letsEat", {sender = wife.entity})
      wife.states:switchTo("eating")
    end
  end
end

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
do_housework = ungod.newStateBehavior("do_housework")

function do_housework.onInit(static, wife)
  print(wife.name .. ": Here we go, back to work.")
end

function do_housework.onUpdate(static, wife, delta)
  print(wife.name .. ": Moppin' the floor.")
end

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
visit_bathroom = ungod.newStateBehavior("visit_bathroom")

function visit_bathroom.onInit(static, wife)
  print(wife.name .. ": Need to powda ma pretty li'l nose.")
  wife.states:returnToPrevious()
end

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
cook_stew = ungod.newStateBehavior("cook_stew")

function cook_stew.onInit(static, wife)
  print(wife.name .. ": Hi Honey! Let me make you some of ma fine stew.")
  ungod.emit("stewReady", {sender = wife.entity}, 0.5)
  wife.states:returnToPrevious()
end

function cook_stew.onExit(static, wife)
  print(wife.name .. ": Puttin' the stew in the oven.")
end

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
eating = ungod.newStateBehavior("eating")

function eating.onInit(static, wife)
    print(wife.name .. ": Nom!")
    ungod.quitApplication()
end
