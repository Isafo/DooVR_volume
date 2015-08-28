//#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_EXPOSE_NATIVE_WGL

#include "Oculus.h"

#include "Shader.h"
#include "MatrixStack.h"

#include "Sphere.h"
#include "MenuItem.h"
#include "Box.h"
#include "menuBox.h"
#include "hexBox.h"
#include "Texture.h"

#include "Wand.h"
#include "Passive3D.h"
#include "TrackingRange.h"

#include "DynamicMesh.h"
#include "StaticMesh.h"

#include "Circle.h"
#include "Square.h"
#include "LineCube.h"
#include "LineSphere.h"
#include "Line.h"

#include "Smooth.h"
#include "Push.h"
#include "Draw.h"
#include "Drag.h"
#include "BuildUp.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <math.h>

// ------- Function declerations --------
//! Sets up a glfw window depending on the resolution of the Oculus Rift device
static void WindowSizeCallback(GLFWwindow *p_Window, int p_Width, int p_Height);
//! checks if the wand is colliding with a menuItem and sets the menuItems state accordingly, returns true if a menuItem choise has occured
int handleMenu(float* wandPosition, menuBox** menuItem, const int nrOfModellingUIButtons, int* state);
void GLRenderCallsOculus();
//! reads all filenames in savedFiles folder
std::vector<std::string> getSavedFileNames();

//! loads a uneditable mesh
void loadStaticMesh(StaticMesh* item, std::string fileName);
//! loads a mesh that can be edited
void loadMesh(DynamicMesh* item, std::string fileName);
//! saves the main mesh to a file
void saveFile(DynamicMesh* item);
// --------------------------------------
// --- Variable Declerations ------------
const bool L_MULTISAMPLING = false;
const int G_DISTORTIONCAPS = 0
| ovrDistortionCap_Vignette
| ovrDistortionCap_Chromatic
| ovrDistortionCap_Overdrive
| ovrDistortionCap_TimeWarp // Turning this on gives ghosting???
;

ovrHmd hmd;
ovrGLConfig g_Cfg;
ovrEyeRenderDesc g_EyeRenderDesc[2];

std::mutex loaderMeshLock, meshLock;

/*! tells the state of the thread
	0 = not running
	1 = running
	2 = not joined after running */
std::atomic <int> th1Status(0);
std::atomic <int> th2Status(0);


