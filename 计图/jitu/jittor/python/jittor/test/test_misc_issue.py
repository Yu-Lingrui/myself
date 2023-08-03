# ***************************************************************
# Copyright (c) 2023 Jittor. All Rights Reserved. 
# Maintainers: Dun Liang <randonlang@gmail.com>. 
# This file is subject to the terms and conditions defined in
# file 'LICENSE.txt', which is part of this source code package.
# ***************************************************************
import unittest
import jittor as jt
import os
import numpy as np
import sys

class TestMiscIssue(unittest.TestCase):
    def test_issue4(self):
        try:
            jt.dirty_fix_pytorch_runtime_error()
            import torch
        except:
            return
        # import with pytorch cause segfault
        src = """N = 100
import jittor as jt
a = jt.random([N, N])
b = a.broadcast([N,N,N], dims=[0]) * a.broadcast([N,N,N], dims=[2])
b = b.sum(1)
b.sync()

import torch
A = torch.rand(N, N)
torch.matmul(A, A)
"""
        assert os.system(f"{sys.executable} -c '{src}'")==0
        src = """N = 100
import torch
A = torch.rand(N, N)
torch.matmul(A, A)

import jittor as jt
a = jt.random([N, N])
b = a.broadcast([N,N,N], dims=[0]) * a.broadcast([N,N,N], dims=[2])
b = b.sum(1)
b.sync()
"""
        assert os.system(f"{sys.executable} -c '{src}'")==0

    def test_mkl_conflict1(self):
        try:
            jt.dirty_fix_pytorch_runtime_error()
            import torch
        except:
            return
        if jt.mkl_ops is None:
            return
        # import with pytorch cause segfault
        src = """
nchw = [2, 3, 100, 100]
oihw = [4, 3, 5, 5]
import jittor as jt
x = jt.random(nchw)
w = jt.random(oihw)
jt.mkl_ops.mkl_conv(x, w, 1, 1, 2, 2).sync()

jt.dirty_fix_pytorch_runtime_error()

import torch
m = torch.nn.Conv2d(3, 4, 5, 1, 2)
m(torch.rand(*nchw))

"""
        assert os.system(f"{sys.executable} -c '{src}'")==0

    def test_mkl_conflict2(self):
        try:
            jt.dirty_fix_pytorch_runtime_error()
            import torch
        except:
            return
        if jt.mkl_ops is None:
            return
        # import with pytorch cause segfault
        src = """
nchw = [2, 3, 100, 100]
oihw = [4, 3, 5, 5]

import torch
m = torch.nn.Conv2d(3, 4, 5, 1, 2)
m(torch.rand(*nchw))

import jittor as jt
x = jt.random(nchw)
w = jt.random(oihw)
jt.mkl_ops.mkl_conv(x, w, 1, 1, 2, 2).sync()


"""
        assert os.system(f"{sys.executable} -c '{src}'")==0

    def test_cuda_lowsm(self):
        if not jt.has_cuda: return
        src = """
import jittor
from jittor.nn import matmul_transpose

a = jittor.ones((3,4,2), dtype="float32")
b = jittor.ones((5, 2), dtype="float32")
print(matmul_transpose(a, b))

jittor.flags.use_cuda = 1
a = jittor.ones((3,4,2), dtype="float32")
b = jittor.ones((5, 2), dtype="float32")
print(matmul_transpose(a, b))
"""
        assert os.system(f"cuda_archs=52 {sys.executable} -c '{src}'")==0

    def test_parallel(self):
        a = jt.code([4], "int", cpu_src="""
            #pragma omp parallel num_threads(4)
            @out(omp_get_thread_num()) = 456;
        """, cpu_header='#include <omp.h>').data
        assert (a==[456]*4).all(), a

    @unittest.skipIf(not jt.compiler.has_cuda, "No CUDA found")
    @jt.flag_scope(use_cuda=1)
    def test_reduce_opt(self):
        a = jt.random((16,512,38,38))
        b = jt.random((16,512,38,38))
        jt.sync([a, b])
        with jt.profile_scope(rerun=10, warmup=10) as rep:
            norm = a.sqr().sum(1, keepdims=True).sqrt()
            c = a / norm
            da = jt.grad(c*b, a)
            jt.sync([c, da])
        gpu_c = c.numpy()
        gpu_da = da.numpy()
        with jt.flag_scope(use_cuda=0):
            norm = a.sqr().sum(1, keepdims=True).sqrt()
            c = a / norm
            da = jt.grad(c*b, a)
            assert np.allclose(gpu_c, c.data, 1e-3)
            assert (np.abs(gpu_da-da.data).max() < 1e-6)

        assert float(rep[1][3]) < 15e6, float(rep[1][3]) # 15ms(about 8ms)

    @unittest.skipIf(not jt.compiler.has_cuda, "No CUDA found")
    @jt.flag_scope(use_cuda=1)
    def test_cuda_min_max(self):
        a = jt.random((10,)) - 2
        assert a.min().data == a.data.min(), (a.min(), a.data.min())
        assert a.max().data == a.data.max(), (a.max(), a.data.max())
        a = jt.random((10,)) + 2
        assert a.min().data == a.data.min(), (a.min(), a.data.min())
        assert a.max().data == a.data.max(), (a.max(), a.data.max())

        a = jt.random((10,)).float64() - 2
        assert a.min().data == a.data.min(), (a.min(), a.data.min())
        assert a.max().data == a.data.max(), (a.max(), a.data.max())
        a = jt.random((10,)).float64() + 2
        assert a.min().data == a.data.min(), (a.min(), a.data.min())
        assert a.max().data == a.data.max(), (a.max(), a.data.max())

    @unittest.skipIf(not jt.compiler.has_cuda, "No CUDA found")
    @jt.flag_scope(use_cuda=1)
    def test_cuda_pow_grad_nan(self):
        a = jt.float32([1,-1, -1000.1])
        da = jt.grad(a**2, a)
        assert np.isnan(da.data).sum()==0, da.data

    def test_tanh_nan(self):
        m=jt.nn.Tanh()
        a = m(jt.array([1000]))
        assert np.isnan(a.data).sum()==0, a

    def test_sigmoid_nan(self):
        a = jt.float32([1,-1, -1000.1])
        da = jt.grad(a.sigmoid(), a)
        assert np.isnan(da.data).sum()==0, da.data

    def test_sequential(self):
        x = jt.nn.Sequential(lambda x:x, lambda x:x)
        n = 0
        for a in x:
            n += 1
        assert n == 2
        assert list(x.keys()) == [0,1]
        p = x.parameters()
        assert len(p)==0

    def test_self_update(self):
        from jittor.models import resnet18
        m = resnet18()
        x = m.state_dict()
        m.load_state_dict(x)

    def test_res2net(self):
        import jittor.models
        net = jittor.models.res2net50(True)
        img = jt.random((2,3,224,224))
        out = net(img)
        print(out.shape, out.sum())
        jt.display_memory_info()
        jt.display_memory_info()
        assert out.shape == [2,1000]

    def test_argmax_memleak(self):
        a = jt.random([10])
        _, m = jt.argmax(a, 0)
        del _
        m.sync()
        g = jt.grad(m*10, a)
        g.sync()
        del a, g, m
        jt.display_memory_info()
        assert jt.liveness_info()["lived_ops"] == 0


if __name__ == "__main__":
    unittest.main()