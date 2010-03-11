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
--        enemy.onTimeStep = function(self)
--            local proj = Level:getSpineProjection(40)
--            local alpha = 0.99
--            local dir = proj.position - enemy:getPosition()
--            dir:normalize()
--            local cur_vel = enemy:getVelocity()
--            local vel = (cur_vel*alpha) + (dir*40*(1-alpha))
--            enemy:setVelocity(vel)
--        end 
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

function AI:flyFromBehind(enemy)
    local proj = Level:getSpineProjection(-10)
    local right = proj.forward:cross{0, 1, 0}
    enemy:setPosition(proj.position + right * 4)
    enemy:setVelocity(proj.forward * 60)
    
    local offsetv = Vector:new{0, math.random(-3, 3), 0}
    local offseth = math.random(-3, 3)
    Level:createTask(function()
        -- Wait 2 seconds
        Level:sleep(2)
        enemy.onTimeStep = function(self)
            local alpha = 0.99
            local proj = Level:getSpineProjection(40)
            local right = proj.forward:cross(Vector.UNIT_Y)
            proj.position = proj.position + offsetv
            proj.position = proj.position + right * offseth
            
            local dir = proj.position - enemy:getPosition()
            dir:normalize()
            local cur_vel = enemy:getVelocity()
            
            local vel = (cur_vel*alpha) + (dir*40*(1-alpha))
            enemy:setVelocity(vel)
        end
    end)
end
