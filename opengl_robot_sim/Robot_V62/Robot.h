//=========================================================================
/**
*  @file      Robot.h
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

#ifndef __ROBOT_H__
#define __ROBOT_H__

#include "stdafx.h"
#include "GLFrame.h"												/**< 包含基本的框架类 */
#include "CBMPLoader.h"
#include "TGALoader.h" 
#include "Font.h"
#include "Camera.h"

/** 从GL_Application派生出一个子类 */
class Robot : GLApplication								
{
public:
	bool	Init();										   /**< 初始化e */
	void	Uninit();									   /**< 卸载 */
	void	Update(DWORD milliseconds);					   /**< 更新 */
	void	Draw();										   /**< 绘制 */

	void    DrawRobot(float xPos, float yPos, float zPos); /**< 绘制机器人 */

	bool    LoadTexture();                  /**< 载入纹理 */
	void    SetLight();                     /**< 设置光源 */
	void    UpdateCamera();                 /**< 更新摄像机 */
	void    CaculateFrameRate();            /**< 计算帧速 */
	void    PrintText();                    /**< 输出文字信息 */

private:
	void    DrawHead(float xPos,float yPos,float zPos);    /**< 绘制头部 */
	void    DrawTorso(float xPos, float yPos, float zPos); /**< 绘制躯干 */
	void    DrawLeg(float xPos, float yPos, float zPos);   /**< 绘制腿 */
	void    DrawArm(float xPos, float yPos, float zPos);   /**< 绘制胳膊 */
    void    DrawMouth(float xPos, float yPos, float zPos);   /**< 绘制嘴巴 */
	void    DrawCube(float xPos, float yPos, float zPos);  /**< 绘制立方体 */
    void    DrawNose(float xPos, float yPos, float zPos);  /**< 绘制立方体 */
	void    DrawHat(float xPos, float yPos, float zPos);
	void    DrawHattop(float xPos, float yPos, float zPos);
	void    DrawGrid();                     /**< 绘制网格地面 */
private:
	friend class GLApplication;							  /**< 父类为它的一个友元类 */
	Robot(const char * class_name);						  /**< 构造函数 */

		
	float angle;            /**< 机器人绕视点旋转的角度 */
	float legAngle[2];		/**< 腿的当前旋转角度 */
    float armAngle[2];      /**< 胳膊的当前旋转角度 */

	/** 用户自定义的程序变量 */ 
	CBMPLoader texture1;                     /**< 位图载入类 */
	CTGALoader texture2;                     /**< TGA文件载入类 */
	
	GLFont     m_Font;                        /**< 字体类 */
	Camera     m_Camera;                      /**< 摄像机类 */     
  	float      m_Fps;                         /**< 帧速 */
};


#endif	// __ROBOT_H__