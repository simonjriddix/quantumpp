#include <cmath>
#include <vector>
#include "gtest/gtest.h"
#include "qpp.h"

using namespace qpp;

// Unit testing "operations.hpp"

/******************************************************************************/
/// BEGIN template <typename Derived1, typename Derived2>
///       dyn_mat<typename Derived1::Scalar> apply(
///       const Eigen::MatrixBase<Derived1>& state,
///       const Eigen::MatrixBase<Derived2>& A, const std::vector<idx>& target,
///       const std::vector<idx>& dims)
TEST(qpp_apply, Qudits) {
    // pure states
    // 1 qubit
    ket psi = 1_ket;
    // X, Y, Z and H
    ket resultX = apply(psi, gt.X, {0}, std::vector<idx>({2}));
    EXPECT_EQ(0_ket, resultX);
    ket resultY = apply(psi, gt.Y, {0}, std::vector<idx>({2}));
    EXPECT_EQ(-1_i * 0_ket, resultY);
    ket resultZ = apply(psi, gt.Z, {0}, std::vector<idx>({2}));
    EXPECT_EQ(-1_ket, resultZ);
    ket resultH = apply(psi, gt.H, {0}, std::vector<idx>({2}));
    EXPECT_NEAR(0, norm(resultH - (0_ket - 1_ket) / std::sqrt(2)), 1e-8);

    // 2 qubits
    psi = 0.8 * 00_ket + 0.6 * 11_ket;
    resultX = apply(psi, gt.X, {1}, {2, 2});
    EXPECT_EQ(0.8 * 01_ket + 0.6 * 10_ket, resultX);
    resultY = apply(psi, gt.Y, {1}, {2, 2});
    EXPECT_EQ(0.8_i * 01_ket - 0.6_i * 10_ket, resultY);
    resultZ = apply(psi, gt.Z, {1}, {2, 2});
    EXPECT_EQ(0.8 * 00_ket - 0.6 * 11_ket, resultZ);
    ket resultCNOT = apply(psi, gt.CNOT, {0, 1}, {2, 2});
    EXPECT_EQ(0.8 * 00_ket + 0.6 * 10_ket, resultCNOT);
    resultCNOT = apply(psi, gt.CNOT, {1, 0}, {2, 2});
    EXPECT_EQ(0.8 * 00_ket + 0.6 * 01_ket, resultCNOT);
    ket resultZZ = apply(psi, kron(gt.Z, gt.Z), {0, 1}, {2, 2});
    EXPECT_EQ(0.8 * 00_ket + 0.6 * 11_ket, resultZZ);

    // 4 qubits
    psi = 0.8 * 0000_ket + 0.6 * 1111_ket;
    ket resultXZ = apply(psi, kron(gt.X, gt.Z), {1, 2}, {2, 2, 2, 2});
    EXPECT_EQ(0.8 * 0100_ket - 0.6 * 1011_ket, resultXZ);
    resultXZ = apply(psi, kron(gt.X, gt.Z), {2, 1}, {2, 2, 2, 2});
    EXPECT_EQ(0.8 * 0010_ket - 0.6 * 1101_ket, resultXZ);
    ket resultTOF = apply(psi, gt.TOF, {1, 2, 0}, {2, 2, 2, 2});
    EXPECT_EQ(0.8 * 0000_ket + 0.6 * 0111_ket, resultTOF);

    // 1 qudit

    // 2 qudits

    // 4 qudits

    // mixed states
}
/******************************************************************************/
/// BEGIN template <typename Derived1, typename Derived2>
///       dyn_mat<typename Derived1::Scalar> apply(
///       const Eigen::MatrixBase<Derived1>& state,
///       const Eigen::MatrixBase<Derived2>& A, const std::vector<idx>& target,
///       idx d = 2)
TEST(qpp_apply, Qubits) {}
/******************************************************************************/
/// BEGIN template <typename Derived> cmat apply(
///       const Eigen::MatrixBase<Derived>& A, const std::vector<cmat>& Ks)
TEST(qpp_apply, Channel) {}
/******************************************************************************/
/// BEGIN template <typename Derived> cmat apply(
///       const Eigen::MatrixBase<Derived>& A, const std::vector<cmat>& Ks,
///       const std::vector<idx>& target, const std::vector<idx>& dims)
TEST(qpp_apply, SubsysChannel) {}
/******************************************************************************/
/// BEGIN template <typename Derived> cmat apply(
///       const Eigen::MatrixBase<Derived>& A, const std::vector<cmat>& Ks,
///       const std::vector<idx>& target, idx d = 2)
TEST(qpp_apply, SubsysChannelQubits) {}
/******************************************************************************/
/// BEGIN template <typename Derived1, typename Derived2>
///       dyn_mat<typename Derived1::Scalar> applyCTRL(
///       const Eigen::MatrixBase<Derived1>& state,
///       const Eigen::MatrixBase<Derived2>& A, const std::vector<idx>& ctrl,
///       const std::vector<idx>& target, const std::vector<idx>& dims,
///       std::vector<idx> shift = {})
TEST(qpp_applyCTRL, NonEmptyControl) {
    std::vector<idx> dims{2, 2, 2, 2}; // 4 qubits
    idx D = prod(dims);                // total dimension

    std::vector<idx> ctrl{2, 0};   // where we apply the control
    std::vector<idx> target{1, 3}; // target

    idx Dtarget = 1; // dimension of the target subsystems
    for (auto i : target)
        Dtarget *= dims[i]; // compute it here

    // some random n qudit pure state
    ket psi = randket(D);

    cmat rho = psi * adjoint(psi); // the corresponding density matrix
    cmat U = randU(Dtarget);       // some random unitary on the target

    // applyCTRL on pure state
    ket A = applyCTRL(psi, U, ctrl, target, dims);

    // applyCTRL on density matrix
    cmat B = applyCTRL(rho, U, ctrl, target, dims);

    // result when using CTRL-U|psi><psi|CTRL-U^\dagger
    cmat result_psi = A * adjoint(A);
    // result when using CTRL-U(rho)CTRL-U^\dagger
    const cmat& result_rho = B;

    double res = norm(result_psi - result_rho);
    EXPECT_NEAR(0, res, 1e-7);
}
/******************************************************************************/
TEST(qpp_applyCTRL, EmptyControl) {
    std::vector<idx> dims{2, 2, 2, 2}; // 4 qubits
    idx D = prod(dims);                // total dimension

    std::vector<idx> ctrl{};          // where we apply the control
    std::vector<idx> target{1, 0, 3}; // target

    idx Dtarget = 1; // dimension of the target subsystems
    for (auto i : target)
        Dtarget *= dims[i]; // compute it here

    // some random n qudit pure state
    ket psi = randket(D);

    cmat rho = psi * adjoint(psi); // the corresponding density matrix
    cmat U = randU(Dtarget);       // some random unitary on the target

    // applyCTRL on pure state
    ket A = applyCTRL(psi, U, ctrl, target, dims);

    // applyCTRL on density matrix
    cmat B = applyCTRL(rho, U, ctrl, target, dims);

    // result when using CTRL-U|psi><psi|CTRL-U^\dagger
    cmat result_psi = A * adjoint(A);
    // result when using CTRL-U(rho)CTRL-U^\dagger
    const cmat& result_rho = B;

    double res = norm(result_psi - result_rho);
    EXPECT_NEAR(0, res, 1e-7);
}
/******************************************************************************/
/// BEGIN template <typename Derived1, typename Derived2>
///       dyn_mat<typename Derived1::Scalar> applyCTRL(
///       const Eigen::MatrixBase<Derived1>& state,
///       const Eigen::MatrixBase<Derived2>& A, const std::vector<idx>& ctrl,
///       const std::vector<idx>& target, idx d = 2,
///       std::vector<idx> shift = {})
TEST(qpp_applyCTRL, Qubits) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_mat<typename Derived::Scalar>
///       applyQFT(const Eigen::MatrixBase<Derived>& A,
///       const std::vector<idx>& target, idx d = 2, bool swap = true)
TEST(qpp_applyQFT, AllTests) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_mat<typename Derived::Scalar>
///       applyTFQ(const Eigen::MatrixBase<Derived>& A,
///       const std::vector<idx>& target, idx d = 2, bool swap = true)
TEST(qpp_applyTQF, AllTests) {}
/******************************************************************************/
/// BEGIN inline std::vector<cmat> choi2kraus(const cmat& A, idx Din, idx Dout)
TEST(qpp_choi2kraus, AllTests) {}
/******************************************************************************/
/// BEGIN inline cmat choi2super(const cmat& A, idx Din, idx Dout)
TEST(qpp_choi2super, AllTests) {}
/******************************************************************************/
/// BEGIN inline cmat kraus2choi(const std::vector<cmat>& Ks)
TEST(qpp_kraus2choi, AllTests) {}
/******************************************************************************/
/// BEGIN inline cmat kraus2super(const std::vector<cmat>& Ks)
TEST(qpp_kraus2super, AllTests) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_mat<typename Derived::Scalar>
///       ptrace(const Eigen::MatrixBase<Derived>& A,
///       const std::vector<idx>& target, const std::vector<idx>& dims)
TEST(qpp_ptrace, Qudits) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_mat<typename Derived::Scalar>
///       ptrace(const Eigen::MatrixBase<Derived>& A,
///       const std::vector<idx>& target, idx d = 2)
TEST(qpp_ptrace, Qubits) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_mat<typename Derived::Scalar>
///       ptrace1(const Eigen::MatrixBase<Derived>& A,
///       const std::vector<idx>& dims)
TEST(qpp_ptrace1, Qudits) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_mat<typename Derived::Scalar>
///       ptrace1(const Eigen::MatrixBase<Derived>& A, idx d = 2)
TEST(qpp_ptrace1, Qubits) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_mat<typename Derived::Scalar>
///       ptrace2(const Eigen::MatrixBase<Derived>& A,
///       const std::vector<idx>& dims)
TEST(qpp_ptrace2, Qudits) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_mat<typename Derived::Scalar>
///       ptrace2(const Eigen::MatrixBase<Derived>& A, idx d = 2)
TEST(qpp_ptrace2, Qubits) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_mat<typename Derived::Scalar>
///       ptranspose(const Eigen::MatrixBase<Derived>& A,
///       const std::vector<idx>& target, const std::vector<idx>& dims)
TEST(qpp_ptranspose, Qudits) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_mat<typename Derived::Scalar>
///       ptranspose(const Eigen::MatrixBase<Derived>& A,
///       const std::vector<idx>& target, idx d = 2)
TEST(qpp_ptranspose, Qubits) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_col_vect<typename Derived::Scalar>
///       QFT(const Eigen::MatrixBase<Derived>& A, idx d = 2,
///       bool swap = true)
TEST(qpp_QFT, AllTests) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_col_vect<typename Derived::Scalar>
///       qRAM(const Eigen::MatrixBase<Derived>& psi, const qram& data,
///       idx DqRAM = -1)
TEST(qpp_qRAM, AllTests) {}
/******************************************************************************/
/// BEGIN inline cmat super2choi(const cmat& A)
TEST(qpp_super2choi, AllTests) {}
/******************************************************************************/
/// BEGIN inline cmat super2kraus(const cmat& A)
TEST(qpp_super2kraus, AllTests) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_mat<typename Derived::Scalar>
///       syspermute(const Eigen::MatrixBase<Derived>& A,
///       const std::vector<idx>& perm, const std::vector<idx>& dims)
TEST(qpp_syspermute, Qudits) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_mat<typename Derived::Scalar>
///       syspermute(const Eigen::MatrixBase<Derived>& A,
///       const std::vector<idx>& perm, idx d = 2)
TEST(qpp_syspermute, Qubits) {}
/******************************************************************************/
/// BEGIN template <typename Derived> dyn_col_vect<typename Derived::Scalar>
///       TFQ(const Eigen::MatrixBase<Derived>& A, idx d = 2, bool swap = true)
TEST(qpp_TFQ, AllTests) {}
/******************************************************************************/