int Oculus::runOvr() {

	//=========================================================================================================================================
	// 1 - Initialize OVR, GLFW and openGL variables
	//=========================================================================================================================================

	// 1.1 - oculus variables \________________________________________________________________________________________________________________
	ovrVector3f g_EyeOffsets[2];
	ovrPosef g_EyePoses[2];
	ovrTexture g_EyeTextures[2];
	OVR::Matrix4f g_ProjectionMatrix[2];
	OVR::Sizei g_RenderTargetSize;
	//ovrVector3f g_CameraPosition;

	int WIN_SCALE[2];

	ovr_Initialize();
	int det;
	// Check for attached head mounted display...
	hmd = ovrHmd_Create(0);
	if (!hmd) {
		printf("No Oculus Rift device attached, using virtual version...\n");
		hmd = ovrHmd_CreateDebug(ovrHmd_DK2);

		det = ovrHmd_Detect();
		std::cout << det << std::endl;
	}

	// Check to see if we are running in "Direct" or "Extended Desktop" mode...
	bool l_DirectMode = ((hmd->HmdCaps & ovrHmdCap_ExtendDesktop) == 0);

	// INITIALIZE GL ////////////////////////////////////////////////////
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	if (L_MULTISAMPLING) glfwWindowHint(GLFW_SAMPLES, 4);
	else glfwWindowHint(GLFW_SAMPLES, 0);


	// SETUP GLFW WINDOW AND CONTEXT /////////////////////////////////////////////////////////////
	// Create a window...
	GLFWwindow* l_Window;
	GLFWmonitor* l_Monitor;
	ovrSizei l_ClientSize;

	if (l_DirectMode) {
		printf("Running in \"Direct\" mode...\n");
		l_Monitor = NULL;

		l_ClientSize.w = hmd->Resolution.w; // Something reasonable, smaller, but maintain aspect ratio...
		l_ClientSize.h = hmd->Resolution.h; // Something reasonable, smaller, but maintain aspect ratio...
	}
	else {// Extended Desktop mode...
		printf("Running in \"Extended Desktop\" mode...\n");
		int l_Count;
		GLFWmonitor** l_Monitors = glfwGetMonitors(&l_Count);
		switch (l_Count) {
		case 0: {
			printf("No monitors found, exiting...\n");
			exit(EXIT_FAILURE);
			break;
		} case 1: {
			printf("Two monitors expected, found only one, using primary...\n");
			l_Monitor = glfwGetPrimaryMonitor();
			break;
		} case 2: {
			printf("Two monitors found, using second monitor...\n");
			l_Monitor = l_Monitors[1];
			break;
		} default: {
			printf("More than two monitors found, using second monitor...\n");
			l_Monitor = l_Monitors[1];
		}
		}

		l_ClientSize.w = hmd->Resolution.w; // 1920 for DK2...
		l_ClientSize.h = hmd->Resolution.h; // 1080 for DK2...

		WIN_SCALE[0] = hmd->Resolution.w;
		WIN_SCALE[1] = hmd->Resolution.h;
	}

	l_Window = glfwCreateWindow(l_ClientSize.w, l_ClientSize.h, "GLFW Oculus Rift Test", l_Monitor, NULL);

	// Check if window creation was succesfull...
	if (!l_Window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// Attach the window in "Direct Mode"...
#if defined(_WIN32)
	if (l_DirectMode) {
		ovrBool l_AttachResult = ovrHmd_AttachToWindow(hmd, glfwGetWin32Window(l_Window), NULL, NULL);
		if (!l_AttachResult) {
			printf("Could not attach to window...");
			exit(EXIT_FAILURE);
		}
	}
#endif

	// Make the context current for this window...
	glfwMakeContextCurrent(l_Window);

	//start GLEW extension handler
	glewExperimental = GL_TRUE;
	GLenum l_GlewResult = glewInit();
	if (l_GlewResult != GLEW_OK) {
		printf("glewInit() error.\n");
		exit(EXIT_FAILURE);
	}

	// Print some info about the OpenGL context...
	int l_Major = glfwGetWindowAttrib(l_Window, GLFW_CONTEXT_VERSION_MAJOR);
	int l_Minor = glfwGetWindowAttrib(l_Window, GLFW_CONTEXT_VERSION_MINOR);
	int l_Profile = glfwGetWindowAttrib(l_Window, GLFW_OPENGL_PROFILE);
	printf("OpenGL: %d.%d ", l_Major, l_Minor);
	if (l_Major >= 3) { // Profiles introduced in OpenGL 3.0...
		if (l_Profile == GLFW_OPENGL_COMPAT_PROFILE) printf("GLFW_OPENGL_COMPAT_PROFILE\n"); else printf("GLFW_OPENGL_CORE_PROFILE\n");
	}
	printf("Vendor: %s\n", (char*)glGetString(GL_VENDOR));
	printf("Renderer: %s\n", (char*)glGetString(GL_RENDERER));

	//CREATE OCULUS TEXTURES AND BIND THESE TO GL///////////////////////////////////////////////////////////////////////////////
	ovrSizei l_EyeTextureSizes[2];

	l_EyeTextureSizes[ovrEye_Left] = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left, hmd->MaxEyeFov[ovrEye_Left], 1.0f);
	l_EyeTextureSizes[ovrEye_Right] = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, hmd->MaxEyeFov[ovrEye_Right], 1.0f);

	// Combine for one texture for both eyes...
	g_RenderTargetSize.w = l_EyeTextureSizes[ovrEye_Left].w + l_EyeTextureSizes[ovrEye_Right].w;
	g_RenderTargetSize.h = (l_EyeTextureSizes[ovrEye_Left].h>l_EyeTextureSizes[ovrEye_Right].h ? l_EyeTextureSizes[ovrEye_Left].h : l_EyeTextureSizes[ovrEye_Right].h);

	// Create the FBO being a single one for both eyes (this is open for debate)...
	GLuint l_FBOId;
	glGenFramebuffers(1, &l_FBOId);
	glBindFramebuffer(GL_FRAMEBUFFER, l_FBOId);

	// The texture we're going to render to...
	GLuint l_TextureId;
	glGenTextures(1, &l_TextureId);
	// "Bind" the newly created texture : all future texture functions will modify this texture...
	glBindTexture(GL_TEXTURE_2D, l_TextureId);
	// Give an empty image to OpenGL (the last "0")
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, g_RenderTargetSize.w, g_RenderTargetSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	// Linear filtering...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Create Depth Buffer...
	GLuint l_DepthBufferId;
	glGenRenderbuffers(1, &l_DepthBufferId);
	glBindRenderbuffer(GL_RENDERBUFFER, l_DepthBufferId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, g_RenderTargetSize.w, g_RenderTargetSize.h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, l_DepthBufferId);

	// Set the texture as our colour attachment #0...
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, l_TextureId, 0);

	// Set the list of draw buffers...
	GLenum l_GLDrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, l_GLDrawBuffers); // "1" is the size of DrawBuffers

	// Check if everything is OK...
	GLenum l_Check = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (l_Check != GL_FRAMEBUFFER_COMPLETE) {
		printf("There is a problem with the FBO.\n");
		exit(EXIT_FAILURE);
	}

	// Unbind...
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// SETUP TEXTURES FOR EACH EYE /////////////////////////////////////////////////////////////////////
	// Left eye...
	g_EyeTextures[ovrEye_Left].Header.API = ovrRenderAPI_OpenGL;
	g_EyeTextures[ovrEye_Left].Header.TextureSize = g_RenderTargetSize;
	g_EyeTextures[ovrEye_Left].Header.RenderViewport.Pos.x = 0;
	g_EyeTextures[ovrEye_Left].Header.RenderViewport.Pos.y = 0;
	g_EyeTextures[ovrEye_Left].Header.RenderViewport.Size = l_EyeTextureSizes[ovrEye_Left];
	((ovrGLTexture&)(g_EyeTextures[ovrEye_Left])).OGL.TexId = l_TextureId;

	// Right eye (mostly the same as left but with the viewport on the right side of the texture)
	g_EyeTextures[ovrEye_Right] = g_EyeTextures[ovrEye_Left];
	g_EyeTextures[ovrEye_Right].Header.RenderViewport.Pos.x = (g_RenderTargetSize.w + 1) / 2;
	g_EyeTextures[ovrEye_Right].Header.RenderViewport.Pos.y = 0;

	// OCULUS RIFT EYE CONFIGURATIONS
	g_Cfg.OGL.Header.API = ovrRenderAPI_OpenGL;
	g_Cfg.OGL.Header.BackBufferSize.w = l_ClientSize.w;
	g_Cfg.OGL.Header.BackBufferSize.h = l_ClientSize.h;
	g_Cfg.OGL.Header.Multisample = (L_MULTISAMPLING ? 1 : 0);

	g_Cfg.OGL.Window = glfwGetWin32Window(l_Window);
	g_Cfg.OGL.DC = GetDC(g_Cfg.OGL.Window);

	// Enable capabilities...
	// ovrHmd_SetEnabledCaps(hmd, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);

	ovrBool l_ConfigureResult = ovrHmd_ConfigureRendering(hmd, &g_Cfg.Config, G_DISTORTIONCAPS, hmd->MaxEyeFov, g_EyeRenderDesc);
	if (!l_ConfigureResult) {
		printf("Configure failed.\n");
		exit(EXIT_FAILURE);
	}

	// Start the sensor which provides the Rift�s pose and motion...
	uint32_t l_SupportedSensorCaps = ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position;
	uint32_t l_RequiredTrackingCaps = 0;
	ovrBool l_TrackingResult = ovrHmd_ConfigureTracking(hmd, l_SupportedSensorCaps, l_RequiredTrackingCaps);
	if (!l_TrackingResult) {
		printf("Could not start tracking...");
		exit(EXIT_FAILURE);
	}

	// set projection matrix for each eye
	g_ProjectionMatrix[ovrEye_Left] = ovrMatrix4f_Projection(g_EyeRenderDesc[ovrEye_Left].Fov, 0.3f, 100.0f, true);
	g_ProjectionMatrix[ovrEye_Right] = ovrMatrix4f_Projection(g_EyeRenderDesc[ovrEye_Right].Fov, 0.3f, 100.0f, true);

	// set IPD offset values
	g_EyeOffsets[ovrEye_Left] = g_EyeRenderDesc[ovrEye_Left].HmdToEyeViewOffset;
	g_EyeOffsets[ovrEye_Right] = g_EyeRenderDesc[ovrEye_Right].HmdToEyeViewOffset;

	glfwSetWindowSizeCallback(l_Window, WindowSizeCallback);

	//=====================================================================================================================================
	// 2 - Variable Declarations
	//=====================================================================================================================================

	const float M_PI = 3.14159265359;

	// 2.2 - Various vectors and matrices \________________________________________________________________________________________________
	// Lightposition 
	float lPos[4] = { 2.0f, 2.0f, 2.0f, 1.0f };
	float lPos2[4] = { -2.0f, 2.0f, 2.0f, 1.0f };
	float lPosTemp[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float LP[4];
	float* pmat4;
	float mat4[16];

	// Save old positions and transforms
	float changePos[3] = { 0.0f };
	float differenceR[16] = { 0.0f };
	float currPos[3] = { 0.0f, 0.0f, 0.0f };
	float nullVec[3] = { 0.0f, 0.0f, 0.0f };
	float translateVector[3] = { 0.0f, 0.0f, 0.0f };
	float moveVec[4]; float nMoveVec[4];
	float tempVec[3]; float tempVec2[3];
	float lastPos[3];
	float lastPos2[3];
	float wandDirection[3];
	float prevWandDirection[4]; prevWandDirection[3] = 1.0f;
	float prevMeshOrientation[16];
	float meshOrientation[16];
	float vVec[4];
	float vMat[16] = { 0.0f };
	float transform[16];
	float toolRad = 0.01;
	float toolStr = 0.01;

	float unitMat[16] = { 0.0f }; unitMat[0] = 1; unitMat[5] = 1; unitMat[10] = 1; unitMat[15] = 1;

	float wandPos[3];
	float wandVelocity[3] = {0};

	float currTime = 0;
	float lastTime;
	float deltaTime;


	float startWandPos[3];

	float* tempVecPtr;
	float temp1, temp2;

	// 2.3 - Threads used for loading and saving meshes \__________________________________________________________________________________
	//! Thread 1 used to load static meshes
	std::thread th1;
	//! Thread 2 used to load and save dynamic meshes
	std::thread th2;

	bool reset = false;

	// indicates if something has been loaded by a thread and gl data has not yet been updated
	bool loadingMesh = false;

	// 2.4 - textures \____________________________________________________________________________________________________________________
	glEnable(GL_TEXTURE_2D);
	// Scene textures

	Texture whiteTex("../Assets/Textures/light.DDS");
	Texture groundTex("../Assets/Textures/Gbord.DDS");
	Texture titleTex("../Assets/Textures/Titel2by1.DDS");
	Texture resetTex("../Assets/Textures/reset.DDS");
	Texture saveTex("../Assets/Textures/save.DDS");
	Texture loadTex("../Assets/Textures/load.DDS");
	Texture wireFrameTex("../Assets/Textures/wireframe.DDS");
	Texture plusTex("../Assets/Textures/plus.DDS");
	Texture minusTex("../Assets/Textures/minus.DDS");

	Texture menuIcons("../Assets/Textures/menuICONS.DDS");
	Texture menuInfo("../Assets/Textures/menyInfo.DDS");
	Texture menuStrings("../Assets/Textures/menuSTRINGS.DDS");
	Texture strengthTex("../Assets/Textures/Size.DDS");

	Texture menuStringsSwe("../Assets/Textures/menuStringsSwe.dds");
	Texture savedFeedbackTex("../Assets/Textures/sparad4by1.dds");
	Texture loadModeInfoTex("../Assets/Textures/loadMode2by1.dds");

	float boardPos[3] = { 0.0f, 0.09f, 0.0f };
	Box board(boardPos[0], boardPos[1], boardPos[2], 1.4, 0.02, 0.70); TrackingRange trackingRange(boardPos[0], (boardPos[1] + (0.25f / 2.0f) + 0.01f) , boardPos[2], 0.50, 0.25, 0.40);

	// 2.5 - Modes \______________________________________________________________________________________________________________________
	/*! Mode 0 = modelling mode
		Mode 1 = loading mode
		Mode 2 = saving mode
		Mode 3 = instructions mode*/
	int mode = 0;
	//! contains the number of any active states
	int activeButton = -1;
	// 2.5.1 - modelling states and variables used in modelling mode >--------------------------------------------------------------------
	
	const int NR_OF_MODELLING_STATES = 3;
	/*! 0 indicates that the state is not active,
		1 indicates that the state has just been activated
		2 indicates that the state is active
		3 indicates that the state has just been deactivated

		modellingState[0] is the use tool state
		modellingState[1] is the moveMesh state
		modellingState[2] is the changeWandSize state
	*/
	int modellingState[NR_OF_MODELLING_STATES] = { 0 };
	int aModellingStateIsActive = 0;

	const int NR_OF_MODELLING_BUTTONS = 4;
	/*!	[0] resetMesh
		[1] save
		[2] load
		[3] Wireframe
		[4] increase wand size */
	menuBox* modellingButton[NR_OF_MODELLING_BUTTONS];
	MenuItem* modellingButtonString[NR_OF_MODELLING_BUTTONS - 1];
	MenuItem* modellingButtonFrame;

	Texture* modellingButtonTex[NR_OF_MODELLING_BUTTONS];
	modellingButtonTex[0] = &menuIcons;
	modellingButtonTex[1] = &menuIcons;
	modellingButtonTex[2] = &menuIcons;
	modellingButtonTex[3] = &menuIcons;

	/*!	0 indicates that the state is not active,
		1 indicates that the state has just been activated
		2 indicates that the state is active
		3 indicates that the state has just been deactivated
		4 indicates that the state is active (on/off)
		5 indicates that the state has been activated and then the button was released

		[0] reset
		[1] save
		[2] load
		[3] Wireframe
		[4] change wand size*/
	static int modellingButtonState[NR_OF_MODELLING_BUTTONS];

	modellingButton[0] = new menuBox(boardPos[0] - 0.2f, boardPos[1] + 0.011f + 0.0125f, boardPos[2] + 0.06, 0.025f, 0.025f, 0.025f, 0, 1, 1, 1, 5, 5);
	modellingButton[1] = new menuBox(boardPos[0] - 0.2f, boardPos[1] + 0.011f + 0.0125f, boardPos[2] + -0.04, 0.025f, 0.025f, 0.025f, 3, 0, 1, 1, 5, 5);
	modellingButton[2] = new menuBox(boardPos[0] - 0.2f, boardPos[1] + 0.011f + 0.0125f, boardPos[2] + -0.08, 0.025f, 0.025f, 0.025f, 4, 0, 1, 1, 5, 5);
	modellingButton[3] = new menuBox(boardPos[0] - 0.2f, boardPos[1] + 0.011f + 0.0125f, boardPos[2] + -0.12, 0.025f, 0.025f, 0.025f, 5, 0, 1, 1, 5, 5);

	// place modellingButton Frame
	modellingButtonFrame = new MenuItem(0.0f, -0.01245f, 0.0f, 0.04f, 0.04f, 0, 3, 1, 1);
	// place modellingButton Strings
	modellingButtonString[0] = new MenuItem(-0.08f, -0.0125f, 0.0f, 0.12f, 0.04f, 1, 3, 4, 1);
	modellingButtonString[1] = new MenuItem(-0.105f, -0.0125f, 0.0f, 0.24f, 0.04f, 0, 1, 7, 1);
	modellingButtonString[2] = new MenuItem(-0.11f, -0.0125f, 0.0f, 0.24f, 0.04f, 0, 0, 8, 1);
	//variable used with the button that switches wireframe, TODO: should be replaced by something
	bool lines = false;

	// MenuItems representing a tool
	const int NR_OF_TOOLS = 3;
	menuBox tool[NR_OF_TOOLS];
	MenuItem toolFrame(boardPos[0] + 0.2, boardPos[1] + 0.011f, boardPos[2] - 0.04, 0.04f, 0.12f, 0, 4, 1, 3);
	MenuItem toolString(boardPos[0] + 0.2f + 0.08f, boardPos[1] + 0.011f, boardPos[2] - 0.04, 0.12f, 0.04f, 1, 4, 3, 1);

	for (int i = 0; i < NR_OF_TOOLS; i++)
		tool[i] = menuBox(boardPos[0] + 0.2, boardPos[1] + 0.011f + 0.0125f, boardPos[2] - 0.04*(i), 0.025f, 0.025f, 0.025f, i, 0, 1, 1, 5, 5);

	menuBox toolSize(boardPos[0] + 0.2, boardPos[1] + 0.011f + 0.075f, boardPos[2] - 0.16, 0.02f, 0.15f, 0.02f, 3, 3, 1, 1, 5, 5);
	menuBox toolSizeFill(boardPos[0] + 0.2, boardPos[1] + 0.09 - 0.075, boardPos[2] - 0.16, 0.015f, 0.0f, 0.015f, 5, 2, 1, 1, 5, 2); toolSizeFill.setDim(0.0f, toolRad * 3, 0.0f);
	MenuItem sizeString(0.14f, -0.075f, 0.0f, 0.24f, 0.04f, 0, 2, 8, 1);


	menuBox toolStrength(boardPos[0] + 0.16f, boardPos[1] + 0.011f + 0.075f, boardPos[2] - 0.16, 0.02f, 0.15f, 0.02f, 3, 3, 1, 1, 5, 5);
	menuBox toolStrengthFill(boardPos[0] + 0.16f, boardPos[1] + 0.011f + 0.075f, boardPos[2] - 0.16, 0.015f, 0.0f, 0.015f, 5, 2, 1, 1, 5, 2); toolStrengthFill.setDim(0.0f, toolStr, 0.0f);
	MenuItem strengthString(0.095f, -0.075f, 0.035f, 0.24f, 0.04f, 1, 5, 9, 1);

	MenuItem trackingInfo(boardPos[0], boardPos[1] + 0.0125f, boardPos[2] - 0.22f, 0.24f, 0.08f, 1, 6, 9, 2);

	/*! tool 0 = push/pull
			 1 = 
	*/
	int activeTool = 2;
	tool[activeTool].setState(true);
	// 2.5.2 - variables used in Load Mode >------------------------------------------------------------------------------------------------
	const int NR_OF_LOAD_BUTTONS = 2;
	/*! 0 = loadFile button
		1 = exitLoad button*/
	menuBox* loadButton[NR_OF_LOAD_BUTTONS];
	/*! 0 indicates that the state is not active,
		1 indicates that the state has just been activated
		2 indicates that the state is active
		3 indicates that the state has just been deactivated
		4 indicates that the state is active (on/off)
		5 indicates that the state has been activated and then the button was released 
	
		[0] = loadFile
		[1] = exitLoad  */
	int loadButtonState[NR_OF_LOAD_BUTTONS] = { 0 };

	Texture* loadButtonTex[NR_OF_LOAD_BUTTONS];
	loadButtonTex[0] = &loadTex;
	loadButtonTex[1] = &minusTex;

	float* tempMoveVec;
	bool changedMesh = false;
	bool failedToStartLoading = false;
	bool maxVelocityNotLoaded = false;

	time_t finishedTime;

	MenuItem loadModeInfo(boardPos[0], boardPos[1] + 0.15f, boardPos[2] - 0.2f, 0.4f, 0.2f);

	// 2.5.2 - variables used in save Mode >------------------------------------------------------------------------------------------------
	MenuItem savedFeedback(boardPos[0] - 0.2f, boardPos[1] + 0.011f + 0.0125f + 0.04f, boardPos[2] + -0.04, 0.08f, 0.04f);

	// 2.6 - Shader variables \_____________________________________________________________________________________________________________
	Shader sceneShader;
	sceneShader.createShader("sceneV.glsl", "sceneF.glsl");
	Shader meshShader;
	meshShader.createShader("meshV.glsl", "meshF.glsl");
	Shader bloomShader;
	bloomShader.createShader("bloomV.glsl", "bloomF.glsl");
	Shader menuShader;
	menuShader.createShader("menuV.glsl", "menuF.glsl");
	Shader flatShader;
	flatShader.createShader("meshFlatV.glsl", "meshFlatF.glsl");


	// 2.6.1 - Uniform variables >-----------------------------------------------------------------------------------------------------------
	GLint locationLP = glGetUniformLocation(sceneShader.programID, "lightPos");
	GLint locationP = glGetUniformLocation(sceneShader.programID, "P"); //perspective matrix
	GLint locationMV = glGetUniformLocation(sceneShader.programID, "MV"); //modelview matrix
//	GLint* MVptr = &locationMV;
	GLint locationTex = glGetUniformLocation(sceneShader.programID, "tex"); //texcoords

	GLint locationMeshMV = glGetUniformLocation(meshShader.programID, "MV"); //modelview matrix
	GLint locationMeshP = glGetUniformLocation(meshShader.programID, "P"); //perspective matrix
	GLint locationMeshLP = glGetUniformLocation(meshShader.programID, "lightPos");
	GLint locationMeshLP2 = glGetUniformLocation(meshShader.programID, "lightPos2");

	GLint locationFlatMV = glGetUniformLocation(flatShader.programID, "MV"); //modelview matrix
	GLint locationFlatP = glGetUniformLocation(flatShader.programID, "P"); //perspective matrix
	GLint locationFlatLP = glGetUniformLocation(flatShader.programID, "lightPos");
	GLint locationFlatLP2 = glGetUniformLocation(flatShader.programID, "lightPos2");

	// 2.7 - Scene objects and variables \___________________________________________________________________________________________________

	// 2.7.1 - Matrix stack and static scene objects >---------------------------------------------------------------------------------------
	MatrixStack MVstack; MVstack.init();
	MatrixStack* MVptr = &MVstack;

	MenuItem title(0.0f, 0.8f, -1.0f, 0.6f, 0.3f);
	//MenuItem menuInfoPanel(boardPos[0] + 0.8f, boardPos[1] + 0.02, boardPos[2] , 0.3f, 0.4f);

	// 2.7.2 - Wand variables >--------------------------------------------------------------------------------------------------------------
	Box boxWand(0.0f, 0.0f, 0.0f, 0.007f, 0.007f, 0.2f);
	// Initilise passive wand
	Passive3D* wand = new Passive3D();
	// Size of the wand tool
	float wandRadius = 0.01f;
	float lastRadius;

	// 2.7.3 - Mesh variables >--------------------------------------------------------------------------------------------------------------
	//DynamicMesh* modellingMesh = new DynamicMesh("2015-07-22_16-08-10.bin");
	DynamicMesh* modellingMesh = new DynamicMesh();
	tempVec[0] = boardPos[0]; tempVec[1] = boardPos[0] + 0.15f; tempVec[2] = boardPos[2];
	modellingMesh->setPosition(tempVec);
	//modellingMesh->load("2015-07-22_16-08-10.bin"); modellingMesh->createBuffers();
	modellingMesh->sphereSubdivide(0.05f); modellingMesh->createBuffers();
	std::string currentMesh = "../Assets/Models/resetMesh.bin";

	// variables for browsing saved meshes
	//Mesh* staticMesh;
	//Mesh* tempStaticMesh = new Mesh();
	std::vector<std::string> meshFile;
	//! fileIndex is the index in fileName of the staticMesh that is shown
	int fileIndex = 0;

	StaticMesh* placeHolder;
	StaticMesh* previewMesh;
	StaticMesh* loaderMesh;

	Tool* currentTool;
	//currentTool = new Push(modellingMesh, wand);
	currentTool = new BuildUp(modellingMesh, wand);

	//=======================================================================================================================================
	//Render loop
	//=======================================================================================================================================

	//ovrHmd_RecenterPose(hmd);
	ovrHmd_DismissHSWDisplay(hmd); // dismiss health safety warning
	unsigned int l_FrameIndex = 0;

	while (!glfwWindowShouldClose(l_Window)) {

		lastTime = currTime;
		currTime = glfwGetTime();
		deltaTime = currTime - lastTime;

		wand->getPosition(wandPos);

		switch (mode) {
			//===============================================================================================================================
			// 3 - Modelling Mode
			//===============================================================================================================================
			case 0: {

				// 3.1 - modellingstates \_____________________________________________________________________________________________________
				//3.1.1 - use modellingtool >--------------------------------------------------------------------------------------------------
			
			if (glfwGetKey(l_Window, GLFW_KEY_PAGE_UP)) {
				if (modellingState[0] == 2) {
					currentTool->moveVertices(modellingMesh, wand, deltaTime);
					
				}
				else if (modellingState[0] == 1) {
					modellingState[0] = 2;
					currentTool->deSelect();
					currentTool->firstSelect(modellingMesh, wand);
				}
				else if (modellingState[0] == 0)
				{
					modellingState[0] = 1;

						aModellingStateIsActive++;
					}
				}
				else {
					if (modellingState[0] == 3) {
						modellingState[0] = 0;
					}
					else if (modellingState[0] != 0) {
						modellingState[0] = 3;
						currentTool->deSelect();
						aModellingStateIsActive--;
					}
					currentTool->deSelect();
					currentTool->firstSelect(modellingMesh, wand);
				}
				
				//3.1.2 - move mesh >-----------------------------------------------------------------------------------------------------------
				
				if (glfwGetKey(l_Window, GLFW_KEY_PAGE_DOWN)) {
					if (modellingState[1] == 0) {
						modellingState[1] = 1;
						wand->getPosition(lastPos);
						modellingMesh->getPosition(lastPos2);

						wand->getDirection(prevWandDirection);
						modellingMesh->getOrientation(prevMeshOrientation);
						linAlg::normVec(prevWandDirection);

						aModellingStateIsActive++;
					}
					else if (modellingState[1] == 1) {
						modellingState[1] = 2;
					}
					else if (modellingState[1] == 2) {
						//	move mesh
						linAlg::calculateVec(wandPos, lastPos, moveVec);
						moveVec[0] = lastPos2[0] + moveVec[0] - wandPos[0];
						moveVec[1] = lastPos2[1] + moveVec[1] - wandPos[1];
						moveVec[2] = lastPos2[2] + moveVec[2] - wandPos[2];

						wand->getDirection(wandDirection);
						linAlg::normVec(wandDirection);

						linAlg::crossProd(vVec, wandDirection, prevWandDirection);
						linAlg::normVec(vVec);
						linAlg::rotAxis(vVec, acos(linAlg::dotProd(prevWandDirection, wandDirection)), transform);

						linAlg::matrixMult(transform, prevMeshOrientation, meshOrientation);

						linAlg::vectorMatrixMult(transform, moveVec, nMoveVec);
						nMoveVec[0] += wandPos[0];
						nMoveVec[1] += wandPos[1];
						nMoveVec[2] += wandPos[2];

						modellingMesh->setPosition(nMoveVec);
						modellingMesh->setOrientation(meshOrientation);
					}
				}
				else {
					if (modellingState[1] == 3) {
						modellingState[1] = 0;
					}
					else if (modellingState[1] != 0) {
						modellingState[1] = 3;

						aModellingStateIsActive--;
					}
				}

				//3.1.2 - temporary keyboardevents >----------------------------------------------------------------------------------------------
				if (glfwGetKey(l_Window, GLFW_KEY_ESCAPE)) {
					glfwSetWindowShouldClose(l_Window, GL_TRUE);
				}
				if (glfwGetKey(l_Window, GLFW_KEY_LEFT)) {
					currentTool->setStrength(0.01f*deltaTime);
				}
				if (glfwGetKey(l_Window, GLFW_KEY_RIGHT)) {
					currentTool->setStrength(-0.01f*deltaTime);
				}

				// 3.2 - handelmenu and menuswitch \______________________________________________________________________________________________
				if (aModellingStateIsActive == 0) {
					activeButton = handleMenu(wandPos, modellingButton, NR_OF_MODELLING_BUTTONS, modellingButtonState);
					switch (activeButton) {
						//3.2.1 - new mesh button>----------------------------------------------------------------------------------------------
						case 0: {
							if (modellingButtonState[activeButton] == 1) {
								// reset mesh
								if (th2Status == 0) {
									th2Status = 1;
									th2 = std::thread(loadMesh, modellingMesh, currentMesh);
								}
								
								modellingButton[activeButton]->setState(true);

								reset = true;

							}
							else if (modellingButtonState[activeButton] == 3) {
								modellingButton[activeButton]->setState(false);
							}
							break;
						}
						//3.2.2 - save mesh button >--------------------------------------------------------------------------------------------
						case 1: {
							// save mesh
							if (modellingButtonState[activeButton] == 1) {
								modellingButton[activeButton]->setState(true);
								if (th2Status == 0) {
									th2Status = 1;
									th2 = std::thread(saveFile, modellingMesh);
									mode = 2; // enter save mode
								}
							}
							else if (modellingButtonState[activeButton] == 3) {
								modellingButton[activeButton]->setState(false);
							}

							break;
						}
						//3.2.3 - load mesh button >--------------------------------------------------------------------------------------------
						case 2: {
							// load mesh
							if (modellingButtonState[activeButton] == 1) { // just activated

								// get all filenames in the savedFiles folder
								meshFile = getSavedFileNames();

								// check if any files were found
								if (meshFile.empty()) {
									// if no files were found leave loading state
									// TODO: display feedback in Oculus that there are no saved files found
									std::cout << "no saved files found!" << std::endl;
								}
								else {
									// saved files found
									// set staticMesh as placeHolder during loading as a loading indicator
									placeHolder = new StaticMesh(); placeHolder->load("../Assets/Models/placeHolder.bin"); placeHolder->createBuffers();
									loaderMesh = new StaticMesh();

									previewMesh = placeHolder;

									tempVec[0] = boardPos[0];
									tempVec[1] = boardPos[1] + 0.1f;
									tempVec[2] = boardPos[2];
									previewMesh->setPosition(tempVec);

									loadButton[1] = new menuBox(boardPos[0] - 0.03f, boardPos[1] + 0.03f, boardPos[2] + 0.05f, 0.06f, 0.01f, 0.02f, 0, 1, 3, 1, 5, 5);
									loadButton[0] = new menuBox(boardPos[0] + 0.03f, boardPos[1] + 0.03f, boardPos[2] + 0.05f, 0.04f, 0.01f, 0.02f, 0, 3, 2, 1, 5, 5);

									// Call thread to load the mesh preview
									if (th1Status == 0) {
										th1Status = 1;
										th1 = std::thread(loadStaticMesh, loaderMesh, meshFile[fileIndex % meshFile.size()]);
										mode = 1; // activate load mode
									}
								}
							}
							break;
						}
						//3.2.4 - wireframe button >---------------------------------------------------------------------------------------------
						case 3: {
							// wireframe
							if (modellingButtonState[activeButton] == 1) {
								if (modellingButton[activeButton]->getState() == true){
									modellingButton[activeButton]->setState(false);
									lines = false;
								}
								else {
									modellingButton[activeButton]->setState(true);
									lines = true;
								}
							}
							break;
						}
						//3.2.4 - wand size button >---------------------------------------------------------------------------------------------
						case 4: {
							// change wand size
							if (modellingButtonState[activeButton] == 1) {
								modellingButton[activeButton]->setState(true);

								lastRadius = wandRadius;

								wand->getPosition(lastPos);
							}

							if (modellingButtonState[activeButton] == 3)
								modellingButton[activeButton]->setState(false);

							break;
						}
					}
					for (int i = 0; i < NR_OF_TOOLS; i++) {
						if (wandPos[0] < tool[i].getPosition()[0] + tool[i].getDim()[0] / 2.f
							&& wandPos[0] > tool[i].getPosition()[0] - tool[i].getDim()[0] / 2.f
							&& wandPos[1] > tool[i].getPosition()[1] - tool[i].getDim()[1] / 2.f
							&& wandPos[1] < tool[i].getPosition()[1] + tool[i].getDim()[1] / 2.f
							&& wandPos[2] > tool[i].getPosition()[2] - tool[i].getDim()[2] / 2.f
							&& wandPos[2] < tool[i].getPosition()[2] + tool[i].getDim()[2] / 2.f) {
							if (!tool[i].getState()) {
								tool[activeTool].setState(false);
								delete currentTool;
								tool[i].setState(true);
								activeTool = i;
								if (i == 0)
									currentTool = new Drag(modellingMesh, wand);
								else if (i == 1)
									currentTool = new Smooth(modellingMesh, wand);
								else if (i == 2)
									currentTool = new BuildUp(modellingMesh, wand);
								
								currentTool->setRadius(toolRad);
								currentTool->setStrength(toolStr);
							}

							break;
						}
					}
					if (wandPos[0] < toolSize.getPosition()[0] + toolSize.getDim()[0] / 2.f
						&& wandPos[0] > toolSize.getPosition()[0] - toolSize.getDim()[0] / 2.f
						&& wandPos[1] > toolSize.getPosition()[1] - toolSize.getDim()[1] / 2.f
						&& wandPos[1] < toolSize.getPosition()[1] + toolSize.getDim()[1] / 2.f
						&& wandPos[2] > toolSize.getPosition()[2] - toolSize.getDim()[2] / 2.f
						&& wandPos[2] < toolSize.getPosition()[2] + toolSize.getDim()[2] / 2.f) {
						tempVecPtr = toolSizeFill.getPosition();
						toolSizeFill.setDim(0.0f, (wandPos[1] - tempVecPtr[1]), 0.0f);
						currentTool->setRadius((wandPos[1] - tempVecPtr[1]) / 3.0f); toolRad = (wandPos[1] - tempVecPtr[1]) / 3.0f;
					}
					else if (wandPos[0] < toolStrength.getPosition()[0] + toolStrength.getDim()[0] / 2.f
						&& wandPos[0] > toolStrength.getPosition()[0] - toolStrength.getDim()[0] / 2.f
						&& wandPos[1] > toolStrength.getPosition()[1] - toolStrength.getDim()[1] / 2.f
						&& wandPos[1] < toolStrength.getPosition()[1] + toolStrength.getDim()[1] / 2.f
						&& wandPos[2] > toolStrength.getPosition()[2] - toolStrength.getDim()[2] / 2.f
						&& wandPos[2] < toolStrength.getPosition()[2] + toolStrength.getDim()[2] / 2.f) {
						tempVecPtr = toolStrengthFill.getPosition();
						toolStrengthFill.setDim(0.0f, (wandPos[1] - tempVecPtr[1]), 0.0f);
						currentTool->setStrength((wandPos[1] - tempVecPtr[1])); toolStr = (wandPos[1] - tempVecPtr[1]) ;
					}
				}

				if (reset) {
					if (th2.joinable()) {
						th2.join();
						th2Status = 0;
						modellingMesh->cleanBuffer();
						modellingMesh->updateOGLData();
						tempVec[0] = boardPos[0]; tempVec[1] = boardPos[0] + 0.15f; tempVec[2] = boardPos[2];
						modellingMesh->setPosition(tempVec);
						reset = false;
					}
				}	

				
				// Begin the frame...
				ovrHmd_BeginFrame(hmd, l_FrameIndex);
				// Get eye poses for both the left and the right eye. g_EyePoses contains all Rift information: orientation, positional tracking and
				// the IPD in the form of the input variable g_EyeOffsets.
				ovrHmd_GetEyePoses(hmd, l_FrameIndex, g_EyeOffsets, g_EyePoses, NULL);
				// Bind the FBO...
				glBindFramebuffer(GL_FRAMEBUFFER, l_FBOId);
				//glModes
				GLRenderCallsOculus();

				for (int l_EyeIndex = 0; l_EyeIndex<ovrEye_Count; l_EyeIndex++) {

					// 3.3 OCULUS/CAMERA TRANSFORMS \______________________________________________________________________________________________
					MVstack.push();
						ovrEyeType l_Eye = hmd->EyeRenderOrder[l_EyeIndex];

						glViewport(g_EyeTextures[l_Eye].Header.RenderViewport.Pos.x,
							g_EyeTextures[l_Eye].Header.RenderViewport.Pos.y,
							g_EyeTextures[l_Eye].Header.RenderViewport.Size.w,
							g_EyeTextures[l_Eye].Header.RenderViewport.Size.h);

						glUseProgram(sceneShader.programID);
						// Pass projection matrix on to OpenGL...
						glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
						glUniform1i(locationTex, 0);

						// Multiply with orientation retrieved from sensor...
						OVR::Quatf l_Orientation = OVR::Quatf(g_EyePoses[l_Eye].Orientation);
						OVR::Matrix4f l_ModelViewMatrix = OVR::Matrix4f(l_Orientation.Inverted());
						MVstack.multiply(&(l_ModelViewMatrix.Transposed().M[0][0]));
						//MVstack.multiply(wand->getOrientation());

						//!-- Translation due to positional tracking (DK2) and IPD...
						float eyePoses[3] = { -g_EyePoses[l_Eye].Position.x, -g_EyePoses[l_Eye].Position.y, -g_EyePoses[l_Eye].Position.z };
						MVstack.translate(eyePoses);
						//wand->getPosition(tempVec);
						//tempVec[0] = -tempVec[0]; tempVec[1] = -tempVec[1]; tempVec[2] = -tempVec[2];
						//MVstack.translate(tempVec);

						//POSSABLY DOABLE IN SHADER
						pmat4 = MVstack.getCurrentMatrix();
						for (int i = 0; i < 16; i++)
							mat4[i] = pmat4[i];

						//linAlg::transpose(mat4);
						linAlg::vectorMatrixMult(mat4, lPos, LP);
						linAlg::vectorMatrixMult(mat4, lPos2, lPosTemp);
						glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix()); //TODO: check if this uniform is needed

						// 3.4 - Scene Matrix stack \__________________________________________________________________________________________________
						MVstack.push();
							// 3.4.1 RENDER BOARD >----------------------------------------------------------------------------------------------------
							glUniform4fv(locationLP, 1, LP);
							MVstack.push();
								MVstack.translate(board.getPosition());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								glBindTexture(GL_TEXTURE_2D, groundTex.getTextureID());
								board.render();
							MVstack.pop();

							glBindTexture(GL_TEXTURE_2D, whiteTex.getTextureID());

							// 3.4.2 Render tracking range >-------------------------------------------------------------------------------------------
							MVstack.push();
								MVstack.translate(trackingRange.getPosition());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
								glLineWidth(2.0f);
								trackingRange.render();
								glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
							MVstack.pop();

							// 3.4.3 Render title >----------------------------------------------------------------------------------------------------

							glUseProgram(bloomShader.programID);
							glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));

							MVstack.push();
								MVstack.translate(title.getPosition());
								MVstack.rotX(1.57079f);
								glBindTexture(GL_TEXTURE_2D, titleTex.getTextureID());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								title.render();
							MVstack.pop();
							/*
							//glUseProgram(menuShader.programID);
							//glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
							MVstack.push();
								MVstack.translate(menuInfoPanel.getPosition());
								//MVstack.rotX(1.57079f);
								//MVstack.rotZ(1.57079f);
								glBindTexture(GL_TEXTURE_2D, menuInfo.getTextureID());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								menuInfoPanel.render();
							MVstack.pop();*/


							// 3.4.4 Render modelling buttons >-----------------------------------------------------------------------------------------
							for (int i = 0; i < NR_OF_MODELLING_BUTTONS; i++) {

								if (modellingButton[i]->getState()) {
									glUseProgram(bloomShader.programID);
									glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
								}
								else {
									glUseProgram(menuShader.programID);
									glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
								}

								MVstack.push();
									glBindTexture(GL_TEXTURE_2D, modellingButtonTex[i]->getTextureID());

									MVstack.translate(modellingButton[i]->getPosition());
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									modellingButton[i]->render();
								MVstack.pop();
							}

							// place modellingButton frames
							glUseProgram(menuShader.programID);
							glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
							glBindTexture(GL_TEXTURE_2D, menuStringsSwe.getTextureID());

							for (int i = 0; i < NR_OF_MODELLING_BUTTONS; i++) {
								MVstack.push();
									MVstack.translate(modellingButton[i]->getPosition());
									MVstack.translate(modellingButtonFrame->getPosition());
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									modellingButtonFrame->render();
								MVstack.pop();
							}

							for (int i = 0; i < NR_OF_MODELLING_BUTTONS - 1; i++) {
								MVstack.push();
									MVstack.translate(modellingButton[i]->getPosition());
									MVstack.translate(modellingButtonString[i]->getPosition());
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									modellingButtonString[i]->render();
								MVstack.pop();
							}
							
							MVstack.push();
								MVstack.translate(toolFrame.getPosition());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								toolFrame.render();
							MVstack.pop();
							MVstack.push();
								MVstack.translate(toolString.getPosition());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								toolString.render();
							MVstack.pop();

							MVstack.push();
								MVstack.translate(trackingInfo.getPosition());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								trackingInfo.render();
							MVstack.pop();



							// 3.4.5 Render mesh >------------------------------------------------------------------------------------------------------
							

							if (lines) {
								glUseProgram(flatShader.programID);
								glUniformMatrix4fv(locationFlatP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));

								MVstack.push();
									MVstack.translate(modellingMesh->getPosition());
									MVstack.multiply(modellingMesh->getOrientation());
									glUniformMatrix4fv(locationFlatMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									glUniform4fv(locationFlatLP, 1, LP);
									glUniform4fv(locationFlatLP2, 1, lPosTemp);

									modellingMesh->render();

							} else {
								glUseProgram(meshShader.programID);
								glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));

								MVstack.push();
									MVstack.translate(modellingMesh->getPosition());
									MVstack.multiply(modellingMesh->getOrientation());
									glUniformMatrix4fv(locationMeshMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									glUniform4fv(locationMeshLP, 1, LP);
									glUniform4fv(locationMeshLP2, 1, lPosTemp);

									modellingMesh->render();

								}
								glUseProgram(sceneShader.programID);
								glBindTexture(GL_TEXTURE_2D, whiteTex.getTextureID());
							//	currentTool->renderIntersection(MVptr, locationMeshMV);

							MVstack.pop();

							
							glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
							// 3.4.6 Render wand >-------------------------------------------------------------------------------------------
							MVstack.push();
								MVstack.translate(wandPos);
								MVstack.multiply(wand->getOrientation());

								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								MVstack.push();
									translateVector[0] = 0.0f;
									translateVector[1] = 0.0f;
									translateVector[2] = -0.1f;
									MVstack.translate(translateVector);
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									
									boxWand.render();		
								MVstack.pop();
								//render brush------------------------

								/*MVstack.push();
									translateVector[0] = 0.0f;
									translateVector[1] = 0.0f;
									translateVector[2] = 1.0f;
									MVstack.translate(translateVector);
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									brushPointer.render();
								MVstack.pop();

								MVstack.push();
									MVstack.scale(wandRadius);
									MVstack.translate(brush.getPosition());
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									brush.render();
								MVstack.pop();*/
								currentTool->render(MVptr, locationMV);
							MVstack.pop();

							// 3.4.3 Render title >----------------------------------------------------------------------------------------------------
							glUseProgram(bloomShader.programID);
							glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
	
								// render tool select GUI
								for (int i = 0; i < NR_OF_TOOLS; i++) {

									if (tool[i].getState()) {
										glUseProgram(bloomShader.programID);
										
										glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
									}
									else {
										glUseProgram(menuShader.programID);
										glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
									}

									glBindTexture(GL_TEXTURE_2D, menuIcons.getTextureID());
									MVstack.push();
										MVstack.translate(tool[i].getPosition());
										glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
										tool[i].render();
									MVstack.pop();
								}
								glBindTexture(GL_TEXTURE_2D, menuStrings.getTextureID());
								glUseProgram(menuShader.programID);
								MVstack.push();
									MVstack.translate(toolSizeFill.getPosition());
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									toolSizeFill.render();
								MVstack.pop();

								MVstack.push();
									MVstack.translate(toolStrengthFill.getPosition());
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									toolStrengthFill.render();
								MVstack.pop();

								glBindTexture(GL_TEXTURE_2D, menuStringsSwe.getTextureID());
								MVstack.push();
									MVstack.translate(toolSize.getPosition());
									MVstack.push();
										MVstack.translate(sizeString.getPosition());
										glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
										sizeString.render();
									MVstack.pop();

									MVstack.push();
										MVstack.translate(strengthString.getPosition());
										glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
										strengthString.render();
									MVstack.pop();

								MVstack.pop();
								

																
								glBindTexture(GL_TEXTURE_2D, menuIcons.getTextureID());
								glUseProgram(bloomShader.programID);
								
								MVstack.push();
									MVstack.translate(toolSize.getPosition());
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									toolSize.render();
								MVstack.pop();

								MVstack.push();
									MVstack.translate(toolStrength.getPosition());
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									toolStrength.render();
								MVstack.pop();
								
						//	}

						MVstack.pop();
					MVstack.pop();

				}
				break;
			}
			case 1: {
				//===============================================================================================================================
				// 4 - LOAD Mode
				//===============================================================================================================================if (th2Status = 0) {
				
			
				
				// 4.1 - Keyboard events \_______________________________________________________________________________________________________
				// 4.1.1 - Move mesh >-----------------------------------------------------------------------------------------------------------
				if (glfwGetKey(l_Window, GLFW_KEY_PAGE_DOWN)) {
					if (modellingState[1] == 0) {

						wandVelocity[0] = 0; wandVelocity[1] = 0; wandVelocity[2] = 0;

						modellingState[1] = 1;
						wand->getPosition(lastPos);
						previewMesh->getPosition(lastPos2);

						wand->getDirection(prevWandDirection);
						previewMesh->getOrientation(prevMeshOrientation);
						linAlg::normVec(prevWandDirection);

						aModellingStateIsActive++;
					}
					else if (modellingState[1] == 1) {
						modellingState[1] = 2;
					}
					else if (modellingState[1] == 2) {
						//	move mesh
						linAlg::calculateVec(wandPos, lastPos, moveVec);
						moveVec[0] = lastPos2[0] + moveVec[0] - wandPos[0];
						moveVec[1] = lastPos2[1] + moveVec[1] - wandPos[1];
						moveVec[2] = lastPos2[2] + moveVec[2] - wandPos[2];

						wand->getDirection(wandDirection);	
						linAlg::normVec(wandDirection);
						
						linAlg::crossProd(vVec, wandDirection, prevWandDirection);
						linAlg::normVec(vVec);
						linAlg::rotAxis(vVec, acos(linAlg::dotProd(prevWandDirection, wandDirection)), transform);

						linAlg::matrixMult(transform, prevMeshOrientation, meshOrientation);

						linAlg::vectorMatrixMult(transform, moveVec, nMoveVec);
						nMoveVec[0] += wandPos[0];
						nMoveVec[1] += wandPos[1];
						nMoveVec[2] += wandPos[2];

						previewMesh->setPosition(nMoveVec);
						previewMesh->setOrientation(meshOrientation);
					}
				}
				else {
					if (modellingState[1] == 3) {
						modellingState[1] = 0;
					}
					else if (modellingState[1] != 0) {
						// just released button
					
						wand->getVelocity(wandVelocity);
						if (linAlg::vecLength(wandVelocity) < 0.4) {
							//changedMesh = true;
							wandVelocity[0] = 0; wandVelocity[1] = 0; wandVelocity[2] = 0;
						}
						modellingState[1] = 3;
						aModellingStateIsActive--;
					}
				}

				if (glfwGetKey(l_Window, GLFW_KEY_ESCAPE)) {
					glfwSetWindowShouldClose(l_Window, GL_TRUE);
				}

				// 4.1.2 - Load next mesh \______________________________________________________________________________________________________
				// move mesh until it leaves the tracking range then load a new mesh
				tempMoveVec = previewMesh->getPosition();
				if (linAlg::vecLength(wandVelocity) != 0) {
				
					tempVec[0] = tempMoveVec[0] + wandVelocity[0] * deltaTime;
					tempVec[1] = tempMoveVec[1] + wandVelocity[1] * deltaTime;
					tempVec[2] = +tempMoveVec[2] + wandVelocity[2] * deltaTime;
					previewMesh->setPosition(tempVec);
				}
				//trackingRange(boardPos[0], (boardPos[1] + (0.25f / 2.0f) + 0.01f), boardPos[2], 0.50, 0.25, 0.40);
				// check if the preview mesh has left the tracking range area and increment fileIndex accordingly
				if (tempMoveVec[0] > 0.25) {

					previewMesh = placeHolder;

					tempVec[0] = -0.25f + (tempMoveVec[0] - 0.25);
					tempVec[1] = placeHolder->getPosition()[1];
					tempVec[2] = placeHolder->getPosition()[2];
					previewMesh->setPosition(tempVec);
					fileIndex--;

					wandVelocity[1] = 0.0f; wandVelocity[2] = 0.0f;
					if (wandVelocity[0] < 0.1f) {
						if (th1Status == 0) {
							th1Status = 1;
							th1 = std::thread(loadStaticMesh, loaderMesh, meshFile[fileIndex % meshFile.size()]);
						} else {
							failedToStartLoading = true;
						}
					} else {
						maxVelocityNotLoaded = true;
					}

					// reset initial starting pos
					wand->getPosition(lastPos);
					previewMesh->getPosition(lastPos2);
					wand->getDirection(prevWandDirection);
					previewMesh->getOrientation(prevMeshOrientation);
					linAlg::normVec(prevWandDirection);
				}

				else if (tempMoveVec[0] < -0.25) {
					previewMesh = placeHolder;

					tempVec[0] = 0.25f + (tempMoveVec[0] + 0.25);
					tempVec[1] = placeHolder->getPosition()[1];
					tempVec[2] = placeHolder->getPosition()[2];
					previewMesh->setPosition(tempVec);

					fileIndex++;

					wandVelocity[1] = 0.0f; wandVelocity[2] = 0.0f;
					if (abs(wandVelocity[0]) < 1.5f) {
						if (th1Status == 0) {
							th1Status = 1;
							th1 = std::thread(loadStaticMesh, loaderMesh, meshFile[fileIndex % meshFile.size()]);
						} else {
							failedToStartLoading = true;
						}
					} else {
						maxVelocityNotLoaded = true;
					}

					// reset initial starting pos
					wand->getPosition(lastPos);
					previewMesh->getPosition(lastPos2);
					wand->getDirection(prevWandDirection);
					previewMesh->getOrientation(prevMeshOrientation);
					linAlg::normVec(prevWandDirection);
				}

				// check if the thread is ready with a new mesh
				if (th1.joinable()) {
					th1.join();
					th1Status = 0;

					// check if the loaded mesh was changed before it was loaded by the thread
					if (failedToStartLoading) {
						th1Status = 1;
						th1 = std::thread(loadStaticMesh, loaderMesh, meshFile[fileIndex % meshFile.size()]);
					} else {
						loaderMesh->setPosition(placeHolder->getPosition());
						previewMesh = loaderMesh;
						previewMesh->createBuffers();
						th1Status = 0;
					}	
				}

				if (abs(wandVelocity[0]) < 1.5f && maxVelocityNotLoaded) {
					if (th1Status == 0) {
						th1Status = 1;
						th1 = std::thread(loadStaticMesh, loaderMesh, meshFile[fileIndex % meshFile.size()]);
						maxVelocityNotLoaded = false;
					}
				}

				// 4.2 - Handle buttons and button switch \______________________________________________________________________________________
				if (aModellingStateIsActive == 0) {
					activeButton = handleMenu(wandPos, loadButton, NR_OF_LOAD_BUTTONS, loadButtonState);

					switch (activeButton) {
						case 0: {
							// load mesh button
							if (loadButtonState[activeButton] == 1) {
								if (th2Status == 0){
									th2Status = 1;
									th2 = std::thread(loadMesh, modellingMesh, meshFile[fileIndex % meshFile.size()]);
									currentMesh = meshFile[fileIndex % meshFile.size()];
									wandVelocity[0] = 0; wandVelocity[1] = 0; wandVelocity[2] = 0;
								}
							}
							break;
						}
						case 1: {
							// quit mode to modelling mode

							if (th1Status != 0) {
								th1.join();
								th1Status = 0;
							}
							if (th2Status != 0) {
								th2.join();
								th2Status = 0;
							}

							wandVelocity[0] = 0; wandVelocity[1] = 0; wandVelocity[2] = 0;
							delete loadButton[0];
							delete loadButton[1];
							delete loaderMesh;
							delete placeHolder;
							mode = 0;

							continue;
						}
						default: {
							break;
						}
					}
				}
				
				if (th2.joinable()) {
					th2.join();
					th2Status = 0;

					modellingMesh->cleanBuffer();
					delete loadButton[0];
					delete loadButton[1];
					delete loaderMesh;
					delete placeHolder;
					mode = 0;
					tempVec[0] = boardPos[0]; tempVec[1] = boardPos[0] + 0.15f; tempVec[2] = boardPos[2];
					modellingMesh->setPosition(tempVec);

					if (th1Status != 0) {
						th1.join();
						th1Status = 0;
					}
					continue;
				}

				

				// Begin the frame...
				ovrHmd_BeginFrame(hmd, l_FrameIndex);
				// Get eye poses for both the left and the right eye. g_EyePoses contains all Rift information: orientation, positional tracking and
				// the IPD in the form of the input variable g_EyeOffsets.
				ovrHmd_GetEyePoses(hmd, l_FrameIndex, g_EyeOffsets, g_EyePoses, NULL);
				// Bind the FBO...
				glBindFramebuffer(GL_FRAMEBUFFER, l_FBOId);
				//glModes
				GLRenderCallsOculus();

				for (int l_EyeIndex = 0; l_EyeIndex<ovrEye_Count; l_EyeIndex++) {

					// 4.3 OCULUS/CAMERA TRANSFORMS \______________________________________________________________________________________________
					MVstack.push();
						ovrEyeType l_Eye = hmd->EyeRenderOrder[l_EyeIndex];

						glViewport(g_EyeTextures[l_Eye].Header.RenderViewport.Pos.x,
							g_EyeTextures[l_Eye].Header.RenderViewport.Pos.y,
							g_EyeTextures[l_Eye].Header.RenderViewport.Size.w,
							g_EyeTextures[l_Eye].Header.RenderViewport.Size.h);

						glUseProgram(sceneShader.programID);
						// Pass projection matrix on to OpenGL...
						glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
						glUniform1i(locationTex, 0);

						// Multiply with orientation retrieved from sensor...
						OVR::Quatf l_Orientation = OVR::Quatf(g_EyePoses[l_Eye].Orientation);
						OVR::Matrix4f l_ModelViewMatrix = OVR::Matrix4f(l_Orientation.Inverted());
						MVstack.multiply(&(l_ModelViewMatrix.Transposed().M[0][0]));

						//!-- Translation due to positional tracking (DK2) and IPD...
						float eyePoses[3] = { -g_EyePoses[l_Eye].Position.x, -g_EyePoses[l_Eye].Position.y, -g_EyePoses[l_Eye].Position.z };
						MVstack.translate(eyePoses);

						//POSSABLY DOABLE IN SHADER
						pmat4 = MVstack.getCurrentMatrix();
						for (int i = 0; i < 16; i++)
							mat4[i] = pmat4[i];

						//linAlg::transpose(mat4);
						linAlg::vectorMatrixMult(mat4, lPos, LP);
						linAlg::vectorMatrixMult(mat4, lPos2, lPosTemp);
						glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());

						// 4.4 - Scene Matrix stack \__________________________________________________________________________________________________
						MVstack.push();
							// 4.4.1 - RENDER BOARD >--------------------------------------------------------------------------------------------------
							glUniform4fv(locationLP, 1, LP);
							MVstack.push();
								MVstack.translate(board.getPosition());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								glBindTexture(GL_TEXTURE_2D, groundTex.getTextureID());
								board.render();
							MVstack.pop();

							glBindTexture(GL_TEXTURE_2D, whiteTex.getTextureID());

							// 4.4.2 - RENDER trackingrange >-----------------------------------------------------------------------------------------
							MVstack.push();
								MVstack.translate(trackingRange.getPosition());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
								glLineWidth(2.0f);
								trackingRange.render();
								glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
							MVstack.pop();

							// 4.4.3 - RENDER title >---------------------------------------------------------------------------------------------------
							glUseProgram(menuShader.programID);
							glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));

							MVstack.push();
								MVstack.translate(title.getPosition());
								MVstack.rotX(1.57079f);
								glBindTexture(GL_TEXTURE_2D, titleTex.getTextureID());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								title.render();
							MVstack.pop();


							// 4.4.4 - RENDER Load buttons >--------------------------------------------------------------------------------------------
							for (int i = 0; i < NR_OF_LOAD_BUTTONS; i++) {

								if (loadButton[i]->getState()) {
									glUseProgram(bloomShader.programID);
									glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
								}
								else {
									glUseProgram(menuShader.programID);
									glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
								}

								MVstack.push();
									glBindTexture(GL_TEXTURE_2D, menuStrings.getTextureID());

									MVstack.translate(loadButton[i]->getPosition());
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									loadButton[i]->render();
								MVstack.pop();
							}

							// render menuinfo during limited time
							glUseProgram(menuShader.programID);
							glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
							MVstack.push();
								glBindTexture(GL_TEXTURE_2D, loadModeInfoTex.getTextureID());
								MVstack.translate(loadModeInfo.getPosition());
								MVstack.rotX(1.57079f);
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								loadModeInfo.render();
							MVstack.pop();

							// 4.4.5 - RENDER meshes >--------------------------------------------------------------------------------------------------
							glUseProgram(meshShader.programID);
							glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));

							MVstack.push();
								MVstack.translate(previewMesh->getPosition());
								MVstack.multiply(previewMesh->getOrientation());
								glUniformMatrix4fv(locationMeshMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								glUniform4fv(locationMeshLP, 1, LP);
								glUniform4fv(locationMeshLP2, 1, lPosTemp);
								previewMesh->render();
							MVstack.pop();

							MVstack.push();
								translateVector[0] = previewMesh->getPosition()[0] + 0.5f;
								translateVector[1] = boardPos[1] + 0.1;
								translateVector[2] = boardPos[2];
								MVstack.translate(translateVector);
								MVstack.multiply(placeHolder->getOrientation());
								glUniformMatrix4fv(locationMeshMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								glUniform4fv(locationMeshLP, 1, LP);
								glUniform4fv(locationMeshLP2, 1, lPosTemp);
								placeHolder->render();
							MVstack.pop();

							MVstack.push();
								translateVector[0] = previewMesh->getPosition()[0] - 0.5f;
								translateVector[1] = boardPos[1] + 0.1;
								translateVector[2] = boardPos[2];
								MVstack.translate(translateVector);
								MVstack.multiply(placeHolder->getOrientation());
								glUniformMatrix4fv(locationMeshMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								glUniform4fv(locationMeshLP, 1, LP);
								glUniform4fv(locationMeshLP2, 1, lPosTemp);
								placeHolder->render();
							MVstack.pop();

							glUseProgram(sceneShader.programID);
							glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));

							// 4.4.6 - RENDER wand >--------------------------------------------------------------------------------------------------
							MVstack.push();
								MVstack.translate(wandPos);
								MVstack.multiply(wand->getOrientation());

								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								MVstack.push();
									translateVector[0] = 0.0f;
									translateVector[1] = 0.0f;
									translateVector[2] = -0.1f;
									MVstack.translate(translateVector);
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									glBindTexture(GL_TEXTURE_2D, groundTex.getTextureID());
									boxWand.render();
								MVstack.pop();
								//render brush------------------------
								MVstack.push();
									/*MVstack.scale(wandRadius);
									glUseProgram(sphereShader.programID);
									glUniformMatrix4fv(locationWandP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
									glUniformMatrix4fv(locationWandMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									sphereWand.render();*/
								MVstack.pop();
							MVstack.pop();
						MVstack.pop();
					MVstack.pop();
				}
				break;
			}

			// ===================================================================================================================================
			// 5 - Save mode
			// ===================================================================================================================================
			case 2: {

				// 5.1 CHECK THREAD \_____________________________________________________________________________________________________________
				if (th2.joinable()) {
					th2.join();
					th2Status = 0;
					finishedTime = time(NULL) + 3; // set timer to wait 5 secounds before leaving save to give user feedback
				}

				if (th2Status == 0) {
					if (time(NULL) >= finishedTime) { // exit save mode when timer reached
						mode = 0; // return to modelling mode
					}
				}

				// Begin the frame...
				ovrHmd_BeginFrame(hmd, l_FrameIndex);
				// Get eye poses for both the left and the right eye. g_EyePoses contains all Rift information: orientation, positional tracking and
				// the IPD in the form of the input variable g_EyeOffsets.
				ovrHmd_GetEyePoses(hmd, l_FrameIndex, g_EyeOffsets, g_EyePoses, NULL);
				// Bind the FBO...
				glBindFramebuffer(GL_FRAMEBUFFER, l_FBOId);
				//glModes
				GLRenderCallsOculus();

				for (int l_EyeIndex = 0; l_EyeIndex<ovrEye_Count; l_EyeIndex++) {

					// 5.2 OCULUS/CAMERA TRANSFORMS \______________________________________________________________________________________________
					MVstack.push();
						ovrEyeType l_Eye = hmd->EyeRenderOrder[l_EyeIndex];

						glViewport(g_EyeTextures[l_Eye].Header.RenderViewport.Pos.x,
							g_EyeTextures[l_Eye].Header.RenderViewport.Pos.y,
							g_EyeTextures[l_Eye].Header.RenderViewport.Size.w,
							g_EyeTextures[l_Eye].Header.RenderViewport.Size.h);

						glUseProgram(sceneShader.programID);
						// Pass projection matrix on to OpenGL...
						glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
						glUniform1i(locationTex, 0);

						// Multiply with orientation retrieved from sensor...
						OVR::Quatf l_Orientation = OVR::Quatf(g_EyePoses[l_Eye].Orientation);
						OVR::Matrix4f l_ModelViewMatrix = OVR::Matrix4f(l_Orientation.Inverted());
						MVstack.multiply(&(l_ModelViewMatrix.Transposed().M[0][0]));

						//!-- Translation due to positional tracking (DK2) and IPD...
						float eyePoses[3] = { -g_EyePoses[l_Eye].Position.x, -g_EyePoses[l_Eye].Position.y, -g_EyePoses[l_Eye].Position.z };
						MVstack.translate(eyePoses);

						//POSSABLY DOABLE IN SHADER
						pmat4 = MVstack.getCurrentMatrix();
						for (int i = 0; i < 16; i++)
							mat4[i] = pmat4[i];

						//linAlg::transpose(mat4);
						linAlg::vectorMatrixMult(mat4, lPos, LP);
						linAlg::vectorMatrixMult(mat4, lPos2, lPosTemp);
						glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix()); //TODO: check if this uniform is needed

						// 5.3 - Scene Matrix stack \__________________________________________________________________________________________________
						MVstack.push();
							// 5.3.1 RENDER BOARD >----------------------------------------------------------------------------------------------------
							glUniform4fv(locationLP, 1, LP);
							MVstack.push();
								MVstack.translate(board.getPosition());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								glBindTexture(GL_TEXTURE_2D, groundTex.getTextureID());
								board.render();
							MVstack.pop();


							glBindTexture(GL_TEXTURE_2D, whiteTex.getTextureID());

							// 5.3.2 Render tracking range >-------------------------------------------------------------------------------------------
							MVstack.push();
								MVstack.translate(trackingRange.getPosition());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
								glLineWidth(2.0f);
								trackingRange.render();
								glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
							MVstack.pop();

							// 5.3.3 Render title >----------------------------------------------------------------------------------------------------
							glUseProgram(menuShader.programID);
							glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
							
							MVstack.push();
								MVstack.translate(title.getPosition());
								MVstack.rotX(1.57079f);
								glBindTexture(GL_TEXTURE_2D, titleTex.getTextureID());
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								title.render();
							MVstack.pop();

							// 5.3.4 Render modelling buttons >-----------------------------------------------------------------------------------------
							// info
							for (int i = 0; i < NR_OF_MODELLING_BUTTONS; i++) {

								if (modellingButton[i]->getState()) {
									glUseProgram(bloomShader.programID);
									glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
								}
								else {
									glUseProgram(menuShader.programID);
									glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
								}

								MVstack.push();
									glBindTexture(GL_TEXTURE_2D, modellingButtonTex[i]->getTextureID());

									MVstack.translate(modellingButton[i]->getPosition());
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									modellingButton[i]->render();
								MVstack.pop();
							}

							if (th2Status == 0) {
								glUseProgram(menuShader.programID);
								glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
								MVstack.push();
								glBindTexture(GL_TEXTURE_2D, savedFeedbackTex.getTextureID());
								MVstack.translate(savedFeedback.getPosition());
								MVstack.rotX(1.57079f);
								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								savedFeedback.render();
								MVstack.pop();
							}


							//glBindTexture(GL_TEXTURE_2D, 0);
							// 5.3.5 Render mesh >------------------------------------------------------------------------------------------------------
							glUseProgram(meshShader.programID);
							glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));


							MVstack.push();
								MVstack.translate(modellingMesh->getPosition());
								MVstack.multiply(modellingMesh->getOrientation());
								glUniformMatrix4fv(locationMeshMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								glUniform4fv(locationMeshLP, 1, LP);
								glUniform4fv(locationMeshLP2, 1, lPosTemp);

								if (lines) {
									glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
									modellingMesh->render();
									glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
								}
								else {
									modellingMesh->render();
								}

							MVstack.pop();

							glUseProgram(sceneShader.programID);
							glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
							// 5.3.6 Render wand >------------------------------------------------------------------------------------------------------
							MVstack.push();
								MVstack.translate(wandPos);
								MVstack.multiply(wand->getOrientation());

								glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
								MVstack.push();
									translateVector[0] = 0.0f;
									translateVector[1] = 0.0f;
									translateVector[2] = -0.1f;
									MVstack.translate(translateVector);
									glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									glBindTexture(GL_TEXTURE_2D, groundTex.getTextureID());
									boxWand.render();
								MVstack.pop();
								//render brush------------------------
								//MVstack.push();
									/*	MVstack.scale(wandRadius);
									glUseProgram(sphereShader.programID);
									glUniformMatrix4fv(locationWandP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
									glUniformMatrix4fv(locationWandMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
									sphereWand.render();*/
								//MVstack.pop();
							MVstack.pop();
						MVstack.pop();
					MVstack.pop();
				}
				break;
			}
		}
		// Back to the default framebuffer...
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Do everything, distortion, front/back buffer swap...
		ovrHmd_EndFrame(hmd, g_EyePoses, g_EyeTextures);
		++l_FrameIndex;
		glfwPollEvents();

	}

	// join threads
	if (th1Status != 0) {
		th1.join();
		th1Status = 0;
	}
	if (th2Status != 0) {
		th2.join();
		th2Status = 0;
	}

	// Clean up FBO...
	glDeleteRenderbuffers(1, &l_DepthBufferId);
	glDeleteTextures(1, &l_TextureId);
	glDeleteFramebuffers(1, &l_FBOId);

	// Clean up window...
	glfwDestroyWindow(l_Window);
	glfwTerminate();

	// Clean up Oculus...
	ovrHmd_Destroy(hmd);
	ovr_Shutdown();

	return 1;
}

