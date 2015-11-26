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
#include "scalarField.h"
#include "Octree.h"

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
//
#include "Add.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <math.h>
#include <time.h>

#include "glm\glm\glm.hpp"
#include "glm\glm\gtc\matrix_transform.hpp"

//#include <OVR/OVR_CAPI.h>
#include <OVR/OVR.h>
#include <OVR/OVR_CAPI_GL.h>
//#include <OVR/Win32_GLAppUtil.h> //needed for OVR GL structs.


struct DepthBuffer
{
	GLuint        texId;

	DepthBuffer(OVR::Sizei size, int sampleCount)
	{
		OVR_ASSERT(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		GLenum internalFormat = GL_DEPTH_COMPONENT24;
		GLenum type = GL_UNSIGNED_INT;
		//if (GLE_ARB_depth_buffer_float)
		//{
		internalFormat = GL_DEPTH_COMPONENT32F;
		type = GL_FLOAT;
		//}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.w, size.h, 0, GL_DEPTH_COMPONENT, type, NULL);
	}
	~DepthBuffer()
	{
		if (texId)
		{
			glDeleteTextures(1, &texId);
			texId = 0;
		}
	}
};

struct TextureBuffer
{
	ovrHmd              hmd;
	ovrSwapTextureSet*  TextureSet;
	GLuint              texId;
	GLuint              fboId;
	OVR::Sizei               texSize;

	TextureBuffer(ovrHmd hmd, bool rendertarget, bool displayableOnHmd, OVR::Sizei size, int mipLevels, unsigned char * data, int sampleCount) :
		hmd(hmd),
		TextureSet(nullptr),
		texId(0),
		fboId(0),
		texSize(0, 0)
	{
		OVR_ASSERT(sampleCount <= 1); // The code doesn't currently handle MSAA textures.

		texSize = size;

		if (displayableOnHmd)
		{
			// This texture isn't necessarily going to be a rendertarget, but it usually is.
			OVR_ASSERT(hmd); // No HMD? A little odd.
			OVR_ASSERT(sampleCount == 1); // ovr_CreateSwapTextureSetD3D11 doesn't support MSAA.

			ovrResult result = ovr_CreateSwapTextureSetGL(hmd, GL_SRGB8_ALPHA8, size.w, size.h, &TextureSet);

			if (OVR_SUCCESS(result))
			{
				for (int i = 0; i < TextureSet->TextureCount; ++i)
				{
					ovrGLTexture* tex = (ovrGLTexture*)&TextureSet->Textures[i];
					glBindTexture(GL_TEXTURE_2D, tex->OGL.TexId);

					if (rendertarget)
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
					}
					else
					{
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
					}
				}
			}
		}
		else
		{
			glGenTextures(1, &texId);
			glBindTexture(GL_TEXTURE_2D, texId);

			if (rendertarget)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
			else
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			}

			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texSize.w, texSize.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}

		if (mipLevels > 1)
		{
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		glGenFramebuffers(1, &fboId);
	}

	~TextureBuffer()
	{
		if (TextureSet)
		{
			ovr_DestroySwapTextureSet(hmd, TextureSet);
			TextureSet = nullptr;
		}
		if (texId)
		{
			glDeleteTextures(1, &texId);
			texId = 0;
		}
		if (fboId)
		{
			glDeleteFramebuffers(1, &fboId);
			fboId = 0;
		}
	}

	OVR::Sizei GetSize() const
	{
		return texSize;
	}

	void SetAndClearRenderSurface(DepthBuffer* dbuffer)
	{
		auto tex = reinterpret_cast<ovrGLTexture*>(&TextureSet->Textures[TextureSet->CurrentIndex]);

		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex->OGL.TexId, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, dbuffer->texId, 0);

		glViewport(0, 0, texSize.w, texSize.h);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_FRAMEBUFFER_SRGB);
	}

	void UnsetRenderSurface()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	}
};

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
//! exports the current mesh to an obj file
void exportFileToObj(DynamicMesh* item);
// --------------------------------------
// --- Variable Declerations ------------
const bool L_MULTISAMPLING = false;



