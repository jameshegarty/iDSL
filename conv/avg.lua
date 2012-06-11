dofile 'lang.lua'

avg = cblock{cset(cout(c(0),x,y),cin(c(0),x-c(1),y) + cin(c(0),x,y) + cin(c(0),x+c(1),y))}


avg = cblock{
cset(cout(c(0),x,y),cin(c(0),x-c(1),y) + cin(c(0),x,y) + cin(c(0),x+c(1),y)),
--cset(cout(c(0),x,y),c(0)),
cfor(cvar('i'),c(-1),c(1),cset(cout(c(0),x,y),cout(c(0),x,y)+cin(c(0),x,y+cvar('i')))),
cset(cvar('lol'),cout(c(0),x,y)),
cset(cout(c(1),x,y),cvar('lol'))
}


avg = cblock{
cset(cout(c(0),x,y),c(0)),


cfor(cvar('i'),c(-1),c(1),
  cfor(cvar('j'),c(-1),c(1),
    cset(cout(c(0),x,y),cout(c(0),x,y)+cin(c(0),x+cvar('i'),y+cvar('j')))
  )
)

}


--serialize(stencilSize(avg))

genDag(avg)

serialize(outTable)
serialize(varTable)
print(astToCode(outTable[0][0][0]))
print(astToCode(outTable[1][0][0]))