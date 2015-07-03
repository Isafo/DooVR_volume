//#define GLFW_EXPOSE_NATIVE_WIN32
//#define GLFW_EXPOSE_NATIVE_WGL

#include "Oculus.h"
#include "Shader.h"
#include "MatrixStack.h"
#include "Sphere.h"
#include "Plane.h"
#include "MenuItem.h"
#include "Box.h"
#include "hexBox.h"
#include "Texture.h"
#include "Wand.h"
#include "Passive3D.h"
#include "TrackingRange.h"

using namespace std;

// ------- Function declerations --------
//! Sets up a glfw window depending on the resolution of the Oculus Rift device
static void WindowSizeCallback(GLFWwindow *p_Window, int p_Width, int p_Height);
//! checks if the wand is colliding with a menuItem and sets the menuItems state accordingly, returns true if a menuItem choise has occured
void handleMenu(float* wandPosition, MenuItem* menuItem, const int nrOfMenuItems, int* state);
void GLRenderCallsOculus();

// --- Variable Declerations ------------
const bool L_MULTISAMPLING = false;
const int G_DISTORTIONCAPS = 0
                             | ovrDistortionCap_Vignette
                             | ovrDistortionCap_Chromatic
                             | ovrDistortionCap_Overdrive
//| ovrDistortionCap_TimeWarp // Turning this on gives ghosting???
;

ovrHmd hmd;
ovrGLConfig g_Cfg;
ovrEyeRenderDesc g_EyeRenderDesc[2];

const float EYEHEIGHT{OVR_DEFAULT_EYE_HEIGHT};
// --------------------------------------

// Global Constant variables
const int nFunctions = 7;
const int nLightsources = 3;

