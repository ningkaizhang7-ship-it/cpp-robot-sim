//=========================================================================
/**
*  @file     robot.cpp
*
*  项目描述： OpenGL变换
*  文件描述:  具体实例类 
*  适用平台： Windows98/2000/NT/XP
*  
*  作者：     WWBOSS
*  电子邮件:  wwboss123@gmail.com
*  创建日期： 2006-09-13	
*  修改日期： 2006-10-10
*
*  在这个类中您必须重载如下几个虚函数
*																								
*	virtual bool Init();														
*		执行所有的初始化工作，如果成功函数返回true							
*																			
*	virtual void Uninit();													
*		执行所有的卸载工作										
*																			
*	virtual void Update(DWORD milliseconds);										
*		执行所有的更新操作，传入的参数为两次操作经过的时间，以毫秒为单位
*																			
*	virtual void Draw();															
*		执行所有的绘制操作
*/
//=========================================================================

#include "Robot.h"											    /**< 包含头文件 */

//#include <gl\gl.h>												/**< 包含OpenGL头文件 */
//#include <gl\glu.h>												
//#include <gl\glaux.h>											
#include <gl.h>												/**< 包含OpenGL头文件 */
#include <glu.h>												
#include <glaux.h>	

#pragma comment(lib, "opengl32.lib")							/**< 包含OpenGL链接库文件 */
#pragma comment(lib, "glu32.lib")							
#pragma comment(lib, "glaux.lib")							

/** 定义一个默认的光源 */
static float diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };	 
static float specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };	 
static float lightPosition[] = { 0.0f, 0.0f, 10.0f, 1.0f };

/** 创建一个程序的实例 */
GLApplication * GLApplication::Create(const char * class_name)
{
	Robot * example = new Robot(class_name);
	return reinterpret_cast<GLApplication *>(example);
}


/** 构造函数 */
Robot::Robot(const char * class_name) : GLApplication(class_name)
{
  /// 初始化用户自定义的程序变量
	angle = 0.0f;                    /**< 设置初始角度为0 */
	legAngle[0] = legAngle[1] = 0.0f;				
	armAngle[0] = armAngle[1] = 0.0f;
}
/** 载入纹理数据 */
bool Robot::LoadTexture()
{
	/** 载入位图文件 */
	if(!texture1.Load("image.bmp"))                          /**< 载入位图文件 */
	{
		MessageBox(NULL,"装载位图文件失败！","错误",MB_OK);  /**< 如果载入失败则弹出对话框 */
		return false;
	}

	/** 载入TGA文件 */
	if(!texture2.Load("sphere.tga"))                         /**< 载入TGA文件 */
	{
		MessageBox(NULL,"装载TGA文件失败！","错误",MB_OK);  /**< 如果载入失败则弹出对话框 */
		return false;
	}
	
	/** 启用纹理映射 */
	glEnable(GL_TEXTURE_2D);


  	return true;
}
/** 设置光源 */
void Robot::SetLight()
{
    /** 定义光源的属性值 */
	//GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f }; 	/**< 环境光参数 */
	GLfloat LightAmbient[]= { 0.2f, 0.2f, 0.2f, 1.0f }; 	/**< 环境光参数 */
	//GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };		/**< 漫射光参数 */
	GLfloat LightDiffuse[]= { 0.5f, 0.5f, 0.5f, 1.0f }; 		/**< 漫射光参数 */
	GLfloat LightSpecular[]= { 1.0f, 1.0f, 1.0f, 1.0f };	/**< 镜面光参数 */
	//GLfloat LightSpecular[]= { 0.5f, 0.5f, 0.5f, 1.0f };	/**< 镜面光参数 */
	GLfloat LightPosition[]= { 0.0f, 0.0f, 2.0f, 1.0f };	/**< 光源位置 */

	/** 设置光源的属性值 */
	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		/**< 设置环境光 */
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		/**< 设置漫射光 */
	glLightfv(GL_LIGHT1, GL_SPECULAR, LightSpecular);	/**< 设置漫射光 */
	glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);	/**< 设置光源位置 */
	
	/** 启用光源 */
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1); 
}


