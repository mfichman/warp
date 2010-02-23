-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Level = Object:new()

-- Waits for the given amount of timesteps before returning
function Level:sleep(time)
    if (time <= 0) then return end
    coroutine.yield(function()
        time = time - 1
        return time <= 0
    end)
end

-- Waits for the player to pass the given spine node before returning
function Level:waitForSpineNode(id)
    if (self:getSpineNodeId() >= id) then return end
    coroutine.yield(function()
        return self:getSpineNodeId() >= id
    end)
end

-- Waits for the player to return to the given spine node
function Level:waitForSpineNodeReverse(id)
    if (self:getSpineNodeId() < id) then return end
    coroutine.yield(function()
        return self:getSpineNodeId() < id
    end)
end


-----------------
-- MUSIC STUFF --
-----------------

-- Waits for the next beat before returning
function Level:waitForBeat()
    local beat = self:getBeat()
    coroutine.yield(function()
        return self:getBeat() ~= beat
    end)
end

-- Waits for the downbeat of the next measure before returning
function Level:waitForDownbeat()
    if (self:getBeat() % 4 == 0) then self:waitForBeat(1) end
    coroutine.yield(function()
        return self:getBeat()%4 == 0
    end)
end

-- Waits for the next set of 4 measures before returning
function Level:waitForBeatSet()
    if (self:getBeat() == 0) then self:waitForBeat(1) end
    coroutine.yield(function()
        return self:getBeat() == 0
    end)
end
