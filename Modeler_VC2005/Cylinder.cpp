#include "Cylinder.h"  
#include <gl/glut.h>  
#include <iostream>  
#include <iomanip>  
#include <cmath>  
  
// ----------------------------------------------------------------  
//  Description:    ���ع��캯��  
//  Para info:      rΪ����Բ�İ뾶  
//              hΪ�߶�  
//              lΪ����Բ�ķֶ���  
//              vΪ�߶���Բ���Ķ���v>=2  
// ----------------------------------------------------------------  
  
Cylinder::Cylinder(float r, float h, int l, int v)  
    : radius(r)  
    , height(h)  
    , lSlice(l)  
    , vSlice(v)  
    , texId(0)  
    , isHasTex(false)  
    , pVertexBuffer(0)  
    , pTexcoord(0)  
{  
    pVertexBuffer = new Vertex3f[vSlice*lSlice];  
    pTexcoord = new float[vSlice];  
      
    _setupVertexTexcoord();  
    _getMatrices();  
    PrintMatrices();  
}  
  
Cylinder::~Cylinder()  
{  
    if (pVertexBuffer)  
    {  
        delete pVertexBuffer;  
        pVertexBuffer = 0;  
    }  
}  
  
// ----------------------------------------------------------------  
//  Description:    Բ������ƺ���  
//  ���ư�����ʱ�ӣ�  
//      v4      v3  
//    
//      v1      v2  
// ----------------------------------------------------------------  
void Cylinder::Render(REND_MODE mode)  
{  
    int i(0);  
    int j(0);  
  
    // ����Բ����  
    for (i=0; i<vSlice-1; i++)  
    {  
        for (j=0; j<lSlice-1; j++)  
        {  
            RenderSlice( pVertexBuffer[(i+1)*lSlice+j],  
                         pVertexBuffer[(i+1)*lSlice+j+1],             
                         pVertexBuffer[i*lSlice+j+1],  
                         pVertexBuffer[i*lSlice+j],   
                         i, j,  
                         mode );  
        }  
  
        RenderSlice( pVertexBuffer[(i+1)*lSlice+j],     //�ڶ���ĩ��  
                     pVertexBuffer[(i+1)*lSlice],       //�ڶ���ʼ��   
                     pVertexBuffer[i*lSlice],           //��һ��ʼ��  
                     pVertexBuffer[i*lSlice+j],         //��һ��ĩ��  
                     i, j+1,  
                     mode );  
  
    }  
}  
  
// ----------------------------------------------------------------  
//  Description:    �����ı��κ���  
//  ��������˳�� 1 2 3 4  
//  GL_TRIANGLE_STRIP�Ļ���˳��1 2 4 3  
//      �ĸ�v�����ṩ��λ�úͷ�������  
//      i��j�ṩ������������  
//      1, 4ΪҪ���⴦��ĵ�  
// ----------------------------------------------------------------  
void Cylinder::RenderSlice( const Vertex3f& v1,   
                            const Vertex3f& v2,   
                            const Vertex3f& v3,   
                            const Vertex3f& v4,  
                            int i, int j,   
                            REND_MODE mode )  
{  
    glDisable(GL_LIGHTING);  
  
    switch(mode)  
    {  
    case SOLID:  
        glBegin(GL_TRIANGLE_STRIP);  
        //glBegin(GL_POLYGON);  
        break;  
    case WIRE:  
        glBegin(GL_LINE_LOOP);  
        break;  
    }  
        glTexCoord2f( v1.u, v1.v );       
        glVertex3f(v1.x, v1.y, v1.z);   /*glNormal3f(v1.nx, v1.ny, v1.nz);*/      
  
        //���⴦��v2������  
        if (j==lSlice)  
        {  
            glTexCoord2f( 1.0f, pTexcoord[i+1] );         
            glVertex3f(v2.x, v2.y, v2.z);   /*glNormal3f(v2.nx, v2.ny, v2.nz);*/      
        }  
        else  
        {  
            glTexCoord2f( v2.u, v2.v );       
            glVertex3f(v2.x, v2.y, v2.z);   /*glNormal3f(v2.nx, v2.ny, v2.nz);*/      
        }  
  
        glTexCoord2f( v4.u, v4.v );       
        glVertex3f(v4.x, v4.y, v4.z);   /*glNormal3f(v4.nx, v4.ny, v4.nz);*/  
          
        //���⴦��v3������  
        if (j==lSlice)  
        {  
            glTexCoord2f( 1.0f, pTexcoord[i] );       
            glVertex3f(v3.x, v3.y, v3.z);   /*glNormal3f(v3.nx, v3.ny, v3.nz);*/      
        }  
        else  
        {  
            glTexCoord2f( v3.u, v3.v );       
            glVertex3f(v3.x, v3.y, v3.z);   /*glNormal3f(v3.nx, v3.ny, v3.nz);*/      
        }  
  
    glEnd();  
  
    // ��ʾ����  
    // RenderSliceNormal( v1, v2, v3, v4 );  
  
    glEnable(GL_LIGHTING);  
}  
  
