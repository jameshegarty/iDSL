function table_print (tt, indent, done)
  done = done or {}
  indent = indent or 0
  if type(tt) == "table" then
    local sb = {}
    for key, value in pairs (tt) do
      table.insert(sb, string.rep (" ", indent)) -- indent it
      if type (value) == "table" and not done [value] then
        done [value] = true
	table.insert(sb, key);
	table.insert(sb, "=");
        table.insert(sb, "{\n");
        table.insert(sb, table_print (value, indent + 2, done))
        table.insert(sb, string.rep (" ", indent)) -- indent it
        table.insert(sb, "}\n");
      elseif "number" == type(key) then
        table.insert(sb, string.format("\"%s\"\n", tostring(value)))
      else
        table.insert(sb, string.format(
            "%s = \"%s\"\n", tostring (key), tostring(value)))
       end
    end
    return table.concat(sb)
  else
    return tt .. "\n"
  end
end

function to_string( tbl )
    if  "nil"       == type( tbl ) then
        return tostring(nil)
    elseif  "table" == type( tbl ) then
        return table_print(tbl)
    elseif  "string" == type( tbl ) then
        return tbl
    else
        return tostring(tbl)
    end
end

function serialize(tbl) print(to_string(tbl)) end


function deepcopy(object)
    local lookup_table = {}
    local function _copy(object)
        if type(object) ~= "table" then
            return object
        elseif lookup_table[object] then
            return lookup_table[object]
        end
        local new_table = {}
        lookup_table[object] = new_table
        for index, value in pairs(object) do
            new_table[_copy(index)] = _copy(value)
        end
        return setmetatable(new_table, getmetatable(object))
    end
    return _copy(object)
end

-------------

astmt = {}

ops={'add','sub','div','eq','le'}

xloc = setmetatable({type='xloc'},astmt)
yloc = setmetatable({type='yloc'},astmt)
cloc = setmetatable({type='cloc'},astmt)

for _,v in pairs(ops) do
  astmt['__'..v]=function(op1,op2) 
    if type(op1)~="table" then print("Error: l must be a iDSL value") end
    if type(op2)~="table" then print("Error: r must be a iDSL value") end
    return setmetatable({type='op',op=v,l=op1,r=op2},astmt) 
  end
end

astmt.__concat = function(op1,op2) return cblock{op1,op2} end

function c(x) return setmetatable({type='const',value=x},astmt) end
function cselect(icond,it,iif) return setmetatable({type='if',cond=icont,t=it,f=iif},astmt) end

function cin(chan,xx,yy) 
  if type(chan)~="table" then print("Error: channel must be a iDSL value") end
  if type(xx)~="table" then print("Error: x must be a iDSL value") end
  if type(yy)~="table" then print("Error: y must be a iDSL value") end

  return setmetatable({type='in',channel=chan,['x']=xx,['y']=yy},astmt) 
end

function cout(chan,xx,yy) return setmetatable({type='out',channel=chan,['x']=xx,['y']=yy},astmt) end
function cvar(nam) return setmetatable({type='var',name=nam},astmt) end

function evalOp(x)
  local v1 = eval(x.l)
  local v2 = eval(x.r)
  
  if(x.op=='add') then
    return c(v1.value+v2.value)
  elseif(x.op=='sub') then
    return c(v1.value-v2.value)
  elseif(x.op=='div') then
    return c(v1.value/v2.value)
  else
    print("Error: op "..x.op.." not implemented")
  end
end

function sampleImage(cc,xx,yy)
  xx = xx % width[0]
  yy = yy % height[0]
  --assert(cc >=0 and cc < channels[0])
  if(cc <0 or cc > channels[0]) then
    print("Error, bad channel when sampling image "..cc)
  end

  return image[yy*width[0]*channels[0]+xx*channels[0]+cc]
end

function eval(ast)
  if(ast.type=='op') then
    return evalOp(ast)
  elseif(ast.type=='const') then
    return ast
  elseif(ast.type=='xloc') then
    return {type='const',value=currentXloc}
  elseif(ast.type=='yloc') then
    return {type='const',value=currentYloc}
  elseif(ast.type=='cloc') then
    return {type='const',value=currentCloc}
  elseif(ast.type=='in') then
    local lx = eval(ast['x'])
    local ly = eval(ast['y'])
    local lc = eval(ast.channel)
    return {type='const',value=sampleImage(lc.value,lx.value,ly.value)}
  else 
    print("Error: type "..ast.type.." not implemented")
  end
