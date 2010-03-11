-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Level = Object:new()

-- Waits given number of seconds before returning
function Level:sleep(time)
    time = time * 100 -- convert from seconds to frames
    if (time <= 0) then return end
    coroutine.yield(function()
        time = time - 1
        return time <= 0
    end)
end

-- Waits for the player to pass the given position along the
-- tube before returning
function Level:waitForDistance(distance)
    if (self:getSpineNodeDistance() >= distance) then return end
    coroutine.yield(function()
        return self:getSpineNodeDistance() >= distance
    end)
end

--------------
-- AI STUFF --
--------------

function Level:createAIEnemy(type, ai_func, ...)
    enemy = Level:createEnemy(type)
    ai_func(AI, enemy, ...)
end


-----------------
-- MUSIC STUFF --
-----------------

-- Waits for a beat mod the given number == 0
function Level:waitForBeatMod(num, mod)
    local beat = self:getBeat()
    coroutine.yield(function()
        local newbeat = self:getBeat();
        return (newbeat % mod == num) and (newbeat ~= beat)
    end)
end

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


