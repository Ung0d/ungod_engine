function input_test.onInit(global, state)

    global.check = false
    global.check2 = false

end

function input_test.onUpdate(global, state, delta)

      global.check = true

end

function input_test.onClose(global, state)

      global.check2 = true

end