/** 初始化OpenGL */
bool Robot::Init()									
{
/** 用户自定义的初始化过程 */
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);						
	glClearDepth(1.0f);											
	glDepthFunc(GL_LEQUAL);										
	glEnable(GL_DEPTH_TEST);									
	glShadeModel(GL_SMOOTH);									
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			
	ResizeDraw(true);											/**< 改变OpenGL窗口大小，直接调用子类的函数 */

	//开启灯光
	glEnable(GL_LIGHTING);
	/** 设置0号光源 */
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
	glEnable(GL_LIGHT0);                                        /**< 启用0号灰色光源,让物体可见 */

	/** 载入纹理 */
	if(!LoadTexture())
		MessageBox(NULL,"载入纹理失败!","错误",MB_OK);
	
	/** 设置光源 */
	SetLight();

	return true;												/**< 成功返回 */
}

/** 用户自定义的卸载函数 */
void Robot::Uninit()									
{
/** 用户自定义的卸载过程 */
	texture1.FreeImage();              /** 释放纹理图像占用的内存 */
	glDeleteTextures(1, &texture1.ID); /**< 删除纹理对象 */

	texture2.FreeImage();              /** 释放纹理图像占用的内存 */
	glDeleteTextures(1, &texture2.ID); /**< 删除纹理对象 */
}

/** 程序更新函数 */
void Robot::Update(DWORD milliseconds)						
{
	if (m_Keys.IsPressed(VK_ESCAPE) == true)					/**< 按ESC退出 */
	{
		TerminateApplication();									
	}

	if (m_Keys.IsPressed(VK_F1) == true)						/**< 按F1切换窗口/全屏模式 */
	{
		ToggleFullscreen();										
	}

	if(m_Keys.IsPressed(0x5a) == false)       /**< 0x5a 字母Z//0x31'1'键没有被按下时 */
	{
		glEnable(GL_LIGHT1);                  /**< 启用0号光源 */
	}
	else
		glDisable(GL_LIGHT1);                 /**< 当被按下时，禁用该光源 */
	
	//角度更新
	if(m_Keys.IsPressed(0x58) == false)       /**< 0x58 字母X//具体按键信息可以查ASCII表*/
	{
		angle = angle + 0.10f;					
		if (angle >= 360.0f)					
			angle = 0.0f;
	}
     
}

