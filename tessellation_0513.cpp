//
//  main.cpp
//  GLUTProgram
//
//  Created by KIM HYUNG SEOK on 2015. 10. 1..
//  Copyright © 2015년 KIM HYUNG SEOK. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace std;

GLuint g_ProgramID;

GLuint VAOs[2];

GLuint VBOs[3]; // 0513 수정, 3개로 만듦


GLuint g_moveLoc;
GLuint g_thetaLoc;

GLuint  g_matLoc;
GLfloat transMat[16];

// Object 1
int g_numVtxObj1;
float g_first[3];
// Object 2
int g_numVtxObj2;
float g_second[3];

glm::mat4 mainMat;

bool rotateMode;
float angle;

vector<GLfloat> cube_vtxBufferData2;
vector<GLuint> cube_colorBufferData2;
vector<GLuint> cube_indexData2;

//
// tessellation height
float diff = 0.1;
float nowheight = 0.1;
//


GLfloat cube_vtxBufferData[72] = {
    -0.1, -0.1, 0.1, // vertex
    1.0, 0.0, 0.0,   // color
    0.1, -0.1, 0.1,
    1.0, 1.0, 0.0,
    0.1, -0.1, -0.1,
    1.0, 1.0, 1.0,
    -0.1, -0.1, -0.1,
    0.0, 1.0, 0.0,
    -0.1, 0.1, 0.1,
    0.0, 1.0, 1.0,
    0.1, 0.1, 0.1,
    0.0, 0.0, 1.0,
    0.1, 0.1, -0.1,
    1.0, 0.0, 1.0,
    -0.1, 0.1, -0.1,
    -0.5, -0.5, -0.5
    // start : 48
};

GLuint cube_indexData[60] {
    0, 1, 4, 4, 1, 5,
    5, 1, 2, 5, 2, 6,
    6, 2, 3, 6, 3, 7,
    7, 3, 0, 7, 0, 4,
    7, 4, 5, 7, 5, 6,
    3, 1, 0, 1, 3, 2
};


GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
    //create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    GLint Result = GL_FALSE;
    int InfoLogLength;
    
    //Read the vertex shader code from the file
    string VertexShaderCode;
    ifstream VertexShaderStream(vertex_file_path, ios::in);
    if(VertexShaderStream.is_open())
    {
        string Line = "";
        while(getline(VertexShaderStream, Line))
            VertexShaderCode += "\n" + Line;
        VertexShaderStream.close();
    }
    
    //Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);
    
    //Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    vector<char> VertexShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &VertexShaderErrorMessage[0]);
    
    //Read the fragment shader code from the file
    string FragmentShaderCode;
    ifstream FragmentShaderStream(fragment_file_path, ios::in);
    if(FragmentShaderStream.is_open())
    {
        string Line = "";
        while(getline(FragmentShaderStream, Line))
            FragmentShaderCode += "\n" + Line;
        FragmentShaderStream.close();
    }
    
    //Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);
    
    //Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    vector<char> FragmentShaderErrorMessage(InfoLogLength);
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    fprintf(stdout, "%s\n", &FragmentShaderErrorMessage[0]);
    
    //Link the program
    fprintf(stdout, "Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
    
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    vector<char> ProgramErrorMessage( max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    fprintf(stdout, "%s\n", &ProgramErrorMessage[0]);
    
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    
    return ProgramID;
}

void    setIdentity(float* mat)
{
    mat[0] = 1.0f;  mat[1] = 0.0f;  mat [2] = 0.0f; mat[3] = 0.0f;
    mat[4] = 0.0f;  mat[5] = 1.0f;  mat[6] = 0.0f;  mat[7] = 0.0f;
    mat[8] = 0.0f;  mat[9] = 0.0f;  mat[10] = 1.0f;  mat[11] = 0.0f;
    mat[12] = 0.0f;    mat[13] = 0.0f;    mat[14] = 0.0f;    mat[15] = 1.0f;
}
void    setTranslation(float* mat, float x, float y, float z)
{
    mat[0] = 1.0f;  mat[1] = 0.0f;  mat [2] = 0.0f; mat[3] = 0.0f;
    mat[4] = 0.0f;  mat[5] = 1.0f;  mat[6] = 0.0f;  mat[7] = 0.0f;
    mat[8] = 0.0f;  mat[9] = 0.0f;  mat[10] = 1.0f; mat[11] = 0.0f;
    mat[12] = x;    mat[13] = y;    mat[14] = z;    mat[15] = 1.0f;
}

void renderScene(void)
{
    //Clear all pixels
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //Let's draw something here
    
    //    setTranslation(transMat, g_x, g_y, 0.0);
    //    glUniformMatrix4fv(g_matLoc, 1, GL_FALSE, transMat);
    
    
    //define the size of point and draw a point.
    
    //GLfloat mat[16];
    //setTranslation(mat, -0.5, g_first[1], g_first[2]);
    
    if (rotateMode == true)
    {
        glm::mat4 rot = glm::rotate(0.1f, glm::vec3(0, 1, 0)); // ?
        //mainMat = mainMat * rot;
        mainMat = rot * mainMat;
        //angle += 0.00001f;
        //rotateMode = false;
    }
    GLuint matLoc = glGetUniformLocation(g_ProgramID, "tranMat");
    glUniformMatrix4fv(matLoc, 1, GL_FALSE, &mainMat[0][0]);
    glBindVertexArray(VAOs[0]);
    //  glDrawArrays(GL_TRIANGLES, 0, g_numVtxObj1);
    glDrawElements(GL_TRIANGLES, g_numVtxObj1, GL_UNSIGNED_INT, ((GLvoid*)(0)));
    /*
     setTraf=nslation(mat, 0.5, g_second[1], g_second[2]);
     glUniformMatrix4fv(matLoc, 1, GL_FALSE, mat);
     glDrawArrays(GL_TRIANGLES, 0, g_numVtxObj1);
     
     setIdentity(mat);
     glUniformMatrix4fv(matLoc, 1, GL_FALSE, mat);
     glBindVertexArray(VAOs[1]);
     glDrawArrays(GL_TRIANGLES, 0, g_numVtxObj2);
     */
    //Double buffer
    glutSwapBuffers();
    
    glutPostRedisplay();
}


void init()
{
    //initilize the glew and check the errors.
    /*
     GLenum res = glewInit();
     if(res != GLEW_OK)
     {
     fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
     }
     */
    //select the background color
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glEnable(GL_DEPTH_TEST);
    
    g_first[0] = g_first[1] = g_first[2] = 0.0f;
    g_second[0] = g_second[1] = g_second[2] = 0.0f;
    
    mainMat = glm::mat4(1.0f);
    
    rotateMode = false;
    angle = 0.0f;
    
    for (int i = 0; i < 48; ++i)//sizeof(cube_vtxBufferData) / sizeof(cube_vtxBufferData[0]); ++i)
    {
        if (i < 36)
        {
            cube_indexData2.push_back(cube_indexData[i]);
        }
        cube_vtxBufferData2.push_back(cube_vtxBufferData[i]);
    }
    //cube_vertexBufferData2.push_back(
    
}

void myKeyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'q':
            g_first[1] -= 0.1;
            break;
        case 'w':
            g_first[1] += 0.1;
            break;
        case 'e':
            g_second[1] -= 0.1;
            break;
        case 'r':
            g_second[1] += 0.1;
            break;
        case 'a':
            g_first[2] += 0.1;
            break;
        case 'z':
            g_first[2] -= 0.1;
            break;
        case 's':
            g_second[2] += 0.1;
            break;
        case 'x':
            g_second[2] -= 0.1;
            break;
        case 'y':
            int idx = cube_vtxBufferData2.size();
            nowheight += diff;
            
            cube_vtxBufferData2.push_back(-0.1);
            cube_vtxBufferData2.push_back(nowheight);
            cube_vtxBufferData2.push_back(0.1);
            
            cube_colorBufferData2.push_back(0.0);
            cube_colorBufferData2.push_back(0.0);
            cube_colorBufferData2.push_back(1.0);
            
            cube_vtxBufferData2.push_back(0.1);
            cube_vtxBufferData2.push_back(nowheight);
            cube_vtxBufferData2.push_back(0.1);
            
            cube_colorBufferData2.push_back(0.0);
            cube_colorBufferData2.push_back(0.0);
            cube_colorBufferData2.push_back(1.0);
            
            cube_vtxBufferData2.push_back(0.1);
            cube_vtxBufferData2.push_back(nowheight);
            cube_vtxBufferData2.push_back(-0.1);
            
            cube_colorBufferData2.push_back(0.0);
            cube_colorBufferData2.push_back(0.0);
            cube_colorBufferData2.push_back(1.0);
            
            cube_vtxBufferData2.push_back(-0.1);
            cube_vtxBufferData2.push_back(nowheight);
            cube_vtxBufferData2.push_back(-0.1);
            
            cube_colorBufferData2.push_back(0.0);
            cube_colorBufferData2.push_back(0.0);
            cube_colorBufferData2.push_back(1.0);
            
//            cube_vtxBufferData[idx++] = 0.0;
//            cube_vtxBufferData[idx++] = 0.2;
//            cube_vtxBufferData[idx++] = 0.1;
//            
//            cube_vtxBufferData[idx++] = 0.0;
//            cube_vtxBufferData[idx++] = 0.0;
//            cube_vtxBufferData[idx++] = 1.0;
//            
//            cube_vtxBufferData[idx++] = 0.2;
//            cube_vtxBufferData[idx++] = 0.2;
//            cube_vtxBufferData[idx++] = 0.1;
//            
//            cube_vtxBufferData[idx++] = 0.0;
//            cube_vtxBufferData[idx++] = 0.0;
//            cube_vtxBufferData[idx++] = 1.0;
//            
//            cube_vtxBufferData[idx++] = 0.2;
//            cube_vtxBufferData[idx++] = 0.2;
//            cube_vtxBufferData[idx++] = -0.1;
//            
//            cube_vtxBufferData[idx++] = 0.0;
//            cube_vtxBufferData[idx++] = 0.0;
//            cube_vtxBufferData[idx++] = 1.0;
//            
//            cube_vtxBufferData[idx++] = 0.0;
//            cube_vtxBufferData[idx++] = 0.2;
//            cube_vtxBufferData[idx++] = -0.1;
//            
//            cube_vtxBufferData[idx++] = 0.0;
//            cube_vtxBufferData[idx++] = 0.0;
//            cube_vtxBufferData[idx++] = 1.0;
            
            glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*12*6, cube_vtxBufferData, GL_STATIC_DRAW);
            
            //idx = 36;
            
            cube_indexData2.push_back(8);
            cube_indexData2.push_back(4);
            cube_indexData2.push_back(5);
            
            cube_indexData2.push_back(8);
            cube_indexData2.push_back(5);
            cube_indexData2.push_back(9);
            
            cube_indexData2.push_back(9);
            cube_indexData2.push_back(5);
            cube_indexData2.push_back(6);
            
            cube_indexData2.push_back(9);
            cube_indexData2.push_back(6);
            cube_indexData2.push_back(10);
            
            cube_indexData2.push_back(10);
            cube_indexData2.push_back(6);
            cube_indexData2.push_back(7);
            
            cube_indexData2.push_back(10);
            cube_indexData2.push_back(7);
            cube_indexData2.push_back(11);
            
            cube_indexData2.push_back(11);
            cube_indexData2.push_back(7);
            cube_indexData2.push_back(4);
            
            cube_indexData2.push_back(11);
            cube_indexData2.push_back(4);
            cube_indexData2.push_back(8);
            