void Cylinder::RenderSliceNormal( const Vertex3f& v1, const Vertex3f& v2, const Vertex3f& v3, const Vertex3f& v4 )  
{  
    glBegin(GL_LINES);  
    glVertex3f(v1.x, v1.y, v1.z);     
    glVertex3f(v1.nx + v1.x, v1.ny+v1.y, v1.nz+v1.z);  
    glEnd();  
  
    glBegin(GL_LINES);  
    glVertex3f(v2.x, v2.y, v2.z);     
    glVertex3f(v2.nx + v2.x, v2.ny+v2.y, v2.nz+v2.z);  
    glEnd();  
  
    glBegin(GL_LINES);  
    glVertex3f(v3.x, v3.y, v3.z);     
    glVertex3f(v3.nx + v3.x, v3.ny+v3.y, v3.nz+v3.z);  
    glEnd();  
  
    glBegin(GL_LINES);  
    glVertex3f(v4.x, v4.y, v4.z);     
    glVertex3f(v4.nx + v4.x, v4.ny+v4.y, v4.nz+v4.z);  
    glEnd();  
}  
  
// ----------------------------------------------------------------  
//  Description:    ����vSlice��lSlice������������  
//                    
// ----------------------------------------------------------------  
void Cylinder::_setupVertexTexcoord()  
{  
    if (!pVertexBuffer || !pTexcoord)  
        return;  
      
    float ds = 1.0f / lSlice;           //x,u ����  
    float dt = 1.0f / (vSlice-1);       //y,v ����  
  
    for (int i=0; i<vSlice; i++)  
    {  
        for (int j=0; j<lSlice; j++)  
        {  
            pVertexBuffer[j + i*lSlice].u = j*ds;  
            pVertexBuffer[j + i*lSlice].v = i*dt;  
        }  
  
        // ���һ�е����������ȫ��Ϊ��1.0  
        // pTexcoord��ֻ��������  
        pTexcoord[i] = i * dt;  
    }  
}  
  
  
// ----------------------------------------------------------------  
//  Description:    ���ݽǶȺ͸߶���ȡ���������  
//  Para info:      thetaΪ��������Բ�еĽǶ�  
//                  hghΪ�߶�    
// ----------------------------------------------------------------  
void Cylinder::_getVertex(float theta, float hgh, Vertex3f& vertex)  
{  
    float cosTheta = cos(theta);  
    float sinTheta = sin(theta);  
  
    // setup position coordinate  
    vertex.x = radius * cosTheta;  
    vertex.y = hgh;  
    vertex.z = radius * sinTheta;  
  
    // setup normal coordinate  
    vertex.nx = cosTheta;  
    vertex.ny = 0;  
    vertex.nz = sinTheta;  
}  
  
// ----------------------------------------------------------------  
//  Description:    ��ȡԲ���Ķ�������  
// ----------------------------------------------------------------  
void Cylinder::_getMatrices()  
{  
    const float pi = 3.1415926;  
  
    float angle = 2.0f * pi / static_cast<float>(lSlice);  
    // ע���ĸΪ�߶ȶ�����1  
    float span = height / static_cast<float>(vSlice-1);  
  
    for (int v=0; v<vSlice; v++)  
    {  
        float y = span * v;  
        for (int l=0; l<lSlice; l++)  
        {  
            float x = angle * static_cast<float>(l);  
            _getVertex(x, y, pVertexBuffer[l + v*lSlice]);  
        }  
    }  
}  
  
// ----------------------------------------------------------------  
//  Description:    ��ӡmatrices�е�Ԫ��  
// ----------------------------------------------------------------  
void Cylinder::PrintMatrices()  
{  
    using namespace std;  
    for (int i=0; i<vSlice*lSlice; i++)  
    {  
        cout << "//---------------------------------------------------------------------" << endl;  
        cout << "row = " << i / lSlice << ".";  
        cout << "coll = " << i % lSlice << endl;  
  
        cout << "position(x, y, z) = " << setprecision (7) << pVertexBuffer[i].x << " "  
                                       << setprecision (7) << pVertexBuffer[i].y << " "  
                                       << setprecision (7) << pVertexBuffer[i].z << endl;  
  
        cout << "texture(u, v) = " << setprecision (7) << pVertexBuffer[i].u << " "  
                                   << setprecision (7) << pVertexBuffer[i].v << endl;  
        cout << "//---------------------------------------------------------------------" << endl;  
    }  
}  