// FrameBufferTest.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

#include <iostream>

//	callback function
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//	input process
void inputProcess(GLFWwindow *window);
//	mouse process
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//	scroll process
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


//	加载纹理
unsigned int loadTexture(char const* path);
//	生成纹理挂载点
//GLuint generateAttachmentTexture(GLboolean depth, GLboolean stencil);


//	window settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// 摄像机信息
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
float fov = 45.0f;

// 计时器
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
	//	glfw init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE

	//	create a window with glfw
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Layered Rendering", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


	//	glad: load all OpenGL function pointers 
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};


	//	开启深度测试
	glEnable(GL_DEPTH_TEST);

	//	创建 着色器
	//	Shader shader("Shader/layeredRendering.vs", "Shader/layeredRendering.fs", "Shader/layeredRendering.gs");
	Shader shader("Shaders/framebuffers.vert", "Shaders/framebuffers.frag");

	unsigned int VBO, VAO;
	//unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(INDICES_NAME), INDICES_NAME, GL_STATIC_DRAW);

	//	位置属性
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	//	纹理坐标属性
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


	//	加载纹理
	unsigned int texturedemo = loadTexture("Resources/Textures/test.jpg");
	//	配置着色器
	shader.use();	//	激活着色器
	shader.setInt("texture1", 0);

	//	配置FBO			FBO = 颜色附件 + 渲染缓冲RBO（RBO = 深度附件 + 模板附件）
	//	声明帧缓冲对象
	unsigned int fbo;	//	帧缓冲对象
	glGenFramebuffers(1, &fbo);	//	生成帧缓冲对象
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);	//	令当前程序绑定此帧缓冲对象，即：告诉程序，本人当前对要对此fbo进行操作
	//	创建颜色附件（颜色挂载）
	unsigned int texture;		//	声明颜色挂载
	glGenTextures(1, &texture);		//	创建颜色挂载
	glBindTexture(GL_TEXTURE_2D, texture);	//	绑定颜色挂载，即：告诉程序，我们现在要对创建的颜色挂载进行操作，保证此操作不影响其他颜色挂载
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);	//	最后参数为 null， 代表先进行占位操作
	//	其他配置
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	//	将此颜色挂载 添加到 FBO中
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	//	创建RBO，包含深度附件 与 模板附件
	unsigned int rbo;		//	声明
	glGenRenderbuffers(1, &rbo);		//	创建
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);	//	绑定
	//	当一个renderbuffer（渲染缓存）被创建时，他没有任何数据存储区域，因此 glRenderbufferStorage 函数 指定储存在renderbuffer中 图像的 宽高，颜色格式 并 按照此规格为之分配储存空间，
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // 对深度和模板缓冲区使用单个renderbuffer对象
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); //	将其挂载到FBO上

	//	开始检查FBO的完整性
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMERBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	//	绑定此FBO，即：告诉程序 使用当前配置好的FBO


	//	渲染主循环
	while (!glfwWindowShouldClose(window))
	{
		//	每帧时间 逻辑
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//	输入事件的处理
		inputProcess(window);

		//	摄像机/视图 转换
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		shader.setMat4("view", view);
		//	将投影矩阵传递给着色器
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("projection", projection);
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f));
		float angle = -25.0f;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.3f, 0.0f));
		shader.setMat4("model", model);


		//	----------------------开始绘制场景---------------------------------
		//	切换到自定义的帧缓冲区
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	//	使用指定颜色 清除自定义缓冲区，随后进行绘制
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//	要清除颜色缓冲以及深度缓冲

		glBindFramebuffer(GL_FRAMEBUFFER, 0);	//	绑定此FBO，即：告诉程序 使用当前配置好的FBO
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);	//	使用指定颜色进行清除
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);//关闭深度测试，绘制矩形框不需要深度


		//	绑定数组
		glBindVertexArray(VAO);
		//	激活纹理单元
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texturedemo);
		//glDrawElements(GL_TRIANGLES, sizeof(INDICES_NAME) / sizeof(GLuint), GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glBindVertexArray(0);


		//	glfw: 交换缓冲区和轮询IO事件（按下/释放键，鼠标移动等）
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	//	可选：一旦资源耗尽其目的，则取消分配所有资源：
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//	glDeleteBuffers(1, &EBO);

		// glfw:  终止，清除以前分配的所有GLFW资源。
	glfwTerminate();
	return 0;
}

//	处理所有输入：查询GLFW是否按下/释放相关键并相应地做出反应
void inputProcess(GLFWwindow *window) {
	float cameraSpeed = 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

//	glfw: 窗口大小发生变化（通过OS或用户调整大小）时，执行此回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	//	确保视口与新窗口尺寸匹配，请注意宽度和高度将远远大于视网膜显示器上指定的高度。
	glViewport(0, 0, width, height);
}

//	mouse process
void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}

GLuint loadTexture(char const* path) {
	//	创建纹理并加载
	unsigned int textureID;
	glGenTextures(1, &textureID);

	//	加载图片，	创建纹理，并生成mipmap
	int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true);		//	告诉stb_image.h，在y轴上反转加载纹理。
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//	设置其他参数
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//	释放资源
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}
	return textureID;
}

/*
//	生成适合帧缓冲附件的纹理
GLuint generateAttachmentTexture(GLboolean depth, GLboolean stencil) {
	//	使用的枚举类型
	GLenum attachment_type;
	if (!depth && !stencil)
		attachment_type = GL_RGB;
	else if (depth && !stencil)
		attachment_type = GL_DEPTH_COMPONENT;
	else if (!depth && stencil)
		attachment_type = GL_STENCIL_INDEX;
	//	创建纹理 并添加纹理数据
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if (!depth && !stencil)
		glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, 800, 600, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
	else // 使用模板和深度测试，需要特殊的格式参数
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;

}
*/



// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
