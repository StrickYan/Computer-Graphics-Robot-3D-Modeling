// The sample model.  You should build a file
// very similar to this for when you make your model.
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>
#include "bitmap.h"
#include <gl\glaux.h>
#include <gl\glu.h> 
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <gl\gl.h> 
#include <gl\glu.h> 
#include <math.h>						// 引入数学函数库中的Sin
#include "modelerglobals.h"

#pragma comment(lib,"GLAUX.LIB")
#include <iostream>
#include <GL/glut.h>
#include <GL/glaux.h>
using namespace std;

#define PI 3.1415926
int hier=0;	//层次显示变量
int tt=30;	//动态效果全局变量
GLfloat		zrot;								// Z 旋转量		
float points[ 75 ][ 75 ][3];	// Points网格顶点数组
GLuint texture[6]; 

#define MAX_PARTICLES 1000	// 定义最大的粒子数
typedef struct   // 创建粒子数据结构
{
	bool active;// 是否激活
	float life;// 粒子生命
	float fade;// 衰减速度

	float x,y,z;//the position
	float xi,yi,zi;//what direction to move
	float xg,yg,zg;//  X ,y,z方向重力加速度
}particles;

float slowdown = 2.0f;	// 减速粒子
float xspeed,yspeed; // X,y方向的速度
float zoom = -30.0f;	// 沿Z轴缩放
GLuint loop;			// 循环变量
particles paticle[MAX_PARTICLES];

AUX_RGBImageRec *LoadBMP(char *Filename)    // 根据位图文件的名称进行加载
{
	FILE *File=NULL;        // 文件指针
	if (!Filename)         // 如果没有指定位图文件名称就返回NULL
	{
		return NULL;         
	}
	File=fopen(Filename,"r");       // 根据指定的位图文件名称，打开该位图文件
	if (File)           // 如果位图文件存在
	{
		fclose(File);         // 因为只是需要判断问题是否存在，而不需要对位图文件进行写操作，所以关闭位图文件
		return auxDIBImageLoad(Filename);   // 其实，只需要一个真正存在的位图文件的名称，实现加载位图文件，并返回一个指针
	}
	return NULL;          // 位图文件加载失败就返回NULL
}

int LoadGLTextures()         // 根据加载的位图创建纹理
{
	int Status=FALSE;         // 指示纹理创建是否成功的标志
	AUX_RGBImageRec *TextureImage[6];     // 创建一个纹理图像数组，这里指定数组大小为6
	memset(TextureImage,0,sizeof(void *)*6);          // 初始化纹理图像数组，为其分配内存
	char *pictures[] = {// 创建一个位图名称数组，对应6幅位图
		"Data/test8.bmp",
		"Data/test9.bmp",
		"Data/test5.bmp",
		"Data/test3.bmp",
		"Data/Star.bmp",
		"Data/Particle.bmp"
	};
	for(int i=0; i<6; i++)// 遍历位图名称数组，根据位图名称分别生成
	{
		if (TextureImage[i]=LoadBMP(pictures[i]))// 加载位图i成功，修改状态标志变量Status为TRUE
		{
			Status=TRUE;        

			glGenTextures(1, &texture[i]);     // 为第i个位图创建纹理
			glBindTexture(GL_TEXTURE_2D, texture[i]);// 将生成的纹理的名称绑定到指定的纹理上
			glTexImage2D(GL_TEXTURE_2D, 0, 3, TextureImage[i]->sizeX, TextureImage[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureImage[i]->data);		
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);   //还原贴图原色		
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		}

		if (TextureImage[i])         // 释放位图数组占用的内存空间
		{
			if (TextureImage[i]->data)       
			{
				free(TextureImage[i]->data);    
			}
			free(TextureImage[i]);        
		}
	}
	return Status;          // 创建纹理并加载，返回成功或者失败的标志Status
}