//            cube_indexData[idx++] = 8;
//            cube_indexData[idx++] = 4;
//            cube_indexData[idx++] = 5;
//            
//            cube_indexData[idx++] = 8;
//            cube_indexData[idx++] = 5;
//            cube_indexData[idx++] = 9;
//            
//            cube_indexData[idx++] = 9;
//            cube_indexData[idx++] = 5;
//            cube_indexData[idx++] = 6;
//            
//            cube_indexData[idx++] = 9;
//            cube_indexData[idx++] = 6;
//            cube_indexData[idx++] = 10;
//            
//            cube_indexData[idx++] = 10;
//            cube_indexData[idx++] = 6;
//            cube_indexData[idx++] = 7;
//            
//            cube_indexData[idx++] = 10;
//            cube_indexData[idx++] = 7;
//            cube_indexData[idx++] = 11;
//            
//            cube_indexData[idx++] = 11;
//            cube_indexData[idx++] = 7;
//            cube_indexData[idx++] = 4;
//            
//            cube_indexData[idx++] = 11;
//            cube_indexData[idx++] = 4;
//            cube_indexData[idx++] = 8;
            
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOs[1]);
            //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*60, &cube_indexData2[0], GL_STATIC_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*cube_indexData2.size(), &cube_indexData2[0], GL_STATIC_DRAW);
            
            g_numVtxObj1 += 8*3;
            break;
    };
    glutPostRedisplay();
}
void myMouse(int button, int status, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (status == GLUT_DOWN)
        {
            rotateMode = true;
            //rotateMode = !rotateMode;
        }
        else if (status == GLUT_UP)
        {
            //rotateMode = false;
        }
    }
    
    float fx, fy;
    fx = 2*x/480.0f - 1;
    fy = -2*y/480.0f + 1;
    
    glm::vec3 u1, u2, u3;
    u1 = glm::vec3(fx, fy, 0);
    u1 = glm::normalize(u1);
    u3 = glm::vec3(0, 0, 1);
    u2 = glm::cross(u3, u1);
    
    mainMat[0] = glm::vec4(u1, 0);
    mainMat[1] = glm::vec4(u2, 0);
    mainMat[2] = glm::vec4(u3, 0);
    
    glutPostRedisplay();
}

