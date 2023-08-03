# ***************************************************************
# Copyright (c) Jittor 2020, Author:
# All Rights Reserved.
# This file is subject to the terms and conditions defined in
# file 'LICENSE.txt', which is part of this source code package.
# ***************************************************************
import jittor as jt
import unittest
import numpy as np
from jittor import models

pass_this_test = False
try:
    jt.dirty_fix_pytorch_runtime_error()
    import torch
    import torchvision
except Exception as e:
    pass_this_test = True

def get_error(a, b):
    return np.abs(a-b) / max(np.abs(a), np.abs(b), 1e-5) , np.abs(a-b)

def check(jt_mod, torch_mod, rtol=1e-2, atol=1e-5, mean_atol=1e-5):
    pa = [ p for p in jt_mod.parameters() if not p.is_stop_grad() ]
    pb = list(torch_mod.parameters())
    assert len(pa) == len(pb)
    error_count = 0
    for a,b in zip(pa, pb):
        assert a.shape == list(b.shape), (a.shape, b.shape, a.name())
        stda, meana = np.std(a.numpy()), np.mean(a.numpy())
        stdb, meanb = np.std(b.detach().numpy()), np.mean(b.detach().numpy())

        r_err, a_err = get_error(stda, stdb)
        if r_err > rtol and a_err > atol:
            error_count += 1
            print("compare std error", stda, stdb, r_err, a_err, a.name(), a.shape)

        r_err, a_err = get_error(meana, meanb)
        if r_err > rtol and a_err > mean_atol:
            error_count += 1
            print("compare mean error", meana, meanb, r_err, a_err, a.name(), a.shape)
    assert error_count == 0

@unittest.skipIf(pass_this_test, f"pass init check, no torch found")
class TestInit(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        jt.seed(0)
        np.random.seed(0)
        torch.manual_seed(0)

    def test_conv(self):
        check(jt.nn.Conv(64, 256, 3), torch.nn.Conv2d(64, 256, 3), rtol=1e-1, mean_atol=1e-2)

    def test_resnet(self):
        check(models.resnet152(), torchvision.models.resnet152(), rtol=5e-2, mean_atol=1e-2)

from jittor import init
from jittor import nn

class TestInitFunc(unittest.TestCase):
    def test_eye(self):
        a = init.eye(2, "float32")
        np.testing.assert_allclose(a.data, [[1,0],[0,1]])
        a = init.eye((2,3), "float32")
        np.testing.assert_allclose(a.data, [[1,0,0],[0,1,0]])

        linear = nn.Linear(2,2)
        init.eye_(linear.weight)
        np.testing.assert_allclose(linear.weight.data, [[1,0],[0,1]])

    def test_constant(self):
        a = init.constant(2, "float32")
        np.testing.assert_allclose(a.data, [0,0])
        a = init.constant((2,3), value=1.)
        np.testing.assert_allclose(a.data, [[1,1,1],[1,1,1]])

        linear = nn.Linear(2,2)
        init.constant_(linear.weight)
        np.testing.assert_allclose(linear.weight.data, [[0,0],[0,0]])

    def test_uniform(self):
        a = init.uniform(5, "float32")
        assert ((a>0) & (a<1)).all()
        a = init.uniform((2,3), low=-1, high=1)
        assert ((a>-1) & (a<1)).all()

        linear = nn.Linear(2,2)
        init.uniform_(linear.weight)
        assert (linear.weight > 0).all()
        linear.weight.uniform_()
        assert (linear.weight > 0).all()


if __name__ == "__main__":
    unittest.main()