int InitGL(GLvoid)								// 此处开始对OpenGL进行所有设置
{
	if (!LoadGLTextures())							// 调用纹理载入子例程
	{
		return FALSE;							// 如果未能载入，返回FALSE
	}

	//glEnable(GL_TEXTURE_2D);						// 启用纹理映射
	glShadeModel(GL_SMOOTH);						// 启用阴影平滑
	//glClearColor(0.0f, 0.0f, 0.0f, 0.5f);					// 黑色背景
	glClearDepth(1.0f);							// 设置深度缓存
	//glEnable(GL_DEPTH_TEST);						// 启用深度测试
	//glDepthFunc(GL_LEQUAL);							// 所作深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// 真正精细的透视修正

	glPolygonMode( GL_BACK, GL_FILL );			// 后表面完全填充
	glPolygonMode( GL_FRONT, GL_LINE );			// 前表面使用线条绘制

	// 沿X平面循环
	for(int x=0; x<75; x++)
	{
		// 沿Y平面循环
		for(int y=0; y<75; y++)
		{
			// 向表面添加波浪效果
			points[x][y][0]=float((x/5.0f)-7.5f);
			points[x][y][1]=float((y/5.0f)-7.5f);
			//points[x][y][2]=float(sin((((x/5.0f)*40.0f)/360.0f)*3.141592654*2.0f));
			points[x][y][2]=float(sin((((x/5.0f)*40.0f)/360.0f)*3.141592654*8.0f));
		}
	}

	return TRUE;								// 初始化 OK

}


// To make a SampleModel, we inherit off of ModelerView
class SampleModel : public ModelerView 
{
public:
	SampleModel(int x, int y, int w, int h, char *label) 
		: ModelerView(x,y,w,h,label) { }

	virtual void draw();
};

// We need to make a creator function, mostly because of
// nasty API stuff that we'd rather stay away from.
ModelerView* createSampleModel(int x, int y, int w, int h, char *label)
{ 
	return new SampleModel(x,y,w,h,label); 
}

