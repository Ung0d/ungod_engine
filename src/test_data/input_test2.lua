function input_test2.onInit(global, state)

    --set up test check flags
    global.click = false
    global.hovered = false
    global.exit = false
    global.release = false

    --create a world
    global.world = state:addWorld()
    global.world:init(-2000,-2000,4000,4000)

    global.e = global.world:createEntity()
    global.e:add():MovementRigidbody()
    global.world:transform():setPosition(global.e, Vec2f.new(40, 40))
    global.world:rigidbody():addMovementCollider(global.e, Vec2f.new(0, 0), Vec2f.new(10, 10))
    global.world:add(global.e)

end

function input_test2.onMouseEnter(global, state, e)

    global.hovered = true

end

function input_test2.onMouseExit(global, state, e)

    global.exit = true

end

function input_test2.onMouseClick(global, state, e)

    global.click = true

end

function input_test2.onMouseReleased(global, state, e)

    global.release = true

end
