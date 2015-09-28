# Clear ALl Workspace Parameters
clear -a

pkg load geometry

filepath = '../../Dataset/UCLIC_AffectiveBodyPostureMotion/Acted_2_(csm1)/DF1_6-13.csm';
skel_data = csmparser(filepath);

# Centroid Function (Accepts an array of coordinates)
# Simply finds the mean of each points, not sure how accruate.
function point = cntrd(points)
    points
    size(points)(1)
    x = 0; y = 0; z = 0;
    for i = 1 : rows(points)
        x += points(i, 1);
        y += points(i, 2);
        z += points(i, 3);
    end
    point = [(x/(size(points)(1))), (y/(size(points)(1))), (z/(size(points)(1)))]
endfunction;

# Static CSM Point Visualizer
# Function to plot the coordinates of one of the skeletal frame from the CSM Dataset File.
# Due to autoscaling, data may look a bit weird. Just zoom out to get proper proportions.
function csmplot_points(skel, meta)
    for i = 1:(size(meta.order)(2))
        eval(strcat("x(end+1) = skel.", meta.order(i), "(1);"));
        eval(strcat("y(end+1) = skel.", meta.order(i), "(2);"));
        eval(strcat("z(end+1) = skel.", meta.order(i), "(3);"));
    end;
    
    # TODO: Find method to scale the plots properly so that it won't be subjected to autoscaling.
    plot3(x,y,z,'.','MarkerSize',10);
    view(3);
endfunction;

