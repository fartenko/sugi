
-- Initialization function
function __init__()
   t = 0
   c = 0
end


-- Update function
function __tick__()
    cls(c)
    -- print("__tick__ " ..t.. " - " ..c, 32, 0, 7)
    -- print("Some other string", 0, 16, 7)

    list = {
        x = 1,
        y = 2,
        sub = {
            1, 2, 3, 4
        }
    }

    print(list.sub[2], 1, 1, 7)

    t = t + 1
    if t % 60 == 0 then 
        c = c + 1
    end
end
