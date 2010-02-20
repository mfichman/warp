-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

wEntity = {}

function wEntity:new()


end



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

-- Waits for the player to return to the given spine node
function wWaitForSpineNodeReverse(id)
    if (wGetSpineNodeId() < id) then return end
    coroutine.yield(function()
        return wGetSpineNodeId() < id
    end)
end


-----------------
-- MUSIC STUFF --
-----------------

-- Waits for the next beat before returning
function wWaitForBeat()
    local beat = wGetBeat()
    coroutine.yield(function()
        return wGetBeat() ~= beat
    end)
end

-- Waits for the downbeat of the next measure before returning
function wWaitForDownbeat()
    if (wGetBeat()%4 == 0) then wWaitForBeat(1) end
    coroutine.yield(function()
        return wGetBeat()%4 == 0
    end)
end

-- Waits for the next set of 4 measures before returning
function wWaitForBeatSet()
    if (wGetBeat() == 0) then wWaitForBeat(1) end
    coroutine.yield(function()
        return wGetBeat() == 0
    end)
end