int createThreeTriangles(GLuint vboID)
{
    GLfloat vtxBufferData[] = {
        -0.5, 0.0, 0.0, 0.0, 0.0, 1.0,
        0.5, 0.0, 0.0, 0.0, 0.0, 1.0,
        0.0, 0.5, 0.0, 0.0, 0.0, 1.0,
        0.0, 0.1, -0.5, 1.0, 0.0, 0.0,
        0.8, 0.1, -0.5, 1.0, 0.0, 0.0,
        0.4, 0.8, -0.5, 1.0, 0.0, 0.0,
        0.0, -0.5, 0.5, 0.0, 1.0, 0.0,
        0.8, -0.5, 0.5, 0.0, 1.0, 0.0,
        0.4, 0.3, 0.5, 0.0, 1.0, 0.0
    };
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*9*6, vtxBufferData, GL_STATIC_DRAW);
    
    return 9;
}

int createTheeLines(GLuint vboID)
{
    GLfloat vtxBufferData[] = {
        -0.7, 0.7, -0.9, 1.0, 1.0, 0.0,
        -0.9, 0.3, -0.9, 1.0, 1.0, 0.0,
        -0.8, 0.5, -0.9, 1.0, 1.0, 1.0,
        -0.4, 0.3, -0.9, 1.0, 1.0, 1.0,
        0.8, 0.5, -0.9, 0.0, 1.0, 1.0,
        0.4, 0.3, -0.9, 0.0, 1.0, 1.0
    };
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*6*6, vtxBufferData, GL_STATIC_DRAW);
    
    return 6;
}

int createIndexedCube2(GLuint vboid1, GLuint vboid2, GLuint vboid3)
{
    glBindBuffer(GL_ARRAY_BUFFER, vboid1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cube_vtxBufferData2.size(),
                 &cube_vtxBufferData2[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vboid2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cube_colorBufferData2.size(),
                 &cube_colorBufferData2[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboid3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * cube_indexData2.size(),
                 &cube_indexData2[0], GL_STATIC_DRAW);
    return (int)cube_indexData2.size();
}


int createIndexedCube(GLuint vboID, GLuint veoID)
{
    glBindBuffer(GL_ARRAY_BUFFER, vboID); // 0508
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*8*6, &cube_vtxBufferData2[0], GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veoID); // 0508
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*36, &cube_indexData2[0], GL_STATIC_DRAW);
    
    return 36;
}

