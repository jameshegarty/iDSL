function massPR(startid, endid) 

gaussPyramidThreshOptions = [1 2 3];
finalMinThreshOptions = [1 2 3];
kOptions = [1 20 40 100 200 400];
windowRadiusOptions = [20 50 100 300];
imageOptions = 1:10;    

ldAreaRatioOptions = [40 80];
ldCharSpacingOptions = [20 30];
ldNumMatchOptions = [3 6];
ldMinDimRatioOptions = [50 90];

size(gaussPyramidThreshOptions,2)*size(finalMinThreshOptions,2)*size(kOptions,2)*size(windowRadiusOptions,2)*size(imageOptions,2)*size(ldAreaRatioOptions,2)*size(ldCharSpacingOptions,2)*size(ldNumMatchOptions,2)*size(ldMinDimRatioOptions,2)

thisid = 1;

ofile = fopen(strcat('./pr',num2str(startid),'.txt'),'w');
%fprintf(ofile,'image descstr k wr gpt fmt Inter Ret Rel interR retR relR\n');
                        
for image=imageOptions
  for gaussPyramidThresh=gaussPyramidThreshOptions
    for finalMinThresh=finalMinThreshOptions
      for k=kOptions
        for windowRadius=windowRadiusOptions

          for ldAreaRatio = ldAreaRatioOptions
            for ldCharSpacing = ldCharSpacingOptions
              for ldNumMatch = ldNumMatchOptions
                for ldMinDimRatio = ldMinDimRatioOptions

                  if(thisid >= startid && thisid < endid)

		    descString = strcat(num2str(windowRadius,'%02d'),'_k',num2str(k,'%02d'),'_fmt',num2str(finalMinThresh,'%02d'),'_gpt',num2str(gaussPyramidThresh,'%02d'),'.mdr',num2str(ldMinDimRatio,'%02d'),'_cs',num2str(ldCharSpacing,'%02d'),'_nm',num2str(ldNumMatch,'%02d'),'_ar',num2str(ldAreaRatio,'%02d'));

                    descString00 = strcat('_inv00_wr',descString);
                    descString01 = strcat('_inv01_wr',descString);


                    refFile = strcat('/scratch/jhegarty/ee368project/ref/',num2str(image),'.png');
                    resFile00 = strcat('./resultsLine/',num2str(image,'%02d'),descString00,'.bmp');
                    resFile00gz = strcat(resFile00,'.gz');
                    resFile01 = strcat('./resultsLine/',num2str(image,'%02d'),descString01,'.bmp');
                    resFile01gz = strcat(resFile01,'.gz');

                    if( exist(resFile00gz)~=0 && exist(resFile01gz)~=0 )
		      
		      system(horzcat('gunzip  ',' ',resFile00gz));
                      system(horzcat('gunzip  ',' ',resFile01gz));

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
                            
                      fprintf(ofile,'%02d %s %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d %02d\n',image,descString00,k,windowRadius,gaussPyramidThresh,finalMinThresh,ldAreaRatio,ldCharSpacing,ldNumMatch,ldMinDimRatio,Inter,Retrieved,Relevant,InterR,RetrievedR,RelevantR);

		      system(horzcat('gzip ',resFile00));
		      system(horzcat('gzip ',resFile01));
        	    else
        	      display('missing');
                      display(resFile00gz);
                      display(resFile01gz);
                    end
                  end
                  thisid=thisid+1;
                end
              end
            end
          end
        end
      end
    end
  end
end
fclose(ofile);
