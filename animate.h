// implement the following functions

// LINEAR ALGEBRA LIBRARY
#include "Eigen/Dense"
//#include "Eigen/Core"
//#include "Eigen/LU"
//#include "Eigen/Geometry"

using Eigen::Matrix4f;
using Eigen::Vector3f;
using Eigen::Vector4f;

struct ArmPart{
    Vector3f joint;
    Vector3f jointNormal; 
    float jointRotation;
    
    Vector3f v1,v2,v3,v4; // armQuad    
    Vector4f slot;        // position where the other arm will fit
};

float xT, yT, zT, delta;
ArmPart arm1, arm2, arm3;


void DrawArm1();
void DrawArm2();
void DrawArm3();
void DrawAxis();
void initializeArms();

Matrix4f getModelview();
Vector4f computeE();



void initializeArms(){
    
    // arm1 initialization
    arm1.joint << 0,0,0.05;
    arm1.jointNormal << 0,0,1;
    arm1.jointRotation = 2;
    
    arm1.v1 << 0,-0.35,0;
    arm1.v2 << 5,-0.35,0;
    arm1.v3 << 5, 0.35,0;
    arm1.v4 << 0, 0.35,0;
    
    arm1.slot << 5,0,0,1;

    // arm2 initialization
    arm2.joint << 0,0,0.05;
    arm2.jointNormal << 0,0,1;
    arm2.jointRotation = -45;
    
    arm2.v1 << 0, -0.35, 0;
    arm2.v2 << 5, -0.35, 0;
    arm2.v3 << 5,  0.35, 0;
    arm2.v4 << 0,  0.35, 0;   
    
    arm2.slot << 5,0,0,1;
    
    // arm3 initialization
    arm3.joint << 0,0,0.05;
    arm3.jointNormal << 0,0,1;
    arm3.jointRotation = -30;
    
    arm3.v1 << 0,-0.35,0;
    arm3.v2 << 5,-0.35,0;
    arm3.v3 << 5, 0.35,0;
    arm3.v4 << 0, 0.35,0;
    
    arm3.slot << 5,0,0,1;
}

void Animation_render(){

    glPushMatrix();
        glRotatef(arm1.jointRotation, arm1.jointNormal[0], arm1.jointNormal[1], arm1.jointNormal[2]);    
        DrawArm1();    
        
        glPushMatrix();      
            //fit arm2 to arm1 slot
            glTranslatef(arm1.slot[0], arm1.slot[1], arm1.slot[2]);
            glRotatef(arm2.jointRotation, arm2.jointNormal[0], arm2.jointNormal[1], arm2.jointNormal[2]);              
            DrawArm2();            
            
            glPushMatrix();
                glTranslatef(arm2.slot[0], arm2.slot[1], arm2.slot[2]);
                glRotatef(arm3.jointRotation, arm3.jointNormal[0], arm3.jointNormal[1], arm3.jointNormal[2]);
                DrawArm3();
                
                // get modelview
                Matrix4f m = getModelview();
                
            glPopMatrix();            
        glPopMatrix();    
    glPopMatrix();
    
    // compute E vector and transform to globa coordinates 
    Vector4f Eh = computeE();
    Eh = m*Eh;
    //std::cout << Eh << std::endl;
    
    glColor3f(0,1,0);
    glBegin(GL_POINTS);
    glVertex3fv(Eh.data());
    glEnd();
    
    DrawAxis();
}

void Animation_init(){

    initializeArms();
    delta = 0.1;

}

void Animation_update(){


}

void Animation_reset(){
    xT = yT = zT = 0;
}





// COMPUTATIONS

Matrix4f getModelview(){
    
    glPushMatrix();
    glLoadIdentity();
    
    glRotatef(arm1.jointRotation, arm1.jointNormal[0], arm1.jointNormal[1], arm1.jointNormal[2]);  
    
    glTranslatef(arm1.slot[0], arm1.slot[1], arm1.slot[2]);
    glRotatef(arm2.jointRotation, arm2.jointNormal[0], arm2.jointNormal[1], arm2.jointNormal[2]); 
    
    glTranslatef(arm2.slot[0], arm2.slot[1], arm2.slot[2]);
    glRotatef(arm3.jointRotation, arm3.jointNormal[0], arm3.jointNormal[1], arm3.jointNormal[2]);

    //glLoadIdentity();
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    glPopMatrix();
    
    return Matrix4f(m);
}

Vector4f computeE(){
    return arm3.slot;    
}





// DRAW FUNCTIONS ---------------------------------

void DrawAxis(){
    glLineWidth(1.0f);
    glColor3f(0.8, 0.8, 0.0);
   
    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(100,0,0);
   
    glVertex3f(0,0,0);
    glVertex3f(0,100,0);
   
    glVertex3f(0,0,0);
    glVertex3f(0,0,100);
   
    glEnd();
    
}

void DrawArm1(){

     glColor3f(1,1,1);
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex3fv(arm1.joint.data());
    glEnd();
    
    glColor3f(0.8, 0, 0);
    glBegin(GL_QUADS);
    glVertex3fv(arm1.v1.data());
    glVertex3fv(arm1.v2.data());    
    glVertex3fv(arm1.v3.data());
    glVertex3fv(arm1.v4.data());
    glEnd();
}

void DrawArm2(){
    
    glColor3f(1,1,1);
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex3fv(arm2.joint.data());
    glEnd();

    glColor3f(0.8, 0, 0);
    glBegin(GL_QUADS);
    glVertex3fv(arm2.v1.data());
    glVertex3fv(arm2.v2.data());    
    glVertex3fv(arm2.v3.data());
    glVertex3fv(arm2.v4.data());
    glEnd();
}

void DrawArm3(){
    
    glColor3f(1,1,1);
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex3fv(arm3.joint.data());
    glEnd();

    glColor3f(0.8, 0, 1);
    glBegin(GL_QUADS);
    glVertex3fv(arm3.v1.data());
    glVertex3fv(arm3.v2.data());    
    glVertex3fv(arm3.v3.data());
    glVertex3fv(arm3.v4.data());
    glEnd();
}