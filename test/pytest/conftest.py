# pytest的配置文件名必须是conftest.py
import os
import pytest

def pytest_addoption(parser):
    parser.addoption("--exec", action="store", default="None", help="The path of test program")


@pytest.fixture
def program(pytestconfig):
    return pytestconfig.getoption("exec")

@pytest.fixture(autouse=True)
def clear_env():
     yield
     os.remove("test.db")
