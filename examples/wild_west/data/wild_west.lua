function wild_west.onInit(global, state)

    print("hello wild west world...")
    state:expire()

end

function wild_west.onClose(global, state)

    print("... and goodbye")

end
