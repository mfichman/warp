-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

AI = Object:new()

function AI:spiral(enemy, distance, diameter)
    distance = distance or 50 
    diameter = diameter or 6 

    enemy.timeElapsed = 0

    local proj = Level:getSpineProjection(-10)
    enemy:setPosition(proj.position)
    enemy:setVelocity(proj.forward * 60)

    Level:createTask(function()
        Level:sleep(1)
        enemy.onTimeStep = function(self)
            local proj = Level:getSpineProjection(distance)
            enemy.timeElapsed = enemy.timeElapsed + .02
            local target_position = proj.position + Level:getPlayerOrientation() * {diameter * math.sin(enemy.timeElapsed), diameter * math.cos(enemy.timeElapsed), 0}
  
            local alpha = 0.6
            local dir = target_position - enemy:getPosition()
            dir:normalize()
            local cur_vel = enemy:getVelocity()
            local vel = (cur_vel*alpha) + (dir*40*(1-alpha))
            enemy:setVelocity(vel)
        end 
    end)
end

function AI:rammingSpeed(enemy)
    local proj = Level:getSpineProjection(100)
    enemy:setPosition(proj.position)
    
    enemy.onTimeStep = function(self)
        
    end

end

function AI:flyFromBehind(enemy)
    local proj = Level:getSpineProjection(-10)
    local left = proj.forward:cross{0, 1, 0}
    left:normalize()
    enemy:setPosition(proj.position)
    enemy:setVelocity(proj.forward * 60)
    enemy:setOrientation(proj.forward)
    
    local offsetv = Vector:new{0, math.random(-3, 3), 0}
    local offseth = math.random(-3, 3)
    Level:createTask(function()
        -- Wait 2 seconds
        Level:sleep(2)
        local onTimeStep = enemy.onTimeStep;
        enemy.onTimeStep = function(self)
            local alpha = 0.99
            local proj = Level:getSpineProjection(40)
            local left = proj.forward:cross(Vector.UNIT_Y) -- note: unit_Y is not perpendicular
            proj.position = proj.position + offsetv
            proj.position = proj.position + left * offseth
            
            local dir = proj.position - enemy:getPosition()
            dir:normalize()
            local cur_vel = enemy:getVelocity()
            
            local vel = (cur_vel*alpha) + (dir*50*(1-alpha))
            enemy:setVelocity(vel)

            -- set orientation
            local left, up, forward = Level:getPlayerOrientation():toAxes()
            local quat = Quaternion:new()
            quat:fromAxes(left * -1, up, forward * -1) 

            enemy:setOrientation(quat)

            onTimeStep(enemy)
        end
    end)
end
