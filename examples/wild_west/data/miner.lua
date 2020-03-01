miner = ungod.newStateBehavior("miner")

--arguments:
-- static is a table shared between all miners
-- miner is a table that can be used to store data for the particular miner instance that is shared between all method calls
-- miner provides some additional members:
-- miner.entity : the underlying ungod entity
-- miner.states : an object to control things like state switches for the entity
-- miner.world : the world the entitiy lives in
function miner.onInit(static, miner)
    print("miner comes along")
end

function miner.onUpdate(static, miner, delta)
    print("miner's mining sweet o nuggd")
end
