/*
 * Skeletal Tracking Algorithm
 * Developed based on the OpenNI Demo Program for Skeletal Detection
 *
 * Yuya Jeremy Ong
 */
#include <XnCppWrapper.h>

// TODO: Parameterize the configuration file path (if possible)
#define CONFIG_FILE_PATH "../../data/SamplesConfig.xml"
#define MAX_NUM_USERS 15

#define CHECK_RC(nRetVal, what)                                     \
if (nRetVal != XN_STATUS_OK) {								        \
    printf("%s failed: %s\n", what, xnGetStatusString(nRetVal));    \
    return nRetVal;						                            \
}

/* Global Parameters */
xn::Context g_Context;
xn::ScriptNode g_scriptNode;
xn::DepthGenerator g_DepthGenerator;
xn::UserGenerator g_UserGenerator;

XnBool g_bNeedPose = FALSE;
XnChar g_strPose[20] = "";

XnBool fileExists(const char *fn) {
	XnBool exists;
	xnOSDoesFileExist(fn, &exists);
	return exists;
}

// Callback: New User Detected
void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& /*generator*/, XnUserID nId, void* /*pCookie*/) {
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d New User Detected - User ID: %d\n", epochTime, nId);
    if (g_bNeedPose)
        g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
    else
        g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}

// Callback: Existing User Lost
void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& /*generator*/, XnUserID nId, void* /*pCookie*/) {
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d User Lost - User ID: %d\n", epochTime, nId);
}

// Callback: Pose Detected
void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& /*capability*/, const XnChar* strPose, XnUserID nId, void* /*pCookie*/) {
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d Pose %s detected for user %d\n", epochTime, strPose, nId);
    g_UserGenerator.GetPoseDetectionCap().StopPoseDetection(nId);
    g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
}

// Callback: Started calibration
void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& /*capability*/, XnUserID nId, void* /*pCookie*/) {
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    printf("%d Calibration started for user %d\n", epochTime, nId);
}

void XN_CALLBACK_TYPE UserCalibration_CalibrationComplete(xn::SkeletonCapability& /*capability*/, XnUserID nId, XnCalibrationStatus eStatus, void* /*pCookie*/) {
    XnUInt32 epochTime = 0;
    xnOSGetEpochTime(&epochTime);
    if (eStatus == XN_CALIBRATION_STATUS_OK) {
        // Calibration succeeded
        printf("%d Calibration complete, start tracking user %d\n", epochTime, nId);
        g_UserGenerator.GetSkeletonCap().StartTracking(nId);
    } else {
        // Calibration failed
        printf("%d Calibration failed for user %d\n", epochTime, nId);
        if(eStatus==XN_CALIBRATION_STATUS_MANUAL_ABORT) {
            printf("Manual abort occured, stop attempting to calibrate!");
            return;
        }
        if (g_bNeedPose)
            g_UserGenerator.GetPoseDetectionCap().StartPoseDetection(g_strPose, nId);
        else
            g_UserGenerator.GetSkeletonCap().RequestCalibration(nId, TRUE);
    }
}

