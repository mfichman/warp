-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Vector = Object:new()

function Vector:set(x, y, z)
    self[1] = x;
    self[2] = y;
    self[3] = z;
end

function Vector:__add(other)
    local result = Vector:new()
    result[1] = self[1] + other[1] 
    result[2] = self[2] + other[2] 
    result[3] = self[3] + other[3] 
    return result
end

function Vector:__sub(other)
    local result = Vector:new()
    result[1] = self[1] - other[1] 
    result[2] = self[2] - other[2] 
    result[3] = self[3] - other[3] 
    return result
end

function Vector:__mul(scalar)
    local result = Vector:new()
    result[1] = self[1] * scalar 
    result[2] = self[2] * scalar 
    result[3] = self[3] * scalar 
    return result
end

function Vector:scale(scalar)
    self[1] = self[1] * scalar
    self[2] = self[1] * scalar
    self[3] = self[1] * scalar
end

function Vector:cross(other)
    local result = Vector:new()
    result[1] = (self[2]*other[3] - self[3]*other[2])
    result[2] = (self[3]*other[1] - self[1]*other[3])
    result[3] = (self[1]*other[2] - self[2]*other[1])
    return result
end

function Vector:dot(other)
    return self[1] * other[1] + self[2] * other[2] + self[3] * other[3]
end

function Vector:length()
    return math.sqrt(self[1]*self[1] + self[2]*self[2] + self[3]*self[3])
end

function Vector:normalize()
    local length = self:length()
    if length == 0 then
        print "Warning: trying to normalize vector of length zero"
    end
    self[1] = self[1] / length
    self[2] = self[2] / length
    self[3] = self[3] / length
    return self
end

function Vector:normalizedCopy()
    local result = Vector:new()
    local length = self:length()
    if length == 0 then
        print "Warning: trying to normalize vector of length zero"
    end
    result[1] = self[1] / length
    result[2] = self[2] / length
    result[3] = self[3] / length
    return result
end
--
---- algorithm adapted from wikipedia article http://en.wikipedia.org/wiki/Quaternion_rotation
--function Vector:applyRotation(q)
--    local t2  =  q[1] * q[2]
--    local t3  =  q[1] * q[3]
--    local t4  =  q[1] * q[4]
--    local t5  = -q[2] * q[2]
--    local t6  =  q[2] * q[3]
--    local t7  =  q[2] * q[4]
--    local t8  = -q[3] * q[3]
--    local t9  =  q[3] * q[4]
--    local t10 = -q[4] * q[4]
--    local newx = 2*( (t8 + t10) * self[1] + (t6 -  t4) * self[2] + (t3 + t7) * self[3]) + self[1]
--    local newy = 2*( (t4 +  t6) * self[1] + (t5 - t10) * self[2] + (t9 - t2) * self[3]) + self[2]
--    local newz = 2*( (t7 -  t3) * self[1] + (t2 -  t9) * self[2] + (t5 + t8) * self[3]) + self[3]
--    self:set(newx, newy, newz)
--end