/** 绘制立方体 */
/*void Robot::DrawCube(float xPos, float yPos, float zPos)
{
	glPushMatrix();//当前矩阵堆栈中的所有矩阵向下压一级
		glTranslatef(xPos, yPos, zPos);
		glBegin(GL_POLYGON);
		
		    //顶面 
		    glVertex3f(0.0f, 0.0f, 0.0f);	 
			glVertex3f(0.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, 0.0f, 0.0f);
			
            // 前面 
			glVertex3f(0.0f, 0.0f, 0.0f);	 
			glVertex3f(-1.0f, 0.0f, 0.0f);
			glVertex3f(-1.0f, -1.0f, 0.0f);
			glVertex3f(0.0f, -1.0f, 0.0f);
			
			// 右面 
			glVertex3f(0.0f, 0.0f, 0.0f);	
			glVertex3f(0.0f, -1.0f, 0.0f);
			glVertex3f(0.0f, -1.0f, -1.0f);
			glVertex3f(0.0f, 0.0f, -1.0f);
			
			// 左面
			glVertex3f(-1.0f, 0.0f, 0.0f);	
			glVertex3f(-1.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, 0.0f);
			
			// 底面
			glVertex3f(0.0f, 0.0f, 0.0f);	
			glVertex3f(0.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, 0.0f);
				
			
			// 后面
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(-1.0f, 0.0f, -1.0f);
			glVertex3f(-1.0f, -1.0f, -1.0f);
			glVertex3f(0.0f, -1.0f, -1.0f);
		glEnd();
	glPopMatrix();
}*/
void Robot::DrawCube(float xPos, float yPos, float zPos)
{
	/** 设置材质属性 */
	GLfloat mat_ambient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat mat_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

	glPushMatrix();
	glTranslatef(xPos, yPos, zPos);
	//glRotatef(rot,1.0f,1.0f,0.0f);
	
	/** 选择纹理 */
	//glBindTexture(GL_TEXTURE_2D, texture1.ID);
	
	/** 开始绘制四边形 */
	glBegin(GL_QUADS);												
		
	    /// 前侧面
		glNormal3f( 0.0f, 0.0f, 1.0f);								/**< 指定法线指向观察者 */
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);	 	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 0.0f, 0.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 0.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, -1.0f, 0.0f);
		
		/// 后侧面
		glNormal3f( 0.0f, 0.0f,-1.0f);								/**< 指定法线背向观察者 */
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 0.0f, -1.0f);	
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);	
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -1.0f, -1.0f);	
		
		/// 顶面
		glNormal3f( 0.0f, 1.0f, 0.0f);								/**< 指定法线向上 */
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);		
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, -1.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 0.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, 0.0f, 0.0f);
		
		/// 底面
		glNormal3f( 0.0f,-1.0f, 0.0f);								/**< 指定法线朝下 */
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);		
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
		
		/// 右侧面
		glNormal3f( 1.0f, 0.0f, 0.0f);								/**< 指定法线朝右 */
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.0f);	
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, -1.0f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, -1.0f);
		
		/// 左侧面
		glNormal3f(-1.0f, 0.0f, 0.0f);								/**< 指定法线朝左 */
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 0.0f, 0.0f);	
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, 0.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, 0.0f);
	glEnd();

	glPopMatrix();
}
/** 绘制一个手臂 */
void Robot::DrawArm(float xPos, float yPos, float zPos)
{
	glPushMatrix();
	    glColor3f(1.0f, 0.0f, 0.0f);    /**< 红色 */	
		glTranslatef(xPos, yPos, zPos);
		glScalef(1.0f, 4.0f, 1.0f);	    /**< 手臂是1x4x1的立方体 */
		DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}
 /**< 绘制嘴巴 */
void Robot::DrawMouth(float xPos, float yPos, float zPos)
{
	glPushMatrix();
	    glColor3f(0.0f, 1.0f, 0.0f);    /**< 红色 */	
		glTranslatef(xPos, yPos, zPos);
		glScalef(1.0f, 0.2f, 0.2f);	    /**< 手臂是1x4x1的立方体 */
		DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}
 /**< 绘制鼻子 */
void Robot::DrawNose(float xPos, float yPos, float zPos)
{
	glPushMatrix();
	    glColor3f(1.0f, 0.0f, 1.0f);    /**< 红色 */	
		glTranslatef(xPos, yPos, zPos);
		glScalef( 0.2f, 0.2f, 1.0f);	    /**< 是1x4x1的立方体 */
		DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}

/** 绘制头部 */
void Robot::DrawHead(float xPos, float yPos, float zPos)
{
	glPushMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);	/**< 白色 */
		glTranslatef(xPos, yPos, zPos);
		glScalef(2.0f, 2.0f, 2.0f);		/**<头部是 2x2x2长方体 */
		glBindTexture(GL_TEXTURE_2D, texture2.ID);
		DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}

/** 绘制机器人的躯干 */ 
void Robot::DrawTorso(float xPos, float yPos, float zPos)
{
	glPushMatrix();
		glColor3f(0.0f, 0.0f, 1.0f);	 /**< 蓝色 */
		glTranslatef(xPos, yPos, zPos);
		glScalef(3.0f, 5.0f, 2.0f);	     /**< 躯干是3x5x2的长方体 */
		glBindTexture(GL_TEXTURE_2D, texture1.ID);
		DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}

/** 绘制一条腿 */
void Robot::DrawLeg(float xPos, float yPos, float zPos)
{
	glPushMatrix();
		glColor3f(1.0f, 1.0f, 0.0f);	/**< 黄色 */
		glTranslatef(xPos, yPos, zPos);
		glScalef(1.0f, 5.0f, 1.0f);		/**< 腿是1x5x1长方体 */
		
		DrawCube(0.0f, 0.0f, 0.0f);
	glPopMatrix();
}

