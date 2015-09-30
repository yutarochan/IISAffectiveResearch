function [FileError, PosData] = ReadCSM(filename) 
%FileError is the boolean variable for the result of reading file. 
% -1 for succeed, 1 for failure.
NumMarkers = -1; 
FileError = -1; 
fp = fopen(filename,'r'); 
if (fp == -1) 
    disp('ERROR: no such file'); 
    FileError = 1;
    return;
end

%Read the header information, especially number of markers and frames
NumMarkers = 0;
NumFrames = 0;
flag = 0;
while (1) 
    HeaderString = fscanf(fp,'%s',1);
    if(strcmp(HeaderString,'$Points')==1)
        break;
    end
    if(strcmp(HeaderString,'$FirstFrame')==1)
        TempString = fscanf(fp,'%s',1);
        FirstFrame = sscanf(TempString,'%g');
    end
    if(strcmp(HeaderString,'$LastFrame')==1)
        TempString = fscanf(fp,'%s',1);
        LastFrame = sscanf(TempString,'%g');
    end
    if(strcmp(HeaderString,'$Order')==1)
        flag = 1;
        continue;
    end
    if(flag == 1 && length(HeaderString) ~= 0)
        NumMarkers = NumMarkers + 1;
    end
end 
NumFrames = LastFrame - FirstFrame + 1;

%Read Markers Details
%may index specific joints here..
%End

% Read the XYZ position data
% Attention!! The first one is index, NOT Position data
Total = NumMarkers*3+1; 
PosData = zeros(Total,NumFrames); 
count = 1; 

while(1)
    tline = fgetl(fp);
    if ~ischar(tline)
        break;
    end % Read whole line into variable tline
    Value = sscanf(tline,'%g'); 
    VecLength = length(Value); 
    if(VecLength==0) 
        continue;
    elseif (VecLength~=Total)
         PosData(1:VecLength,count)=Value(1:VecLength);
         PosData(VecLength+1:Total,count)=0.0;
         count=count+1;
    else
         PosData(:,count)=Value(:);
         count=count+1;
    end
    
end
any(PosData,1)==0
PosData(:,any(PosData,1)==0) = [];
% delete columns that are all zeros