# pytest的配置文件名必须是conftest.py

import pytest
import subprocess

def pytest_addoption(parser):
    parser.addoption("--exec", action="store", default="None", help="The path of test program")

@pytest.fixture()
def cmd_runner(pytestconfig):
    cmdopt = pytestconfig.getoption("exec")
    p = subprocess.Popen(args=[cmdopt],  
    stdin=subprocess.PIPE, 
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True)
    return p
