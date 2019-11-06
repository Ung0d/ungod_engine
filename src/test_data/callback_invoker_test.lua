function callback_invoker_test.onInit(global, testme)

    global.test1 = 2 + testme

end

function callback_invoker_test.onExit(global, testme)

    global.test2 = testme + 13

end
