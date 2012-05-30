function [InterR, RetrievedR, RelevantR]=compRegions(outL,refL)
  RelevantR = size(unique(refL),1);

  retCount = max(max(unique(outL)));
  relCount = max(max(unique(refL)));

              tsize = size(refL,1)*size(refL,2);
              relevantCount=zeros(1,relCount);
              renameList=zeros(1,retCount);
              deadList=zeros(1,retCount);

              for i=1:tsize
                refLabel = refL(i);
                retLabel = outL(i);
                if(refLabel~=0 && retLabel~=0)
                  if(renameList(retLabel)==0)
                    renameList(retLabel)=refLabel;
                    relevantCount(refLabel)=relevantCount(refLabel)+1;
                  elseif(renameList(retLabel)~=refLabel && deadList(retLabel)==0)
                    %this shape must have crossed over two reference labels
                    deadList(retLabel)=1;
                    relevantCount(renameList(retLabel))=relevantCount(renameList(retLabel))-1;
                  end
                end
              end

	      InterR = size(find(relevantCount>0),2);


              for i=1:tsize
                if(deadList(outL(i))==0 && renameList(outL(i))~=0)
                  outL(i)=renameList(outL(i));
                end
              end

	      RetrievedR = size(unique(outL),1);

              assert(InterR <= RetrievedR);
              assert(InterR <= RelevantR);

end
