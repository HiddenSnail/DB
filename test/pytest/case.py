import subprocess
from typing import List

def cmd_to_inst(cmd:List[str], handle_last:bool = True):
    if handle_last == True:
        inst = '\n'.join(cmd) + '\n'
    else:
        inst = '\n'.join(cmd)
    return inst

def inst_to_cmd(inst:str):
    cmd = inst.split("\n")
    return cmd


def test_insert(script_runner):
    cmd = [
        "insert 1 user1 person1@example.com", 
        "select", 
        ".exit"
    ]
    expect = [
        "db> Executed.",
        "db> (1, user1, person1@example.com)",
        "Executed.",
        "db> "]

    out, err = script_runner.communicate(cmd_to_inst(cmd))
    output = inst_to_cmd(out)
    assert output == expect


def test_table_full(script_runner):
    s = "insert {n} user{n} person{n}@example.com"
    # 左闭右开区间，需要插到最大值再多一个元素
    cmd = [s.format(n=i+1) for i in range (0, 1401)]
    cmd.append(".exit")
    out, err = script_runner.communicate(cmd_to_inst(cmd))
    
    output = inst_to_cmd(out)
    expect = "db> Error: Table full."
    assert output[-2] == expect


def test_insert_string_maximum_length(script_runner):
    long_username = 32 * "a"
    long_email = 255 * "a"
    
    cmd = [f"insert 1 {long_username} {long_email}",
           "select",
           ".exit"]
    out, err = script_runner.communicate(cmd_to_inst(cmd))
    output = inst_to_cmd(out)

    expect = ["db> Executed.",
        f"db> (1, {long_username}, {long_email})",
        "Executed.",
        "db> "]
    assert output == expect