end

function stencilSizeOp(ast,size)
  local size,v1 = stencilSize(ast.l,size)
  local size,v2 = stencilSize(ast.r,size)
  
  if(ast.op=='add') then
    return size,c(v1.value+v2.value)
  elseif(ast.op=='sub') then
    return size,c(v1.value-v2.value)
  else
    print("Error: op "..ast.op.." not implemented")
  end
end

function stencilSize(ast,size)
  size = size or {['inp']={xmin=10000,xmax=-10000,ymin=10000,ymax=-10000},['out']={xmin=10000,xmax=-10000,ymin=10000,ymax=-10000}}

  if (ast.type=='block') then
    for k,v in pairs(ast.body) do size=stencilSize(v,size) end
    return size,ast
  elseif(ast.type=='set') then
    size = stencilSize(ast.l,size)
    size = stencilSize(ast.r,size)
    return size,ast
  elseif(ast.type=='xloc') then
    return size,{type='const',value=0}
  elseif(ast.type=='const') then
    return size,ast
  elseif(ast.type=='yloc') then
    return size,{type='const',value=0}
  elseif(ast.type=='out') then
    _,astx = stencilSize(ast.x,size)
    _,asty = stencilSize(ast.y,size)
    if(astx.value<size.out.xmin) then size.out.xmin=astx.value end
    if(astx.value>size.out.xmax) then size.out.xmax=astx.value end
    if(asty.value<size.out.ymin) then size.out.ymin=asty.value end
    if(asty.value>size.out.ymax) then size.out.ymax=asty.value end
    return size,ast
  elseif(ast.type=='in') then
    _,astx = stencilSize(ast.x,size)
    _,asty = stencilSize(ast.y,size)
    if(astx.value<size.inp.xmin) then size.inp.xmin=astx.value end
    if(astx.value>size.inp.xmax) then size.inp.xmax=astx.value end
    if(asty.value<size.inp.ymin) then size.inp.ymin=asty.value end
    if(asty.value>size.inp.ymax) then size.inp.ymax=asty.value end
    return size,c(0)
  elseif(ast.type=='op') then
    return stencilSizeOp(ast,size)
  end
  
  print("Error: type "..ast.type.." not implemented")

end


outTable={}
varTable={}

function genDagOp(ast)
  local v1 = genDag(ast.l)
  local v2 = genDag(ast.r)

  --if(v1==nil or v2==nil) then print("error");serialize(ast); end

  if(v1.type=='const' and v2.type=='const') then
    if(ast.op=='add') then
      return c(v1.value+v2.value)
    elseif(ast.op=='sub') then
      return c(v1.value-v2.value)
    else
      print("DAG Error: op "..ast.op.." not implemented")
    end
  end

  ast.l = v1
  ast.r = v2
  return ast
end

function genDag(ast)
  if(ast.type=='set') then
    if(ast.l.type=='out') then
      astc = genDag(ast.l.channel)
      astx = genDag(ast.l.x)
      asty = genDag(ast.l.y)
      outTable[astc.value]=outTable[astc.value] or {}
      outTable[astc.value][astx.value]=outTable[astc.value][astx.value] or {}
      outTable[astc.value][astx.value][asty.value]=genDag(ast.r)
    elseif(ast.l.type=='var') then
      varTable[ast.l.name] = genDag(ast.r)
    end
    
    return c(0)
  elseif (ast.type=='block') then
    for k,v in pairs(ast.body) do genDag(v) end
    return c(0)
  elseif (ast.type=='for') then
    local i = genDag(ast.s).value
    local e = genDag(ast.e).value
    local vn = ast.v.name

    print("for "..i.." "..e.." "..vn)

    while(i<=e) do
      print(vn..'='..i)
      varTable[vn]=c(i)    
      tblock = deepcopy(ast.block)
      genDag(tblock)
      i = i+1
    end
    return c(0)
  elseif(ast.type=='xloc') then
    return c(0)
  elseif(ast.type=='yloc') then
    return c(0)
  elseif(ast.type=='in') then
    ast.channel = genDag(ast.channel)
    ast.x = genDag(ast.x)
    ast.y = genDag(ast.y)
    return ast
  elseif(ast.type=='const') then
    return ast
  elseif(ast.type=='op') then
    return genDagOp(ast)
  elseif(ast.type=='var') then
    print('get var '..ast.name)
    return varTable[ast.name]
  elseif(ast.type=='out') then
    astc = genDag(ast.channel)
    astx = genDag(ast.x)
    asty = genDag(ast.y)
    --print(astc.value .. " " ..astx.value .. " " .. asty.value)
    --serialize(outTable[astc.value][astx.value][asty.value])
    if(outTable[astc.value][astx.value][asty.value]==nil) then
      print("ERROR: out value at index "..astc.value .. " " ..astx.value .. " " .. asty.value.." not set yet!")
    end

    return outTable[astc.value][astx.value][asty.value]
  end

  print("DAG Error: type "..ast.type.." not implemented")
