import cddp
import numpy as np
import pinocchio as se3


np.set_printoptions(linewidth=400, suppress=True, threshold=np.nan)

display = True
plot = True


# Creating the system model
import rospkg
path = rospkg.RosPack().get_path('hyq_description')
urdf = path + '/robots/hyq.urdf'
robot = se3.robot_wrapper.RobotWrapper(urdf, path, se3.JointModelFreeFlyer())
model = robot.model
system = cddp.NumDiffSparseConstrainedForwardDynamics(model)
q0 = robot.q0
q0[7] = -0.2
q0[7+1] = 0.75
q0[7+2] = -1.5
q0[7+3] = -0.2
q0[7+4] = -0.75
q0[7+5] = 1.5
q0[7+6] = -0.2
q0[7+7] = 0.75
q0[7+8] = -1.5
q0[7+9] = -0.2
q0[7+10] = -0.75
q0[7+11] = 1.5

v0 = np.zeros((system.getTangentDimension(), 1))
u0 = np.zeros((system.getTangentDimension(), 1))
x0 = np.vstack([q0, v0])

data = system.createData()
system.g(data, q0, v0, u0)





# Defining the SE3 task
frame_name = 'base_link'
M_des = cddp.se3.SE3(np.eye(3), np.array([ [0.1], [0.], [0.] ]))
se3_cost = cddp.SE3RunningCost(model, frame_name, M_des)
w_se3 = 1000.*np.array([1., 1., 1., 1., 1., 1.])
se3_cost.setWeights(w_se3)

# Defining the velocity and control regularization
xu_reg = cddp.StateControlQuadraticRegularization()
wx = 1e-4 * np.hstack([ np.zeros(model.nv), np.ones(model.nv) ])
wu = 1e-4 * np.ones(system.getControlDimension())
xu_reg.setWeights(wx, wu)

# Adding the cost functions to the cost manager
cost_manager = cddp.CostManager()
cost_manager.addRunning(xu_reg)
cost_manager.addRunning(se3_cost)

# Setting up the DDP problem
timeline = np.arange(0.0, 0.25, 1e-3)  # np.linspace(0., 0.5, 51)
ddp = cddp.DDP(system, cost_manager, timeline)

# Solving the problem
ddp.compute(x0)


# Printing the final goal
frame_idx = model.getFrameId(frame_name)
xf = ddp.intervals[-1].x
qf = xf[:7]
print robot.framePosition(qf, frame_idx)


if plot:
  X = ddp.getStateTrajectory()
  U = ddp.getControlSequence()
  V = ddp.getTotalCostSequence()
  gradU = ddp.getConvergenceSequence()
  cddp.plotDDPSolution(model, X, U, V, gradU)

if display:
  T = timeline
  X = ddp.getStateTrajectory()
  cddp.visualizePlan(robot, x0, T, X)#, frame_idx)