// We are going to override (is that the right word?) the draw()
// method of ModelerView to draw out SampleModel
void SampleModel::draw()
{
	// This call takes care of a lot of the nasty projection 
	// matrix stuff.  Unless you want to fudge directly with the 
	// projection matrix, don't bother with this ...
	ModelerView::draw();

	if(VAL(WALK_ON)==1)
		tt=(tt+1)%120;  //每次调用draw函数时tt增加相应的值

	InitGL();

	// draw the floor
	setAmbientColor(.1f,.1f,.1f);
	setDiffuseColor(COLOR_RED);
	glPushMatrix();
	glTranslated(-5,0,-5);
	//drawBox(10,0.01f,10);
	//drawTriangle(0,0,0,0,0,3,3,0,3);
	glPopMatrix();

	// draw the sample model
	setAmbientColor(.1f,.1f,.1f);
	setDiffuseColor(COLOR_YELLOW);
	glPushMatrix();
	glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));

	if(VAL(DEFAULT_CAMERA_POINT)==1)	//恢复相机默认位置
	{
		glTranslated(-VAL(XPOS), -VAL(YPOS), -VAL(ZPOS));
	}


	if((tt/30==0)||(tt/30==2))	//收步 身体上移
		glTranslated(0 , 0.0005*(tt%60) , 0);	
	else
		glTranslated(0 , -0.0005*(tt%60) , 0);	


	//躯干中央的长方体/圆柱
	setDiffuseColor((float)255/255,(float)255/255,(float)0/255);
	glPushMatrix();
	hier++;
	if(VAL(HIERARCHY)>=hier){
		//glTranslated(-0.5, 0, -0.7);
		//glScaled(1, 3, 1.4);
		//drawBox(1,1,1);
		glScaled(1.5, 1, 1.5);
		glRotated(-90, 1.0, 0.0, 0.0);
		drawCylinder(3, 0.5, 0.5);
	}
	hier--;
	glPopMatrix();

	//左腿
	setDiffuseColor((float)139/255,(float)139/255,(float)122/255);
	glPushMatrix();	
	hier++;
	if(VAL(HIERARCHY)>=hier){
		if(tt/60==0)
			glRotated(-30+tt%60, 1.0, 0.0, 0.0);
		else
			glRotated(30-tt%60, 1.0, 0.0, 0.0);

		glTranslated(-1.9 , 0.5 , -0.8);	

		glEnable(GL_TEXTURE_2D);	
		glBindTexture(GL_TEXTURE_2D, texture[0]);//   选择第一个纹理texture[0]，进行贴纹理
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, -1.2f, 1.61f); // Bottom Left Of The Texture and Quad
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.2f, -1.2f, 1.61f); // Bottom Right Of The Texture and Quad
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.2f, 0.0f, 1.6f); // Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.0f, 1.6f); // Top Left Of The Texture and Quad
		glEnd();

		glDisable(GL_TEXTURE_2D);	

		drawBox(1.2, -1.2 , 1.6);
	}

	setDiffuseColor((float)190/255,(float)190/255,(float)190/255);
	hier++;
	if(VAL(HIERARCHY)>=hier){
		glTranslated(0.4 , -1.0 , 0.6);	
		glRotated(-5, 0.0, 0.0, 1.0);
		drawBox(0.5, -1.2 , 0.5);
	}

	setDiffuseColor((float)255/255,(float)255/255,(float)0/255);
	hier++;
	if(VAL(HIERARCHY)>=hier){
		glTranslated(-0.5 , -1.0 , -0.5);	
		drawBox(1.6, -2.5 , 1.6);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		setDiffuseColor(0.7f,0.7f,0.7f);
		glTranslated(0 , -2.7 , 0);	
		glRotated(5, 0.0, 0.0, 1.0);
		drawBox(1.6, -0.2 , 1.6);
	}

	//三个圆柱圈
	hier++;
	if(VAL(HIERARCHY)>=hier){
		glRotated(-5, 0.0, 0.0, 1.0);
		glTranslated(0 , 2.7 , 0);	
		glTranslated(0.5 , 1.0 , 0.5);	
		glRotated(90, 1.0, 0.0, 0.0);
		glTranslated(0.3 , 0.3 , 1.3);	
		drawCylinder(0.6, 1.2, 1.2);
	}
	hier++;
	if(VAL(HIERARCHY)>=hier){
		glTranslated(0 , 0 , 0.7);	
		drawCylinder(0.6, 1.2, 1.2);
	}
	hier++;
	if(VAL(HIERARCHY)>=hier){
		glTranslated(0 , 0 , 0.7);	
		drawCylinder(0.6, 1.2, 1.2);
	}

	//左脚喷气装置   // 绘制粒子
	hier++;
	if(VAL(HIERARCHY)>=hier){
		//glPushMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		//really nice perspective calculations
		glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
		glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texture[5]);
		//glLoadIdentity();
		glScaled(VAL(JET_SYSTEM_ON),VAL(JET_SYSTEM_ON),VAL(JET_SYSTEM_ON));
		glRotated(5, 0.0, 0.0, 1.0);
		glRotated(-90, 1.0, 0.0, 0.0);
		glTranslated(0,-1.8,30);


		for (loop = 0;loop < MAX_PARTICLES;++loop)
		{
			if (paticle[loop].active)//the particle is alive
			{
				float x = paticle[loop].x;
				float y = paticle[loop].y;
				//our scene is moved into the screen
				float z = paticle[loop].z + zoom;

				//cout<<int(paticle[loop].x)<<int(paticle[loop].y)<<int(paticle[loop].z)<<endl;

				//draw particles : use triangle strip instead of quad to speed
				glBegin(GL_TRIANGLE_STRIP);   // 绘制三角形带
				//top right
				glTexCoord2d(1,1);
				glVertex3f(x + 0.5f,y + 0.5f,z);
				//top left
				glTexCoord2d(0,1);
				glVertex3f(x - 0.5f,y + 0.5f,z);
				//bottom right
				glTexCoord2d(1,0);
				glVertex3f(x + 0.5f,y - 0.5f,z);
				//bottom left
				glTexCoord2d(0,0);
				glVertex3f(x - 0.5f,y - 0.5f,z);
				glEnd();

				//Move On The X Axis By X Speed 
				paticle[loop].x+=paticle[loop].xi/(slowdown*1000);    // 更新X坐标的位置
				//Move On The Y Axis By Y Speed 
				paticle[loop].y+=paticle[loop].yi/(slowdown*1000); 
				//Move On The Z Axis By Z Speed 
				paticle[loop].z+=paticle[loop].zi/(slowdown*1000); 

				//add gravity or resistance : acceleration
				paticle[loop].xi += paticle[loop].xg;    // 更新X轴方向速度大小
				paticle[loop].yi += paticle[loop].yg;
				paticle[loop].zi += paticle[loop].zg;

				//decrease particles' life
				paticle[loop].life -= paticle[loop].fade;   // 减少粒子的生命值

				//if particle is dead,rejuvenate it
				if (paticle[loop].life < 0.0f)
				{
					paticle[loop].life = 1.0f;//alive again   // 产生一个新的粒子
					paticle[loop].fade = float(rand() % 100) / 1000.0f + 0.05f;

					//reset its position
					paticle[loop].x = 0.0f;  // 新粒子出现在坐标的中央
					paticle[loop].y = 0.0f;
					paticle[loop].z = 0.0f;

					//X Axis Speed And Direction 
					paticle[loop].xi=xspeed+float((rand()%60)-32.0f); 
					// Y Axis Speed And Direction 
					paticle[loop].yi=yspeed+float((rand()%60)-30.0f); 
					// Z Axis Speed And Direction 
					paticle[loop].zi=float((rand()%60)-30.0f); 
				}
			}
		}
		//glutSwapBuffers();
		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		//glPopMatrix();
	}
	hier=hier-8;
	glPopMatrix();


	//右腿
	glPushMatrix();	
	hier++;
	if(VAL(HIERARCHY)>=hier){
		if(tt/60==0)
			glRotated(30-tt%60, 1.0, 0.0, 0.0);
		else
			glRotated(-30+tt%60, 1.0, 0.0, 0.0);
		glTranslated(1.9 , 0.5 , -0.8);	
		setDiffuseColor((float)139/255,(float)139/255,(float)122/255);
		glEnable(GL_TEXTURE_2D);		
		glBindTexture(GL_TEXTURE_2D, texture[1]);//   选择第一个纹理texture[1]，进行贴纹理
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.2f, -1.2f, 1.61f); // Bottom Left Of The Texture and Quad
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.0f, -1.2f, 1.61f); // Bottom Right Of The Texture and Quad
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.0f, 0.0f, 1.6f); // Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.2f, 0.0f, 1.6f); // Top Left Of The Texture and Quad
		glEnd();	
		glDisable(GL_TEXTURE_2D);	
		drawBox(-1.2, -1.2 , 1.6);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		setDiffuseColor((float)190/255,(float)190/255,(float)190/255);
		glTranslated(-0.4 , -1.0 , 0.6);	
		glRotated(5, 0.0, 0.0, 1.0);
		drawBox(-0.5, -1.2 , 0.5);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		setDiffuseColor((float)139/255,(float)139/255,(float)122/255);
		glTranslated(0.5 , -1.0 , -0.5);	
		drawBox(-1.6, -2.5 , 1.6);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		setDiffuseColor(0.7f,0.7f,0.7f);
		glTranslated(0 , -2.7 , 0);	
		glRotated(-5, 0.0, 0.0, 1.0);
		drawBox(-1.6, -0.2 , 1.6);
	}

	//右脚喷气装置
	hier++;
	if(VAL(HIERARCHY)>=hier){
		//glPushMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		//really nice perspective calculations
		glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
		glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,texture[5]);
		//glLoadIdentity();
		glScaled(VAL(JET_SYSTEM_ON),VAL(JET_SYSTEM_ON),VAL(JET_SYSTEM_ON));
		glRotated(5, 0.0, 0.0, 1.0);
		//glRotated(-90, 1.0, 0.0, 0.0);
		glTranslated(-0.9,-0.8,30.8);

		for (loop = 0;loop < MAX_PARTICLES;++loop)
		{
			if (paticle[loop].active)//the particle is alive
			{
				float x = paticle[loop].x;
				float y = paticle[loop].y;
				//our scene is moved into the screen
				float z = paticle[loop].z + zoom;		
				//cout<<int(paticle[loop].x)<<int(paticle[loop].y)<<int(paticle[loop].z)<<endl;
				//draw particles : use triangle strip instead of quad to speed
				glBegin(GL_TRIANGLE_STRIP);
				//top right
				glTexCoord2d(1,1);
				glVertex3f(x + 0.5f,y + 0.5f,z);
				//top left
				glTexCoord2d(0,1);
				glVertex3f(x - 0.5f,y + 0.5f,z);
				//bottom right
				glTexCoord2d(1,0);
				glVertex3f(x + 0.5f,y - 0.5f,z);
				//bottom left
				glTexCoord2d(0,0);
				glVertex3f(x - 0.5f,y - 0.5f,z);
				glEnd();

				//Move On The X Axis By X Speed 
				paticle[loop].x+=paticle[loop].xi/(slowdown*1000); 
				//Move On The Y Axis By Y Speed 
				paticle[loop].y+=paticle[loop].yi/(slowdown*1000); 
				//Move On The Z Axis By Z Speed 
				paticle[loop].z+=paticle[loop].zi/(slowdown*1000); 

				//add gravity or resistance : acceleration
				paticle[loop].xi += paticle[loop].xg;
				paticle[loop].yi += paticle[loop].yg;
				paticle[loop].zi += paticle[loop].zg;

				//decrease particles' life
				paticle[loop].life -= paticle[loop].fade;

				//if particle is dead,rejuvenate it
				if (paticle[loop].life < 0.0f)
				{
					paticle[loop].life = 1.0f;//alive again
					paticle[loop].fade = float(rand() % 100) / 1000.0f + 0.05f;

					//reset its position
					paticle[loop].x = 0.0f;
					paticle[loop].y = 0.0f;
					paticle[loop].z = 0.0f;

					//X Axis Speed And Direction 
					paticle[loop].xi=xspeed+float((rand()%60)-32.0f); 
					// Y Axis Speed And Direction 
					paticle[loop].yi=yspeed+float((rand()%60)-30.0f); 
					// Z Axis Speed And Direction 
					paticle[loop].zi=float((rand()%60)-30.0f); 
				}
			}
		}
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		//glPopMatrix();
	}

	hier=hier-5;
	glPopMatrix();

	if(tt/60==0)
		glRotated(VAL(BODY_ROTATE)-30+tt%60, 0.0, 1.0, 0.0);	
	else
		glRotated(VAL(BODY_ROTATE)+30-tt%60, 0.0, 1.0, 0.0);	

	//头部： 脖子长方体 头部 正方体
	glPushMatrix();
	setDiffuseColor((float)255/255,(float)250/255,(float)250/255);
	hier++;
	if(VAL(HIERARCHY)>=hier){
		glRotated(VAL(HEAD_ROTATE), 0.0, 1.0, 0.0);
		glTranslated(-0.2 , 3 , -0.2);	
		//glScaled(0.4, 1, 0.4);
		//drawBox(1,1,1);
		drawBox(0.4,1,0.4);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		glTranslated(-0.2,0.6,-0.2);

		glEnable(GL_TEXTURE_2D);		
		glBindTexture(GL_TEXTURE_2D, texture[3]);//   选择第一个纹理texture[3]，进行贴纹理
		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 0.81f); // Bottom Left Of The Texture and Quad
		glTexCoord2f(1.0f, 0.0f); glVertex3f(0.8f, 0.0f, 0.81f); // Bottom Right Of The Texture and Quad
		glTexCoord2f(1.0f, 1.0f); glVertex3f(0.8f, 0.8f, 0.8f); // Top Right Of The Texture and Quad
		glTexCoord2f(0.0f, 1.0f); glVertex3f(0.0f, 0.8f, 0.8f); // Top Left Of The Texture and Quad
		glEnd();	
		glDisable(GL_TEXTURE_2D);	

		drawBox(0.8,0.8,0.8);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		//头盔
		int n=15;
		setDiffuseColor((float)255/255,(float)250/255,(float)250/255);
		glScaled(VAL(CAP),VAL(CAP),VAL(CAP));
		glTranslatef(0.4f,0.48f,0.4f);	
		glRotated(180,0.0,1.0,0.0);
		glScaled(0.7,0.5,0.7);
		for(int i=0;i<n;i++)	//帽子
		{
			drawTriangle(1.2*cos(2*PI/n/2*i),0,1.2*sin(2*PI/n/2*i),1.2*cos(2*PI/n/2*(i+1)),0,
				1.2*sin(2*PI/n/2*(i+1)),0.9*cos(2*PI/n/2*i),0.5,0.9*sin(2*PI/n/2*i));
			drawTriangle(1.2*cos(PI/n*(i+1)),0,1.2*sin(PI/n*(i+1)),0.9*cos(PI/n*i),
				0.5,0.9*sin(PI/n*i),0.9*cos(PI/n*(i+1)),0.5,0.9*sin(PI/n*(i+1)));
		}
		for(int i=0;i<n;i++)
		{
			drawTriangle(0.9*cos(2*PI/n*i),0.5,0.9*sin(2*PI/n*i),0.9*cos(2*PI/n*(i+1)),0.5,
				0.9*sin(2*PI/n*(i+1)),0.6*cos(2*PI/n*i),1,0.6*sin(2*PI/n*i));
			drawTriangle(0.9*cos(2*PI/n*(i+1)),0.5,0.9*sin(2*PI/n*(i+1)),0.6*cos(2*PI/n*i),
				1,0.6*sin(2*PI/n*i),0.6*cos(2*PI/n*(i+1)),1,0.6*sin(2*PI/n*(i+1)));
		}
		for(int i=0;i<n;i++)
		{
			drawTriangle(0.6*cos(2*PI/n*i),1,0.6*sin(2*PI/n*i),0.6*cos(2*PI/n*(i+1)),
				1,0.6*sin(2*PI/n*(i+1)),0.3*cos(2*PI/n*i),2,0.3*sin(2*PI/n*i));
			drawTriangle(0.6*cos(2*PI/n*(i+1)),1,0.6*sin(2*PI/n*(i+1)),0.3*cos(2*PI/n*i),
				2,0.3*sin(2*PI/n*i),0.3*cos(2*PI/n*(i+1)),2,0.3*sin(2*PI/n*(i+1)));
		}
		for(int i=0;i<n;i++)
		{
			drawTriangle(0.3*cos(2*PI/n*i),2,0.3*sin(2*PI/n*i),0.3*cos(2*PI/n*(i+1)),2,
				0.3*sin(2*PI/n*(i+1)),0,2.8,0);
		}

	}

	hier=hier-3;
	glPopMatrix();
	//左躯干的圆柱
	glPushMatrix();	

	//背后波浪形斗篷
	glPushMatrix();
	hier++;
	if(VAL(HIERARCHY)>=hier){
		int x, y;						// 循环变量		
		glScaled(VAL(Cloak),VAL(Cloak),VAL(Cloak));	

		if(VAL(WALK_ON)==1)
		{
			glTranslatef(0.0f,0.0f,-2.3f);				// 移入屏幕个单位
			glScaled(0.2,0.45,0.2);
			glRotatef(44,1.0f,0.0f,0.0f);
		}
		else
		{
			glTranslatef(0.0f,0.0f,-1.6f);				// 移入屏幕个单位
			glScaled(0.2,0.45,0.1);
			glRotatef(30,1.0f,0.0f,0.0f);
		}


		glEnable(GL_TEXTURE_2D);	
		glBindTexture(GL_TEXTURE_2D, texture[2]);		// 选择纹理
		glBegin(GL_QUADS);					// 四边形绘制开始

		for( x = 0; x < 74; x++ )				// 沿 X 平面 0-74 循环(75点)
		{
			for( y = 0; y < 74; y++ )			// 沿 Y 平面 0-74 循环(75点)
			{	
				glTexCoord2f(0.0f, 0.0f);	// 第一个纹理坐标 (左下角)
				glVertex3f( points[x][y][0], points[x][y][1], -points[x][y][2] );
				glTexCoord2f( 0.0f,1.0f );	// 第二个纹理坐标 (左上角)
				glVertex3f( points[x][y+1][0], points[x][y+1][1], -points[x][y+1][2] );
				glTexCoord2f( 1.0f,1.0f );	// 第三个纹理坐标 (右上角)
				glVertex3f( points[x+1][y+1][0], points[x+1][y+1][1], -points[x+1][y+1][2] );
				glTexCoord2f( 1.0f,0.0f );	// 第四个纹理坐标 (右下角)
				glVertex3f( points[x+1][y][0], points[x+1][y][1], -points[x+1][y][2] );
			}
		}
		glEnd();						// 四边形绘制结束
		glDisable(GL_TEXTURE_2D);	

	}
	hier--;
	glPopMatrix();

	glPushMatrix();
	hier++;
	if(VAL(HIERARCHY)>=hier){
		setDiffuseColor((float)105/255,(float)105/255,(float)105/255);
		glTranslated(-0.6,2.5,0);	
		glRotated(-90, 0.0, 1.0, 0.0);
		drawCylinder(1, 1.5, 1.5);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		//左肩圆球
		setDiffuseColor((float)139/255,(float)139/255,(float)122/255);
		glTranslated(0,0,1.2);
		if(VAL(LEFT_ARM_LINK)==1){	//左手联动控制盾牌防御姿势

			glRotated(60, 0.0, 1.0, 0.0);	
		}
		glRotated(VAL(LEFT_ARM_ROTATE), 0.0, 0.0, 1.0);	

		drawSphere(1);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		//左臂圆柱1
		setDiffuseColor((float)190/255,(float)190/255,(float)190/255);
		glRotated(70, 1.0, 0.0, 0.0);
		drawCylinder(2.0, 0.2, 0.2);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		//左臂圆柱2
		setDiffuseColor((float)139/255,(float)139/255,(float)122/255);
		glTranslated(0,0,1.8);

		if(VAL(LEFT_ARM_LINK)==1){	//控制盾牌防御姿势

			glRotated(30, 0.0, 1.0, 0.0);	
		}

		drawCylinder(1, 0.6, 0.6);
	}

	setDiffuseColor((float)255/255,(float)255/255,(float)255/255);
	hier++;
	if(VAL(HIERARCHY)>=hier){
		//左臂圆柱3 炮火群
		glTranslated(0,0,1);
		drawCylinder(0.3, 0.15, 0.15);
	}
	hier++;
	if(VAL(HIERARCHY)>=hier){
		glTranslated(0,-0.4,0);
		drawCylinder(0.3, 0.15, 0.15);
	}
	hier++;
	if(VAL(HIERARCHY)>=hier){
		glTranslated(0,0.8,0);
		drawCylinder(0.3, 0.15, 0.15);
	}
	hier++;
	if(VAL(HIERARCHY)>=hier){
		glTranslated(-0.4,-0.4,0);
		drawCylinder(0.3, 0.15, 0.15);
	}
	hier++;
	if(VAL(HIERARCHY)>=hier){
		glTranslated(0.8,0,0);
		drawCylinder(0.3, 0.15, 0.15);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		//盾牌 默认隐藏，通过控制条可以显示
		setDiffuseColor((float)190/255,(float)190/255,(float)190/255);
		glScaled(VAL(SHIELD),VAL(SHIELD),VAL(SHIELD));	
		glRotated(-70, 1.0, 0.0, 0.0);
		glTranslated(0,0,0.5);
		drawCylinder(0.7, 3, 0.1);
	}
	hier=hier-10;
	glPopMatrix();
	glPopMatrix();

	//右躯干的圆柱
	hier++;
	if(VAL(HIERARCHY)>=hier){
		glPushMatrix();	
		setDiffuseColor((float)105/255,(float)105/255,(float)105/255);
		glTranslated(0.6,2.5,0);	
		glRotated(90, 0.0, 1.0, 0.0);
		drawCylinder(1, 1.5, 1.5);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		//右肩圆球
		setDiffuseColor((float)139/255,(float)139/255,(float)122/255);
		glRotated(VAL(RIGHT_ARM_ROTATE), 0.0, 0.0, 1.0);	
		glTranslated(0,0,1.2);

		if(VAL(RIGHT_ARM_LINK)==1){	//右手联动控制攻击剑进攻姿势

			glRotated(-70, 0.0, 0.0, 1.0);	
		}

		drawSphere(1);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		//右臂圆柱1
		setDiffuseColor((float)190/255,(float)190/255,(float)190/255);
		glRotated(70, 1.0, 0.0, 0.0);
		drawCylinder(2.0, 0.2, 0.2);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		//右臂圆柱2
		glTranslated(0,0,1.8);
		setDiffuseColor((float)139/255,(float)139/255,(float)122/255);
		if(VAL(RIGHT_ARM_LINK)==1){	//右手联动控制攻击剑进攻姿势

			glRotated(-60, 0.0, 1.0, 0.0);	
		}

		drawCylinder(1, 0.6, 0.6);
	}

	hier++;
	if(VAL(HIERARCHY)>=hier){
		//右臂圆柱3 攻击剑
		setDiffuseColor((float)255/255,(float)255/255,(float)255/255);
		glRotated(10+tt, 0.0, 0.0, 1.0);
		glTranslated(0,0,1);
		glScalef(1, 1, VAL(SWORD_LENGTH));
		drawCylinder(4, 0.3, 0);
	}

	hier=hier-5;
	glPopMatrix();

	//unsigned char* texture_data = NULL; // it stores an array of pixels value
	//int texture_width = 0; // the width of the texture
	//int texture_height = 0; // the height of the texture

	//// Read the bitmap
	//texture_data = readBMP("CS341logo.bmp", texture_width, texture_height);

	//// Generate The Texture
	//glTexImage2D(GL_TEXTURE_2D, 0, 3, texture_width, texture_height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data);

	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
	//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Linear Filtering

	//glEnable(GL_TEXTURE_2D);
	////glDisable(GL_LIGHTING);
	//glBegin(GL_QUADS);
	////glNormal3d(0,1,0);
	//glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.9f, -0.7f, 0.8f); // Bottom Left Of The Texture and Quad
	//glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.7f, -0.7f, 0.8f); // Bottom Right Of The Texture and Quad
	//glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.7f, 0.5f, 0.8f); // Top Right Of The Texture and Quad
	//glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.9f, 0.5f, 0.8f); // Top Left Of The Texture and Quad
	//glDisable(GL_TEXTURE_2D);
	////glEnable(GL_LIGHTING);
	//glEnd();

	//delete [] texture_data;

	glPopMatrix();

}


