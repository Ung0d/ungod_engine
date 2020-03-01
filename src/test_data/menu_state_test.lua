function menu_state_test.onInit(global, state)

    global.check_init = true

end

function menu_state_test.onUpdate(global, state, delta)

    global.check_update = true
    ungod.addMenuState(2, "test_data/menu_state_test2.lua")
    ungod.quitApplication()

end
