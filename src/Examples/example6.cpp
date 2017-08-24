#include <Qt3DIncludes.h>
#include <GaussIncludes.h>
#include <FEMIncludes.h>

//Any extra things I need such as constraints
#include <ConstraintFixedPoint.h>
#include <TimeStepperLinearModes.h>

using namespace Gauss;
using namespace FEM;
using namespace ParticleSystem; //For Force Spring

/* Tetrahedral finite elements */

//typedef physical entities I need

//typedef scene
typedef PhysicalSystemFEM<double, LinearTet> FEMLinearTets;

typedef World<double, std::tuple<FEMLinearTets *>, std::tuple<ForceSpring<double> *>, std::tuple<ConstraintFixedPoint<double> *> > MyWorld;
typedef TimeStepperLinearModes<double> MyTimeStepper;

typedef Scene<MyWorld, MyTimeStepper> MyScene;

int main(int argc, char **argv) {
    std::cout<<"Test Linear FEM \n";
    
    //Setup Physics
    MyWorld world;
    
    //new code -- load tetgen files
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    
    readTetgen(V, F, dataDir()+"/meshesTetgen/Beam/Beam.node", dataDir()+"/meshesTetgen/Beam/Beam.ele");
    
    FEMLinearTets *test = new FEMLinearTets(V,F);
    world.addSystem(test);
    world.finalize();
    
    
    //Modal analysis
    auto modes = linearModalAnalysis(world, 10);
    
    //generate wiggly animations
    MyTimeStepper stepper(1e-3, modes.first, modes.second, 9);
    
    MyScene *scene = new MyScene(&world, &stepper);
    
    //Display
    QGuiApplication app(argc, argv);
    
    GAUSSVIEW(scene);
    gaussView.startScene();
    
    return app.exec();
}
