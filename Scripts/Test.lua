-- Warp: CS248 Final Project
-- Copyright (c) 2010 Matt Fichman


-- Waits for the given amount of timesteps before returning
function wSleep(time)
    if (time <= 0) then return end
    coroutine.yield(function()
        time = time - 1
        return time <= 0
    end)
end

-- Waits for the player to pass the given spine node before returning
function wWaitForSpineNode(id)
    if (wGetSpineNodeId() >= id) then return end
    coroutine.yield(function()
        return wGetSpineNodeId() >= id
    end)
end

-- Waits for the next beat before returning
function wWaitForBeat()
    --local beat = wGetBeat()
    --coroutine.yield(function()
    --    return wGetBeat() >= beat
    --end)
end


print("Sleeping")
wSleep(100)
print("Awake")

print("Waiting")
wWaitForSpineNode(10)
print("Awake")

wSetLight("Light", {diffuse={1, 0, 0}})

