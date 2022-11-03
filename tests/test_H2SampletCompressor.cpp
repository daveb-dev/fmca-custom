// This file is part of FMCA, the Fast Multiresolution Covariance Analysis
// package.
//
// Copyright (c) 2022, Michael Multerer
//
// All rights reserved.
//
// This source code is subject to the GNU Affero General Public License v3.0
// license and without any warranty, see <https://github.com/muchip/FMCA>
// for further information.
//
//#define EIGEN_DONT_PARALLELIZE
#include <Eigen/Dense>
#include <iostream>

#include "../FMCA/Samplets"
#include "../FMCA/src/Samplets/samplet_matrix_compressor.h"
#include "../FMCA/src/util/Tictoc.h"

#define NPTS 10000
#define DIM 3
#define MPOLE_DEG 6

struct exponentialKernel {
  double operator()(const FMCA::Matrix &x, const FMCA::Matrix &y) const {
    return exp(-(x - y).norm());
  }
};

namespace FMCA {
template <typename Functor>
Vector matrixColumnGetter(const Matrix &P, const std::vector<Index> &idcs,
                          const Functor &fun, Index colID) {
  Vector retval(P.cols());
  retval.setZero();
  for (auto i = 0; i < retval.size(); ++i)
    retval(i) = fun(P.col(idcs[i]), P.col(idcs[colID]));
  return retval;
}
}  // namespace FMCA

using Interpolator = FMCA::TotalDegreeInterpolator;
using SampletInterpolator = FMCA::MonomialInterpolator;
using Moments = FMCA::NystromMoments<Interpolator>;
using SampletMoments = FMCA::NystromSampletMoments<SampletInterpolator>;
using MatrixEvaluator = FMCA::NystromEvaluator<Moments, exponentialKernel>;
using H2SampletTree = FMCA::H2SampletTree<FMCA::ClusterTree>;

int main() {
  FMCA::Tictoc T;
  const auto function = exponentialKernel();
  const FMCA::Matrix P = 0.5 * (FMCA::Matrix::Random(DIM, NPTS).array() + 1);
  const FMCA::Scalar threshold = 1e-4;
  const Moments mom(P, MPOLE_DEG);
  const MatrixEvaluator mat_eval(mom, function);

  for (int dtilde = 4; dtilde <= 4; ++dtilde) {
    for (double eta = 1.2; eta >= 0.4; eta -= 0.2) {
      std::cout << "dtilde= " << dtilde << " eta= " << eta << std::endl;
      const SampletMoments samp_mom(P, dtilde - 1);
      H2SampletTree hst(mom, samp_mom, 0, P);
      T.tic();
      FMCA::internal::SampletMatrixCompressor<H2SampletTree> Scomp;
      Scomp.init(hst, eta, threshold);
      T.toc("planner:");
      T.tic();
      Scomp.compress(mat_eval);
      T.toc("compressor:");
      T.tic();
      const auto &trips = Scomp.triplets();
      T.toc("triplets:");
      std::cout << "anz: " << std::round(trips.size() / FMCA::Scalar(NPTS))
                << std::endl;
      FMCA::Vector x(NPTS), y1(NPTS), y2(NPTS);
      FMCA::Scalar err = 0;
      FMCA::Scalar nrm = 0;
      for (auto i = 0; i < 10; ++i) {
        FMCA::Index index = rand() % P.cols();
        x.setZero();
        x(index) = 1;
        y1 = FMCA::matrixColumnGetter(P, hst.indices(), function, index);
        x = hst.sampletTransform(x);
        y2.setZero();
        for (const auto &i : trips) {
          y2(i.row()) += i.value() * x(i.col());
          if (i.row() != i.col()) y2(i.col()) += i.value() * x(i.row());
        }
        y2 = hst.inverseSampletTransform(y2);
        err += (y1 - y2).squaredNorm();
        nrm += y1.squaredNorm();
      }
      err = sqrt(err / nrm);
      std::cout << "compression error:        " << err << std::endl
                << std::flush;
      std::cout << std::string(60, '-') << std::endl;
    }
  }
  return 0;
}