int createCube(GLuint vboID)
{
//    GLfloat vtxBufferData[] =  {
//        -0.1f,-0.1f,-0.1f, 1.0, 0.0, 0.0, // triangle 1 : begin
//        -0.1f,-0.1f, 0.1f, 1.0, 0.0, 0.0,
//        -0.1f, 0.1f, 0.1f, 1.0, 0.0, 0.0, // triangle 1 : end
//        -0.1f,-0.1f,-0.1f, 1.0, 0.0, 0.0,
//        -0.1f, 0.1f, 0.1f, 1.0, 0.0, 0.0,
//        -0.1f, 0.1f,-0.1f, 1.0, 0.0, 0.0,
//        
//        0.1f, 0.1f,-0.1f, 0.0, 1.0, 0.0, // triangle 3 : begin
//        -0.1f,-0.1f,-0.1f, 0.0, 1.0, 0.0,
//        -0.1f, 0.1f,-0.1f, 0.0, 1.0, 0.0,  // triangle 3 : end
//        0.1f, 0.1f,-0.1f, 0.0, 1.0, 0.0,
//        0.1f,-0.1f,-0.1f, 0.0, 1.0, 0.0,
//        -0.1f,-0.1f,-0.1f, 0.0, 1.0, 0.0,
//        
//        0.1f,-0.1f, 0.1f, 0.0, 0.0, 1.0,
//        -0.1f,-0.1f,-0.1f, 0.0, 0.0, 1.0,
//        0.1f,-0.1f,-0.1f, 0.0, 0.0, 1.0,
//        0.1f,-0.1f, 0.1f, 0.0, 0.0, 1.0,
//        -0.1f,-0.1f, 0.1f, 0.0, 0.0, 1.0,
//        -0.1f,-0.1f,-0.1f, 0.0, 0.0, 1.0,
//        
//        -0.1f, 0.1f, 0.1f, 1.0, 1.0, 0.0,
//        -0.1f,-0.1f, 0.1f, 1.0, 1.0, 0.0,
//        0.1f,-0.1f, 0.1f, 1.0, 1.0, 0.0,
//        0.1f, 0.1f, 0.1f, 1.0, 1.0, 0.0,
//        -0.1f, 0.1f, 0.1f, 1.0, 1.0, 0.0,
//        0.1f,-0.1f, 0.1f, 1.0, 1.0, 0.0,
//        
//        0.1f, 0.1f, 0.1f, 1.0, 0.0, 1.0,
//        0.1f,-0.1f,-0.1f, 1.0, 0.0, 1.0,
//        0.1f, 0.1f,-0.1f, 1.0, 0.0, 1.0,
//        0.1f,-0.1f,-0.1f, 1.0, 0.0, 1.0,
//        0.1f, 0.1f, 0.1f, 1.0, 0.0, 1.0,
//        0.1f,-0.1f, 0.1f, 1.0, 0.0, 1.0,
//        
//        0.1f, 0.1f, 0.1f, 0.0, 1.0, 1.0,
//        0.1f, 0.1f,-0.1f, 0.0, 1.0, 1.0,
//        -0.1f, 0.1f,-0.1f, 0.0, 1.0, 1.0,
//        0.1f, 0.1f, 0.1f, 0.0, 1.0, 1.0,
//        -0.1f, 0.1f,-0.1f, 0.0, 1.0, 1.0,
//        -0.1f, 0.1f, 0.1f, 0.0, 1.0, 1.0
//    };
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*36*6, &cube_vtxBufferData2[0], GL_STATIC_DRAW); // 0508
    
    return 36;
}

int main(int argc, char **argv)
{
    //init GLUT and create Window
    //initialize the GLUT
    glutInit(&argc, argv);
    //GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_DOUBLE | GLUT_RGBA
                        | GLUT_DEPTH);
    //These two functions are used to define the position and size of the window.
    glutInitWindowPosition(200, 200);
    glutInitWindowSize(480, 480);
    //This is used to define the name of the window.
    glutCreateWindow("Simple OpenGL Window");
    
    //call initization function
    init();
    
    //3.
    GLuint programID = LoadShaders("VertexShader_0508.txt", "FragmentShader_0508.txt");
    glUseProgram(programID);
    g_ProgramID = programID;
    
    g_moveLoc = glGetUniformLocation(programID,"move");
    
    g_thetaLoc = glGetUniformLocation(programID,"theta");
    
    g_matLoc = glGetUniformLocation(programID, "tranMat");
    //1.
    //Generate VAO
    GLuint VertexArrayID;
    glGenVertexArrays(2, VAOs); // 왜 두 개를 생성하는 것일까.
    //2.
    // Generate VBO
    
    glGenBuffers(3, VBOs);
    
    glBindVertexArray(VAOs[0]);
    
    //g_numVtxObj1 = createIndexedCube(VBOs[0], VBOs[1]); // 0513 수정
    g_numVtxObj1 = createIndexedCube2(VBOs[0], VBOs[1], VBOs[2]);
    
    GLuint posID = glGetAttribLocation(programID, "a_Pos");
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]); // 0513 bind buffer
    glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, ((GLvoid*)(0)));
    //glVertexAttribPointer
    //glVertexAttribPointer(posID, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, ((GLvoid*)(0)));
    //  2번째:3개 읽음/5 번째: 6개씩 뛴다.
    glEnableVertexAttribArray(posID);
    
    GLuint colID = glGetAttribLocation(programID, "a_Col");
    glVertexAttribPointer(colID, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, ((GLvoid*)(sizeof(GLfloat)*3)));
    // 마지막 : 시작 주소
    glEnableVertexAttribArray(colID);
    
    glutMouseFunc(myMouse);
    glutKeyboardFunc(myKeyboard);
    glutDisplayFunc(renderScene);
    
    //enter GLUT event processing cycle
    glutMainLoop();
    
    glDeleteVertexArrays(1, &VertexArrayID);
    
    return 1;
}
