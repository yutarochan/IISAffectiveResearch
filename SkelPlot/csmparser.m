# CSM 3D Model File Parser
# Author: Yuya Jeremy Ong
# Parses A .csm 3D model data file and returns a data container with frame and meta data.

function dat = csmparser (filepath)
    data = fopen(filepath, 'r');

    if (data < 0)
        disp('Error: Please provide a valid csm datafile.');
        exit(1);
    else
        # Extract CSM Datafile Header Metadata - Assumes the structure of the csm file format is consistent and structued as follows:
        meta.filename = substr(fgetl(data), 10);
        meta.date = substr(fgetl(data), 6);
        meta.time = substr(fgetl(data), 6);
        meta.actor = substr(fgetl(data), 7);
        fskipl(data); fskipl(data);
        meta.comments = fgetl(data);
        fskipl(data);
        meta.firstFrame = substr(fgetl(data), 12);
        meta.lastFrame = substr(fgetl(data), 11);
        meta.rate = substr(fgetl(data), 6);
        fskipl(data); fskipl(data);
        meta.order = strsplit(strtrim(fgetl(data)));
        fskipl(data); fskipl(data); fskipl(data);

        # Parse out Temporal Skeletal Data
        frames = {};
        while (!feof(data))
            # Attempt to work with the dynamic ordering of the joint types listed in meta.order
            skeleton = [];
            rowdat = str2num(fgetl(data));

            # TODO: Better optimize the row vector segmentation algorithm (potential bottle neck!)
            idx = 1;
            n = size(rowdat)(2);
            for i = 2 : 3 : n
                eval(strcat("skeleton.", meta.order(idx), " = ", mat2str(rowdat(i : i + 2)), ";"));
                idx++;
            end;
            frames(end+1) = skeleton;
        end;

        dat.frames = frames;
        dat.meta = meta;
    end;
endfunction;