static void WindowSizeCallback(GLFWwindow* p_Window, int p_Width, int p_Height) {
	if (p_Width>0 && p_Height>0) {
		g_Cfg.OGL.Header.BackBufferSize.w = p_Width;
		g_Cfg.OGL.Header.BackBufferSize.h = p_Height;

		ovrBool l_ConfigureResult = ovrHmd_ConfigureRendering(hmd, &g_Cfg.Config, G_DISTORTIONCAPS, hmd->MaxEyeFov, g_EyeRenderDesc);
		if (!l_ConfigureResult) {
			printf("Configure failed.\n");
			exit(EXIT_FAILURE);
		}
	}
}

void GLRenderCallsOculus(){
	// Clear...
	//GL calls
	glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_FLAT);
	glShadeModel(GL_FLAT);
	glCullFace(GL_BACK);
	//glDisable(GL_TEXTURE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CCW);
	//glEnable(GL_POLYGON_STIPPLE);

	if (L_MULTISAMPLING) {
		glEnable(GL_MULTISAMPLE);
	}
	else {
		glDisable(GL_MULTISAMPLE);
	}
}

//! checks if a menu item is choosen and sets the appropriate state 
int handleMenu(float* wandPosition, menuBox** menuItem, const int nrOfModellingUIButtons, int* state) {
	for (int i = 0; i < nrOfModellingUIButtons; i++) {
		if (wandPosition[0] < menuItem[i]->getPosition()[0] + menuItem[i]->getDim()[0] /2.0f
			&& wandPosition[0] > menuItem[i]->getPosition()[0] - menuItem[i]->getDim()[0] / 2.0f
			&& wandPosition[1] > menuItem[i]->getPosition()[1] - menuItem[i]->getDim()[1] / 2.0f
			&& wandPosition[1] < menuItem[i]->getPosition()[1] + menuItem[i]->getDim()[1] / 2.0f
			&& wandPosition[2] > menuItem[i]->getPosition()[2] - menuItem[i]->getDim()[2] / 2.0f
			&& wandPosition[2] < menuItem[i]->getPosition()[2] + menuItem[i]->getDim()[2] / 2.0f) {									// check the item on left side

			// set state
			if (state[i] == 0) {
				state[i] = 1;				// set to just pressed
			}
			else {
				state[i] = 2;				// set to held down
			}
			return i;
		}
		else {
			if (state[i] == 2 || state[i] == 1) {
				state[i] = 3;				// set to just released
				//menuItem[i].setState(false);
				return i;
			}
			else if (state[i] == 3) {
				state[i] = 0;				// set to deactivated
			}
		}
	}
	return -1;
}

std::vector<std::string> getSavedFileNames() {
	const std::string folder = "../savedFiles";
	std::vector<std::string> names;
	char search_path[200];
	sprintf(search_path, "%s/*.bin", folder.c_str());
	WIN32_FIND_DATA fd;

	// load saved filenames in savedFiles dir
	HANDLE hFind = ::FindFirstFile(search_path, &fd);
	if (hFind != INVALID_HANDLE_VALUE) {
		do {
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
				std::string tempFileName = fd.cFileName;
				names.push_back("../savedFiles/" + tempFileName);
			}
		} while (::FindNextFile(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}

void loadStaticMesh(StaticMesh* item, std::string fileName) {
	loaderMeshLock.lock();
		item->load(fileName);
	loaderMeshLock.unlock();
	th1Status = 2;
}

void loadMesh(DynamicMesh* item, std::string fileName) {
	meshLock.lock();
		item->load(fileName);
	meshLock.unlock();
	th2Status = 2;
}

void saveFile(DynamicMesh* item) {
	meshLock.lock();
		item->save();
	meshLock.unlock();
	th2Status = 2;
}