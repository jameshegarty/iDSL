function massPR() 

gaussPyramidThreshOptions = [1 2 3];
finalMinThreshOptions = [1 2 3];
kOptions = [1 20 40 100 200 400];
windowRadiusOptions = [20 50 100 300];
imageOptions = 1:10;    


thisid = 1;

ofile = fopen('./pr.txt','w');
fprintf(ofile,'image descstr k wr gpt fmt Inter Ret Rel interR retR relR\n');
                        
for image=imageOptions
  for gaussPyramidThresh=gaussPyramidThreshOptions
    for finalMinThresh=finalMinThreshOptions
      for k=kOptions
        for windowRadius=windowRadiusOptions

%num2str(image,'%02d'),
  descString00 = strcat('_inv00_wr',num2str(windowRadius,'%02d'),'_k',num2str(k,'%02d'),'_fmt',num2str(finalMinThresh,'%02d'),'_gpt',num2str(gaussPyramidThresh,'%02d'));
  descString01 = strcat('_inv01_wr',num2str(windowRadius,'%02d'),'_k',num2str(k,'%02d'),'_fmt',num2str(finalMinThresh,'%02d'),'_gpt',num2str(gaussPyramidThresh,'%02d'));

            %display(descString00);

            refFile = strcat('/scratch/jhegarty/ee368project/ref/',num2str(image),'.png');
            resFile00 = strcat('./results/',num2str(image,'%02d'),descString00,'/res.bmp');
            resFile01 = strcat('./results/',num2str(image,'%02d'),descString01,'/res.bmp');

            if( exist(resFile00)~=0 && exist(resFile01)~=0 )

	      ref = logical(imread(refFile));
              refL = bwlabel(ref);
              out00 = logical(rgb2gray(imread(resFile00)));
              out01 = logical(rgb2gray(imread(resFile01)));
              out = out00 | out01;
              outL = bwlabel(out);
              outL = outL+max(max(refL));
                            
              Inter = sum(sum(ref&out));
              Retrieved = sum(sum(out));
              Relevant = sum(sum(ref));
assert(Inter <= Retrieved);
assert(Inter <= Relevant);

              [InterR, RetrievedR, RelevantR] = compRegions(outL,refL);
                            
              fprintf(ofile,'%02d %s %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d\n',image,descString00,k,windowRadius,gaussPyramidThresh,finalMinThresh,Inter,Retrieved,Relevant,InterR,RetrievedR,RelevantR);
	    else
	      display('missing');
              display(resFile00);
              display(resFile01);
            end


        end
      end
    end
  end
end
fclose(ofile);