/** 在指定位置绘制机器人 */
void Robot::DrawRobot(float xPos, float yPos, float zPos)
{
	static bool leg1 = true;		/**< 机器人腿的状态，true向前，flase向后 */
	static bool leg2 = false;		
	
	static bool arm1 = true;
	static bool arm2 = false;

	glPushMatrix();	

		glTranslatef(xPos, yPos, zPos);	/**< 定位 */

	    ///绘制各个部分
		DrawHead(1.0f, 2.0f, 0.0f);     /**< 绘制头部 */	
		DrawMouth(0.5f, 0.5f, 0.2f);      /**< 绘制嘴巴 */
		DrawTorso(1.5f, 0.0f, 0.0f);   /**< 绘制躯干 */
		DrawNose(0.1f, 1.1f, 1.0f);      /**< 绘制嘴巴 */
		///绘制胳膊
		glPushMatrix();
			///如果胳膊正在向前运动，则递增角度，否则递减角度 
			if (arm1)
				armAngle[0] = armAngle[0] + 0.1f;
			else
				armAngle[0] = armAngle[0] - 0.1f;

			///如果胳膊达到其最大角度则改变其状态
			if (armAngle[0] >= 15.0f)
					arm1 = false;
			if (armAngle[0] <= -15.0f)
					arm1 = true;

			///平移并旋转后绘制胳膊
			glTranslatef(0.0f, -0.5f, 0.0f);
			glRotatef(armAngle[0], 1.0f, 0.0f, 0.0f);
			DrawArm(2.55f, 0.0f, -0.5f);
		glPopMatrix();

		///同上arm0
		glPushMatrix();
			
			if (arm2)
				armAngle[1] = armAngle[1] + 0.1f;
			else
				armAngle[1] = armAngle[1] - 0.1f;

			
			if (armAngle[1] >= 15.0f)
					arm2 = false;
			if (armAngle[1] <= -15.0f)
					arm2 = true;

			
			glTranslatef(0.0f, -0.5f, 0.0f);
			glRotatef(armAngle[1], 1.0f, 0.0f, 0.0f);
			DrawArm(-1.55f, 0.0f, -0.5f);
		glPopMatrix();

		///绘制腿部
		glPushMatrix();					

			///如果腿正在向前运动，则递增角度，否则递减角度 
			if (leg1)
				legAngle[0] = legAngle[0] + 0.1f;
			else
				legAngle[0] = legAngle[0] - 0.1f;

			///如果腿达到其最大角度则改变其状态
			if (legAngle[0] >= 15.0f)
					leg1 = false;
			if (legAngle[0] <= -15.0f)
					leg1 = true;

			///平移并旋转后绘制胳膊
			glTranslatef(0.0f, -0.5f, 0.0f);
			glRotatef(legAngle[0], 1.0f, 0.0f, 0.0f);
			glBindTexture(GL_TEXTURE_2D, texture2.ID);
			DrawLeg(-0.5f, -5.0f, -0.5f);

		glPopMatrix();

		///同上leg1
		glPushMatrix();

			if (leg2)
				legAngle[1] = legAngle[1] + 0.1f;
			else
				legAngle[1] = legAngle[1] - 0.1f;

			if (legAngle[1] >= 15.0f)
				leg2 = false;
			if (legAngle[1] <= -15.0f)
				leg2 = true;

			glTranslatef(0.0f, -0.5f, 0.0f);
			glRotatef(legAngle[1], 1.0f, 0.0f, 0.0f);
			glBindTexture(GL_TEXTURE_2D, texture1.ID);
			DrawLeg(1.5f, -5.0f, -0.5f);

		glPopMatrix();
	glPopMatrix();
}

/** 绘制函数 */
void Robot::Draw()											
{
/** 用户自定义的绘制过程 */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
	glLoadIdentity();											
	
	///进行模型变换并绘制机器人
	glPushMatrix();
	  glTranslatef(0.0f, 0.0f, -30.0f);	
	  glRotatef(angle, 0.0f, 1.0f, 0.0f);
	  DrawRobot(0.0f, 0.0f, 0.0f);
    glPopMatrix();

	
	glFlush();													/**< 强制执行所有的OpenGL命令 */
}
