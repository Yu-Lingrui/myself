# ***************************************************************
# Copyright (c) 2023 Jittor. All Rights Reserved. 
# Maintainers: 
#     Wenyang Zhou <576825820@qq.com>
#     Dun Liang <randonlang@gmail.com>. 
# 
# This file is subject to the terms and conditions defined in
# file 'LICENSE.txt', which is part of this source code package.
# ***************************************************************
import unittest
import os, sys
import jittor as jt
import jittor_utils as jit_utils

class TestLock(unittest.TestCase):
    def test(self):
        if os.environ.get('lock_full_test', '0') == '1':
            cache_path = os.path.join(jit_utils.home(), ".cache", "jittor", "lock")
            assert os.system(f"rm -rf {cache_path}") == 0
            cmd = f"cache_name=lock {sys.executable} -m jittor.test.test_example"
        else:
            cmd = f"{sys.executable} -m jittor.test.test_example"
        print("run cmd twice", cmd)
        assert os.system(f"{cmd} & {cmd} & wait %1 && wait %2") == 0


if __name__ == "__main__":
    unittest.main()