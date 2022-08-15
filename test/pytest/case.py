from subprocess import Popen
from typing import List

def cmd_to_inst(cmd:List[str], handle_last:bool = True) -> str:
    if handle_last == True:
        inst = '\n'.join(cmd) + '\n'
    else:
        inst = '\n'.join(cmd)
    return inst

def inst_to_cmd(inst:str) -> List[str]:
    cmd = inst.split("\n")
    return cmd

def run_cmd(cmd_runner:Popen, cmd:List[str]) -> List[str]:
    out, err = cmd_runner.communicate(cmd_to_inst(cmd))
    return inst_to_cmd(out)

def test_insert(cmd_runner):
    cmd = [
        "insert 1 user1 person1@example.com", 
        "select", 
        ".exit"
    ]
    expect = [
        "db> Executed.",
        "db> (1, user1, person1@example.com)",
        "Executed.",
        "db> "
    ]

    output = run_cmd(cmd_runner, cmd)
    assert output == expect


def test_table_full(cmd_runner):
    s = "insert {n} user{n} person{n}@example.com"
    cmd = [s.format(n=i+1) for i in range (0, 1401)]
    cmd.append(".exit")
    output = run_cmd(cmd_runner, cmd)
    expect = "db> Error: Table full."
    assert output[-2] == expect


def test_insert_string_maximum_length(cmd_runner):
    long_username = 32 * "a"
    long_email = 255 * "a"
    
    cmd = [
        f"insert 1 {long_username} {long_email}",
        "select",
        ".exit"
    ]
    output = run_cmd(cmd_runner, cmd)

    expect = [
        "db> Executed.",
        f"db> (1, {long_username}, {long_email})",
        "Executed.",
        "db> "
    ]
    assert output == expect


def test_print_error_massage_when_string_too_long(cmd_runner):
    long_username = 33 * "a"
    long_email = 256 * "a"
    cmd = [
        f"insert 1 {long_username} {long_email}",
        "select",
        ".exit"
    ]
    expect = [
        "db> String is too long.",
        "db> Executed.",
        "db> "
    ]
    output = run_cmd(cmd_runner, cmd)
    assert output == expect


def test_insert_nagative_id(cmd_runner):
    cmd = [
        "insert -1 cstack foo@bar.com",
        "select",
        ".exit"
    ]
    expect = [
        "db> ID must be positive.",
        "db> Executed.",
        "db> "
    ]
    output = run_cmd(cmd_runner, cmd)
    assert output == expect