int main(int argc, char* argv[]) {
    XnStatus nRetVal = XN_STATUS_OK;
    xn::EnumerationErrors errors;

    // Load Configuration Data File
    const char *fn = NULL;
    if (fileExists(CONFIG_FILE_PATH))
        fn = CONFIG_FILE_PATH;
    else {
        printf("Could not find the data file '%s'. Aborting.\n", CONFIG_FILE_PATH);
        return XN_STATUS_ERROR;
    }
    printf("Reading configuration data file from: '%s'\n", fn);

    // Read XML Configuration Data File
    nRetVal = g_Context.InitFromXmlFile(fn, g_scriptNode, &errors);
    if (nRetVal == XN_STATUS_NO_NODE_PRESENT) {
        XnChar strError[1024];
        errors.ToString(strError, 1024);
        printf("%s\n", strError);
        return (nRetVal);
    } else if (nRetVal != XN_STATUS_OK) {
        printf("Open failed: %s\n", xnGetStatusString(nRetVal));
        return (nRetVal);
    }

    // Find Depth Node
    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_DEPTH, g_DepthGenerator);
    CHECK_RC(nRetVal,"No depth");

    // Find User Node
    nRetVal = g_Context.FindExistingNode(XN_NODE_TYPE_USER, g_UserGenerator);
    if (nRetVal != XN_STATUS_OK) {
        nRetVal = g_UserGenerator.Create(g_Context);
        CHECK_RC(nRetVal, "Find user generator");
    }

    // Initialize Callback and Handle Calibration
    XnCallbackHandle hUserCallbacks, hCalibrationStart, hCalibrationComplete, hPoseDetected;
    if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_SKELETON)) {
        printf("Supplied user generator doesn't support skeleton\n");
        return 1;
    }
    nRetVal = g_UserGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, NULL, hUserCallbacks);
    CHECK_RC(nRetVal, "Register to user callbacks");
    nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, NULL, hCalibrationStart);
    CHECK_RC(nRetVal, "Register to calibration start");
    nRetVal = g_UserGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationComplete, NULL, hCalibrationComplete);
    CHECK_RC(nRetVal, "Register to calibration complete");

    // Pose Detection - Don't know what did does exactly, TODO: Read documentation on this...
    if (g_UserGenerator.GetSkeletonCap().NeedPoseForCalibration()) {
        g_bNeedPose = TRUE;
        if (!g_UserGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)) {
            printf("Pose required, but not supported\n");
            return 1;
        }
        nRetVal = g_UserGenerator.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, NULL, hPoseDetected);
        CHECK_RC(nRetVal, "Register to Pose Detected");
        g_UserGenerator.GetSkeletonCap().GetCalibrationPose(g_strPose);
    }

    // Initialize Skeleton Tracking Modules
    g_UserGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);

    nRetVal = g_Context.StartGeneratingAll();
    CHECK_RC(nRetVal, "StartGenerating");

    XnUserID aUsers[MAX_NUM_USERS];
    XnUInt16 nUsers;
    XnSkeletonJointTransformation torsoJoint;

    // Data Structure for Joint Data
    struct skeleton {
        XnSkeletonJointTransformation head;
        XnSkeletonJointTransformation neck;
        XnSkeletonJointTransformation torso;
        XnSkeletonJointTransformation waist;

        XnSkeletonJointTransformation left_collar;
        XnSkeletonJointTransformation left_shoulder;
        XnSkeletonJointTransformation left_elbow;
        XnSkeletonJointTransformation left_wrist;
        XnSkeletonJointTransformation left_hand;
        XnSkeletonJointTransformation left_fingertip;

        XnSkeletonJointTransformation right_collar;
        XnSkeletonJointTransformation right_shoulder;
        XnSkeletonJointTransformation right_elbow;
        XnSkeletonJointTransformation right_wrist;
        XnSkeletonJointTransformation right_hand;
        XnSkeletonJointTransformation right_fingertip;

        XnSkeletonJointTransformation left_hip;
        XnSkeletonJointTransformation left_knee;
        XnSkeletonJointTransformation left_ankle;
        XnSkeletonJointTransformation left_foot;

        XnSkeletonJointTransformation right_hip;
        XnSkeletonJointTransformation right_knee;
        XnSkeletonJointTransformation right_ankle;
        XnSkeletonJointTransformation right_foot;
    };

    printf("Tracking Initialized...\n");
    if(g_bNeedPose) printf("Assume calibration pose\n");

    // Initialize Tracking Loop
    while (!xnOSWasKeyboardHit()) {
        g_Context.WaitOneUpdateAll(g_UserGenerator);
        nUsers=MAX_NUM_USERS;
        g_UserGenerator.GetUsers(aUsers, nUsers);

        // Loop Over Each User Being Tracked
        for(XnUInt16 i=0; i<nUsers; i++) {
            if(g_UserGenerator.GetSkeletonCap().IsTracking(aUsers[i])==FALSE)
                continue;

            // Extract Skeletal Data into Skeleton Data Object
            skeleton skel;
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_HEAD, skel.head);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_NECK, skel.neck);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_TORSO, skel.torso);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_WAIST, skel.waist);

            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_COLLAR, skel.left_collar);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_SHOULDER, skel.left_shoulder);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_ELBOW, skel.left_elbow);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_WRIST, skel.left_wrist);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_HAND, skel.left_hand);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_FINGERTIP, skel.left_fingertip);

            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_COLLAR, skel.right_collar);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_SHOULDER, skel.right_shoulder);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_ELBOW, skel.right_elbow);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_WRIST, skel.right_wrist);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_HAND, skel.right_hand);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_FINGERTIP, skel.right_fingertip);

            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_HIP, skel.left_hip);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_KNEE, skel.left_knee);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_ANKLE, skel.left_ankle);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_LEFT_FOOT, skel.left_foot);

            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_HIP, skel.right_hip);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_KNEE, skel.right_knee);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_ANKLE, skel.right_ankle);
            g_UserGenerator.GetSkeletonCap().GetSkeletonJoint(aUsers[i], XN_SKEL_RIGHT_FOOT, skel.right_foot);

            // Display data... or do whatever we want with the coordinate data.
            /* Several Options to Consider:
             * - UDP/TCP Socket Stream to Server
             * - Direct hook to MATLAB
             * - Use the data structrue directly in a CPP library (form an API to access this much easily - abstract layers with support classes to calcualte joint trajectories).
             */
            // Data Format - Each joint is basically a (x, y, z coordinate).
            // printf("[UserID %d]: Head at (%6.2f,%6.2f,%6.2f)\n",aUsers[i], skel.torso.position.position.X, skel.torso.position.position.Y, skel.torso.position.position.Z);
            // printf("[UserID %d]: NECK at (%6.2f,%6.2f,%6.2f)\n",aUsers[i], skel.neck.position.position.X, skel.neck.position.position.Y, skel.neck.position.position.Z);

            // TODO: Consider other tracking methodologies or implement our own trackers with more DOF/Joints in the data.
            // TODO: Depending on how dataset looks, see if we can normalize the data somehow to use.

            // TODO: Research/Develop angle trajectory libraries which outputs vector/matrix of data points.
        }
    }
    g_scriptNode.Release();
    g_DepthGenerator.Release();
    g_UserGenerator.Release();
    g_Context.Release();
}
