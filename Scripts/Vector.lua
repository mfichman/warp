-- Warp: CS248 Final Project
-- Matt Fichman & Francesco Georg

Vector = Object:new()

function Vector:set(x, y, z)
    self[1] = x;
    self[2] = y;
    self[3] = z;
end

function Vector:__add(other)
    local result = Object:new(Vector)
    result[1] = self[1] + other[1] 
    result[2] = self[2] + other[2] 
    result[3] = self[3] + other[3] 
    return result
end

function Vector:__sub(other)
    local result = Object:new(Vector)
    result[1] = self[1] - other[1] 
    result[2] = self[2] - other[2] 
    result[3] = self[3] - other[3] 
    return result
end

function Vector:cross(other)
    local result = Object:new(Vector)
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