end

opToSymb={}
opToSymb['add']='+'
opToSymb['sub']='-'
opToSymb['div']='/'

function astToCodeHuman(ast)
  res = ""

  if(ast.type=='op') then
    res = astToCodeHuman(ast.l)..opToSymb[ast.op]..astToCodeHuman(ast.r)
  elseif(ast.type=='in') then
    res = "cin("..astToCodeHuman(ast.channel)..","..astToCodeHuman(ast.x)..","..astToCodeHuman(ast.y)..")"
  elseif(ast.type=='const') then
    res = tostring(ast.value)
  elseif(ast.type=='var') then
    res = ast.name
  elseif(ast.type=='xloc') then
    res = 'x'
  elseif(ast.type=='yloc') then
    res = 'y'
  elseif(ast.type=='cloc') then
    res = 'c'
  else 
    print("astToCodeHuman error: type "..ast.type.." not implemented")
  end

  return res
end

function astToCode(ast)
  res = ""

  if(ast.type=='op') then
    res = "("..astToCode(ast.l)..")"..opToSymb[ast.op].."("..astToCode(ast.r)..")"
  elseif(ast.type=='in') then
    res = "cin("..astToCode(ast.channel)..","..astToCode(ast.x)..","..astToCode(ast.y)..")"
  elseif(ast.type=='const') then
    res = tostring(ast.value)
  elseif(ast.type=='var') then
    res = ast.name
  elseif(ast.type=='xloc') then
    res = 'x'
  elseif(ast.type=='yloc') then
    res = 'y'
  elseif(ast.type=='cloc') then
    res = 'c'
  else 
    print("astToCode error: type "..ast.type.." not implemented")
  end

  return res
end

function apply(imgfile, func)
  local ffi = require("ffi")
  ffi.cdef[[
    bool saveImageUC(const char *file, int width, int height, int channels, unsigned char *data);
    unsigned char* loadImageUC(const char *file, int* width, int* height, int *channels);
    int printf(const char *fmt, ...);
    void * malloc ( size_t size );
  ]]

  width = ffi.new("int[1]",0)
  height = ffi.new("int[1]",0)
  channels = ffi.new("int[1]",0)

  util = ffi.load("../examples/libutil.so")

  print("START"..imgfile)
  image=util.loadImageUC(imgfile,width,height,channels)
  print("END"..width[0].." "..height[0].." "..channels[0])

  dag = func(cloc,xloc,yloc)
  --serialize(dag)
  codeString = astToCode(dag)
  print(codeString)
  print(astToCodeHuman(dag))
  local code = loadstring("return "..codeString)

  --local output = ffi.C.malloc(width[0]*height[0]*channels[0])
  --output={}
  local output = ffi.new("unsigned char[?]",width[0]*height[0]*channels[0])

  c = function(x) return x end
  cin = function(cc,xx,yy) return sampleImage(cc,xx,yy) end
  
  for yy = 0,height[0]-1 do
--  for yy = 0,100 do
    for xx = 0,width[0]-1 do
      for cc = 0,channels[0]-1 do
        currentXloc = xx
	currentYloc = yy
	currentCloc = cc
	  -- eval using our interpreter (slow)
--        output[yy*width[0]*channels[0]+xx*channels[0]+cc] = eval(dag).value
	  
	  -- generate dag each time
--        output[yy*width[0]*channels[0]+xx*channels[0]+cc] = func(cc,xx,yy)

	  -- eval using luajitted dag
	  x = xx; y = yy; c = cc
	  output[yy*width[0]*channels[0]+xx*channels[0]+cc] = code()
      end
    end
  end

  util.saveImageUC("out.bmp",width[0],height[0],channels[0],output)
end