int Oculus::runOvr() {

	ovrVector3f g_EyeOffsets[2];
	ovrPosef g_EyePoses[2];
	ovrTexture g_EyeTextures[2];
	OVR::Matrix4f g_ProjectionMatrix[2];
	OVR::Sizei g_RenderTargetSize;
	//ovrVector3f g_CameraPosition;

	GLfloat I[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
					  0.0f, 1.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 1.0f, 0.0f,
					  0.0f, 0.0f, 0.0f, 1.0f };

	GLfloat P[16] = { 2.42f, 0.0f, 0.0f, 0.0f,
					  0.0f, 2.42f, 0.0f, 0.0f,
					  0.0f, 0.0f, -1.0f, -1.0f,
					  0.0f, 0.0f, -0.2f, 0.0f };
	

	// states
	//! contains the number of any active state
	vector<int> activeStates;

	/*! 0 indicates that the state is not active, 
	 * 1 indicates that the state has just been activated
	 * 2 indicates that the state is pressed
	 * 3 indicates that the state has just been deactivated
	 * 4 indicates that the state is active (on/off)
	 *
	 *
	 * state[0] is the modelling state
	 * state[1] is the moveMesh state
 	 */
	const int nrOfStates = 2;
	int state[nrOfStates] = { 0 };

	// Lightposition 
	float lPos[4] = { 2.0f, 2.0f, 2.0f, 1.0f};
	float lPos2[4] = { -2.0f, 2.0f, 2.0f, 1.0f };
	float lPosTemp[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float LP[4];
	float* pmat4;
	float mat4[16];

	// Save old positions and transforms
	float changePos[3] = { 0.0f };
	float differenceR[16] = { 0.0f };
	float* lastPos;
	float currPos[3] = { 0.0f, 0.0f, 0.0f };
	float translateVector[3] = { 0.0f, 0.0f, 0.0f };
	float moveVec[3];

	// Configuration variables
	int regCounter = 0;
	int n = 0;
	int resetCounter = 0;

	float pos[16] = { 0.0f };
	float invPos[16] = { 0.0f };
	float eyeHeight = OVR_DEFAULT_EYE_HEIGHT;
	float MAX_HEX_HEIGHT = -eyeHeight + 0.95f;
	float MIN_HEX_HEIGHT = -eyeHeight + 0.9f;

	// FPS
	double fps = 0;

	// Size of the wand tool
	float wandRadius = 0.01f;

	// States
	bool buttonPressed = false;
	bool buttonHeld = false;
	bool buttonReleased = false;
	bool lines = false;

	const int nrOfMenuItems = 6;
	/*!
		[0] save
		[1] load
		[2] reset
		[3] Wireframe
		[4] increase wand size
		[5] decrease wand size
	*/
	MenuItem menuItem[nrOfMenuItems];
	
	/*!
	0 indicates that the state is not active,
	1 indicates that the state has just been activated
	2 indicates that the state is active
	3 indicates that the state has just been deactivated

	[0] save
	[1] load
	[2] reset
	[3] Wireframe
	[4] increase wand size
	[5] decrease wand size
	*/
	static int menuState[nrOfMenuItems];


	// Location used for UNIFORMS in shader
	GLint locationLP;
	GLint locationP;
	GLint locationMV;
	GLint locationTex;

	GLint locationMeshMV;
	//GLint locationMeshLP[nLightsources + 1];
	GLint locationMeshP;
	GLint locationMeshLP;
	GLint locationMeshLP2;

	GLint locationWandMV;
	GLint locationWandP;

	//INITIALIZE OVR /////////////////////////////////////////////////////
	ovr_Initialize();
	int det;
	// Check for attached head mounted display...
	hmd = ovrHmd_Create(0);
	if (!hmd) {
		printf("No Oculus Rift device attached, using virtual version...\n");
		hmd = ovrHmd_CreateDebug(ovrHmd_DK2);

		det = ovrHmd_Detect();
		cout << det << endl;
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
	} else {// Extended Desktop mode...
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

	//DECLARE AND CREATE SHADERS ///////////////////////////////////////////////////////////////////////////////////
	Shader phongShader;
	phongShader.createShader("vertexshader.glsl", "fragmentshader.glsl");
	Shader meshShader;
	meshShader.createShader("vshader.glsl", "fshader.glsl");
	Shader sphereShader;
	sphereShader.createShader("vShaderWand.glsl", "fShaderWand.glsl");
	Shader bloomShader;
	bloomShader.createShader("vShaderBloom.glsl", "fShaderBloom.glsl");
	Shader menuShader;
	menuShader.createShader("vShaderMenuItem.glsl", "fShaderMenuItem.glsl");

	// CREATE MATRIX STACK
	MatrixStack MVstack;
	MVstack.init();
	
	// DECLARE SCENE OBJECTS ///////////////////////////////////////////////////////////////////////////////////
	Box board(0.0f, -0.28f, -0.25f, 1.4, 0.02, 0.70);
	TrackingRange trackingRange(0.0f, -0.145f, -0.25f, 0.50, 0.25, 0.50);
	
	MenuItem title(0.0f, 0.9f, -0.95f, 0.5f, 0.5f);

	for (int i = -nrOfMenuItems/2; i < nrOfMenuItems / 2; i++) {
		menuItem[i + nrOfMenuItems/2] = MenuItem(0.2f, -0.26f, -0.25f + i * 0.055f, 0.05f, 0.05);
	}

	// Wand = Box + sphere
	Box boxWand(0.0f, 0.0f, 0.0f, 0.007f, 0.007f, 0.2f);
	Sphere sphereWand(0.0f, 0.0f, 0.0f, 1.0f);

	// Initilise passive wand
	Passive3D* wand = new Passive3D();
	
	// TEXTURES ///////////////////////////////////////////////////////////////////////////////////////////////
	glEnable(GL_TEXTURE_2D);
	// Wand function textures
	Texture move("../Textures/move.DDS");
	Texture dilate("../Textures/up.DDS");
	Texture erode("../Textures/down.DDS");
	Texture dnp("../Textures/push.DDS");
	
	// Scene textures
	Texture whiteTex("../Textures/light.DDS");
	Texture groundTex("../Textures/stone.DDS");
	Texture coregister("../Textures/coregister3.DDS");
	Texture hexTex("../Textures/panel3.DDS");
	Texture menuInfoTex("../Textures/info.DDS");
	Texture menuLoadTex("../Textures/load.DDS");
	Texture menuMinusTex("../Textures/minus.DDS");
	Texture menuPlusTex("../Textures/plus.DDS");
	Texture menuResetTex("../Textures/reset.DDS");
	Texture menuSaveTex("../Textures/save.DDS");
	Texture menuWireTex("../Textures/wireframe.DDS");
	Texture titleTex("../Textures/Title.DDS");

	GLuint currentTexID = move.getTextureID();

	//UNIFORM VARIABLES WITH SHADER ///////////////////////////////////////////////////////////////////////////
	locationMV = glGetUniformLocation(phongShader.programID, "MV");						// ModelView Matrix
	locationP = glGetUniformLocation(phongShader.programID, "P");						// Perspective Matrix
	locationLP = glGetUniformLocation(phongShader.programID, "lightPos");				// Light position
	locationTex = glGetUniformLocation(phongShader.programID, "tex");					// Texture Matrix

	locationMeshMV = glGetUniformLocation(meshShader.programID, "MV");					// ModelView Matrix
	locationMeshP = glGetUniformLocation(meshShader.programID, "P");					// Perspective Matrix
	locationMeshLP = glGetUniformLocation(meshShader.programID, "lightPos");
	locationMeshLP2 = glGetUniformLocation(meshShader.programID, "lightPos2");

	locationWandMV = glGetUniformLocation(sphereShader.programID, "MV");					// ModelView Matrix
	locationWandP = glGetUniformLocation(sphereShader.programID, "P");						// Perspective Matrix

	//ovrHmd_RecenterPose(hmd);
	ovrHmd_DismissHSWDisplay(hmd); // dismiss health safety warning

	Mesh* mTest = new Mesh(0.05f);

	// Main loop...
	unsigned int l_FrameIndex = 0;
	// RENDER LOOP ////////////////////////////////////////////////////////////////////////////////////////
	while (!glfwWindowShouldClose(l_Window)) {
		
		// STATES
		// modelingstate
		if (glfwGetKey(l_Window, GLFW_KEY_SPACE)) {
			if (state[0] == 0) {
				state[0] = 1;
				activeStates.push_back(0);
			} else if (state[0] == 1) {
				state[0] = 2;
			}
		} else {
			if (state[0] == 3) {
				state[0] = 0;
			} else if (state[0] != 0) {
				state[0] = 3;
				activeStates.erase(remove(activeStates.begin(), activeStates.end(), 0), activeStates.end());
			}
		}

		// moveMesh state
		if (glfwGetKey(l_Window, GLFW_KEY_LEFT_SHIFT)) {
			if (state[1] == 0) {
				state[1] = 1;
				activeStates.push_back(1);
			} else if (state[1] == 1) {
				state[1] = 2;
			}
		} else {
			if (state[1] == 3) {
				state[1] = 0;
			} else if (state[1] != 0) {
				state[1] = 3;
				activeStates.erase(remove(activeStates.begin(), activeStates.end(), 1), activeStates.end());
			}
		}

		// Switch to execute active states, checks menu choices if none are active
		if (activeStates.empty()) {
			handleMenu(wand->getWandPosition(), menuItem, nrOfMenuItems, menuState);
			for (int i = 0; i < nrOfMenuItems; i++) {
				switch (i) {
					case 0: {
						if (menuState[i] == 1) {
							// save file
						}
					break;
					}
					case 1: {
						if (menuState[i] == 1) {
							// loadfile
						}
						break;
					}
					case 2: {
						if (menuState[i] == 1){
							// reset mesh
							delete mTest; // Reset mesh
							mTest = new Mesh(0.3f);
						}
						break;
					}
					case 3: {
						// wireframe
						if (menuState[i] == 4){
							lines = true;
						} else if (menuState[i] == 0){
							lines = false;
						}
						break;
					}
					case 4: {
						if (menuState[i] == 2 || menuState[i] == 1) {
							wandRadius += 0.001f;
						}
					break;
					}
					case 5: {
						if (menuState[i] == 2 || menuState[i] == 1) {
							wandRadius -= 0.001f;
						}
						break;
					}
				}
			}
		} else {
			for (int i = 0; i < activeStates.size(); i++) {
				switch (activeStates[i]) {
				  case 0: {
					mTest->sculpt(wand->getWandPosition(), lastPos, wandRadius, true);
					break;
				  }
				  case 1: {
					pmat4 = mTest->getPosition();

					/*
					cout << "#";
					cout << moveVec[0];
					cout << moveVec[1];
					cout << moveVec[2] << endl;
					cout << "::";
					cout << lastPos[0];
					cout << lastPos[1];
					cout << lastPos[2] << endl;
					cout << "||";
					cout << wand->getWandPosition()[0];
					cout << wand->getWandPosition()[1];
					cout << wand->getWandPosition()[2] << endl;
					currPos[0] = 1.0f; currPos[1] = 1.0f; currPos[2] = 1.0f;
					translateVector[0] = 1.5f; translateVector[1] = 1.5f; translateVector[2] = 1.5f;
					linAlg::calculateVec(moveVec, currPos, translateVector);
					cout << "#2";
					cout << moveVec[0];
					cout << moveVec[1];
					cout << moveVec[2] << endl;
					cout << 0.0001f - 0.00015f;
					*/

					linAlg::calculateVec(moveVec, wand->getWandPosition(), lastPos);
					pmat4[0] += moveVec[0];
					pmat4[1] += moveVec[1];
					pmat4[2] += moveVec[2];

					mTest->setPosition(pmat4);
					break;
				  }
				  default: {
					  break;
				  }
				}
			}
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		// KEYBORD EVENTS
		if (glfwGetKey(l_Window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(l_Window, GL_TRUE);
		}
		if (glfwGetKey(l_Window, GLFW_KEY_Q)) {
			wandRadius += 0.001f;
		}
		if (glfwGetKey(l_Window, GLFW_KEY_W)) {
			wandRadius -= 0.001f;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		lastPos = wand->getWandPosition();

		// Begin the frame...
		ovrHmd_BeginFrame(hmd, l_FrameIndex);

		// Get eye poses for both the left and the right eye. g_EyePoses contains all Rift information: orientation, positional tracking and
		// the IPD in the form of the input variable g_EyeOffsets.
		ovrHmd_GetEyePoses(hmd, l_FrameIndex, g_EyeOffsets, g_EyePoses, NULL);

		// Bind the FBO...
		glBindFramebuffer(GL_FRAMEBUFFER, l_FBOId);

		GLRenderCallsOculus();

		
		for (int l_EyeIndex = 0; l_EyeIndex<ovrEye_Count; l_EyeIndex++) {
			
			//OCULUS/CAMERA TRANSFORMS ------------------------------------------------------------------------------
			MVstack.push();
				ovrEyeType l_Eye = hmd->EyeRenderOrder[l_EyeIndex];

				glViewport(g_EyeTextures[l_Eye].Header.RenderViewport.Pos.x,
						   g_EyeTextures[l_Eye].Header.RenderViewport.Pos.y,
						   g_EyeTextures[l_Eye].Header.RenderViewport.Size.w,
						   g_EyeTextures[l_Eye].Header.RenderViewport.Size.h);

				glUseProgram(phongShader.programID);
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

				linAlg::transpose(mat4);
				linAlg::vectorMatrixMult(mat4, lPos, LP);
				linAlg::vectorMatrixMult(mat4, lPos2, lPosTemp);
				glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
			
				//SCENEOBJECT TRANSFORMS----------------------------------------------------------------
				MVstack.push();
					glBindTexture(GL_TEXTURE_2D, hexTex.getTextureID());

					//RENDER DIFFERENT HEXBOXES---------------------------------------------------------------------
					glUniform4fv(locationLP, 1, LP);
					MVstack.push();
						MVstack.translate(board.getPosition());
						glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
						glBindTexture(GL_TEXTURE_2D, groundTex.getTextureID());
						board.render();
					MVstack.pop();

					glBindTexture(GL_TEXTURE_2D, whiteTex.getTextureID());
					//TRACKINGRANGE

					MVstack.push();
						MVstack.translate(trackingRange.getPosition());
						glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
						glLineWidth(2.0f);
						trackingRange.render();
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					MVstack.pop();

					// TITLE
					glUseProgram(menuShader.programID);
					glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));

					MVstack.push();
						MVstack.translate(title.getPosition());
						MVstack.rotX(1.57079f);
						glBindTexture(GL_TEXTURE_2D, titleTex.getTextureID());
						glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
						title.render();
					MVstack.pop();

					//  MENU ITEMS -----------------------------------------------------------------------------------------------------
					// info
					for (int i = 0; i < nrOfMenuItems; i++) {

						if (menuItem[i].getState()) {
							glUseProgram(bloomShader.programID);
							glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
						} else {
							glUseProgram(menuShader.programID);
							glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
						}

						MVstack.push();

							if (i == 0)
								glBindTexture(GL_TEXTURE_2D, menuSaveTex.getTextureID());
							else if (i == 1)
								glBindTexture(GL_TEXTURE_2D, menuLoadTex.getTextureID());
							else if (i == 2)
								glBindTexture(GL_TEXTURE_2D, menuResetTex.getTextureID());
							else if (i == 3)
								glBindTexture(GL_TEXTURE_2D, menuWireTex.getTextureID());
							else if (i == 4)
								glBindTexture(GL_TEXTURE_2D, menuPlusTex.getTextureID());
							else
								glBindTexture(GL_TEXTURE_2D, menuMinusTex.getTextureID());

							MVstack.translate(menuItem[i].getPosition());
							glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
							menuItem[i].render();
						MVstack.pop();
					}



				

					glBindTexture(GL_TEXTURE_2D, 0);
					//RENDER MESH -----------------------------------------------------------------------
					glUseProgram(meshShader.programID);
					glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));

					MVstack.push();
						MVstack.translate(mTest->getPosition());
						MVstack.multiply(mTest->getOrientation());
						glUniformMatrix4fv(locationMeshMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
						//cout << lPosTemp[0] << " " << lPosTemp[1] << " " << lPosTemp[2] << " " << lPosTemp[3] << endl;
						glUniform4fv(locationMeshLP, 1, LP);
						glUniform4fv(locationMeshLP2, 1, lPosTemp);

						if (lines) {
						//if (true) {
							glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
							mTest->render();
							glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
						} else {
							mTest->render();
						}
					MVstack.pop();
						
					glUseProgram(phongShader.programID);
					glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
					//RENDER WAND---------------------------------------------------------------------------
					MVstack.push();
						MVstack.translate(wand->getWandPosition());
						MVstack.multiply(wand->getWandOrientation());

						glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
						MVstack.push();
							translateVector[0] = 0.0f;
							translateVector[1] = 0.0f;
							translateVector[2] = -0.1f;
							MVstack.translate(translateVector);
							glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
							glBindTexture(GL_TEXTURE_2D, hexTex.getTextureID());
							boxWand.render();
						MVstack.pop();

						MVstack.push();
							MVstack.scale(wandRadius);
							glUseProgram(sphereShader.programID);
							glUniformMatrix4fv(locationWandP, 1, GL_FALSE, &(g_ProjectionMatrix[l_Eye].Transposed().M[0][0]));
							glUniformMatrix4fv(locationWandMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
							sphereWand.render();
						MVstack.pop();	
					MVstack.pop();
						
					glUseProgram(phongShader.programID);
				MVstack.pop();

			MVstack.pop();			
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
    } else {
        glDisable(GL_MULTISAMPLE);
    }
}

//! checks if a menu item is choosen and sets the appropriate state 
void handleMenu(float* wandPosition, MenuItem* menuItem, const int nrOfMenuItems, int* state) {

	// check if the wandPosition is in the same Y and X position as the menuItem row
	if (wandPosition[1] < menuItem[0].getPosition()[1] + 0.01f && wandPosition[1] > menuItem[0].getPosition()[1] - 0.01f
		&& wandPosition[0] > menuItem[0].getPosition()[0] - menuItem[0].getDim()[0] / 2.f
		&& wandPosition[0] < menuItem[0].getPosition()[0] + menuItem[0].getDim()[0] / 2.f) {

		for (int i = 0; i < nrOfMenuItems; i++) {
			// check what menuitem is pressed
			if (wandPosition[2] < menuItem[i].getPosition()[2] + menuItem[i].getDim()[1] / 2.f
				&& wandPosition[2] > menuItem[i].getPosition()[2] - menuItem[i].getDim()[1] / 2.f) {
				
				// active on off state?
				if (state[i] != 4)
					menuItem[i].setState(true);
				else
					menuItem[i].setState(false);

				// set state
				if (state[i] == 0) {
					state[i] = 1;				// set to just pressed

					if (i == 3) {				// state 3 has on/off switch
						state[i] = 4;
					}

				} else if (state[i] == 1) {
					state[i] = 2;				// set to pressed down
				} else {
					state[i] = 0;				// deactivate on/off function
				}

			} 
		}
	} else {
		for (int i = 0; i < nrOfMenuItems; i++) {
			
			if (state[i] != 4)
				menuItem[i].setState(false);

			if (state[i] == 2 || state[i] == 1) {
				state[i] = 3;				// set to just released
			}
			else if (state[i] == 3) {
				state[i] = 0;				// set to deactivated
			}
		}
	}
}