int main()
{
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)
	ModelerControl controls[NUMCONTROLS];

	controls[DEFAULT_CAMERA_POINT] = ModelerControl("default_camera_point", 0, 1, 1, 0);
	controls[XPOS] = ModelerControl("X Position", -5, 5, 0.1, 0);
	controls[YPOS] = ModelerControl("Y Position", 0, 5, 0.1, 0);
	controls[ZPOS] = ModelerControl("Z Position", -5, 5, 0.1, 0);
	//controls[HEIGHT] = ModelerControl("Height", 1, 2.5, 0.1, 1);
	controls[BODY_ROTATE] = ModelerControl("body_Rotate", -110, 110, 1, 0);
	controls[SWORD_LENGTH] = ModelerControl("sword_Length", 0.2, 2.0, 0.1, 1);
	controls[HEAD_ROTATE] = ModelerControl("head_Rotate", -110, 110, 1, 0);
	controls[LEFT_ARM_ROTATE] = ModelerControl("left_ARM_Rotate", -150, 180, 1, 0);
	controls[RIGHT_ARM_ROTATE] = ModelerControl("right_ARM_Rotate", -180, 150, 1, 0);
	controls[LEFT_ARM_LINK] = ModelerControl("left_ARM_link", 0, 1, 1, 0);
	controls[RIGHT_ARM_LINK] = ModelerControl("right_ARM_link", 0, 1, 1, 0);

	controls[CAP] = ModelerControl("cap", 0, 1, 1, 1);
	controls[SHIELD] = ModelerControl("shield", 0, 1, 1, 0);

	controls[LIGHT_X_POSITION] = ModelerControl("LIGHT_X_POSITION", -10, 10, 1, 4);
	controls[LIGHT_Y_POSITION] = ModelerControl("LIGHT_Y_POSITION", -10, 10, 1, 2);
	controls[LIGHT_Z_POSITION] = ModelerControl("LIGHT_Z_POSITION", -10, 10, 1, -4);

	controls[LIGHT_Diffuse] = ModelerControl("LIGHT_Diffuse", 0, 255, 1, 1);

	controls[HIERARCHY] = ModelerControl("change the level of detail", 0, 10, 1, 10);
	controls[Cloak] = ModelerControl("Cloak", 0, 1, 1, 1);
	controls[WALK_ON] = ModelerControl("walk_on", 0, 1, 1, 0);
	controls[JET_SYSTEM_ON] = ModelerControl("jet_system_on", 0, 1, 1, 0);


	controls[RESET] = ModelerControl("reset", 0, 1, 1, 0);

	ModelerApplication::Instance()->Init(&createSampleModel, controls, NUMCONTROLS);

	if(VAL(RESET)==1){
		for(int i=0;i<NUMCONTROLS-1;i++)
			ModelerApplication::Instance()->SetControlValue(i,controls[i].m_value);
	}
	for (loop = 0;loop < MAX_PARTICLES;++loop)   //初始化所有的粒子
	{
		paticle[loop].active = true;	// 使所有的粒子为激活状态
		paticle[loop].life = 1.0f;//full life is 1.0f	// 所有的粒子生命值为最大
		//life decrease rate(a random value add 0.003f : never equals 0)
		paticle[loop].fade = float(rand() % 100) / 1000.0f + 0.05f;	// 随机生成衰减速率

		paticle[loop].xi = float((rand() % 50) - 26.0f) * 10.0f; // 随机生成X轴方向速度
		paticle[loop].yi = float((rand() % 50) - 25.0f) * 10.0f;
		paticle[loop].zi = float((rand() % 50) - 25.0f) * 10.0f;
		//paticle[loop].yi = -50.0f;

		paticle[loop].xg=0.0f;
		paticle[loop].yg=-7.8f;		//  设置Y轴方向加速度为-7.8
		paticle[loop].zg=0.0f;
	}


	return ModelerApplication::Instance()->Run();
}
