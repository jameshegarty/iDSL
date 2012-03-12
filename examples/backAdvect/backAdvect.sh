if [ $# -ne 3 ]; then echo "Usage: backAdvect.sh input.png vectors.flo output.png"; exit; fi

ImageStack -load $1 -load $2 -evalchannels "(x+[x,y])/width" "(y+[x,y])/height" -warp -save $3