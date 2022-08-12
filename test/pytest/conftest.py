# pytest�����ļ�,�ļ�������Ϊconftest.py

import pytest
import subprocess

def pytest_addoption(parser):
    parser.addoption("--exec", action="store", default="None", help="The path of test program")

@pytest.fixture()
def script_runner(pytestconfig):
	# �����ö����ȡcmdopt��ֵ
    cmdopt = pytestconfig.getoption("exec")
    p = subprocess.Popen(args=[cmdopt],  
    stdin=subprocess.PIPE, 
    stdout=subprocess.PIPE,
    stderr=subprocess.PIPE,
    text=True)
    return p
