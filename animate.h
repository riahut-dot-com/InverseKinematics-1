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
Vector4f computeE();
Vector4f ComputeJacobian(ArmPart a1, ArmPart a2, ArmPart a3, Vector4f E);

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
    arm3.jointRotation = -30;
    
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
        //glRotatef(arm1.jointRotation, arm1.jointNormal[0], arm1.jointNormal[1], arm1.jointNormal[2]); 
    
    arm1.jointWorld = model*arm1.joint;
    DrawArm1(view*model);    
    
    model.translate(arm1.slot);
    model.rotate(AngleAxisf(arm2.jointRotation, arm2.jointNormal));
    DrawArm2(view*model);
    
    
    model.translate(arm2.slot);
    model.rotate(AngleAxisf(arm3.jointRotation, arm3.jointNormal));
    DrawArm3(view*model);
    
    // compute E vector and transform to globa coordinates 
    //Vector4f Eh = computeE();
    //Eh = m*Eh;
    //std::cout << Eh << std::endl;
    //ComputeJacobian(arm1,arm2,arm3,Eh);
    
    glMultMatrixf(view.data());
        
    DrawG();
    DrawAxis();
    
    //glLoadIdentity();
//    glColor3f(0,1,0);
//    glBegin(GL_POINTS);
//    glVertex3fv(Eh.data());
//    glEnd();
    
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

Vector4f computeE(){
    Vector4f e; e << arm3.slot,1;
    return e;    
}

float computeTheta(ArmPart p){
    //Vector3f axisX << 0,0,1;
    //p.slot - p.joint;
}

Vector4f ComputeJacobian(ArmPart a1, ArmPart a2, ArmPart a3, Vector4f _E){
    Matrix3f j;
    Vector3f E, jn1,jn2,jn3,jn4;
    Vector3f p1,p2,p3;
    
    E << _E(0), _E(1), _E(2);
    
    jn1 << a1.jointNormal(0), a1.jointNormal(1), a1.jointNormal(2);
    jn2 << a2.jointNormal(0), a2.jointNormal(1), a2.jointNormal(2);
    jn3 << a3.jointNormal(0), a3.jointNormal(1), a3.jointNormal(2);
    
    //std::cout << a1.jointWorld << std::endl;
    p1 << a1.jointWorld(0), a1.jointWorld(1), a1.jointWorld(2);
    p2 << a2.jointWorld(0), a2.jointWorld(1), a2.jointWorld(2) ;
    p3 << a3.jointWorld(0), a3.jointWorld(1), a3.jointWorld(2);
    
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