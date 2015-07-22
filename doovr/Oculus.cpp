//#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_EXPOSE_NATIVE_WGL

#include "Oculus.h"
#include "Shader.h"
#include "MatrixStack.h"
#include "Sphere.h"
#include "MenuItem.h"
#include "Box.h"
#include "hexBox.h"
#include "Texture.h"
#include "Wand.h"
#include "Passive3D.h"
#include "TrackingRange.h"
#include "DynamicMesh.h"
#include "StaticMesh.h"

#include <thread>
#include <mutex>

// ------- Function declerations --------
//! Sets up a glfw window depending on the resolution of the Oculus Rift device
static void WindowSizeCallback(GLFWwindow *p_Window, int p_Width, int p_Height);
//! checks if the wand is colliding with a menuItem and sets the menuItems state accordingly, returns true if a menuItem choise has occured
int handleMenu(float* wandPosition, MenuItem** menuItem, const int nrOfModellingUIButtons, int* state);
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

	//glfwSetKeyCallback(l_Window, KeyCallback);
	glfwSetWindowSizeCallback(l_Window, WindowSizeCallback);

	//=====================================================================================================================================
	// 2 - Variable Declarations
	//=====================================================================================================================================

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
	float translateVector[3] = { 0.0f, 0.0f, 0.0f };
	float moveVec[3];
	float tempVec[3];
	float lastPos[3];
	float lastPos2[3];

	float* wandStartPos;
	float* wandCurrPos;
	float* wandNewPos;
	float* wandVelocity;
	float* listStartPos;

	float currTime = 0;
	float lastTime;
	float deltaTime;

	float listVelocity = 0.0f;
	float prevListVelocity = 0.0f;
	float listAcelleration = 0.0f;
	float listLeft = 0.0f;
	float listRight = 0.0f;

	// 2.3 - Threads used for loading and saving meshes \__________________________________________________________________________________
	std::thread th1; // not used
	std::thread th2; //
	// indicates if something has been loaded by a thread and gl data has not yet been updated
	bool loadingMesh = false;

	// 2.4 - textures \____________________________________________________________________________________________________________________
	glEnable(GL_TEXTURE_2D);
	// Scene textures

	Texture whiteTex("../Assets/Textures/light.DDS");
	Texture groundTex("../Assets/Textures/stone.DDS");
	Texture titleTex("../Assets/Textures/Title.DDS");
	Texture resetTex("../Assets/Textures/reset.DDS");
	Texture saveTex("../Assets/Textures/save.DDS");
	Texture loadTex("../Assets/Textures/load.DDS");
	Texture wireFrameTex("../Assets/Textures/wireframe.DDS");
	Texture plusTex("../Assets/Textures/plus.DDS");
	Texture minusTex("../Assets/Textures/minus.DDS");

	// 2.5 - Modes \______________________________________________________________________________________________________________________
	//! contains the number of any active states
	int mode = 0;
	int activeButton = -1;
	// 2.5.1 - modelling states and variables used in modelling mode >--------------------------------------------------------------------
	/*! 0 indicates that the state is not active,
	1 indicates that the state has just been activated
	2 indicates that the state is active
	3 indicates that the state has just been deactivated

	state[0] is the use tool state
	state[1] is the moveMesh state
	state[2] is the changeWandSize state
	*/
	const int NR_OF_MODELLING_STATES = 3;
	int modellingState[NR_OF_MODELLING_STATES] = { 0 };
	int aModellingStateIsActive = 0;

	const int NR_OF_MODELLING_BUTTONS = 5;
	/*!	[0] resetMesh
	[1] save
	[2] load
	[3] Wireframe
	[4] increase wand size */
	MenuItem* modellingButton[NR_OF_MODELLING_BUTTONS];

	Texture* modellingButtonTex[NR_OF_MODELLING_BUTTONS];
	modellingButtonTex[0] = &resetTex;
	modellingButtonTex[1] = &saveTex;
	modellingButtonTex[2] = &loadTex;
	modellingButtonTex[3] = &wireFrameTex;
	modellingButtonTex[4] = &plusTex;

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

	modellingButton[0] = new MenuItem(-0.2f, -0.26f, -0.2f, 0.08f, 0.08); // place reset menuItem on the other side

	for (int i = -(NR_OF_MODELLING_BUTTONS - 1) / 2 + 1; i <= (NR_OF_MODELLING_BUTTONS - 1) / 2; i++) {
		modellingButton[i + (NR_OF_MODELLING_BUTTONS - 1) / 2] = new MenuItem(0.2f, -0.26f, -0.25f + i * 0.0801f, 0.08f, 0.08);
	}
	//variable used with the button that switches wireframe, TODO: should be replaced by something
	bool lines = false;

	// 2.5.2 - variables used in Load Mode >------------------------------------------------------------------------------------------------
	//first two items are used as covers at both ends of the list
	MenuItem* scrollList[7];
	int leftListIndex = 2;
	Texture* scrollListTex[7];
	scrollListTex[0] = &loadTex;
	scrollListTex[1] = &loadTex;
	scrollListTex[2] = &loadTex;
	scrollListTex[3] = &loadTex;
	scrollListTex[4] = &loadTex;
	scrollListTex[5] = &loadTex;
	scrollListTex[6] = &loadTex;

	const int NR_OF_LOAD_BUTTONS = 3;
	/*! 0 = scrollArea
	1 = loadFile button
	2 = exitLoad button*/
	MenuItem* loadButton[NR_OF_LOAD_BUTTONS];
	int loadButtonState[NR_OF_LOAD_BUTTONS] = { 0 };

	Texture* loadButtonTex[NR_OF_LOAD_BUTTONS];
	//loadButtonTex[0] = scrollcover
	loadButtonTex[1] = &loadTex;
	loadButtonTex[2] = &minusTex;

	// 2.6 - Shader variables \_____________________________________________________________________________________________________________
	Shader sceneShader;
	sceneShader.createShader("sceneV.glsl", "sceneF.glsl");
	Shader meshShader;
	meshShader.createShader("meshV.glsl", "meshF.glsl");
	Shader bloomShader;
	bloomShader.createShader("bloomV.glsl", "bloomF.glsl");
	Shader menuShader;
	menuShader.createShader("menuV.glsl", "menuF.glsl");

	// 2.6.1 - Uniform variables >-----------------------------------------------------------------------------------------------------------
	GLint locationLP = glGetUniformLocation(sceneShader.programID, "lightPos");
	GLint locationP = glGetUniformLocation(sceneShader.programID, "P"); //perspective matrix
	GLint locationMV = glGetUniformLocation(sceneShader.programID, "MV"); //modelview matrix
	GLint locationTex = glGetUniformLocation(sceneShader.programID, "tex"); //texcoords

	GLint locationMeshMV = glGetUniformLocation(meshShader.programID, "MV"); //modelview matrix
	GLint locationMeshP = glGetUniformLocation(meshShader.programID, "P"); //perspective matrix
	GLint locationMeshLP = glGetUniformLocation(meshShader.programID, "lightPos");
	GLint locationMeshLP2 = glGetUniformLocation(meshShader.programID, "lightPos2");

	// 2.7 - Scene objects and variables \___________________________________________________________________________________________________

	// 2.7.1 - Matrix stack and static scene objects >---------------------------------------------------------------------------------------
	MatrixStack MVstack; MVstack.init();
	Box board(0.0f, -0.28f, -0.25f, 1.4, 0.02, 0.70); TrackingRange trackingRange(0.0f, -0.145f, -0.25f, 0.50, 0.25, 0.50);
	MenuItem title(0.0f, 0.9f, -0.95f, 0.5f, 0.5f);
	MenuItem wandSizePanel(0.2f, -0.25f, -0.12f, 0.10f, 0.03f);

	// 2.7.2 - Wand variables >--------------------------------------------------------------------------------------------------------------
	Box boxWand(0.0f, 0.0f, 0.0f, 0.007f, 0.007f, 0.2f);
	// Initilise passive wand
	Passive3D* wand = new Passive3D();
	// Size of the wand tool
	float wandRadius = 0.01f;
	float lastRadius;

	// 2.7.3 - Mesh variables >--------------------------------------------------------------------------------------------------------------
	DynamicMesh* modellingMesh = new DynamicMesh("2015-07-22_16-08-10.bin");
	//DynamicMesh* modellingMesh = new DynamicMesh(0.05f);
	// variables for browsing saved meshes
	//Mesh* staticMesh;
	//Mesh* tempStaticMesh = new Mesh();
	std::vector<std::string> meshFile;
	//! fileIndex is the index in fileName of the staticMesh that is shown
	int fileIndex = 0;

	StaticMesh* placeHolder;
	StaticMesh* previewMesh;
	StaticMesh* loaderMesh;

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

		switch (mode) {
			//===============================================================================================================================
			// 3 - Modelling Mode
			//===============================================================================================================================
		case 0: {

			// 3.1 - modellingstates \_____________________________________________________________________________________________________
			//3.1.1 - use modellingtool >--------------------------------------------------------------------------------------------------
			modellingMesh->markUp(wand, wandRadius);
			if (glfwGetKey(l_Window, GLFW_KEY_SPACE)) {
				if (modellingState[0] == 0) {
					modellingState[0] = 1;
					modellingMesh->push(wand, wandRadius);

					aModellingStateIsActive++;
				}
				else if (modellingState[0] == 1) {
					modellingState[0] = 2;

				}
				else if (modellingState[0] == 2)
				{
					modellingMesh->push(wand, wandRadius);
				}
			}
			else {
				if (modellingState[0] == 3) {
					modellingState[0] = 0;
				}
				else if (modellingState[0] != 0) {
					modellingState[0] = 3;

					aModellingStateIsActive--;
				}
			}
			modellingMesh->updateOGLData();
			//3.1.2 - move mesh >-----------------------------------------------------------------------------------------------------------
			if (glfwGetKey(l_Window, GLFW_KEY_LEFT_ALT)) {
				if (modellingState[1] == 0) {
					modellingState[1] = 1;
					lastPos[0] = wand->getPosition()[0];
					lastPos[1] = wand->getPosition()[1];
					lastPos[2] = wand->getPosition()[2];
					lastPos2[0] = modellingMesh->getPosition()[0];
					lastPos2[1] = modellingMesh->getPosition()[1];
					lastPos2[2] = modellingMesh->getPosition()[2];

					aModellingStateIsActive++;
				}
				else if (modellingState[1] == 1) {
					modellingState[1] = 2;
				}
				else if (modellingState[1] == 2)
				{
					//	move mesh
					linAlg::calculateVec(wand->getPosition(), lastPos, moveVec);
					moveVec[0] = lastPos2[0] + moveVec[0];
					moveVec[1] = lastPos2[1] + moveVec[1];
					moveVec[2] = lastPos2[2] + moveVec[2];

					modellingMesh->setPosition(moveVec);
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
			if (glfwGetKey(l_Window, GLFW_KEY_Q)) {
				wandRadius += 0.001f;
			}
			if (glfwGetKey(l_Window, GLFW_KEY_W)) {
				wandRadius -= 0.001f;
			}

			// 3.2 - handelmenu and menuswitch \______________________________________________________________________________________________
			if (aModellingStateIsActive == 0) {

				activeButton = handleMenu(wand->getPosition(), modellingButton, NR_OF_MODELLING_BUTTONS, modellingButtonState);
				switch (activeButton) {
					//3.2.1 - new mesh button>----------------------------------------------------------------------------------------------
				case 0: {
					if (modellingButtonState[activeButton] == 1) {
						// reset mesh
						delete modellingMesh; // Reset mesh
						modellingMesh = new DynamicMesh(0.3f);

						modellingButton[activeButton]->setState(true);

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

						if (meshLock.try_lock()) {
							meshLock.unlock();
							th2 = std::thread(saveFile, modellingMesh);
						}
					}

					//TODO: MOVE THIS JOIN AND REMOVE this if
					else if (modellingButtonState[activeButton] == 2) {
						if (th2.joinable()) {
							th2.join();
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
							//modellingState[3] = 0;
						}
						else {
							// saved files found
							// set staticMesh as main mesh as a temporary loading mesh indicator
							placeHolder = new StaticMesh(); placeHolder->load("../Assets/Models/placeHolder.bin"); placeHolder->createBuffers();
							loaderMesh = new StaticMesh();

							previewMesh = placeHolder;

							th1 = std::thread(loadStaticMesh, loaderMesh, meshFile[fileIndex % meshFile.size()]);

							// create the scrollList and the scrollListCover
							for (int i = -2; i < 3; i++) {
								scrollList[i + 2] = new MenuItem(0.05f * i, -0.26f, -0.1, 0.05, 0.05);
							}
							scrollList[5] = new MenuItem(-0.125f, -0.2599f, -0.1f, 0.07f, 0.06f);
							scrollList[6] = new MenuItem(0.125f, -0.2599f, -0.1f, 0.07f, 0.06f);

							scrollList[2]->setState(true);

							loadButton[0] = new MenuItem(0.0f, -0.2599f, -0.1f, 0.4f, 0.05f);
							loadButton[1] = new MenuItem(0.05f, -0.2599f, -0.15f, 0.1, 0.05);
							loadButton[2] = new MenuItem(-0.05f, -0.2599f, -0.15f, 0.1, 0.05);

							//modellingState[3] = 2;
							//activeStates.push_back(3);
							mode = 1;
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

						lastPos[0] = wand->getPosition()[0];
						lastPos[1] = wand->getPosition()[1];
						lastPos[2] = wand->getPosition()[2];
					}

					if (modellingButtonState[activeButton] == 3)
						modellingButton[activeButton]->setState(false);

					break;
				}
				}

			}

			// ================================////////////// join thread after load, TODO refactor this
			if (th2.joinable()) {
				th2.join();
			}
			// ================================//////////////

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
				glUseProgram(menuShader.programID);
				glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));

				MVstack.push();
				MVstack.translate(title.getPosition());
				MVstack.rotX(1.57079f);
				glBindTexture(GL_TEXTURE_2D, titleTex.getTextureID());
				glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
				title.render();
				MVstack.pop();

				// 3.4.4 Render modelling buttons >-----------------------------------------------------------------------------------------
				// info
				for (int i = 0; i < NR_OF_MODELLING_BUTTONS; i++) {

					if (modellingButton[i]->getState()) {
						glUseProgram(bloomShader.programID);
						glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
					}
					else {
						glUseProgram(menuShader.programID);
						glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
					}

					MVstack.push();
					glBindTexture(GL_TEXTURE_2D, modellingButtonTex[i]->getTextureID());

					MVstack.translate(modellingButton[i]->getPosition());
					glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
					modellingButton[i]->render();
					MVstack.pop();
				}

				//glBindTexture(GL_TEXTURE_2D, 0);
				// 3.4.5 Render mesh >------------------------------------------------------------------------------------------------------
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
				// 3.4.6 Render wand >-------------------------------------------------------------------------------------------
				MVstack.push();
				MVstack.translate(wand->getPosition());
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
				/*	MVstack.scale(wandRadius);
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
		case 1: {
			//===============================================================================================================================
			// 4 - LOAD Mode
			//===============================================================================================================================
			// 4.1 - Keyboard events \_______________________________________________________________________________________________________
			// 4.1.1 - Move mesh >-----------------------------------------------------------------------------------------------------------
			if (glfwGetKey(l_Window, GLFW_KEY_LEFT_ALT)) {
				if (modellingState[1] == 0) {
					modellingState[1] = 1;
					lastPos[0] = wand->getPosition()[0];
					lastPos[1] = wand->getPosition()[1];
					lastPos[2] = wand->getPosition()[2];
					lastPos2[0] = modellingMesh->getPosition()[0];
					lastPos2[1] = modellingMesh->getPosition()[1];
					lastPos2[2] = modellingMesh->getPosition()[2];

					aModellingStateIsActive++;
				}
				else if (modellingState[1] == 1) {
					modellingState[1] = 2;

					//	move mesh
					linAlg::calculateVec(wand->getPosition(), lastPos, moveVec);
					moveVec[0] = lastPos2[0] + moveVec[0];
					moveVec[1] = lastPos2[1] + moveVec[1];
					moveVec[2] = lastPos2[2] + moveVec[2];

					modellingMesh->setPosition(moveVec);
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

			// 4.1.1 - Close application >---------------------------------------------------------------------------------------------------
			if (glfwGetKey(l_Window, GLFW_KEY_ESCAPE)) {
				glfwSetWindowShouldClose(l_Window, GL_TRUE);
			}


			// 4.2 - Handle buttons and button switch \______________________________________________________________________________________

			activeButton = handleMenu(wand->getPosition(), loadButton, NR_OF_LOAD_BUTTONS, loadButtonState);

			switch (activeButton) {
			case 0:{
				if (loadButtonState[activeButton] == 1) {

					listVelocity = 0.0f;
					listAcelleration = 0.0f;

					scrollList[5]->setState(true);
					scrollList[6]->setState(true);
				}
				else if (loadButtonState[activeButton] == 2) {

					wandVelocity = wand->getVelocity(deltaTime);
					listVelocity = wandVelocity[0];

					/*for (int i = 0; i < 5; i++)
					{
						listStartPos = scrollList[i]->getPosition();

						listLeft = -0.11f - (listStartPos[0] + listVelocity*deltaTime );
						listRight = listStartPos[0] + listVelocity*deltaTime - 0.11f;

						if (listLeft > 0.0f)
						{
							tempVec[0] = 0.11 - listLeft; tempVec[1] = listStartPos[1]; tempVec[2] = listStartPos[2];
							scrollList[i]->setPosition(tempVec);
						}
						else if (listRight > 0.0f)
						{
							tempVec[0] = -0.11 + listRight; tempVec[1] = listStartPos[1]; tempVec[2] = listStartPos[2];
							scrollList[i]->setPosition(tempVec);
						}
						else
						{
							tempVec[0] = listStartPos[0] + listVelocity*deltaTime + i*0.05f; tempVec[1] = listStartPos[1]; tempVec[2] = listStartPos[2];
							scrollList[i]->setPosition(tempVec);
						}
					}*/

				}
				else if (loadButtonState[activeButton] == 3)
				{
					wandVelocity = wand->getVelocity(deltaTime);
					listVelocity = wandVelocity[0];
					listAcelleration = -0.5*wandVelocity[0];

					scrollList[5]->setState(false);
					scrollList[6]->setState(false);
				}
				break;
			}
			case 1: {

				break;
			}
			case 2: {

				break;
			}
			default:{

				break;
			}
			}


			if (listVelocity == 0) {
				if (previewMesh != loaderMesh && loaderMeshLock.try_lock()) {
					if (th1.joinable()) {
						th1.join();
					}

					loaderMeshLock.unlock();
					previewMesh = placeHolder;
					th1 = std::thread(loadStaticMesh, loaderMesh, meshFile[fileIndex % meshFile.size()]);
				}
			}
			else {
				

				for (int i = 0; i < 5; i++) {
					listStartPos = scrollList[i]->getPosition();

					if (listStartPos[0] < 0.01f && listStartPos[0] > -0.01f) {
						scrollList[i]->setState(true);
						if (listVelocity > 0.04f && listVelocity < -0.04f) {

							listVelocity = listVelocity + listAcelleration*deltaTime;
							listAcelleration = -0.5*listVelocity;
						}
						else {
							listVelocity = listVelocity + listAcelleration*deltaTime;
							listAcelleration = -1.0*listVelocity;
						}
					}
					else {
						scrollList[i]->setState(false);
					}


					listLeft = -0.125f - (listStartPos[0] + listVelocity*deltaTime);
					listRight = listStartPos[0] + listVelocity*deltaTime - 0.125f;
					if (listLeft > 0.0f) {
						tempVec[0] = 0.125f; tempVec[1] = listStartPos[1]; tempVec[2] = listStartPos[2];
						scrollList[i]->setPosition(tempVec);
						fileIndex++;
					}
					else if (listRight > 0.0f) {
						tempVec[0] = -0.125f; tempVec[1] = listStartPos[1]; tempVec[2] = listStartPos[2];
						scrollList[i]->setPosition(tempVec);
						fileIndex--;
					}
					else {
						tempVec[0] = listStartPos[0] + listVelocity*deltaTime; tempVec[1] = listStartPos[1]; tempVec[2] = listStartPos[2];
						scrollList[i]->setPosition(tempVec);
					}
					//tempVec[0] = listStartPos[0] + moveVec[0] + i*0.05f; tempVec[1] = listStartPos[1]; tempVec[2] = listStartPos[2];
					//scrollList[(leftListIndex + i)%5]->setPosition(tempVec);
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
				glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));

				MVstack.push();
					MVstack.translate(title.getPosition());
					MVstack.rotX(1.57079f);
					glBindTexture(GL_TEXTURE_2D, titleTex.getTextureID());
					glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
					title.render();
				MVstack.pop();


				// 4.4.4 - RENDER Load buttons >--------------------------------------------------------------------------------------------
				for (int i = 1; i < NR_OF_LOAD_BUTTONS; i++) {

					if (loadButton[i]->getState()) {
						glUseProgram(bloomShader.programID);
						glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
					}
					else {
						glUseProgram(menuShader.programID);
						glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
					}

					MVstack.push();
					glBindTexture(GL_TEXTURE_2D, loadButtonTex[i]->getTextureID());

					MVstack.translate(loadButton[i]->getPosition());
					glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
					loadButton[i]->render();
					MVstack.pop();
				}


				// 4.4.5 - RENDER listitems >---------------------------------------------------------------------------------------------
				// info
				for (int i = 6; i > -1; i--) {

					if (scrollList[i]->getState()) {
						glUseProgram(bloomShader.programID);
						glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
					}
					else {
						glUseProgram(menuShader.programID);
						glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
					}

					MVstack.push();

					glBindTexture(GL_TEXTURE_2D, scrollListTex[i]->getTextureID());

					MVstack.translate(scrollList[i]->getPosition());
					glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
					scrollList[i]->render();
					MVstack.pop();
				}

				//glBindTexture(GL_TEXTURE_2D, 0);

				// 4.4.6 - RENDER mesh >--------------------------------------------------------------------------------------------------
				glUseProgram(meshShader.programID);
				glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));

				if (th1.joinable()) {
					th1.join();
					previewMesh = loaderMesh;
					previewMesh->createBuffers();
				}

				MVstack.push();
					MVstack.translate(previewMesh->getPosition());
					MVstack.multiply(previewMesh->getOrientation());
					glUniformMatrix4fv(locationMeshMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
					glUniform4fv(locationMeshLP, 1, LP);
					glUniform4fv(locationMeshLP2, 1, lPosTemp);

					if (lines) {
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						previewMesh->render();
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					}
					else {
						previewMesh->render();
					}
				MVstack.pop();

				glUseProgram(sceneShader.programID);
				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));

				// 4.4.7 - RENDER wand >--------------------------------------------------------------------------------------------------
				MVstack.push();
				MVstack.translate(wand->getPosition());
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

		}
		// Back to the default framebuffer...
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Do everything, distortion, front/back buffer swap...
		ovrHmd_EndFrame(hmd, g_EyePoses, g_EyeTextures);
		++l_FrameIndex;
		glfwPollEvents();

	}

	// Clean up FBO...
	glDeleteRenderbuffers(1, &l_DepthBufferId);
	glDeleteTextures(1, &l_TextureId);
	glDeleteFramebuffers(1, &l_FBOId);

	// Clean up Oculus...
	ovrHmd_Destroy(hmd);
	ovr_Shutdown();

	// Clean up window...
	glfwDestroyWindow(l_Window);
	glfwTerminate();

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
int handleMenu(float* wandPosition, MenuItem** menuItem, const int nrOfModellingUIButtons, int* state) {


	for (int i = 0; i < nrOfModellingUIButtons; i++) {
		if (wandPosition[1] < menuItem[i]->getPosition()[1] + 0.02f
			&& wandPosition[1] > menuItem[i]->getPosition()[1] - 0.02f
			&& wandPosition[0] > menuItem[i]->getPosition()[0] - menuItem[i]->getDim()[0] / 4.f
			&& wandPosition[0] < menuItem[i]->getPosition()[0] + menuItem[i]->getDim()[0] / 4.f
			&& wandPosition[2] > menuItem[i]->getPosition()[2] - menuItem[i]->getDim()[1] / 4.f
			&& wandPosition[2] < menuItem[i]->getPosition()[2] + menuItem[i]->getDim()[1] / 4.f) {									// check the item on left side

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
}

void loadMesh(DynamicMesh* item, std::string fileName) {
	meshLock.lock();
	item->load(fileName);
	meshLock.unlock();
}

void saveFile(DynamicMesh* item) {
	meshLock.lock();
	item->save();
	meshLock.unlock();
}