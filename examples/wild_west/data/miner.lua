-- entities can have a global state (is invoked all the time)
-- along with switchable states of which always the activate state is invoked (after the global one)
-- ungod.newGlobalBehavior(name) creates the global state (optionally)
-- ungod.newStateBehavior(name) creates switchable states
-- the first switchable state create will be the initial state the entity is in

miner_global = ungod.newGlobalBehavior("miner_global")

--arguments:
-- static is a table shared between all miners
-- miner is a table that can be used to store data for the particular miner instance that is shared between all method calls
-- miner provides some additional members:
-- miner.entity : the underlying ungod entity
-- miner.states : an object to control things like state switches for the entity
-- miner.world : the world the entitiy lives in
function miner_global.onInit(static, miner)
    print("miner " .. miner.name .. " comes along")
    miner.location = "street"
    miner.num_gold = 0
    miner.money_in_bank = 0
    miner.thirst = 0
    miner.fatigue = 0
end

function miner_global.onUpdate(static, miner, delta)
    miner.thirst = miner.thirst + 1
end

function miner_global.onSerialize(static, miner, node, context)
  context:serialize("name", miner.name)
end

function miner_global.onDeserialize(static, miner, node, context)
  miner.name = context:deserializeString("name", node)
end

function miner_global.changeLocation(miner, location)
    if miner.location ~= location then
      print(miner.name .. " is entering " .. location)
      miner.location = location
    end
end


-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
enter_mine_and_dig_for_nugget = ungod.newStateBehavior("enter_mine_and_dig_for_nugget")

function enter_mine_and_dig_for_nugget.onInit(static, miner)
    miner_global.changeLocation(miner, "the mine")
end

function enter_mine_and_dig_for_nugget.onUpdate(static, miner, delta)
    if ungod.randProb() < 0.7 then
      miner.num_gold = miner.num_gold + 1
      print(miner.name .. " is pickin' up a nugget")
    end
    miner.fatigue = miner.fatigue + 1
    if miner.thirst >= 6 then
      print(miner.name .. ": Boy, ah sure is thursty! Walkin' to the saloon!")
      miner.states:switchTo("visit_saloon")
    elseif miner.num_gold >= 10 then
      print(miner.name .. ": Ah'm leavin' the gold mine with mah pockets full o' sweet gold")
      miner.states:switchTo("visit_bank")
    end
end



-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
visit_bank = ungod.newStateBehavior("visit_bank")

function visit_bank.onInit(static, miner)
    miner_global.changeLocation(miner, "the bank")
end

function visit_bank.onUpdate(static, miner, delta)
  miner.money_in_bank = miner.money_in_bank + miner.num_gold
  miner.num_gold = 0
  print(miner.name .. ": Depositin' gold. Total savings now: " .. miner.money_in_bank)
  if miner.money_in_bank > 20 then
    print(miner.name .. ": Woohoo! Rich enough for now. Back home to mah li'l lady")
    miner.states:switchTo("go_home")
  else
    miner.states:switchTo("enter_mine_and_dig_for_nugget")
  end
end


-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
visit_saloon = ungod.newStateBehavior("visit_saloon")

function visit_saloon.onInit(static, miner)
    miner_global.changeLocation(miner, "the saloon")
end

function visit_saloon.onUpdate(static, miner, delta)
    print(miner.name .. ": That's mighty fine slippin' liquor!")
    miner.thirst = 0
    print(miner.name .. ": Leavin' the saloon, feelin' good")
    miner.states:switchTo("enter_mine_and_dig_for_nugget")
end


-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
go_home = ungod.newStateBehavior("go_home")

function go_home.onInit(static, miner)
    miner_global.changeLocation(miner, "home")
    print(miner.name .. ": Home sweet home!")
    ungod.emit("hiHoneyAmHome", {sender = miner.entity})
    miner.letsEatLink = ungod.listen(miner.entity, "letsEat")
end

function go_home.onUpdate(static, miner, delta)
    if miner.fatigue > 2 then
      miner.fatigue = miner.fatigue - 2
      print(miner.name .. ": ZzZzZz")
    else
      print(miner.name .. ": What a God-darn fantastic nap! Time to find more gold!")
    end
end

function go_home.onCustomEvent(static, miner, evt)
  if evt.type == "letsEat" then
    miner.states:switchTo("eating")
  end
end

function go_home.onExit(static, miner)
  miner.letsEatLink:disconnect()
  miner.letsEatLink = nil
end

-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
eating = ungod.newStateBehavior("eating")

function eating.onInit(static, miner)
    miner_global.changeLocation(miner, "home")
    print(miner.name .. ": Nom!")
    ungod.quitApplication()
end
