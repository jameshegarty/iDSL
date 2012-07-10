dofile "lang.lua"

function avg(ch,x,y)
  return (cin(ch,x-c(1),y)+cin(ch,x,y)+cin(ch,x+c(1),y))/c(3)
end


function avg(ch,x,y)
  out = c(0)

  for xx = -2,2 do
    for yy = -2,2 do
      out = out + cin(ch,x+c(xx),y+c(yy))
    end
  end

  return out / c(5*5)
  --return (cin(ch,x-c(1),y)+cin(ch,x,y)+cin(ch,x+c(1),y))/c(3)
end

apply("img.bmp",avg)