# Static CSM Compressed Data Point Visualizer
function csmplot_ptcomp(skel)
    # Perform data compression based on kinect specifications.
    # TODO: Check with James to see whether reducing points can deteriment in accuracy of points.
    
    # HEAD-TOP LINE
    line([skel.RFHD(1); skel.LFHD(1)],[skel.RFHD(2); skel.LFHD(2)],[skel.RFHD(3); skel.LFHD(3)],'linewidth',2,'Color','green');
    hold on
    
    # HEAD - CENTROID(RFHD, RBHD, LFHD, LBHD)
    pt1 = cntrd([skel.RFHD; skel.LFHD]);
    plot3(pt1(1),pt1(2),pt1(3),'.','MarkerSize',10,'Color','red');
    hold on
    
    # MIDPT - CENTROID(RBHD, LBHD)
    pt2 = cntrd([skel.RBHD; skel.LBHD]);
    plot3(pt2(1),pt2(2),pt2(3),'.','MarkerSize',10,'Color','red');
    hold on
    
    pt1_2 = cntrd([pt1; pt2]);
    plot3(pt1_2(1), pt1_2(2), pt1_2(3), '.','MarkerSize',10,'Color','red');
    hold on
    
    # LINE(HEAD, NECK)
    line([pt1(1); pt2(1)], [pt1(2); pt2(2)], [pt1(3); pt2(3)],'linewidth',2,'Color','green');
    hold on
    
    # LINE(LSHOULD, RSHOULD)
    line([skel.RBHD(1); skel.LBHD(1)],[skel.RBHD(2); skel.LBHD(2)],[skel.RBHD(3); skel.LBHD(3)],'linewidth',2,'Color','green');
    hold on

    # LINE(LSHOULD, RSHOULD)
    line([skel.C7(1); skel.CLAV(1)],[skel.C7(2); skel.CLAV(2)],[skel.C7(3); skel.CLAV(3)],'linewidth',2,'Color','green');
    hold on    
    
    pt3 = centroid([skel.CLAV; skel.C7]);
    plot3(pt3(1), pt3(2), pt3(3), '.', 'MarkerSize',10,'Color','red');
    hold on
    
    # NECK TO HEAD MIDPTS
    line([pt3(1); pt1_2(1)],[pt3(2); pt1_2(2)],[pt3(3); pt1_2(3)],'linewidth',2,'Color','blue');
    hold on
 
    line([skel.LSHO(1); pt3(1)],[skel.LSHO(2); pt3(2)],[skel.LSHO(3); pt3(3)],'linewidth',2,'Color','green');
    hold on
    
    line([skel.RSHO(1); pt3(1)],[skel.RSHO(2); pt3(2)],[skel.RSHO(3); pt3(3)],'linewidth',2,'Color','green');
    hold on     
    
    line([skel.T10(1); skel.STRN(1)],[skel.T10(2); skel.STRN(2)],[skel.T10(3); skel.STRN(3)],'linewidth',2,'Color','green');
    hold on
    
    pt4 = centroid([skel.T10; skel.STRN]);
    plot3(pt4(1), pt4(2), pt4(3), '.', 'MarkerSize',10,'Color','red');
    hold on
    
    line([skel.RSHO(1); pt4(1)],[skel.RSHO(2); pt4(2)],[skel.RSHO(3); pt4(3)],'linewidth',2,'Color','green');
    hold on     
    
    line([skel.LSHO(1); pt4(1)],[skel.LSHO(2); pt4(2)],[skel.LSHO(3); pt4(3)],'linewidth',2,'Color','green');
    hold on
    
    # LEFT WAIST MIDPIT
    pt5 = centroid([skel.LFWT; skel.LBWT]);
    plot3(pt5(1), pt5(2), pt5(3), '.', 'MarkerSize',10,'Color','red');
    hold on
    
    # RIGHT WAIST MIDPT
    pt6 = centroid([skel.RFWT; skel.RBWT]);
    plot3(pt6(1), pt6(2), pt6(3), '.', 'MarkerSize',10,'Color','red');
    hold on
    
    # LOWER BOTTOM VECTOR
    line([pt5(1); pt6(1)],[pt5(2); pt6(2)],[pt5(3); pt6(3)],'linewidth',2,'Color','green');
    hold on
    
    line([pt5(1); pt4(1)],[pt5(2); pt4(2)],[pt5(3); pt4(3)],'linewidth',2,'Color','green');
    hold on
    
    line([pt4(1); pt6(1)],[pt4(2); pt6(2)],[pt4(3); pt6(3)],'linewidth',2,'Color','green');
    hold on
    ####
    
    # BACK VECTOR
    line([skel.T10(1); skel.C7(1)],[skel.T10(2); skel.C7(2)],[skel.T10(3); skel.C7(3)],'linewidth',2,'Color','green');
    hold on
    
    # CHEST VECTOR
    line([skel.STRN(1); skel.CLAV(1)],[skel.STRN(2); skel.CLAV(2)],[skel.STRN(3); skel.CLAV(3)],'linewidth',2,'Color','green');
    hold on
    
    # HEAD-NECK MIDPT
    pt7 = cntrd([pt1; pt3]);
    plot3(pt7(1),pt7(2),pt7(3),'.','MarkerSize',10,'Color','magenta');
    hold on
    
    #WAIST MIDPT
    pt8 = cntrd([pt5; pt6]);
    plot3(pt8(1),pt8(2),pt8(3),'.','MarkerSize',10,'Color','magenta');
    hold on
    
    
    # BUTTOM VECTOR FRONT-BACK
    line([skel.STRN(1); pt8(1)],[skel.STRN(2); pt8(2)],[skel.STRN(3); pt8(3)],'linewidth',2,'Color','green');
    hold on
    
    line([skel.T10(1); pt8(1)],[skel.T10(2); pt8(2)],[skel.T10(3); pt8(3)],'linewidth',2,'Color','green');
    hold on
    
    # LR MIDPT WAIST VECTOR
    # line([pt5(1); pt6(1)],[pt5(2); pt6(2)],[pt5(3); pt6(3)],'linewidth',2,'Color','green');
    # hold on
    
    # SPINE 
    #pt = skel.C7;
    #xs(end+1) = pt(1); ys(end+1) = pt(2) = zs(end+1) = pt(3);
    #pt = skel.CLAV;
    #xs(end+1) = pt(1); ys(end+1) = pt(2) = zs(end+1) = pt(3);
    
    #plot3(xs,ys,zs,'.','MarkerSize',10,'Color','red');
    
    # LEGS - FRONT WAIST, THIGH, KNEE
    
    #RIGHT LEGS
    line([skel.RFWT(1); skel.RTHI(1)],[skel.RFWT(2); skel.RTHI(2)],[skel.RFWT(3); skel.RTHI(3)],'linewidth',2,'Color','green');
    hold on
    line([skel.T10(1); pt8(1)],[skel.T10(2); pt8(2)],[skel.T10(3); pt8(3)],'linewidth',2,'Color','green');
    hold on
    line([skel.T10(1); pt8(1)],[skel.T10(2); pt8(2)],[skel.T10(3); pt8(3)],'linewidth',2,'Color','green');
    hold on
    
    #RIGHT FRONT WAIST
    plot3(skel.RFWT(1), skel.RFWT(2), skel.RFWT(3),'.','MarkerSize',10,'Color','red');
    hold on    
    
    #RIGHT THIGH 
    plot3(skel.RTHI(1), skel.RTHI(2), skel.RTHI(3),'.','MarkerSize',10,'Color','red');
    hold on    
    
    # RIGHT KNEE
    plot3(skel.RKNE(1), skel.RKNE(2), skel.RKNE(3),'.','MarkerSize',10,'Color','red');
    hold on
    
    view(3);
endfunction;

frame = cell2mat(skel_data.frames(429));
csmplot_points(frame, skel_data.meta);
hold on
csmplot_ptcomp(frame);

#csmplot_ptcomp(frame);

# Plot Test 
#plot3(1,1,1,'.','MarkerSize',10);
#view(3);

# Reduce CSM Coordinates to Kinect Coordinates
# head = cntrd([frame.RFHD; frame.RBHD; frame.LFHD; frame.LBHD]);
