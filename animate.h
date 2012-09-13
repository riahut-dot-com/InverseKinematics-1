// implement the following functions

// LINEAR ALGEBRA LIBRARY
#include "Eigen/Dense"
//#include "Eigen/Core"
//#include "Eigen/LU"
//#include "Eigen/Geometry"

using Eigen::Affine3f;
using Eigen::Matrix3f;
using Eigen::Matrix4f;
using Eigen::Vector3f;
using Eigen::Vector4f;
using Eigen::AngleAxisf;

struct ArmPart{
    Vector4f joint;
    Vector4f jointWorld;
    Vector3f jointNormal; 
    float jointRotation;
    //float globaltheta;
    
    Vector3f v1,v2,v3,v4; // armQuad    
    Vector3f slot;        // position where the other arm will fit
};

float xT, yT, zT, delta;
ArmPart arm1, arm2, arm3;
Vector4f G;


void DrawArm1(Affine3f view);
void DrawArm2(Affine3f view);
void DrawArm3(Affine3f view);
void DrawG();
void DrawAxis();
void initializeArms();

Affine3f getModelview();
Vector4f hmg(Vector3f v);
Vector3f invHmg(Vector4f v);
//Vector4f computeE();
Matrix3f ComputeJacobian(ArmPart a1, ArmPart a2, ArmPart a3, Vector4f E);

Affine3f view;
Affine3f model;

//Affine3f model;
//float computeTheta(ArmPart p);



void initializeArms(){
    
    G << 7,10,0,1;
    
    // arm1 initialization
    arm1.joint << 0,0,0,1;
    arm1.jointNormal << 0,0,1;
    arm1.jointRotation = 45;
    
    arm1.v1 << 0,-0.35,0;
    arm1.v2 << 5,-0.35,0;
    arm1.v3 << 5, 0.35,0;
    arm1.v4 << 0, 0.35,0;
    
    arm1.slot << 5,0,0;

    // arm2 initialization
    arm2.joint << 0,0,0,1;
    arm2.jointNormal << 0,0,1;
    arm2.jointRotation = -45;
    
    arm2.v1 << 0, -0.35, 0;
    arm2.v2 << 5, -0.35, 0;
    arm2.v3 << 5,  0.35, 0;
    arm2.v4 << 0,  0.35, 0;   
    
    arm2.slot << 5,0,0;
    
    // arm3 initialization
    arm3.joint << 0,0,0,1;
    arm3.jointNormal << 0,0,1;
    arm3.jointRotation = -45;
    
    arm3.v1 << 0,-0.35,0;
    arm3.v2 << 5,-0.35,0;
    arm3.v3 << 5, 0.35,0;
    arm3.v4 << 0, 0.35,0;
    
    arm3.slot << 5,0,0;
}

void Animation_render(){
    view = getModelview();    
    model = Affine3f::Identity();
    glLoadIdentity();
 
    model.rotate(AngleAxisf(arm1.jointRotation, arm1.jointNormal));    
    arm1.jointWorld = model*arm1.joint;
    DrawArm1(view*model);    
    
    model.translate(arm1.slot);
    model.rotate(AngleAxisf(arm2.jointRotation, arm2.jointNormal));
    arm2.jointWorld = model*arm2.joint;
    DrawArm2(view*model);
    
    
    model.translate(arm2.slot);
    model.rotate(AngleAxisf(arm3.jointRotation, arm3.jointNormal));
    arm3.jointWorld = model*arm3.joint;
    DrawArm3(view*model);
    
    // compute E vector in world coordiantes
    Vector4f E = model*hmg(arm3.slot); 
    
    Matrix3f j = ComputeJacobian(arm1,arm2,arm3,E);
    
    glMultMatrixf(view.data());
        
    DrawG();
    DrawAxis();   
   
    
    glColor3f(0,1,0);
    glBegin(GL_POINTS);
    glVertex3fv(E.data());
    glEnd();
    /*
    glColor3f(1,0,1);
    glBegin(GL_POINTS);
    glVertex3fv(arm1.jointWorld.data());
    glEnd();
    
    glColor3f(1,0,1);
    glBegin(GL_POINTS);
    glVertex3fv(arm2.jointWorld.data());
    glEnd();
    
    glColor3f(1,0,1);
    glBegin(GL_POINTS);
    glVertex3fv(arm3.jointWorld.data());
    glEnd();
    */

}

void Animation_init(){
    view = Affine3f::Identity();
    model = Affine3f::Identity();
    
    initializeArms();
    delta = 0.1;

}


void Animation_update(){


}

void Animation_reset(){
    xT = yT = zT = 0;
}





// COMPUTATIONS

Affine3f getModelview(){
     
    float m[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, m);
    
    return Affine3f(Matrix4f(m));
}

/* transform vector to homogenous space*/
Vector4f hmg(Vector3f v){
    Vector4f b;
    b << v,1;
    return b;
}

Vector3f invHmg(Vector4f v){
    Vector3f a(v.data());
    return a;
    //return v;
}

Matrix3f ComputeJacobian(ArmPart a1, ArmPart a2, ArmPart a3, Vector4f _E){
    Matrix3f j;
    Vector3f E, jn1,jn2,jn3,jn4;
    Vector3f p1,p2,p3;
    
    E = invHmg(_E);
    
    jn1 = a1.jointNormal;
    jn2 = a2.jointNormal;
    jn3 = a3.jointNormal;
    
    p1 << invHmg(a1.jointWorld);
    p2 << invHmg(a2.jointWorld);
    p3 << invHmg(a3.jointWorld);
    
    //Vector4f a1 = 
    j.col(0) << jn1.cross(E-p1);
    j.col(1) << jn2.cross(E-p2);
    j.col(2) << jn3.cross(E-p3);
    
    //std::cout << j << std::endl;
    //j.col(1) << j
}





// DRAW FUNCTIONS ---------------------------------

void DrawG(){
    glColor3f(1,1,1);
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    glVertex3fv(G.data());
    glEnd();
}

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

void DrawArm1(Affine3f modelview){

    glPushMatrix();
    //Affine3f world = getModelview();    
    glLoadIdentity();
    glMultMatrixf(modelview.data()); 
    
    
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
    
    glPopMatrix();
}

void DrawArm2(Affine3f view){
    
    glPushMatrix();
    //glMultMatrixf(view.data());    
    Affine3f world = getModelview();
    glLoadIdentity();
    glMultMatrixf((view*world).data()); 
    
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
    
    glPopMatrix();
}

void DrawArm3(Affine3f view){
    glPushMatrix();
    Affine3f world = getModelview();
    glLoadIdentity();
    glMultMatrixf((world*view).data());  
    
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
    
    glPopMatrix();
}