static const unsigned int required_tracking_caps = 0;

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
	ovr_Initialize(nullptr);

	// 1.1 - oculus variables \________________________________________________________________________________________________________________
	TextureBuffer * eyeRenderTexture[2] = { nullptr, nullptr };
	DepthBuffer   * eyeDepthBuffer[2] = { nullptr, nullptr };
	ovrGLTexture  * mirrorTexture = nullptr;
	GLuint          mirrorFBO = 0;

	OVR::Matrix4f oProjectionMatrix[2];
	ovrVector3f ViewOffset[2];
	ovrPosef EyeRenderPose[2];

	ovrFrameTiming ftiming;
	ovrTrackingState hmdState;


	ovrHmd HMD;
	ovrGraphicsLuid luid;
	ovrResult result = ovr_Create(&HMD, &luid);
	if (!OVR_SUCCESS(result))
		std::cout << "Oculus fail";

	ovrHmdDesc hmdDesc = ovr_GetHmdDesc(HMD);

	// Setup Window and Graphics
	// Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution
	ovrSizei windowSize = { hmdDesc.Resolution.w, hmdDesc.Resolution.h };
	// INITIALIZE GL \__________________________________________________________________________________________________________________________
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	if (L_MULTISAMPLING) glfwWindowHint(GLFW_SAMPLES, 4);
	else glfwWindowHint(GLFW_SAMPLES, 0);


	// SETUP GLFW WINDOW AND CONTEXT /////////////////////////////////////////////////////////////
	// Create a window...
	GLFWwindow* l_Window;
	GLFWmonitor* l_Monitor = NULL;
	l_Window = glfwCreateWindow(windowSize.w, windowSize.h, "GLFW Oculus Rift Test", l_Monitor, NULL);

	// Check if window creation was succesfull...
	if (!l_Window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
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

	// Set up HMD tracking \______________________________________________________________________________________________________________________

	// Start the sensor which informs of the Rift's pose and motion
	result = ovr_ConfigureTracking(HMD, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0);
	if (!OVR_SUCCESS(result))
	{
		//if (retryCreate) goto Done;
		//VALIDATE(OVR_SUCCESS(result), "Failed to configure tracking.");
		std::cout << "error";
	}

	// Make eye render buffers
	for (int eye = 0; eye < 2; ++eye)
	{
		ovrSizei idealTextureSize = ovr_GetFovTextureSize(HMD, ovrEyeType(eye), hmdDesc.DefaultEyeFov[eye], 1);
		eyeRenderTexture[eye] = new TextureBuffer(HMD, true, true, idealTextureSize, 1, NULL, 1);
		eyeDepthBuffer[eye] = new DepthBuffer(eyeRenderTexture[eye]->GetSize(), 0);

		if (!eyeRenderTexture[eye]->TextureSet)
		{
			//if (retryCreate) goto Done;
			//VALIDATE(false, "Failed to create texture.");
			std::cout << "error";
		}
	}

	// Create mirror texture and an FBO used to copy mirror texture to back buffer
	result = ovr_CreateMirrorTextureGL(HMD, GL_SRGB8_ALPHA8, windowSize.w, windowSize.h, reinterpret_cast<ovrTexture**>(&mirrorTexture));
	if (!OVR_SUCCESS(result))
	{
		//if (retryCreate) goto Done;
		//VALIDATE(false, "Failed to create mirror texture.");
		std::cout << "errror";
	}

	// Configure the mirror read buffer
	glGenFramebuffers(1, &mirrorFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture->OGL.TexId, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	ovrEyeRenderDesc EyeRenderDesc[2];
	EyeRenderDesc[0] = ovr_GetRenderDesc(HMD, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
	EyeRenderDesc[1] = ovr_GetRenderDesc(HMD, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

	// Turn off vsync to let the compositor do its magic
	//wglSwapIntervalEXT(0);



	//WandView \____________________________________________________________________________

	// create and set up the FBO
	GLuint wandViewFBO;
	glGenFramebuffers(1, &wandViewFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, wandViewFBO);

	GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };

	GLuint wandShadowMap;
	glGenTextures(1, &wandShadowMap);
	glBindTexture(GL_TEXTURE_2D, wandShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//glDrawBuffer(GL_NONE); // No color buffer is drawn to.
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	*/
	//Assign the shadow map to texture channel 0 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, wandShadowMap);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, wandShadowMap, 0);

	GLuint pickingTexture;
	glGenTextures(1, &pickingTexture);
	glBindTexture(GL_TEXTURE_2D, pickingTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB_FLOAT32_ATI, 1024, 1024,
		0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		pickingTexture, 0);

	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, wandShadowMap, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


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
	float tempVec[3];
	float lastPos[3];
	float lastPos2[3];
	float wandDirection[3];
	float prevWandDirection[4]; prevWandDirection[3] = 1.0f;
	float prevMeshOrientation[16];
	float meshOrientation[16];
	float vVec[4];
	float vMat[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };

	float biasMatrix[16] = { 0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f };

	float transform[16];
	float toolRad = 0.01;
	float toolStr = 0.01;
	float LMVP[16];

	float unitMat[16] = { 0.0f }; unitMat[0] = 1; unitMat[5] = 1; unitMat[10] = 1; unitMat[15] = 1;

	float wandPos[3];
	float wandVelocity[3] = { 0 };

	float currTime = 0;
	float lastTime;
	float deltaTime;

	float pixel;

	float intersectionP[3];
	float intersectionN[3];

	float* tempVecPtr;

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
	Texture greyTex("../Assets/Textures/lightBlue.DDS");
	Texture groundTex("../Assets/Textures/bordPixlar.DDS");
	Texture titleTex("../Assets/Textures/Titel2by1.DDS");
	Texture resetTex("../Assets/Textures/reset.DDS");
	Texture saveTex("../Assets/Textures/save.DDS");
	Texture loadTex("../Assets/Textures/load.DDS");
	Texture wireFrameTex("../Assets/Textures/wireframe.DDS");
	Texture plusTex("../Assets/Textures/plus.DDS");
	Texture minusTex("../Assets/Textures/minus.DDS");
	Texture meshTex("../Assets/Textures/carrots.DDS");
	Texture midTest("../Assets/Textures/midTest.DDS");

	Texture menuIcons("../Assets/Textures/menuICONS.DDS");
	Texture menuInfo("../Assets/Textures/menyInfo.DDS");
	Texture menuStrings("../Assets/Textures/menuSTRINGS.DDS");
	Texture strengthTex("../Assets/Textures/Size.DDS");

	Texture menuStringsSwe("../Assets/Textures/menuStringsSwe.dds");
	Texture savedFeedbackTex("../Assets/Textures/sparad4by1.dds");
	Texture loadModeInfoTex("../Assets/Textures/loadMode2by1.dds");

	float boardPos[3] = { 0.0f, -0.26f, 0.0f };
	//float boardPos[3] = { 0.0f, -0.22f, 0.0f };
	Box board(boardPos[0], boardPos[1] - 1.08*0.5, boardPos[2] - 0.148, 1.4, 1.08, 0.70); TrackingRange trackingRange(boardPos[0], (boardPos[1] + (0.25f / 2.0f) - 0.002f), boardPos[2], 0.50, 0.25, 0.40);
	MenuItem ground(boardPos[0], boardPos[1] - 1.08, boardPos[2], 50, 50);

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

	const int NR_OF_MODELLING_BUTTONS = 5;
	/*!	[0] resetMesh

		[1] save
		[2] load
		[3] Wireframe
		[4] export to obj*/

	menuBox* modellingButton[NR_OF_MODELLING_BUTTONS];
	MenuItem* modellingButtonString[NR_OF_MODELLING_BUTTONS - 2];
	MenuItem* modellingButtonFrame;

	Texture* modellingButtonTex = &menuIcons;

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

	modellingButton[0] = new menuBox(boardPos[0] - 0.2f, boardPos[1] + 0.001f + 0.0125f, boardPos[2] + 0.06, 0.025f, 0.025f, 0.025f, 0, 1, 1, 1, 5, 5);
	modellingButton[1] = new menuBox(boardPos[0] - 0.2f, boardPos[1] + 0.001f + 0.0125f, boardPos[2] + -0.04, 0.025f, 0.025f, 0.025f, 3, 0, 1, 1, 5, 5);
	modellingButton[2] = new menuBox(boardPos[0] - 0.2f, boardPos[1] + 0.001f + 0.0125f, boardPos[2] + -0.08, 0.025f, 0.025f, 0.025f, 4, 0, 1, 1, 5, 5);
	modellingButton[4] = new menuBox(boardPos[0] - 0.2f, boardPos[1] + 0.001f + 0.0125f, boardPos[2] + -0.12, 0.025f, 0.025f, 0.025f, 0, 1, 1, 1, 5, 5);
	modellingButton[3] = new menuBox(boardPos[0] - 0.2f, boardPos[1] + 0.001f + 0.0125f, boardPos[2] + -0.16, 0.025f, 0.025f, 0.025f, 5, 0, 1, 1, 5, 5);

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
	MenuItem toolFrame(boardPos[0] + 0.2, boardPos[1] + 0.001f, boardPos[2] - 0.04, 0.04f, 0.12f, 0, 4, 1, 3);
	MenuItem toolString(boardPos[0] + 0.2f + 0.08f, boardPos[1] + 0.001f, boardPos[2] - 0.04, 0.12f, 0.04f, 1, 4, 3, 1);

	for (int i = 0; i < NR_OF_TOOLS; i++)
		tool[i] = menuBox(boardPos[0] + 0.2, boardPos[1] + 0.001f + 0.0125f, boardPos[2] - 0.04*(i), 0.025f, 0.025f, 0.025f, i, 0, 1, 1, 5, 5);

	menuBox toolSize(boardPos[0] + 0.2, boardPos[1] + 0.001f + 0.075f, boardPos[2] - 0.16, 0.02f, 0.15f, 0.02f, 3, 3, 1, 1, 5, 5);
	menuBox toolSizeFill(boardPos[0] + 0.2, boardPos[1] + 0.09 - 0.075, boardPos[2] - 0.16, 0.015f, 0.0f, 0.015f, 5, 2, 1, 1, 5, 2); toolSizeFill.setDim(0.0f, toolRad * 3, 0.0f);
	MenuItem sizeString(0.14f, -0.075f, 0.0f, 0.24f, 0.04f, 0, 2, 8, 1);


	menuBox toolStrength(boardPos[0] + 0.16f, boardPos[1] + 0.001f + 0.075f, boardPos[2] - 0.16, 0.02f, 0.15f, 0.02f, 3, 3, 1, 1, 5, 5);
	menuBox toolStrengthFill(boardPos[0] + 0.16f, boardPos[1] + 0.001f + 0.075f, boardPos[2] - 0.16, 0.015f, 0.0f, 0.015f, 5, 2, 1, 1, 5, 2); toolStrengthFill.setDim(0.0f, toolStr, 0.0f);
	MenuItem strengthString(0.095f, -0.075f, 0.035f, 0.24f, 0.04f, 1, 5, 9, 1);

	MenuItem trackingInfo(boardPos[0], boardPos[1] + 0.001f, boardPos[2] - 0.22f, 0.24f, 0.08f, 1, 6, 9, 2);

	/*! tool 0 = push/pull
	1 =
	*/
	int activeTool = 0;
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
	Shader projectionShader;
	projectionShader.createShader("projectionV.glsl", "projectionF.glsl");


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
	GLint locationMeshM = glGetUniformLocation(meshShader.programID, "modelMatrix");
	GLint locationMeshWP = glGetUniformLocation(meshShader.programID, "wandPos");
	GLint locationMeshWD = glGetUniformLocation(meshShader.programID, "wandDirr");
	GLint locationMeshTex = glGetUniformLocation(meshShader.programID, "tex"); //texture sampler
	GLint locationMeshDTex = glGetUniformLocation(meshShader.programID, "dTex"); //texture sampler
	GLint locationMeshLMVP = glGetUniformLocation(meshShader.programID, "LMVP");
	GLint locationMeshPP = glGetUniformLocation(meshShader.programID, "PP");
	GLint locationMeshIntersectionP = glGetUniformLocation(meshShader.programID, "IntersectionP");
	GLint locationMeshIntersectionN = glGetUniformLocation(meshShader.programID, "IntersectionN");
	GLint locationMeshRad = glGetUniformLocation(meshShader.programID, "Radius");


	GLint locationFlatMV = glGetUniformLocation(flatShader.programID, "MV"); //modelview matrix
	GLint locationFlatP = glGetUniformLocation(flatShader.programID, "P"); //perspective matrix
	GLint locationFlatLP = glGetUniformLocation(flatShader.programID, "lightPos");
	GLint locationFlatLP2 = glGetUniformLocation(flatShader.programID, "lightPos2");
	GLint locationFlathM = glGetUniformLocation(flatShader.programID, "modelMatrix");
	GLint locationFlathWP = glGetUniformLocation(flatShader.programID, "wandPos");
	GLint locationFlatWD = glGetUniformLocation(flatShader.programID, "wandDirr");
	GLint locationFlatTex = glGetUniformLocation(flatShader.programID, "tex"); //texture sampler

	GLint locationProjMV = glGetUniformLocation(projectionShader.programID, "MV"); //modelview matrix
	GLint locationProjP = glGetUniformLocation(projectionShader.programID, "P"); //perspective matrix
	GLint locationProjTex = glGetUniformLocation(projectionShader.programID, "tex"); //texcoords

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
	ScalarField sTest(128, 128, 128, 0.1, 0.1, 0.1);
	Octree* oTest = new Octree(1.0f);
	//DynamicMesh* modellingMesh = new DynamicMesh("2015-07-22_16-08-10.bin");
	DynamicMesh* modellingMesh = new DynamicMesh();
	tempVec[0] = boardPos[0]; tempVec[1] = boardPos[1] + 0.07f; tempVec[2] = boardPos[2];
	modellingMesh->setPosition(tempVec);
	//modellingMesh->load("2015-07-22_16-08-10.bin"); modellingMesh->createBuffers();
	//modellingMesh->sphereSubdivide(0.05f); modellingMesh->createBuffers();
	
	clock_t begin = clock(); //DEBUG TEST
	//modellingMesh->generateMC(oTest->root);
	clock_t end = clock(); //DEBUG TEST
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC; //DEBUG TEST
	modellingMesh->createBuffers();
	
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

	VertexTool* currentTool;
	currentTool = new Push(modellingMesh, wand);
	ScalarTool* currentSTool;
	currentSTool = new Add;
	//currentTool = new Drag(modellingMesh, wand);

	//=======================================================================================================================================
	//Render loop
	//=======================================================================================================================================
	oProjectionMatrix[0] = ovrMatrix4f_Projection(hmdDesc.DefaultEyeFov[0], 0.2f, 1000.0f, ovrProjection_RightHanded);
	oProjectionMatrix[1] = ovrMatrix4f_Projection(hmdDesc.DefaultEyeFov[1], 0.2f, 1000.0f, ovrProjection_RightHanded);
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
					//currentTool->moveVertices(modellingMesh, wand, deltaTime);
					currentSTool->changeScalarData(modellingMesh, wand, oTest);
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
				//currentTool->firstSelect(modellingMesh, wand);
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
				
				//3.2.4 - export to obj file button >---------------------------------------------------------------------------------------------
				case 4: {
					if (modellingButtonState[activeButton] == 1) {
						if (th2Status == 0) {
							th2Status = 1;
							th2 = std::thread(exportFileToObj, modellingMesh);
						}
						modellingButton[activeButton]->setState(true);
					}
					else if (modellingButtonState[activeButton] == 3) {
						modellingButton[activeButton]->setState(false);
					}
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
					currentTool->setStrength((wandPos[1] - tempVecPtr[1])); toolStr = (wandPos[1] - tempVecPtr[1]);
				}
			}

			if (reset) {
				if (th2.joinable()) {
					th2.join();
					th2Status = 0;
					modellingMesh->cleanBuffer();
					modellingMesh->updateOGLData();
					tempVec[0] = boardPos[0]; tempVec[1] = boardPos[1] + 0.07f; tempVec[2] = boardPos[2];
					modellingMesh->setPosition(tempVec);
					reset = false;
				}
			}

			//wandViewMAP -------------------------------------------
			glViewport(0, 0, 1024, 1024);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, wandViewFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glUseProgram(projectionShader.programID);

			//glm::mat4 projP = glm::perspective(30.0f, 1.0f, 0.01f, 10.0f);
			glm::mat4 projP = glm::ortho(-0.1f, 0.1f, -0.1f, 0.1f, 0.0f, 1.0f);
			glm::transpose(projP);
			glUniformMatrix4fv(locationProjP, 1, GL_FALSE, &projP[0][0]);
			//glUniformMatrix4fv(locationP, 1, GL_FALSE, &(g_ProjectionMatrix[0].Transposed().M[0][0]));
			MVstack.push();

			//pmat4 = wand->getOrientation();
			//linAlg::invertMatrix(pmat4, unitMat);
			//linAlg::transpose(unitMat);
			//MVstack.multiply(pmat4);

			//wand->getPosition(wandPos);
			glm::vec3 gWandPos = glm::vec3(wandPos[0], wandPos[1], wandPos[2]);
			wand->getDirection(tempVec);
			glm::vec3 gWandDirr = glm::vec3(tempVec[0], tempVec[1], tempVec[2]);
			//tempVec[0] = -wandPos[0]; tempVec[1] = -wandPos[1]; tempVec[2] = -wandPos[2];
			//MVstack.translate(tempVec);

			glm::normalize(gWandDirr);

			glm::mat4 camTrans = glm::lookAt(gWandPos, gWandPos + gWandDirr, glm::vec3(0.0f, 0.0f, 1.0f));
			//glm::mat4 camTrans = glm::lookAt(glm::vec3(modellingMesh->getPosition()[0], modellingMesh->getPosition()[1], modellingMesh->getPosition()[2] + 0.1), glm::vec3(modellingMesh->getPosition()[0], modellingMesh->getPosition()[1], modellingMesh->getPosition()[2]), glm::vec3(0.0f, 1.0f, 0.0f));
			MVstack.multiply(&camTrans[0][0]);

			MVstack.push();
			MVstack.translate(modellingMesh->getPosition());
			MVstack.multiply(modellingMesh->getOrientation());
			//MVstack.multiply(&projP[0][0]);
			linAlg::matrixMult(&projP[0][0], MVstack.getCurrentMatrix(), LMVP);
			glUniformMatrix4fv(locationProjMV, 1, GL_FALSE, LMVP);
			linAlg::matrixMult(biasMatrix, LMVP, LMVP);

			modellingMesh->render();
			MVstack.pop();
			MVstack.pop();

			//glBindFramebuffer(GL_FRAMEBUFFER, 0);

			//glBindFramebuffer(GL_READ_FRAMEBUFFER, wandViewFBO);
			//glReadBuffer(GL_COLOR_ATTACHMENT0);

			//glReadPixels(512, 512, 1, 1, GL_RGB, GL_FLOAT, &pixel);

			//std::cout << pixel << std::endl;
			glReadBuffer(GL_NONE);
			//glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
			//currentTool->findIntersection(modellingMesh, wand, Pixel);
			currentTool->getIntersection(intersectionP, intersectionN);
			//linAlg::normVec(intersectionN);

			glm::vec3 interP = glm::vec3(intersectionP[0], intersectionP[1], intersectionP[2]);
			wand->getDirection(tempVec);
			glm::vec3 interN = glm::vec3(intersectionN[0], intersectionN[1], intersectionN[2]);
			//tempVec[0] = -wandPos[0]; tempVec[1] = -wandPos[1]; tempVec[2] = -wandPos[2];
			//MVstack.translate(tempVec);


			glm::mat4 interTrans = glm::lookAt(gWandPos, gWandPos + gWandDirr, glm::vec3(0.0f, 0.0f, 1.0f));
			//glm::mat4 camTrans = glm::lookAt(glm::vec3(modellingMesh->getPosition()[0], modellingMesh->getPosition()[1], modellingMesh->getPosition()[2] + 0.1), glm::vec3(modellingMesh->getPosition()[0], modellingMesh->getPosition()[1], modellingMesh->getPosition()[2]), glm::vec3(0.0f, 1.0f, 0.0f));

			MVstack.push();
			MVstack.multiply(&interTrans[0][0]);
			MVstack.translate(modellingMesh->getPosition());
			MVstack.multiply(modellingMesh->getOrientation());
			//MVstack.multiply(&projP[0][0]);
			linAlg::matrixMult(&projP[0][0], MVstack.getCurrentMatrix(), LMVP);
			linAlg::matrixMult(biasMatrix, LMVP, LMVP);
			MVstack.pop();


			// get hmd eye poses \__________________________________________________________________________________________________________________
			// Get both eye poses simultaneously, with IPD offset already included.
			// Get eye poses, feeding in correct IPD offset
			ViewOffset[0] = EyeRenderDesc[0].HmdToEyeViewOffset;
			ViewOffset[1] = EyeRenderDesc[1].HmdToEyeViewOffset;

			ftiming = ovr_GetFrameTiming(HMD, 0);
			hmdState = ovr_GetTrackingState(HMD, ftiming.DisplayMidpointSeconds);
			ovr_CalcEyePoses(hmdState.HeadPose.ThePose, ViewOffset, EyeRenderPose);


			GLRenderCallsOculus();

			for (int l_EyeIndex = 0; l_EyeIndex < ovrEye_Count; l_EyeIndex++) {
				// Increment to use next texture, just before writing

				eyeRenderTexture[l_EyeIndex]->TextureSet->CurrentIndex = (eyeRenderTexture[l_EyeIndex]->TextureSet->CurrentIndex + 1) % eyeRenderTexture[l_EyeIndex]->TextureSet->TextureCount;

				// Switch to eye render target
				eyeRenderTexture[l_EyeIndex]->SetAndClearRenderSurface(eyeDepthBuffer[l_EyeIndex]);



				// 3.3 OCULUS/CAMERA TRANSFORMS \______________________________________________________________________________________________
				MVstack.push();
				glUseProgram(sceneShader.programID);
				// Pass projection matrix on to OpenGL...
				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
				//glUniformMatrix4fv(locationP, 1, GL_FALSE, &projP[0][0]);
				glUniform1i(locationTex, 0);

				// Multiply with orientation retrieved from sensor...
				OVR::Quatf l_Orientation = OVR::Quatf(EyeRenderPose[l_EyeIndex].Orientation);
				OVR::Matrix4f l_ModelViewMatrix = OVR::Matrix4f(l_Orientation.Inverted());
				MVstack.multiply(&(l_ModelViewMatrix.Transposed().M[0][0]));
				//MVstack.multiply(wand->getOrientation());

				//!-- Translation due to positional tracking (DK2) and IPD...
				float eyePoses[3] = { -EyeRenderPose[l_EyeIndex].Position.x, -EyeRenderPose[l_EyeIndex].Position.y, -EyeRenderPose[l_EyeIndex].Position.z };
				MVstack.translate(eyePoses);
				//wand->getPosition(tempVec);
				//tempVec[0] = -tempVec[0]; tempVec[1] = -tempVec[1]; tempVec[2] = -tempVec[2];
				//MVstack.translate(tempVec);
				//MVstack.multiply(&camTrans[0][0]);

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
				glBindTexture(GL_TEXTURE_2D, greyTex.getTextureID());
				board.render();
				MVstack.pop();

				// Render Ground >----------------------------------------------------------------------------------------------------------------
				glUniform4fv(locationLP, 1, LP);
				MVstack.push();
					MVstack.translate(ground.getPosition());
					glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
					glBindTexture(GL_TEXTURE_2D, whiteTex.getTextureID());
					ground.render();
				MVstack.pop();


				// 3.4.3 Render title >----------------------------------------------------------------------------------------------------

				glUseProgram(bloomShader.programID);
				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));

				MVstack.push();
				MVstack.translate(title.getPosition());
				MVstack.rotX(1.57079f);
				glBindTexture(GL_TEXTURE_2D, titleTex.getTextureID());
				glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
				title.render();
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

				/*
				//glUseProgram(menuShader.programID);
				//glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(g_ProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
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
						glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
					}
					else {
						glUseProgram(menuShader.programID);
						glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
					}

					MVstack.push();
					glBindTexture(GL_TEXTURE_2D, modellingButtonTex->getTextureID());

					MVstack.translate(modellingButton[i]->getPosition());
					glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
					modellingButton[i]->render();
					MVstack.pop();
				}

				// place modellingButton frames
				glUseProgram(menuShader.programID);
				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
				glBindTexture(GL_TEXTURE_2D, menuStringsSwe.getTextureID());

				for (int i = 0; i < NR_OF_MODELLING_BUTTONS; i++) {
					MVstack.push();
					MVstack.translate(modellingButton[i]->getPosition());
					MVstack.translate(modellingButtonFrame->getPosition());
					glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
					modellingButtonFrame->render();
					MVstack.pop();
				}

				//TODO fix the -2 the last buttons (flatshading) and export dont have strings
				for (int i = 0; i < NR_OF_MODELLING_BUTTONS - 2; i++) {
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
					glUniformMatrix4fv(locationFlatP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));

					MVstack.push();
					MVstack.translate(modellingMesh->getPosition());
					MVstack.multiply(modellingMesh->getOrientation());
					glUniformMatrix4fv(locationFlatMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
					glUniform4fv(locationFlatLP, 1, LP);
					glUniform4fv(locationFlatLP2, 1, lPosTemp);

					modellingMesh->render();

				}
				else {
					glUseProgram(meshShader.programID);

					glUniformMatrix4fv(locationMeshPP, 1, GL_FALSE, &projP[0][0]);
					glUniformMatrix4fv(locationMeshLMVP, 1, GL_FALSE, LMVP);
					glUniform3fv(locationMeshIntersectionP, 1, intersectionP);
					glUniform3fv(locationMeshIntersectionN, 1, intersectionN);
					glUniform1f(locationMeshRad, toolRad);
					wand->getPosition(tempVec);
					glUniform3fv(locationMeshWP, 1, tempVec);

					wand->getDirection(tempVec);
					linAlg::normVec(tempVec);
					glUniform3fv(locationMeshWD, 1, tempVec);

					tempVecPtr = modellingMesh->getPosition();
					vMat[12] = tempVecPtr[0]; vMat[13] = tempVecPtr[1]; vMat[14] = tempVecPtr[2];
					linAlg::matrixMult(vMat, modellingMesh->getOrientation(), transform);
					glUniformMatrix4fv(locationMeshM, 1, GL_FALSE, transform);

					glBindTexture(GL_TEXTURE_2D, meshTex.getTextureID());
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
					glUniform1i(locationMeshTex, 0);


					glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));

					glBindTexture(GL_TEXTURE_2D, wandShadowMap);
					glUniform1i(locationMeshDTex, 0);

					MVstack.push();
					MVstack.translate(modellingMesh->getPosition());
					MVstack.multiply(modellingMesh->getOrientation());
					glUniformMatrix4fv(locationMeshMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
					glUniform4fv(locationMeshLP, 1, LP);
					glUniform4fv(locationMeshLP2, 1, lPosTemp);

					modellingMesh->render();
				}

				MVstack.pop();

				glUseProgram(sceneShader.programID);
				glBindTexture(GL_TEXTURE_2D, whiteTex.getTextureID());
				//	currentTool->renderIntersection(MVptr, locationMeshMV);

				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
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

				glUseProgram(bloomShader.programID);

				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));

				// render tool select GUI
				for (int i = 0; i < NR_OF_TOOLS; i++) {

					if (tool[i].getState()) {
						glUseProgram(bloomShader.programID);

						glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
					}
					else {
						glUseProgram(menuShader.programID);
						glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
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
				eyeRenderTexture[l_EyeIndex]->UnsetRenderSurface();
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
					}
					else {
						failedToStartLoading = true;
					}
				}
				else {
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
					}
					else {
						failedToStartLoading = true;
					}
				}
				else {
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
				}
				else {
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
				tempVec[0] = boardPos[0]; tempVec[1] = boardPos[1] + 0.07f; tempVec[2] = boardPos[2];
				modellingMesh->setPosition(tempVec);

				if (th1Status != 0) {
					th1.join();
					th1Status = 0;
				}
				continue;
			}




			ViewOffset[0] = EyeRenderDesc[0].HmdToEyeViewOffset;
			ViewOffset[1] = EyeRenderDesc[1].HmdToEyeViewOffset;

			ftiming = ovr_GetFrameTiming(HMD, 0);
			hmdState = ovr_GetTrackingState(HMD, ftiming.DisplayMidpointSeconds);
			ovr_CalcEyePoses(hmdState.HeadPose.ThePose, ViewOffset, EyeRenderPose);

			//glModes
			GLRenderCallsOculus();

			for (int l_EyeIndex = 0; l_EyeIndex<ovrEye_Count; l_EyeIndex++) {

				eyeRenderTexture[l_EyeIndex]->TextureSet->CurrentIndex = (eyeRenderTexture[l_EyeIndex]->TextureSet->CurrentIndex + 1) % eyeRenderTexture[l_EyeIndex]->TextureSet->TextureCount;

				// Switch to eye render target
				eyeRenderTexture[l_EyeIndex]->SetAndClearRenderSurface(eyeDepthBuffer[l_EyeIndex]);


				// 4.3 OCULUS/CAMERA TRANSFORMS \______________________________________________________________________________________________
				MVstack.push();

				glUseProgram(sceneShader.programID);
				// Pass projection matrix on to OpenGL...
				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
				glUniform1i(locationTex, 0);

				// Multiply with orientation retrieved from sensor...
				OVR::Quatf l_Orientation = OVR::Quatf(EyeRenderPose[l_EyeIndex].Orientation);
				OVR::Matrix4f l_ModelViewMatrix = OVR::Matrix4f(l_Orientation.Inverted());
				MVstack.multiply(&(l_ModelViewMatrix.Transposed().M[0][0]));

				//!-- Translation due to positional tracking (DK2) and IPD...
				float eyePoses[3] = { -EyeRenderPose[l_EyeIndex].Position.x, -EyeRenderPose[l_EyeIndex].Position.y, -EyeRenderPose[l_EyeIndex].Position.z };
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
				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));

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
						glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
					}
					else {
						glUseProgram(menuShader.programID);
						glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
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
				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
				MVstack.push();
				glBindTexture(GL_TEXTURE_2D, loadModeInfoTex.getTextureID());
				MVstack.translate(loadModeInfo.getPosition());
				MVstack.rotX(1.57079f);
				glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
				loadModeInfo.render();
				MVstack.pop();

				// 4.4.5 - RENDER meshes >--------------------------------------------------------------------------------------------------
				glUseProgram(meshShader.programID);
				glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));

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
				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));

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
				eyeRenderTexture[l_EyeIndex]->UnsetRenderSurface();
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


			// get hmd eye poses \__________________________________________________________________________________________________________________
			// Get both eye poses simultaneously, with IPD offset already included.
			ViewOffset[0] = EyeRenderDesc[0].HmdToEyeViewOffset;
			ViewOffset[1] = EyeRenderDesc[1].HmdToEyeViewOffset;

			ftiming = ovr_GetFrameTiming(HMD, 0);
			hmdState = ovr_GetTrackingState(HMD, ftiming.DisplayMidpointSeconds);
			ovr_CalcEyePoses(hmdState.HeadPose.ThePose, ViewOffset, EyeRenderPose);


			GLRenderCallsOculus();

			for (int l_EyeIndex = 0; l_EyeIndex < ovrEye_Count; l_EyeIndex++) {
				// Increment to use next texture, just before writing

				eyeRenderTexture[l_EyeIndex]->TextureSet->CurrentIndex = (eyeRenderTexture[l_EyeIndex]->TextureSet->CurrentIndex + 1) % eyeRenderTexture[l_EyeIndex]->TextureSet->TextureCount;

				// Switch to eye render target
				eyeRenderTexture[l_EyeIndex]->SetAndClearRenderSurface(eyeDepthBuffer[l_EyeIndex]);



				glUseProgram(sceneShader.programID);
				// Pass projection matrix on to OpenGL...
				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
				glUniform1i(locationTex, 0);

				// Multiply with orientation retrieved from sensor...
				OVR::Quatf l_Orientation = OVR::Quatf(EyeRenderPose[l_EyeIndex].Orientation);
				OVR::Matrix4f l_ModelViewMatrix = OVR::Matrix4f(l_Orientation.Inverted());
				MVstack.multiply(&(l_ModelViewMatrix.Transposed().M[0][0]));

				//!-- Translation due to positional tracking (DK2) and IPD...
				float eyePoses[3] = { -EyeRenderPose[l_EyeIndex].Position.x, -EyeRenderPose[l_EyeIndex].Position.y, -EyeRenderPose[l_EyeIndex].Position.z };
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
				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));

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
						glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
					}
					else {
						glUseProgram(menuShader.programID);
						glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
					}

					MVstack.push();
					glBindTexture(GL_TEXTURE_2D, modellingButtonTex->getTextureID());

					MVstack.translate(modellingButton[i]->getPosition());
					glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
					modellingButton[i]->render();
					MVstack.pop();
				}

				if (th2Status == 0) {
					glUseProgram(menuShader.programID);
					glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
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
				glUniformMatrix4fv(locationMeshP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));


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
				glUniformMatrix4fv(locationP, 1, GL_FALSE, &(oProjectionMatrix[l_EyeIndex].Transposed().M[0][0]));
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
				eyeRenderTexture[l_EyeIndex]->UnsetRenderSurface();
			}
			break;
		}
		}
		// Back to the default framebuffer...
		//eyeRenderTexture[l_EyeIndex]->UnsetRenderSurface();

		// Do distortion rendering, Present and flush/sync

		// Set up positional data.
		ovrViewScaleDesc viewScaleDesc;
		viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
		viewScaleDesc.HmdToEyeViewOffset[0] = ViewOffset[0];
		viewScaleDesc.HmdToEyeViewOffset[1] = ViewOffset[1];

		ovrLayerEyeFov ld;
		ld.Header.Type = ovrLayerType_EyeFov;
		ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

		for (int eye = 0; eye < 2; ++eye)
		{
			ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureSet;
			ld.Viewport[eye] = OVR::Recti(eyeRenderTexture[eye]->GetSize());
			ld.Fov[eye] = hmdDesc.DefaultEyeFov[eye];
			ld.RenderPose[eye] = EyeRenderPose[eye];
		}

		ovrLayerHeader* layers = &ld.Header;
		ovrResult result = ovr_SubmitFrame(HMD, 0, &viewScaleDesc, &layers, 1);
		// exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
		if (!OVR_SUCCESS(result))
			std::cout << "error";

		//isVisible = (result == ovrSuccess);

		// Blit mirror texture to back buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		GLint w = mirrorTexture->OGL.Header.TextureSize.w;
		GLint h = mirrorTexture->OGL.Header.TextureSize.h;
		glBlitFramebuffer(0, h, w, 0,
			0, 0, w, h,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		glfwSwapBuffers(l_Window);

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
	//glDeleteRenderbuffers(1, &l_DepthBufferId);
	//glDeleteTextures(1, &l_TextureId);
	//glDeleteFramebuffers(1, &l_FBOId);

	// Clean up window...
	glfwDestroyWindow(l_Window);
	glfwTerminate();

	// Clean up Oculus...
	ovr_Destroy(HMD);
	ovr_Shutdown();

	return 1;
}

static void WindowSizeCallback(GLFWwindow* p_Window, int p_Width, int p_Height) {
	/*if (p_Width>0 && p_Height>0) {
	g_Cfg.OGL.Header.BackBufferSize.w = p_Width;
	g_Cfg.OGL.Header.BackBufferSize.h = p_Height;

	ovrBool l_ConfigureResult = ovrHmd_ConfigureRendering(hmd, &g_Cfg.Config, G_DISTORTIONCAPS, hmd->MaxEyeFov, g_EyeRenderDesc);
	if (!l_ConfigureResult) {
	printf("Configure failed.\n");
	exit(EXIT_FAILURE);
	}
	}*/
}

void GLRenderCallsOculus(){
	// Clear...
	//GL calls
	glClearColor(0.01f, 0.01f, 0.01f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_FRAMEBUFFER_SRGB);
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
		if (wandPosition[0] < menuItem[i]->getPosition()[0] + menuItem[i]->getDim()[0] / 2.0f
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

void exportFileToObj(DynamicMesh* item) {
	meshLock.lock();
	item->exportToObj();
	meshLock.unlock();
	th2Status = 2;
}