function input_test.onInit(global, state)

    state:inputHandler():addBinding("released", ungod.InputType.KeyboardInput, ungod.Keys.A)
    state:inputHandler():addBinding("pressed", ungod.InputType.KeyboardInput, ungod.Keys.A)

    global.check = false
    global.check2 = false

end

function input_test.onButtonPressed(global, binding, state)

    if binding == "pressed" then
        global.check = true
    end

end

function input_test.onButtonReleased(global, binding, state)

    if binding == "released" then
        global.check2 = true
    end

end

function input_test.onInitManager(global, inputmanager)

    global.click = false
    global.hovered = false
    global.exit = false
    global.release = false

    inputmanager:onMouseEnter(onMouseEnter, global)
    inputmanager:onMouseExit(onMouseExit, global)
    inputmanager:onMouseClick(onMouseClick, global)
    inputmanager:onMouseReleased(onMouseReleased, global)

end

function input_test.onMouseEnter(global, e, state)

    global.hovered = true

end

function input_test.onMouseExit(global, e, state)

    global.exit = true

end

function input_test.onMouseClick(global, e, state)

    global.click = true

end

function input_test.onMouseReleased(global, e, state)

    global.release = true

end
