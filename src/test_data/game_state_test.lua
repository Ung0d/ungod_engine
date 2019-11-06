function game_state_test.onInit(global, state)

    global.check_init = true

    --emit a custom event
    data = { value = 5}
    ungod.emit("nice_type", data)
    ungod.quitApplication()

end

function game_state_test.onCustomEvent(global, state, event)

    global.check_custom_event = true
    global.check_custom_event_type = (event.type == "nice_type")
    global.check_custom_event_value = (event.data.value == 5)
    ungod.quitApplication()

end
