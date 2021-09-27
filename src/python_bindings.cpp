// 2020 John Lees and Gerry Tonkin-Hill

#include "wtsne.hpp"
#include "pairsnp.hpp"

PYBIND11_MODULE(SCE, m) {
  m.doc() = "Stochastic cluster embedding";
  m.attr("version") = VERSION_INFO;

  // Animation class
  py::class_<sce_results<double>, std::shared_ptr<sce_results<double>>>(m, "sce_result")
    .def(py::init<const bool, const size_t, const uint64_t>())
    .def("animated", &sce_results<double>::is_animated)
    .def("n_frames", &sce_results<double>::n_frames)
    .def("get_eq", &sce_results<double>::get_eq)
    .def("get_embedding", &sce_results<double>::get_embedding)
    .def("get_embedding_frame", &sce_results<double>::get_embedding_frame, py::arg("frame"));

  // Exported functions
  m.def("wtsne", &wtsne, py::return_value_policy::take_ownership,
        "Run stochastic cluster embedding", py::arg("I_vec"), py::arg("J_vec"),
        py::arg("dist_vec"), py::arg("weights"),
        py::arg("perplexity"), py::arg("maxIter"), py::arg("nRepuSamp") = 5,
        py::arg("eta0") = 1, py::arg("bInit") = 0, py::arg("animated"),
        py::arg("n_workers") = 128,
        py::arg("n_threads") = 1, py::arg("seed") = 1);

  m.def("pairsnp", &pairsnp, py::return_value_policy::take_ownership,
        "Run pairsnp", py::arg("fasta"), py::arg("n_threads"),
        py::arg("dist"), py::arg("knn"));

#ifdef GPU_AVAILABLE
  // NOTE: python always uses fp64 so cannot easily template these (which
  // would just give one function name exported but with different type
  // prototypes). To do this would require numpy (python)/Eigen (C++) which
  // support both precisions. But easier just to stick with List (python)/
  // std::vector (C++) and allow fp64->fp32 conversion when called

  // Use fp64 for double precision (slower, more accurate)
  m.def("wtsne_gpu_fp64", &wtsne_gpu<double>,
        py::return_value_policy::take_ownership,
        "Run stochastic cluster embedding with CUDA", py::arg("I_vec"),
        py::arg("J_vec"), py::arg("dist_vec"), py::arg("weights"),
        py::arg("perplexity"), py::arg("maxIter"), py::arg("blockSize") = 128,
        py::arg("n_workers") = 128, py::arg("nRepuSamp") = 5,
        py::arg("eta0") = 1, py::arg("bInit") = 0, py::arg("cpu_threads") = 1,
        py::arg("device_id") = 0, py::arg("seed") = 1);
  // Use fp32 for single precision (faster, less accurate)
  m.def("wtsne_gpu_fp32", &wtsne_gpu<float>,
        py::return_value_policy::take_ownership,
        "Run stochastic cluster embedding with CUDA", py::arg("I_vec"),
        py::arg("J_vec"), py::arg("dist_vec"), py::arg("weights"),
        py::arg("perplexity"), py::arg("maxIter"), py::arg("blockSize") = 128,
        py::arg("n_workers") = 128, py::arg("nRepuSamp") = 5,
        py::arg("eta0") = 1, py::arg("bInit") = 0, py::arg("cpu_threads") = 1,
        py::arg("device_id") = 0, py::arg("seed") = 1);
#endif
}
