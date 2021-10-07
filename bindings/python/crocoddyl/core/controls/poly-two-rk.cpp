///////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
//
// Copyright (C) 2021, University of Edinburgh, University of Trento
// Copyright note valid unless otherwise stated in individual files.
// All rights reserved.
///////////////////////////////////////////////////////////////////////////////

#include "python/crocoddyl/core/core.hpp"
#include "python/crocoddyl/core/control-base.hpp"
#include "crocoddyl/core/controls/poly-two-rk.hpp"

namespace crocoddyl {
namespace python {

void exposeControlParametrizationPolyTwoRK() {
  bp::register_ptr_to_python<boost::shared_ptr<ControlParametrizationModelPolyTwoRK> >();

  bp::class_<ControlParametrizationModelPolyTwoRK, bp::bases<ControlParametrizationModelAbstract> >(
      "ControlParametrizationModelPolyTwoRK",
      "Second-order polynomial control for RK integrators.\n\n"
      "This control is a quadratic function of time (normalized in [0,1]).\n"
      "It comes in two versions, one specialized for RK3 integration, another for RK4 integration."
      "The first third of the parameter vector contains the initial value of the differential control w,\n"
      "the second third contains the value of w at t=0.5 (for RK4) or 1/3 (for RK3), and the last third is the final\n"
      "value\n of w at time t=1 (for RK4) or 2/3 (for RK3).",
      bp::init<std::size_t, RKType>(bp::args("self", "nw", "rktype"),
                                    "Initialize the control dimensions.\n\n"
                                    ":param nw: dimension of differential control space\n"
                                    ":param rktype: type of RK parametrization"))
      .def<void (ControlParametrizationModelPolyTwoRK::*)(const boost::shared_ptr<ControlParametrizationDataAbstract>&,
                                                          double, const Eigen::Ref<const Eigen::VectorXd>&) const>(
          "calc", &ControlParametrizationModelPolyTwoRK::calc, bp::args("self", "data", "t", "u"),
          "Compute the control value.\n\n"
          ":param data: poly-two-rk data\n"
          ":param t: normalized time in [0, 1]\n"
          ":param u: control parameters (dim control.nu)")
      .def<void (ControlParametrizationModelPolyTwoRK::*)(const boost::shared_ptr<ControlParametrizationDataAbstract>&,
                                                          double, const Eigen::Ref<const Eigen::VectorXd>&) const>(
          "calcDiff", &ControlParametrizationModelPolyTwoRK::calcDiff, bp::args("self", "data", "t", "u"),
          "Compute the Jacobian of the control value with respect to the control parameters.\n"
          "It assumes that calc has been run first.\n\n"
          ":param data: poly-two-rk data\n"
          ":param t: normalized time in [0, 1]\n"
          ":param u: control parameters (dim control.nu)")
      .def<void (ControlParametrizationModelPolyTwoRK::*)(const boost::shared_ptr<ControlParametrizationDataAbstract>&,
                                                          double, const Eigen::Ref<const Eigen::VectorXd>&) const>(
          "params", &ControlParametrizationModelPolyTwoRK::params, bp::args("self", "data", "t", "w"),
          "Compute the control parameters.\n\n"
          ":param data: poly-two-rk data\n"
          ":param t: normalized time in [0, 1]\n"
          ":param w: control value (dim control.nw)")
      .def("convertBounds", &ControlParametrizationModelPolyTwoRK::convertBounds, bp::args("self", "w_lb", "w_ub"),
           "Convert the bounds on the control to bounds on the control parameters.\n\n"
           ":param w_lb: lower bounds on w (dim control.nw)\n"
           ":param w_ub: upper bounds on w (dim control.nw)\n"
           ":return u_lb, u_ub: lower and upper bounds on the control parameters (dim control.nu)")
      .def("multiplyByJacobian", &ControlParametrizationModelPolyTwoRK::multiplyByJacobian_J,
           bp::args("self", "data", "A"),
           "Compute the product between the given matrix A and the derivative of the control with respect to the "
           "parameters.\n\n"
           "It assumes that calc has been run first.\n"
           ":param data: poly-two-rk data\n"
           ":param A: matrix to multiply (dim na x control.nw)\n"
           ":return Product between A and the partial derivative of the control (dim na x control.nu)")
      .def(
          "multiplyJacobianTransposeBy", &ControlParametrizationModelPolyTwoRK::multiplyJacobianTransposeBy_J,
          bp::args("self", "data", "A"),
          "Compute the product between the transpose of the derivative of the control with respect to the parameters\n"
          "and a given matrix A.\n\n"
          "It assumes that calc has been run first.\n"
          ":param data: poly-two-rk data\n"
          ":param A: matrix to multiply (dim control.nw x na)\n"
          ":return Product between the partial derivative of the control (transposed) and A (dim control.nu x "
          "na)");

  boost::python::register_ptr_to_python<boost::shared_ptr<ControlParametrizationDataPolyTwoRK> >();

  bp::class_<ControlParametrizationDataPolyTwoRK, bp::bases<ControlParametrizationDataAbstract> >(
      "ControlParametrizationDataPolyTwoRK", "Control-parametrization data for the second-order polynomial control.",
      bp::init<ControlParametrizationModelPolyTwoRK*>(bp::args("self", "model"),
                                                      "Create control-parametrization data.\n\n"
                                                      ":param model: second-order polynomial control model"))
      .add_property("c", bp::make_getter(&ControlParametrizationDataPolyTwoRK::c, bp::return_internal_reference<>()),
                    "polynomial coefficients of the second-order control model");
}

}  // namespace python
}  // namespace crocoddyl
